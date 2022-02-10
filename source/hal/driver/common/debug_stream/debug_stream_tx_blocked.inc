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

#include "vsf_cfg.h"

#if VSF_HAL_USE_DEBUG_STREAM == ENABLED

#if     VSF_USE_SIMPLE_STREAM == ENABLED
#   define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#   include "service/vsf_service.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
static void __vsf_debug_stream_tx_init(vsf_stream_t *stream);
static uint_fast32_t __vsf_debug_stream_tx_write(vsf_stream_t* stream, uint8_t* buf, uint_fast32_t size);
static uint_fast32_t __vsf_debug_stream_tx_stream_get_data_length(vsf_stream_t* stream);
static uint_fast32_t __vsf_debug_stream_tx_stream_get_avail_length(vsf_stream_t* stream);
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
static const vsf_stream_op_t __vsf_debug_stream_tx_op = {
    .init               = __vsf_debug_stream_tx_init,
    .write              = __vsf_debug_stream_tx_write,
    .get_data_length    = __vsf_debug_stream_tx_stream_get_data_length,
    .get_avail_length   = __vsf_debug_stream_tx_stream_get_avail_length,
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
vsf_stream_t VSF_DEBUG_STREAM_TX = {
    .op         = &__vsf_debug_stream_tx_op,
};
#elif   VSF_USE_STREAM == ENABLED
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_SIMPLE_STREAM == ENABLED
static void __vsf_debug_stream_tx_init(vsf_stream_t *stream)
{
    __VSF_DEBUG_STREAM_TX_INIT();
    vsf_stream_connect_rx(stream);
}

static uint_fast32_t __vsf_debug_stream_tx_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    __VSF_DEBUG_STREAM_TX_WRITE_BLOCKED(buf, size);
    return size;
}

static uint_fast32_t __vsf_debug_stream_tx_stream_get_data_length(vsf_stream_t* stream)
{
    return 0;
}

static uint_fast32_t __vsf_debug_stream_tx_stream_get_avail_length(vsf_stream_t* stream)
{
    return 0xFFFFFFFF;
}

#elif   VSF_USE_STREAM == ENABLED
#endif

#endif      // VSF_HAL_USE_DEBUG_STREAM
