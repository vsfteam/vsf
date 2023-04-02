#ifndef __ELF_H__
#define __ELF_H__

#include <stdint.h>

// http://www.skyfree.org/linux/references/ELF_Format.pdf

/*----------------------------------------------------------------------------*
 * ELF32                                                                      *
 *----------------------------------------------------------------------------*/

typedef uint32_t        Elf32_Addr;
typedef uint32_t        Elf32_Off;
typedef uint32_t        Elf32_Word;
typedef int32_t         Elf32_Sword;
typedef uint16_t        Elf32_Half;

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
#define EI_NIDENT       16
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
} Elf32_Hdr;

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

#define ELF32_R_SYM(__INFO)             ((__INFO) >> 8)
#define ELF32_R_TYPE(__INFO)            ((unsigned char)(__INFO))
#define ELF32_R_INFO(__SYM, __TYPE)     (((__SYM) << 8) + (unsigned char)(__TYPE))
    Elf32_Word          r_info;
} Elf32_Rel;

typedef struct Elf32_Rela {
    Elf32_Addr          r_offset;
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
#define PF_R            0x4
#define PF_W            0x2
#define PF_X            0x1
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

/*----------------------------------------------------------------------------*
 * ELF64                                                                      *
 *----------------------------------------------------------------------------*/

typedef uint64_t        Elf64_Addr;
typedef uint16_t        Elf64_Half;
typedef uint64_t        Elf64_Off;
typedef int32_t         Elf64_Sword;
typedef int64_t         Elf64_Sxword;
typedef uint32_t        Elf64_Word;
typedef uint64_t        Elf64_Lword;
typedef uint64_t        Elf64_Xword;

typedef struct {
    unsigned char       e_ident[EI_NIDENT]; /* File identification. */
    Elf64_Half          e_type;             /* File type. */
    Elf64_Half          e_machine;          /* Machine architecture. */
    Elf64_Word          e_version;          /* ELF format version. */
    Elf64_Addr          e_entry;            /* Entry point. */
    Elf64_Off           e_phoff;            /* Program header file offset. */
    Elf64_Off           e_shoff;            /* Section header file offset. */
    Elf64_Word          e_flags;            /* Architecture-specific flags. */
    Elf64_Half          e_ehsize;           /* Size of ELF header in bytes. */
    Elf64_Half          e_phentsize;        /* Size of program header entry. */
    Elf64_Half          e_phnum;            /* Number of program header entries. */
    Elf64_Half          e_shentsize;        /* Size of section header entry. */
    Elf64_Half          e_shnum;            /* Number of section header entries. */
    Elf64_Half          e_shstrndx;         /* Section name strings section. */
} Elf64_Hdr;

typedef struct {
    Elf64_Word          sh_name;            /* Section name (index into the section header string table). */
    Elf64_Word          sh_type;            /* Section type. */
    Elf64_Xword         sh_flags;           /* Section flags. */
    Elf64_Addr          sh_addr;            /* Address in memory image. */
    Elf64_Off           sh_offset;          /* Offset in file. */
    Elf64_Xword         sh_size;            /* Size in bytes. */
    Elf64_Word          sh_link;            /* Index of a related section. */
    Elf64_Word          sh_info;            /* Depends on section type. */
    Elf64_Xword         sh_addralign;       /* Alignment in bytes. */
    Elf64_Xword         sh_entsize;         /* Size of each entry in section. */
} Elf64_Shdr;

typedef struct {
    Elf64_Word          p_type;             /* Entry type. */
    Elf64_Word          p_flags;            /* Access permission flags. */
    Elf64_Off           p_offset;           /* File offset of contents. */
    Elf64_Addr          p_vaddr;            /* Virtual address in memory image. */
    Elf64_Addr          p_paddr;            /* Physical address (not used). */
    Elf64_Xword         p_filesz;           /* Size of contents in file. */
    Elf64_Xword         p_memsz;            /* Size of contents in memory. */
    Elf64_Xword         p_align;            /* Alignment in memory and file. */
} Elf64_Phdr;

typedef struct {
    Elf64_Sxword        d_tag;              /* Entry type. */
    union {
        Elf64_Xword     d_val;              /* Integer value. */
        Elf64_Addr      d_ptr;              /* Address value. */
    } d_un;
} Elf64_Dyn;

typedef struct {
    Elf64_Addr          r_offset;           /* Location to be relocated. */

#define	ELF64_R_SYM(__INFO)             ((__INFO) >> 32)
#define	ELF64_R_TYPE(__INFO)            ((__INFO) & 0xffffffffL)
#define	ELF64_R_INFO(__SYM, __TYPE)     (((__SYM) << 32) + ((__TYPE) & 0xffffffffL))
    Elf64_Xword         r_info;             /* Relocation type and symbol index. */
} Elf64_Rel;

typedef struct {
    Elf64_Addr          r_offset;           /* Location to be relocated. */
    Elf64_Xword         r_info;             /* Relocation type and symbol index. */
    Elf64_Sxword        r_addend;           /* Addend. */
} Elf64_Rela;

typedef struct {
    Elf64_Word          st_name;            /* String table index of name. */
    unsigned char       st_info;            /* Type and binding information. */
    unsigned char       st_other;           /* Reserved (not used). */
    Elf64_Half          st_shndx;           /* Section index of symbol. */
    Elf64_Addr          st_value;           /* Symbol value. */
    Elf64_Xword         st_size;            /* Size of associated object. */
} Elf64_Sym;

/*----------------------------------------------------------------------------*
 * Common                                                                     *
 *----------------------------------------------------------------------------*/

#ifdef __VSF64__
#   define Elf_Addr     Elf64_Addr
#   define Elf_Off      Elf64_Off
#   define Elf_Word     Elf64_Word
#   define Elf_Sword    Elf64_Sword
#   define Elf_Half     Elf64_Half

#   define Elf_Hdr      Elf64_Hdr
#   define Elf_Shdr     Elf64_Shdr
#   define Elf_Sym      Elf64_Sym
#   define Elf_Rel      Elf64_Rel
#   define Elf_Rela     Elf64_Rela
#   define Elf_Phdr     Elf64_Phdr
#   define Elf_Dyn      Elf64_Dyn

#   define ELF_R_SYM    ELF64_R_SYM
#   define ELF_R_TYPE   ELF64_R_TYPE
#   define ELF_R_INFO   ELF64_R_INFO
#else
#   define Elf_Addr     Elf32_Addr
#   define Elf_Off      Elf32_Off
#   define Elf_Word     Elf32_Word
#   define Elf_Sword    Elf32_Sword
#   define Elf_Half     Elf32_Half

#   define Elf_Hdr      Elf32_Hdr
#   define Elf_Shdr     Elf32_Shdr
#   define Elf_Sym      Elf32_Sym
#   define Elf_Rel      Elf32_Rel
#   define Elf_Rela     Elf32_Rela
#   define Elf_Phdr     Elf32_Phdr
#   define Elf_Dyn      Elf32_Dyn

#   define ELF_R_SYM    ELF32_R_SYM
#   define ELF_R_TYPE   ELF32_R_TYPE
#   define ELF_R_INFO   ELF32_R_INFO
#endif

/*----------------------------------------------------------------------------*
 * ARCH related                                                               *
 *----------------------------------------------------------------------------*/

// Relocation codes for ARM
#define R_ARM_NONE               0             /* No relocation */
#define R_ARM_PC24               1             /* ARM       ((S + A) | T) - P */
#define R_ARM_ABS32              2             /* Data      (S + A) | T */
#define R_ARM_REL32              3             /* Data      ((S + A) | T) - P */
#define R_ARM_LDR_PC_G0          4             /* ARM       S + A - P */
#define R_ARM_ABS16              5             /* Data      S + A */
#define R_ARM_ABS12              6             /* ARM       S + A */
#define R_ARM_THM_ABS5           7             /* Thumb16   S + A */
#define R_ARM_ABS8               8             /* Data      S + A */
#define R_ARM_SBREL32            9             /* Data      ((S + A) | T) - B(S) */
#define R_ARM_THM_CALL           10            /* Thumb32   ((S + A) | T) - P */
#define R_ARM_THM_PC8            11            /* Thumb16   S + A - Pa */
#define R_ARM_BREL_ADJ           12            /* Data      ?B(S) + A */
#define R_ARM_TLS_DESC           13            /* Data     */
#define R_ARM_THM_SWI8           14            /* Obsolete */
#define R_ARM_XPC25              15            /* Obsolete */
#define R_ARM_THM_XPC22          16            /* Obsolete */
#define R_ARM_TLS_DTPMOD32       17            /* Data      Module[S] */
#define R_ARM_TLS_DTPOFF32       18            /* Data      S + A - TLS */
#define R_ARM_TLS_TPOFF32        19            /* Data      S + A - tp */
#define R_ARM_COPY               20            /* Miscellaneous */
#define R_ARM_GLOB_DAT           21            /* Data      (S + A) | T */
#define R_ARM_JUMP_SLOT          22            /* Data      (S + A) | T */
#define R_ARM_RELATIVE           23            /* Data      B(S) + A */
#define R_ARM_GOTOFF32           24            /* Data      ((S + A) | T) - GOT_ORG */
#define R_ARM_BASE_PREL          25            /* Data      B(S) + A - P */
#define R_ARM_GOT_BREL           26            /* Data      GOT(S) + A - GOT_ORG */
#define R_ARM_PLT32              27            /* ARM       ((S + A) | T) - P */
#define R_ARM_CALL               28            /* ARM       ((S + A) | T) - P */
#define R_ARM_JUMP24             29            /* ARM       ((S + A) | T) - P */
#define R_ARM_THM_JUMP24         30            /* Thumb32   ((S + A) | T) - P */
#define R_ARM_BASE_ABS           31            /* Data      B(S) + A */
#define R_ARM_ALU_PCREL_7_0      32            /* Obsolete */
#define R_ARM_ALU_PCREL_15_8     33            /* Obsolete */
#define R_ARM_ALU_PCREL_23_15    34            /* Obsolete */
#define R_ARM_LDR_SBREL_11_0_NC  35            /* ARM       S + A - B(S) */
#define R_ARM_ALU_SBREL_19_12_NC 36            /* ARM       S + A - B(S) */
#define R_ARM_ALU_SBREL_27_20_CK 37            /* ARM       S + A - B(S) */
#define R_ARM_TARGET1            38            /* Miscellaneous (S + A) | T or ((S + A) | T) - P */
#define R_ARM_SBREL31            39            /* Data      ((S + A) | T) - B(S) */
#define R_ARM_V4BX               40            /* Miscellaneous */
#define R_ARM_TARGET2            41            /* Miscellaneous */
#define R_ARM_PREL31             42            /* Data      ((S + A) | T) - P */
#define R_ARM_MOVW_ABS_NC        43            /* ARM       (S + A) | T */
#define R_ARM_MOVT_ABS           44            /* ARM       S + A */
#define R_ARM_MOVW_PREL_NC       45            /* ARM       ((S + A) | T) - P */
#define R_ARM_MOVT_PREL          46            /* ARM       S + A - P */
#define R_ARM_THM_MOVW_ABS_NC    47            /* Thumb32   (S + A) | T */
#define R_ARM_THM_MOVT_ABS       48            /* Thumb32   S + A */
#define R_ARM_THM_MOVW_PREL_NC   49            /* Thumb32   ((S + A) | T) - P */
#define R_ARM_THM_MOVT_PREL      50            /* Thumb32   S + A - P */
#define R_ARM_THM_JUMP19         51            /* Thumb32   ((S + A) | T) - P */
#define R_ARM_THM_JUMP6          52            /* Thumb16   S + A - P */
#define R_ARM_THM_ALU_PREL_11_0  53            /* Thumb32   ((S + A) | T) - Pa */
#define R_ARM_THM_PC12           54            /* Thumb32   S + A - Pa */
#define R_ARM_ABS32_NOI          55            /* Data      S + A */
#define R_ARM_REL32_NOI          56            /* Data      S + A - P */
#define R_ARM_ALU_PC_G0_NC       57            /* ARM       ((S + A) | T) - P */
#define R_ARM_ALU_PC_G0          58            /* ARM       ((S + A) | T) - P */
#define R_ARM_ALU_PC_G1_NC       59            /* ARM       ((S + A) | T) - P */
#define R_ARM_ALU_PC_G1          60            /* ARM       ((S + A) | T) - P */
#define R_ARM_ALU_PC_G2          61            /* ARM       ((S + A) | T) - P */
#define R_ARM_LDR_PC_G1          62            /* ARM       S + A - P */
#define R_ARM_LDR_PC_G2          63            /* ARM       S + A - P */
#define R_ARM_LDRS_PC_G0         64            /* ARM       S + A - P */
#define R_ARM_LDRS_PC_G1         65            /* ARM       S + A - P */
#define R_ARM_LDRS_PC_G2         66            /* ARM       S + A - P */
#define R_ARM_LDC_PC_G0          67            /* ARM       S + A - P */
#define R_ARM_LDC_PC_G1          68            /* ARM       S + A - P */
#define R_ARM_LDC_PC_G2          69            /* ARM       S + A - P */
#define R_ARM_ALU_SB_G0_NC       70            /* ARM       ((S + A) | T) - B(S) */
#define R_ARM_ALU_SB_G0          71            /* ARM       ((S + A) | T) - B(S) */
#define R_ARM_ALU_SB_G1_NC       72            /* ARM       ((S + A) | T) - B(S) */
#define R_ARM_ALU_SB_G1          73            /* ARM       ((S + A) | T) - B(S) */
#define R_ARM_ALU_SB_G2          74            /* ARM       ((S + A) | T) - B(S) */
#define R_ARM_LDR_SB_G0          75            /* ARM       S + A - B(S) */
#define R_ARM_LDR_SB_G1          76            /* ARM       S + A - B(S) */
#define R_ARM_LDR_SB_G2          77            /* ARM       S + A - B(S) */
#define R_ARM_LDRS_SB_G0         78            /* ARM       S + A - B(S) */
#define R_ARM_LDRS_SB_G1         79            /* ARM       S + A - B(S) */
#define R_ARM_LDRS_SB_G2         80            /* ARM       S + A - B(S) */
#define R_ARM_LDC_SB_G0          81            /* ARM       S + A - B(S) */
#define R_ARM_LDC_SB_G1          82            /* ARM       S + A - B(S) */
#define R_ARM_LDC_SB_G2          83            /* ARM       S + A - B(S) */
#define R_ARM_MOVW_BREL_NC       84            /* ARM       ((S + A) | T) - B(S) */
#define R_ARM_MOVT_BREL          85            /* ARM       S + A - B(S) */
#define R_ARM_MOVW_BREL          86            /* ARM       ((S + A) | T) - B(S) */
#define R_ARM_THM_MOVW_BREL_NC   87            /* Thumb32   ((S + A) | T) - B(S) */
#define R_ARM_THM_MOVT_BREL      88            /* Thumb32   S + A - B(S) */
#define R_ARM_THM_MOVW_BREL      89            /* Thumb32   ((S + A) | T) - B(S) */
#define R_ARM_TLS_GOTDESC        90            /* Data */
#define R_ARM_TLS_CALL           91            /* ARM */
#define R_ARM_TLS_DESCSEQ        92            /* ARM       TLS relaxation */
#define R_ARM_THM_TLS_CALL       93            /* Thumb32 */
#define R_ARM_PLT32_ABS          94            /* Data      PLT(S) + A */
#define R_ARM_GOT_ABS            95            /* Data      GOT(S) + A */
#define R_ARM_GOT_PREL           96            /* Data      GOT(S) + A - P */
#define R_ARM_GOT_BREL12         97            /* ARM       GOT(S) + A - GOT_ORG */
#define R_ARM_GOTOFF12           98            /* ARM       S + A - GOT_ORG */
#define R_ARM_GOTRELAX           99            /* Miscellaneous */
#define R_ARM_GNU_VTENTRY        100           /* Data */
#define R_ARM_GNU_VTINHERIT      101           /* Data */
#define R_ARM_THM_JUMP11         102           /* Thumb16   S + A - P */
#define R_ARM_THM_JUMP8          103           /* Thumb16   S + A - P */
#define R_ARM_TLS_GD32           104           /* Data      GOT(S) + A - P */
#define R_ARM_TLS_LDM32          105           /* Data      GOT(S) + A - P */
#define R_ARM_TLS_LDO32          106           /* Data      S + A - TLS */
#define R_ARM_TLS_IE32           107           /* Data      GOT(S) + A - P */
#define R_ARM_TLS_LE32           108           /* Data      S + A - tp */
#define R_ARM_TLS_LDO12          109           /* ARM       S + A - TLS */
#define R_ARM_TLS_LE12           110           /* ARM       S + A - tp */
#define R_ARM_TLS_IE12GP         111           /* ARM       GOT(S) + A - GOT_ORG */
#define R_ARM_ME_TOO             128           /* Obsolete */
#define R_ARM_THM_TLS_DESCSEQ16  129           /* Thumb16 */
#define R_ARM_THM_TLS_DESCSEQ32  130           /* Thumb32 */

#endif      // __ELF_H__
