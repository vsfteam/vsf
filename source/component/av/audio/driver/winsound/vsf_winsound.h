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

#ifndef __VSF_WINSOUND_H__
#define __VSF_WINSOUND_H__

/*============================ INCLUDES ======================================*/

#include "../../../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED && VSF_USE_WINSOUND == ENABLED

#include "component/av/vsf_av.h"

#if     defined(__VSF_WINSOUND_CLASS_IMPLEMENT)
#   undef __VSF_WINSOUND_CLASS_IMPLEMENT
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

// avoid to use windows.h, fix if any conflicts
#define VSF_WIN_HWAVEOUT            VSF_WIN_HANDLE
/* wave data block header */
typedef struct __vsf_win_wavehdr_tag {
    VSF_WIN_LPSTR       lpData;                 /* pointer to locked data buffer */
    VSF_WIN_DWORD       dwBufferLength;         /* length of data buffer */
    VSF_WIN_DWORD       dwBytesRecorded;        /* used for input only */
    VSF_WIN_DWORD_PTR   dwUser;                 /* for client's use */
    VSF_WIN_DWORD       dwFlags;                /* assorted flags (see defines) */
    VSF_WIN_DWORD       dwLoops;                /* loop control counter */
    struct __vsf_win_wavehdr_tag FAR *lpNext;   /* reserved for driver */
    VSF_WIN_DWORD_PTR   reserved;               /* reserved for driver */
} __VSF_WIN_WAVEHDR;

typedef struct vk_winsound_playback_buffer_t {
    __VSF_WIN_WAVEHDR header;
    uint8_t *buffer;
} vk_winsound_playback_buffer_t;

typedef struct vk_winsound_playback_ctx_t {
    vsf_arch_irq_thread_t irq_thread;
    vk_audio_stream_t *audio_stream;

    VSF_WIN_HWAVEOUT hwo;
    VSF_WIN_HANDLE hEvent;

    vk_winsound_playback_buffer_t buffer[2];

    bool is_playing;
    bool fill_ticktock;
    bool play_ticktock;
    uint8_t buffer_taken;
} vk_winsound_playback_ctx_t;

#undef HWAVEOUT
#endif

vsf_class(vk_winsound_dev_t) {
    private_member(
        bool is_inited;
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
        vk_winsound_playback_ctx_t playback_ctx;
#endif
    )

    public_member(
        implement(vk_audio_dev_t)
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

extern const vk_audio_drv_t vk_winsound_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_AUDIO && VSF_USE_WINSOUND
#endif      // __VSF_WINSOUND_H__
