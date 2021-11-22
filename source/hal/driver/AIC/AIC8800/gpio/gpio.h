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
 *  limitations under the License.                                           *x
 *                                                                           *
 ****************************************************************************/

#ifndef __HAL_DRIVER_AIC8800_GPIO_H__
#define __HAL_DRIVER_AIC8800_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#if VSF_HAL_USE_GPIO == ENABLED
#include "../__device.h"
#include "./i_reg_gpio.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum io_feature_t {
    IO_PULL_UP              = (1 << 0),           //!< enable pull-up resistor
    IO_PULL_DOWN            = (1 << 1),           //!< enable pull-down resistor
//todo:
    IO_OPEN_DRAIN           = 0,
    IO_DISABLE_INPUT        = 0,
    IO_INVERT_INPUT         = 0,
    IO_FILTER_BYPASS        = 0,
    IO_FILTER_2CLK          = 0,
    IO_FILTER_4CLK          = 0,
    IO_FILTER_8CLK          = 0,
    IO_FILTER_CLK_SRC0      = 0,
    IO_FILTER_CLK_SRC1      = 0,
    IO_FILTER_CLK_SRC2      = 0,
    IO_FILTER_CLK_SRC3      = 0,
    IO_FILTER_CLK_SRC4      = 0,
    IO_FILTER_CLK_SRC5      = 0,
    IO_FILTER_CLK_SRC6      = 0,
    IO_FILTER_CLK_SRC7      = 0,
    IO_HIGH_DRV             = 0,
    IO_HIGH_DRIVE           = 0,
    IO_HIGH_DRIVE_STRENGTH  = 0,
};

//todo: remove
enum io_pin_no_t {
    TODO_REMOVE,
};
/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_io.h"

typedef struct gpio_reg_t {
    GPIO_REG_T *GPIO;
    AIC_IOMUX_TypeDef *IOMUX;
} gpio_reg_t;

struct vsf_gpio_t {
    gpio_reg_t REG;
    bool is_pmic;
};

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/**
 \~english
    GPIOA instance
 \~chinese
    GPIOA 实例
 */
extern vsf_gpio_t vsf_gpio0;

/**
 \~english
    GPIOB instance
 \~chinese
    GPIOB 实例
 */
extern vsf_gpio_t vsf_gpio1;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* VSF_HAL_USE_GPIO */
#endif /* EOF */