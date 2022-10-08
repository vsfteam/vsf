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

#ifndef __VSF_HW_FLASH_MAL_H__
#define __VSF_HW_FLASH_MAL_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_HW_FLASH_MAL == ENABLED && VSF_HAL_USE_FLASH == ENABLED

#include "hal/vsf_hal.h"

#if     defined(__VSF_HW_FLASH_MAL_CLASS_IMPLEMENT)
#   undef __VSF_HW_FLASH_MAL_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_hw_flash_mal_t) {
    implement(vk_mal_t)

    vsf_flash_capability_t cap;

    public_member(
        vsf_hw_flash_t *flash;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_mal_drv_t vk_hw_flash_mal_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_MAL && VSF_MAL_USE_HW_FLASH_MAL && VSF_HAL_USE_FLASH
#endif      // __VSF_HW_FLASH_MAL_H__
