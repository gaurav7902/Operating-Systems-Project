#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    int pid = fork();

    if (pid < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        char buf[100];
        int src;

        pause(20);
        recvmsg(&src, buf, sizeof(buf));
        printf("child: first message from %d: %s\n", src, buf);

        recvmsg(&src, buf, sizeof(buf));
        printf("child: second message from %d: %s\n", src, buf);
        exit(0);
    }

    printf("parent: sending first message\n");
    sendmsg(pid, "message-1", 10);

    // This blocks until child receives the first message. --gaurav
    printf("parent: sending second message (blocks if mailbox is full)\n");
    sendmsg(pid, "message-2", 10);

    wait(0);
    exit(0);
}