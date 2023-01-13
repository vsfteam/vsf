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
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __vk_audio_i2s_init(vk_audio_i2s_dev_t *audio_i2s, vsf_i2s_cfg_t *cfg);

static inline vsf_err_t __vk_audio_i2s_rx_init(vk_audio_i2s_dev_t *audio_i2s, vsf_i2s_cfg_t *cfg)
{
    return vsf_i2s_rx_init(audio_i2s->i2s, cfg);
}

static inline void __vk_audio_i2s_rx_fini(vk_audio_i2s_dev_t *audio_i2s)
{
    vsf_i2s_rx_fini(audio_i2s->i2s);
}

static inline vsf_err_t __vk_audio_i2s_rx_start(vk_audio_i2s_dev_t *audio_i2s)
{
    return vsf_i2s_rx_start(audio_i2s->i2s);
}

static inline vsf_err_t __vk_audio_i2s_rx_pause(vk_audio_i2s_dev_t *audio_i2s)
{
    return vsf_i2s_rx_pause(audio_i2s->i2s);
}

static inline vsf_err_t __vk_audio_i2s_rx_resume(vk_audio_i2s_dev_t *audio_i2s)
{
    return vsf_i2s_rx_resume(audio_i2s->i2s);
}

static inline vsf_err_t __vk_audio_i2s_tx_init(vk_audio_i2s_dev_t *audio_i2s, vsf_i2s_cfg_t *cfg)
{
    return vsf_i2s_tx_init(audio_i2s->i2s, cfg);
}

static inline void __vk_audio_i2s_tx_fini(vk_audio_i2s_dev_t *audio_i2s)
{
    vsf_i2s_tx_fini(audio_i2s->i2s);
}

static inline vsf_err_t __vk_audio_i2s_tx_start(vk_audio_i2s_dev_t *audio_i2s)
{
    return vsf_i2s_tx_start(audio_i2s->i2s);
}

static inline vsf_err_t __vk_audio_i2s_tx_pause(vk_audio_i2s_dev_t *audio_i2s)
{
    return vsf_i2s_tx_pause(audio_i2s->i2s);
}

static inline vsf_err_t __vk_audio_i2s_tx_resume(vk_audio_i2s_dev_t *audio_i2s)
{
    return vsf_i2s_tx_resume(audio_i2s->i2s);
}

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_AUDIO && VSF_HAL_USE_I2S
#endif      // __VSF_AUDIO_I2S_H__
