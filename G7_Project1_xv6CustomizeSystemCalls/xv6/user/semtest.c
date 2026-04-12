#include "kernel/types.h"
#include "user/user.h"

int main(){
    sem_init(0);

    if(fork() == 0){
        sleep(2);
        printf("Child: Waiting for signal...\n");
        sem_wait();
        printf("Child: Received signal! Exiting. \n");
        exit(0);
    }else{
        printf("Parent: Sleeping for a bit...\n");
        sleep(10);
        printf("Parent: Sending signal.\n");
        sem_signal();
        wait(0);
    }
    exit(0);
}