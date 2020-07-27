#include <stdio.h>
#include <elf.h>
#include "log_util.h"
#include <string.h>
#include <vector>
#include <stdlib.h>

using namespace std;

int main()
{
    Elf64_Ehdr ehdr;

    FILE *fd = fopen("/mnt/d/code/linux_learning/build/out/elfparse/lib/libelftest.so", "rb");

    fread(&ehdr, sizeof(ehdr), 1, fd);

    LOGD(" Start of section headers = %d", ehdr.e_shoff);
    fseek(fd, ehdr.e_shoff, SEEK_SET);

    vector<Elf64_Shdr> shdr_vector;
    for (size_t i = 0; i < ehdr.e_shnum; i++)
    {
        Elf64_Shdr elf_shdr;
        fread(&elf_shdr, sizeof(elf_shdr), 1, fd);
        shdr_vector.push_back(elf_shdr);
    }
    unsigned char str_tab_text[shdr_vector.at(ehdr.e_shstrndx).sh_size];
    fseek(fd, shdr_vector.at(ehdr.e_shstrndx).sh_offset, SEEK_SET);
    fread(str_tab_text, sizeof(str_tab_text), 1, fd);
    Elf64_Shdr elf_shdr_text;
    for (size_t j = 0; j < shdr_vector.size(); j++)
    {
        const char *str1 = (char *)&str_tab_text[shdr_vector.at(j).sh_name];
        LOGD("index = %d name index = %4d\tname str = %s", j, shdr_vector.at(j).sh_name, &str_tab_text[shdr_vector.at(j).sh_name]);

        int res = strcmp(".newsec", str1);
        if (res == 0)
        {
            memcpy(&elf_shdr_text, &shdr_vector.at(j), sizeof(elf_shdr_text));
        }
    }

    LOGD("name index = %4d\tname str = %s", elf_shdr_text.sh_name, &str_tab_text[elf_shdr_text.sh_name]);
    fseek(fd, elf_shdr_text.sh_offset, SEEK_SET);
    unsigned char section_text_data[elf_shdr_text.sh_size];
    fread(section_text_data, sizeof(section_text_data), 1, fd);
    for (size_t k = 0; k < elf_shdr_text.sh_size; k++)
    {
        printf("%X ", (int)section_text_data[k]);
        section_text_data[k] = ~section_text_data[k];
    }
    printf("\r\n");

    for (size_t l = 0; l < elf_shdr_text.sh_size; l++)
    {
        printf("%X ", (int)section_text_data[l]);
    }
    printf("\r\n");
    FILE *decript_elf_file = fopen("/mnt/d/code/linux_learning/build/out/elfparse/lib/libdecript_elf.so", "wb+");

    fseek(fd, 0, SEEK_SET);
    unsigned char temp_data[4096];
    bzero(temp_data, 4096);
    size_t length = 0;
    while (!feof(fd))
    {
        length = fread(temp_data, sizeof(unsigned char), 4096, fd);
        int n = feof(fd);
        LOGD("length = %d n = %d", length, n);
        fwrite(temp_data, sizeof(unsigned char), length, decript_elf_file);
    }
    LOGD("length = %d", length);
    fseek(decript_elf_file, elf_shdr_text.sh_offset, SEEK_SET);
    fwrite(section_text_data, elf_shdr_text.sh_size, 1, decript_elf_file);

    //修改elf head
    ehdr.e_shoff = elf_shdr_text.sh_offset;
    ehdr.e_shentsize = elf_shdr_text.sh_size;
    //写入修改后的头

    fseek(decript_elf_file, 0, SEEK_SET);
    fwrite((const void*)(&ehdr),sizeof(ehdr),1,decript_elf_file);

    fflush(decript_elf_file);
    fclose(fd);
    fclose(decript_elf_file);
    LOGD("end");
    return 1;
}