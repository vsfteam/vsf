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

/*============================ MACROS ========================================*/

#define VSF_I2C_CFG_REIMPLEMENT_CMD     ENABLED

/*============================ TYPES =========================================*/

typedef enum em_i2c_cmd_t {
    I2C_CMD_WRITE       = (0x00ul << 3),
    I2C_CMD_READ        = (0x01ul << 3),
    I2C_CMD_RW_MASK     = I2C_CMD_WRITE | I2C_CMD_READ,

    I2C_CMD_START       = (0x01ul <<  0),
    I2C_CMD_STOP        = (0x01ul << 22),
    I2C_CMD_RESTAR      = (0x01ul << 21),
    I2C_CMD_COND_RESTAR = (0x01ul << 21),

    I2C_CMD_7_BITS      = (0x00ul << 23),
    I2C_CMD_10_BITS     = (0x01ul << 23),
    I2C_CMD_BITS_MASK   =  I2C_CMD_7_BITS
                         | I2C_CMD_10_BITS,

    __I2C_CMD_HW_MASK   = I2C_CMD_RW_MASK | I2C_CMD_START | I2C_CMD_RESTAR,

    I2C_CMD_ALL_MASK    = I2C_CMD_RW_MASK | I2C_CMD_COND_RESTAR | I2C_CMD_BITS_MASK,
} em_i2c_cmd_t;

/*============================ INCLUDES ======================================*/

#define VSF_I2C_CFG_API_DECLARATION_PREFIX          vsf_hw
#define VSF_I2C_CFG_INSTANCE_DECLARATION_PREFIX     VSF_HW
#include "hal/driver/common/i2c/i2c_template.h"

/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* __HAL_DRIVER_AIC8800_I2C_H__ */
#endif /* EOF */