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
#include "vsf.h"

#if VSF_USE_USB_HOST == ENABLED && APP_USE_USBH_DEMO == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USBH_USE_UAC == ENABLED
describe_mem_stream(__user_usbh_uac_rx_stream, 100)
describe_mem_stream(__user_usbh_uac_tx_stream, 200)
static void __usrapp_usbh_uac_on_stream(void *param, vsf_stream_evt_t evt)
{
    vsf_stream_t *stream = param;
    switch (evt) {
    case VSF_STREAM_ON_TX:
        vsf_stream_write(stream, NULL, vsf_stream_get_free_size(stream));
        break;
    case VSF_STREAM_ON_RX:
        vsf_stream_read(stream, NULL, vsf_stream_get_buff_size(stream));
        break;
    }
}
void vsf_usbh_uac_on_new(void *uac, usb_uac_ac_interface_header_desc_t *ac_header)
{
    vk_usbh_uac_stream_t * stream;

    vsf_trace(VSF_TRACE_INFO, "usbh_uac: new dev" VSF_TRACE_CFG_LINEEND);
    // TODO: parse UAC units
    vsf_trace(VSF_TRACE_INFO, "  descriptors:" VSF_TRACE_CFG_LINEEND);
    vsf_trace_buffer(VSF_TRACE_INFO, ac_header, ac_header->wTotalLength);

    for (int i = 0; i < ac_header->bInCollection; i++) {
        stream = vsf_usbh_uac_get_stream_info(uac, i);
        vsf_trace(VSF_TRACE_INFO, "  stream%d:" VSF_TRACE_CFG_LINEEND, i);
        vsf_trace(VSF_TRACE_INFO, "    direction: %s" VSF_TRACE_CFG_LINEEND, stream->is_in ? "IN" : "OUT");
        vsf_trace(VSF_TRACE_INFO, "    format: 0x%04X" VSF_TRACE_CFG_LINEEND, stream->format);
        vsf_trace(VSF_TRACE_INFO, "    channel_num: %d" VSF_TRACE_CFG_LINEEND, stream->channel_num);
        vsf_trace(VSF_TRACE_INFO, "    sample_size: %d" VSF_TRACE_CFG_LINEEND, stream->sample_size);
        vsf_trace(VSF_TRACE_INFO, "    sample_rate: %d" VSF_TRACE_CFG_LINEEND, stream->sample_rate);

        if (stream->is_in) {
            vsf_usbh_uac_connect_stream(uac, i, &__user_usbh_uac_rx_stream.use_as__vsf_stream_t);

            __user_usbh_uac_rx_stream.rx.param = &__user_usbh_uac_rx_stream;
            __user_usbh_uac_rx_stream.rx.evthandler = __usrapp_usbh_uac_on_stream;
            vsf_stream_connect_rx(&__user_usbh_uac_rx_stream.use_as__vsf_stream_t);
        } else {
            vsf_usbh_uac_connect_stream(uac, i, &__user_usbh_uac_tx_stream.use_as__vsf_stream_t);

            __user_usbh_uac_tx_stream.tx.param = &__user_usbh_uac_tx_stream;
            __user_usbh_uac_tx_stream.tx.evthandler = __usrapp_usbh_uac_on_stream;
            vsf_stream_connect_tx(&__user_usbh_uac_tx_stream.use_as__vsf_stream_t);
            vsf_stream_write(&__user_usbh_uac_tx_stream.use_as__vsf_stream_t, NULL, vsf_stream_get_free_size(&__user_usbh_uac_tx_stream.use_as__vsf_stream_t));
        }
    }
}
#endif

#if APP_USE_LINUX_DEMO == ENABLED
int usbh_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

    usrapp_usbh_common_init();
    return 0;
}

#endif
