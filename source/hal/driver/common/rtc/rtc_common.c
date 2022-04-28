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

#include "hal/driver/driver.h"

#if VSF_HAL_USE_RTC == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#if VSF_RTC_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_rtc_init(vsf_rtc_t *rtc_ptr, rtc_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->init != NULL);

    return rtc_ptr->op->init(rtc_ptr, cfg_ptr);
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

time_t vsf_rtc_get_second(vsf_rtc_t *rtc_ptr)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->get_second != NULL);

    return rtc_ptr->op->get_second(rtc_ptr);
}

vsf_err_t vsf_rtc_set_second(vsf_rtc_t *rtc_ptr, time_t time)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op != NULL);
    VSF_HAL_ASSERT(rtc_ptr->op->set_second != NULL);

    return rtc_ptr->op->set_second(rtc_ptr, time);
}

#endif /* VSF_RTC_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_RTC == ENABLED */
