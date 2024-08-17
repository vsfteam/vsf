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

#ifndef __VSF_SDMMC_MAL_H__
#define __VSF_SDMMC_MAL_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_SDMMC_MAL == ENABLED && VSF_HAL_USE_SDIO == ENABLED

#include "hal/vsf_hal.h"
#include "kernel/vsf_kernel.h"

#if     defined(__VSF_SDMMC_MAL_CLASS_IMPLEMENT)
#   undef __VSF_SDMMC_MAL_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER != ENABLED
#   error Please enable VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_sdmmc_mal_t) {
    public_member(
        implement(vk_mal_t)
        vsf_sdio_t             *sdio;
        vsf_arch_prio_t         hw_priority;
        implement(vsf_sdio_probe_t)
    )
    private_member(
        vsf_callback_timer_t    timer;
        vsf_eda_t              *eda;
        bool                    is_probing;
        bool                    is_stopping;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_mal_drv_t vk_sdmmc_mal_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_MAL && VSF_MAL_USE_SDMMC_MAL && VSF_HAL_USE_SDIO
#endif      // __VSF_SDMMC_MAL_H__
