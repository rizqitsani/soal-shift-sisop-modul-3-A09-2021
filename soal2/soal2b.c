#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#define BARIS 4
#define KOLOM 6

typedef struct arg {
  int value;
  int limit;
  int colIndex;
  int rowIndex;
} arg_t;

int (*sharedValue)[KOLOM];
int resultMatrix[BARIS][KOLOM];
int inputMatrix[4][6];

int getDesiredValue(int value, int limit) {
  int result = 1, multiplier;

  if (limit >= value) {
    multiplier = 2;
  } else {
    multiplier = value - limit + 1;
  }

  for (int i = multiplier; i <= value; i++) {
    result *= i;
  }

  return result;
}

void *threadFunction(void* argument) {
  for(int i = 0; i < BARIS; i++) {
    for(int j = 0; j < KOLOM; j++) {
      if (sharedValue[i][j] == 0 || inputMatrix[i][j] == 0) {
        resultMatrix[i][j] = 0;
      } else {
        resultMatrix[i][j] = getDesiredValue(sharedValue[i][j], inputMatrix[i][j]);
      }
    }
  }
}

int main() {
  key_t key = 7890;

  int shmid = shmget(key, sizeof(int[BARIS][KOLOM]), IPC_CREAT | 0666);
  sharedValue = shmat(shmid, NULL, 0);

  pthread_t tid;
  arg_t args;

  printf("Input kedua:\n");
  for(int i = 0; i < BARIS; i++) {
    for(int j = 0; j < KOLOM; j++) {
      scanf("%d", &inputMatrix[i][j]);
    }
  }

  printf("Matriks yang diterima: \n");

  for (int i = 0; i < BARIS; i++) {
    for (int j = 0; j < KOLOM; j++) {
      printf("%d\t", sharedValue[i][j]);
    }

    printf("\n");
  }

  pthread_create(&tid, NULL, &threadFunction, NULL);

  pthread_join(tid, NULL);

  printf("\nMatriks yang dihasilkan: \n");

  for (int i = 0; i < BARIS; i++) {
    for (int j = 0; j < KOLOM; j++) {
      printf("%d\t", resultMatrix[i][j]);
    }

    printf("\n");
  }

  shmdt(sharedValue);
  shmctl(shmid, IPC_RMID, NULL);
}