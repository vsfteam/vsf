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


/*============================ INCLUDES ======================================*/

#if VSF_HAL_USE_I2C == ENABLED

#include "kernel/vsf_kernel.h"
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_I2C_CFG_PROTECT_LEVEL
#   define VSF_I2C_CFG_PROTECT_LEVEL        interrupt
#endif

#define __vsf_i2c_protect                   vsf_protect(VSF_I2C_CFG_PROTECT_LEVEL)
#define __vsf_i2c_unprotect                 vsf_unprotect(VSF_I2C_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static uint16_t __vsf_i2c_get_send_cmd_data(i2c_type_ptr *i2c_ptr)
{
    __i2c_req_by_cmd_t *req_by_cmd = &i2c_ptr->__req_by_cmd;
    uint16_t ret_data = (uint16_t)(req_by_cmd->buffer_ptr[req_by_cmd->idx - 1]);
    return ret_data;
}

/**
 * \~chinese
 * @brief i2c低级函数，发送一个消息给硬件
 *        配合模板快速实现 I2C 驱动
 *
 * @param[in] i2c_ptr: 结构体 i2c_type_ptr 的指针，参考 @ref i2c_type_ptr
 * @param[in] data : 将要发送的数据或地址
 * @param[in] command : 将要发送的命令，参考 @ref em_i2c_cmd_t
 * @return vsf_err_t : 不支持的消息将会返回负数
 */
static vsf_err_t __vsf_i2c_send_cmd(i2c_type_ptr *i2c_ptr,
                                    uint16_t data,
                                    em_i2c_cmd_t command);

/**
 * \~chinese
 * @brief i2c 低级函数，I2C 主机中断回调函数
 *            配合模板快速实现 I2C 驱动
 *
 * @param[in] i2c_ptr: 结构体 i2c_type_ptr 的指针，参考 @ref i2c_type_ptr
 * @param[in] interrupt_mask : 一个或者多个中断的组合
 * @return none
 */
static void __vsf_i2c_master_irq_handler(i2c_type_ptr *i2c_ptr, uint32_t interrupt_mask, uint32_t param)
{
    i2c_cfg_t *cfg = &i2c_ptr->cfg;
    __i2c_req_by_cmd_t *req_by_cmd = &i2c_ptr->__req_by_cmd;
    if (interrupt_mask & I2C_IRQ_MASK_MASTER_NACK_DETECT) {
        if (NULL != cfg->isr.handler_fn) {
            cfg->isr.handler_fn(cfg->isr.target_ptr, (i2c_type_ptr *)i2c_ptr, interrupt_mask);
        }
        return;
    }
    req_by_cmd->idx++;
    if (req_by_cmd->idx > req_by_cmd->count) {
        if (NULL != cfg->isr.handler_fn) {
            cfg->isr.handler_fn(cfg->isr.target_ptr, (i2c_type_ptr *)i2c_ptr, interrupt_mask);
        }
        //todo: has no function
    } else {
        em_i2c_cmd_t temp_cmd;
        if (req_by_cmd->idx == req_by_cmd->count) {
            temp_cmd = req_by_cmd->cmd;
        } else {
            temp_cmd = req_by_cmd->cmd & ~I2C_CMD_STOP;
        }
        temp_cmd &= ~(I2C_CMD_START | I2C_CMD_RESTAR);
        vsf_err_t ret = __vsf_i2c_send_cmd(i2c_ptr, __vsf_i2c_get_send_cmd_data(i2c_ptr), temp_cmd);
    }
}

static void __i2c_driver_master_request_init(i2c_type_ptr *i2c_ptr,
                                          uint16_t address,
                                          em_i2c_cmd_t cmd,
                                          uint16_t count,
                                          uint8_t *buffer_ptr)
{
    i2c_ptr->__req_by_cmd.address = address;
    i2c_ptr->__req_by_cmd.cmd = cmd;
    i2c_ptr->__req_by_cmd.count = count;
    i2c_ptr->__req_by_cmd.idx = 0;
    i2c_ptr->__req_by_cmd.buffer_ptr = buffer_ptr;
}

vsf_err_t i2c_driver_master_request(i2c_type_ptr *i2c_ptr,
                                 uint16_t address,
                                 em_i2c_cmd_t cmd,
                                 uint16_t count,
                                 uint8_t *buffer_ptr)
{
    __i2c_driver_master_request_init(i2c_ptr, address, cmd, count, buffer_ptr);
    if (count > 0) {
        cmd = cmd & ~I2C_CMD_STOP;
    }
    vsf_protect_t orig = __vsf_i2c_protect();
    vsf_err_t ret = __vsf_i2c_send_cmd(i2c_ptr, address, cmd);
    __vsf_i2c_unprotect(orig);
    return ret;
}

/*============================ IMPLEMENTATION ================================*/

#endif
