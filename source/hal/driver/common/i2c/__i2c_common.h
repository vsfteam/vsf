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

#ifndef __HAL_DRIVER_COMMON_VSF_HAL_I2C_CS_H__
#define __HAL_DRIVER_COMMON_VSF_HAL_I2C_CS_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/**
 * \~chinese
 * @brief i2c低级函数，发送一个消息(em_i2c_msg_flags_t的组合)给硬件
 *
 * @param message : 将要发送的消息
 * @return vsf_err_t : 不支持的消息将会返回负数
 */
extern vsf_err_t __vsf_i2c_send_msg(uint32_t message);

/**
 * \~chinese
 * @brief i2c低级函数，接收到中断(em_i2c_irq_mask_t的组合)
 *
 * @param interrupt_mask : 将要发送的消息
 * @return vsf_err_t : 不支持的消息将会返回负数
 */
extern vsf_err_t __vsf_i2c_receive_isr(uint32_t interrupt_mask);

/*============================ IMPLEMENTATION ================================*/



#endif
