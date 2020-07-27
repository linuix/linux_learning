#include <stdio.h>
#include <log_util.h>
#include <string.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <elf.h>
#include <vector>
#include <sys/mman.h>

using namespace std;

// extern "C" int invok_interface(int a, int b, char c);

typedef int (*InvokInterface)(int, int, char);

unsigned long getLibAddr(const char *elfName);

int main()
{
    LOGD("start decription");
    void *elf_hold = dlopen("/mnt/d/code/linux_learning/build/out/elfparse/lib/libdecript_elf.so", RTLD_LAZY);
    if (elf_hold)
    {
        unsigned long base_addr = getLibAddr("libdecript_elf.so");
        LOGD("base_addr = %X", base_addr);
        if (base_addr)
        {
            Elf64_Ehdr *elf_ehdr;

            elf_ehdr = (Elf64_Ehdr *)base_addr;

            unsigned long section_table_addr = base_addr + elf_ehdr->e_shoff;

            LOGD("section addr = %X e_shentsize = %X e_shoff = %X", section_table_addr, elf_ehdr->e_shentsize, elf_ehdr->e_shoff);

            if (mprotect((void *)base_addr, elf_ehdr->e_shentsize, PROT_READ | PROT_EXEC | PROT_WRITE) != 0)
            {
                LOGD("mprotect read write exec failed!");
                return -1;
            }
            for (size_t i = 0; i < elf_ehdr->e_shentsize; i++)
            {
                char *data = (char *)(section_table_addr + i);
                *data = ~(*data);
            }

            if (mprotect((void *)base_addr, elf_ehdr->e_shentsize, PROT_READ | PROT_EXEC) == 0)
            {
                LOGD("mprotect read exec failed!");
                return -1;
            }

            InvokInterface invoke_interface = (InvokInterface)dlsym(elf_hold, "init_invok_interface");

            int res = invoke_interface(1, 2, 'e');
            LOGD("res = %d", res);
        }
    }

    return 0;
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
