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
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/**
 * \~chinese
 * @brief i2c低级函数，发送一个消息给硬件
 *        配合模板快速实现 I2C 驱动
 *
 * @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 * @param[in] data : 将要发送的数据或地址
 * @param[in] command : 将要发送的命令，参考 @ref em_i2c_cmd_t
 * @return vsf_err_t : 不支持的消息将会返回负数
 */
static vsf_err_t __vsf_i2c_send_cmd(vsf_i2c_t *i2c_ptr,
                                    uint16_t data,
                                    em_i2c_cmd_t command);

/**
 * \~chinese
 * @brief i2c 低级函数，I2C 主机中断回调函数
 *            配合模板快速实现 I2C 驱动
 *
 * @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 * @param[in] interrupt_mask : 一个或者多个中断的组合
 * @return none
 */
static void __vsf_i2c_master_irq_handler(vsf_i2c_t *i2c_ptr,
                                         uint32_t interrupt_mask,
                                         uint32_t param);

/*============================ IMPLEMENTATION ================================*/

#endif
