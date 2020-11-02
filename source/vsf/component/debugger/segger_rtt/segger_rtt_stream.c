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

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#include "../vsf_debugger.h"

#if VSF_DEBUGGER_CFG_CONSOLE == VSF_DEBUGGER_CFG_CONSOLE_SEGGER_RTT

#include "RTT/SEGGER_RTT.h"

#include "utilities/vsf_utilities.h"

/*============================ MACROS ========================================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
#   ifndef VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE
#       define VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE         32
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
#if     VSF_USE_SIMPLE_STREAM == ENABLED
static void __vsf_segger_rtt_stream_init(vsf_stream_t *stream);
static uint_fast32_t __vsf_segger_rtt_stream_write(   vsf_stream_t *stream, 
                                                    uint8_t *buf, 
                                                    uint_fast32_t size);
static uint_fast32_t __vsf_segger_rtt_stream_get_data_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_segger_rtt_stream_get_avail_length(vsf_stream_t *stream);
#elif   VSF_USE_STREAM == ENABLED
static vsf_err_t __vsf_segger_rtt_stream_tx_send_pbuf(vsf_stream_tx_t *obj_ptr, 
                                                    vsf_pbuf_t *pblock);
static 
vsf_stream_status_t __vsf_segger_rtt_stream_tx_get_status(vsf_stream_tx_t *obj_ptr);

static vsf_err_t __vsf_segger_rtt_stream_tx_dat_drn_evt_reg(  
                                            vsf_stream_tx_t *obj_ptr, 
                                            vsf_stream_dat_drn_evt_t event);
#endif

/*============================ LOCAL VARIABLES ===============================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
static uint8_t __vsf_debug_stream_rx_buff[VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE];
static const vsf_stream_op_t __vsf_segger_rtt_stream_tx_op = {
    .init               = __vsf_segger_rtt_stream_init,
    .get_data_length    = __vsf_segger_rtt_stream_get_data_length,
    .get_avail_length   = __vsf_segger_rtt_stream_get_avail_length,
    .write              = __vsf_segger_rtt_stream_write,
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
vsf_stream_t VSF_DEBUG_STREAM_TX = {
    .op = &__vsf_segger_rtt_stream_tx_op,
};

vsf_mem_stream_t VSF_DEBUG_STREAM_RX = {
    .op         = &vsf_mem_stream_op,
    .buffer     = __vsf_debug_stream_rx_buff,
    .size       = sizeof(__vsf_debug_stream_rx_buff),
};

#elif   VSF_USE_STREAM == ENABLED
static const i_stream_pbuf_tx_t __segger_rtt_stream_tx = {
    .Send =         &__vsf_segger_rtt_stream_tx_send_pbuf,
    .GetStatus =    &__vsf_segger_rtt_stream_tx_get_status,
    .DataDrainEvent = {
        .Register = &__vsf_segger_rtt_stream_tx_dat_drn_evt_reg,
    },
};

const vsf_stream_tx_t VSF_DEBUG_STREAM_TX = {
    .piMethod = &__segger_rtt_stream_tx,
};
#endif
/*============================ IMPLEMENTATION ================================*/

static void __vsf_segger_rtt_stream_init_imp(void)
{
    static bool __vsf_debug_stream_is_inited = false;
    if (!__vsf_debug_stream_is_inited) {
        __vsf_debug_stream_is_inited = true;

        SEGGER_RTT_ConfigUpBuffer(0, "debug_stream", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
    }
}

WEAK(vsf_stdout_init) 
void vsf_stdout_init(void)
{
    __vsf_segger_rtt_stream_init_imp();
}

WEAK(vsf_stderr_init) 
void vsf_stderr_init(void)
{
    __vsf_segger_rtt_stream_init_imp();
}

WEAK(vsf_stdin_init) 
void vsf_stdin_init(void)
{
    __vsf_segger_rtt_stream_init_imp();
}

WEAK(vsf_stdout_putchar)
int vsf_stdout_putchar(char ch)
{
    return SEGGER_RTT_Write(0, (const void *)&ch, 1);
}

WEAK(vsf_stderr_putchar) 
int vsf_stderr_putchar(char ch)
{
    return SEGGER_RTT_Write(0, (const void *)&ch, 1);
}

WEAK(vsf_stdin_getchar) 
int vsf_stdin_getchar(void)
{
    int ch = 0;
    SEGGER_RTT_Read(0, (void *)&ch, 1);
    return ch;
}

#if VSF_USE_SIMPLE_STREAM == ENABLED
void VSF_DEBUG_STREAM_POLL(void)
{
    uint_fast32_t size = SEGGER_RTT_HasData(0);
    uint8_t ch;

    while (size-- > 0) {
        SEGGER_RTT_Read(0, (void *)&ch, 1);
        VSF_STREAM_WRITE(&VSF_DEBUG_STREAM_RX, &ch, 1);
    }
}

static void __vsf_segger_rtt_stream_init(vsf_stream_t *stream)
{
    __vsf_segger_rtt_stream_init_imp();
}

static uint_fast32_t __vsf_segger_rtt_stream_write(vsf_stream_t *stream, 
                                                    uint8_t *buf, 
                                                    uint_fast32_t size)
{
    __vsf_segger_rtt_stream_init_imp();
    return SEGGER_RTT_Write(0, (const void *)buf, size);
}

static uint_fast32_t __vsf_segger_rtt_stream_get_data_length(vsf_stream_t *stream)
{
    return 0;
}

uint_fast32_t __vsf_segger_rtt_stream_get_avail_length(vsf_stream_t *stream)
{
    return -1;
}

#elif   VSF_USE_STREAM == ENABLED

static vsf_err_t __vsf_segger_rtt_stream_tx_send_pbuf(vsf_stream_tx_t *obj_ptr, 
                                                    vsf_pbuf_t *block_ptr)
{
    vsf_err_t result = VSF_ERR_NONE;
    do {
        if (NULL == block_ptr) {
            result = VSF_ERR_INVALID_PTR;
            break;
        }
    #if VSF_PBUF_CFG_INDIRECT_RW_SUPPORT == DISABLED
        if (vsf_pbuf_capability_get(block_ptr).is_no_direct_access) {
            //! no direct access: todo add support
            result = VSF_ERR_NOT_ACCESSABLE;
            break;
        } else 
    #endif
        {
            SEGGER_RTT_Write(0, (const void *)vsf_pbuf_buffer_get(block_ptr), 
                            vsf_pbuf_size_get(block_ptr));
        }
        
    } while(0);

    vsf_pbuf_free(block_ptr);

    return result;
}

static 
vsf_stream_status_t __vsf_segger_rtt_stream_tx_get_status(vsf_stream_tx_t *obj_ptr)
{
    return (vsf_stream_status_t){0};//s_tNuStream.Status;
}

static vsf_err_t __vsf_segger_rtt_stream_tx_dat_drn_evt_reg(  
                                            vsf_stream_tx_t *obj_ptr, 
                                            vsf_stream_dat_drn_evt_t event)
{
    //s_tNuStream.event = event;
    return VSF_ERR_NONE;
}
#endif
#endif
