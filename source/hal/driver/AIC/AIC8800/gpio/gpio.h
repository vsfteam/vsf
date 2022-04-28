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
 *  limitations under the License.                                           *x
 *                                                                           *
 ****************************************************************************/

#ifndef __HAL_DRIVER_AIC8800_HW_GPIO_H__
#define __HAL_DRIVER_AIC8800_HW_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED
#   include "../__device.h"
#   include "./i_reg_gpio.h"
#   include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"

/*============================ MACROS ========================================*/

#define VSF_IO_REIMPLEMENT_FEATURE          ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum io_feature_t {
    //! no-pull resistor
    IO_NOT_PULL             =  (0 << IOMUX_GPIO_CONFIG_PULL_FRC_LSB),
    //! pull-up resistor
    IO_PULL_UP              = ((1 << IOMUX_AGPIO_CONFIG_PULL_FRC_LSB) | (1 << IOMUX_AGPIO_CONFIG_PULL_UP_LSB)),
    //! pull-down resistor
    IO_PULL_DOWN            = ((1 << IOMUX_AGPIO_CONFIG_PULL_FRC_LSB) | (1 << IOMUX_AGPIO_CONFIG_PULL_DN_LSB)),
    __IO_PULL_MASK          = IOMUX_AGPIO_CONFIG_PULL_FRC_MASK | IOMUX_AGPIO_CONFIG_PULL_DN_MASK | IOMUX_AGPIO_CONFIG_PULL_UP_MASK,

    __IO_FEATURE_MASK       = __IO_PULL_MASK | IOMUX_GPIO_CONFIG_SEL_MASK,
} io_feature_t;

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_API_DECLARATION_PREFIX         vsf_hw
#define VSF_GPIO_CFG_INSTANCE_DECLARATION_PREFIX    VSF_HW
#include "hal/driver/common/io/io_template.h"

#endif /* VSF_HAL_USE_GPIO */
#endif /* __HAL_DRIVER_AIC8800_HW_GPIO_H__ */