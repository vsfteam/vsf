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

#include "../../../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED && VSF_USE_WINSOUND == ENABLED

#define __VSF_AUDIO_CLASS_INHERIT__
#define __VSF_WINSOUND_CLASS_IMPLEMENT
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__

#include "service/vsf_service.h"
#include "../../vsf_audio.h"
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

#if VSF_AUDIO_USE_PLAY == ENABLED
dcl_vsf_peda_methods(static, __vk_winsound_play_set_volume)
dcl_vsf_peda_methods(static, __vk_winsound_play_set_mute)
dcl_vsf_peda_methods(static, __vk_winsound_play_start)
dcl_vsf_peda_methods(static, __vk_winsound_play_stop)

static void __vk_winsound_play_irq_thread(void *arg);
#endif

#if VSF_AUDIO_CFG_USE_CATURE == ENABLED
dcl_vsf_peda_methods(static, __vk_winsound_capture_set_volume)
dcl_vsf_peda_methods(static, __vk_winsound_capture_set_mute)
dcl_vsf_peda_methods(static, __vk_winsound_capture_start)
dcl_vsf_peda_methods(static, __vk_winsound_capture_stop)

static void __vk_winsound_capture_irq_thread(void *arg);
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vk_audio_drv_t vk_winsound_drv = {
    .init           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_init),
#if VSF_AUDIO_USE_PLAY == ENABLED
    .play_drv       = {
        .volume     = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_play_set_volume),
        .mute       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_play_set_mute),
        .play       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_play_start),
        .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_play_stop),
    },
#endif
#if VSF_AUDIO_CFG_USE_CATURE == ENABLED
    .capture_drv    = {
        .volume     = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_capture_set_volume),
        .mute       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_capture_set_mute),
        .capture    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_capture_start),
        .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winsound_capture_stop),
    },
#endif
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

__vsf_component_peda_ifs_entry(__vk_winsound_init, vk_audio_init)
{
    vsf_peda_begin();
    vk_winsound_dev_t *dev = (vk_winsound_dev_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        if (!dev->is_inited) {
            dev->is_inited = true;
            dev->play_ctx.hEvent = CreateEvent(NULL, 0, 0, NULL);

#if VSF_AUDIO_USE_PLAY == ENABLED
            __vsf_arch_irq_init(&dev->play_ctx.irq_thread, "winsound_play", __vk_winsound_play_irq_thread, dev->hw_prio);
#endif
        }
        vsf_eda_return(VSF_ERR_NONE);
        break;
    }
    vsf_peda_end();
}

#if VSF_AUDIO_USE_PLAY == ENABLED
__vsf_component_peda_ifs_entry(__vk_winsound_play_set_volume, vk_audio_play_set_volume)
{
    vsf_peda_begin();
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winsound_play_set_mute, vk_audio_play_set_mute)
{
    vsf_peda_begin();
    vsf_peda_end();
}

static bool __vk_winsound_play_buffer(vk_winsound_dev_t *dev, uint8_t *buffer, uint_fast32_t size)
{
    if (dev->play_ctx.buffer_taken < dimof(dev->play_ctx.buffer)) {
        vk_winsound_play_buffer_t *winsound_buffer =
            dev->play_ctx.fill_ticktock ? &dev->play_ctx.buffer[0] : &dev->play_ctx.buffer[1];

        vsf_protect_t orig = vsf_protect_int();
            dev->play_ctx.buffer_taken++;
        vsf_unprotect_int(orig);

        dev->play_ctx.fill_ticktock = !dev->play_ctx.fill_ticktock;
        memcpy(winsound_buffer->buffer, buffer, size);
        winsound_buffer->header.lpData = (LPSTR)winsound_buffer->buffer;
        winsound_buffer->header.dwBufferLength = size;
        winsound_buffer->header.dwFlags = 0;
        waveOutPrepareHeader(dev->play_ctx.hwo, (LPWAVEHDR)&winsound_buffer->header, sizeof(WAVEHDR));
        if (MMSYSERR_NOERROR == waveOutWrite(dev->play_ctx.hwo, (LPWAVEHDR)&winsound_buffer->header, sizeof(WAVEHDR))) {
            __vsf_winsound_trace(VSF_TRACE_DEBUG, "play stream: %d bytes %08X\r\n", size, winsound_buffer->buffer);
        }

        return true;
    }
    return false;
}

static void __vk_winsound_play_evthandler(void *param, vsf_stream_evt_t evt)
{
    vk_winsound_dev_t *dev = param;
    uint_fast32_t datasize;
    uint8_t *buff;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN:
        while (dev->play_ctx.is_playing && (dev->play_ctx.buffer_taken < dimof(dev->play_ctx.buffer))) {
            __vsf_winsound_trace(VSF_TRACE_DEBUG, "play stream evthandler\r\n");
            datasize = vsf_stream_get_rbuf(dev->play.stream, &buff);
            if (!datasize) { break; }

            if (__vk_winsound_play_buffer(dev, buff, datasize)) {
                vsf_stream_read(dev->play.stream, (uint8_t *)buff, datasize);
            }
        }
        break;
    }
}

static void __vk_winsound_play_irq_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_winsound_play_ctx_t *play = container_of(irq_thread, vk_winsound_play_ctx_t, irq_thread);
    vk_winsound_dev_t *dev = container_of(play, vk_winsound_dev_t, play);

    __vsf_arch_irq_set_background(irq_thread);
    while (1) {
        WaitForSingleObject(play->hEvent, INFINITE);

        __vsf_arch_irq_start(irq_thread);
            if (play->buffer_taken > 0) {
//                vk_winsound_play_buffer_t *winsound_buffer =
//                    dev->play_ctx.play_ticktock ? &dev->play_ctx.buffer[0] : &dev->play_ctx.buffer[1];
//                WAVEHDR *header = (WAVEHDR *)&winsound_buffer->header;
                dev->play_ctx.play_ticktock = !dev->play_ctx.play_ticktock;
                __vsf_winsound_trace(VSF_TRACE_DEBUG, "winsound irq: %02X %d %08X\r\n", header->dwFlags, header->dwBufferLength, header->lpData);
                // seems no need to wait WHDR_DONE
//                if (header->dwFlags & WHDR_DONE) {
                    vsf_protect_t orig = vsf_protect_int();
                        dev->play_ctx.buffer_taken--;
                    vsf_unprotect_int(orig);
                    __vk_winsound_play_evthandler(dev, VSF_STREAM_ON_IN);
//                }
                __vsf_winsound_trace(VSF_TRACE_DEBUG, "winsound irq end\r\n");
            }
        __vsf_arch_irq_end(irq_thread, false);
    }
}

__vsf_component_peda_ifs_entry(__vk_winsound_play_start, vk_audio_play_start)
{
    vsf_peda_begin();
    vk_winsound_dev_t *dev = (vk_winsound_dev_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT: {
            WAVEFORMATEX wfx    = {
                .wFormatTag     = WAVE_FORMAT_PCM,
                .nChannels      = dev->play.format.channel_num,
                .nSamplesPerSec = dev->play.format.sample_rate,
                .wBitsPerSample = dev->play.format.sample_bit_width,
            };

            switch (wfx.wFormatTag) {
            case WAVE_FORMAT_PCM:
                wfx.nBlockAlign     = wfx.nChannels * wfx.wBitsPerSample / 8;
                wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
                break;
            }

            if (dev->play_ctx.is_playing) {
                VSF_AV_ASSERT(false);
            do_return_fail:
                vsf_eda_return(VSF_ERR_FAIL);
                return;
            }
  
            if (MMSYSERR_NOERROR != waveOutOpen((LPHWAVEOUT)&dev->play_ctx.hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)dev->play_ctx.hEvent, 0L, CALLBACK_EVENT)) {
                goto do_return_fail;
            }

            uint_fast32_t half_buffer_size = vsf_stream_get_buff_size(dev->play.stream) / 2;
            dev->play_ctx.buffer[0].buffer = vsf_heap_malloc(half_buffer_size);
            dev->play_ctx.buffer[1].buffer = vsf_heap_malloc(half_buffer_size);
            if ((NULL == dev->play_ctx.buffer[0].buffer) || (NULL == dev->play_ctx.buffer[1].buffer)) {
                VSF_AV_ASSERT(false);
                goto do_return_fail;
            }

            dev->play_ctx.is_playing = true;
            dev->play_ctx.fill_ticktock = false;
            dev->play_ctx.play_ticktock = false;
            dev->play_ctx.buffer_taken = 0;
            dev->play.stream->rx.param = dev;
            dev->play.stream->rx.evthandler = __vk_winsound_play_evthandler;
            vsf_stream_connect_rx(dev->play.stream);
            if (vsf_stream_get_data_size(dev->play.stream)) {
                __vk_winsound_play_evthandler(dev, VSF_STREAM_ON_IN);
            }

            vsf_eda_return(VSF_ERR_NONE);
            break;
        }
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winsound_play_stop, vk_audio_play_stop)
{
    vsf_peda_begin();
    vk_winsound_dev_t *dev = (vk_winsound_dev_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        dev->play_ctx.is_playing = false;
        waveOutClose(dev->play_ctx.hwo);
        // TODO: make sure play.stream will not be used
        dev->play.stream = NULL;

        if (dev->play_ctx.buffer[0].buffer != NULL) {
            vsf_heap_free(dev->play_ctx.buffer[0].buffer);
            dev->play_ctx.buffer[0].buffer = NULL;
        }
        if (dev->play_ctx.buffer[1].buffer != NULL) {
            vsf_heap_free(dev->play_ctx.buffer[1].buffer);
            dev->play_ctx.buffer[1].buffer = NULL;
        }

        vsf_eda_return(VSF_ERR_NONE);
        break;
    }
    vsf_peda_end();
}
#endif

#endif
