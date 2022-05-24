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

#define VSF_RTC_CFG_PREFIX                    vsf_hw
#define VSF_RTC_CFG_UPPERCASE_PREFIX          VSF_HW

/*============================ INCLUDES ======================================*/

#include "./rtc.h"

#if VSF_HAL_USE_RTC == ENABLED


#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/pmic/pmic_api.h"
#include "hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/time/time_api.h"
#include "../__device.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_rtc_t {
#if VSF_RTC_CFG_IMPLEMENT_OP == ENABLED
    vsf_rtc_t vsf_rtc;
#endif

    uint32_t freq;
    rtc_cfg_t cfg;

} vsf_hw_rtc_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_rtc_init(vsf_hw_rtc_t *hw_rtc_ptr, rtc_cfg_t *cfg_ptr)
{
    return VSF_ERR_NONE;
}

fsm_rt_t vsf_hw_rtc_enable(vsf_hw_rtc_t *hw_rtc_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_rtc_ptr);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_rtc_disable(vsf_hw_rtc_t *hw_rtc_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_rtc_ptr);

    return fsm_rt_cpl;
}

vsf_err_t vsf_hw_rtc_get_time(vsf_hw_rtc_t *hw_rtc_ptr, time_t *second_ptr, time_t *milliseconds_ptr)
{
    uint32_t sec;
    uint32_t usec;
    VSF_HAL_ASSERT((second_ptr != NULL) || (milliseconds_ptr != NULL));

    int result = aic_time_get(SINCE_EPOCH, &sec, &usec);
    VSF_HAL_ASSERT(result == 0);
    (void) result;

    if (second_ptr != NULL) {
        *second_ptr = sec;
    }

    if (milliseconds_ptr != NULL) {
        *milliseconds_ptr = usec / 1000;
    }

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_rtc_set_time(vsf_hw_rtc_t *hw_rtc_ptr, time_t second, time_t milliseconds)
{
    aic_time_update((uint32_t)second, milliseconds * 1000);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_rtc_get(vsf_hw_rtc_t *hw_rtc_ptr, vsf_rtc_tm_t *rtc_tm)
{
    VSF_HAL_ASSERT(NULL != hw_rtc_ptr);
    VSF_HAL_ASSERT(NULL != rtc_tm);

    time_t second, milliseconds;
    vsf_err_t result = vsf_hw_rtc_get_time(hw_rtc_ptr, &second, &milliseconds);
    if (result != VSF_ERR_NONE) {
        return result;
    }

    struct tm tm_local;
    gmtime_offset_r(&second, &tm_local, 0);

    rtc_tm->tm_sec  = tm_local.tm_sec;          // [0 .. 59]
    rtc_tm->tm_min  = tm_local.tm_min;          // [0 .. 59]
    rtc_tm->tm_hour = tm_local.tm_hour;         // [0 .. 23]
    rtc_tm->tm_mday = tm_local.tm_mday;         // [1 .. 31]
    rtc_tm->tm_wday = tm_local.tm_wday + 1;     // [1 .. 7]   - [Sunday, Monday, ..., Saturday]
    rtc_tm->tm_mon  = tm_local.tm_mon;          // [1 .. 12]  - [January -- December]
    rtc_tm->tm_year = tm_local.tm_year + 1900;  // [1900 .. ]
    rtc_tm->tm_ms   = milliseconds;

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_rtc_set(vsf_hw_rtc_t *hw_rtc_ptr, const vsf_rtc_tm_t *rtc_tm)
{
    VSF_HAL_ASSERT(NULL != hw_rtc_ptr);
    VSF_HAL_ASSERT(NULL != rtc_tm);
    VSF_HAL_ASSERT(vsf_rtc_tm_is_epoch_time(rtc_tm));

    struct tm tm_local;
    tm_local.tm_sec   = rtc_tm->tm_sec;         // [0 .. 59]
    tm_local.tm_min   = rtc_tm->tm_min;         // [0 .. 59]
    tm_local.tm_hour  = rtc_tm->tm_hour;        // [0 .. 23]
    tm_local.tm_mday  = rtc_tm->tm_mday;        // [1 .. 31]
    tm_local.tm_mon   = rtc_tm->tm_mon  - 1;     // [1 .. 12]  - [January -- December]
    tm_local.tm_year  = rtc_tm->tm_year - 1900; // [1900 .. ]
    time_t time = mk_gmtime_offset_r(&tm_local, 0);

    return vsf_hw_rtc_set_time(hw_rtc_ptr, time, rtc_tm->tm_ms);
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RTC_CFG_IMP_LV0(__COUNT, __hal_op)                                  \
    vsf_hw_rtc_t vsf_hw_rtc##__COUNT = {                                        \
        .freq = 0,                                                              \
        __hal_op                                                                \
    };
#include "hal/driver/common/rtc/rtc_template.inc"

#endif      // VSF_HAL_USE_RTC
