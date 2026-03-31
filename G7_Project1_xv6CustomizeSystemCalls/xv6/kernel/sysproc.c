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


// message passing between processes --gaurav
uint64
sys_sendmsg(void)
{
  int pid, len;
  uint64 msg_ptr;

  argint(0, &pid);
  argaddr(1, &msg_ptr);
  argint(2, &len);

  if(len > MSGSIZE) return -1;

  struct proc *target = find_proc(pid);
  if(target == 0) return -1;

  struct proc *p = myproc();

  acquire(&target->msg_lock);

  if(target->msg_count == MAXMSG){
      release(&target->msg_lock);
      return -1; // queue full
  }

  struct message *m = &target->msg_queue[target->msg_tail];

  m->src_pid = p->pid;
  m->len = len;

  if(copyin(p->pagetable, m->data, msg_ptr, len) < 0){
      release(&target->msg_lock);
      return -1;
  }

  target->msg_tail = (target->msg_tail + 1) % MAXMSG;
  target->msg_count++;

  wakeup(target);  // wake receiver if sleeping

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

    struct proc *p = myproc();

    acquire(&p->msg_lock);

    while(p->msg_count == 0){
        sleep(p, &p->msg_lock);  // block until message arrives
    }

    struct message *m = &p->msg_queue[p->msg_head];

    int len = m->len;
    if(len > maxlen) len = maxlen;

    if(copyout(p->pagetable, buf_ptr, m->data, len) < 0){
        release(&p->msg_lock);
        return -1;
    }

    if(copyout(p->pagetable, src_pid_ptr, (char*)&m->src_pid, sizeof(int)) < 0){
        release(&p->msg_lock);
        return -1;
    }

    p->msg_head = (p->msg_head + 1) % MAXMSG;
    p->msg_count--;

    release(&p->msg_lock);

    return len;
}