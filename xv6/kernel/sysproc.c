#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

extern struct proc proc[NPROC];

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Get the priority of the current process
uint64
sys_getpriority(void)
{
  return myproc()->priority;
}

// Set the priority of a process
// Can only be set by the parent process
uint64
sys_setpriority(void)
{
  int pid, priority;
  struct proc *p;
  struct proc *current = myproc();

  argint(0, &pid);
  argint(1, &priority);

  // Validate priority range
  if(priority < 0 || priority > 20)
    return -1;

  // Find the process with the given pid
  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->pid == pid){
      // Check if the current process is the parent of the target process
      if(p->parent != current){
        release(&p->lock);
        return -1; // Not the parent, cannot set priority
      }

      // Set the priority
      p->priority = priority;

      // Adjust tickets based on priority (higher priority = more tickets)
      p->tickets = 1 + priority;

      release(&p->lock);
      return 0;
    }
    release(&p->lock);
  }

  return -1; // Process not found
}
