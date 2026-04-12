#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char *get_state_name(int state)
{
    switch (state) {
    case 0:
        return "UNUSED";
    case 1:
        return "USED";
    case 2:
        return "SLEEPING";
    case 3:
        return "RUNNABLE";
    case 4:
        return "RUNNING";
    case 5:
        return "ZOMBIE";
    default:
        return "UNKNOWN";
    }
}

// Helper function to convert flag to state code.
int get_state_from_flag(char *flag)
{
    if (strcmp(flag, "-r") == 0)
        return 4; // RUNNING
    if (strcmp(flag, "-s") == 0)
        return 2; // SLEEPING
    if (strcmp(flag, "-u") == 0)
        return 0; // UNUSED
    if (strcmp(flag, "-z") == 0)
        return 5; // ZOMBIE
    if (strcmp(flag, "-n") == 0)
        return 3; // RUNNABLE
    return -1;
}

int main(int argc, char *argv[])
{
    struct procinfo info;

    // Case 1: No argument, show all processes.
    if (argc == 1) {
        printf("PID\tSTATE\tSIZE\tPARENT\tPRIORITY\n");
        printf("----------------------------------------\n");

        for (int i = 1; i < 10; i++) {
            if (getprocinfo(i, &info) == 0) {
                printf("%d\t%s\t%d\t%d\t%d\n", info.pid, get_state_name(info.state),
                             info.sz, info.parent_pid, info.priority);
            }
        }
    }
    // Case 2: One argument, either a flag or a PID.
    else if (argc == 2) {
    int state_filter = get_state_from_flag(argv[1]);

        // Case: flag (like -r, -s, etc.)
        if (state_filter != -1) {
            printf("PID\tSTATE\tSIZE\tPARENT\tPRIORITY\n");
            printf("----------------------------------------\n");

            for (int i = 1; i < 10; i++) {
                if (getprocinfo(i, &info) == 0 && info.state == state_filter) {
                    printf("%d\t%s\t%d\t%d\t%d\n", info.pid,
                                 get_state_name(info.state), info.sz, info.parent_pid,
                                 info.priority);
        }
            }
    }

    // Case: PID
    else {
            int pid = atoi(argv[1]);

            if (getprocinfo(pid, &info) == 0) {
                printf("PID\tSTATE\tSIZE\tPARENT\tPRIORITY\n");
                printf("----------------------------------------\n");

                printf("%d\t%s\t%d\t%d\t%d\n", info.pid, get_state_name(info.state),
                             info.sz, info.parent_pid, info.priority);
            } else {
                printf("Process not found\n");
            }
    }
}

    // Case 3: Invalid usage.
    else {
        printf("Usage:\n");
        printf("  getprocinfo        (show all)\n");
        printf("  getprocinfo -r     (show only RUNNING processes)\n");
        printf("  getprocinfo <pid>  (specific process)\n");
    }

    exit(0);
}
