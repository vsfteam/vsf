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
/*============================ TYPES =========================================*/

typedef enum vsf_i2c_cmd_t {
    VSF_I2C_CMD_WRITE           = (0x00ul << 3),
    VSF_I2C_CMD_READ            = (0x01ul << 3),

    VSF_I2C_CMD_START           = (0x01ul << 0),
    VSF_I2C_CMD_NO_START        = (0x00ul << 0),

    VSF_I2C_CMD_RESTART         = (0x01ul << 21),
    VSF_I2C_CMD_NO_RESTART      = (0x00ul << 21),

    VSF_I2C_CMD_7_BITS          = (0x00ul << 23),
    VSF_I2C_CMD_10_BITS         = (0x01ul << 23),

    __I2C_CMD_HW_MASK           = VSF_I2C_CMD_WRITE |
                                  VSF_I2C_CMD_READ |
                                  VSF_I2C_CMD_START |
                                  VSF_I2C_CMD_RESTART,

    VSF_I2C_CMD_STOP            = (0x01ul << 25),
    VSF_I2C_CMD_NO_STOP         = (0x00ul << 25),
} vsf_i2c_cmd_t;

/*============================ INCLUDES ======================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* __HAL_DRIVER_AIC8800_I2C_H__ */
#endif /* EOF */