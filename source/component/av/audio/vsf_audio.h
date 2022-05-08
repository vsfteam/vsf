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

#ifndef __VSF_AUDIO_H__
#define __VSF_AUDIO_H__

#include "../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED

#include "kernel/vsf_kernel.h"

#if     defined(__VSF_AUDIO_CLASS_IMPLEMENT)
#   undef __VSF_AUDIO_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_AUDIO_CLASS_INHERIT__)
#   undef __VSF_AUDIO_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_audio_stream_drv_t {
    vsf_peda_evthandler_t       control;
    vsf_peda_evthandler_t       start;
    vsf_peda_evthandler_t       stop;
} vk_audio_stream_drv_t;

vsf_class(vk_audio_drv_t) {
    protected_member(
        vsf_peda_evthandler_t   init;
    )
};

typedef union vk_audio_format_t {
    struct {
        uint8_t                 channel_num;        // eg: 2
        uint8_t                 sample_bit_width;   // eg: 16
        uint16_t                sample_rate;        // eg: 44100/48000
    } PACKED;
    // if value is 0, means adaptive format(from format in vk_audio_start)
    uint32_t                    value;
} vk_audio_format_t;

typedef struct vk_audio_stream_t {
    uint8_t                     stream_index;       // index start from 0
    uint8_t                     dir_in1out0;        // 1 for capture, 0 for playback

    vk_audio_format_t           format;
    const vk_audio_stream_drv_t *drv;
    vsf_stream_t                *stream;
} vk_audio_stream_t;

vsf_class(vk_audio_dev_t) {
    public_member(
        const vk_audio_drv_t    *drv;
        vsf_arch_prio_t         hw_prio;
        uint8_t                 stream_num;
        vk_audio_stream_t       stream[0];
    )
};

__vsf_component_peda_ifs(vk_audio_init)
__vsf_component_peda_ifs(vk_audio_control,
    vk_audio_stream_t           *audio_stream;
    vk_av_control_type_t        type;
    vk_av_control_value_t       value;
)
__vsf_component_peda_ifs(vk_audio_start,
    vk_audio_stream_t           *audio_stream;
)
__vsf_component_peda_ifs(vk_audio_stop,
    vk_audio_stream_t           *audio_stream;
)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_audio_init(vk_audio_dev_t *pthis);

extern vsf_err_t vk_audio_control(vk_audio_dev_t *pthis, uint_fast8_t stream_idx,
            vk_av_control_type_t type, vk_av_control_value_t value);
extern vsf_err_t vk_audio_start(vk_audio_dev_t *pthis, uint_fast8_t stream_idx,
            vsf_stream_t *stream, vk_audio_format_t *format);
extern vsf_err_t vk_audio_stop(vk_audio_dev_t *pthis, uint_fast8_t stream_idx);

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#if VSF_AUDIO_USE_DECODER_WAV == ENABLED
#   include "./decoder/wav/vsf_wav.h"
#endif

#endif      // VSF_USE_AUDIO
#endif      // __VSF_AUDIO_H__
