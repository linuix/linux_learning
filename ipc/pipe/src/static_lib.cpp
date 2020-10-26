#include"static_lib.h"

extern "C"
char* getCharFromStatiLib(int length){

    LOGD("getCharFromStatiLib");
    char *res = static_cast<char *>(malloc(1024));
    strcpy(res,"12345");
    printf("%s,%d,%s,%d\n",__func__,__LINE__,res, sizeof(res));
    return res;
}
