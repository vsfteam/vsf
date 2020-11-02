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

#if VSF_USE_STREAM == ENABLED
#include <stdio.h>
#include <stdarg.h>

/*============================ MACROS ========================================*/
#ifndef CDC_BUFFER_BLOCK_SIZE
#   define CDC_BUFFER_BLOCK_SIZE        256
#endif
#ifndef CDC_BUFFER_BLOCK_COUNT
#   define CDC_BUFFER_BLOCK_COUNT       4
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
declare_pbuf_pool(cdc_pbuf_pool_t)
def_pbuf_pool(cdc_pbuf_pool_t, CDC_BUFFER_BLOCK_SIZE)

/*============================ GLOBAL VARIABLES ==============================*/
NO_INIT static vsf_stream_src_t g_tStreamSourceCDC;
NO_INIT vsf_stream_fifo_t s_tCDCStreamFIFO;
#if USER_APP_CFG_CDC_POOL == ENABLED
NO_INIT vsf_pool(cdc_pbuf_pool_t) g_tGenericPBUFPool;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
#if USER_APP_CFG_CDC_POOL == ENABLED
implement_pbuf_pool(cdc_pbuf_pool_t)
#endif

#if VSF_USE_USB_DEVICE == ENABLED

void cdc_stream_init(void)
{
    //! initialise stream fifo
    do {
        vsf_stream_fifo_init(&s_tCDCStreamFIFO, NULL);
    } while(0);

#if USER_APP_CFG_CDC_POOL == ENABLED
    //! initialise pbuf pool
    init_pbuf_pool( cdc_pbuf_pool_t, 
                    &g_tGenericPBUFPool,
                    VSF_PBUF_ADAPTER_CDC_SRC,   
                    CDC_BUFFER_BLOCK_COUNT);
#endif
    //! initialise stream source
    do {
        vsf_stream_src_cfg_t cfg = {
            .ptTX = &s_tCDCStreamFIFO.TX,                                       //!< connect stream TX 
#if USER_APP_CFG_CDC_POOL == ENABLED
            .tRequestPBUFEvent = 
                vsf_pbuf_pool_req_pbuf_evt( cdc_pbuf_pool_t, 
                                            &g_tGenericPBUFPool),
#endif
        };

        VSF_STREAM_SRC.Init(&g_tStreamSourceCDC, &cfg);

    } while(0);
}

void usrapp_log_info(const char *fmt, ...)
{
    vsf_pbuf_t *block_ptr = NULL; 
    __vsf_sched_safe(
    #if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
        if (!s_tCDCStreamFIFO.RX.piMethod->GetStatus(&s_tCDCStreamFIFO.RX).IsOpen) {
            vsf_sched_safe_exit();
            return;
        }
    #endif
        block_ptr = vsf_stream_src_new_pbuf(&g_tStreamSourceCDC, -1, -1);
        if (NULL == block_ptr) {
            //! no enough buffer
            vsf_sched_safe_exit();
            return;
        }
        //vsf_pbuf_size_reset(block_ptr);
    )

    do {
        va_list ap;
        va_start(ap, fmt);

        vsf_pbuf_capability_t tCapability = vsf_pbuf_capability_get(block_ptr);
        ASSERT(0 == tCapability.is_no_write && 0 == tCapability.is_no_read);
#if VSF_PBUF_CFG_INDIRECT_RW_SUPPORT == ENABLED
        if (tCapability.is_no_direct_access) {
            /*! \Note NO Direct Memory Access is supported. If you assume this
                      SHOULD NOT happen in your application, then you can use 
                      the simplified access as shown in else part
             */
            char chLocalBuffer[CDC_BUFFER_BLOCK_SIZE];
            uint_fast32_t hwWrittenSize = 
                vsnprintf(chLocalBuffer, sizeof(chLocalBuffer), fmt, ap);
            vsf_pbuf_buffer_write(block_ptr, chLocalBuffer, hwWrittenSize, 0);
        } else 
#endif
        { 
            vsf_pbuf_size_set(block_ptr, vsnprintf(   vsf_pbuf_buffer_get(block_ptr), 
                                                    vsf_pbuf_size_get(block_ptr), 
                                                    fmt, 
                                                    ap));
        }

        //! write stream
        VSF_STREAM_SRC.Block.Send(&g_tStreamSourceCDC, block_ptr);
    } while (0);
}
#else
void usrapp_log_info(const char *fmt, ...)
{
}
#endif

#endif