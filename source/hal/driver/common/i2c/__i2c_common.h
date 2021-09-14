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

#if VSF_HAL_I2C_IMP_REQUEST_BY_CMD == ENABLED


/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define vsf_hal_i2c_def_req_by_cmd()                                            \
        __i2c_req_by_cmd_t     __req_by_cmd;

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_i2c_master_irq_handler

/*============================ TYPES =========================================*/

typedef struct __i2c_req_by_cmd_t {
    uint16_t            address;
    em_i2c_cmd_t        cmd;
    uint16_t            count;
    uint16_t            idx;
    uint8_t             *buffer_ptr;
} __i2c_req_by_cmd_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#endif // VSF_HAL_I2C_IMP_REQUEST_BY_CMD
#endif
