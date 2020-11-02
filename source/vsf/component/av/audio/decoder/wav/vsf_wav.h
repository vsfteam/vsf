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

#ifndef __VSF_WAV_H__
#define __VSF_WAV_H__

#include "../../../vsf_av_cfg.h"

#if VSF_AUDIO_USE_DECODER_WAV == ENABLED

#include "kernel/vsf_kernel.h"
#include "component/av/audio/vsf_audio.h"

#if     defined(__VSF_WAV_CLASS_IMPLEMENT)
#   undef __VSF_WAV_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_wav_t)

typedef enum vk_wav_state_t {
    VSF_WAV_STATE_RIFF,
    VSF_WAV_STATE_FORMAT,
    VSF_WAV_STATE_DATA,
    VSF_WAV_STATE_PLAY,
} vk_wav_state_t;

def_simple_class(vk_wav_t) {
    public_member(
        vk_audio_dev_t      *audio_dev;
        vsf_stream_t        *stream;
        vsf_err_t           result;
    )
    private_member(
        vsf_eda_t           eda;
        vk_audio_format_t   format;
        vk_wav_state_t      state;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_wav_play_start(vk_wav_t *wav);
extern vsf_err_t vk_wav_play_stop(vk_wav_t *wav);

#ifdef __cplusplus
}
#endif

#endif      // VSF_AUDIO_USE_DECODER_WAV
#endif      // __VSF_WAV_H__
