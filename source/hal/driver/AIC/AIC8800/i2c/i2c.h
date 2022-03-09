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

#ifndef __HAL_DRIVER_AIC8800_I2C_H__
#define __HAL_DRIVER_AIC8800_I2C_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_I2C == ENABLED

#include "../__device.h"
#include "hal/driver/common/template/vsf_template_hal_driver.h"

/*============================ MACROS ========================================*/

#define VSF_I2C_CFG_REIMPLEMENT_STATUS          ENABLED

/*============================ TYPES =========================================*/

typedef struct i2c_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t                            : 1;
            uint32_t                is_enabled  : 1;
            uint32_t                irq_enabled : 1;
            uint32_t                            : 29;
        } status_bool;
    };
} i2c_status_t;

typedef struct vsf_hw_i2c_t vsf_hw_i2c_t;

/*============================ INCLUDES ======================================*/

// undef after include vsf_template_i2c.h
#define VSF_I2C_CFG_DEC_PREFIX                      vsf_hw
#define VSF_I2C_CFG_DEC_UPPERCASE_PREFIX            VSF_HW
#define VSF_I2C_CFG_DEC_LV0(__count, __dont_care)   \
    extern vsf_hw_i2c_t vsf_hw_i2c ## __count;

#include "hal/driver/common/template/vsf_template_i2c.h"

/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* __HAL_DRIVER_AIC8800_I2C_H__ */
#endif /* EOF */