// user/testforklimit.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // 1. Check if the user passed an argument
    if(argc <= 1){
        printf("Error: Please provide the max number of children.\n");
        printf("Usage: testforklimit <limit>\n");
        exit(1);
    }

    // 2. Convert the string argument into an integer
    int limit = atoi(argv[1]);

    printf("Setting fork limit to %d...\n", limit);
    fork_with_limit(limit);

    // 3. We will try to fork 'limit + 2' times to prove the block works
    int attempts = limit + 2;

    for(int i = 1; i <= attempts; i++) {
        int pid = fork();
        
        if (pid < 0) {
            printf("Fork %d failed! Limit of %d reached.\n", i, limit);
        } else if (pid == 0) {
            // Child process exits immediately
            exit(0); 
        } else {
            // Parent process waits for the child to finish
            wait(0);
            printf("Fork %d successful! Child PID: %d\n", i, pid);
        }
    }
    
    exit(0);
}