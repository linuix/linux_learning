#include <stdio.h>
#include <stdlib.h>
#include <cstring>

char* getCharFromStatiLib(int length){
    printf("%s length = %d \n",__func__,length);

    char *res = static_cast<char *>(malloc(1024));
//    res = "hello";
//    sprintf(res,"hello");
    strcpy(res,"12345");
    printf("%s,%d,%s,%d\n",__func__,__LINE__,res, sizeof(res));

    return res;
}
