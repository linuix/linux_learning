#pragma GCC diagnostic error "-std=c++11"
#include <iostream>
#include<stdio.h>
#include<unistd.h>
#include"static_lib.h"
#include "log_util.h"

#define MAX_LINE 1024

int main() {
    int n;
    int fd[2];
    pid_t pid;
    char line[MAX_LINE];

    LOGD("hello");

    if (pipe(fd) != 0){
        LOGD("create pipe faile%d",errno);
        exit(0);
    }



    int res = fork();
    if (res>0)
    {//主进程
        LOGD("process id = %d ",res);
        char *res = getCharFromStatiLib(5);
        LOGD("rest = %s",res);
        close(fd[0]);
        int size = write(fd[1],res,5);
        LOGD("write size = %d",size);
    }else
    {
        LOGD("main process id = %d ",res);
        close(fd[1]);
        n = read(fd[0],line,MAX_LINE);
        write(STDOUT_FILENO,line,n);
        LOGD("main process pid = %d read = %s",res,line);
    }
    
    return 0;
}



