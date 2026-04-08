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


// message passing between processes --gaurav
uint64
sys_sendmsg(void)
{
  int pid, len;
  uint64 msg_ptr;

  argint(0, &pid);
  argaddr(1, &msg_ptr);
  argint(2, &len);

  if(len < 0 || len > MSGSIZE)
    return -1;

  struct proc *target = find_proc(pid);
  if(target == 0)
    return -1;

  struct proc *p = myproc();

  acquire(&target->msg_lock);

  while(target->mailbox_full){
    if(killed(p)){
      release(&target->msg_lock);
      return -1;
    }
    sleep(target, &target->msg_lock);
  }

  if(copyin(p->pagetable, target->mailbox_data, msg_ptr, len) < 0){
    release(&target->msg_lock);
    return -1;
  }

  target->mailbox_src_pid = p->pid;
  target->mailbox_len = len;
  target->mailbox_full = 1;

  wakeup(target);  // wake receiver waiting on empty mailbox --gaurav

  release(&target->msg_lock);

  return 0;
}

uint64
sys_recvmsg(void)
{
  uint64 src_pid_ptr, buf_ptr;
  int maxlen;

  argaddr(0, &src_pid_ptr);
  argaddr(1, &buf_ptr);
  argint(2, &maxlen);
  if(maxlen < 0)
    return -1;

  struct proc *p = myproc();

  acquire(&p->msg_lock);

  while(p->mailbox_full == 0){
    if(killed(p)){
      release(&p->msg_lock);
      return -1;
    }
    sleep(p, &p->msg_lock);  // block until a message arrives --gaurav
  }

  int len = p->mailbox_len;
  if(len > maxlen)
    len = maxlen;

  if(copyout(p->pagetable, buf_ptr, p->mailbox_data, len) < 0){
    release(&p->msg_lock);
    return -1;
  }

  if(copyout(p->pagetable, src_pid_ptr, (char*)&p->mailbox_src_pid, sizeof(int)) < 0){
    release(&p->msg_lock);
    return -1;
  }

  p->mailbox_full = 0;
  p->mailbox_len = 0;

  wakeup(p);  // wake blocked sender waiting for free slot --gaurav

  release(&p->msg_lock);

  return len;
}