#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int pid;
int fd1[2];
int fd2[2];

int main() {
	pid_t child_id;
  int status;

  if (pipe(fd1) == -1) {
    fprintf(stderr, "Pipe Failed");
    return 1;
  }

  child_id = fork();
  if (child_id < 0) {
    exit(EXIT_FAILURE);
  }

  if (child_id == 0) {
    close(fd1[0]);
    dup2(fd1[1], STDOUT_FILENO);

    execlp("ps", "ps", "aux", NULL);
  }

  if (pipe(fd2) == -1) {
    fprintf(stderr, "Pipe Failed");
    return 1;
  }

  child_id = fork();
  if (child_id < 0)
  {
    exit(EXIT_FAILURE);
  }

  if (child_id == 0) {
    close(fd1[1]);
    dup2(fd1[0], STDIN_FILENO);

    close(fd2[0]);
    dup2(fd2[1], STDOUT_FILENO);

    execlp("sort", "sort", "-nrk", "3.3", NULL);
  }

  close(fd1[0]);
  close(fd1[1]);

  close(fd2[1]);
  dup2(fd2[0], STDIN_FILENO);
  close(fd2[0]);

  execlp("head", "head", "-5", NULL);
}
