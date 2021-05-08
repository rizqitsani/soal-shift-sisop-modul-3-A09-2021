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

int resultMatrix[BARIS][KOLOM];

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
  arg_t *args = argument;
  if (args->value == 0 || args->limit == 0) {
    resultMatrix[args->rowIndex][args->colIndex] = 0;
  } else {
    resultMatrix[args->rowIndex][args->colIndex] = getDesiredValue(args->value, args->limit);
  }
}

int main() {
  key_t key = 7890;

  int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
  int *sharedValue = shmat(shmid, NULL, 0);
  
  pthread_t tid[BARIS * KOLOM];
  int receivedMatrix[BARIS][KOLOM];
  int threadIndex;
  arg_t args;

  int inputMatrix[4][6]={
    {1, 2, 2, 4, 1, 3},
    {2, 1, 3, 3, 1, 0},
    {1, 2, 4, 4, 2, 1},
    {1, 3, 1, 1, 2, 2},
  };

  for(int i = 0; i < BARIS; i++) {
    for(int j = 0; j < KOLOM; j++) {
      printf("Terima: %d \n", *sharedValue);
      receivedMatrix[i][j] = *sharedValue;

      args.value = receivedMatrix[i][j];
      args.limit = inputMatrix[i][j];
      args.colIndex = j;
      args.rowIndex = i;

      pthread_create(&tid[threadIndex], NULL, &threadFunction, (void*) &args);

      threadIndex++;
      sleep(3);
    }
  }

  printf("Matriks yang diterima: \n");

  for (int i = 0; i < BARIS; i++) {
    for (int j = 0; j < KOLOM; j++) {
      printf("%d\t", receivedMatrix[i][j]);
    }

    printf("\n");
  }

  printf("\nMatriks yang dihasilkan: \n");

  for (int i = 0; i < BARIS; i++) {
    for (int j = 0; j < KOLOM; j++) {
      printf("%d\t", resultMatrix[i][j]);
    }

    printf("\n");
  }

  for (int i = 0; i < threadIndex; i++) {
    pthread_join(tid[i], NULL);
  }

  shmdt(sharedValue);
  shmctl(shmid, IPC_RMID, NULL);
}