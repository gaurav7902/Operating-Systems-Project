#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "vm.h"
#include "proc.h"

extern struct proc proc[NPROC];

// System call handler for getprocinfo
// Extracts arguments from user space and calls kernel logic

uint64
sys_getprocinfo(void)
{
    int pid;
    uint64 addr;  // user-space address for struct

    // Get first argument (pid)
	argint(0, &pid);
    // Get second argument (pointer to struct in user space)
	argaddr(1, &addr);

    struct proc *p;
    struct procinfo info;

    // Traverse process table to find matching PID
    for(p = proc; p < &proc[NPROC]; p++){
        if(p->pid == pid){

            // Fill struct with process data
            info.pid = p->pid;
            info.state = p->state;
            info.sz = p->sz;
            info.parent_pid = p->parent ? p->parent->pid : -1;
    	    info.priority = p->priority;
            // Copy data from kernel → user space
            if(copyout(myproc()->pagetable, addr, (char*)&info, sizeof(info)) < 0)
                return -1;

            return 0;  // success
        }
    }

    return -1;  // PID not found
}

// Implementation of getppid system call
// Returns the parent process ID of the current process

uint64
sys_getppid(void)
{
    struct proc *p = myproc();   // Get pointer to current process

    // Safety check: if no parent exists (should not happen normally)
    if(p->parent == 0)
        return -1;

    // Return parent process ID
    return p->parent->pid;
}

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
