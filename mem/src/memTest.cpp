#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "log_util.h"
#define min(a,b) ((a)<=(b)?(a):(b))



void *memmove1(void *dest, const void *source, size_t count);
void *memcpy1(void *dest, const void *source, size_t count);
void outArray(const char *outArray, size_t count);
int main()
{
    // char *a = (char*)malloc(10);
    // memcpy(a,"abcdef",strlen("abcdef"));
    // char *b = a+2;
    // b = &(a[1]);
    // memmove1(b,a,5);
    // outArray(b,5);
    // int i = min(1,5);
    // LOGD("min = %d",i);
    long a[5]={1,2,3,4,5};
    long *ptr=(long *)(a+1);
    LOGD("ptr = 0X%X",ptr);
    LOGD("*ptr = 0X%X",*ptr);

    LOGD("a = 0X%X",a);
    LOGD("&a = 0X%X",&a);


    LOGD("%d,%d",*(a+1),*(ptr-1)); 
    LOGD("%d,%d",*(a),*(ptr-2)); 

}

void *memmove1(void *dest, const void *source, size_t count)
{
    assert((NULL != dest) && (NULL != source));
    char *tmp_source, *tmp_dest;
    tmp_source = (char *)source;
    tmp_dest = (char *)dest;
    if ((dest + count < source) || (source + count) < dest)
    { // 如果没有重叠区域
        while (count--)
            *tmp_dest++ = *tmp_source++;
    }
    else
    { //如果有重叠
        tmp_source += count - 1;
        tmp_dest += count - 1;
        while (count--)
            *--tmp_dest = *--tmp_source;
    }
    return dest;
}


void *memcpy1(void *dest, const void *source, size_t count)
{
    assert((NULL != dest) && (NULL != source));
    char *tmp_dest = (char *)dest;
    char *tmp_source = (char *)source;
    while (count--) //不对是否存在重叠区域进行判断
        *tmp_dest++ = *tmp_source++;
    return dest;
}

void outArray(const char *array,size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        LOGD("%c",array[i]);
    }
    
}
