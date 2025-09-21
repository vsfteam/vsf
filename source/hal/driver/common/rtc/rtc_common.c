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

#define VSF_RTC_CFG_FUNCTION_RENAME DISABLED

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_RTC == ENABLED

#include "hal/driver/driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_RTC_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_rtc_init(vsf_rtc_t *rtc_ptr, vsf_rtc_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->init != NULL);

    return rtc_ptr->op->init(rtc_ptr, cfg_ptr);
}

void vsf_rtc_fini(vsf_rtc_t *rtc_ptr)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->init != NULL);

    rtc_ptr->op->fini(rtc_ptr);
}

fsm_rt_t vsf_rtc_enable(vsf_rtc_t *rtc_ptr)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->enable != NULL);

    return rtc_ptr->op->enable(rtc_ptr);
}

fsm_rt_t vsf_rtc_disable(vsf_rtc_t *rtc_ptr)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->disable != NULL);

    return rtc_ptr->op->disable(rtc_ptr);
}

vsf_err_t vsf_rtc_get(vsf_rtc_t *rtc_ptr, vsf_rtc_tm_t *rtc_tm)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->get != NULL);

    return rtc_ptr->op->get(rtc_ptr, rtc_tm);
}

vsf_err_t vsf_rtc_set(vsf_rtc_t *rtc_ptr, const vsf_rtc_tm_t *rtc_tm)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->set != NULL);

    return rtc_ptr->op->set(rtc_ptr, rtc_tm);
}

vsf_err_t vsf_rtc_get_time(vsf_rtc_t *rtc_ptr, vsf_rtc_time_t *second_ptr, vsf_rtc_time_t *millisecond_ptr)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->get_time != NULL);

    return rtc_ptr->op->get_time(rtc_ptr, second_ptr, millisecond_ptr);
}

vsf_err_t vsf_rtc_set_time(vsf_rtc_t *rtc_ptr, vsf_rtc_time_t second, vsf_rtc_time_t millisecond)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->set_time != NULL);

    return rtc_ptr->op->set_time(rtc_ptr, second, millisecond);
}

vsf_rtc_capability_t vsf_rtc_capability(vsf_rtc_t *rtc_ptr)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->capability != NULL);

    return rtc_ptr->op->capability(rtc_ptr);
}

vsf_err_t vsf_rtc_get_configuration(vsf_rtc_t *rtc_ptr, vsf_rtc_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->get_configuration != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return rtc_ptr->op->get_configuration(rtc_ptr, cfg_ptr);
}

#endif /* VSF_RTC_CFG_MULTI_CLASS == ENABLED */


static bool _vsf_rtc_is_valid(const vsf_rtc_tm_t *rtc_tm)
{
    VSF_HAL_ASSERT(rtc_tm != NULL);

    if (rtc_tm->tm_sec > 59) {
        return false;
    }
    if (rtc_tm->tm_min > 59) {
        return false;
    }
    if (rtc_tm->tm_hour > 23) {
        return false;
    }
    if ((rtc_tm->tm_mday < 1) || (rtc_tm->tm_mday > 23)) {
        return false;
    }
    if ((rtc_tm->tm_mday < 1) || (rtc_tm->tm_mday > 7)) {
        return false;
    }
    if ((rtc_tm->tm_mon < 1) || (rtc_tm->tm_mon > 12)) {
        return false;
    }
    if (rtc_tm->tm_year < 1900) {
        return false;
    }

    return true;
}

bool vsf_rtc_tm_is_valid(const vsf_rtc_tm_t *rtc_tm)
{
    VSF_HAL_ASSERT(rtc_tm != NULL);

    if (rtc_tm->tm_year < 1900) {
        return false;
    }

    return _vsf_rtc_is_valid(rtc_tm);
}

bool vsf_rtc_tm_is_epoch_time(const vsf_rtc_tm_t *rtc_tm)
{
    VSF_HAL_ASSERT(rtc_tm != NULL);

    if (rtc_tm->tm_year < 1970) {
        return false;
    }

    return _vsf_rtc_is_valid(rtc_tm);
}


#endif /* VSF_HAL_USE_RTC == ENABLED */
