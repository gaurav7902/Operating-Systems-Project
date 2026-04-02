// user/testyield.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int pid = fork();

    if(pid < 0){
        printf("Fork failed!\n");
        exit(1);
    }

    for(int i = 0; i < 5; i++) {
        if(pid == 0) {
            printf("Child  (PID %d): iteration %d\n", getpid(), i);
            yield_cpu(); // Immediately give up the CPU
        } else {
            printf("Parent (PID %d): iteration %d\n", getpid(), i);
            yield_cpu(); // Immediately give up the CPU
        }
    }

    if(pid > 0) {
        wait(0); // Parent waits for child to finish before exiting
    }
    
    exit(0);
}