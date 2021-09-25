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

#ifndef __HAL_DRIVER_COMMON_I2C_MULTIPLEX_H__
#define __HAL_DRIVER_COMMON_I2C_MULTIPLEX_H__

#if VSF_HAL_I2C_IMP_MULTIPLEX_I2C == ENABLED

/*============================ INCLUDES ======================================*/

/*============================ MACROS ========================================*/

#if     defined(__VSF_I2C_MULTIPLEX_CLASS_IMPLEMENT)
#   undef __VSF_I2C_MULTIPLEX_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __MULTIPLEX_I2C_DEF(__N, __MI2C)                                        \
    extern vsf_i2c_multiplex_t __MI2C##__N;

/*============================ INCLUDES ======================================*/

#include "utilities/ooc_class.h"

/*============================ TYPES =========================================*/

typedef struct request_info_t{
    vsf_slist_node_t                request_node;
    i2c_cfg_t                       cfg;
    em_i2c_cmd_t                    cmd;
    em_i2c_irq_mask_t               irq_mask;
    uint8_t                         *buffer_ptr;
    uint16_t                        address;
    uint16_t                        count;
    struct {
        uint32_t                    is_init             : 1;
        uint32_t                    is_enabled          : 1;
        uint32_t                    is_irq_enabled      : 1;
        uint32_t                    is_busy             : 1;
        uint32_t                    is_in_queue         : 1;
        uint32_t                    is_restar           : 1;
        uint32_t                                        : 26;
    } status;
} request_info_t;

vsf_class(vsf_i2c_multiplexer_t) {
    private_member(
        vsf_slist_queue_t           request_slist;
        i2c_type_ptr                *i2c_ptr;
        bool                        is_busy;
    )
};

vsf_class(vsf_i2c_multiplex_t) {
    private_member(
        vsf_i2c_multiplexer_t       *multiplexer;
        implement(request_info_t)
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C0 == ENABLED && (I2C_PORT_MASK & (1 << 0))
#   ifndef VSF_HAL_I2C0_MULTIPLEX_CNT
#       define VSF_HAL_I2C0_MULTIPLEX_CNT 2
#   endif
VSF_MREPEAT(VSF_HAL_I2C0_MULTIPLEX_CNT, __MULTIPLEX_I2C_DEF, VSF_I2C0_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C1 == ENABLED && (I2C_PORT_MASK & (1 << 1))
#   ifndef VSF_HAL_I2C1_MULTIPLEX_CNT
#       define VSF_HAL_I2C1_MULTIPLEX_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_I2C1_MULTIPLEX_CNT, __MULTIPLEX_I2C_DEF, VSF_I2C1_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C2 == ENABLED && (I2C_PORT_MASK & (1 << 2))
#   ifndef VSF_HAL_I2C2_MULTIPLEX_CNT
#       define VSF_HAL_I2C2_MULTIPLEX_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_I2C2_MULTIPLEX_CNT, __MULTIPLEX_I2C_DEF, VSF_I2C2_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C3 == ENABLED && (I2C_PORT_MASK & (1 << 3))
#   ifndef VSF_HAL_I2C3_MULTIPLEX_CNT
#       define VSF_HAL_I2C3_MULTIPLEX_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_I2C3_MULTIPLEX_CNT, __MULTIPLEX_I2C_DEF, VSF_I2C3_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C4 == ENABLED && (I2C_PORT_MASK & (1 << 4))
#   ifndef VSF_HAL_I2C4_MULTIPLEX_CNT
#       define VSF_HAL_I2C4_MULTIPLEX_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_I2C4_MULTIPLEX_CNT, __MULTIPLEX_I2C_DEF, VSF_I2C4_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C5 == ENABLED && (I2C_PORT_MASK & (1 << 5))
#   ifndef VSF_HAL_I2C5_MULTIPLEX_CNT
#       define VSF_HAL_I2C5_MULTIPLEX_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_I2C5_MULTIPLEX_CNT, __MULTIPLEX_I2C_DEF, VSF_I2C5_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C6 == ENABLED && (I2C_PORT_MASK & (1 << 6))
#   ifndef VSF_HAL_I2C6_MULTIPLEX_CNT
#       define VSF_HAL_I2C6_MULTIPLEX_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_I2C6_MULTIPLEX_CNT, __MULTIPLEX_I2C_DEF, VSF_I2C6_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C7 == ENABLED && (I2C_PORT_MASK & (1 << 7))
#   ifndef VSF_HAL_I2C7_MULTIPLEX_CNT
#       define VSF_HAL_I2C7_MULTIPLEX_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_I2C7_MULTIPLEX_CNT, __MULTIPLEX_I2C_DEF, VSF_I2C7_MULTIPLEX)
#endif

#endif // VSF_HAL_I2C_IMP_REQUEST_BY_CMD
#endif
