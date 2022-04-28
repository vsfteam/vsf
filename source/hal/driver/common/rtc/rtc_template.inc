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

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/



#define vsf_real_rtc_t          VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t)
#define vsf_real_rtc_capability VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_capability)


/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_RTC_CFG_REIMPLEMENT_CAPABILITY == DISABLED
rtc_capability_t vsf_real_rtc_capability(vsf_real_rtc_t *rtc_ptr)
{
    rtc_capability_t rtc_capability = {
        0,
    };

    return rtc_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     rtc
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              RTC

#ifndef VSF_RTC_CFG_PREFIX
#   error "Please define VSF_RTC_CFG_PREFIX in rtc driver"
#endif

#ifndef VSF_RTC_CFG_UPPERCASE_PREFIX
#   error "Please define VSF_RTC_CFG_UPPERCASE_PREFIX in rtc driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif  /* VSF_HAL_USE_RTC */
