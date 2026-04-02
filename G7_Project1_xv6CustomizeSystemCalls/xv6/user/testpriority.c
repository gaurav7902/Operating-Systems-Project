// user/testpriority.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// A function to burn CPU time
void burn_cpu() {
    volatile int dummy = 0; // volatile stops compiler from optimizing this away
    for(int i = 0; i < 50000000; i++) {
        dummy = dummy + 1;
    }
}

int main(int argc, char *argv[]) {
    // 1. Check if the user provided exactly 3 arguments (plus the command name)
    if (argc != 4) {
        printf("Error: Please provide priorities for 3 children.\n");
        printf("Usage: testpriority <priority1> <priority2> <priority3>\n");
        exit(1);
    }

    // 2. Convert string arguments to integers
    int prio1 = atoi(argv[1]);
    int prio2 = atoi(argv[2]);
    int prio3 = atoi(argv[3]);

    int pid1, pid2, pid3;

    printf("Starting Priority Test with priorities: %d, %d, %d\n", prio1, prio2, prio3);

    // Child 1
    pid1 = fork();
    if (pid1 == 0) {
        set_priority(getpid(), prio1); // Set priority from argument 1
        burn_cpu();
        printf("Child 1 (Priority %d) finished!\n", prio1);
        exit(0);
    }

    // Child 2
    pid2 = fork();
    if (pid2 == 0) {
        set_priority(getpid(), prio2); // Set priority from argument 2
        burn_cpu();
        printf("Child 2 (Priority %d) finished!\n", prio2);
        exit(0);
    }

    // Child 3
    pid3 = fork();
    if (pid3 == 0) {
        set_priority(getpid(), prio3); // Set priority from argument 3
        burn_cpu();
        printf("Child 3 (Priority %d) finished!\n", prio3);
        exit(0);
    }

    // Parent waits for all 3 children to finish
    wait(0); wait(0); wait(0);
    
    printf("All children finished.\n");
    exit(0);
}