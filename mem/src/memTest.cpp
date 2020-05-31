#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "log_util.h"

void *memmove1(void *dest, const void *source, size_t count);
void *memcpy1(void *dest, const void *source, size_t count);
void outArray(const char *outArray, size_t count);
int main()
{
    char *a = (char*)malloc(10);
    memcpy(a,"abcdef",strlen("abcdef"));
    char *b = a+2;
    b = &(a[1]);
    memmove1(b,a,5);
    outArray(b,5);
    // LOGD("%d",1);
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
