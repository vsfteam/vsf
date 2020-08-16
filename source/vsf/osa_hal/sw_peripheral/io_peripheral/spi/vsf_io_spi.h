/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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
#include "../io_periperhal_cfg.h"

#ifndef __OSA_HAL_DRIVER_IO_SPI_H__
#define __OSA_HAL_DRIVER_IO_SPI_H__

#if VSF_USE_IO_SPI == ENABLED

#include "hal/vsf_hal.h"
#include "kernel/vsf_kernel.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_IO_SPI_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__

#elif   defined(__VSF_IO_SPI_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_io_spi_t)

def_simple_class(vsf_io_spi_t) {
    public_member(
        struct {
            vsf_gpio_t *port;
            uint8_t sck_pin;
            int8_t miso_pin;    // -1 for unused
            int8_t mosi_pin;    // -1 for unused
            int8_t ss_pin;      // -1 for unused
        } io;
        struct {
            void (*handler)(vsf_io_spi_t *io_spi);
        } callback;
        uint16_t freq_khz;
        union {
            struct {
                uint8_t chpa    : 1;
                uint8_t chol    : 1;
                uint8_t msbf    : 1;
            };
            uint8_t mode;
        };
    )
    private_member(
        uint8_t *tx;
        uint8_t *rx;
        uint32_t size;
        uint32_t cur_pos;
        vsf_callback_timer_t cb_timer;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_io_spi_init(vsf_io_spi_t *io_spi);
extern vsf_err_t vsf_io_spi_transact(vsf_io_spi_t *io_spi, uint8_t *tx, uint8_t *rx, uint_fast32_t size);

#undef __VSF_IO_SPI_CLASS_IMPLEMENT
#undef __VSF_IO_SPI_CLASS_INHERIT

#ifdef __cplusplus
}
#endif

#endif
#endif

/* EOF */
