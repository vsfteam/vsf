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

#define __VSF_I2C_UTIL_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

#if VSF_HAL_USE_I2C == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_i2c_regacc_irqhandler(vsf_i2c_regacc_t *i2c_regacc, vsf_i2c_irq_mask_t irq_mask)
{
    if (irq_mask != VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
        // nack/abort: mark idle so the client(eg. an address prober) can
        // start the next transfer instead of tripping the !is_busy assert
        i2c_regacc->is_busy = false;
        return VSF_ERR_FAIL;
    }

    if (i2c_regacc->is_regaddr) {
        i2c_regacc->is_regaddr = false;
        if (i2c_regacc->is_read) {
            vsf_i2c_master_request(i2c_regacc->i2c_ptr, i2c_regacc->i2c_addr,
                VSF_I2C_CMD_RESTART | VSF_I2C_CMD_READ | VSF_I2C_CMD_STOP,
                i2c_regacc->datalen, i2c_regacc->data);
        } else {
            vsf_i2c_master_request(i2c_regacc->i2c_ptr, i2c_regacc->i2c_addr,
                VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP,
                i2c_regacc->datalen, i2c_regacc->data);
        }
        return VSF_ERR_NOT_READY;
    }

    i2c_regacc->is_busy = false;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_i2c_regacc(vsf_i2c_regacc_t *i2c_regacc, uint_fast16_t reg,
                            bool is_read, uint8_t *data, uint_fast16_t datalen)
{
    VSF_HAL_ASSERT((i2c_regacc != NULL) && (i2c_regacc->i2c_ptr != NULL));
    VSF_HAL_ASSERT((data != NULL) && (datalen > 0));
    VSF_HAL_ASSERT(!i2c_regacc->is_busy);
    // a register number beyond one byte on a one-byte address config is
    // necessarily a caller bug - the high byte would go out silently
    // dropped(set reg_addr_len for devices with a 16-bit register map)
    VSF_HAL_ASSERT((i2c_regacc->reg_addr_len >= 2) || (reg <= 0xFF));

    i2c_regacc->is_busy = true;
    i2c_regacc->is_regaddr = true;
    i2c_regacc->is_read = is_read;
    i2c_regacc->data = data;
    i2c_regacc->datalen = datalen;
    i2c_regacc->reg = reg;

    uint_fast8_t regaddr_len;
    if (i2c_regacc->reg_addr_len >= 2) {
        regaddr_len = 2;
        i2c_regacc->reg_buf[0] = (uint8_t)(reg >> 8);
        i2c_regacc->reg_buf[1] = (uint8_t)reg;
    } else {
        regaddr_len = 1;
        i2c_regacc->reg_buf[1] = (uint8_t)reg;
    }
    vsf_i2c_master_request(i2c_regacc->i2c_ptr, i2c_regacc->i2c_addr,
            VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE,
            regaddr_len, &i2c_regacc->reg_buf[2 - regaddr_len]);
    return VSF_ERR_NONE;
}

#endif      // VSF_HAL_USE_I2C
