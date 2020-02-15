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

#ifndef __VSF_WINSOUND_H__
#define __VSF_WINSOUND_H__

/*============================ INCLUDES ======================================*/

#include "../../../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED && VSF_USE_WINSOUND == ENABLED

#include "component/av/vsf_av.h"

#if     defined(VSF_WINSOUND_IMPLEMENT)
#   undef VSF_WINSOUND_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_WINSOUND_INHERIT)
#   undef VSF_WINSOUND_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_winsound_dev_t)

#if VSF_AUDIO_CFG_USE_PLAY == ENABLED
struct vk_winsound_play_buffer_t {
    WAVEHDR header;
    uint8_t *buffer;
};
typedef struct vk_winsound_play_buffer_t vk_winsound_play_buffer_t;

struct vk_winsound_play_ctx_t {
    vsf_arch_irq_thread_t irq_thread;

    HWAVEOUT hwo;
    HANDLE hEvent;

    vk_winsound_play_buffer_t buffer[2];

    bool is_playing;
    bool fill_ticktock;
    bool play_ticktock;
    uint8_t buffer_taken;
};
typedef struct vk_winsound_play_ctx_t vk_winsound_play_ctx_t;
#endif

def_simple_class(vk_winsound_dev_t) {
    implement(vk_audio_dev_t)

    private_member(
        bool is_inited;
#if VSF_AUDIO_CFG_USE_PLAY == ENABLED
        vk_winsound_play_ctx_t play;
#endif
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_audio_drv_t vk_winsound_drv;

/*============================ PROTOTYPES ====================================*/

#endif      // VSF_USE_AUDIO && VSF_USE_WINSOUND
#endif      // __VSF_WINSOUND_H__
