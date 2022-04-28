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

// force convert vsf_i2c_init to vsf_i2c_init
#define VSF_I2C_CFG_PREFIX                  vsf_gpio
#define VSF_I2C_CFG_UPPERCASE_PREFIX        VSF_GPIO
#define VSF_I2C_CFG_REQUEST_TEMPLATE        ENABLED

#define __VSF_HAL_GPIO_I2C_CLASS_IMPLEMENT  ENABLED

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO_I2C == ENABLED

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    GPIO_I2C_CMD_ADDRESS                        = (0x01ul << 0),
    GPIO_I2C_CMD_DATA                           = (0x01ul << 1),
    GPIO_I2C_CMD_ACK_0                          = (0x01ul << 2),
    GPIO_I2C_CMD_ACK_1                          = (0x01ul << 3),
    GPIO_I2C_CMD_ACK_2                          = (0x01ul << 4),
    I2C_CMD_STOP_1                              = (0x01ul << 5),
    I2C_CMD_RESTAR_1                            = (0x01ul << 6),

    GPIO_I2C_CMD_SCL_FALLING_EDGE               = (0x01ul << 7),
    GPIO_I2C_CMD_SCL_RISING_EDGE                = (0x00ul << 8),
    GPIO_I2C_CMD_SCL_EDGE                       = (0x01ul << 9),
};

/*============================ INCLUDES ======================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vsf_i2c_master_irq_handler_base(vsf_gpio_i2c_t *gpio_i2c_ptr, uint32_t interrupt_mask, uint32_t param)
{
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);

    gpio_i2c_ptr->is_busy = false;
    gpio_i2c_ptr->irq_mask = 0;

    vsf_i2c_request_irq_handler((vsf_i2c_t *)gpio_i2c_ptr, &gpio_i2c_ptr->request, interrupt_mask, param);

    return VSF_ERR_NONE;
}

static void __vsf_gpio_i2c_callback(vsf_gpio_i2c_callback_timer_t *on_timer)
{
    vsf_gpio_i2c_t *gpio_i2c_ptr = container_of(on_timer, vsf_gpio_i2c_t, callback_timer);
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);

    if (gpio_i2c_ptr->irq_mask & I2C_IRQ_MASK_MASTER_NACK_DETECT) {
        if (gpio_i2c_ptr->cmd & I2C_CMD_STOP) {
            goto label_stop;
        }  else {
            __vsf_i2c_master_irq_handler_base(gpio_i2c_ptr,
                                              gpio_i2c_ptr->irq_mask & gpio_i2c_ptr->enabled_irq_mask, 0);
            return;
        }
    }
    if (gpio_i2c_ptr->cmd & I2C_CMD_START) {
        vsf_gpio_write(gpio_i2c_ptr->sda.gpio, 0,
                       gpio_i2c_ptr->sda.pin_mask);
        vsf_gpio_write(gpio_i2c_ptr->scl.gpio, 0,
                       gpio_i2c_ptr->scl.pin_mask);
        gpio_i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_STARTED;
        gpio_i2c_ptr->cmd &= ~I2C_CMD_START;
        goto label_address;
    } else if (gpio_i2c_ptr->cmd & I2C_CMD_RESTAR) {
        vsf_gpio_write(gpio_i2c_ptr->sda.gpio,
                       gpio_i2c_ptr->sda.pin_mask,
                       gpio_i2c_ptr->sda.pin_mask);
        vsf_gpio_write(gpio_i2c_ptr->scl.gpio,
                       gpio_i2c_ptr->scl.pin_mask,
                       gpio_i2c_ptr->scl.pin_mask);
        gpio_i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_STARTED;
        gpio_i2c_ptr->cmd &= ~I2C_CMD_RESTAR;
        gpio_i2c_ptr->gpio_i2c_cmd |= I2C_CMD_RESTAR_1;
        goto label_return;
    } else if (gpio_i2c_ptr->gpio_i2c_cmd & I2C_CMD_RESTAR_1) {
        vsf_gpio_write(gpio_i2c_ptr->sda.gpio, 0,
                       gpio_i2c_ptr->sda.pin_mask);
        vsf_gpio_write(gpio_i2c_ptr->scl.gpio, 0,
                       gpio_i2c_ptr->scl.pin_mask);
        gpio_i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_STARTED;
        goto label_address;
    } else if (gpio_i2c_ptr->gpio_i2c_cmd & GPIO_I2C_CMD_SCL_FALLING_EDGE) {
        vsf_gpio_write(gpio_i2c_ptr->scl.gpio, 0,
                       gpio_i2c_ptr->scl.pin_mask);
        gpio_i2c_ptr->gpio_i2c_cmd &= ~GPIO_I2C_CMD_SCL_FALLING_EDGE;
        goto label_return;
    } else if (gpio_i2c_ptr->gpio_i2c_cmd & GPIO_I2C_CMD_ADDRESS) {
    label_address:
        if (gpio_i2c_ptr->cmd & I2C_CMD_10_BITS) {//todo:test 10 bit
            uint16_t temp = gpio_i2c_ptr->data & 0x3ff;
            if (gpio_i2c_ptr->cmd & I2C_CMD_RW_MASK) {
                temp |= 0xf100;
            } else {
                temp |= 0xf000;
            }
            gpio_i2c_ptr->data = temp;
        } else {
            gpio_i2c_ptr->data <<= 1;
            if (gpio_i2c_ptr->cmd & I2C_CMD_RW_MASK) {
                gpio_i2c_ptr->cmd &= ~I2C_CMD_RW_MASK;
                gpio_i2c_ptr->data |= 0x01;//read
            }
        }
        gpio_i2c_ptr->gpio_i2c_cmd = GPIO_I2C_CMD_DATA;
        goto label_return;
    } else if (gpio_i2c_ptr->gpio_i2c_cmd & GPIO_I2C_CMD_DATA) {
        if (gpio_i2c_ptr->cmd & I2C_CMD_RW_MASK) {
            uint8_t gpio_value = vsf_gpio_read(gpio_i2c_ptr->sda.gpio);
            if (gpio_value & gpio_i2c_ptr->sda.pin_mask) {
                gpio_value = gpio_i2c_ptr->bit_mask;
            } else {
                gpio_value = 0;
            }
            gpio_i2c_ptr->data |= gpio_value;
        } else {
            bool status = gpio_i2c_ptr->data & gpio_i2c_ptr->bit_mask;
            vsf_gpio_write(gpio_i2c_ptr->sda.gpio,
                           status ? ~(uint32_t)0 : (uint32_t)0,
                           gpio_i2c_ptr->sda.pin_mask);
        }
        vsf_gpio_write(gpio_i2c_ptr->scl.gpio,
                       gpio_i2c_ptr->scl.pin_mask,
                       gpio_i2c_ptr->scl.pin_mask);
        gpio_i2c_ptr->bit_mask >>= 1;
        if (    (0 == gpio_i2c_ptr->bit_mask)//todo:test 10 bit
            ||  ((1 << 7) == gpio_i2c_ptr->bit_mask)) {
            if (gpio_i2c_ptr->irq_mask & I2C_IRQ_MASK_MASTER_STARTED) {
                if (gpio_i2c_ptr->cmd & I2C_CMD_10_BITS) {
                    gpio_i2c_ptr->cmd &= ~I2C_CMD_10_BITS;
                    gpio_i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_10_BITS_ADDRESS_SEND;
                } else {
                    gpio_i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_ADDRESS_SEND;
                }
            } else {
                gpio_i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_TX_EMPTY;
            }
            gpio_i2c_ptr->gpio_i2c_cmd = GPIO_I2C_CMD_ACK_0;
        }
    } else if (gpio_i2c_ptr->gpio_i2c_cmd & GPIO_I2C_CMD_ACK_0) {
        vsf_gpio_set_input(gpio_i2c_ptr->sda.gpio,
                           gpio_i2c_ptr->sda.pin_mask);

        vsf_gpio_write(gpio_i2c_ptr->scl.gpio,
                       gpio_i2c_ptr->scl.pin_mask,
                       gpio_i2c_ptr->scl.pin_mask);
        if (    (vsf_gpio_read(gpio_i2c_ptr->sda.gpio))
            &   (gpio_i2c_ptr->sda.pin_mask)) {
            gpio_i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_NACK_DETECT;
            vsf_gpio_set_output(gpio_i2c_ptr->sda.gpio,
                                gpio_i2c_ptr->sda.pin_mask);
            __vsf_i2c_master_irq_handler_base(gpio_i2c_ptr,
                                              gpio_i2c_ptr->irq_mask & gpio_i2c_ptr->enabled_irq_mask,
                                              0);
            return;
        } else {
            vsf_gpio_set_output(gpio_i2c_ptr->sda.gpio,
                                gpio_i2c_ptr->sda.pin_mask);
            gpio_i2c_ptr->gpio_i2c_cmd = GPIO_I2C_CMD_ACK_1;
            goto label_return;
        }
    } else if (gpio_i2c_ptr->gpio_i2c_cmd & GPIO_I2C_CMD_ACK_1) {
        vsf_gpio_write(gpio_i2c_ptr->scl.gpio, 0,
                       gpio_i2c_ptr->scl.pin_mask);

        vsf_gpio_write(gpio_i2c_ptr->sda.gpio,
                       gpio_i2c_ptr->sda.pin_mask,
                       gpio_i2c_ptr->sda.pin_mask);
        gpio_i2c_ptr->gpio_i2c_cmd = 0;
        goto label_return;
    } else if (gpio_i2c_ptr->cmd & I2C_CMD_STOP) {
    label_stop:
        vsf_gpio_write(gpio_i2c_ptr->sda.gpio, 0,
                       gpio_i2c_ptr->sda.pin_mask);
        vsf_gpio_write(gpio_i2c_ptr->scl.gpio,
                       gpio_i2c_ptr->scl.pin_mask,
                       gpio_i2c_ptr->scl.pin_mask);
        gpio_i2c_ptr->cmd = 0;
        gpio_i2c_ptr->gpio_i2c_cmd = I2C_CMD_STOP_1;
        goto label_return;
    } else if (gpio_i2c_ptr->gpio_i2c_cmd & I2C_CMD_STOP_1) {
        vsf_gpio_write(gpio_i2c_ptr->sda.gpio,
                       gpio_i2c_ptr->sda.pin_mask,
                       gpio_i2c_ptr->sda.pin_mask);
        __vsf_i2c_master_irq_handler_base(gpio_i2c_ptr,
                                          gpio_i2c_ptr->irq_mask & gpio_i2c_ptr->enabled_irq_mask,
                                          0);
        return;
    }
    if (    (0 != gpio_i2c_ptr->cmd)
        ||  (0 != gpio_i2c_ptr->gpio_i2c_cmd)) {
        gpio_i2c_ptr->gpio_i2c_cmd |= GPIO_I2C_CMD_SCL_FALLING_EDGE;
    label_return:
        vsf_gpio_i2c_callback_timer_add_us(on_timer, VSF_GPIO_I2C_CFG_MAX_CLOCK_HZ / gpio_i2c_ptr->request.cfg.clock_hz);
    } else {
        __vsf_i2c_master_irq_handler_base(gpio_i2c_ptr,
                                          gpio_i2c_ptr->irq_mask & gpio_i2c_ptr->enabled_irq_mask,
                                          0);
        return;
    }
}

vsf_err_t vsf_gpio_i2c_init(vsf_i2c_t *i2c_ptr, i2c_cfg_t *cfg_ptr)
{
    vsf_gpio_i2c_t *gpio_i2c_ptr = (vsf_gpio_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    if (cfg_ptr->clock_hz > VSF_GPIO_I2C_CFG_MAX_CLOCK_HZ) {
        return VSF_ERR_INVALID_PARAMETER;
    }


    vsf_gpio_config_pin(gpio_i2c_ptr->scl.gpio, gpio_i2c_ptr->scl.pin_mask, IO_PULL_UP);
    vsf_gpio_config_pin(gpio_i2c_ptr->sda.gpio, gpio_i2c_ptr->sda.pin_mask, IO_PULL_UP);
    vsf_gpio_set_output(gpio_i2c_ptr->scl.gpio, gpio_i2c_ptr->scl.pin_mask);
    vsf_gpio_set_output(gpio_i2c_ptr->sda.gpio, gpio_i2c_ptr->sda.pin_mask);

    gpio_i2c_ptr->request.cfg = *cfg_ptr;
    vsf_gpio_i2c_callback_timer_init(&gpio_i2c_ptr->callback_timer, __vsf_gpio_i2c_callback);

    return VSF_ERR_NONE;
}

void vsf_gpio_i2c_fini(vsf_i2c_t *i2c_ptr)
{
    vsf_gpio_i2c_t *gpio_i2c_ptr = (vsf_gpio_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);
}

fsm_rt_t vsf_gpio_i2c_enable(vsf_i2c_t *i2c_ptr)
{
    vsf_gpio_i2c_t *gpio_i2c_ptr = (vsf_gpio_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);

    gpio_i2c_ptr->is_enabled = true;

    return fsm_rt_cpl;
}

fsm_rt_t vsf_gpio_i2c_disable(vsf_i2c_t *i2c_ptr)
{
    vsf_gpio_i2c_t *gpio_i2c_ptr = (vsf_gpio_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);

    gpio_i2c_ptr->is_enabled = false;

    return fsm_rt_cpl;
}

void vsf_gpio_i2c_irq_enable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    vsf_gpio_i2c_t *gpio_i2c_ptr = (vsf_gpio_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);

    gpio_i2c_ptr->enabled_irq_mask = irq_mask;
}

void vsf_gpio_i2c_irq_disable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    vsf_gpio_i2c_t *gpio_i2c_ptr = (vsf_gpio_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);

    gpio_i2c_ptr->enabled_irq_mask &= ~irq_mask;
}

i2c_status_t vsf_gpio_i2c_status(vsf_i2c_t *i2c_ptr)
{
    vsf_gpio_i2c_t *gpio_i2c_ptr = (vsf_gpio_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);

    i2c_status_t status = {
        .use_as__peripheral_status_t.is_busy = gpio_i2c_ptr->is_busy,
    };

    return status;
}

vsf_err_t __vsf_i2c_send_cmd(vsf_i2c_t *i2c_ptr, uint16_t data, em_i2c_cmd_t command)
{
    vsf_gpio_i2c_t *gpio_i2c_ptr = (vsf_gpio_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);

    if (!gpio_i2c_ptr->is_enabled) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    if (gpio_i2c_ptr->is_busy) {
        return VSF_ERR_FAIL;
    }
    gpio_i2c_ptr->is_busy = true;

    //todo: Check for unsupported
    if (    (command & I2C_CMD_RW_MASK)
        && !(command & I2C_CMD_START)
        && !(command & I2C_CMD_RESTAR)) {
        gpio_i2c_ptr->data = 0;
    } else {
        gpio_i2c_ptr->data = data;
    }
    gpio_i2c_ptr->cmd = command;
    gpio_i2c_ptr->gpio_i2c_cmd = GPIO_I2C_CMD_DATA;
    if (command & I2C_CMD_10_BITS) {
        gpio_i2c_ptr->bit_mask = 0x1ul << 15;
    } else {
        gpio_i2c_ptr->bit_mask = 0x1ul << 7;
    }
    __vsf_gpio_i2c_callback(&gpio_i2c_ptr->callback_timer);

    return VSF_ERR_NONE;
}

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#include "hal/driver/common/i2c/i2c_template.inc"

#endif

