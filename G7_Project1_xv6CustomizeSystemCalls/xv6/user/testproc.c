#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char* get_state_name(int state) {
    switch(state) {
        case 0: return "UNUSED";
        case 1: return "USED";
        case 2: return "SLEEPING";
        case 3: return "RUNNABLE";
        case 4: return "RUNNING";
        case 5: return "ZOMBIE";
        default: return "UNKNOWN";
    }
}


int main(void) {
    struct procinfo info;

    printf("PID\tSTATE\t\tSIZE\tPARENT\tPRIORITY\n");
    printf("------------------------------------------\n");

    for(int i = 1; i < 10; i++) {
        if(getprocinfo(i, &info) == 0) {
            printf("%d\t%s\t%d\t%d\t%d\n",info.pid,get_state_name(info.state),info.sz,
                   info.parent_pid,     
                   info.priority);
        }
    }

    exit(0);
}
