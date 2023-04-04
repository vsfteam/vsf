/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_ELF == ENABLED && defined(__CPU_X64__)

#include "utilities/vsf_utilities.h"
#include "../elf.h"

/*============================ MACROS ========================================*/

#define R_X86_64_NONE       0   /* No reloc */
#define R_X86_64_64         1   /* Direct 64 bit  */
#define R_X86_64_PC32       2   /* PC relative 32 bit signed */
#define R_X86_64_GOT32      3   /* 32 bit GOT entry */
#define R_X86_64_PLT32      4   /* 32 bit PLT address */
#define R_X86_64_COPY       5   /* Copy symbol at runtime */
#define R_X86_64_GLOB_DAT   6   /* Create GOT entry */
#define R_X86_64_JUMP_SLOT  7   /* Create PLT entry */
#define R_X86_64_RELATIVE   8   /* Adjust by program base */
#define R_X86_64_GOTPCREL   9   /* 32 bit signed pc relative offset to GOT */
#define R_X86_64_32         10  /* Direct 32 bit zero extended */
#define R_X86_64_32S        11  /* Direct 32 bit sign extended */
#define R_X86_64_16         12  /* Direct 16 bit zero extended */
#define R_X86_64_PC16       13  /* 16 bit sign extended pc relative */
#define R_X86_64_8          14  /* Direct 8 bit sign extended  */
#define R_X86_64_PC8        15  /* 8 bit sign extended pc relative */

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

int vsf_elfloader_relocate_sym(Elf_Addr tgtaddr, int type, Elf_Addr tgtvalue)
{
    switch (type) {
    case R_X86_64_GLOB_DAT:
    case R_X86_64_JUMP_SLOT:
        *(uint64_t *)tgtaddr = tgtvalue;
        return 0;
    }
    return -1;
}

#endif      // VSF_USE_LOADER && VSF_LOADER_USE_ELF && defined(__ARM_ARCH_PROFILE)
