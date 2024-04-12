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

#include "../driver.h"

#if VSF_HAL_USE_RTC == ENABLED

// for vsf_trace_warning
#include "service/vsf_service.h"

#include <Windows.h>

/*============================ MACROS ========================================*/

#ifndef VSF_HW_RTC_CFG_MULTI_CLASS
#   define VSF_HW_RTC_CFG_MULTI_CLASS VSF_RTC_CFG_MULTI_CLASS
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_hw_rtc_t {
#if VSF_HW_RTC_CFG_MULTI_CLASS == ENABLED
    vsf_rtc_t vsf_rtc;
#endif

    bool is_enabled;
} vsf_hw_rtc_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_rtc_init(vsf_hw_rtc_t *hw_rtc_ptr, vsf_rtc_cfg_t *cfg_ptr)
{
    hw_rtc_ptr->is_enabled = false;
    return VSF_ERR_NONE;
}

void vsf_hw_rtc_fini(vsf_hw_rtc_t *hw_rtc_ptr)
{
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

vsf_err_t vsf_hw_rtc_get(vsf_hw_rtc_t *hw_rtc_ptr, vsf_rtc_tm_t *rtc_tm)
{
    if (!hw_rtc_ptr->is_enabled) {
        vsf_trace_warning("rtc_get is called before enabled, but OK for windows. Just a warning!!!" VSF_TRACE_CFG_LINEEND);
    }

    SYSTEMTIME system_time;
    GetLocalTime(&system_time);

    if (rtc_tm != NULL) {
        rtc_tm->tm_sec = system_time.wSecond;
        rtc_tm->tm_min = system_time.wMinute;
        rtc_tm->tm_hour = system_time.wHour;
        rtc_tm->tm_mday = system_time.wDay;
        rtc_tm->tm_wday = system_time.wDayOfWeek == 0 ? 7 : system_time.wDayOfWeek;
        rtc_tm->tm_mon = system_time.wMonth;
        rtc_tm->tm_year = system_time.wYear;
        rtc_tm->tm_ms = system_time.wMilliseconds;
    }
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_rtc_set(vsf_hw_rtc_t *hw_rtc_ptr, const vsf_rtc_tm_t *rtc_tm)
{
    return VSF_ERR_FAIL;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RTC_CFG_IMP_PREFIX                      vsf_hw
#define VSF_RTC_CFG_IMP_UPCASE_PREFIX               VSF_HW
#define VSF_RTC_CFG_GET_TIME_API_TEMPLATE           ENABLED
#define VSF_RTC_CFG_SET_TIME_API_TEMPLATE           ENABLED
#define VSF_RTC_CFG_CAPABILITY_T_IRQ_MASK           0

#define VSF_RTC_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    vsf_hw_rtc_t vsf_hw_rtc ## __IDX  = {                                       \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/rtc/rtc_template.inc"

#endif
