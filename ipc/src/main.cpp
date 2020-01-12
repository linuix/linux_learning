#pragma GCC diagnostic error "-std=c++11"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "static_lib.h"
#include "log_util.h"

#define MAX_LINE 1024

void handler(int s) //信号到来，则执行这个函数，输出超时
{
    LOGD("信号破裂 %d ", s);
    exit(1);
}

int main()
{
    int n;
    int fd[2];
    pid_t pid;
    char buffer[MAX_LINE];
    signal(SIGPIPE, handler);

    LOGD("hello");

    if (pipe(fd) != 0)
    {
        LOGD("create pipe faile%d", errno);
        exit(0);
    }

    pid = fork();
    if (pid > 0)
    { //主进程
        LOGD("main process id = %d child process %d", getpid(), pid);
        char *msg = "from main msg";
        // close(fd[0]);
        sleep(1);

        int size = write(fd[1], msg, strlen(msg));

        sleep(2);
        size = read(fd[0], buffer, MAX_LINE);
        LOGD("main proce read = %s", buffer);
    }
    else
    {
        pid = getpid();
        LOGD("child process id = %d ", pid);
        // close(fd[1]);
        n = read(fd[0], buffer, MAX_LINE);
        LOGD("child process read = %s length %d", buffer, n);
        sleep(3);

        write(fd[1], buffer, n);
        LOGD("child process end ");
    }

    return 0;
}
