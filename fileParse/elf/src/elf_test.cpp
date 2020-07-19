#include <stdio.h>
#include "log_util.h"
#include <string.h>

extern "C"
int invok_interface(int a, int b, char c){
    int res = a + b;//函数开始
    LOGD("res = %d",res);//输出结果
    LOGD("c = %c",c);
    return res;
}
