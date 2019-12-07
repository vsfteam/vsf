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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_CFG_SUPPORT_DEVICE_DEBUGGER_SERIAL_PORT == ENABLED

#include <stdio.h>

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
#define VSFSTREAM_CLASS_INHERIT
#include "service/vsf_service.h"
#endif
#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
static void vsf_debug_stream_init(vsf_stream_t* stream);
static uint_fast32_t vsf_debug_stream_write(vsf_stream_t* stream,
            uint8_t* buf, uint_fast32_t size);
static uint_fast32_t vsf_debug_stream_get_data_length(vsf_stream_t* stream);
#elif   VSF_USE_SERVICE_STREAM == ENABLED

#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
const vsf_stream_op_t vsf_debug_stream_op = {
    .init = vsf_debug_stream_init,
    .get_data_length = vsf_debug_stream_get_data_length,
    .write = vsf_debug_stream_write,
};

vsf_stream_t  VSF_DEBUG_STREAM_TX = {
        .op = &vsf_debug_stream_op,
};

#elif   VSF_USE_SERVICE_STREAM == ENABLED

#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
static void vsf_debug_stream_init(vsf_stream_t* stream)
{

}

static uint_fast32_t vsf_debug_stream_write(vsf_stream_t* stream,
            uint8_t* buf, uint_fast32_t size)
{
    return fwrite(buf, 1, size, stdout);
}

static uint_fast32_t vsf_debug_stream_get_data_length(vsf_stream_t* stream)
{
    return 0;
}
#elif   VSF_USE_SERVICE_STREAM == ENABLED
#endif

#endif

/* EOF */
