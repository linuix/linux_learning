## ELF 文件解析

### elf 文件生成
  预编译 -> 编译 -> 汇编 -> 链接  
.c -> .i > .s -> .o -> .bin

- 预编译  
-> gcc -E hello.c -o hello.i  
　　1.删除注释  
　　2.展开宏定义  
　　3.处理条件预编译 #if  
　　4.处理“#include”预编译指令，将包含的“.h”文件插入对应位置  
　　5.添加行号和文件标识符  
　　6.保留#pragma编译器指令

- 编译  
-> gcc -s hello.i -o hello.s  
　　1.扫描（词法分析）:将源代码程序输入扫描器，将源代码的字符序列分割成一系列记号。例array[index] =  (index + 4) * (2 + 6);  
　　2.语法分析：基于词法分析得到的一系列记号，生成语法树。  
　　3.语义分析：由语义分析器完成，指示判断是否合法，并不判断对错。又分静态语义：隐含浮点型到整形的转换，会报warning，动态语义：在运行时才能确定：例1除以3  
　　4.源代码优化（中间语言生成）：中间代码（语言）使得编译器分为前端和后端，前端产生与机器（或环境）无关的中间代码，编译器的后端将中间代码转换为目标机器代码，目的：一个前端对多个后端，适应不同平台。  
　　5.代码生成器:依赖于目标机器，依赖目标机器的不同字长，寄存器，数据类型等；目标代码优化器:选择合适的寻址方式，左移右移代替乘除，删除多余指令。

- 汇编  
 -> gcc -c hello.s -o hello.o  
　　1.汇编器是将汇编代码转变成可以执行的指令，生成 目标文件。

- 链接  
-> gcc hello.o -o hello  
　　1.符号决议：也可以说地址绑定，分动态链接和静态链接  
　　2.链接过程主要包括：地址和空间的分配，符号决议和重定位。（通过编译器的5个步骤后，我们获得目标代码，但是里面的各个地址还没有确定，空间还没有分配。）  
　　３.重定位：假设此时又两个文件：A，B。A需要B中的某个函数mov的地址，未链接前将地址置为0，当A与B链接后修改目标地址，完成重定位。


### 内存分布
内存地址由低到高依次为：.text > .data > .bbs > 堆 > 内存映射 > 栈

[内存分布图](./内存分布图.jpg)


### elf head 

- elf head 32位占52个字节 64位占64个字节 可以使用readelf -h xx 来进行读取
- elf 头中会指明elf字符串表的索引(e_shstrndx)
- elf中的所有的section相关的值均可以为0

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

### 段加密
- elf hard中所有section字段均可以直接修改，且不影响运行
- __attribute__((constructor)) void init_invok_interface 函数可以保证在invok_interface函数之前运行
- __attribute__((section(".newsec"))) int invok_interface(int a, int b, char c)可以自定义节名，并生成一个名字为“.newsec”的节头表
- cat /proc/XXX/maps 可以查看内存地址
- mprotect(void *__addr, size_t __len, int __prot) 用修改内存的读写执行权限

### 疑问

mprotest() 为基址但是解密的时候地址却为section offset ？

