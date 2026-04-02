// user/testsleep.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // 1. Check if the user actually passed an argument
    if(argc <= 1){
        printf("Error: Please provide the number of ticks to sleep.\n");
        printf("Usage: testsleep <ticks>\n");
        exit(1);
    }

    // 2. Convert the string argument (argv[1]) into an integer
    int ticks = atoi(argv[1]);

    // 3. Print the message and call our custom system call
    printf("Going to sleep for %d ticks...\n", ticks);
    
    sleep_for(ticks); 
    
    printf("Woke up!\n");
    
    exit(0);
}