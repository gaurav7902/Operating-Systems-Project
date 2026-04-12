#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"

struct sem s; //global semaphore (ishika)

uint64 sys_sem_init(void){
    int n;
    argint(0, &n);
    initlock(&s.lock, "semaphore");
    s.value = n;
    return 0;
}

uint64 sys_sem_wait(void){
    acquire(&s.lock);
    while(s.value <= 0){
        sleep(&s, &s.lock); //block process
    }
    s.value--;
    release(&s.lock);
    return 0;
}

uint64 sys_sem_signal(void){
    acquire(&s.lock);
    s.value++;
    wakeup(&s); //wake up waiting processes
    release(&s.lock);
    return 0;
}
