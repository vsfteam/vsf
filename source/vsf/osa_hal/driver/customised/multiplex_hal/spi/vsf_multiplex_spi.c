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
#define __VSF_MULTIPLEX_SPI_CLASS_IMPLEMENT
#define __VSF_MULTIPLEX_HAL_CLASS_INHERIT
#include "./vsf_multiplex_spi.h"

#if VSF_USE_MULTIPLEX_HAL == ENABLED && VSF_USE_MULTIPLEX_SPI == ENABLED

#include "component/vsf_component.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_multiplex_spi_init(vsf_multiplex_spi_t *spi)
{
    vsf_eda_mutex_init(&spi->mutex);
    return vsf_io_spi_init(&spi->use_as__vsf_io_spi_t);
}

static void __vsf_multiplex_spi_transact_handler(vsf_io_spi_t *io_spi)
{
    vsf_multiplex_spi_t *spi = (vsf_multiplex_spi_t *)io_spi;
    vsf_eda_post_evt(spi->notifier_eda, VSF_EVT_RETURN);
}

__vsf_component_peda_private_entry(__vsf_multiplex_spi_transact,
    uint8_t *tx;
    uint8_t *rx;
    uint32_t size;
) {
    vsf_peda_begin();
    vsf_multiplex_spi_t *spi = (vsf_multiplex_spi_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != vsf_eda_mutex_enter(&spi->mutex)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        spi->callback.handler = __vsf_multiplex_spi_transact_handler;
        spi->notifier_eda = vsf_eda_get_cur();
        vsf_io_spi_transact(&spi->use_as__vsf_io_spi_t, vsf_local.tx, vsf_local.rx, vsf_local.size);
        break;
    case VSF_EVT_RETURN:
        vsf_eda_mutex_leave(&spi->mutex);
        vsf_eda_return();
        break;
    }
    vsf_peda_end();
}

vsf_err_t vsf_multiplex_spi_transact(vsf_multiplex_spi_t *spi, uint8_t *tx, uint8_t *rx, uint_fast32_t size)
{
    vsf_err_t err;
    VSF_OSA_HAL_ASSERT(spi != NULL);
    __vsf_component_call_peda(__vsf_multiplex_spi_transact, err, spi,
        .tx     = tx,
        .rx     = rx,
        .size   = size,
    )
    return err;
}

#endif
/* EOF */
