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
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/
/*============================ INCLUDES ======================================*/

#include "./i2c_gpio.h"
#if VSF_HAL_USE_I2C_GPIO == ENABLED

//for debug
#include "vsf.h"

#undef i2c_debug
//#define i2c_debug(...)                  vsf_trace(VSF_TRACE_DEBUG, __VA_ARGS__)
#define i2c_debug(...)


/*============================ MACROS ========================================*/

#ifndef VSF_I2C_GPIO_MAX_CLOCK_HZ
#   define VSF_I2C_GPIO_MAX_CLOCK_HZ            1000000
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    I2C_GPIO_CMD_ADDRESS                        = (0x01ul << 0),
    I2C_GPIO_CMD_DATA                           = (0x01ul << 1),
    I2C_GPIO_CMD_ACK_0                          = (0x01ul << 2),
    I2C_GPIO_CMD_ACK_1                          = (0x01ul << 3),
    I2C_GPIO_CMD_ACK_2                          = (0x01ul << 4),
    I2C_CMD_STOP_1                              = (0x01ul << 5),

    I2C_GPIO_CMD_SCL_FALLING_EDGE               = (0x01ul << 6),
    I2C_GPIO_CMD_SCL_RISING_EDGE                = (0x00ul << 7),
    I2C_GPIO_CMD_SCL_EDGE                       = (0x01ul << 8),
};

/*============================ INCLUDES ======================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

vsf_i2c_t vsf_i2c0;


/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

WEAK(sda_setting)
void sda_setting(bool status)
{
    if (status) {
        vsf_gpio_set(&vsf_gpio0, 1 << 10);
    } else {
        vsf_gpio_clear(&vsf_gpio0, 1 << 10);
    }
}

WEAK(scl_setting)
void scl_setting(bool status)
{
    if (status) {
        vsf_gpio_set(&vsf_gpio0, 1 << 12);
    } else {
        vsf_gpio_clear(&vsf_gpio0, 1 << 12);
    }
}

static vsf_err_t __vsf_i2c_receive_isr(vsf_i2c_t *i2c_ptr, uint32_t interrupt_mask)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    i2c_cfg_t *cfg = &i2c_ptr->cfg;
    i2c_ptr->status.use_as__peripheral_status_t.is_busy = false;
    if (NULL != cfg->isr.handler_fn) {
        cfg->isr.handler_fn(cfg->isr.target_ptr, (vsf_i2c_t *)i2c_ptr, interrupt_mask);
    } else {
        return VSF_ERR_INVALID_PARAMETER;
    }
    return VSF_ERR_NONE;
}

static void __i2c_gpio_callback(VSF_I2C_GPIO_USE_CALL_BACK_TIMER *on_timer)
{
    vsf_i2c_t *i2c_ptr = container_of(on_timer, vsf_i2c_t, callback_timer);

    if (i2c_ptr->i2c_gpio_cmd & I2C_GPIO_CMD_SCL_FALLING_EDGE) {
        vsf_gpio_write(i2c_ptr->gpio_info.gpio, 0,
                       i2c_ptr->gpio_info.scl_pin_mask);
        i2c_ptr->i2c_gpio_cmd &= ~I2C_GPIO_CMD_SCL_FALLING_EDGE;
        i2c_debug("\I2C_GPIO_CMD_SCL_FALLING_EDGE\n");
        goto label_return;
    } else if (     (i2c_ptr->cmd & I2C_CMD_START)/*
               |    (i2c_ptr->cmd & I2C_CMD_RESTAR)*/) {
        vsf_gpio_write(i2c_ptr->gpio_info.gpio, 0,
                       i2c_ptr->gpio_info.sda_pin_mask);
        i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_STARTED;
        i2c_ptr->cmd &= ~I2C_CMD_START;
        i2c_ptr->i2c_gpio_cmd = I2C_GPIO_CMD_ADDRESS;
    } else if (i2c_ptr->i2c_gpio_cmd & I2C_GPIO_CMD_ADDRESS) {

        if (i2c_ptr->cmd & I2C_CMD_10_BITS) {//todo:test 10 bit
            uint16_t temp = i2c_ptr->data & 0x3ff;
            if (i2c_ptr->cmd & I2C_CMD_RW_MASK) {
                temp |= 0xf100;
            } else {
                temp |= 0xf000;
            }
            i2c_ptr->data = temp;
        } else {
            i2c_ptr->data <<= 1;
            if (i2c_ptr->cmd & I2C_CMD_RW_MASK) {
                i2c_ptr->cmd &= ~I2C_CMD_RW_MASK;
                i2c_ptr->data |= 0x01;//read
            }
        }
        i2c_ptr->i2c_gpio_cmd = I2C_GPIO_CMD_DATA;
        goto label_send;
    } else if (i2c_ptr->i2c_gpio_cmd & I2C_GPIO_CMD_DATA) {
    label_send:
        if (i2c_ptr->cmd & I2C_CMD_RW_MASK) {
            uint8_t gpio_value = vsf_gpio_read(i2c_ptr->gpio_info.gpio);
        if (gpio_value & i2c_ptr->gpio_info.sda_pin_mask) {
            gpio_value = i2c_ptr->bit_mask;
        } else {
            gpio_value = 0;
        }
            i2c_ptr->data |= gpio_value;
        } else {
            bool status = i2c_ptr->data & i2c_ptr->bit_mask;
            vsf_gpio_write(i2c_ptr->gpio_info.gpio,
                           status ? ~(uint32_t)0 : (uint32_t)0,
                           i2c_ptr->gpio_info.sda_pin_mask);
        }
        vsf_gpio_write(i2c_ptr->gpio_info.gpio,
                       i2c_ptr->gpio_info.scl_pin_mask,
                       i2c_ptr->gpio_info.scl_pin_mask);
        i2c_debug("\nI2C_GPIO_CMD_DATA\nbit_mask:%x, data:%x, status:%d\n",
                  i2c_ptr->bit_mask, i2c_ptr->data, i2c_ptr->data & i2c_ptr->bit_mask);
        i2c_ptr->bit_mask >>= 1;
        if (    (0 == i2c_ptr->bit_mask)//todo:test 10 bit
            ||  ((1 << 7) == i2c_ptr->bit_mask)) {

            if (i2c_ptr->irq_mask & I2C_IRQ_MASK_MASTER_STARTED) {
                if (i2c_ptr->cmd & I2C_CMD_10_BITS) {
                    i2c_ptr->cmd &= ~I2C_CMD_10_BITS;
                    i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_10_BITS_ADDRESS_SEND;
                } else {
                    i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_ADDRESS_SEND;
                }
            } else {
                i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_TX_EMPTY;
            }
            i2c_ptr->i2c_gpio_cmd = I2C_GPIO_CMD_ACK_0;
        }
    } else if (i2c_ptr->i2c_gpio_cmd & I2C_GPIO_CMD_ACK_0) {
        vsf_gpio_set_input(i2c_ptr->gpio_info.gpio,
                           i2c_ptr->gpio_info.sda_pin_mask);
        vsf_gpio_set_input(i2c_ptr->gpio_info.gpio,
                           i2c_ptr->gpio_info.scl_pin_mask);
        if (!(  (vsf_gpio_read(i2c_ptr->gpio_info.gpio))//wait ack
            &   (i2c_ptr->gpio_info.scl_pin_mask))) {
            goto label_return;
        }
        vsf_gpio_set_output(i2c_ptr->gpio_info.gpio,
                            i2c_ptr->gpio_info.scl_pin_mask);
        vsf_gpio_write(i2c_ptr->gpio_info.gpio,
                       i2c_ptr->gpio_info.scl_pin_mask,
                       i2c_ptr->gpio_info.scl_pin_mask);

        if (    (vsf_gpio_read(i2c_ptr->gpio_info.gpio))//nack
            &   (i2c_ptr->gpio_info.sda_pin_mask)) {
            i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_NACK_DETECT;
        }
        i2c_ptr->i2c_gpio_cmd = I2C_GPIO_CMD_ACK_1;
        goto label_return;
    } else if (i2c_ptr->i2c_gpio_cmd & I2C_GPIO_CMD_ACK_1) {
        if (    (vsf_gpio_read(i2c_ptr->gpio_info.gpio))//nack
            &   (i2c_ptr->gpio_info.sda_pin_mask)) {
            i2c_ptr->irq_mask |= I2C_IRQ_MASK_MASTER_NACK_DETECT;
        }
        vsf_gpio_write(i2c_ptr->gpio_info.gpio, 0,
                       i2c_ptr->gpio_info.scl_pin_mask);

        vsf_gpio_set_output(i2c_ptr->gpio_info.gpio,
                            i2c_ptr->gpio_info.sda_pin_mask);

        vsf_gpio_write(i2c_ptr->gpio_info.gpio, 0,
                       i2c_ptr->gpio_info.sda_pin_mask);
        if ((1 << 7) == i2c_ptr->bit_mask) {
            i2c_ptr->i2c_gpio_cmd = I2C_GPIO_CMD_DATA;
        } else {
            i2c_ptr->i2c_gpio_cmd = 0;
        }
        goto label_return;
    } else if (i2c_ptr->cmd & I2C_CMD_STOP) {
        vsf_gpio_write(i2c_ptr->gpio_info.gpio,
                       i2c_ptr->gpio_info.scl_pin_mask,
                       i2c_ptr->gpio_info.scl_pin_mask);
        i2c_ptr->cmd &= ~I2C_CMD_STOP;
        i2c_ptr->i2c_gpio_cmd = I2C_CMD_STOP_1;
        goto label_return;
    } else if (i2c_ptr->i2c_gpio_cmd & I2C_CMD_STOP_1) {
        vsf_gpio_write(i2c_ptr->gpio_info.gpio,
                       i2c_ptr->gpio_info.sda_pin_mask,
                       i2c_ptr->gpio_info.sda_pin_mask);
        i2c_ptr->i2c_gpio_cmd = 0;
    } else if (I2C_CMD_RW_MASK == i2c_ptr->cmd) {
        i2c_ptr->cmd = 0;
    }
    if (    (0 != i2c_ptr->cmd)
        ||  (0 != i2c_ptr->i2c_gpio_cmd)) {
        i2c_ptr->i2c_gpio_cmd |= I2C_GPIO_CMD_SCL_FALLING_EDGE;
    label_return:
        vsf_callback_timer_add_us(on_timer,
                                  VSF_I2C_GPIO_MAX_CLOCK_HZ / i2c_ptr->cfg.clock_hz);
    } else {
        __vsf_i2c_receive_isr(i2c_ptr,
                                   i2c_ptr->irq_mask & i2c_ptr->enabled_irq_mask);
    }
}

void i2c_gpio_callback(vsf_i2c_t *i2c_ptr)
{
    __i2c_gpio_callback(&i2c_ptr->callback_timer);
}

static vsf_err_t __vsf_i2c_init(vsf_i2c_t *i2c_ptr, i2c_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr->gpio_info.gpio);
    vsf_gpio_config_pin(i2c_ptr->gpio_info.gpio,
                        i2c_ptr->gpio_info.scl_pin_mask | i2c_ptr->gpio_info.sda_pin_mask,
                        IO_PULL_UP);
    vsf_gpio_set_output(i2c_ptr->gpio_info.gpio,
                        i2c_ptr->gpio_info.scl_pin_mask | i2c_ptr->gpio_info.sda_pin_mask);
    i2c_ptr->callback_timer.on_timer = __i2c_gpio_callback;
    vsf_callback_timer_init(&i2c_ptr->callback_timer);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_i2c_init(vsf_i2c_t *i2c_ptr, i2c_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != i2c_ptr) && (NULL != cfg_ptr));
    if (cfg_ptr->clock_hz > VSF_I2C_GPIO_MAX_CLOCK_HZ) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    i2c_ptr->cfg = *cfg_ptr;
    return __vsf_i2c_init((vsf_i2c_t *)i2c_ptr, cfg_ptr);
}

void vsf_i2c_fini(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    if (i2c_ptr->status.status_bool.is_enabled) {
        VSF_HAL_ASSERT(false);
    }
    //todo:
}

fsm_rt_t vsf_i2c_enable(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    i2c_ptr->status.status_bool.is_enabled = true;
    return fsm_rt_cpl;
}

fsm_rt_t vsf_i2c_disable(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    i2c_ptr->status.status_bool.is_enabled = false;
    return fsm_rt_cpl;
}

void vsf_i2c_irq_enable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    i2c_ptr->enabled_irq_mask = irq_mask;
}

void vsf_i2c_irq_disable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    i2c_ptr->enabled_irq_mask &= ~irq_mask;
}

i2c_status_t vsf_i2c_status(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return i2c_ptr->status;
}

vsf_err_t __vsf_i2c_send_cmd(vsf_i2c_t *i2c_ptr, uint16_t data, em_i2c_cmd_t command)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    if (i2c_ptr->status.status_bool.is_enabled) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    if (i2c_ptr->status.use_as__peripheral_status_t.is_busy) {
        return VSF_ERR_FAIL;
    }
    i2c_ptr->status.use_as__peripheral_status_t.is_busy = true;
    //todo: Check for unsupported
    if (    (command & I2C_CMD_RW_MASK)
        &&  !(command & I2C_CMD_START)) {
        i2c_ptr->data = 0;
    } else {
        i2c_ptr->data = data;
    }
    i2c_ptr->cmd = command;
    i2c_ptr->i2c_gpio_cmd = I2C_GPIO_CMD_DATA;
    if (command & I2C_CMD_10_BITS) {
        i2c_ptr->bit_mask = 0x1ul << 15;
    } else {
        i2c_ptr->bit_mask = 0x1ul << 7;
    }
    i2c_gpio_callback(i2c_ptr);
    return VSF_ERR_NONE;
}


#endif /* VSF_HAL_USE_I2C_GPIO */