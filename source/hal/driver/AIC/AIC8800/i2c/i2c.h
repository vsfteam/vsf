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
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/I2CM/reg_i2cm.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum em_i2c_feature_t {
    I2C_MODE_MASTER                             = (0x1ul << 0),  // select master mode
    I2C_MODE_SLAVE                              = (0x0ul << 0),  // select slave mode
    I2C_MODE_MASK                               = (0x1ul << 0),

    // TODO: Ultra Fast-mode I2C-bus protocol
    I2C_SPEED_STANDARD_MODE                     = (0x0ul << 1),  // up to 100 kbit/s
    I2C_SPEED_FAST_MODE                         = (0x1ul << 1),  // up to 400 kbit/s
    I2C_SPEED_FAST_MODE_PLUS                    = (0x2ul << 1),  // up to 1 Mbit/s
    I2C_SPEED_HIGH_SPEED_MODE                   = (0x3ul << 1),  // up to 3.4 Mbit/s
    I2C_SPEED_MASK                              = (0x3ul << 1),
};

// use enumeration implementation
enum em_i2c_irq_mask_t {
    // i2c hardware interrupt status, usually used by i2c template
    I2C_IRQ_MASK_MASTER_STARTED                 = (0x1ul <<  0),
    I2C_IRQ_MASK_MASTER_ADDRESS_SEND            = (0x1ul <<  1),
    I2C_IRQ_MASK_MASTER_10_BITS_ADDRESS_SEND    = (0x1ul <<  2),
    I2C_IRQ_MASK_MASTER_STOP_DETECT             = (0x1ul <<  3),
    I2C_IRQ_MASK_MASTER_NACK_DETECT             = (0x1ul <<  4),
    I2C_IRQ_MASK_MASTER_ARBITRATION_LOST        = (0x1ul <<  5),
    I2C_IRQ_MASK_MASTER_TX_EMPTY                = (0x1ul <<  6),
    I2C_IRQ_MASK_MASTER_ERROR                   = (0x1ul <<  7),

    I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE       = (0x1ul <<  8),
    I2C_IRQ_MASK_MASTER_ADDRESS_NACK            = (0x1ul << 10),

    I2C_IRQ_MASK_MASTER_ALL                     =   I2C_IRQ_MASK_MASTER_STARTED
                                                |   I2C_IRQ_MASK_MASTER_ADDRESS_SEND
                                                |   I2C_IRQ_MASK_MASTER_10_BITS_ADDRESS_SEND
                                                |   I2C_IRQ_MASK_MASTER_STOP_DETECT
                                                |   I2C_IRQ_MASK_MASTER_NACK_DETECT
                                                |   I2C_IRQ_MASK_MASTER_ARBITRATION_LOST
                                                |   I2C_IRQ_MASK_MASTER_TX_EMPTY
                                                |   I2C_IRQ_MASK_MASTER_ERROR
                                                |   I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                                                |   I2C_IRQ_MASK_MASTER_ADDRESS_NACK,
};

enum em_i2c_cmd_t {
    I2C_CMD_WRITE                               = (0x00ul << 3),
    I2C_CMD_READ                                = (0x01ul << 3),
    I2C_CMD_RW_MASK                             = (0x01ul << 3),

    I2C_CMD_START                               = (0x01ul << 0),
    I2C_CMD_STOP                                = (0x01ul << 2),
    I2C_CMD_RESTAR                              = (0x01ul << 21),
    I2C_CMD_ADDRESS                             = I2C_CMD_START
                                                | I2C_CMD_STOP
                                                | I2C_CMD_RESTAR,

    I2C_CMD_7_BITS                              = (0x00ul << 0),
    I2C_CMD_10_BITS                             = (0x00ul << 0),
    I2C_CMD_BITS_MASK                           = (0x00ul << 0),

    I2C_ALL_CMD                                 = I2C_CMD_RW_MASK
                                                | I2C_CMD_ADDRESS
                                                | I2C_CMD_BITS_MASK,
};

/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_i2c.h"

/*============================ TYPES =========================================*/

struct i2c_status_t {
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

struct i2c_capability_t {
    uint32_t                                :32;
    // TODO
};

struct vsf_i2c_t {
    AIC_I2CM_TypeDef                *REG_PARAM;
    i2c_cfg_t                       cfg;
    i2c_capability_t                capability;
    i2c_status_t                    status;
    em_i2c_irq_mask_t               irq_mask;
    uint16_t                        data_length;
    uint8_t                         *data;
};

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern vsf_i2c_t vsf_i2c0;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* __HAL_DRIVER_AIC8800_I2C_H__ */
#endif /* EOF */