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
//#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/gpio/reg_gpio.h"
//#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"
/*============================ MACROS ========================================*/

#ifndef GPIO_COUNT
#   define GPIO_COUNT                       2
#endif

#define GPIO_SET                            (1)

#define GPIO_PORT_A                         (BIT(0))
#define GPIO_PORT_B                         (BIT(1))

#define GPIO_PIN_0					        (BIT(0))
#define GPIO_PIN_1					        (BIT(1))
#define GPIO_PIN_2					        (BIT(2))
#define GPIO_PIN_3					        (BIT(3))
#define GPIO_PIN_4					        (BIT(4))
#define GPIO_PIN_5					        (BIT(5))
#define GPIO_PIN_6					        (BIT(6))
#define GPIO_PIN_7					        (BIT(7))
#define GPIO_PIN_8					        (BIT(8))
#define GPIO_PIN_9					        (BIT(9))
#define GPIO_PIN_10					        (BIT(10))
#define GPIO_PIN_11					        (BIT(11))
#define GPIO_PIN_12					        (BIT(12))
#define GPIO_PIN_13					        (BIT(13))
#define GPIO_PIN_14					        (BIT(14))
#define GPIO_PIN_15					        (BIT(15))
#define GPIO_PIN_ALL                        (0XFFFF)

#define GPIO_PIN_0_OUTPUT                   (BIT(0))
#define GPIO_PIN_0_INPUT					(0)

#define GPIO_PIN_1_OUTPUT					(BIT(1))
#define GPIO_PIN_1_INPUT					(0)

#define GPIO_PIN_2_OUTPUT					(BIT(2))
#define GPIO_PIN_2_INPUT					(0)

#define GPIO_PIN_3_OUTPUT					(BIT(3))
#define GPIO_PIN_3_INPUT					(0)

#define GPIO_PIN_4_OUTPUT					(BIT(4))
#define GPIO_PIN_4_INPUT					(0)

#define GPIO_PIN_5_OUTPUT					(BIT(5))
#define GPIO_PIN_5_INPUT					(0)

#define GPIO_PIN_6_OUTPUT					(BIT(6))
#define GPIO_PIN_6_INPUT					(0)

#define GPIO_PIN_7_OUTPUT					(BIT(7))
#define GPIO_PIN_7_INPUT					(0)

#define GPIO_PIN_8_OUTPUT					(BIT(8))
#define GPIO_PIN_8_INPUT					(0)

#define GPIO_PIN_9_OUTPUT					(BIT(9))
#define GPIO_PIN_9_INPUT					(0)

#define GPIO_PIN_10_OUTPUT					(BIT(10))
#define GPIO_PIN_10_INPUT                   (0)

#define GPIO_PIN_11_OUTPUT					(BIT(11))
#define GPIO_PIN_11_INPUT					(0)

#define GPIO_PIN_12_OUTPUT					(BIT(12))
#define GPIO_PIN_12_INPUT					(0)

#define GPIO_PIN_13_OUTPUT					(BIT(13))
#define GPIO_PIN_13_INPUT					(0)

#define GPIO_PIN_14_OUTPUT					(BIT(14))
#define GPIO_PIN_14_INPUT					(0)

#define GPIO_PIN_15_OUTPUT					(BIT(15))
#define GPIO_PIN_15_INPUT					(0)

#define GPIO_PIN_ALL_OUTPUT                 (0XFFFF)
#define GPIO_PIN_ALL_INPUT                  (0)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//todo: remove
enum io_pin_no_t {
    TEMP_DETOUR                     = GPIO_PORT_A,
};
/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_io.h"

typedef struct gpio_reg_t {
    GPIO_REG_T                  *gpio;
    IOMUX_REG_T                 *iomux;
} gpio_reg_t;

typedef struct vsf_gpio_t {
    gpio_reg_t                  reg;
    void                        *current_param;
    uint8_t                     port;
} vsf_gpio_t;


/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern vsf_gpio_t vsf_gpio0;
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* VSF_HAL_USE_GPIO */
#endif /* EOF */