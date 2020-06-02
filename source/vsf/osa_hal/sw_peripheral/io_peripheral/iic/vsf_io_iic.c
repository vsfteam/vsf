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
#define __VSF_IO_IIC_CLASS_IMPLEMENT
#include "./vsf_io_iic.h"

#if VSF_USE_IO_IIC == ENABLED
/*! \note 
     You can ONLY include driver.h in *.c source code but NEVER the vsf_sw_xxx.h
 */
#include "hal/driver/driver.h"

/*============================ MACROS ========================================*/

#ifndef IIC_READ
#   define IIC_READ                     1
#endif

#ifndef IIC_WRITE
#   define IIC_WRITE                    0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_io_iic_scl_dominant(vsf_io_iic_t *io_iic)
{
    vsf_gpio_clear(io_iic->io.port, 1 << io_iic->io.scl_pin);
}

static void __vsf_io_iic_scl_recessive(vsf_io_iic_t *io_iic)
{
    vsf_gpio_set(io_iic->io.port, 1 << io_iic->io.scl_pin);
}

static bool __vsf_io_iic_scl_get(vsf_io_iic_t *io_iic)
{
    return vsf_gpio_read(io_iic->io.port) & (1 << io_iic->io.scl_pin);
}

static void __vsf_io_iic_sda_dominant(vsf_io_iic_t *io_iic)
{
    vsf_gpio_clear(io_iic->io.port, 1 << io_iic->io.sda_pin);
}

static void __vsf_io_iic_sda_recessive(vsf_io_iic_t *io_iic)
{
    vsf_gpio_set(io_iic->io.port, 1 << io_iic->io.sda_pin);
}

static bool __vsf_io_iic_sda_get(vsf_io_iic_t *io_iic)
{
    return vsf_gpio_read(io_iic->io.port) & (1 << io_iic->io.sda_pin);
}

static void __vsf_io_iic_on_timer(vsf_callback_timer_t *timer)
{
    vsf_io_iic_t *io_iic = container_of(timer, vsf_io_iic_t, cb_timer);
    bool state_inc = true;
    uint_fast32_t delay = 500 / io_iic->freq_khz;

    if (io_iic->is_tx || io_iic->is_rx) {
        bool is_data_phase = io_iic->state < 32;

        delay >>= 1;
        if (36 == io_iic->state) {
            if (io_iic->is_addr) {
                io_iic->is_addr = false;
                if (io_iic->is_rx) {
                    io_iic->is_tx = false;
                }
                goto data_start;
            } else {
                io_iic->size--;
                if (io_iic->is_rx) {
                    *io_iic->buffer = io_iic->cur_data;
                    __vsf_io_iic_sda_recessive(io_iic);
                }
                io_iic->buffer++;

            data_start:
                if (io_iic->is_tx) {
                    io_iic->cur_data = *io_iic->buffer;
                }

                if (io_iic->size > 0) {
                    if (io_iic->byte_interval_us) {
                        delay = io_iic->byte_interval_us;
                    }
                } else {
                data_end:
                    if (io_iic->flags & VSF_IO_IIC_NO_STOP) {
                        goto trans_end;
                    } else {
                        io_iic->is_tx = 0;
                        io_iic->is_rx = 0;
                    }
                }
                io_iic->state = -1;
            }
        } else {
            uint_fast8_t bit_state = io_iic->state & 0x03;
            switch (bit_state) {
            case 0:
                if (is_data_phase) {
                    if (io_iic->is_tx) {
                        if (io_iic->cur_data & 0x80) {
                            __vsf_io_iic_sda_recessive(io_iic);
                        } else {
                            __vsf_io_iic_sda_dominant(io_iic);
                        }
                        io_iic->cur_data <<= 1;
                    }
                } else if (!io_iic->is_addr && io_iic->is_rx && (io_iic->size > 1)) {
                    __vsf_io_iic_sda_dominant(io_iic);
                } else {
                    __vsf_io_iic_sda_recessive(io_iic);
                }
                break;
            case 1:
                __vsf_io_iic_scl_recessive(io_iic);
                break;
            case 2:
                if (!__vsf_io_iic_scl_get(io_iic)) {
                    state_inc = false;
                } else {
                    io_iic->sda_sample = __vsf_io_iic_sda_get(io_iic);
                }
                break;
            case 3:
                __vsf_io_iic_scl_dominant(io_iic);
                if (!io_iic->is_addr && is_data_phase && io_iic->is_rx) {
                    io_iic->cur_data <<= 1;
                    if (io_iic->sda_sample) {
                        io_iic->cur_data |= 1;
                    } else {
                        io_iic->cur_data &= ~1;
                    }
                } else if (!is_data_phase && io_iic->is_tx) {
                    if (io_iic->sda_sample) {
                        if (io_iic->is_addr) {
                            io_iic->flags &= ~VSF_IO_IIC_NO_STOP;
                            io_iic->size = -1;
                        }
                        goto data_end;
                    }
                }
                break;
            }
        }
    } else {
        if (!io_iic->is_started) {
            if (io_iic->flags & VSF_IO_IIC_NO_START) {
            do_trans:
                io_iic->is_started = true;
                if (io_iic->flags & VSF_IO_IIC_NO_ADDR) {
                    io_iic->is_rx = io_iic->cur_data & 1;
                    io_iic->is_tx = !io_iic->is_rx;
                    io_iic->cur_data = *io_iic->buffer;
                } else {
                    io_iic->is_addr = true;
                    io_iic->is_tx = true;
                    io_iic->is_rx = io_iic->cur_data & 1;
                }
                io_iic->state = -1;
            } else {
                switch (io_iic->state) {
                case 0:
                    __vsf_io_iic_sda_recessive(io_iic);
                    break;
                case 1:
                    __vsf_io_iic_scl_recessive(io_iic);
                    break;
                case 2:
                    if (!__vsf_io_iic_scl_get(io_iic)) {
                        state_inc = false;
                    } else {
                        // error recovery is not supported
                        VSF_IOP_ASSERT(__vsf_io_iic_sda_get(io_iic));
                        __vsf_io_iic_sda_dominant(io_iic);
                    }
                    break;
                case 3:
                    __vsf_io_iic_scl_dominant(io_iic);
                    goto do_trans;
                }
            }
        } else if (io_iic->flags & VSF_IO_IIC_NO_STOP) {
        trans_end: {
                int_fast32_t result = io_iic->size;
                io_iic->size = 0;
                if (io_iic->callback.handler != NULL) {
                    io_iic->callback.handler(io_iic, result);
                }
            }
            return;
        } else {
            switch (io_iic->state) {
            case 0:
                __vsf_io_iic_sda_dominant(io_iic);
                break;
            case 1:
                __vsf_io_iic_scl_recessive(io_iic);
                break;
            case 2:
                if (!__vsf_io_iic_scl_get(io_iic)) {
                    state_inc = false;
                } else {
                    __vsf_io_iic_sda_recessive(io_iic);
                }
                break;
            case 3:
                goto trans_end;
            }
        }
    }

    if (state_inc) {
        io_iic->state++;
    }
    vsf_callback_timer_add_us(&io_iic->cb_timer, delay);
}

vsf_err_t vsf_io_iic_init(vsf_io_iic_t *io_iic)
{
    ASSERT((io_iic != NULL) && (io_iic->io.port != NULL));
    vsf_gpio_t *gpio = io_iic->io.port;
    io_iic->cb_timer.on_timer = __vsf_io_iic_on_timer;

    vsf_gpio_config_pin(gpio, 1 << io_iic->io.scl_pin, IO_OUTPUT_OD | IO_INPUT_PU);
    vsf_gpio_config_pin(gpio, 1 << io_iic->io.sda_pin, IO_OUTPUT_OD | IO_INPUT_PU);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_io_iic_transact(vsf_io_iic_t *io_iic, uint_fast8_t addr, uint8_t *buffer, uint_fast32_t size, uint_fast8_t flags)
{
    // max byte size is 28-bit, 1-bit for state, 3-bit for bit size
    ASSERT((io_iic != NULL) && (io_iic->io.port != NULL) && !io_iic->size);

    io_iic->cur_data = addr;
    io_iic->size = size;
    io_iic->buffer = buffer;
    io_iic->flags = flags;
    io_iic->__state = 0;
    io_iic->state = 0;
    __vsf_io_iic_on_timer(&io_iic->cb_timer);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_io_iic_read(vsf_io_iic_t *io_iic, uint_fast8_t addr, uint8_t *buffer, uint_fast32_t size, uint_fast8_t flags)
{
    return vsf_io_iic_transact(io_iic, (addr << 1) | IIC_READ, buffer, size, flags);
}

vsf_err_t vsf_io_iic_write(vsf_io_iic_t *io_iic, uint_fast8_t addr, uint8_t *buffer, uint_fast32_t size, uint_fast8_t flags)
{
    return vsf_io_iic_transact(io_iic, (addr << 1) | IIC_WRITE, buffer, size, flags);
}

#endif
/* EOF */
