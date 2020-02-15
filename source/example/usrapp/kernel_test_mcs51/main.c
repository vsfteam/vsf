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

#include "vsf.h"
#include <stdio.h>
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void usrapp_sem_test_start(void);
extern void usrapp_mutex_test_start(void);
extern void usrapp_timer_test_start(void);

#if VSF_USE_TRACE == ENABLED
static uint_fast32_t vsf_stdio_console_stream_write(
        vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t vsf_stdio_console_stream_get_data_length(vsf_stream_t *stream);
#endif

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_USE_TRACE == ENABLED
static const vsf_stream_op_t vsf_stdio_console_stream_op = {
    .get_data_length    = vsf_stdio_console_stream_get_data_length,
    .write              = vsf_stdio_console_stream_write,
};

vsf_stream_t VSF_DEBUG_STREAM_TX = {
    .op = &vsf_stdio_console_stream_op,
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_TRACE == ENABLED
static uint_fast32_t vsf_stdio_console_stream_write(
        vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    return printf("%s", buf);
}

static uint_fast32_t vsf_stdio_console_stream_get_data_length(vsf_stream_t *stream)
{
    return 0;
}
#endif

int main(void)
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_init((vsf_stream_t *)&VSF_DEBUG_STREAM_TX);
#endif

    usrapp_sem_test_start();
//    usrapp_mutex_test_start();
//    usrapp_timer_test_start();
    return 0;
}

/* EOF */
