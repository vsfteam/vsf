/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                  *
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

#define VSF_WDT_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_WDT == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#if VSF_WDT_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_wdt_init(vsf_wdt_t *wdt_ptr, vsf_wdt_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op->init != NULL);

    return wdt_ptr->op->init(wdt_ptr, cfg_ptr);
}

void vsf_wdt_fini(vsf_wdt_t *wdt_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op->init != NULL);

    wdt_ptr->op->fini(wdt_ptr);
}

vsf_err_t vsf_wdt_get_configuration(vsf_wdt_t *wdt_ptr, vsf_wdt_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op->get_configuration != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return wdt_ptr->op->get_configuration(wdt_ptr, cfg_ptr);
}

fsm_rt_t vsf_wdt_enable(vsf_wdt_t *wdt_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op->enable != NULL);

    return wdt_ptr->op->enable(wdt_ptr);
}

fsm_rt_t vsf_wdt_disable(vsf_wdt_t *wdt_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op->disable != NULL);

    return wdt_ptr->op->disable(wdt_ptr);
}

vsf_wdt_capability_t vsf_wdt_capability(vsf_wdt_t *wdt_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op->capability != NULL);

    return wdt_ptr->op->capability(wdt_ptr);
}

void vsf_wdt_feed(vsf_wdt_t *wdt_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op != NULL);
    VSF_HAL_ASSERT(wdt_ptr->op->feed != NULL);

    wdt_ptr->op->feed(wdt_ptr);
}

#endif /* VSF_WDT_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_WDT == ENABLED */
