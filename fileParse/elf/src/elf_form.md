## ELF 文件解析

### elf head 

- elf head 32位占52个字节 64位占64个字节 可以使用readelf -l xx 来进行读取
- elf 头中会指明elf字符串表的索引(e_shstrndx)

```
#define EI_NIDENT 16
typedef struct{
    unsigned char e_ident[EI_NIDENT]; //16个字节的数组，定义了一些标识信息。最开头4B称为魔数，标识目标文件的类型或者格式。加载或读取文件时，可用魔数确认文件类型是否正确。再后面12字节，标识32位还是64位、大端还是小端、ELF头的版本号。
    Elf64_Half e_type;     //目标文件类型，是可重定位文件、可执行文件、共享库文件还是其它
    Elf64_Half e_machine;  //机器结构类型，是IA32、AMD64、SPARC V9还是其它
    Elf64_Word e_version; //目标文件版本
    Elf64_Addr e_entry;  //程序执行的入口地址，可重定位目标文件应该为0，且没有程序头表（段头表）等执行视图
    Elf64_Off  e_phoff; //程序头表（段头表）的起始位置
    Elf64_Off  e_shoff;   //节头表的起始位置
    Elf64_Word e_flags;   //处理器特定标志
    Elf64_Half e_ehsize;    //ELF头结构大小
    Elf64_Half e_phentsize;   //程序头表（段头表）的表项长度
    Elf64_Half e_phnum;       //程序头表（段头表）的表项数目
    Elf64_Half e_shentsize;    //节头表的表项长度
    Elf64_Half e_shnum;   //节头表的表项数目
    Elf64_Half e_shstrndx;  //字符串表在节头表中的索引，即节头表中第多少项是字符串表（String Table）。
}Elf64_Ehdr;
```

### section

```
typedef struct {
    Elf64_Word sh_name;   //节名字符串在.strtab节（字符串表）中的偏移
    Elf64_Word sh_type;   //节类型：无效/代码或数据/符号/字符串/...
    Elf64_Word sh_flags;  //节标志：该节在虚拟空间中的访问属性
    Elf64_Addr sh_addr;   //虚拟地址：若可被加载，则对应虚拟地址
    Elf64_Off  sh_offset; //在文件中的偏移地址，对.bss节而言则无意义
    Elf64_Word sh_size;   //节在文件中所占的长度
    Elf64_Word sh_link;   //sh_link和sh_info用于与链接相关的节（如 .rel.text节、.rel.data节、.symtab节等）
    Elf64_Word sh_info;
    Elf64_Word sh_addralign; //节的对齐要求
    Elf64_Word sh_entsize;   //节中每个表项的长度，0表示无固定长度表项
} Elf64_Shdr;
```

### segment
```
typedef struct {
    Elf64_Word p_type;   //此数组元素描述的段的类型，或者如何解释此数组元素的信息。
    Elf64_Off p_offset;  //此成员给出从文件头到该段第一个字节的偏移
    Elf64_Addr p_vaddr;  //此成员给出段的第一个字节将被放到内存中的虚拟地址
    Elf64_Addr p_paddr;  //此成员仅用于与物理地址相关的系统中。System V忽略所有应用程序的物理地址信息。
    Elf64_Word p_filesz; //此成员给出段在文件映像中所占的字节数。可以为0。
    Elf64_Word p_memsz;  //此成员给出段在内存映像中占用的字节数。可以为0。
    Elf64_Word p_flags;  //此成员给出与段相关的标志。
    Elf64_Word p_align;  //此成员给出段在文件中和内存中如何对齐。
} Elf64_phdr;
```