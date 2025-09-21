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

#if VSF_RTC_CFG_GET_API_TEMPLATE == ENABLED && VSF_RTC_CFG_GET_TIME_API_TEMPLATE == ENABLED
#   error "VSF_RTC_CFG_GET_API_TEMPLATE and  VSF_RTC_CFG_GET_TIME_API_TEMPLATE Only one can be defined at a time"
#endif

#if VSF_RTC_CFG_SET_API_TEMPLATE == ENABLED && VSF_RTC_CFG_SET_TIME_API_TEMPLATE == ENABLED
#   error "VSF_RTC_CFG_SET_API_TEMPLATE and  VSF_RTC_CFG_SET_TIME_API_TEMPLATE Only one can be defined at a time"
#endif

#ifndef VSF_RTC_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_RTC_CFG_REIMPLEMENT_API_GET_CONFIGURATION        DISABLED
#endif

#ifndef VSF_RTC_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_RTC_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#ifdef VSF_RTC_CFG_IMP_REMAP_PREFIX
#   undef VSF_RTC_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_RTC_CFG_REIMPLEMENT_API_CAPABILITY               ENABLED
#   undef VSF_RTC_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_RTC_CFG_REIMPLEMENT_API_GET_CONFIGURATION        ENABLED
#endif

#if VSF_RTC_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_RTC_CFG_CAPABILITY_T_IRQ_MASK
#       define VSF_RTC_CFG_CAPABILITY_T_IRQ_MASK        VSF_RTC_IRQ_ALL_BITS_MASK
#   endif
#endif

#define vsf_real_rtc_t          VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t)
#define vsf_real_rtc_capability VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_capability)
#define vsf_real_rtc_get_configuration      VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_get_configuration)
#define vsf_real_rtc_get        VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_get)
#define vsf_real_rtc_set        VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_set)
#define vsf_real_rtc_get_time   VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_get_time)
#define vsf_real_rtc_set_time   VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_set_time)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_RTC_CFG_GET_API_TEMPLATE == ENABLED
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
#endif

#if VSF_RTC_CFG_SET_API_TEMPLATE == ENABLED
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

#if VSF_RTC_CFG_GET_TIME_API_TEMPLATE == ENABLED
vsf_err_t vsf_real_rtc_get_time(vsf_hw_rtc_t *rtc_ptr, vsf_rtc_time_t *second_ptr, vsf_rtc_time_t *milliseconds_ptr)
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
#endif

#if VSF_RTC_CFG_SET_TIME_API_TEMPLATE == ENABLED
vsf_err_t vsf_real_rtc_set_time(vsf_real_rtc_t *rtc_ptr, vsf_rtc_time_t second, vsf_rtc_time_t milliseconds)
{
    VSF_HAL_ASSERT(rtc_ptr != NULL);

    struct tm *t = gmtime((const time_t *)&second);
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

#if VSF_RTC_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_rtc_capability_t vsf_real_rtc_capability(vsf_real_rtc_t *rtc_ptr)
{
    vsf_rtc_capability_t rtc_capability = {
        .irq_mask = VSF_RTC_CFG_CAPABILITY_IRQ_MASK,
    };

    return rtc_capability;
}
#endif

#if VSF_RTC_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_rtc_get_configuration(vsf_real_rtc_t *rtc_ptr, vsf_rtc_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != rtc_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(0); // Default implementation: not supported, trigger assertion
    return VSF_ERR_NOT_SUPPORT;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_RTC_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_RTC_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_RTC_CFG_CAPABILITY_T_IRQ_MASK
#undef vsf_real_rtc_t
#undef vsf_real_rtc_capability
#undef vsf_real_rtc_get_configuration

/*============================ MACROS ========================================*/

#ifdef VSF_RTC_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_rtc_t            VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t)
#   define vsf_imp_rtc_init         VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_init)
#   define vsf_imp_rtc_enable       VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_enable)
#   define vsf_imp_rtc_disable      VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_disable)
#   define vsf_imp_rtc_capability   VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_capability)
#   define vsf_imp_rtc_get          VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_get)
#   define vsf_imp_rtc_set          VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_set)
#   define vsf_imp_rtc_get_time     VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_get_time)
#   define vsf_imp_rtc_set_time     VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_set_time)
#   define vsf_imp_rtc_get_configuration    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_get_configuration)

#   define vsf_remap_rtc_t          VSF_MCONNECT(VSF_RTC_CFG_IMP_REMAP_PREFIX, _rtc_t)
#   define vsf_remap_rtc_init       VSF_MCONNECT(VSF_RTC_CFG_IMP_REMAP_PREFIX, _rtc_init)
#   define vsf_remap_rtc_enable     VSF_MCONNECT(VSF_RTC_CFG_IMP_REMAP_PREFIX, _rtc_enable)
#   define vsf_remap_rtc_disable    VSF_MCONNECT(VSF_RTC_CFG_IMP_REMAP_PREFIX, _rtc_disable)
#   define vsf_remap_rtc_capability VSF_MCONNECT(VSF_RTC_CFG_IMP_REMAP_PREFIX, _rtc_capability)
#   define vsf_remap_rtc_get        VSF_MCONNECT(VSF_RTC_CFG_IMP_REMAP_PREFIX, _rtc_get)
#   define vsf_remap_rtc_set        VSF_MCONNECT(VSF_RTC_CFG_IMP_REMAP_PREFIX, _rtc_set)
#   define vsf_remap_rtc_get_time   VSF_MCONNECT(VSF_RTC_CFG_IMP_REMAP_PREFIX, _rtc_get_time)
#   define vsf_remap_rtc_set_time   VSF_MCONNECT(VSF_RTC_CFG_IMP_REMAP_PREFIX, _rtc_set_time)
#   define vsf_remap_rtc_get_configuration  VSF_MCONNECT(VSF_RTC_CFG_IMP_REMAP_PREFIX, _rtc_get_configuration)

#   define VSF_RTC_CFG_IMP_REMAP_FUNCTIONS                                                                        \
        vsf_err_t vsf_imp_rtc_init(vsf_imp_rtc_t *rtc_ptr, vsf_rtc_cfg_t *cfg_ptr)            \
        {                                                                                     \
            VSF_HAL_ASSERT(rtc_ptr != NULL);                                                  \
            return vsf_remap_rtc_init(rtc_ptr, cfg_ptr);                                      \
        }                                                                                     \
        void vsf_imp_rtc_fini(vsf_imp_rtc_t *rtc_ptr)                                         \
        {                                                                                     \
            VSF_HAL_ASSERT(rtc_ptr != NULL);                                                  \
            vsf_remap_rtc_fini(rtc_ptr);                                                      \
        }                                                                                     \
        fsm_rt_t vsf_imp_rtc_enable(vsf_imp_rtc_t *rtc_ptr)                                   \
        {                                                                                     \
            VSF_HAL_ASSERT(rtc_ptr != NULL);                                                  \
            return vsf_remap_rtc_enable(rtc_ptr);                                             \
        }                                                                                     \
        fsm_rt_t vsf_imp_rtc_disable(vsf_imp_rtc_t *rtc_ptr)                                  \
        {                                                                                     \
            VSF_HAL_ASSERT(rtc_ptr != NULL);                                                  \
            return vsf_remap_rtc_disable(rtc_ptr);                                            \
        }                                                                                     \
        vsf_err_t vsf_imp_rtc_get(vsf_imp_rtc_t *rtc_ptr, vsf_imp_rtc_tm_t *rtc_tm)           \
        {                                                                                     \
            VSF_HAL_ASSERT(rtc_ptr != NULL);                                                  \
            return vsf_remap_rtc_get(rtc_ptr, rtc_tm);                                        \
        }                                                                                     \
        vsf_err_t vsf_imp_rtc_set(vsf_imp_rtc_t *rtc_ptr, const vsf_imp_rtc_tm_t *rtc_tm)     \
        {                                                                                     \
            VSF_HAL_ASSERT(rtc_ptr != NULL);                                                  \
            return vsf_remap_rtc_set(rtc_ptr, rtc_tm);                                        \
        }                                                                                     \
        vsf_err_t vsf_imp_rtc_get_time(vsf_imp_rtc_t *rtc_ptr, vsf_imp_rtc_time_t *second_ptr,\
                                       vsf_imp_rtc_time_t *millisecond_ptr)                   \
        {                                                                                     \
            VSF_HAL_ASSERT(rtc_ptr != NULL);                                                  \
            return vsf_remap_rtc_get_time(rtc_ptr, second_ptr, millisecond_ptr);              \
        }                                                                                     \
        vsf_err_t vsf_imp_rtc_set_time(vsf_imp_rtc_t *rtc_ptr, vsf_imp_rtc_time_t second,     \
                                       vsf_imp_rtc_time_t millisecond)                        \
        {                                                                                     \
            VSF_HAL_ASSERT(rtc_ptr != NULL);                                                  \
            return vsf_remap_rtc_set_time(rtc_ptr, second, millisecond);                      \
        }                                                                                     \
        vsf_rtc_capability_t vsf_imp_rtc_capability(vsf_imp_rtc_t *rtc_ptr)                   \
        {                                                                                     \
            VSF_HAL_ASSERT(rtc_ptr != NULL);                                                  \
            return vsf_remap_rtc_capability(rtc_ptr);                                         \
        }                                                                                     \
                                                                                              \
        vsf_err_t vsf_imp_rtc_get_configuration(vsf_imp_rtc_t *rtc_ptr, vsf_rtc_cfg_t *cfg_ptr) \
        {                                                                                     \
            VSF_HAL_ASSERT(rtc_ptr != NULL);                                                  \
            return vsf_remap_rtc_get_configuration(rtc_ptr, cfg_ptr);                         \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _rtc
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _RTC

#if !defined(VSF_RTC_CFG_IMP_PREFIX) && !defined(VSF_RTC_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_RTC_CFG_IMP_PREFIX in rtc driver"
#endif

#if !defined(VSF_RTC_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_RTC_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_RTC_CFG_IMP_UPCASE_PREFIX in rtc driver"
#endif

#ifndef VSF_RTC_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_RTC_CFG_IMP_COUNT_MASK_PREFIX        VSF_RTC_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_RTC_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_RTC_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_RTC_CFG_IMP_PREFIX
#undef VSF_RTC_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_RTC_CFG_IMP_UPCASE_PREFIX
#undef VSF_RTC_CFG_IMP_DEVICE_PREFIX
#undef VSF_RTC_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_RTC_CFG_IMP_LV0
#undef VSF_RTC_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_RTC_CFG_IMP_HAS_OP
#undef VSF_RTC_CFG_IMP_EXTERN_OP

#undef vsf_imp_rtc_t
#undef vsf_imp_rtc_init
#undef vsf_imp_rtc_enable
#undef vsf_imp_rtc_disable
#undef vsf_imp_rtc_capability
#undef vsf_imp_rtc_get
#undef vsf_imp_rtc_set
#undef vsf_imp_rtc_get_time
#undef vsf_imp_rtc_set_time
#undef vsf_imp_rtc_get_configuration

#undef vsf_remap_rtc_t
#undef vsf_remap_rtc_init
#undef vsf_remap_rtc_enable
#undef vsf_remap_rtc_disable
#undef vsf_remap_rtc_capability
#undef vsf_remap_rtc_get
#undef vsf_remap_rtc_set
#undef vsf_remap_rtc_get_time
#undef vsf_remap_rtc_set_time
#undef vsf_remap_rtc_get_configuration

#endif  /* VSF_HAL_USE_RTC */

