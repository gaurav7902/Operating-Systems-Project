#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int pid = fork();

    if (pid == 0) {
        char buf[100];
        int src;

        recvmsg(&src, buf, sizeof(buf));
        printf("Received from %d: %s\n", src, buf);
    } else {
        char *msg = "Hello from parent";
        sendmsg(pid, msg, strlen(msg) + 1);
    }

    exit(0);
}