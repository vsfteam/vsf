/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#ifndef __HAL_DRIVER_ALLWINNER_F1CX00S_DRAM_H__
#define __HAL_DRIVER_ALLWINNER_F1CX00S_DRAM_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"
#include "utilities/vsf_utilities.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum f1cx00s_dram_type_t {
    DRAM_TYPE_SDR               = 0,
    DRAM_TYPE_DDR               = 1,
    DRAM_TYPE_MDDR              = 2,
};

typedef struct f1cx00s_dram_param_t {
    uint32_t base;              /* dram base address */
    uint32_t size;              /* dram size (unit: MByte) */
    uint32_t clk;               /* dram work clock (unit: MHz) */
    uint32_t access_mode;       /* 0: interleave mode 1: sequence mode */
    uint32_t cs_num;            /* dram chip count  1: one chip  2: two chip */
    uint32_t ddr8_remap;        /* for 8bits data width DDR 0: normal  1: 8bits */
    enum f1cx00s_dram_type_t sdr_ddr;
    uint32_t bwidth;            /* dram bus width */
    uint32_t col_width;         /* column address width */
    uint32_t row_width;         /* row address width */
    uint32_t bank_size;         /* dram bank count */
    uint32_t cas;               /* dram cas */
} f1cx00s_dram_param_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

// return dram size in MB if > 0, else error
extern int f1cx00s_dram_init(f1cx00s_dram_param_t *param);

#endif
/* EOF */
