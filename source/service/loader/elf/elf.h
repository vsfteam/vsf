#ifndef __ELF_H__
#define __ELF_H__

#include <stdint.h>

// http://www.skyfree.org/linux/references/ELF_Format.pdf

typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Word;
typedef int32_t Elf32_Sword;
typedef uint16_t Elf32_Half;

typedef struct Elf32_Hdr {
#define EI_MAG0         0
#   define ELFMAG0      0x7F
#define EI_MAG1         1
#   define ELFMAG1      'E'
#define EI_MAG2         2
#   define ELFMAG2      'L'
#define EI_MAG3         3
#   define ELFMAG3      'F'
#define EI_CLASS        4
#   define ELFCLASSNONE 0
#   define ELFCLASS32   1
#   define ELFCLASS64   2
#define EI_DATA         5
#   define ELFDATANONE  0
#   define ELFDATA2LSB  1
#   define ELFDATA2MSB  2
#define EI_VERSION      6
#define EI_PAD          7
#define EI_NINDENT      16
    unsigned char       e_ident[EI_NIDENT];

#define ET_NONE         0
#define ET_REL          1
#define ET_EXEC         2
#define ET_DYN          3
#define ET_CORE         4
#define ET_LOPROC       0xFF00
#define ET_HIPROC       0xFFFF
    Elf32_Half          e_type;

#define EM_NONE         0
#define EM_M32          1
#define EM_SPARC        2
#define EM_386          3
#define EM_68K          4
#define EM_88K          5
#define EM_860          7
#define EM_MIPS         8
#define EM_ARM          0x28
    Elf32_Half          e_machine;

#define EV_NONE         0
#define EV_CURRENT      1
    Elf32_Word          e_version;

    Elf32_Addr          e_entry;
    Elf32_Off           e_phoff;
    Elf32_Off           e_shoff;
    Elf32_Word          e_flags;
    Elf32_Half          e_ehsize;
    Elf32_Half          e_phentsize;
    Elf32_Half          e_phnum;
    Elf32_Half          e_shentsize;
    Elf32_Half          e_shnum;
    Elf32_Half          e_shstrndx;
};

#define SHN_UNDEF       0
#define SHN_LORESERVE   0xFF00
#define SHN_LOPROC      0xFF00
#define SHN_HIPROC      0xFF1F
#define SHN_ABS         0xFFF1
#define SHN_COMMON      0xFFF2
#define SHN_HIRESERVE   0xFFFF

typedef struct Elf32_Shdr {
    Elf32_Word          sh_name;

#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB       10
#define SHT_DYNSYM      11
#define SHT_LOPROC      0x70000000
#define SHT_HIPROC      0x7FFFFFFF
#define SHT_LOUSER      0x80000000
#define SHT_HIUSER      0x8FFFFFFF
    Elf32_Word          sh_type;

#define SHF_WRITE       0x1
#define SHF_ALLOC       0x2
#define SHF_EXECINSTR   0x4
#define SHF_MASKPROC    0xF0000000
    Elf32_Word          sh_flags;

    Elf32_Addr          sh_addr;
    Elf32_Off           sh_offset;
    Elf32_Word          sh_size;
    Elf32_Word          sh_link;
    Elf32_Word          sh_info;
    Elf32_Word          sh_addralign;
    Elf32_Word          sh_entsize;
} Elf32_Shdr;

typedef struct Elf32_Sym {
    Elf32_Word          st_name;
    Elf32_Addr          st_value;
    Elf32_Word          st_size;

#define STB_LOCAL       0
#define STB_GLOBAL      1
#define STB_WEAK        2
#define STB_LOPROC      13
#define STB_HIPROC      15
#define STT_NOTYPE      0
#define STT_OBJECT      1
#define STT_FUNC        2
#define STT_SECTION     3
#define STT_FILE        4
#define STT_LOPROC      13
#define STT_HIPROC      15
#define ELF32_ST_BIND(__INFO)           ((__INFO) >> 4)
#define ELF32_ST_TYPE(__INFO)           ((__UNFO) & 0xF)
#define ELF32_ST_INFO(__BIND, __TYPE)   (((__BIND) << 4) + ((__TYPE) & 0xF))
    unsigned char       st_info;

    unsigned char       st_other;

    Elf32_Half          st_shndx;
} Elf32_Sym;

typedef struct Elf32_Rel {
    Elf32_Addr          r_offset;
    Elf32_Word          r_info;
} Elf32_Rel;

typedef struct Elf32_Rela {
    Elf32_Addr          r_offset;

#define ELF32_R_SYM(__INFO)             ((__INFO) >> 8)
#define ELF32_R_TYPE(__INFO)            ((unsigned char)(__INFO))
#define ELF32_R_INFO(__SYM, __TYPE)     (((__SYM) << 8) + (unsigned char)(__TYPE))
    Elf32_Word          r_info;
    Elf32_Sword         r_addend;
} Elf32_Rela;

typedef struct Elf32_Phdr {
#define PT_NULL         0
#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4
#define PT_SHLIB        5
#define PT_PHDR         6
#define PT_LOPROC       0x70000000
#define PT_HIPROC       0x7FFFFFFF
    Elf32_Word          p_type;

    Elf32_Off           p_offset;
    Elf32_Addr          p_vaddr;
    Elf32_Addr          p_paddr;
    Elf32_Word          p_filesz;
    Elf32_Word          p_memsz;
    Elf32_Word          p_flags;
    Elf32_Word          p_align;
} Elf32_Phdr;

typedef struct Elf32_Dyn {
#define DT_NULL         0
#define DT_NEEDED       1
#define DT_PLTRELSZ     2
#define DT_PLTGOT       3
#define DT_HASH         4
#define DT_STRTAB       5
#define DT_SYMTAB       6
#define DT_RELA         7
#define DT_RELASZ       8
#define DT_RELAENT      9
#define DT_STRSZ        10
#define DT_SYMENT       11
#define DT_INIT         12
#define DT_FINI         13
#define DT_SONAME       14
#define DT_RPATH        15
#define DT_SYMBOLIC     16
#define DT_REL          17
#define DT_RELSZ        18
#define DT_RELENT       19
#define DT_PLTREL       20
#define DT_DEBUG        21
#define DT_TEXTREL      22
#define DT_JMPREL       23
#define DT_LOPROC       0x70000000
#define DT_HIPROC       0x7FFFFFFF
    Elf32_Sword         d_tag;

    union {
        Elf32_Word      d_val;
        Elf32_Addr      d_ptr;
    } d_un;
} Elf32_Dyn;

#endif      // __ELF_H__
