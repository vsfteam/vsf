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

#ifndef __VSF_AIC1000A_H__
#define __VSF_AIC1000A_H__

/*============================ INCLUDES ======================================*/

#include "../../../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED && VSF_AUDIO_USE_AIC1000A == ENABLED

#include "component/av/vsf_av.h"
#include "hal/vsf_hal.h"

#if     defined(__VSF_AIC1000A_CLASS_IMPLEMENT)
#   undef __VSF_AIC1000A_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_aic1000a_dev_t) {
    public_member(
        implement(vk_audio_i2s_dev_t)
        vsf_gpio_t *pwrkey_port;
        vsf_gpio_t *psi_port;
        uint8_t pwrkey_pin;
        uint8_t psi_clk_pin;
        uint8_t psi_dat_pin;
    )

    private_member(
        bool is_inited;

        struct {
            bool is_started;

            bool mem_cleared;
            uint8_t ch_en;
            uint8_t ch_ana_pu;
        } dac;
        struct {
            bool is_started;

            bool mem_cleared;
            uint8_t ch_en;
            uint8_t ch_ana_pu;
            uint8_t ch_d36_en;
            uint8_t ch_d36_d3;
            uint8_t mic_matrix_type;
        } adc;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_audio_drv_t vk_aic1000a_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_AUDIO && VSF_AUDIO_USE_AIC1000A
#endif      // __VSF_AIC1000A_H__
