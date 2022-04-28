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

#ifndef __HAL_DRIVER_COMMON_MULTIPLEX_SPI_H__
#define __HAL_DRIVER_COMMON_MULTIPLEX_SPI_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SPI == ENABLED

#if     defined(__VSF_MULTIPLEX_SPI_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#   undef __VSF_MULTIPLEX_SPI_CLASS_IMPLEMENT
#elif   defined(__VSF_MULTIPLEX_SPI_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#   undef __VSF_MULTIPLEX_SPI_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

// undef after include vsf_template_spi.h
#define VSF_SPI_CFG_API_DECLARATION_PREFIX              vsf_multiplex
#include "hal/driver/common/template/vsf_template_spi.h"

/*============================ MACROS ========================================*/

#ifndef VSF_MULTIPLEXER_SPI_CFG_MASK_TYPE
#   define VSF_MULTIPLEXER_SPI_CFG_MASK_TYPE        uint8_t
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef VSF_MULTIPLEXER_SPI_CFG_MASK_TYPE spi_multi_mask_t;

typedef struct multiplex_spi_request_t {
    void *out_buffer_ptr;
    void *in_buffer_ptr;
    uint_fast32_t count;
} multiplex_spi_request_t;

vsf_declare_class(vsf_multiplex_spi_t)

vsf_class(vsf_multiplex_spi_info_t) {
    public_member(
        vsf_spi_t *spi;
    )

    private_member(
        vsf_slist_queue_t list;

        // inited configraction
        vsf_multiplex_spi_t *cfg_spi_ptr;
        spi_multi_mask_t     init_mask;

        // enable/disable mask
        spi_multi_mask_t     en_mask;

        // cs mask
        spi_multi_mask_t     cs_mask;

        // irq mask
        em_spi_irq_mask_t irq_mask;     // All CS IRQ Mask Wire-OR
    )
};

vsf_class(vsf_multiplex_spi_t) {
    public_member(
        vsf_multiplex_spi_info_t *spi_info_ptr;
        int8_t cs_index;
        vsf_gpio_t *gpio;
        uint32_t pin_mask;
    )

    private_member(
        vsf_slist_node_t node;
        spi_cfg_t spi_cfg;              // init and re-init
        em_spi_irq_mask_t irq_mask;     // enable/disable interrupt
        uint_fast32_t transfered_count;
        multiplex_spi_request_t request;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif  /*VSF_HAL_USE_SPI*/

#endif  /*__HAL_DRIVER_COMMON_MULTIPLEX_SPI_H__*/
