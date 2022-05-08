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

#if VSF_USE_AUDIO == ENABLED && VSF_USE_WINSOUND == ENABLED

#define __VSF_AUDIO_CLASS_INHERIT__
#define __VSF_WINSOUND_CLASS_IMPLEMENT
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__

#include "service/vsf_service.h"
#include "component/av/vsf_av.h"
#include "./vsf_winsound.h"

#include <Windows.h>

#pragma comment(lib, "winmm.lib")

// Code from MSDN uses original naming conventions

/*============================ MACROS ========================================*/

#if VSF_WINSOUND_CFG_TRACE == ENABLED
#   define __vsf_winsound_trace(...)        vsf_trace(__VA_ARGS__)
#else
#   define __vsf_winsound_trace(...)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __vk_winsound_init)

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
dcl_vsf_peda_methods(static, __vk_winsound_playback_control)
dcl_vsf_peda_methods(static, __vk_winsound_playback_start)
dcl_vsf_peda_methods(static, __vk_winsound_playback_stop)

static void __vk_winsound_playback_irq_thread(void *arg);
#endif

#if VSF_AUDIO_CFG_USE_CATURE == ENABLED
dcl_vsf_peda_methods(static, __vk_winsound_capture_control)
dcl_vsf_peda_methods(static, __vk_winsound_capture_start)
dcl_vsf_peda_methods(static, __vk_winsound_capture_stop)

static void __vk_winsound_capture_irq_thread(void *arg);
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vk_audio_drv_t vk_winsound_drv = {
    .init       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_init),
};

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
static const vk_audio_stream_drv_t __vk_winsound_stream_drv_playback = {
    .control    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_playback_control),
    .start      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_playback_start),
    .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_playback_stop),
};
#endif

#if VSF_AUDIO_CFG_USE_CATURE == ENABLED
static const vk_audio_stream_drv_t __vk_winsound_stream_drv_capture = {
    .control    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_capture_control),
    .start      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_capture_start),
    .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_capture_stop),
};
#endif

/*============================ IMPLEMENTATION ================================*/

__vsf_component_peda_ifs_entry(__vk_winsound_init, vk_audio_init)
{
    vsf_peda_begin();
    vk_winsound_dev_t *dev = container_of(&vsf_this, vk_winsound_dev_t, use_as__vk_audio_dev_t);
    vk_winsound_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    uint_fast8_t stream_idx = 0;

    switch (evt) {
    case VSF_EVT_INIT:
        if (!dev->is_inited) {
            dev->is_inited = true;
            playback_ctx->hEvent = CreateEvent(NULL, 0, 0, NULL);

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
            dev->stream[stream_idx].dir_in1out0 = 0;
            dev->stream[stream_idx].format.value = 0;
            dev->stream[stream_idx].drv = &__vk_winsound_stream_drv_playback;
            stream_idx++;
            __vsf_arch_irq_init(&playback_ctx->irq_thread, "winsound_play",
                        __vk_winsound_playback_irq_thread, dev->hw_prio);
#endif
#if VSF_AUDIO_USE_CATURE == ENABLED
            dev->stream[stream_idx].dir_in1out0 = 1;
            dev->stream[stream_idx].drv = &__vk_winsound_stream_drv_capture;
            stream_idx++;
#endif
            dev->stream_num = stream_idx;
        }
        vsf_eda_return(VSF_ERR_NONE);
        break;
    }
    vsf_peda_end();
}

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
__vsf_component_peda_ifs_entry(__vk_winsound_playback_control, vk_audio_control)
{
    vsf_peda_begin();
    vsf_peda_end();
}

static bool __vk_winsound_playback_buffer(vk_winsound_dev_t *dev, uint8_t *buffer, uint_fast32_t size)
{
    vk_winsound_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    if (playback_ctx->buffer_taken < dimof(playback_ctx->buffer)) {
        vk_winsound_playback_buffer_t *winsound_buffer =
            playback_ctx->fill_ticktock ? &playback_ctx->buffer[0] : &playback_ctx->buffer[1];

        vsf_protect_t orig = vsf_protect_int();
            playback_ctx->buffer_taken++;
        vsf_unprotect_int(orig);

        playback_ctx->fill_ticktock = !playback_ctx->fill_ticktock;
        memcpy(winsound_buffer->buffer, buffer, size);
        winsound_buffer->header.lpData = (LPSTR)winsound_buffer->buffer;
        winsound_buffer->header.dwBufferLength = size;
        winsound_buffer->header.dwFlags = 0;
        waveOutPrepareHeader(playback_ctx->hwo, (LPWAVEHDR)&winsound_buffer->header, sizeof(WAVEHDR));
        if (MMSYSERR_NOERROR == waveOutWrite(playback_ctx->hwo, (LPWAVEHDR)&winsound_buffer->header, sizeof(WAVEHDR))) {
            __vsf_winsound_trace(VSF_TRACE_DEBUG, "%d [winsound]: play stream: %d bytes %08X\r\n", vsf_systimer_get_ms(), size, winsound_buffer->buffer);
        }

        return true;
    }
    return false;
}

static void __vk_winsound_playback_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vk_audio_stream_t *audio_stream = param;
    vk_audio_stream_t *audio_stream_base = audio_stream - audio_stream->stream_index;
    vk_winsound_dev_t *dev = container_of(audio_stream_base, vk_winsound_dev_t, stream);
    vk_winsound_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    uint_fast32_t datasize;
    uint8_t *buff;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN:
        while (playback_ctx->is_playing && (playback_ctx->buffer_taken < dimof(playback_ctx->buffer))) {
            __vsf_winsound_trace(VSF_TRACE_DEBUG, "%d [winsound]: play stream evthandler\r\n", vsf_systimer_get_ms());
            datasize = vsf_stream_get_rbuf(stream, &buff);
            if (!datasize) { break; }

            if (__vk_winsound_playback_buffer(dev, buff, datasize)) {
                vsf_stream_read(stream, (uint8_t *)buff, datasize);
            }
        }
        break;
    }
}

static void __vk_winsound_playback_irq_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_winsound_playback_ctx_t *playback_ctx = container_of(irq_thread, vk_winsound_playback_ctx_t, irq_thread);

    __vsf_arch_irq_set_background(irq_thread);
    while (1) {
        WaitForSingleObject(playback_ctx->hEvent, INFINITE);

        __vsf_arch_irq_start(irq_thread);
            if (playback_ctx->buffer_taken > 0) {
                vk_winsound_playback_buffer_t *winsound_buffer =
                    playback_ctx->play_ticktock ? &playback_ctx->buffer[0] : &playback_ctx->buffer[1];
                WAVEHDR *header = (WAVEHDR *)&winsound_buffer->header;
                VSF_UNUSED_PARAM(header);
                playback_ctx->play_ticktock = !playback_ctx->play_ticktock;

                __vsf_winsound_trace(VSF_TRACE_DEBUG, "%d [winsound]: playback evt %d\r\n", vsf_systimer_get_ms(), header->dwFlags);
                vsf_protect_t orig = vsf_protect_int();
                    playback_ctx->buffer_taken--;
                vsf_unprotect_int(orig);
                __vk_winsound_playback_evthandler(playback_ctx->audio_stream->stream, playback_ctx->audio_stream, VSF_STREAM_ON_IN);
            }
        __vsf_arch_irq_end(irq_thread, false);
    }
}

__vsf_component_peda_ifs_entry(__vk_winsound_playback_start, vk_audio_start)
{
    vsf_peda_begin();
    vk_winsound_dev_t *dev = container_of(&vsf_this, vk_winsound_dev_t, use_as__vk_audio_dev_t);
    vk_winsound_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;

    switch (evt) {
    case VSF_EVT_INIT: {
            WAVEFORMATEX wfx    = {
                .wFormatTag     = WAVE_FORMAT_PCM,
                .nChannels      = audio_stream->format.channel_num,
                .nSamplesPerSec = audio_stream->format.sample_rate,
                .wBitsPerSample = audio_stream->format.sample_bit_width,
            };

            switch (wfx.wFormatTag) {
            case WAVE_FORMAT_PCM:
                wfx.nBlockAlign     = wfx.nChannels * wfx.wBitsPerSample / 8;
                wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
                break;
            }

            if (playback_ctx->is_playing) {
                VSF_AV_ASSERT(false);
            do_return_fail:
                vsf_eda_return(VSF_ERR_FAIL);
                return;
            }

            if (MMSYSERR_NOERROR != waveOutOpen((LPHWAVEOUT)&playback_ctx->hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)playback_ctx->hEvent, 0L, CALLBACK_EVENT)) {
                goto do_return_fail;
            }

            uint_fast32_t half_buffer_size = vsf_stream_get_buff_size(audio_stream->stream) / 2;
            playback_ctx->buffer[0].buffer = vsf_heap_malloc(half_buffer_size);
            playback_ctx->buffer[1].buffer = vsf_heap_malloc(half_buffer_size);
            if ((NULL == playback_ctx->buffer[0].buffer) || (NULL == playback_ctx->buffer[1].buffer)) {
                VSF_AV_ASSERT(false);
                goto do_return_fail;
            }

            playback_ctx->audio_stream = audio_stream;
            playback_ctx->is_playing = true;
            playback_ctx->fill_ticktock = false;
            playback_ctx->play_ticktock = false;
            playback_ctx->buffer_taken = 0;
            audio_stream->stream->rx.param = audio_stream;
            audio_stream->stream->rx.evthandler = __vk_winsound_playback_evthandler;
            vsf_stream_connect_rx(audio_stream->stream);
            if (vsf_stream_get_data_size(audio_stream->stream)) {
                __vk_winsound_playback_evthandler(audio_stream->stream, audio_stream, VSF_STREAM_ON_IN);
            }

            vsf_eda_return(VSF_ERR_NONE);
            break;
        }
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winsound_playback_stop, vk_audio_stop)
{
    vsf_peda_begin();
    vk_winsound_dev_t *dev = container_of(&vsf_this, vk_winsound_dev_t, use_as__vk_audio_dev_t);
    vk_winsound_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;

    switch (evt) {
    case VSF_EVT_INIT:
        playback_ctx->is_playing = false;
        waveOutClose(playback_ctx->hwo);
        // TODO: make sure play.stream will not be used
        audio_stream->stream = NULL;

        if (playback_ctx->buffer[0].buffer != NULL) {
            vsf_heap_free(playback_ctx->buffer[0].buffer);
            playback_ctx->buffer[0].buffer = NULL;
        }
        if (playback_ctx->buffer[1].buffer != NULL) {
            vsf_heap_free(playback_ctx->buffer[1].buffer);
            playback_ctx->buffer[1].buffer = NULL;
        }

        vsf_eda_return(VSF_ERR_NONE);
        break;
    }
    vsf_peda_end();
}
#endif

#endif
