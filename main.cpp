#pragma GCC diagnostic error "-std=c++11"
#include <iostream>
#include<stdio.h>
#include<unistd.h>
#include <term_entry.h>
#include "log_util.h"


char* getCharFromStatiLib(int length);


int main() {



    int n;
    int fd[2];
    pid_t pid;
    char line[MAX_LINE];

    LOGD("hello");
    printf("%s\n",__func__);

    if (pipe(fd) != 0){
        printf("create pipe faile%d\n",errno);
        exit(0);
    }



    int res = fork();
    if (res>0)
    {//主进程
        printf("process id = %d \n",res);
        char *res = getCharFromStatiLib(5);
        printf("rest = %s\n",res);
        close(fd[0]);
        int size = write(fd[1],res,5);
        printf("write size = %d\n",size);
    }else
    {
        printf("main process id = %d \n",res);
        close(fd[1]);
        n = read(fd[0],line,MAX_LINE);
        write(STDOUT_FILENO,line,n);
        printf("main process pid = %d read = %s\n",res,line);
    }
    
    return 0;
}



