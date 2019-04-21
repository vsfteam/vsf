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

#include "../driver.h"
#include "./NuConsole.h"

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
#define VSFSTREAM_CLASS_INHERIT
#include "service/vsfstream/vsfstream.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
static void vsf_nu_console_stream_init(vsf_stream_t *stream)
{
    NuConsole_Init();
}

static uint_fast32_t vsf_nu_console_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    return NuConsole_Write(buf, size);
}

static uint_fast32_t vsf_nu_console_stream_get_data_length(vsf_stream_t *stream)
{
    return 0;
}

const vsf_stream_op_t vsf_nu_console_stream_op = {
    .init               = vsf_nu_console_stream_init,
    .get_data_length    = vsf_nu_console_stream_get_data_length,
    .write              = vsf_nu_console_stream_write,
};
#endif