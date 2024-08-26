/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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

#include "sthal.h"
#include "hal/vsf_hal.h"

#if VSF_HAL_USE_USART == ENABLED

/*============================ MACROS ========================================*/

#define __ST_HAL_VERSION_MAIN (0x01U)
#define __ST_HAL_VERSION_SUB1 (0x08U)
#define __ST_HAL_VERSION_SUB2 (0x02U)
#define __ST_HAL_VERSION_RC   (0x00U)
#define __ST_HAL_VERSION                                                       \
    ((__ST_HAL_VERSION_MAIN << 24U) | (__ST_HAL_VERSION_SUB1 << 16U) |         \
     (__ST_HAL_VERSION_SUB2 << 8U) | (__ST_HAL_VERSION_RC))

#ifndef VSF_STHAL_USE_VSFHAL_INIT
#   define VSF_STHAL_USE_VSFHAL_INIT            DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

volatile uint32_t       uwTick;
HAL_TickFreqTypeDef uwTickFreq = HAL_TICK_FREQ_DEFAULT;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#if VSF_STHAL_TICK_USE_SYSTIMER == ENABLED
#if VSF_STHAL_USE_CALL_SYSTIMER_INIT == ENABLED

#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER
void vsf_systimer_evthandler(vsf_systimer_tick_t tick)
{
    vsf_systimer_set_idle();
}
#else
#   error "TODO: support more systimer mode"
#endif

__weak HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    VSF_HAL_ASSERT(0);
    return HAL_OK;
}

__weak void HAL_IncTick(void)
{
    VSF_HAL_ASSERT(0);
}

uint32_t HAL_GetTickPrio(void)
{
    VSF_HAL_ASSERT(0);
    return 0;
}

HAL_StatusTypeDef HAL_SetTickFreq(HAL_TickFreqTypeDef Freq)
{
    VSF_HAL_ASSERT(0);
    return HAL_OK;
}

HAL_TickFreqTypeDef HAL_GetTickFreq(void)
{
    return uwTickFreq;
}

__weak void HAL_Delay(uint32_t Delay)
{
    uint32_t tickstart = HAL_GetTick();
    uint32_t wait      = Delay;

    if (wait < HAL_MAX_DELAY) {
        wait += (uint32_t)(uwTickFreq);
    }

    while ((HAL_GetTick() - tickstart) < wait) {
    }
}

__weak void HAL_SuspendTick(void)
{
    VSF_HAL_ASSERT(0);
}

__weak void HAL_ResumeTick(void)
{
    VSF_HAL_ASSERT(0);
}
#endif
__weak uint32_t HAL_GetTick(void)
{
    vsf_systimer_tick_t tick = vsf_systimer_get();
    return vsf_systimer_tick_to_ms(tick);
}
#endif

HAL_StatusTypeDef HAL_Init(void)
{
#if VSF_STHAL_USE_VSFHAL_INIT == ENABLED
    vsf_hal_init();
#endif

#if (VSF_STHAL_TICK_USE_SYSTIMER == ENABLED) && VSF_STHAL_USE_CALL_SYSTIMER_INIT == ENABLED
#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER
    vsf_systimer_init();
    vsf_systimer_set_idle();
#else
#   error "TODO: support more systimer mode"
#endif
#endif

    HAL_MspInit();
    return HAL_OK;
}


HAL_StatusTypeDef HAL_DeInit(void)
{
    return HAL_OK;
}

__weak void HAL_MspInit(void) {}

__weak void HAL_MspDeInit(void) {}

uint32_t HAL_GetHalVersion(void)
{
    return __ST_HAL_VERSION;
}

#endif
