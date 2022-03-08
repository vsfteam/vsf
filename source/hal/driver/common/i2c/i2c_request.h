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

#ifndef __HAL_DRIVER_I2C_CMD_TO_REQUEST_H__
#define __HAL_DRIVER_I2C_CMD_TO_REQUEST_H__

#if VSF_HAL_USE_I2C == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef vsf_err_t vsf_i2c_request_send_cmd_fn(vsf_i2c_t *i2c_ptr,
                                              em_i2c_cmd_t command,
                                              uint16_t data);

typedef struct vsf_i2c_request_t {
    i2c_cfg_t                       cfg;
    uint16_t                        address;
    em_i2c_cmd_t                    cmd;
    uint16_t                        idx;
    uint16_t                        count;
    uint8_t                        *buffer_ptr;
    vsf_i2c_request_send_cmd_fn    *fn;
} vsf_i2c_request_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_i2c_request_irq_handler(vsf_i2c_t *i2c_ptr,
                                        vsf_i2c_request_t *i2c_request_ptr,
                                        uint32_t interrupt_mask,
                                        uint32_t param);

extern vsf_err_t vsf_i2c_request_master_request(vsf_i2c_t *i2c_ptr,
                                                vsf_i2c_request_t *i2c_request_ptr,
                                                uint16_t address,
                                                em_i2c_cmd_t cmd,
                                                uint16_t count,
                                                uint8_t *buffer_ptr);

/*============================ IMPLEMENTATION ================================*/


#endif /* VSF_HAL_USE_I2C */
#endif /* __HAL_DRIVER_I2C_CMD_TO_REQUEST_H__ */
