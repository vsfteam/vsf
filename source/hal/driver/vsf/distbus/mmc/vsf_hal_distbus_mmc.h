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

#ifndef __VSF_HAL_DISTBUS_MMC_H__
#define __VSF_HAL_DISTBUS_MMC_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_HAL_USE_MMC == ENABLED && VSF_HAL_DISTBUS_USE_MMC == ENABLED

#include "hal/driver/common/template/vsf_template_mmc.h"

#if     defined(__VSF_HAL_DISTBUS_MMC_CLASS_IMPLEMENT)
#   undef __VSF_HAL_DISTBUS_MMC_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_hal_distbus_mmc_t) {
    protected_member(
        vsf_distbus_service_t               service;
    )
    private_member(
        vsf_distbus_t                       *distbus;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_hal_distbus_mmc_register_service(vsf_distbus_t *distbus, vsf_hal_distbus_mmc_t *mmc);

/*============================ INCLUDES ======================================*/

#define VSF_MMC_CFG_DEC_PREFIX              vsf_hal_distbus
#define VSF_MMC_CFG_DEC_UPCASE_PREFIX       VSF_HAL_DISTBUS
#include "hal/driver/common/mmc/mmc_template.h"

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */

