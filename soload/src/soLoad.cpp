#include <dlfcn.h>
#include "log_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   


typedef char *(*Method)(int);

int main()
{
    char buf[80];
    getcwd(buf, sizeof(buf));
    LOGD("current working directory: %s", buf);
    void *handle = dlopen("./libs/libtestlib.so", RTLD_LAZY);
    if (handle)
    {
        LOGD("dlopen success");
        Method method = (char *(*)(int))dlsym(handle, "getCharFromStatiLib");
        if (!method)
        {
            LOGE("dlsym error");
            return 0;
        }
        method(123);
    }
    else
    {
        LOGE("ldopen error");
    }

    return 1;
}