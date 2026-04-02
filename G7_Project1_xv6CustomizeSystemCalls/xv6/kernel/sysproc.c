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

// ============================================================
// Alarm Signal System Calls
// ============================================================

// sys_alarm_signal(int ticks, void (*handler)())
//
// Register a periodic alarm. Every 'ticks' timer interrupts,
// the kernel will divert this process's execution to 'handler'.
// Pass ticks=0 to disable the alarm.
uint64
sys_alarm_signal(void)
{
  int interval;
  uint64 handler;

  argint(0, &interval);
  argaddr(1, &handler);

  struct proc *p = myproc();
  p->alarm_interval = interval;
  p->alarm_handler = handler;
  p->alarm_ticks_left = interval;  // start counting from now
  p->alarm_active = 0;
  return 0;
}

// sys_alarm_return()
//
// Called at the end of the user's alarm handler. Restores the
// trapframe that was saved before diverting to the handler, so
// the process resumes execution exactly where it was interrupted.
uint64
sys_alarm_return(void)
{
  struct proc *p = myproc();

  // Restore the trapframe that was saved when the alarm fired.
  memmove(p->trapframe, p->alarm_saved_tf, sizeof(struct trapframe));

  // Reset the countdown for the next alarm period.
  p->alarm_ticks_left = p->alarm_interval;

  // Allow future alarms to fire again.
  p->alarm_active = 0;

  return p->trapframe->a0;  // preserve the original return value
}
