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

#include "./rtc.h"

#if VSF_HAL_USE_RTC == ENABLED

// for NO_INIT
#include "utilities/vsf_utilities.h"

#include <Windows.h>

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_rtc_t {
#if VSF_SPI_CFG_MULTI_INSTANCES == ENABLED
    vsf_rtc_t vsf_rtc;
#endif

    bool is_enabled;
} vsf_hw_rtc_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

NO_INIT vsf_hw_rtc_t vsf_hw_rtc0;

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_rtc_init(vsf_rtc_t *rtc_ptr, rtc_cfg_t *cfg_ptr)
{
    vsf_hw_rtc_t *hw_rtc_ptr = (vsf_hw_rtc_t *)rtc_ptr;
    hw_rtc_ptr->is_enabled = false;
    return VSF_ERR_NONE;
}

fsm_rt_t vsf_hw_rtc_enable(vsf_rtc_t *rtc_ptr)
{
    vsf_hw_rtc_t *hw_rtc_ptr = (vsf_hw_rtc_t *)rtc_ptr;
    hw_rtc_ptr->is_enabled = true;
    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_rtc_disable(vsf_rtc_t *rtc_ptr)
{
    vsf_hw_rtc_t *hw_rtc_ptr = (vsf_hw_rtc_t *)rtc_ptr;
    hw_rtc_ptr->is_enabled = false;
    return fsm_rt_cpl;
}

vsf_err_t vsf_hw_rtc_get(vsf_rtc_t *rtc_ptr, vsf_rtc_tm_t *tm)
{
    vsf_hw_rtc_t *hw_rtc_ptr = (vsf_hw_rtc_t *)rtc_ptr;
    if (hw_rtc_ptr->is_enabled) {
        SYSTEMTIME system_time;
        GetSystemTime(&system_time);

        if (tm != NULL) {
            tm->tm_sec = system_time.wSecond;
            tm->tm_min = system_time.wMinute;
            tm->tm_hour = system_time.wHour;
            tm->tm_day = system_time.wDay;
            tm->tm_wday = system_time.wDayOfWeek == 0 ? 7 : system_time.wDayOfWeek;
            tm->tm_mon = system_time.wMonth;
            tm->tm_year = system_time.wYear;
        }
        return VSF_ERR_NONE;
    }
    return VSF_ERR_FAIL;
}

vsf_err_t vsf_hw_rtc_set(vsf_rtc_t *rtc_ptr, const vsf_rtc_tm_t *tm)
{
    return VSF_ERR_FAIL;
}

#endif
