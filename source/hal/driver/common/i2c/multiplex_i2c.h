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

#ifndef __HAL_DRIVER_COMMON_I2C_MULTIPLEX_H__
#define __HAL_DRIVER_COMMON_I2C_MULTIPLEX_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if (VSF_HAL_USE_I2C == ENABLED) && (VSF_HAL_USE_MULTIPLEX_I2C == ENABLED)

#if defined(__VSF_I2C_MULTIPLEX_CLASS_IMPLEMENT)
#   undef __VSF_I2C_MULTIPLEX_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#define VSF_I2C_CFG_DEC_PREFIX              vsf_multiplex
#include "hal/driver/common/template/vsf_template_i2c.h"

/*============================ MACROS ========================================*/

#ifndef VSF_MULTIPLEXER_I2C_CFG_MASK_TYPE
#   define VSF_MULTIPLEXER_I2C_CFG_MASK_TYPE        uint8_t
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ INCLUDES ======================================*/
/*============================ TYPES =========================================*/

typedef VSF_MULTIPLEXER_I2C_CFG_MASK_TYPE i2c_multi_mask_t;

vsf_declare_class(vsf_multiplex_i2c_t)

vsf_class(vsf_multiplexer_i2c_t) {
    public_member(
        vsf_i2c_t *i2c_ptr;
    )

    private_member(
        vsf_slist_queue_t slist_queue;


        vsf_multiplex_i2c_t *current_m_i2c;
        i2c_multi_mask_t init_mask;
        i2c_multi_mask_t en_mask;
        // irq mask
        em_i2c_irq_mask_t irq_mask;     // All CS IRQ Mask Wire-OR

        bool is_busy;
    )
};

vsf_class(vsf_multiplex_i2c_t) {
    public_member(
        uint8_t id;
        vsf_multiplexer_i2c_t *multiplexer;
    )

    private_member(
        vsf_slist_node_t slist_node;
        i2c_cfg_t cfg;
        em_i2c_irq_mask_t  irq_mask;

        struct {
            em_i2c_cmd_t cmd;
            uint8_t *buffer_ptr;
            uint16_t address;
            uint16_t count;
        } request;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#endif // VSF_HAL_I2C_IMP_REQUEST_BY_CMD
#endif
