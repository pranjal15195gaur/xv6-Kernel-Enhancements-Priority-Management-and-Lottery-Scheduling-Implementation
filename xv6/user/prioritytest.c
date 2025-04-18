#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;

  printf("Parent process priority: %d\n", getpriority());

  // Create a child process
  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // Child process
    printf("Child process priority before change: %d\n", getpriority());
    sleep(10); // Give parent time to change priority
    printf("Child process priority after change: %d\n", getpriority());
    exit(0);
  } else {
    // Parent process
    printf("Parent setting child priority to 10\n");
    if(setpriority(pid, 10) < 0){
      printf("setpriority failed\n");
    }

    // Try to set our own priority (should fail)
    printf("Parent trying to set its own priority to 15\n");
    if(setpriority(getpid(), 15) < 0){
      printf("setpriority on self failed as expected\n");
    } else {
      printf("setpriority on self succeeded unexpectedly\n");
    }

    // Try to set an invalid priority
    printf("Parent trying to set invalid priority 25\n");
    if(setpriority(pid, 25) < 0){
      printf("setpriority with invalid priority failed as expected\n");
    } else {
      printf("setpriority with invalid priority succeeded unexpectedly\n");
    }

    wait(0);
  }

  exit(0);
}
