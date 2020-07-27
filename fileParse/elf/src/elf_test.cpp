#include <stdio.h>
#include <log_util.h>
#include <string.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <elf.h>
#include <sys/mman.h>
#include <errno.h>

unsigned long getLibAddr(const char *elfName);

extern "C"
    __attribute__((section(".newsec"))) int
    invok_interface(int a, int b, char c)
{
    int res = a + b;       //函数开始
    LOGD("res = %d", res); //输出结果
    LOGD("c = %c", c);
    return res;
}

__attribute__((constructor)) void init_invok_interface()
{

    if(1==1){
        return;
    }
    LOGD("init_invok_interface");

    unsigned long base_addr = getLibAddr("libelftest.so");
    LOGD("base_addr = %X", base_addr);
    Elf64_Ehdr *elf_ehdr;

    elf_ehdr = (Elf64_Ehdr *)base_addr;
    unsigned long newsec_addr = base_addr + elf_ehdr->e_shoff;
    LOGD("e_shoff = %12X e_shentsize = %12X newsec_addr = %12X", elf_ehdr->e_shoff, elf_ehdr->e_shentsize, newsec_addr);
    if (mprotect((void *)base_addr, elf_ehdr->e_shentsize, PROT_READ | PROT_EXEC | PROT_WRITE) == 0)
    {
        LOGD("newsec write seccess!");
        for (size_t i = 0; i < elf_ehdr->e_shentsize; i++)
        {
            unsigned char *data = (unsigned char *)(newsec_addr + i);
            *data = ~(*data);
        }
    }
    if (mprotect((void *)base_addr, elf_ehdr->e_shentsize, PROT_READ | PROT_EXEC) == 0)
    {
        LOGD("newsec exec seccess!");
    }

    LOGD("erron = %d", errno);
}

unsigned long getLibAddr(const char *elfName)
{
    unsigned long ret = 0;
    char buf[4096], *temp;
    int pid;
    FILE *fp;
    pid = getpid();
    sprintf(buf, "/proc/%d/maps", pid);
    fp = fopen(buf, "r");
    if (fp == NULL)
    {
        puts("open failed");
        goto _error;
    }
    while (fgets(buf, sizeof(buf), fp))
    {
        if (strstr(buf, elfName))
        {
            temp = strtok(buf, "-");
            ret = strtoul(temp, NULL, 16);
            break;
        }
    }
_error:
    fclose(fp);
    return ret;
}
