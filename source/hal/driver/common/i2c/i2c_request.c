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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_I2C == ENABLED

#include "hal/driver/driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_I2C_REQUEST_CFG_PROTECT_LEVEL
#   define VSF_I2C_REQUEST_CFG_PROTECT_LEVEL    interrupt
#endif

#define __vsf_i2c_protect                       vsf_protect(VSF_I2C_REQUEST_CFG_PROTECT_LEVEL)
#define __vsf_i2c_unprotect                     vsf_unprotect(VSF_I2C_REQUEST_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * \~chinese
 * @brief i2c 低级函数，I2C 主机中断回调函数
 *            配合模板快速实现 I2C 驱动
 *
 * @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 * @param[in] vsf_i2c_request_t: 结构体 vsf_i2c_request_t 的指针，参考 @ref vsf_i2c_request_t
 * @param[in] interrupt_mask : 一个或者多个中断的组合
 * @return none
 */
void vsf_i2c_request_irq_handler(vsf_i2c_t *i2c_ptr,
                                 vsf_i2c_request_t *i2c_request_ptr,
                                 uint32_t interrupt_mask,
                                 uint32_t param)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(NULL != i2c_request_ptr);

    vsf_i2c_cfg_t *cfg = &i2c_request_ptr->cfg;
    uint32_t cur_interrupt_mask = 0;

    if (interrupt_mask & VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT) {
        cur_interrupt_mask |= VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT;
    } else {
        if (++i2c_request_ptr->idx <= i2c_request_ptr->count) {
            vsf_i2c_cmd_t temp_cmd = i2c_request_ptr->cmd & ~(VSF_I2C_CMD_START | VSF_I2C_CMD_RESTART);
            if (i2c_request_ptr->idx != i2c_request_ptr->count) {
                temp_cmd &= ~VSF_I2C_CMD_STOP;
            }
            uint16_t data = i2c_request_ptr->buffer_ptr[i2c_request_ptr->idx - 1];
            vsf_err_t ret = i2c_request_ptr->fn(i2c_ptr, data, temp_cmd);
            VSF_HAL_ASSERT(ret == VSF_ERR_NONE);
        } else {
            cur_interrupt_mask = VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;
        }
    }

    if (cur_interrupt_mask) {
        if (NULL != cfg->isr.handler_fn) {
            cfg->isr.handler_fn(cfg->isr.target_ptr, i2c_ptr, cur_interrupt_mask);
        }
    }
}

vsf_err_t vsf_i2c_request_master_request(vsf_i2c_t *i2c_ptr,
                                         vsf_i2c_request_t *i2c_request_ptr,
                                         uint16_t address,
                                         vsf_i2c_cmd_t cmd,
                                         uint16_t count,
                                         uint8_t *buffer_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(NULL != i2c_request_ptr);
    VSF_HAL_ASSERT(NULL != i2c_request_ptr->fn);

    i2c_request_ptr->address = address;
    i2c_request_ptr->cmd = cmd;
    i2c_request_ptr->count = count;
    i2c_request_ptr->idx = 0;
    i2c_request_ptr->buffer_ptr = buffer_ptr;

    if (count > 0) {
        cmd = cmd & ~VSF_I2C_CMD_STOP;
    }

    vsf_protect_t orig = __vsf_i2c_protect();
    vsf_err_t ret = i2c_request_ptr->fn(i2c_ptr, address, cmd);
    __vsf_i2c_unprotect(orig);

    return ret;
}

#endif // VSF_HAL_USE_I2C == ENABLED

