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