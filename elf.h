//
// Created by leewy on 2016/10/27.
//

#ifndef RISCV_SIM_ELF_H
#define RISCV_SIM_ELF_H
using Elf_Char = unsigned char;
using Elf_Half = unsigned short;
using Elf_Word = unsigned int;
using Elf_Sword = unsigned long long;
struct elf_ident{
    Elf_Word    EI_MAG;
    Elf_Char    EI_CLASS;
    Elf_Char    EI_DATA;
    Elf_Char    EI_VERSION;
    Elf_Char    EI_OSABI;
    Elf_Char    EI_ABIVERSION;
    Elf_Char    EI_PAD[7];              //unused
    elf_ident() = default;
    elf_ident(const char * elf_buffer);
};
//EI_MAG
const Elf_Word EIMAG      = 0x464c457f;//0x7F followed by ELF(45 4c 46)
//EI_CLASS
const Elf_Char EICLASS_32   = 1;        //32bit format
const Elf_Char EICLASS_64   = 2;        //64bit format
//EI_DATA
const Elf_Char EIDATA_LITTLE= 1;        //little endianness
const Elf_Char EIDATA_BIG   = 2;        //big endianness
//EI_VERSION
const Elf_Char EIVERSION    = 1;        //Set to 1 for the original version of ELF.
// EI_OSABI
const Elf_Char EIABI_SYSV   = 0;        //System V ABI
const Elf_Char EIABI_LINUX  = 3;        //Linux ABI

struct elf32_header{
    elf_ident   e_ident;                //Magic
    Elf_Half    e_type;                 //Type
    Elf_Half    e_machine;              //Machine
    Elf_Word    e_version;              //Version
    Elf_Word    e_entry;                //Entry point address
    Elf_Word    e_phoff;                //Start of program header
    Elf_Word    e_shoff;                //Start of section header
    Elf_Word    e_flag;                 //Flags
    Elf_Half    e_ehsize;               //Size of this header
    Elf_Half    e_phentsize;            //Size of program headers
    Elf_Half    e_phnum;                //Number of program headers
    Elf_Half    e_shentsize;            //Size of section headers
    Elf_Half    e_shunm;                //Number of section headers
    Elf_Half    e_shstrndx;             //Section header string table index
    elf32_header() = default;
    elf32_header(const char* elf_buffer);
};

struct elf64_header{
    elf_ident       e_ident;            //Magic
    Elf_Half        e_type;             //Type
    Elf_Half        e_machine;          //Machine
    Elf_Word        e_version;          //Version
    Elf_Sword       e_entry;            //Entry point address
    Elf_Sword       e_phoff;            //Start of program header
    Elf_Sword       e_shoff;            //Start of section header
    Elf_Word        e_flag;             //Flags
    Elf_Half        e_ehsize;           //Size of this header
    Elf_Half        e_phentsize;        //Size of program headers
    Elf_Half        e_phnum;            //Number of program headers
    Elf_Half        e_shentsize;        //Size of section headers
    Elf_Half        e_shnum;            //Number of section headers
    Elf_Half        e_shstrndx;         //Section header string table index
    elf64_header() = default;
    elf64_header(const char * elf_buffer);
};

// e_type
const Elf_Half ET_NONE      = 0;        //No file type
const Elf_Half ET_REL       = 1;        //Relocatable file
const Elf_Half ET_EXEC      = 2;        //Executable file
const Elf_Half ET_DYN       = 3;        //Shared object file
const Elf_Half ET_CORE      = 4;        //Core file

// e_machine
const Elf_Half EM_NONE      = 0;        //No machine type
const Elf_Half EM_386       = 3;        //x86
const Elf_Half EM_x86_64    = 0x3e;     //x86_64
const Elf_Half EM_riscv     = 0xf3;     //risc-v

// e_version
const Elf_Word EV_NODE      = 0;        //Invalid version
const Elf_Word EV_CURRENT   = 1;        //Current version

struct Elf32_Shdr{
    Elf_Word	sh_name;
    Elf_Word	sh_type;
    Elf_Word	sh_flags;
    Elf_Word	sh_addr;
    Elf_Word	sh_offset;
    Elf_Word	sh_size;
    Elf_Word	sh_link;
    Elf_Word	sh_info;
    Elf_Word	sh_addralign;
    Elf_Word	sh_entsize;
};
struct Elf64_Shdr{
    Elf_Word	sh_name;
    Elf_Word	sh_type;
    Elf_Sword	sh_flags;
    Elf_Sword	sh_addr;
    Elf_Sword	sh_offset;
    Elf_Sword	sh_size;
    Elf_Word	sh_link;
    Elf_Word	sh_info;
    Elf_Sword	sh_addralign;
    Elf_Sword	sh_entsize;
};
//sh_flags
const Elf_Word SHF_WRITE = 0x1;
const Elf_Word SHF_ALLOC = 0x2;
const Elf_Word SHF_EXECINSTR = 0x4;
const Elf_Word SHF_MERGE = 0x10;
const Elf_Word SHF_STRINGS = 0x20;
const Elf_Word SHF_INFO_LINK = 0x40;
const Elf_Word SHF_LINK_ORDER = 0x80;
const Elf_Word SHF_OS_NONCONFORMING = 0x100;
const Elf_Word SHF_GROUP = 0x200;
const Elf_Word SHF_MASKOS = 0x0ff00000;
const Elf_Word SHF_MASKPROC = 0xf0000000;
/*
 *  http://www.sco.com/developers/gabi/2000-07-17/ch4.sheader.html
Name	        Type	            Attributes
.bss	        SHT_NOBITS	        SHF_ALLOC+SHF_WRITE
.comment	    SHT_PROGBITS	    none
.data	        SHT_PROGBITS	    SHF_ALLOC+SHF_WRITE
.data1	        SHT_PROGBITS	    SHF_ALLOC+SHF_WRITE
.debug	        SHT_PROGBITS	    none
.dynamic	    SHT_DYNAMIC	        see below
.dynstr	        SHT_STRTAB	        SHF_ALLOC
.dynsym	        SHT_DYNSYM	        SHF_ALLOC
.fini	        SHT_PROGBITS	    SHF_ALLOC+SHF_EXECINSTR
.fini_array	    SHT_FINI_ARRAY	    SHF_ALLOC+SHF_WRITE
.got	        SHT_PROGBITS	    see below
.hash	        SHT_HASH	        SHF_ALLOC
.init	        SHT_PROGBITS	    SHF_ALLOC+SHF_EXECINSTR
.init_array	    SHT_INIT_ARRAY	    SHF_ALLOC+SHF_WRITE
.interp	        SHT_PROGBITS	    see below
.line	        SHT_PROGBITS	    none
.note	        SHT_NOTE	        none
.plt	        SHT_PROGBITS	    see below
.preinit_array	SHT_PREINIT_ARRAY	SHF_ALLOC+SHF_WRITE
.relname	    SHT_REL	            see below
.relaname	    SHT_RELA	        see below
.rodata	        SHT_PROGBITS	    SHF_ALLOC
.rodata1	    SHT_PROGBITS	    SHF_ALLOC
.shstrtab	    SHT_STRTAB	        none
.strtab	        SHT_STRTAB	        see below
.symtab	        SHT_SYMTAB	        see below
.symtab_shndx	SHT_SYMTAB_SHNDX    see below
.text	        SHT_PROGBITS	    SHF_ALLOC+SHF_EXECINSTR
*
 */

struct  Elf32_Phdr{
    Elf_Word      p_type;
    Elf_Word      p_offset;
    Elf_Word      p_vaddr;
    Elf_Word      p_paddr;
    Elf_Word      p_filesz;
    Elf_Word      p_memsz;
    Elf_Word      p_flags;
    Elf_Word      p_align;
};

struct  Elf64_Phdr{
    Elf_Word      p_type;
    Elf_Word      p_flags;
    Elf_Sword     p_offset;
    Elf_Sword     p_vaddr;
    Elf_Sword     p_paddr;
    Elf_Sword     p_filesz;
    Elf_Sword     p_memsz;
    Elf_Sword     p_align;
    Elf64_Phdr(const char* elf_buffer);
};
//p_type
const Elf_Word PT_LOAD = 1;

#endif //RISCV_SIM_ELF_H
