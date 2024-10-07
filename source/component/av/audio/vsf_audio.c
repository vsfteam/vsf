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

#include "../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_AUDIO_CLASS_IMPLEMENT
#define __VSF_EDA_CLASS_INHERIT__

#include "utilities/vsf_utilities.h"
#include "component/av/vsf_av.h"

/*============================ MACROS ========================================*/

#if VSF_USE_SIMPLE_STREAM != ENABLED
#   error VSF_USE_SIMPLE_STREAM must be enabled for audio
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use scsi
#endif

#if VSF_KERNEL_USE_SIMPLE_SHELL != ENABLED
#   error VSF_KERNEL_USE_SIMPLE_SHELL must be enabled
#endif

#if VSF_AUDIO_USE_PLAYBACK != ENABLED && VSF_AUDIO_USE_CAPTURE != ENABLED
#   error at least one of VSF_AUDIO_USE_PLAYBACK/VSF_AUDIO_USE_CAPTURE must be enabled
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vk_audio_init(vk_audio_dev_t *pthis)
{
    vsf_err_t err;
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->drv != NULL)
                &&  (pthis->drv->init != NULL));

    if (pthis->stream != NULL) {
        for (uint_fast8_t i = 0; i < pthis->stream_num; i++) {
            pthis->stream[i].stream_index = i;
        }
    }
    __vsf_component_call_peda_ifs(vk_audio_init, err, pthis->drv->init, 0, pthis);
    return err;
}

vsf_err_t vk_audio_control(vk_audio_dev_t *pthis, uint_fast8_t stream_idx,
                    vk_av_control_type_t type, vk_av_control_value_t value)
{
    vsf_err_t err;
    vk_audio_stream_t *audio_stream = &pthis->stream[stream_idx];
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->stream_num >= stream_idx)
                &&  (audio_stream->drv != NULL)
                &&  (audio_stream->drv->control != NULL));

    __vsf_component_call_peda_ifs(vk_audio_control, err, audio_stream->drv->control, 0, pthis,
        .audio_stream   = audio_stream,
        .type           = type,
        .value          = value,
    );
    return err;
}

vsf_err_t vk_audio_start(vk_audio_dev_t *pthis, uint_fast8_t stream_idx,
                    vsf_stream_t *stream, vk_audio_format_t *format)
{
    vsf_err_t err;
    vk_audio_stream_t *audio_stream = &pthis->stream[stream_idx];
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (format != NULL)
                &&  (stream != NULL)
                &&  (pthis->stream_num > stream_idx)
                &&  (audio_stream->drv != NULL)
                &&  (audio_stream->drv->start != NULL)
                &&  (NULL == audio_stream->stream));

    audio_stream->stream = stream;
    audio_stream->format = *format;
    __vsf_component_call_peda_ifs(vk_audio_start, err, audio_stream->drv->start, 0, pthis,
        .audio_stream   = audio_stream,
    );
    return err;
}

vsf_err_t vk_audio_stop(vk_audio_dev_t *pthis, uint_fast8_t stream_idx)
{
    vsf_err_t err;
    vk_audio_stream_t *audio_stream = &pthis->stream[stream_idx];
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->stream_num >= stream_idx)
                &&  (audio_stream->drv != NULL)
                &&  (audio_stream->drv->stop != NULL));

    __vsf_component_call_peda_ifs(vk_audio_stop, err, audio_stream->drv->stop, 0, pthis,
        .audio_stream   = audio_stream,
    );
    return err;
}

#if VSF_AUDIO_USE_PLAYBACK == ENABLED || VSF_AUDIO_USE_CAPTURE == ENABLED
static void __vsf_audio_ticktock_stream_adapter_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vsf_audio_ticktock_stream_adapter_t *adapter = param;
    vsf_eda_post_evt(&adapter->eda, (evt << 1) + (stream == adapter->stream_tx ? 0 : 1) + VSF_EVT_USER);
}
#endif

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
static void __vsf_audio_playback_ticktock_stream_adapter_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_audio_ticktock_stream_adapter_t *adapter = vsf_container_of(eda, vsf_audio_ticktock_stream_adapter_t, eda);

    if (evt >= VSF_EVT_USER) {
        evt -= VSF_EVT_USER;

        vsf_stream_evt_t stream_evt = evt >> 1;
        vsf_stream_t *stream = evt & 1 ? adapter->stream_rx : adapter->stream_tx;
        uint_fast32_t bufsize = vsf_stream_get_buff_size(adapter->stream_rx), halfsize = bufsize >> 1;
        uint_fast32_t size;
        uint8_t *buf;

        vsf_stream_adapter_evthandler(stream, adapter, stream_evt);

        // check stream_rx(ticktock buffer) is not full, and stream_tx(audio buffer)
        //  has no date. Because stream_rx can be read(in task/interrupt of higher priority)
        //  before the check below, in which case stream_rx is not full. So in such
        //  context, stream_tx must be check.
        while ( (vsf_stream_get_data_size(adapter->stream_rx) < bufsize)
            &&  (0 == vsf_stream_get_data_size(adapter->stream_tx))) {
            size = vsf_stream_get_wbuf(adapter->stream_rx, &buf);
            VSF_AV_ASSERT(size >= halfsize);
            memset(buf, 0, halfsize);
            vsf_stream_write(adapter->stream_rx, NULL, halfsize);
        }
    }
}

void vsf_audio_playback_ticktock_stream_adapter_init(vsf_audio_ticktock_stream_adapter_t *adapter)
{
    adapter->eda.fn.evthandler = __vsf_audio_playback_ticktock_stream_adapter_evthandler;
#   if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    adapter->eda.on_terminate = NULL;
#   endif
    vsf_eda_init(&adapter->eda, VSF_AUDIO_CFG_EDA_PRIORITY);
#   if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info(&adapter->eda, "audio_playback", NULL, 0);
#   endif

    adapter->stream_rx->tx.evthandler = __vsf_audio_ticktock_stream_adapter_evthandler;
    adapter->stream_rx->tx.param = adapter;
    vsf_stream_connect_tx(adapter->stream_rx);

    adapter->stream_tx->rx.evthandler = __vsf_audio_ticktock_stream_adapter_evthandler;
    adapter->stream_tx->rx.param = adapter;
    vsf_stream_connect_rx(adapter->stream_tx);
}
#endif

#if VSF_AUDIO_USE_CAPTURE == ENABLED
static void __vsf_audio_capture_ticktock_stream_adapter_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_audio_ticktock_stream_adapter_t *adapter = vsf_container_of(eda, vsf_audio_ticktock_stream_adapter_t, eda);

    if (evt >= VSF_EVT_USER) {
        evt -= VSF_EVT_USER;

        vsf_stream_evt_t stream_evt = evt >> 1;
        vsf_stream_t *stream = evt & 1 ? adapter->stream_rx : adapter->stream_tx;
        uint_fast32_t bufsize = vsf_stream_get_buff_size(adapter->stream_tx), halfsize = bufsize >> 1;

        vsf_stream_adapter_evthandler(stream, adapter, stream_evt);
        if (vsf_stream_get_data_size(adapter->stream_tx) >= bufsize) {
            vsf_stream_read(adapter->stream_tx, NULL, halfsize);
        }
    }
}

void vsf_audio_capture_ticktock_stream_adapter_init(vsf_audio_ticktock_stream_adapter_t *adapter)
{
    adapter->eda.fn.evthandler = __vsf_audio_capture_ticktock_stream_adapter_evthandler;
#   if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    adapter->eda.on_terminate = NULL;
#   endif
    vsf_eda_init(&adapter->eda, VSF_AUDIO_CFG_EDA_PRIORITY);
#   if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info(&adapter->eda, "audio_capture", NULL, 0);
#   endif

    adapter->stream_rx->tx.evthandler = __vsf_audio_ticktock_stream_adapter_evthandler;
    adapter->stream_rx->tx.param = adapter;
    vsf_stream_connect_tx(adapter->stream_rx);

    adapter->stream_tx->rx.evthandler = __vsf_audio_ticktock_stream_adapter_evthandler;
    adapter->stream_tx->rx.param = adapter;
    vsf_stream_connect_rx(adapter->stream_tx);
}
#endif

#endif
