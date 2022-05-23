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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_TIMER_DEMO == ENABLED && VSF_HAL_USE_TIMER == ENABLED

/*============================ MACROS ========================================*/

#ifdef APP_TIMER_DEMO_CFG_TIMER_PREFIX
#   undef VSF_TIMER_CFG_PREFIX
#   define VSF_TIMER_CFG_PREFIX                         APP_TIMER_DEMO_CFG_TIMER_PREFIX
#endif

#ifndef APP_TIMER_DEMO_CFG_TIMER
#   define APP_TIMER_DEMO_CFG_TIMER                     (vsf_timer_t *)&vsf_hw_timer2
#endif

#ifndef APP_TIMER_DEMO_CFG_MODE
#   define APP_TIMER_DEMO_CFG_MODE                      TIMER_MODE_CONTINUES
#endif

#ifndef APP_TIMER_DEMO_CFG_FREQ
#   define APP_TIMER_DEMO_CFG_FREQ                      (1 * 1000 * 1000)   // 1us
#endif

#ifndef APP_TIMER_DEMO_CFG_MAX_COUNT
#   define APP_TIMER_DEMO_CFG_MAX_COUNT                 (2 * 1000 * 1000)   // 2s
#endif

#ifndef APP_TIMER_DEMO_IRQ_PRIO
#   define APP_TIMER_DEMO_IRQ_PRIO                      vsf_arch_prio_2
#endif

/*============================ IMPLEMENTATION ================================*/

void __timer_demo_handler(void *target_ptr, vsf_timer_t *timer_ptr, em_timer_irq_mask_t irq_mask)
{
    if (irq_mask == VSF_TIMER_IRQ_MASK_OVERFLOW) {
        static uint32_t cnt = 0;
        vsf_trace_debug("timer interrupt :%d" VSF_TRACE_CFG_LINEEND, cnt++);
    }
}

void __us_ticker_irq_handler(void)
{
    static uint32_t cnt = 0;
    vsf_trace_debug("timer interrupt :%d" VSF_TRACE_CFG_LINEEND, cnt++);
}

static void __timer_demo(void)
{
    vsf_err_t result;

    timer_cfg_t timer_cfg = {
        .mode           = APP_TIMER_DEMO_CFG_MODE,
        .freq           = APP_TIMER_DEMO_CFG_FREQ,
        .max_count      = APP_TIMER_DEMO_CFG_MAX_COUNT,
        .isr            = {
            .handler_fn = __timer_demo_handler,
            .target_ptr = NULL,
            .prio       = APP_TIMER_DEMO_IRQ_PRIO,
        },
    };

    result = vsf_timer_init(APP_TIMER_DEMO_CFG_TIMER, &timer_cfg);
    VSF_ASSERT(result == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_timer_enable(APP_TIMER_DEMO_CFG_TIMER));

    vsf_timer_irq_enable(APP_TIMER_DEMO_CFG_TIMER, VSF_TIMER_IRQ_MASK_OVERFLOW);
}

#if APP_USE_LINUX_DEMO == ENABLED
int timer_main(int argc, char *argv[])
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

    __timer_demo();

    return 0;
}

#endif
