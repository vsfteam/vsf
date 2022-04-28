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

#include "vsf.h"

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_ADC_DEMO == ENABLED && VSF_HAL_USE_ADC == ENABLED

/*============================ MACROS ========================================*/

#ifdef APP_ADC_DEMO_CFG_ADC_PREFIX
#   undef VSF_ADC_CFG_PREFIX
#   define VSF_ADC_CFG_PREFIX                       APP_ADC_DEMO_CFG_ADC_PREFIX
#endif

#ifndef APP_ADC_DEMO_CFG_ADC
#   define APP_ADC_DEMO_CFG_ADC                     (vsf_adc_t *)&vsf_hw_adc0
#endif

#ifndef APP_ADC_DEMO_CFG_REQUEST_ONCE_DEMO
#   define APP_ADC_DEMO_CFG_REQUEST_ONCE_DEMO       ENABLED
#endif

#ifndef APP_ADC_DEMO_CFG_REQUEST_DEMO
#   define APP_ADC_DEMO_CFG_REQUEST_DEMO            DISABLED
#endif

#ifndef APP_ADC_DEMO_CFG_FEATURE
#   define APP_ADC_DEMO_CFG_FEATURE                 (SCAN_CONV_SEQUENCE_MODE | ADC_REF_VDD_1)
#endif

#ifndef APP_ADC_DEMO_CFG_CLOCK
#   define APP_ADC_DEMO_CFG_CLOCK                   (1 * 1000 * 1000)
#endif

#ifndef APP_ADC_DEMO_IRQ_PRIO
#   define APP_ADC_DEMO_IRQ_PRIO                    vsf_arch_prio_2
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ TYPES =========================================*/

typedef struct app_adc_request_once_demo_t {
    uint16_t value;
} app_adc_request_once_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

app_adc_request_once_demo_t __request_once_demo;

/*============================ IMPLEMENTATION ================================*/

#if APP_ADC_DEMO_CFG_REQUEST_ONCE_DEMO == ENABLED
static void __request_once_demo_isr_handler(void *target_ptr, vsf_adc_t *adc_ptr)
{
    VSF_ASSERT(adc_ptr != NULL);

    vsf_trace_debug("enter timer overflow interrupt");

    vsf_err_t result = vsf_adc_disable(adc_ptr);
    VSF_ASSERT(result == VSF_ERR_NONE);
    (void)result;
}

static void __adc_request_once_demo(void)
{
    vsf_err_t result;

    adc_cfg_t adc_cfg = {
        .feature        = APP_ADC_DEMO_CFG_FEATURE,
        .isr            = {
            .handler_fn = __request_once_demo_isr_handler,
            .target_ptr = NULL,
            .prio       = APP_ADC_DEMO_IRQ_PRIO,
        },
    };

    result = vsf_adc_init(APP_ADC_DEMO_CFG_ADC, &adc_cfg);
    VSF_ASSERT(result == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_adc_enable(APP_ADC_DEMO_CFG_ADC));

    adc_channel_cfg_t adc_channel_cfg = {
        .channel = 0,
    };
    vsf_adc_channel_request_once(APP_ADC_DEMO_CFG_ADC, &adc_channel_cfg, &__request_once_demo.value);
}
#endif

#if APP_ADC_DEMO_CFG_REQUEST_DEMO == ENABLED
static void __adc_request_demo_handler(void *target_ptr, vsf_adc_t *adc_ptr)
{
    vsf_trace_debug("enter timer overflow interrupt");
}

static void __adc_requests_demo(void)
{
    vsf_err_t result;

    adc_cfg_t adc_cfg = {
        .feature        = APP_ADC_DEMO_CFG_FEATURE,
        .clock_freq     = APP_ADC_DEMO_CFG_CLOCK,
        .isr            = {
            .handler_fn = __adc_request_demo_handler,
            .target_ptr = NULL,
            .prio       = APP_ADC_DEMO_IRQ_PRIO,
        },
    };

    result = vsf_adc_init(APP_ADC_DEMO_CFG_ADC, &adc_cfg);
    VSF_ASSERT(result == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_adc_enable(APP_ADC_DEMO_CFG_ADC));

    adc_channel_cfg_t adc_channel_cfg = {
        .channel = 0,
    };
    vsf_adc_channel_request_once(APP_ADC_DEMO_CFG_ADC, &adc_channel_cfg, &__request_once_demo.value);
}
#endif

#if APP_USE_LINUX_DEMO == ENABLED
int adc_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   endif
#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif
#endif

#if APP_ADC_DEMO_CFG_REQUEST_ONCE_DEMO == ENABLED
    __adc_request_once_demo();
#elif APP_ADC_DEMO_CFG_REQUEST_ONCE_DEMO == ENABLED
    __adc_requests_demo();
#endif

    return 0;
}

#endif
