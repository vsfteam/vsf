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

#ifndef __VSF_AUDIO_DUMMY_H__
#define __VSF_AUDIO_DUMMY_H__

/*============================ INCLUDES ======================================*/

#include "../../../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED && VSF_AUDIO_USE_DUMMY == ENABLED

#include "component/av/vsf_av.h"

#if     defined(__VSF_AUDIO_DUMMY_CLASS_IMPLEMENT)
#   undef __VSF_AUDIO_DUMMY_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_AUDIO_USE_PLAYBACK == ENABLED

typedef struct vk_audio_dummy_playback_buffer_t {
    vsf_callback_timer_t timer;
    void *param;
} vk_audio_dummy_playback_buffer_t;

typedef struct vk_audio_dummy_playback_ctx_t {
    vk_audio_stream_t *audio_stream;
    vk_audio_dummy_playback_buffer_t buffer[2];

    bool is_playing;
    bool fill_ticktock;
    uint8_t buffer_taken;
} vk_audio_dummy_playback_ctx_t;

#endif

vsf_class(vk_audio_dummy_dev_t) {
    public_member(
        implement(vk_audio_dev_t)
    )

    private_member(
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
        vk_audio_dummy_playback_ctx_t playback_ctx;
#endif
        vk_audio_stream_t __stream[
#if VSF_AUDIO_USE_PLAYBACK == ENABLED && VSF_AUDIO_USE_CAPTURE == ENABLED
            2
#else
            1
#endif
        ];
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_audio_drv_t vk_audio_dummy_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_AUDIO && VSF_AUDIO_USE_DUMMY
#endif      // __VSF_AUDIO_DUMMY_H__
