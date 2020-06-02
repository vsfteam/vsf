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
#define __VSF_IO_SPI_CLASS_IMPLEMENT
#include "./vsf_io_spi.h"

#if VSF_USE_IO_SPI == ENABLED
/*! \note 
     You can ONLY include driver.h in *.c source code but NEVER the vsf_sw_xxx.h
 */
#include "hal/driver/driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_io_spi_on_timer(vsf_callback_timer_t *timer)
{
    vsf_io_spi_t *io_spi = container_of(timer, vsf_io_spi_t, cb_timer);
    vsf_gpio_t *gpio = io_spi->io.port;
    uint_fast8_t mask = (io_spi->cur_pos >> 1) & 0x07;
    uint8_t *tx = io_spi->tx ? &io_spi->tx[io_spi->cur_pos >> (1 + 3)] : NULL;
    uint8_t *rx = io_spi->rx ? &io_spi->rx[io_spi->cur_pos >> (1 + 3)] : NULL;
    bool is_sample_point = io_spi->cur_pos++ & 1;

    if (    (io_spi->chpa && (io_spi->cur_pos < io_spi->size))
        ||  (!io_spi->chpa && (io_spi->cur_pos > 1))) {
        vsf_gpio_toggle(gpio, 1 << io_spi->io.sck_pin);
    }

    mask = 1 << (io_spi->msbf ? 7 - mask : mask);
    if (rx && is_sample_point && (io_spi->io.miso_pin >= 0)) {
        if (vsf_gpio_read(gpio) & (1 << io_spi->io.miso_pin)) {
            *rx |= mask;
        } else {
            *rx &= ~mask;
        }
    }
    if (tx && !is_sample_point && (io_spi->io.mosi_pin >= 0)) {
        if (*tx & mask) {
            vsf_gpio_set(gpio, 1 << io_spi->io.mosi_pin);
        } else {
            vsf_gpio_clear(gpio, 1 << io_spi->io.mosi_pin);
        }
    }

    if (io_spi->cur_pos >= io_spi->size) {
        io_spi->size = 0;
        if (io_spi->io.ss_pin >= 0) {
            vsf_gpio_set(gpio, 1 << io_spi->io.ss_pin);
        }
        if (io_spi->callback.handler != NULL) {
            io_spi->callback.handler(io_spi);
        }
    } else {
        vsf_callback_timer_add_us(&io_spi->cb_timer, 500 / io_spi->freq_khz);
    }
}

vsf_err_t vsf_io_spi_init(vsf_io_spi_t *io_spi)
{
    VSF_HAL_ASSERT((io_spi != NULL) && (io_spi->io.port != NULL));
    vsf_gpio_t *gpio = io_spi->io.port;
    io_spi->cb_timer.on_timer = __vsf_io_spi_on_timer;

    vsf_gpio_config_pin(gpio, 1 << io_spi->io.sck_pin, IO_OUTPUT_PP);
    if (io_spi->chol) {
        vsf_gpio_set(gpio, 1 << io_spi->io.sck_pin);
    } else {
        vsf_gpio_clear(gpio, 1 << io_spi->io.sck_pin);
    }

    if (io_spi->io.miso_pin != io_spi->io.mosi_pin) {
        if (io_spi->io.miso_pin >= 0) {
            vsf_gpio_config_pin(gpio, 1 << io_spi->io.miso_pin, IO_INPUT_FLOAT);
        }
        if (io_spi->io.mosi_pin >= 0) {
            vsf_gpio_config_pin(gpio, 1 << io_spi->io.mosi_pin, IO_OUTPUT_PP);
        }
    }
    if (io_spi->io.ss_pin >= 0) {
        vsf_gpio_config_pin(gpio, 1 << io_spi->io.ss_pin, IO_OUTPUT_PP);
        vsf_gpio_set(gpio, 1 << io_spi->io.ss_pin);
    }
    return VSF_ERR_NONE;
}

vsf_err_t vsf_io_spi_transact(vsf_io_spi_t *io_spi, uint8_t *tx, uint8_t *rx, uint_fast32_t size)
{
    // max byte size is 28-bit, 1-bit for state, 3-bit for bit size
    VSF_HAL_ASSERT((io_spi != NULL) && (io_spi->io.port != NULL) && (io_spi->freq_khz <= 500) && !(size >> 28) && !io_spi->size);

    if (io_spi->io.miso_pin == io_spi->io.mosi_pin) {
        vsf_gpio_t *gpio = io_spi->io.port;
        if ((tx != NULL) && (rx != NULL)) {
            VSF_HAL_ASSERT(false);
        } else if (tx != NULL) {
            vsf_gpio_config_pin(gpio, 1 << io_spi->io.miso_pin, IO_OUTPUT_PP);
        } else if (rx != NULL) {
            vsf_gpio_config_pin(gpio, 1 << io_spi->io.miso_pin, IO_INPUT_FLOAT);
        } else {
            VSF_HAL_ASSERT(false);
        }
    }

    io_spi->tx = tx;
    io_spi->rx = rx;
    io_spi->size = (size << 4) + 1;
    io_spi->cur_pos = 0;

    if (io_spi->io.ss_pin >= 0) {
        vsf_gpio_t *gpio = io_spi->io.port;
        vsf_gpio_clear(gpio, 1 << io_spi->io.ss_pin);
    }
    __vsf_io_spi_on_timer(&io_spi->cb_timer);
    return VSF_ERR_NONE;
}

#endif
/* EOF */
