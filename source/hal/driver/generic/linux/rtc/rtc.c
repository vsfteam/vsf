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

#define VSF_RTC_CFG_PREFIX              vsf_hw
#define VSF_RTC_CFG_UPPERCASE_PREFIX    VSF_HW

#include "./rtc.h"

#if VSF_HAL_USE_RTC == ENABLED

// for NO_INIT
#include "utilities/vsf_utilities.h"

#include <sys/time.h>

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_rtc_t {
#if VSF_RTC_CFG_MULTI_CLASS == ENABLED
    vsf_rtc_t vsf_rtc;
#endif

    bool is_enabled;
} vsf_hw_rtc_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_rtc_init(vsf_hw_rtc_t *hw_rtc_ptr, rtc_cfg_t *cfg_ptr)
{
    hw_rtc_ptr->is_enabled = false;
    return VSF_ERR_NONE;
}

fsm_rt_t vsf_hw_rtc_enable(vsf_hw_rtc_t *hw_rtc_ptr)
{
    hw_rtc_ptr->is_enabled = true;
    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_rtc_disable(vsf_hw_rtc_t *hw_rtc_ptr)
{
    hw_rtc_ptr->is_enabled = false;
    return fsm_rt_cpl;
}

time_t vsf_hw_rtc_get_second(vsf_hw_rtc_t *hw_rtc_ptr)
{
    // TODO:
    return 0;
}

vsf_err_t vsf_hw_rtc_set_second(vsf_hw_rtc_t *rtc_ptr, time_t second)
{
    return VSF_ERR_FAIL;
}

vsf_err_t vsf_hw_rtc_get(vsf_hw_rtc_t *hw_rtc_ptr, vsf_rtc_tm_t *rtc_tm)
{
    if (hw_rtc_ptr->is_enabled) {
        struct timeval tv;
        struct tm tm;
        gettimeofday(&tv, NULL);
        localtime_r(tv.tv_sec, &tm);

        if (rtc_tm != NULL) {
            rtc_tm->tm_sec = tm.tm_sec;
            rtc_tm->tm_min = tm.tm_min;
            rtc_tm->tm_hour = tm.tm_hour;
            rtc_tm->tm_mday = tm.tm_mday;
            rtc_tm->tm_wday = tm.tm_wday;
            rtc_tm->tm_mon = tm.tm_mon;
            rtc_tm->tm_year = tm.tm_year;
            rtc_tm->tm_ms = tv.tv_usec / 1000;
        }
        return VSF_ERR_NONE;
    }
    return VSF_ERR_FAIL;
}

vsf_err_t vsf_hw_rtc_set(vsf_hw_rtc_t *hw_rtc_ptr, const vsf_rtc_tm_t *rtc_tm)
{
    return VSF_ERR_FAIL;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RTC_CFG_IMP_LV0(__COUNT, __dont_care)                               \
    NO_INIT vsf_hw_rtc_t vsf_hw_rtc##__COUNT;
#include "hal/driver/common/rtc/rtc_template.inc"

#endif
