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

#ifndef __HAL_DRIVER_AIC8800_I2S_H__
#define __HAL_DRIVER_AIC8800_I2S_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_I2S == ENABLED

#include "../__device.h"

/*============================ MACROS ========================================*/

#define VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK   ENABLED

/*============================ TYPES =========================================*/

typedef enum vsf_i2s_irq_mask_t {
    I2S_IRQ_MASK_TX_CPL         = (0x1ul <<  0),
    I2S_IRQ_MASK_RX_CPL         = (0x1ul <<  1),

    I2S_IRQ_MASK_ALL            = I2S_IRQ_MASK_TX_CPL | I2S_IRQ_MASK_RX_CPL,
} vsf_i2s_irq_mask_t;

/*============================ INCLUDES ======================================*/

#define VSF_I2S_CFG_DEC_PREFIX                  vsf_hw
#define VSF_I2S_CFG_DEC_UPCASE_PREFIX           VSF_HW
#include "hal/driver/common/i2s/i2s_template.h"

/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* __HAL_DRIVER_AIC8800_I2S_H__ */
#endif /* EOF */