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

/*============================ INCLUDES ======================================*/

#include "../../../vsf_av_cfg.h"
// for VSF_HAL_USE_I2S
#include "hal/vsf_hal.h"

#if VSF_USE_AUDIO == ENABLED && VSF_HAL_USE_I2S == ENABLED

#define __VSF_AUDIO_CLASS_INHERIT__
#define __VSF_AUDIO_I2S_CLASS_IMPLEMENT
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__

#include "service/vsf_service.h"
#include "component/av/vsf_av.h"
#include "./vsf_audio_i2s.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t __vk_audio_i2s_init(__vk_audio_i2s_dev_t *audio_i2s, vsf_i2s_cfg_t *cfg)
{
    if (cfg != NULL) {
        if ((cfg->data_sample_rate != 0) && (cfg->data_sample_rate != cfg->hw_sample_rate)) {
            // make sure src(sample rate converter) is supported
            vsf_i2s_capability_t capability = vsf_i2s_capability(audio_i2s->i2s);
            if (!capability.i2s_capability.is_src_supported) {
                return VSF_ERR_NOT_SUPPORT;
            }
        }
        cfg->feature |= audio_i2s->i2s_feature;
    }
    return vsf_i2s_init(audio_i2s->i2s, cfg);
}

vsf_err_t __vk_audio_i2s_rx_init(__vk_audio_i2s_dev_t *audio_i2s, vsf_i2s_cfg_t *cfg)
{
    return vsf_i2s_rx_init(audio_i2s->i2s, cfg);
}

vsf_err_t __vk_audio_i2s_tx_init(__vk_audio_i2s_dev_t *audio_i2s, vsf_i2s_cfg_t *cfg)
{
    return vsf_i2s_tx_init(audio_i2s->i2s, cfg);
}

#endif
