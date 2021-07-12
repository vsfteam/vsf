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
#include "../../__device.h"
#if VSF_HAL_USE_GPIO == ENABLED
#include "./i_reg_gpio.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum io_feature_t {
    IO_PULL_UP              = (1<<4),           //!< enable pull-up resistor
    IO_OPEN_DRAIN           = (1<<10),          //!< enable open-drain mode

    IO_DISABLE_INPUT        = (1<<7),          //!< disable input
    IO_INVERT_INPUT         = (1<<6),          //!< invert the input pin level


    IO_FILTER_BYPASS        = (0<<11),           //!< filter is bypassed
    IO_FILTER_2CLK          = (1<<11),           //!< levels should keep 2 clks
    IO_FILTER_4CLK          = (2<<11),           //!< levels should keep 4 clks
    IO_FILTER_8CLK          = (3<<11),           //!< levels should keep 8 clks

    IO_FILTER_CLK_SRC0      = (0<<13),           //!< select clock src 0 for filter
    IO_FILTER_CLK_SRC1      = (1<<13),           //!< select clock src 1 for filter
    IO_FILTER_CLK_SRC2      = (2<<13),           //!< select clock src 2 for filter
    IO_FILTER_CLK_SRC3      = (3<<13),           //!< select clock src 3 for filter
    IO_FILTER_CLK_SRC4      = (4<<13),           //!< select clock src 4 for filter
    IO_FILTER_CLK_SRC5      = (5<<13),           //!< select clock src 5 for filter
    IO_FILTER_CLK_SRC6      = (6<<13),           //!< select clock src 6 for filter
    IO_FILTER_CLK_SRC7      = (7<<13),           //!< select clock src 7 for filter

    IO_HIGH_DRV             = (1<<9),           //!< enable high drive strength
    IO_HIGH_DRIVE           = (1<<9),           //!< enable high drive strength
    IO_HIGH_DRIVE_STRENGTH  = (1<<9),           //!< enable high drive strength
};

//todo: remove
enum io_pin_no_t {
    TODO_REMOVE,
};
/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_io.h"

typedef struct gpio_reg_t {
    GPIO_REG_T                  *GPIO;
    AIC_IOMUX_TypeDef           *IOMUX;
} gpio_reg_t;

typedef struct vsf_gpio_t {
    gpio_reg_t                  REG;
} vsf_gpio_t;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/

/**
 ** port definition:
 **     High 16 bits is port B
 **     Low 16 bits is port A
 **/
extern vsf_gpio_t vsf_gpio0;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* VSF_HAL_USE_GPIO */
#endif /* EOF */