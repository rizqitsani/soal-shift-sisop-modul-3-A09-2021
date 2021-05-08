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
  int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
  int *sharedValue = shmat(shmid, NULL, 0);

  int result[BARIS_KIRI][KOLOM_KANAN];

  int sum = 0;
  int first[BARIS_KIRI][KOLOM_KIRI] = {
    {2, 3, 1},
    {1, 2, 3},
    {1, 3, 3},
    {2, 1, 2}
  };

  int second[BARIS_KANAN][KOLOM_KANAN] = {
    {1, 2, 5, 4, 1, 1},
    {1, 5, 1, 3, 2, 2},
    {1, 4, 3, 3, 2, 4}
  };

  for (int i = 0; i < BARIS_KIRI; i++) {
    for (int j = 0; j < KOLOM_KANAN; j++) {
      for (int k = 0; k < KOLOM_KIRI; k++) {
        sum += first[i][k] * second[k][j];
      }

      result[i][j] = sum;
      sum = 0;
    }
  }

  printf("Hasil Matriks:\n");
  for (int i = 0; i < BARIS_KIRI; i++) {
    for (int j = 0; j < KOLOM_KANAN; j++) {
      printf("%d\t", result[i][j]);
    }

    printf("\n");
  }

  for (int i = 0; i < BARIS_KIRI; i++) {
    for (int j = 0; j < KOLOM_KANAN; j++) {
      *sharedValue = result[i][j];
      sleep(3);
      printf("Kirim: %d \n", *sharedValue);
    }
  }

  shmdt(sharedValue);
  shmctl(shmid, IPC_RMID, NULL);

	return 0;
}