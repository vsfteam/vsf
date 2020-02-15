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

#ifndef __VSF_AUDIO_H__
#define __VSF_AUDIO_H__

#include "../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED

#include "kernel/vsf_kernel.h"

#if     defined(VSF_AUDIO_IMPLEMENT)
#   undef VSF_AUDIO_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_AUDIO_INHERIT)
#   undef VSF_AUDIO_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_audio_dev_t)

struct vk_audio_play_drv_t {
    void (*volume)(uintptr_t, vsf_evt_t);
    void (*mute)(uintptr_t, vsf_evt_t);
    void (*play)(uintptr_t, vsf_evt_t);
    void (*stop)(uintptr_t, vsf_evt_t);
};
typedef struct vk_audio_play_drv_t vk_audio_play_drv_t;

struct vk_audio_capture_drv_t {
    void (*volume)(uintptr_t, vsf_evt_t);
    void (*mute)(uintptr_t, vsf_evt_t);
    void (*capture)(uintptr_t, vsf_evt_t);
    void (*stop)(uintptr_t, vsf_evt_t);
};
typedef struct vk_audio_capture_drv_t vk_audio_capture_drv_t;

struct vk_audio_drv_t {
    void (*init)(uintptr_t, vsf_evt_t);
#if VSF_AUDIO_CFG_USE_PLAY == ENABLED
    const vk_audio_play_drv_t play_drv;
#endif
#if VSF_AUDIO_CFG_USE_CATURE == ENABLED
    const vk_audio_capture_drv_t capture_drv;
#endif
};
typedef struct vk_audio_drv_t vk_audio_drv_t;

struct vk_audio_format_t {
    uint8_t channel_num;        // eg: 2
    uint8_t sample_bit_width;   // eg: 16
    uint16_t sample_rate;       // eg: 44100/48000
};
typedef struct vk_audio_format_t vk_audio_format_t;

def_simple_class(vk_audio_dev_t) {
    public_member(
        const vk_audio_drv_t *drv;
        vsf_arch_prio_t hw_prio;
    )
    protected_member(
        struct {
            vsf_err_t err;
            union {
                uint16_t volume;
                bool mute;
                vk_audio_format_t format;
            } param;
        } ctx;

        vsf_stream_t *stream_play;
        vsf_stream_t *stream_capture;
    )
};
typedef struct vk_audio_dev_t vk_audio_dev_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

vsf_err_t vk_audio_init(vk_audio_dev_t *pthis);

#if VSF_AUDIO_CFG_USE_PLAY == ENABLED
vsf_err_t vk_audio_play_set_volume(vk_audio_dev_t *pthis, uint_fast16_t volume);
vsf_err_t vk_audio_play_set_mute(vk_audio_dev_t *pthis, bool mute);
vsf_err_t vk_audio_play_start(vk_audio_dev_t *pthis, vsf_stream_t *stream, vk_audio_format_t *format);
vsf_err_t vk_audio_play_pause(vk_audio_dev_t *pthis);
vsf_err_t vk_audio_play_stop(vk_audio_dev_t *pthis);
#endif

#if VSF_AUDIO_CFG_USE_CAPTURE == ENABLED
vsf_err_t vk_audio_capture_set_volume(vk_audio_dev_t *pthis, uint_fast16_t volume);
vsf_err_t vk_audio_capture_set_mute(vk_audio_dev_t *pthis, bool mute);
vsf_err_t vk_audio_capture_start(vk_audio_dev_t *pthis, vsf_stream_t *stream);
vsf_err_t vk_audio_capture_pause(vk_audio_dev_t *pthis);
vsf_err_t vk_audio_capture_stop(vk_audio_dev_t *pthis);
#endif

/*============================ INCLUDES ======================================*/

#if VSF_USE_DECODER_WAV == ENABLED
#   include "./decoder/wav/vsf_wav.h"
#endif

#endif      // VSF_USE_AUDIO
#endif      // __VSF_AUDIO_H__
