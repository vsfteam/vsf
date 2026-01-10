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

#if VSF_HAL_USE_RTC == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_RTC_CLASS_IMPLEMENT
#include "./vsf_remapped_rtc.h"

#if VSF_RTC_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_RTC_CFG_MULTI_CLASS == ENABLED
const vsf_rtc_op_t vsf_remapped_rtc_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_RTC_APIS(vsf_remapped_rtc)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_rtc_init(vsf_remapped_rtc_t *rtc, vsf_rtc_cfg_t *cfg)
{
    VSF_HAL_ASSERT((rtc != NULL) && (rtc->target != NULL));
    return vsf_rtc_init(rtc->target, cfg);
}

void vsf_remapped_rtc_fini(vsf_remapped_rtc_t *rtc)
{
    VSF_HAL_ASSERT((rtc != NULL) && (rtc->target != NULL));
    vsf_rtc_fini(rtc->target);
}

vsf_err_t vsf_remapped_rtc_get_configuration(vsf_remapped_rtc_t *rtc, vsf_rtc_cfg_t *cfg)
{
    VSF_HAL_ASSERT((rtc != NULL) && (rtc->target != NULL));
    return vsf_rtc_get_configuration(rtc->target, cfg);
}

vsf_rtc_capability_t vsf_remapped_rtc_capability(vsf_remapped_rtc_t *rtc)
{
    VSF_HAL_ASSERT((rtc != NULL) && (rtc->target != NULL));
    return vsf_rtc_capability(rtc->target);
}

fsm_rt_t vsf_remapped_rtc_enable(vsf_remapped_rtc_t *rtc)
{
    VSF_HAL_ASSERT((rtc != NULL) && (rtc->target != NULL));
    return vsf_rtc_enable(rtc->target);
}

fsm_rt_t vsf_remapped_rtc_disable(vsf_remapped_rtc_t *rtc)
{
    VSF_HAL_ASSERT((rtc != NULL) && (rtc->target != NULL));
    return vsf_rtc_disable(rtc->target);
}

vsf_err_t vsf_remapped_rtc_get(vsf_remapped_rtc_t *rtc, vsf_rtc_tm_t *rtc_tm)
{
    VSF_HAL_ASSERT((rtc != NULL) && (rtc->target != NULL));
    return vsf_rtc_get(rtc->target, rtc_tm);
}

vsf_err_t vsf_remapped_rtc_set(vsf_remapped_rtc_t *rtc, const vsf_rtc_tm_t *rtc_tm)
{
    VSF_HAL_ASSERT((rtc != NULL) && (rtc->target != NULL));
    return vsf_rtc_set(rtc->target, rtc_tm);
}

vsf_err_t vsf_remapped_rtc_get_time(vsf_remapped_rtc_t *rtc, vsf_rtc_time_t *second, vsf_rtc_time_t *millisecond)
{
    VSF_HAL_ASSERT((rtc != NULL) && (rtc->target != NULL));
    return vsf_rtc_get_time(rtc->target, second, millisecond);
}

vsf_err_t vsf_remapped_rtc_set_time(vsf_remapped_rtc_t *rtc, vsf_rtc_time_t second, vsf_rtc_time_t millisecond)
{
    VSF_HAL_ASSERT((rtc != NULL) && (rtc->target != NULL));
    return vsf_rtc_set_time(rtc->target, second, millisecond);
}

vsf_err_t vsf_remapped_rtc_ctrl(vsf_remapped_rtc_t *rtc, vsf_rtc_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT((rtc != NULL) && (rtc->target != NULL));
    return vsf_rtc_ctrl(rtc->target, ctrl, param);
}

#endif
#endif
