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

#ifndef __HAL_I2C_REGACC_H__
#define __HAL_I2C_REGACC_H__

/* Implement common register access code for i2c */

/*============================ INCLUDES ======================================*/

#if VSF_HAL_USE_I2C == ENABLED

#if defined(__VSF_I2C_REGACC_CLASS_IMPLEMENT)
#   undef __VSF_I2C_REGACC_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ INCLUDES ======================================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_i2c_regacc_t) {
    public_member(
        vsf_i2c_t *i2c_ptr;
        uint8_t i2c_addr;
    )
    private_member(
        uint8_t is_busy         : 1;
        uint8_t is_read         : 1;
        uint8_t is_data_written : 1;
        uint8_t reg;
        uint16_t datalen;
        void *data;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

extern vsf_err_t vsf_i2c_regacc_irqhandler(vsf_i2c_regacc_t *i2c_regacc,
                        vsf_i2c_irq_mask_t irq_mask);
extern vsf_err_t vsf_i2c_regacc(vsf_i2c_regacc_t *i2c_regacc, uint_fast8_t reg,
                        bool is_read, uint8_t *data, uint_fast16_t datalen);

#endif      // VSF_HAL_I2C_IMP_REQUEST_BY_CMD
#endif      // __HAL_I2C_REG_ACCESS_H__
