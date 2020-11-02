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

#if     defined(__VSF_AUDIO_CLASS_IMPLEMENT)
#   undef __VSF_AUDIO_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_AUDIO_CLASS_INHERIT__)
#   undef __VSF_AUDIO_CLASS_INHERIT__
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_audio_dev_t)
dcl_simple_class(vk_audio_drv_t)

typedef struct vk_audio_play_drv_t {
    vsf_peda_evthandler_t volume;
    vsf_peda_evthandler_t mute;
    vsf_peda_evthandler_t play;
    vsf_peda_evthandler_t stop;
} vk_audio_play_drv_t;

typedef struct vk_audio_capture_drv_t {
    vsf_peda_evthandler_t volume;
    vsf_peda_evthandler_t mute;
    vsf_peda_evthandler_t capture;
    vsf_peda_evthandler_t stop;
} vk_audio_capture_drv_t;

def_simple_class(vk_audio_drv_t) {
    protected_member(
        vsf_peda_evthandler_t init;
#if VSF_AUDIO_USE_PLAY == ENABLED
        const vk_audio_play_drv_t play_drv;
#endif
#if VSF_AUDIO_CFG_USE_CATURE == ENABLED
        const vk_audio_capture_drv_t capture_drv;
#endif
    )
};

typedef struct vk_audio_format_t {
    uint8_t channel_num;        // eg: 2
    uint8_t sample_bit_width;   // eg: 16
    uint16_t sample_rate;       // eg: 44100/48000
} vk_audio_format_t;

typedef struct vk_audio_stream_t {
    vk_audio_format_t format;
    vsf_stream_t *stream;
} vk_audio_stream_t;

def_simple_class(vk_audio_dev_t) {
    public_member(
        const vk_audio_drv_t *drv;
        vsf_arch_prio_t hw_prio;
    )
    protected_member(
#if VSF_AUDIO_USE_PLAY == ENABLED
        vk_audio_stream_t play;
#endif
#if VSF_AUDIO_USE_CAPTURE == ENABLED
        vk_audio_stream_t capture;
#endif
    )
};

__vsf_component_peda_ifs(vk_audio_init)

#if VSF_AUDIO_USE_PLAY == ENABLED
__vsf_component_peda_ifs(vk_audio_play_set_volume,
    uint16_t volume;
)
__vsf_component_peda_ifs(vk_audio_play_set_mute,
    bool mute;
)
__vsf_component_peda_ifs(vk_audio_play_start)
__vsf_component_peda_ifs(vk_audio_play_stop)
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

vsf_err_t vk_audio_init(vk_audio_dev_t *pthis);

#if VSF_AUDIO_USE_PLAY == ENABLED
vsf_err_t vk_audio_play_set_volume(vk_audio_dev_t *pthis, uint_fast16_t volume);
vsf_err_t vk_audio_play_set_mute(vk_audio_dev_t *pthis, bool mute);
vsf_err_t vk_audio_play_start(vk_audio_dev_t *pthis, vsf_stream_t *stream, vk_audio_format_t *format);
vsf_err_t vk_audio_play_pause(vk_audio_dev_t *pthis);
vsf_err_t vk_audio_play_stop(vk_audio_dev_t *pthis);
#endif

#if VSF_AUDIO_USE_CAPTURE == ENABLED
vsf_err_t vk_audio_capture_set_volume(vk_audio_dev_t *pthis, uint_fast16_t volume);
vsf_err_t vk_audio_capture_set_mute(vk_audio_dev_t *pthis, bool mute);
vsf_err_t vk_audio_capture_start(vk_audio_dev_t *pthis, vsf_stream_t *stream);
vsf_err_t vk_audio_capture_pause(vk_audio_dev_t *pthis);
vsf_err_t vk_audio_capture_stop(vk_audio_dev_t *pthis);
#endif

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#include "./decoder/wav/vsf_wav.h"

#endif      // VSF_USE_AUDIO
#endif      // __VSF_AUDIO_H__
