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
#define VSF_I2C_CFG_IMP_PREFIX              vsf_gpio
#define VSF_I2C_CFG_IMP_UPCASE_PREFIX       VSF_GPIO
#define VSF_I2C_CFG_IMP_EXTERN_OP           ENABLED

#define __VSF_HAL_GPIO_I2C_CLASS_IMPLEMENT  ENABLED

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO_I2C == ENABLED

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_gpio_i2c_delay(vsf_gpio_i2c_t *gpio_i2c_ptr)
{
}

vsf_err_t vsf_gpio_i2c_init(vsf_gpio_i2c_t *gpio_i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    vsf_gpio_config_pin(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->scl_pin, VSF_IO_OPEN_DRAIN | VSF_IO_PULL_UP);
    vsf_gpio_config_pin(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin, VSF_IO_OPEN_DRAIN | VSF_IO_PULL_UP);
    vsf_gpio_set_input(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->scl_pin);
    vsf_gpio_set_input(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);

    gpio_i2c_ptr->cfg = *cfg_ptr;
    if (NULL == gpio_i2c_ptr->fn_delay) {
        gpio_i2c_ptr->fn_delay = __vsf_gpio_i2c_delay;
    }
    return VSF_ERR_NONE;
}

void vsf_gpio_i2c_fini(vsf_gpio_i2c_t *gpio_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);
}

fsm_rt_t vsf_gpio_i2c_enable(vsf_gpio_i2c_t *gpio_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);
    gpio_i2c_ptr->is_enabled = true;
    return fsm_rt_cpl;
}

fsm_rt_t vsf_gpio_i2c_disable(vsf_gpio_i2c_t *gpio_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);
    gpio_i2c_ptr->is_enabled = false;
    return fsm_rt_cpl;
}

void vsf_gpio_i2c_irq_enable(vsf_gpio_i2c_t *gpio_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);
    gpio_i2c_ptr->enabled_irq_mask = irq_mask;
}

void vsf_gpio_i2c_irq_disable(vsf_gpio_i2c_t *gpio_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);
    gpio_i2c_ptr->enabled_irq_mask &= ~irq_mask;
}

vsf_i2c_status_t vsf_gpio_i2c_status(vsf_gpio_i2c_t *gpio_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != gpio_i2c_ptr);

    vsf_i2c_status_t status = {
        .use_as__peripheral_status_t.is_busy = gpio_i2c_ptr->is_busy,
    };

    return status;
}

static void __vsf_gpio_i2c_scl_dominant(vsf_gpio_i2c_t *gpio_i2c_ptr)
{
    vsf_gpio_set(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->scl_pin);
    vsf_gpio_set_output(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->scl_pin);
}

static void __vsf_gpio_i2c_scl_recessive(vsf_gpio_i2c_t *gpio_i2c_ptr)
{
    vsf_gpio_set_input(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->scl_pin);

    uint32_t scl_sample;
    do {
        scl_sample = vsf_gpio_read(gpio_i2c_ptr->port) & (1 << gpio_i2c_ptr->scl_pin);
    } while (!scl_sample);
}

static bool __vsf_gpio_i2c_out(vsf_gpio_i2c_t *gpio_i2c_ptr, uint32_t data, uint8_t bits)
{
    vsf_gpio_set_output(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
    for (; bits > 0; bits--) {
        __vsf_gpio_i2c_scl_dominant(gpio_i2c_ptr);
        if (data & (1 << (bits - 1))) {
            vsf_gpio_set(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
        } else {
            vsf_gpio_clear(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
        }
        gpio_i2c_ptr->fn_delay(gpio_i2c_ptr);
        __vsf_gpio_i2c_scl_recessive(gpio_i2c_ptr);
        gpio_i2c_ptr->fn_delay(gpio_i2c_ptr);
    }

    vsf_gpio_set_input(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
    __vsf_gpio_i2c_scl_dominant(gpio_i2c_ptr);
    gpio_i2c_ptr->fn_delay(gpio_i2c_ptr);
    __vsf_gpio_i2c_scl_recessive(gpio_i2c_ptr);
    gpio_i2c_ptr->fn_delay(gpio_i2c_ptr);
    return !(vsf_gpio_read(gpio_i2c_ptr->port) & (1 << gpio_i2c_ptr->sda_pin));
}

static uint8_t __vsf_gpio_i2c_in(vsf_gpio_i2c_t *gpio_i2c_ptr, bool ack)
{
    uint8_t bits = 8, data = 0;

    vsf_gpio_set_input(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
    for (; bits > 0; bits--) {
        __vsf_gpio_i2c_scl_dominant(gpio_i2c_ptr);
        gpio_i2c_ptr->fn_delay(gpio_i2c_ptr);
        __vsf_gpio_i2c_scl_recessive(gpio_i2c_ptr);
        if (vsf_gpio_read(gpio_i2c_ptr->port) & (1 << gpio_i2c_ptr->sda_pin)) {
            data |= 1 << (bits - 1);
        }
        gpio_i2c_ptr->fn_delay(gpio_i2c_ptr);
    }

    __vsf_gpio_i2c_scl_dominant(gpio_i2c_ptr);
    if (ack) {
        vsf_gpio_clear(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
    } else {
        vsf_gpio_set(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
    }
    vsf_gpio_set_output(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
    gpio_i2c_ptr->fn_delay(gpio_i2c_ptr);
    __vsf_gpio_i2c_scl_recessive(gpio_i2c_ptr);
    gpio_i2c_ptr->fn_delay(gpio_i2c_ptr);
    return data;
}

static void __vsf_gpio_i2c_isrhandler(vsf_gpio_i2c_t *gpio_i2c_ptr)
{
    if (gpio_i2c_ptr->cfg.isr.handler_fn != NULL) {
        gpio_i2c_ptr->cfg.isr.handler_fn(gpio_i2c_ptr->cfg.isr.target_ptr,
                (vsf_i2c_t *)gpio_i2c_ptr, gpio_i2c_ptr->irq_mask);
    }
}

vsf_err_t vsf_gpio_i2c_master_request(vsf_gpio_i2c_t *gpio_i2c_ptr,
                                    uint16_t address,
                                    vsf_i2c_cmd_t cmd,
                                    uint16_t count,
                                    uint8_t *buffer)
{
    bool is_read = (cmd & VSF_I2C_CMD_RW_MASK) == VSF_I2C_CMD_READ;
    uint16_t transfered_count = 0;
    if ((cmd & VSF_I2C_CMD_START) || (cmd & VSF_I2C_CMD_RESTART)) {
        vsf_gpio_clear(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
        gpio_i2c_ptr->fn_delay(gpio_i2c_ptr);

        uint32_t rw_mask = is_read ? 1 : 0;
        uint8_t bits = ((cmd & VSF_I2C_CMD_BITS_MASK) == VSF_I2C_CMD_7_BITS) ? 8 : 11;
        bool acked = __vsf_gpio_i2c_out(gpio_i2c_ptr, address | rw_mask, bits);
        gpio_i2c_ptr->irq_mask = VSF_I2C_IRQ_MASK_MASTER_STARTED;
        if (!acked) {
            gpio_i2c_ptr->irq_mask |= VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK;
            __vsf_gpio_i2c_isrhandler(gpio_i2c_ptr);
            return VSF_ERR_NONE;
        }
    }

    if (is_read) {
        for (; transfered_count < count; transfered_count++) {
            *buffer++ = __vsf_gpio_i2c_in(gpio_i2c_ptr, transfered_count < (count - 1));
        }
    } else {
        for (bool ack = true; ack && (transfered_count < count); transfered_count++) {
            ack = __vsf_gpio_i2c_out(gpio_i2c_ptr, *buffer++, 8);
        }
    }

    if (cmd & VSF_I2C_CMD_STOP) {
        __vsf_gpio_i2c_scl_dominant(gpio_i2c_ptr);
        vsf_gpio_clear(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
        vsf_gpio_set_output(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
        gpio_i2c_ptr->fn_delay(gpio_i2c_ptr);
        __vsf_gpio_i2c_scl_recessive(gpio_i2c_ptr);
        vsf_gpio_set_input(gpio_i2c_ptr->port, 1 << gpio_i2c_ptr->sda_pin);
    }
    gpio_i2c_ptr->transfered_count = transfered_count;
    return VSF_ERR_NONE;
}

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#include "hal/driver/common/i2c/i2c_template.inc"

#endif

