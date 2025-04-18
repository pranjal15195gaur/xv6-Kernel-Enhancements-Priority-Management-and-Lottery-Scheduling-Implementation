#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// A CPU-bound task that just burns cycles
void
burn_cpu(int iterations)
{
  int i, j;
  int dummy = 0;
  
  for(i = 0; i < iterations; i++){
    for(j = 0; j < 1000000; j++){
      dummy += j;
    }
  }
}

int
main(int argc, char *argv[])
{
  int i;
  int num_children = 3;
  int pids[3];
  int priorities[3] = {1, 10, 20}; // Low, medium, high priority
  
  printf("Starting lottery scheduler test with %d processes\n", num_children);
  
  // Create child processes with different priorities
  for(i = 0; i < num_children; i++){
    pids[i] = fork();
    if(pids[i] < 0){
      printf("fork failed\n");
      exit(1);
    }
    
    if(pids[i] == 0){
      // Child process
      printf("Child %d (PID %d) starting with priority %d\n", i, getpid(), priorities[i]);
      
      // Burn some CPU cycles
      burn_cpu(10);
      
      printf("Child %d (PID %d) with priority %d finished\n", i, getpid(), priorities[i]);
      exit(0);
    } else {
      // Parent process - set the child's priority
      setpriority(pids[i], priorities[i]);
      printf("Set child %d (PID %d) priority to %d\n", i, pids[i], priorities[i]);
    }
  }
  
  // Wait for all children to complete
  for(i = 0; i < num_children; i++){
    wait(0);
  }
  
  printf("All child processes completed\n");
  exit(0);
}
