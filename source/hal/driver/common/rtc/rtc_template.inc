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

#if VSF_HAL_USE_RTC == ENABLED

/*============================ INCLUDES ======================================*/

#if VSF_RTC_CFG_IMPLEMENT_TM_TO_TIME == ENABLED
#   include <time.h>
#endif

/*============================ MACROS ========================================*/

#define vsf_real_rtc_t          VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t)
#define vsf_real_rtc_capability VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_capability)
#define vsf_real_rtc_get        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_get)
#define vsf_real_rtc_set        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_set)
#define vsf_real_rtc_get_time   VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_get_time)
#define vsf_real_rtc_set_time   VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_set_time)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_RTC_CFG_IMPLEMENT_PWM_SET == DISABLED
vsf_err_t vsf_real_rtc_pwm_set(vsf_real_rtc_t *rtc_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != rtc_ptr);

    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}
#endif

#if VSF_RTC_CFG_REIMPLEMENT_CAPABILITY == DISABLED
rtc_capability_t vsf_real_rtc_capability(vsf_real_rtc_t *rtc_ptr)
{
    rtc_capability_t rtc_capability = {
        0,
    };

    return rtc_capability;
}
#endif

#if VSF_RTC_CFG_IMPLEMENT_TIME_TO_TM == ENABLED
vsf_err_t vsf_real_rtc_get(vsf_real_rtc_t *rtc_ptr, vsf_rtc_tm_t *rtc_tm)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_tm != NULL);

    time_t second, milliseconds;
    vsf_err_t result = vsf_real_rtc_get_time(rtc_ptr, &second, &milliseconds);
    if (result != VSF_ERR_NONE) {
        return result;
    }

    struct tm *t = gmtime(&second);
    if (t == NULL) {
        return VSF_ERR_FAIL;
    }

    rtc_tm->tm_sec  = t->tm_sec;
    rtc_tm->tm_min  = t->tm_min;
    rtc_tm->tm_hour = t->tm_hour;
    rtc_tm->tm_mday = t->tm_mday;
    rtc_tm->tm_wday = t->tm_wday + 1;
    rtc_tm->tm_mon  = t->tm_mon  + 1;
    rtc_tm->tm_year = t->tm_year + 1900;
    rtc_tm->tm_ms   = milliseconds;

    return VSF_ERR_NONE;
}

vsf_err_t vsf_real_rtc_set(vsf_real_rtc_t *rtc_ptr, const vsf_rtc_tm_t *rtc_tm)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT(rtc_tm != NULL);

    struct tm t = {
        .tm_sec  = rtc_tm->tm_sec,
        .tm_min  = rtc_tm->tm_min,
        .tm_hour = rtc_tm->tm_hour,
        .tm_mday = rtc_tm->tm_mday,
        .tm_mon  = rtc_tm->tm_mon - 1,
        .tm_year = rtc_tm->tm_year - 1900,
    };

    time_t second = mktime(&t);
    return vsf_real_rtc_set_time(rtc_ptr, second,  rtc_tm->tm_ms);
}
#endif

#if VSF_RTC_CFG_IMPLEMENT_TM_TO_TIME == ENABLED
vsf_err_t vsf_real_rtc_get_time(vsf_hw_rtc_t *rtc_ptr, time_t *second_ptr, time_t *milliseconds_ptr)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);
    VSF_HAL_ASSERT((second_ptr != NULL) || (milliseconds_ptr != NULL));

    vsf_rtc_tm_t rtc_tm;
    vsf_err_t result = vsf_real_rtc_get(rtc_ptr, &rtc_tm);
    if (result != VSF_ERR_NONE) {
        return result;
    }

    if (second_ptr != NULL) {
        struct tm t = {
            .tm_sec  = rtc_tm.tm_sec,
            .tm_min  = rtc_tm.tm_min,
            .tm_hour = rtc_tm.tm_hour,
            .tm_mday = rtc_tm.tm_mday,
            .tm_mon  = rtc_tm.tm_mon - 1,
            .tm_year = rtc_tm.tm_year - 1900,
        };
        *second_ptr = mktime(&t);
    }

    if (milliseconds_ptr != NULL) {
        *milliseconds_ptr = rtc_tm.tm_ms;
    }

    return VSF_ERR_NONE;
}

vsf_err_t vsf_real_rtc_set_second(vsf_real_rtc_t *rtc_ptr, time_t second, time_t milliseconds)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);

    struct tm *t = gmtime(&second);
    if (NULL == t) {
        return VSF_ERR_FAIL;
    }

    vsf_rtc_tm_t rtc_tm = {
        .tm_sec  = t->tm_sec,
        .tm_min  = t->tm_min,
        .tm_hour = t->tm_hour,
        .tm_mday = t->tm_mday,
        .tm_mon  = t->tm_mon + 1,
        .tm_year = t->tm_year + 1900,
        .tm_ms   = milliseconds,
    };

    return vsf_real_rtc_set(rtc_ptr, &rtc_tm);
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     _rtc
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _RTC

#ifndef VSF_RTC_CFG_PREFIX
#   error "Please define VSF_RTC_CFG_PREFIX in rtc driver"
#endif

#ifndef VSF_RTC_CFG_UPPERCASE_PREFIX
#   error "Please define VSF_RTC_CFG_UPPERCASE_PREFIX in rtc driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif  /* VSF_HAL_USE_RTC */
