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

#define VSF_AUDIO_INHERIT
#define VSF_WINSOUND_IMPLEMENT
#define VSFSTREAM_CLASS_INHERIT

// TODO: use dedicated include
#include "vsf.h"
#include "./vsf_winsound.h"

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

static void __vk_winsound_init(uintptr_t target, vsf_evt_t evt);

#if VSF_AUDIO_CFG_USE_PLAY == ENABLED
static void __vk_winsound_set_play_volume(uintptr_t target, vsf_evt_t evt);
static void __vk_winsound_set_play_mute(uintptr_t target, vsf_evt_t evt);
static void __vk_winsound_play(uintptr_t target, vsf_evt_t evt);
static void __vk_winsound_stop_play(uintptr_t target, vsf_evt_t evt);

static void __vk_winsound_play_irq_thread(void *arg);
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vk_audio_drv_t vk_winsound_drv = {
    .init           = __vk_winsound_init,
#if VSF_AUDIO_CFG_USE_PLAY == ENABLED
    .play_drv       = {
        .volume     = __vk_winsound_set_play_volume,
        .mute       = __vk_winsound_set_play_mute,
        .play       = __vk_winsound_play,
        .stop       = __vk_winsound_stop_play,
    },
#endif
#if VSF_AUDIO_CFG_USE_CATURE == ENABLED
    .capture_drv    = {
        .volume     = __vk_winsound_set_capture_volume,
        .mute       = __vk_winsound_set_capture_mute,
        .play       = __vk_winsound_capture,
        .stop       = __vk_winsound_stop_capture,
    },
#endif
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vk_winsound_init(uintptr_t target, vsf_evt_t evt)
{
    vk_winsound_dev_t *dev = (vk_winsound_dev_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        if (!dev->is_inited) {
            dev->is_inited = true;
            dev->play.hEvent = CreateEvent(NULL, 0, 0, NULL);

#if VSF_AUDIO_CFG_USE_PLAY == ENABLED
            dev->play.irq_thread.name = "winsound_play";
            __vsf_arch_irq_init(&dev->play.irq_thread, __vk_winsound_play_irq_thread, dev->hw_prio, true);
#endif
        }
        dev->ctx.err = VSF_ERR_NONE;
        vsf_eda_return();
        break;
    }
}

#if VSF_AUDIO_CFG_USE_PLAY == ENABLED
static void __vk_winsound_set_play_volume(uintptr_t target, vsf_evt_t evt)
{

}

static void __vk_winsound_set_play_mute(uintptr_t target, vsf_evt_t evt)
{
}

static bool __vk_winsound_play_buffer(vk_winsound_dev_t *dev, uint8_t *buffer, uint_fast32_t size)
{
    if (dev->play.buffer_taken < dimof(dev->play.buffer)) {
        vk_winsound_play_buffer_t *winsound_buffer =
            dev->play.fill_ticktock ? &dev->play.buffer[0] : &dev->play.buffer[1];

        vsf_protect_t orig = vsf_protect_int();
            dev->play.buffer_taken++;
        vsf_unprotect_int(orig);

        dev->play.fill_ticktock = !dev->play.fill_ticktock;
        memcpy(winsound_buffer->buffer, buffer, size);
        winsound_buffer->header.lpData = (LPSTR)winsound_buffer->buffer;
        winsound_buffer->header.dwBufferLength = size;
        winsound_buffer->header.dwFlags = 0;
        waveOutPrepareHeader(dev->play.hwo, &winsound_buffer->header, sizeof(WAVEHDR));
        if (MMSYSERR_NOERROR == waveOutWrite(dev->play.hwo, &winsound_buffer->header, sizeof(WAVEHDR))) {
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
        while (dev->play.is_playing && (dev->play.buffer_taken < dimof(dev->play.buffer))) {
            __vsf_winsound_trace(VSF_TRACE_DEBUG, "play stream evthandler\r\n");
            datasize = vsf_stream_get_rbuf(dev->stream_play, &buff);
            if (!datasize) { break; }

            if (__vk_winsound_play_buffer(dev, buff, datasize)) {
                vsf_stream_read(dev->stream_play, (uint8_t *)buff, datasize);
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
                vk_winsound_play_buffer_t *winsound_buffer =
                    dev->play.play_ticktock ? &dev->play.buffer[0] : &dev->play.buffer[1];
                WAVEHDR *header = &winsound_buffer->header;
                dev->play.play_ticktock = !dev->play.play_ticktock;
                __vsf_winsound_trace(VSF_TRACE_DEBUG, "winsound irq: %02X %d %08X\r\n", header->dwFlags, header->dwBufferLength, header->lpData);
                // seems no need to wait WHDR_DONE
//                if (header->dwFlags & WHDR_DONE) {
                    vsf_protect_t orig = vsf_protect_int();
                        dev->play.buffer_taken--;
                    vsf_unprotect_int(orig);
                    __vk_winsound_play_evthandler(dev, VSF_STREAM_ON_IN);
//                }
                __vsf_winsound_trace(VSF_TRACE_DEBUG, "winsound irq end\r\n");
            }
        __vsf_arch_irq_end(irq_thread, false);
    }
}

static void __vk_winsound_play(uintptr_t target, vsf_evt_t evt)
{
    vk_winsound_dev_t *dev = (vk_winsound_dev_t *)target;

    switch (evt) {
    case VSF_EVT_INIT: {
            WAVEFORMATEX wfx    = {
                .wFormatTag     = WAVE_FORMAT_PCM,
                .nChannels      = dev->ctx.param.format.channel_num,
                .nSamplesPerSec = dev->ctx.param.format.sample_rate,
                .wBitsPerSample = dev->ctx.param.format.sample_bit_width,
            };

            switch (wfx.wFormatTag) {
            case WAVE_FORMAT_PCM:
                wfx.nBlockAlign     = wfx.nChannels * wfx.wBitsPerSample / 8;
                wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
                break;
            }

            if (dev->play.is_playing) {
                VSF_AV_ASSERT(false);
                dev->ctx.err = VSF_ERR_FAIL;
                goto do_return;
            }
  
            if (MMSYSERR_NOERROR != waveOutOpen(&dev->play.hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)dev->play.hEvent, 0L, CALLBACK_EVENT)) {
                dev->ctx.err = VSF_ERR_FAIL;
                goto do_return;
            }

            uint_fast32_t half_buffer_size = vsf_stream_get_buff_size(dev->stream_play) / 2;
            dev->play.buffer[0].buffer = vsf_heap_malloc(half_buffer_size);
            dev->play.buffer[1].buffer = vsf_heap_malloc(half_buffer_size);
            if ((NULL == dev->play.buffer[0].buffer) || (NULL == dev->play.buffer[1].buffer)) {
                VSF_AV_ASSERT(false);
                dev->ctx.err = VSF_ERR_FAIL;
                goto do_return;
            }

            dev->play.is_playing = true;
            dev->play.fill_ticktock = false;
            dev->play.play_ticktock = false;
            dev->play.buffer_taken = 0;
            dev->stream_play->rx.param = dev;
            dev->stream_play->rx.evthandler = __vk_winsound_play_evthandler;
            vsf_stream_connect_rx(dev->stream_play);
            if (vsf_stream_get_data_size(dev->stream_play)) {
                __vk_winsound_play_evthandler(dev, VSF_STREAM_ON_IN);
            }

            dev->ctx.err = VSF_ERR_NONE;
        do_return:
            vsf_eda_return();
            break;
        }
    }
}

static void __vk_winsound_stop_play(uintptr_t target, vsf_evt_t evt)
{
    vk_winsound_dev_t *dev = (vk_winsound_dev_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        dev->play.is_playing = false;
        waveOutClose(dev->play.hwo);
        // TODO: make sure stream_play will not be used
        dev->stream_play = NULL;

        if (dev->play.buffer[0].buffer != NULL) {
            vsf_heap_free(dev->play.buffer[0].buffer);
            dev->play.buffer[0].buffer = NULL;
        }
        if (dev->play.buffer[1].buffer != NULL) {
            vsf_heap_free(dev->play.buffer[1].buffer);
            dev->play.buffer[1].buffer = NULL;
        }

        dev->ctx.err = VSF_ERR_NONE;
        vsf_eda_return();
        break;
    }
}
#endif

#endif
