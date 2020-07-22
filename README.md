# linux_learning
-
## hook
-    记录Linux下hook的例子
## ipc
-    记录进程间通信代码
## soload
-    dlopen dlsys 动态加载so库
## signal
-    信号量例子
## mem
-    memcpy 和 memmove 例子
-    memmove在内存重叠的时候能避免出错，memcpy则不能。
## helloWorld
- linux 程序设计学习
    - src 中包括hello world 
    - shell 编程学习
## fileParse
- elf 文件解析
    - fseek() 成功返回0
    - 指针转基础类型
    ```
    elf64_shdr->sh_addr = *(Elf64_Addr *)section_table;       
    memcpy(&elf64_shdr->sh_offset, section_table, 8);
    ```
    - elf 64位 head 占64字节，32位占52字节 
    
    ### 系统预定义的section name  
    系统预定义了一些节名（以.开头），这些节有其特定的类型和含义。  
    .bss：包含程序运行时未初始化的数据（全局变量和静态变量）。当程序运行时，这些数据初始化为0。 其类型为SHT_NOBITS，表示不占文件空间。SHF_ALLOC + SHF_WRITE，运行时要占用内存的。
    .comment 包含版本控制信息（是否包含程序的注释信息？不包含，注释在预处理时已经被删除了）。类型为SHT_PROGBITS。  
    .data和.data1，包含初始化的全局变量和静态变量。 类型为SHT_PROGBITS，标志为SHF_ALLOC + SHF_WRITE（占用内存，可写）。  
    .debug，包含了符号调试用的信息，我们要想用gdb等工具调试程序，需要该类型信息，类型为SHT_PROGBITS。  
    .dynamic，类型SHT_DYNAMIC，包含了动态链接的信息。标志SHF_ALLOC，是否包含SHF_WRITE和处理器有关。  
    .dynstr，SHT_STRTAB，包含了动态链接用的字符串，通常是和符号表中的符号关联的字符串。标志 SHF_ALLOC  
    .dynsym，类型SHT_DYNSYM，包含动态链接符号表， 标志SHF_ALLOC。  
    .fini，类型SHT_PROGBITS，程序正常结束时，要执行该section中的指令。标志SHF_ALLOC + SHF_EXECINSTR（占用内存可执行）。现在ELF还包含.fini_array section。  
    .got，类型SHT_PROGBITS，全局偏移表(global offset table)，以后会重点讲。  
    .hash，类型SHT_HASH，包含符号hash表，以后细讲。标志SHF_ALLOC。  
    .init，SHT_PROGBITS，程序运行时，先执行该节中的代码。SHF_ALLOC + SHF_EXECINSTR，和.fini对应。现在ELF还包含.init_array section。  
    .interp，SHT_PROGBITS，该节内容是一个字符串，指定了程序解释器的路径名。如果文件中有一个可加载的segment包含该节，属性就包含SHF_ALLOC，否则不包含。  
    .line，SHT_PROGBITS，包含符号调试的行号信息，描述了源程序和机器代码的对应关系。gdb等调试器需要此信息。  
    .note Note Section, 类型SHT_NOTE，以后单独讲。  
    .plt 过程链接表（Procedure Linkage Table），类型SHT_PROGBITS,以后重点讲。  
    .relNAME，类型SHT_REL, 包含重定位信息。如果文件有一个可加载的segment包含该section，section属性将包含SHF_ALLOC，否则不包含。NAME，是应用重定位的节的名字，比如.text的重定位信息存储在.rel.text中。  
    .relaname 类型SHT_RELA，和.rel相同。SHT_RELA和SHT_REL的区别，会在讲重定位的时候说明。  
    .rodata和.rodata1。类型SHT_PROGBITS, 包含只读数据，组成不可写的段。标志SHF_ALLOC。  
    .shstrtab，类型SHT_STRTAB，包含section的名字。有读者可能会问：section header中不是已经包含名字了吗，为什么把名字集中存放在这里？ sh_name 包含的是.shstrtab 中的索引，真正的字符串存储在.shstrtab中。那么section names为什么要集中存储？我想是这样：如果有相同的字符串，就可以共用一块存储空间。如果字符串存在包含关系，也可以共用一块存储空间。  
    .strtab SHT_STRTAB，包含字符串，通常是符号表中符号对应的变量名字。如果文件有一个可加载的segment包含该section，属性将包含SHF_ALLOC。字符串以\0结束， section以\0开始，也以\0结束。一个.strtab可以是空的，它的sh_size将是0。针对空字符串表的非0索引是允许的。  
    symtab，类型SHT_SYMTAB，Symbol Table，符号表。包含了定位、重定位符号定义和引用时需要的信息。符号表是一个数组，Index 0 第一个入口，它的含义是undefined symbol index， STN_UNDEF。如果文件有一个可加载的segment包含该section，属性将包含SHF_ALLOC。
    

-