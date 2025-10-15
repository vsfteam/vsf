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

#ifndef __VSF_HAL_DISTBUS_SDIO_H__
#define __VSF_HAL_DISTBUS_SDIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SDIO == ENABLED && VSF_HAL_DISTBUS_USE_SDIO == ENABLED

#if     defined(__VSF_HAL_DISTBUS_SDIO_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_DISTBUS_SDIO_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_HAL_DISTBUS_SDIO_CFG_MULTI_CLASS
#   define VSF_HAL_DISTBUS_SDIO_CFG_MULTI_CLASS     VSF_SDIO_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if defined(__VSF_HAL_DISTBUS_SDIO_CLASS_IMPLEMENT) || defined(__VSF_HAL_DISTBUS_SDIO_CLASS_INHERIT__)
enum {
    VSF_HAL_DISTBUS_SDIO_CMD_ADDR_RANGE,
};
#endif

/*============================ INCLUDES ======================================*/

#ifdef VSF_HAL_DISTBUS_AS_REAL_DRIVER
#   include "hal/driver/common/template/vsf_template_sdio.h"
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_hal_distbus_sdio_t) {
#if VSF_HAL_DISTBUS_SDIO_CFG_MULTI_CLASS == ENABLED
    public_member(
        implement(vsf_sdio_t)
    )
#endif
    protected_member(
        vsf_distbus_service_t               service;
        struct {
            vsf_sdio_isr_handler_t          *handler;
            void                            *target;
            uint32_t                        enabled_mask;
            uint32_t                        triggered_mask;
            uint16_t                        no;
            uint32_t                        status;
            uint32_t                        resp[4];
        } irq;
    )
    private_member(
        vsf_distbus_t                       *distbus;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern uint32_t vsf_hal_distbus_sdio_register_service(vsf_distbus_t *distbus, vsf_hal_distbus_sdio_t *sdio, void *info, uint32_t infolen);

#ifdef __cplusplus
}
#endif

#undef __VSF_HAL_DISTBUS_SDIO_CLASS_IMPLEMENT
#undef __VSF_HAL_DISTBUS_SDIO_CLASS_INHERIT__

#endif
#endif
/* EOF */

