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

#ifndef __VSF_AUDIO_I2S_H__
#define __VSF_AUDIO_I2S_H__

/*============================ INCLUDES ======================================*/

#include "../../../vsf_av_cfg.h"
// for VSF_HAL_USE_I2S
#include "hal/vsf_hal.h"

#if VSF_USE_AUDIO == ENABLED && VSF_HAL_USE_I2S == ENABLED

#include "component/av/vsf_av.h"

#if     defined(__VSF_AUDIO_I2S_CLASS_IMPLEMENT)
#   undef __VSF_AUDIO_I2S_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_AUDIO_I2S_CLASS_INHERIT__)
#   undef __VSF_AUDIO_I2S_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_audio_i2s_dev_t) {
    public_member(
        implement(vk_audio_dev_t)
        vsf_i2s_t           *i2s;
        vsf_i2s_feature_t   i2s_feature;
        vsf_arch_prio_t     arch_prio;
    )
    protected_member(
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
        struct {
            bool stream_started;
            uint8_t channel_num;
            uint8_t sample_bitlen;
            uint16_t sample_rate;
            uint16_t hw_sample_rate;
        } playback;
#endif
#if VSF_AUDIO_USE_CAPTURE == ENABLED
        struct {
            bool stream_started;
            uint8_t channel_num;
            uint8_t sample_bitlen;
            uint16_t sample_rate;
            uint16_t hw_sample_rate;
        } capture;
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

extern const vk_audio_drv_t vk_audio_i2s_drv;
extern const vk_audio_stream_drv_t vk_audio_i2s_stream_drv_playback;
extern const vk_audio_stream_drv_t vk_audio_i2s_stream_drv_capture;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_AUDIO && VSF_HAL_USE_I2S
#endif      // __VSF_AUDIO_I2S_H__
