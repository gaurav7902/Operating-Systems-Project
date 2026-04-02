#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
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
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
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
  return kkill(pid);
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

uint64
sys_yield_cpu(void)
{
  // xv6 already has an internal kernel function called yield().
  // This function changes the process state from RUNNING to RUNNABLE
  // and hands control back to the scheduler.
  yield(); 
  
  return 0; // Return 0 on success
}

uint64
sys_sleep_for(void)
{
  int n;       // This will hold the number of ticks
  uint ticks0; // This holds the starting time

  // Grab the 0th argument (the integer) from the user
  argint(0, &n); 

  // Acquire the lock for the clock ticks so we can read it safely
  acquire(&tickslock);
  ticks0 = ticks; // Record the current time

  // Loop until the current time - start time is greater than 'n'
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    // Go to sleep, wake up when the global 'ticks' changes
    sleep(&ticks, &tickslock); 
  }
  release(&tickslock);
  
  return 0; // Success
}

uint64
sys_fork_with_limit(void)
{
  int limit;
  argint(0, &limit); // Get the limit argument from user
  
  if (limit < 0) return -1; // Protect against negative limits

  myproc()->fork_limit = limit; // Set the limit for the current process
  
  return 0; // Success
}

uint64
sys_set_priority(void)
{
  int pid, priority;
  extern struct proc proc[]; // Tell this file that the proc array exists in memory
  struct proc *p;
  
  // In xv6-riscv, argint returns void, so we just call it directly:
  argint(0, &pid);
  argint(1, &priority);

  // Loop through all processes to find the one with the matching PID
  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->pid == pid){
      p->priority = priority; // Update the priority!
      release(&p->lock);
      return 0; // Success
    }
    release(&p->lock);
  }
  
  return -1; // PID not found
}