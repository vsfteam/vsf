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

#if VSF_HAL_USE_DEBUG_STREAM == ENABLED
#   if VSF_USE_SIMPLE_STREAM == ENABLED
#       define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#       include "service/vsf_service.h"
#   endif
#endif
#include "hal/arch/vsf_arch.h"
#include <Windows.h>

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_DEBUG_STREAM == ENABLED
#   ifndef VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE
#       define VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE         1024
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_HAL_USE_DEBUG_STREAM == ENABLED
#   if VSF_USE_SIMPLE_STREAM == ENABLED
static uint_fast32_t __vsf_x86_debug_stream_tx_write(vsf_stream_t* stream,
            uint8_t* buf, uint_fast32_t size);
static uint_fast32_t __vsf_x86_debug_stream_tx_get_data_length(vsf_stream_t* stream);
static uint_fast32_t __vsf_x86_debug_stream_tx_get_avail_length(vsf_stream_t* stream);
#   elif   VSF_USE_STREAM == ENABLED

#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_HAL_USE_DEBUG_STREAM == ENABLED
#   if VSF_USE_SIMPLE_STREAM == ENABLED
static const vsf_stream_op_t __vsf_x86_debug_stream_tx_op = {
    .get_data_length = __vsf_x86_debug_stream_tx_get_data_length,
    .get_avail_length = __vsf_x86_debug_stream_tx_get_avail_length,
    .write = __vsf_x86_debug_stream_tx_write,
};
static uint8_t __vsf_x86_debug_stream_rx_buff[VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE];
static vsf_arch_irq_thread_t __vsf_x86_debug_stream_rx_irq;
#   endif

static HANDLE hOut, hIn;
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_HAL_USE_DEBUG_STREAM == ENABLED
#   if VSF_USE_SIMPLE_STREAM == ENABLED
vsf_stream_t VSF_DEBUG_STREAM_TX = {
    .op         = &__vsf_x86_debug_stream_tx_op,
};

vsf_mem_stream_t VSF_DEBUG_STREAM_RX = {
    .op         = &vsf_mem_stream_op,
    .buffer     = __vsf_x86_debug_stream_rx_buff,
    .size       = sizeof(__vsf_x86_debug_stream_rx_buff),
};

#   elif   VSF_USE_STREAM == ENABLED

#   endif
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_HAL_USE_DEBUG_STREAM == ENABLED
#   if VSF_USE_SIMPLE_STREAM == ENABLED
static uint_fast32_t __vsf_x86_debug_stream_tx_write(vsf_stream_t* stream,
            uint8_t* buf, uint_fast32_t size)
{
    DWORD wsize;
    WriteFile(hOut, buf, size, &wsize, NULL);
    return wsize;
}

static uint_fast32_t __vsf_x86_debug_stream_tx_get_data_length(vsf_stream_t* stream)
{
    return 0;
}

static uint_fast32_t __vsf_x86_debug_stream_tx_get_avail_length(vsf_stream_t* stream)
{
    return 0xFFFFFFFF;
}

static void __vsf_x86_debug_stream_rx_irqhandler(void *arg)
{
    vsf_arch_irq_thread_t *thread = arg;
    DWORD rsize;
    char ch;

    __vsf_arch_irq_set_background(thread);
    while (1) {
        do {
            ReadFile(hIn, &ch, 1, &rsize, NULL);
        } while (!rsize);

        __vsf_arch_irq_start(thread);
            VSF_STREAM_WRITE(&VSF_DEBUG_STREAM_RX, (uint8_t *)&ch, 1);
        __vsf_arch_irq_end(thread, false);
    }
}

static void __vsf_x86_debug_stream_init(void)
{
    DWORD mode;

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hIn = GetStdHandle(STD_INPUT_HANDLE);

    GetConsoleMode(hIn, &mode);
    mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
    SetConsoleMode(hIn, mode);

    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);

    VSF_STREAM_CONNECT_TX(&VSF_DEBUG_STREAM_RX);
    __vsf_arch_irq_init(&__vsf_x86_debug_stream_rx_irq, "debug_stream_rx",
        __vsf_x86_debug_stream_rx_irqhandler, vsf_arch_prio_0);
}
#   elif   VSF_USE_STREAM == ENABLED
#   endif
#endif

bool vsf_driver_init(void)
{
#if VSF_HAL_USE_DEBUG_STREAM == ENABLED
    __vsf_x86_debug_stream_init();
#endif
    return true;
}

/* EOF */
