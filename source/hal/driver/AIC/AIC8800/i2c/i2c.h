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

#ifndef __HAL_DRIVER_AIC8800_I2C_H__
#define __HAL_DRIVER_AIC8800_I2C_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#if VSF_HAL_USE_I2C == ENABLED
#include "../__device.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/i2cm/reg_i2cm.h"

/*============================ MACROS ========================================*/

#undef I2C_TEMPLATE_USE_MODULAR_NAME
#define I2C_TEMPLATE_USE_MODULAR_NAME           hw

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_i2c.h"

/*============================ TYPES =========================================*/

struct i2c_type_status {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t                            : 1;
            uint32_t                is_enabled  : 1;
            uint32_t                irq_enabled : 1;
            uint32_t                            : 29;
        } status_bool;
    };
};


struct i2c_type_ptr {
    AIC_I2CM_TypeDef                *REG_PARAM;
    i2c_cfg_t                       cfg;
    i2c_capability_t                capability;
    i2c_type_status                 status;
    em_i2c_irq_mask_t               irq_mask;
    uint16_t                        data_length;
    struct {
        uint16_t                    address;
        uint16_t                    data_size;
        uint16_t                    data_offset;
        em_i2c_cmd_t                cmd;
    } recall_info;
    uint8_t                         *data;
    VSF_I2C_INTTERFACE_TYPE_DEFINE
};

/*============================ INCLUDES ======================================*/

#if VSF_HAL_I2C_IMP_MULTIPLEX_I2C == ENABLED
#   include "hal/driver/common/i2c/i2c_multiplex/__i2c_multiplex_common.h"
#endif

/*============================ GLOBAL VARIABLES ==============================*/

extern i2c_type_ptr vsf_hw_i2c0;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* __HAL_DRIVER_AIC8800_I2C_H__ */
#endif /* EOF */