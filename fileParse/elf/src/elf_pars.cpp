#include <stdio.h>
#include <elf.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <log_util.h>

using namespace std;

extern "C" int invok_interface(int a, int b, char c);

void parse_ELF(const char *path);
Elf64_Ehdr *parse_header(unsigned char *header);
vector<Elf64_Shdr *> *parse_section_table(unsigned char *section_table, Elf64_Ehdr *elf_ehdr);

int main()
{
    // int res = invok_interface(1,2,'a');

    parse_ELF("/mnt/d/code/linux_learning/fileParse/elf/file/libelftest.so");
    return 1;
}

void parse_ELF(const char *path)
{
    FILE *elfFile = NULL;
    elfFile = fopen(path, "rb");

    unsigned char header[64];
    size_t elf_head_size = fread(header, 1, 64, elfFile);
    printf("header size = %ld\r\n", elf_head_size);
    Elf64_Ehdr *elf_ehdr = parse_header(header);
    printf("section_table_offer = %ld\r\n", elf_ehdr->e_shoff);

    int res = fseek(elfFile, elf_ehdr->e_shoff, SEEK_SET);
    printf("res = %d\r\n", res);
    int section_table_size, section_table_count;
    section_table_count = elf_ehdr->e_shnum;
    section_table_size = elf_ehdr->e_shentsize;
    printf("section_table_count = %d  section_table_size = %d\r\n", section_table_count, section_table_size);
    unsigned char section_table[section_table_count * section_table_size];
    size_t section_table_length = fread(section_table, section_table_size, section_table_count, elfFile);
    printf("section_table_length = %ld\r\n", section_table_length);

    vector<Elf64_Shdr *> *elf_shdr_vector = NULL;

    elf_shdr_vector = parse_section_table(section_table, elf_ehdr);

    unsigned char shstrtab_str[elf_shdr_vector->at(elf_ehdr->e_shstrndx)->sh_size];
    fseek(elfFile, elf_shdr_vector->at(elf_ehdr->e_shstrndx)->sh_offset, SEEK_SET);
    fread(shstrtab_str, 1, elf_shdr_vector->at(elf_ehdr->e_shstrndx)->sh_size, elfFile);
    for (int i = 0; i < elf_shdr_vector->size(); i++)
    {
        LOGD("strtab = %s", &shstrtab_str[elf_shdr_vector->at(i)->sh_name]);
    }
}

Elf64_Ehdr *parse_header(unsigned char *header)
{
    Elf64_Ehdr *elf_ehdr = NULL;
    elf_ehdr = (Elf64_Ehdr *)malloc(sizeof(Elf64_Ehdr));

    unsigned char magic[16];
    memcpy(magic, header, 16);
    memccpy(elf_ehdr->e_ident, magic, 0, 16);

    printf("elf_ehdr addr %x\r\n", elf_ehdr->e_ident);
    printf("%-40s", "Magic:");
    for (int i = 0; i < sizeof(magic); i++)
    {
        printf("%02X ", magic[i]);
    }
    printf("\n");
    printf("%-40s\t %2X %c %c %c\n", "elf文件标识:", magic[0], magic[1], magic[2], magic[3]);

    printf("%-40s", "CLASS: ");
    switch (magic[4])
    {
    case 0:
        printf("无效 class\n");
        break;
    case 1:
        printf("32 位\n");
        break;
    case 2:
        printf("64 位\n");
        break;
    default:
        printf("错误数据\n");
        break;
    }
    printf("%-40s", "Data : ");
    switch (magic[5])
    {
    case 0:
        printf("无效的数据编码\n");
        break;
    case 1:
        printf("小端 \r\n");
        break;
    case 2:
        printf("大端 \n");
        break;
    default:
        printf("错误数据\n");
        break;
    }
    printf("%-40s %d\n", "版本：", magic[6]);

    printf("%-40s", "OS / ABI标识 :");
    switch (magic[7])
    {
    case 0:
        printf("UNIX System V ABI Alias\n");
        break;
    case 1:
        printf("HP-UX\n");
        break;
    case 2:
        printf("NetBSD \n");
        break;
    case 7:
        printf("IBM AIX.\r\n");
        break;
    case 64:
        printf("ARM EABI \r\n");
        break;
    case 97:
        printf("ARM \r\n");
        break;
    default:
        printf("数据错误\n");
        break;
    }
    printf("%-40s %d\r\n", "ABI 版本：", magic[8]);

    header += 16; //magic 已解析完，跳过magic，7btye被跳过
    printf("%-40s", "ELF 文件类型");
    elf_ehdr->e_type = *(uint16_t *)header;

    switch (*(uint16_t *)header)
    {
    case 0:
        printf("类型无效\r\n");
        break;
    case 1:
        printf("重定位文件类型\r\n");
        break;
    case 2:
        printf("可执行文件类型\r\n");
        break;
    case 3:
        printf("共享对象文件类型\r\n");
        break;
    case 4:
        printf("核心文件类型\r\n");
        break;
    case 5:
        printf("默认文件类型\r\n");
        break;
    default:
        printf("类型错误！\r\n");
        break;
    }

    printf("%-40s", "CPU 架构：");
    header += 2;
    elf_ehdr->e_machine = *(uint16_t *)header;

    switch (*(uint16_t *)header)
    {
    case EM_386:
        printf("Intel 80386 \r\n");
        break;
    case EM_ARM:
        printf("ARM \r\n");
        break;
    case EM_X86_64:
        printf("AMD x86-64 architecture\r\n");
        break;
    default:
        printf("暂不支持！\r\n");
    }
    header += 2;

    elf_ehdr->e_version = *(uint32_t *)header;
    printf("%-40s %d\r\n", "Version : ", elf_ehdr->e_version);
    header += 4;

    elf_ehdr->e_entry = *(uint64_t *)header;
    printf("%-40s %d\r\n", "入口地址：", elf_ehdr->e_entry);
    header += 8;

    elf_ehdr->e_phoff = *(uint64_t *)header;
    printf("%-40s %d\r\n", "端头表起始地址：", elf_ehdr->e_phoff);
    header += 8;

    elf_ehdr->e_shoff = *(uint64_t *)header;
    printf("%-40s %d\r\n", "节头表起始地址：", elf_ehdr->e_shoff);
    header += 8;

    elf_ehdr->e_flags = *(uint32_t *)header;
    printf("%-40s %d\r\n", "处理器特定的标志", elf_ehdr->e_flags);
    header += 4;

    elf_ehdr->e_ehsize = *(uint16_t *)header;
    printf("%-40s %d\r\n", "ELF 头的大小：", elf_ehdr->e_ehsize);
    header += 2;

    elf_ehdr->e_phentsize = *(uint16_t *)header;
    printf("%-40s %d\r\n", "ELF 段头表大小：", elf_ehdr->e_phentsize);
    header += 2;

    elf_ehdr->e_phnum = *(uint16_t *)header;
    printf("%-40s %d\r\n", "ELF 段头表的数量：", elf_ehdr->e_phnum);
    header += 2;

    elf_ehdr->e_shentsize = *(uint16_t *)header;
    printf("%-40s %d\r\n", "ELF 节头表大小：", elf_ehdr->e_shentsize);
    header += 2;

    elf_ehdr->e_shnum = *(uint16_t *)header;
    printf("%-40s %d\t\r\n", "ELF 节头表的数量：", elf_ehdr->e_shnum);
    header += 2;

    elf_ehdr->e_shstrndx = *(uint16_t *)header;
    printf("%-40s %d\t\r\n", "ELF 节区字符串表位置：", elf_ehdr->e_shstrndx);

    printf("------------------------------------------------------------------------\r\n");
    return elf_ehdr;
}

vector<Elf64_Shdr *> *parse_section_table(unsigned char *section_table, Elf64_Ehdr *elf_ehdr)
{
    printf("------------------------------节头表------------------------------\r\n");

    vector<Elf64_Shdr *> *elf_shdr_vector = NULL;

    elf_shdr_vector = (vector<Elf64_Shdr *> *)malloc(sizeof(vector<Elf64_Shdr *>));

    for (size_t i = 0; i < elf_ehdr->e_shnum; i++)
    {
        Elf64_Shdr *elf64_shdr = NULL;
        elf64_shdr = (Elf64_Shdr *)malloc(sizeof(Elf64_Shdr));
        elf64_shdr->sh_name = *(Elf64_Word *)section_table;
        section_table += 4;

        elf64_shdr->sh_type = *(Elf64_Word *)section_table;
        section_table += 4;

        elf64_shdr->sh_flags = *(Elf64_Xword *)section_table;
        section_table += 8;

        elf64_shdr->sh_addr = *(Elf64_Addr *)section_table;
        section_table += 8;

        memcpy(&elf64_shdr->sh_offset, section_table, 8);
        section_table += 8;

        elf64_shdr->sh_size = *(Elf64_Xword *)section_table;
        section_table += 8;

        elf64_shdr->sh_link = *(Elf64_Word *)section_table;
        section_table += 4;

        elf64_shdr->sh_info = *(Elf64_Word *)section_table;
        section_table += 4;

        elf64_shdr->sh_addralign = *(Elf64_Xword *)section_table;
        section_table += 8;

        elf64_shdr->sh_entsize = *(Elf64_Xword *)section_table;
        section_table += 8;

        // section_table += elf_ehdr->e_shentsize;
        elf_shdr_vector->push_back(elf64_shdr);

        printf("table = %d sh_name = %d  table off = %X, section size = %X\r\n", i, elf64_shdr->sh_name, elf64_shdr->sh_offset, elf64_shdr->sh_size);
    }

    return elf_shdr_vector;
}