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

#ifndef __VSF_INPUT_CST8_H__
#define __VSF_INPUT_CST8_H__

/*  Driver for CST8 series from Hynitron Microelectronics  */

/*============================ INCLUDES ======================================*/

#include "component/input/vsf_input_cfg.h"

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_CST8 == ENABLED

#include "hal/vsf_hal.h"

#if     defined(__VSF_INPUT_CST8_CLASS_IMPLEMENT)
#   undef __VSF_INPUT_CST8_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_I2C != ENABLED
#   error Touch Screen chip CST8 series need I2C enabled
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_input_cst8_t) {
    public_member(
        union {
            vsf_i2c_regacc_t regacc;
            vsf_i2c_regacc_t;
        };

        vsf_gpio_irq_distributor_t *gpio_int_port;
        uint8_t gpio_int_pin_idx;

        uint16_t width;
        uint16_t height;
    )
    private_member(
        union {
            struct {
                uint8_t dummy;
                uint8_t getture_id;
                uint8_t finger_num;
                uint8_t xpos_h;
                uint8_t xpos_l;
                uint8_t ypos_h;
                uint8_t ypos_l;
            };
            uint8_t buffer[7];
        };
        vsf_arch_prio_t prio;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_input_cst8_init(vk_input_cst8_t *csr8, vsf_arch_prio_t prio);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_INPUT && VSF_INPUT_USE_HID
#endif      // __VSF_INPUT_HID_H__
