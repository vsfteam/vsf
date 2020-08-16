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

#include "../driver.h"
#include "./NuConsole.h"

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
#define VSFSTREAM_CLASS_INHERIT
#include "service/vsf_service.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*
static struct {
    bool                        bInitialised;
    //vsf_stream_status_t         tStatus;
    //vsf_stream_dat_drn_evt_t    tEvent;
} s_tNuStream = {0};
*/
/*============================ PROTOTYPES ====================================*/
#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
static void vsf_nu_console_stream_init(vsf_stream_t *stream);
static uint_fast32_t vsf_nu_console_stream_write(   vsf_stream_t *stream, 
                                                    uint8_t *buf, 
                                                    uint_fast32_t size);
static uint_fast32_t vsf_nu_console_stream_get_data_length(vsf_stream_t *stream);
#elif   VSF_USE_SERVICE_STREAM == ENABLED
static vsf_err_t vsf_nu_console_stream_tx_send_pbuf(vsf_stream_tx_t *obj_ptr, 
                                                    vsf_pbuf_t *pblock);
static 
vsf_stream_status_t vsf_nu_console_stream_tx_get_status(vsf_stream_tx_t *obj_ptr);

static vsf_err_t vsf_nu_console_stream_tx_dat_drn_evt_reg(  
                                            vsf_stream_tx_t *obj_ptr, 
                                            vsf_stream_dat_drn_evt_t tEvent);
#endif
/*============================ GLOBAL VARIABLES ==============================*/
#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
const vsf_stream_op_t vsf_nu_console_stream_op = {
    .init               = vsf_nu_console_stream_init,
    .get_data_length    = vsf_nu_console_stream_get_data_length,
    .write              = vsf_nu_console_stream_write,
};

vsf_stream_t  VSF_DEBUG_STREAM = {
    .op = &vsf_nu_console_stream_op,
};

#elif   VSF_USE_SERVICE_STREAM == ENABLED
static const i_stream_pbuf_tx_t s_iNUConsoleStreamTx = {
    .Send =         &vsf_nu_console_stream_tx_send_pbuf,
    .GetStatus =    &vsf_nu_console_stream_tx_get_status,
    .DataDrainEvent = {
        .Register = &vsf_nu_console_stream_tx_dat_drn_evt_reg,
    },
};

const vsf_stream_tx_t VSF_DEBUG_STREAM_TX = {
    .piMethod = &s_iNUConsoleStreamTx,
};
#endif
/*============================ IMPLEMENTATION ================================*/

WEAK(vsf_stdout_init) 
void vsf_stdout_init(void)
{
    //NuConsole_Init();
}

WEAK(vsf_stderr_init) 
void vsf_stderr_init(void)
{
    //NuConsole_Init();
}

WEAK(vsf_stdin_init) 
void vsf_stdin_init(void)
{
    //NuConsole_Init();
}

WEAK(vsf_stdout_putchar) int vsf_stdout_putchar(char ch)
{
    return NuConsole_Write((const uint8_t *)&ch, 1);
}

WEAK(vsf_stderr_putchar) 
int vsf_stderr_putchar(char ch)
{
    return NuConsole_Write((const uint8_t *)&ch, 1);
}

WEAK(vsf_stdin_getchar) 
int vsf_stdin_getchar(void)
{
    int ch = 0;
    NuConsole_Read((uint8_t *)&ch, 1);
    return ch;
}

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
static void vsf_nu_console_stream_init(vsf_stream_t *stream)
{
    //NuConsole_Init();
}

static uint_fast32_t vsf_nu_console_stream_write(   vsf_stream_t *stream, 
                                                    uint8_t *buf, 
                                                    uint_fast32_t size)
{
    /*if (!s_tNuStream.bInitialised) {
        s_tNuStream.bInitialised = true;
        NuConsole_Init();
    }*/
    return NuConsole_Write(buf, size);
}

static uint_fast32_t vsf_nu_console_stream_get_data_length(vsf_stream_t *stream)
{
    return 0;
}
#elif   VSF_USE_SERVICE_STREAM == ENABLED

static vsf_err_t vsf_nu_console_stream_tx_send_pbuf(vsf_stream_tx_t *obj_ptr, 
                                                    vsf_pbuf_t *block_ptr)
{
    vsf_err_t result = VSF_ERR_NONE;
    /*if (!s_tNuStream.bInitialised) {
        s_tNuStream.bInitialised = true;
        NuConsole_Init();
    }*/
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
            NuConsole_Write(vsf_pbuf_buffer_get(block_ptr), 
                            vsf_pbuf_size_get(block_ptr));
        }
        
    } while(0);

    vsf_pbuf_free(block_ptr);

    return result;
}

static 
vsf_stream_status_t vsf_nu_console_stream_tx_get_status(vsf_stream_tx_t *obj_ptr)
{
    return (vsf_stream_status_t){0};//s_tNuStream.tStatus;
}

static vsf_err_t vsf_nu_console_stream_tx_dat_drn_evt_reg(  
                                            vsf_stream_tx_t *obj_ptr, 
                                            vsf_stream_dat_drn_evt_t tEvent)
{
    //s_tNuStream.tEvent = tEvent;
    return VSF_ERR_NONE;
}
#endif
#endif
