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

#include "../../../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED && VSF_AUDIO_USE_DUMMY == ENABLED

#define __VSF_AUDIO_CLASS_INHERIT__
#define __VSF_AUDIO_DUMMY_CLASS_IMPLEMENT
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__

#include "service/vsf_service.h"
#include "component/av/vsf_av.h"
#include "./vsf_audio_dummy.h"

/*============================ MACROS ========================================*/

#if VSF_AUDIO_DUMMY_CFG_TRACE == ENABLED
#   define __vsf_audio_dummy_trace(...)         vsf_trace(__VA_ARGS__)
#else
#   define __vsf_audio_dummy_trace(...)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __vk_audio_dummy_init)

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
dcl_vsf_peda_methods(static, __vk_audio_dummy_playback_control)
dcl_vsf_peda_methods(static, __vk_audio_dummy_playback_start)
dcl_vsf_peda_methods(static, __vk_audio_dummy_playback_stop)

static void __vk_audio_dummy_playback_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt);
#endif

#if VSF_AUDIO_USE_CATURE == ENABLED
dcl_vsf_peda_methods(static, __vk_audio_dummy_capture_control)
dcl_vsf_peda_methods(static, __vk_audio_dummy_capture_start)
dcl_vsf_peda_methods(static, __vk_audio_dummy_capture_stop)
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vk_audio_drv_t vk_audio_dummy_drv = {
    .init       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_dummy_init),
};

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
static const vk_audio_stream_drv_t __vk_audio_dummy_stream_drv_playback = {
    .control    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_dummy_playback_control),
    .start      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_dummy_playback_start),
    .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_dummy_playback_stop),
};
#endif

#if VSF_AUDIO_USE_CATURE == ENABLED
static const vk_audio_stream_drv_t __vk_audio_dummy_stream_drv_capture = {
    .control    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_dummy_capture_control),
    .start      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_dummy_capture_start),
    .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_dummy_capture_stop),
};
#endif

/*============================ IMPLEMENTATION ================================*/

__vsf_component_peda_ifs_entry(__vk_audio_dummy_init, vk_audio_init)
{
    vsf_peda_begin();
    vk_audio_dummy_dev_t *dev = vsf_container_of(&vsf_this, vk_audio_dummy_dev_t, use_as__vk_audio_dev_t);

    switch (evt) {
    case VSF_EVT_INIT:
        if (0 == dev->stream_num) {
            dev->stream = dev->__stream;
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
            dev->stream[dev->stream_num].dir_in1out0 = 0;
            dev->stream[dev->stream_num].format.value = 0;
            dev->stream[dev->stream_num].drv = &__vk_audio_dummy_stream_drv_playback;
            dev->stream[dev->stream_num].dev = &dev->use_as__vk_audio_dev_t;
            dev->stream_num++;
#endif
#if VSF_AUDIO_USE_CATURE == ENABLED
            dev->stream[dev->stream_num].dir_in1out0 = 1;
            dev->stream[dev->stream_num].drv = &__vk_audio_dummy_stream_drv_capture;
            dev->stream[dev->stream_num].dev = &dev->use_as__vk_audio_dev_t;
            dev->stream_num++;
#endif
        }
        vsf_eda_return(VSF_ERR_NONE);
        break;
    }
    vsf_peda_end();
}

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
__vsf_component_peda_ifs_entry(__vk_audio_dummy_playback_control, vk_audio_control)
{
    vsf_peda_begin();
    vsf_peda_end();
}

static void __vk_audio_dummy_ontimer(vsf_callback_timer_t *timer)
{
    vk_audio_dummy_playback_ctx_t *playback_ctx = vsf_container_of(timer, vk_audio_dummy_playback_ctx_t, timer);

    __vsf_audio_dummy_trace(VSF_TRACE_DEBUG, "%d [audio_dummy]: on_timer\r\n", vsf_systimer_get_ms());

    playback_ctx->is_timing = false;
    __vk_audio_dummy_playback_evthandler(playback_ctx->audio_stream->stream, playback_ctx->audio_stream, VSF_STREAM_ON_IN);
}

static bool __vk_audio_dummy_playback_buffer(vk_audio_dummy_dev_t *dev, uint8_t *buffer, uint_fast32_t size)
{
    vk_audio_dummy_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    vk_audio_format_t *audio_format = &playback_ctx->audio_stream->format;

    // __vk_audio_dummy_playback_buffer can be called in stream evthandler and on_timer event,
    //  with different priority, so need protect here
    vsf_protect_t orig = vsf_protect_int();
    if (!playback_ctx->is_timing) {
        playback_ctx->is_timing = true;
        vsf_unprotect_int(orig);

        uint_fast32_t nsamples = size / (audio_format->channel_num * VSF_AUDIO_DATA_TYPE_BITLEN(audio_format->datatype.value) >> 3);
        vsf_callback_timer_add_us(&playback_ctx->timer, nsamples * 1000000 / (audio_format->sample_rate * 100));

        return true;
    }
    vsf_unprotect_int(orig);
    return false;
}

static void __vk_audio_dummy_playback_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vk_audio_stream_t *audio_stream = param;
    vk_audio_stream_t *audio_stream_base = audio_stream - audio_stream->stream_index;
    vk_audio_dummy_dev_t *dev = vsf_container_of(audio_stream_base, vk_audio_dummy_dev_t, __stream);
    vk_audio_dummy_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    uint_fast32_t datasize;
    uint8_t *buff;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN:
        if (playback_ctx->is_playing) {
            __vsf_audio_dummy_trace(VSF_TRACE_DEBUG, "%d [audio_dummy]: play stream evthandler\r\n", vsf_systimer_get_ms());
            datasize = vsf_stream_get_rbuf(stream, &buff);
            if (!datasize) { break; }

            if (__vk_audio_dummy_playback_buffer(dev, buff, datasize)) {
                vsf_stream_read(stream, (uint8_t *)buff, datasize);
            }
        }
        break;
    }
}

__vsf_component_peda_ifs_entry(__vk_audio_dummy_playback_start, vk_audio_start)
{
    vsf_peda_begin();
    vk_audio_dummy_dev_t *dev = vsf_container_of(&vsf_this, vk_audio_dummy_dev_t, use_as__vk_audio_dev_t);
    vk_audio_dummy_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;

    switch (evt) {
    case VSF_EVT_INIT:
        if (playback_ctx->is_playing) {
            VSF_AV_ASSERT(false);
            vsf_eda_return(VSF_ERR_FAIL);
            return;
        }

        playback_ctx->audio_stream = audio_stream;
        playback_ctx->is_playing = true;
        playback_ctx->is_timing = false;
        vsf_callback_timer_init(&playback_ctx->timer);
        playback_ctx->timer.on_timer = __vk_audio_dummy_ontimer;
        audio_stream->stream->rx.param = audio_stream;
        audio_stream->stream->rx.evthandler = __vk_audio_dummy_playback_evthandler;
        vsf_stream_connect_rx(audio_stream->stream);
        if (vsf_stream_get_data_size(audio_stream->stream)) {
            __vk_audio_dummy_playback_evthandler(audio_stream->stream, audio_stream, VSF_STREAM_ON_IN);
        }

        vsf_eda_return(VSF_ERR_NONE);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_audio_dummy_playback_stop, vk_audio_stop)
{
    vsf_peda_begin();
    vk_audio_dummy_dev_t *dev = vsf_container_of(&vsf_this, vk_audio_dummy_dev_t, use_as__vk_audio_dev_t);
    vk_audio_dummy_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;

    switch (evt) {
    case VSF_EVT_INIT:
        playback_ctx->is_playing = false;
        // TODO: make sure play.stream will not be used
        audio_stream->stream = NULL;
        vsf_eda_return(VSF_ERR_NONE);
        break;
    }
    vsf_peda_end();
}
#endif

#endif
