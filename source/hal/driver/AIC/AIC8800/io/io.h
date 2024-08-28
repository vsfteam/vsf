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

#ifndef __HAL_DRIVER_AIC8800_HW_IO_H__
#define __HAL_DRIVER_AIC8800_HW_IO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_IO == ENABLED

#include "../__device.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_io_mode_t {
    // bit 9  : IOMUX_AGPIO_CONFIG_PULL_UP_LSB
    // bit 8  : IOMUX_AGPIO_CONFIG_PULL_DN_LSB
    // bit 16 : IOMUX_GPIO_CONFIG_PULL_FRC_LSB
    __AIC8800_IO_MODE_ALL_BITS      = (1 << 8) | (1 << 9) | (1 << 16),

    VSF_IO_PULL_UP                  = (1 << 16) | (1 << 9),
    VSF_IO_PULL_DOWN                = (1 << 16) | (1 << 8),
    VSF_IO_NO_PULL_UP_DOWN          = (0 << 16) | (0 << 8),

    VSF_IO_INPUT                    = (0 << 17),
    VSF_IO_OUTPUT_OPEN_DRAIN        = (1 << 17),
    VSF_IO_OUTPUT_PUSH_PULL         = (2 << 17),
    VSF_IO_ANALOG                   = (3 << 17),
    VSF_IO_EXTI                     = (4 << 17),
    VSF_IO_AF                       = (5 << 17),

    VSF_IO_EXTI_MODE_NONE           = (0 << 21),
    VSF_IO_EXTI_MODE_LOW_LEVEL      = (1 << 21),
    VSF_IO_EXTI_MODE_HIGH_LEVEL     = (2 << 21),
    VSF_IO_EXTI_MODE_RISING         = (3 << 21),
    VSF_IO_EXTI_MODE_FALLING        = (4 << 21),
    VSF_IO_EXTI_MODE_RISING_FALLING = (5 << 21),

} vsf_io_mode_t;

/*============================ PROTOTYPES ====================================*/

extern uint32_t aic8800_io_reg_read(bool is_pmic, volatile uint32_t *reg);
extern void aic8800_io_reg_mask_write(bool is_pmic, volatile uint32_t *reg,
                                      uint32_t wdata, uint32_t wmask);
/*============================ INCLUDES ======================================*/

#endif /* VSF_HAL_USE_IO */
#endif /* __HAL_DRIVER_AIC8800_HW_IO_H__ */
