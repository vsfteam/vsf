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

#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_ELF == ENABLED && defined(__ARM_ARCH_PROFILE)

#include "utilities/vsf_utilities.h"
#define __VSF_ELFLOADER_CLASS_INHERIT__
#include "../../vsf_loader.h"

/*============================ MACROS ========================================*/

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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

int vsf_elfloader_arch_relocate_sym(vsf_elfloader_t *elfloader, Elf_Addr tgtaddr, int type, Elf_Addr tgtvalue)
{
    switch (type) {
    // (S + A) | T
    case R_ARM_ABS32:
        *(uint32_t *)tgtaddr += tgtvalue;
        return 0;
    case R_ARM_GLOB_DAT:
    case R_ARM_JUMP_SLOT:
        *(uint32_t *)tgtaddr = tgtvalue;
        return 0;
    case R_ARM_RELATIVE:
        *(uint32_t *)tgtaddr = (uint32_t)vsf_elfloader_remap(elfloader, (void *)*(uint32_t *)tgtaddr);
        return 0;
    }
    return -1;
}

#endif      // VSF_USE_LOADER && VSF_LOADER_USE_ELF && defined(__ARM_ARCH_PROFILE)
