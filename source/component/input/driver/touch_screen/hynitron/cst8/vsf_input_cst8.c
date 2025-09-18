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

/*============================ INCLUDES ======================================*/

#define __VSF_I2C_UTIL_CLASS_INHERIT__
#define __VSF_INPUT_CST8_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

// for vsf_input
#include "component/vsf_component.h"

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_CST8 == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_cst8_evt_t {
    CST8_EVT_ON_PRESS       = 0x00,
    CST8_EVT_ON_MOVE        = 0x80,
    CST8_EVT_ON_RELEASE     = 0x40,
} vsf_cst8_evt_t;

typedef enum vsf_cst8_mode_t {
    CST8_MODE_GESTURE       = 0x11,
    CST8_MODE_TOUCH         = 0x60,
} vsf_cst8_mode_t;

typedef enum vsf_cst8_cmd_t {
    CST8_CMD_CHIPID         = 0xA7,
    CST8_CMD_FIRMVER        = 0xA9,
    CST8_CMD_MODE           = 0xFA,
} vsf_cst8_cmd_t;

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vk_input_cst8_eint_isrhandler(void *target_ptr, vsf_gpio_t *gpio_ptr,
                vsf_gpio_pin_mask_t pin_mask)
{
    vk_input_cst8_t *cst8 = target_ptr;
    vsf_gpio_exti_irq_disable(cst8->gpio_int_port, 1 << cst8->gpio_int_pin_idx);
    vsf_i2c_regacc(&cst8->regacc, 0, true, cst8->buffer, sizeof(cst8->buffer));
}

static void __vk_input_cst8_i2c_isrhandler(void *target_ptr, vsf_i2c_t *i2c_ptr,
                vsf_i2c_irq_mask_t irq_mask)
{
    vk_input_cst8_t *cst8 = target_ptr;
    vsf_err_t err = vsf_i2c_regacc_irqhandler(&cst8->regacc, irq_mask);
    if (err < 0) {
        vsf_trace_error("dst8xx: i2c failure" VSF_TRACE_CFG_LINEEND);
    } else if (VSF_ERR_NONE == err) {
        switch (cst8->reg) {
        case CST8_CMD_CHIPID:
            vsf_trace_debug("dst8xx: chip_id %02X" VSF_TRACE_CFG_LINEEND, (uint8_t)cst8->buffer[0]);
            vsf_i2c_regacc(&cst8->regacc, CST8_CMD_FIRMVER, true, cst8->buffer, 1);
            break;
        case CST8_CMD_FIRMVER:
            vsf_trace_debug("dst8xx: firm_ver %02X" VSF_TRACE_CFG_LINEEND, (uint8_t)cst8->buffer[0]);
            cst8->buffer[0] = CST8_MODE_TOUCH;
            vsf_i2c_regacc(&cst8->regacc, CST8_CMD_MODE, false, cst8->buffer, 1);
            break;
        case CST8_CMD_MODE:
            vsf_trace_debug("dst8xx: inited" VSF_TRACE_CFG_LINEEND);
            goto enable_exti;
        case 0: {
                vk_touchscreen_evt_t ts_evt = {
                    .dev            = cst8,
                    .info.width     = cst8->width,
                    .info.height    = cst8->height,
                };
                vsf_cst8_evt_t cst8_evt = cst8->xpos_h & 0xF0;

                bool is_down = (cst8_evt == CST8_EVT_ON_PRESS) || (cst8_evt == CST8_EVT_ON_MOVE);
                uint16_t x = ((cst8->xpos_h & 0x0F) << 8) | cst8->xpos_l;
                uint16_t y = ((cst8->ypos_h & 0x0F) << 8) | cst8->ypos_l;
                vsf_input_touchscreen_set(&ts_evt, 0, is_down, 0, x, y);
                vsf_input_on_touchscreen(&ts_evt);
            }
        enable_exti:
            vsf_gpio_exti_irq_enable(cst8->gpio_int_port, 1 << cst8->gpio_int_pin_idx);
            break;
        }
    }
}

vsf_err_t vk_input_cst8_init(vk_input_cst8_t *cst8, vsf_arch_prio_t prio)
{
    VSF_INPUT_ASSERT((cst8 != NULL) && (prio != vsf_arch_prio_invalid));

    vsf_i2c_init(cst8->i2c_ptr, &(vsf_i2c_cfg_t){
        .mode           = VSF_I2C_MODE_MASTER | VSF_I2C_SPEED_FAST_MODE | VSF_I2C_ADDR_7_BITS,
        .clock_hz       = 400 * 1000,
        .isr            = {
            .handler_fn = __vk_input_cst8_i2c_isrhandler,
            .target_ptr = cst8,
            .prio       = prio,
        },
    });
    vsf_i2c_irq_enable(cst8->i2c_ptr, VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                                |   VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                                |   VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT);
    vsf_i2c_enable(cst8->i2c_ptr);

    vsf_gpio_cfg_t cfg = {
        .mode = VSF_GPIO_PULL_UP | VSF_GPIO_EXTI | VSF_GPIO_EXTI_MODE_FALLING,
    };
    vsf_gpio_port_config_pins(cst8->gpio_int_port->gpio, 1 << cst8->gpio_int_pin_idx, &cfg);
    vsf_gpio_irq_distributor_pin_config(cst8->gpio_int_port, 1 << cst8->gpio_int_pin_idx, &(vsf_gpio_exti_irq_cfg_t){
        .handler_fn = __vk_input_cst8_eint_isrhandler,
        .target_ptr = cst8,
        .prio = cst8->prio,
    });
    cst8->prio = prio;
    vsf_gpio_exti_irq_disable(cst8->gpio_int_port, 1 << cst8->gpio_int_pin_idx);

    return vsf_i2c_regacc(&cst8->regacc, CST8_CMD_CHIPID, true, cst8->buffer, 1);
}

#endif
