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

#include "hal/vsf_hal.h"

#if VSF_HAL_USE_WDT == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_WDT_CLASS_IMPLEMENT
#include "./vsf_remapped_wdt.h"

#if VSF_WDT_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_WDT_CFG_MULTI_CLASS == ENABLED
const vsf_wdt_op_t vsf_remapped_wdt_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_WDT_APIS(vsf_remapped_wdt)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_wdt_init(vsf_remapped_wdt_t *wdt, vsf_wdt_cfg_t *cfg)
{
    VSF_HAL_ASSERT((wdt != NULL) && (wdt->target != NULL));
    return vsf_wdt_init(wdt->target, cfg);
}

void vsf_remapped_wdt_fini(vsf_remapped_wdt_t *wdt)
{
    VSF_HAL_ASSERT((wdt != NULL) && (wdt->target != NULL));
    vsf_wdt_fini(wdt->target);
}

vsf_err_t vsf_remapped_wdt_get_configuration(vsf_remapped_wdt_t *wdt, vsf_wdt_cfg_t *cfg)
{
    VSF_HAL_ASSERT((wdt != NULL) && (wdt->target != NULL));
    return vsf_wdt_get_configuration(wdt->target, cfg);
}

vsf_wdt_capability_t vsf_remapped_wdt_capability(vsf_remapped_wdt_t *wdt)
{
    VSF_HAL_ASSERT((wdt != NULL) && (wdt->target != NULL));
    return vsf_wdt_capability(wdt->target);
}

fsm_rt_t vsf_remapped_wdt_enable(vsf_remapped_wdt_t *wdt)
{
    VSF_HAL_ASSERT((wdt != NULL) && (wdt->target != NULL));
    return vsf_wdt_enable(wdt->target);
}

fsm_rt_t vsf_remapped_wdt_disable(vsf_remapped_wdt_t *wdt)
{
    VSF_HAL_ASSERT((wdt != NULL) && (wdt->target != NULL));
    return vsf_wdt_disable(wdt->target);
}

void vsf_remapped_wdt_feed(vsf_remapped_wdt_t *wdt)
{
    VSF_HAL_ASSERT((wdt != NULL) && (wdt->target != NULL));
    vsf_wdt_feed(wdt->target);
}

#endif
#endif
