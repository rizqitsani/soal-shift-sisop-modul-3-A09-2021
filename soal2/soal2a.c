#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#define BARIS_KIRI 4
#define KOLOM_KIRI 3
#define BARIS_KANAN 3
#define KOLOM_KANAN 6

int main()
{
  key_t key = 7890;
  int (*sharedValue)[KOLOM_KANAN];
  int shmid = shmget(key, sizeof(int[BARIS_KIRI][KOLOM_KANAN]), IPC_CREAT | 0666);
  sharedValue = shmat(shmid, NULL, 0);

  int result[BARIS_KIRI][KOLOM_KANAN];
  int first[BARIS_KIRI][KOLOM_KIRI];
  int second[BARIS_KANAN][KOLOM_KANAN];

  int sum = 0;

  printf("Input pertama:\n");
  for(int i = 0; i < BARIS_KIRI; i++) {
    for(int j =0; j < KOLOM_KIRI; j++) {
      scanf("%d", &first[i][j]);
    }
  }

  printf("Input kedua:\n");
  for(int i = 0; i < BARIS_KANAN; i++) {
    for(int j = 0; j < KOLOM_KANAN; j++) {
      scanf("%d", &second[i][j]);
    }
  }
  printf("\n");

  for (int i = 0; i < BARIS_KIRI; i++) {
    for (int j = 0; j < KOLOM_KANAN; j++) {
      for (int k = 0; k < KOLOM_KIRI; k++) {
        sum += first[i][k] * second[k][j];
      }

      sharedValue[i][j] = sum;
      result[i][j] = sum;
      sum = 0;
    }
  }

  printf("Hasil Matriks:\n");
  for (int i = 0; i < BARIS_KIRI; i++) {
    for (int j = 0; j < KOLOM_KANAN; j++) {
      sharedValue[i][j] = result[i][j];
      printf("%d\t", sharedValue[i][j]);
    }

    printf("\n");
  }

  sleep(10);
  shmdt(sharedValue);
  shmctl(shmid, IPC_RMID, NULL);

	return 0;
}