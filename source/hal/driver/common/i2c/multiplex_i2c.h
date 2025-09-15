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

#ifndef __HAL_DRIVER_COMMON_MULTIPLEX_I2C_H__
#define __HAL_DRIVER_COMMON_MULTIPLEX_I2C_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if (VSF_HAL_USE_I2C == ENABLED) && (VSF_HAL_USE_MULTIPLEX_I2C == ENABLED)

#if defined(__VSF_I2C_MULTIPLEX_CLASS_IMPLEMENT)
#   undef __VSF_I2C_MULTIPLEX_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#ifndef VSF_MULTIPLEX_I2C_CFG_MULTI_CLASS
#   define VSF_MULTIPLEX_I2C_CFG_MULTI_CLASS        VSF_I2C_CFG_MULTI_CLASS
#endif

#ifndef VSF_MULTIPLEXER_I2C_CFG_MASK_TYPE
#   define VSF_MULTIPLEXER_I2C_CFG_MASK_TYPE        uint8_t
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ INCLUDES ======================================*/

#if VSF_MULTIPLEX_I2C_CFG_MULTI_CLASS == ENABLED
#   define __describe_multiplex_i2c_op()        .op = &vsf_multiplex_i2c_op,
#else
#   define __describe_multiplex_i2c_op()
#endif

#define __describe_multiplex_i2c(__multiplexer, __name)                         \
    vsf_multiplex_i2c_t __name = {                                              \
        __describe_multiplex_i2c_op()                                           \
        .multiplexer = &(__multiplexer),                                        \
    };

#define __describe_multiplexer_i2c(__name, __i2c, ...)                          \
    vsf_multiplexer_i2c_t __name = {                                            \
        .i2c_ptr = __i2c,                                                       \
    };                                                                          \
    VSF_MFOREACH_ARG1(__describe_multiplex_i2c, __name, __VA_ARGS__)

#define describe_multiplexer_i2c(__name, __i2c, ...)                            \
    __describe_multiplexer_i2c(__name, __i2c, __VA_ARGS__)

/*============================ TYPES =========================================*/

typedef VSF_MULTIPLEXER_I2C_CFG_MASK_TYPE vsf_i2c_multiplex_mask_t;

vsf_declare_class(vsf_multiplex_i2c_t)

vsf_class(vsf_multiplexer_i2c_t) {
    public_member(
        vsf_i2c_t *i2c_ptr;
    )

    private_member(
        vsf_slist_queue_t waiting_queue;

        vsf_multiplex_i2c_t *req_m_i2c;
        vsf_multiplex_i2c_t *inited_m_i2c;
        vsf_i2c_irq_mask_t irq_mask;     // All CS IRQ Mask Wire-OR
        vsf_i2c_multiplex_mask_t init_mask;
        vsf_i2c_multiplex_mask_t en_mask;
    )
};

vsf_class(vsf_multiplex_i2c_t) {
    public_member(
#if VSF_MULTIPLEX_I2C_CFG_MULTI_CLASS == ENABLED
        implement(vsf_i2c_t)
#endif
        vsf_multiplexer_i2c_t * const multiplexer;
    )

    private_member(
        vsf_slist_node_t slist_node;
        vsf_i2c_cfg_t cfg;
        vsf_i2c_irq_mask_t irq_mask;
        uint8_t id;

        struct {
            vsf_i2c_cmd_t cmd;
            uint8_t *buffer_ptr;
            uint16_t address;
            uint16_t count;
        } request;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_I2C_CFG_DEC_PREFIX              vsf_multiplex
#define VSF_I2C_CFG_DEC_UPCASE_PREFIX       VSF_MULTIPLEX
#define VSF_I2C_CFG_DEC_EXTERN_OP           ENABLED
#include "hal/driver/common/i2c/i2c_template.h"

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif // VSF_HAL_I2C_IMP_REQUEST_BY_CMD
#endif
