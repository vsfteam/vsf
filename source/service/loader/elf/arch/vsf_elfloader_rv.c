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

#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_ELF == ENABLED && (defined(__CPU_RV__) || defined(__CPU_RISCV__))

#include "utilities/vsf_utilities.h"
#define __VSF_ELFLOADER_CLASS_INHERIT__
#include "../../vsf_loader.h"

/*============================ MACROS ========================================*/

// refer to the RISC-V ELF psABI:
//   https://github.com/riscv-non-isa/riscv-elf-psabi-doc
// NOTE: RISC-V has no R_RISCV_GLOB_DAT. The GOT entries of external symbols
//  use R_RISCV_32/64 (S+A), which is equivalent when the addend is 0.
#define R_RISCV_NONE            0             /* No relocation */
#define R_RISCV_32              1             /* Data      S + A */
#define R_RISCV_64              2             /* Data      S + A (64bit) */
#define R_RISCV_RELATIVE        3             /* Data      B(S) + A */
#define R_RISCV_COPY            4             /* Miscellaneous */
#define R_RISCV_JUMP_SLOT       5             /* Data      S */

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

int vsf_elfloader_arch_relocate_sym(vsf_elfloader_t *elfloader, Elf_Addr tgtaddr, int type, Elf_Addr tgtvalue)
{
    switch (type) {
    // (S + A), including the GOT entries of external symbols
    case R_RISCV_32:
        *(uint32_t *)tgtaddr += tgtvalue;
        return 0;
#ifdef __VSF64__
    case R_RISCV_64:
        *(uint64_t *)tgtaddr += tgtvalue;
        return 0;
#endif
    // S
    case R_RISCV_JUMP_SLOT:
#ifdef __VSF64__
        *(uint64_t *)tgtaddr = tgtvalue;
#else
        *(uint32_t *)tgtaddr = tgtvalue;
#endif
        return 0;
    // B(S) + A
    // NOTE: RISC-V uses RELA, the addend is in r_addend(already merged into
    //  tgtvalue by the generic loop), the in-place content is 0. Using
    //  *tgtaddr as the addend here writes remap(0) to every slot(breaks e.g.
    //  busybox's applet_main[] table).
    case R_RISCV_RELATIVE:
#ifdef __VSF64__
        *(uint64_t *)tgtaddr = (uint64_t)vsf_elfloader_remap(elfloader, (void *)tgtvalue);
#else
        *(uint32_t *)tgtaddr = (uint32_t)vsf_elfloader_remap(elfloader, (void *)tgtvalue);
#endif
        return 0;
    }
    return -1;
}

#endif      // VSF_USE_LOADER && VSF_LOADER_USE_ELF && defined(__CPU_RV__) || defined(__CPU_RISCV__)
