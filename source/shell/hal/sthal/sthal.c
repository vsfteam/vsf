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

/*============================ MACROS ========================================*/

#define __ST_HAL_VERSION_MAIN (0x01U)
#define __ST_HAL_VERSION_SUB1 (0x08U)
#define __ST_HAL_VERSION_SUB2 (0x02U)
#define __ST_HAL_VERSION_RC   (0x00U)
#define __ST_HAL_VERSION                                                       \
    ((__ST_HAL_VERSION_MAIN << 24U) | (__ST_HAL_VERSION_SUB1 << 16U) |         \
     (__ST_HAL_VERSION_SUB2 << 8U) | (__ST_HAL_VERSION_RC))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

volatile uint32_t       uwTick;
HAL_TickFreqTypeDef uwTickFreq = HAL_TICK_FREQ_DEFAULT;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

bool __sthal_check_timeout(uint32_t start, uint32_t timeout)
{
    if (timeout == HAL_MAX_DELAY) {
        return false;
    } else if (timeout == 0U) {
        return true;
    }

    return (HAL_GetTick() - start) > timeout;
}


VSF_CAL_WEAK(HAL_InitTick)
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    VSF_STHAL_ASSERT(0);
    return HAL_OK;
}

VSF_CAL_WEAK(HAL_IncTick)
void HAL_IncTick(void)
{
    VSF_STHAL_ASSERT(0);
}

VSF_CAL_WEAK(HAL_GetTickPrio)
uint32_t HAL_GetTickPrio(void)
{
    VSF_STHAL_ASSERT(0);
    return 0;
}

VSF_CAL_WEAK(HAL_SetTickFreq)
HAL_StatusTypeDef HAL_SetTickFreq(HAL_TickFreqTypeDef Freq)
{
    VSF_STHAL_ASSERT(0);
    return HAL_OK;
}

VSF_CAL_WEAK(HAL_SuspendTick)
void HAL_SuspendTick(void)
{
    VSF_STHAL_ASSERT(0);
}

VSF_CAL_WEAK(HAL_ResumeTick)
void HAL_ResumeTick(void)
{
    VSF_STHAL_ASSERT(0);
}

VSF_CAL_WEAK(HAL_GetTickFreq)
HAL_TickFreqTypeDef HAL_GetTickFreq(void)
{
    return uwTickFreq;
}

VSF_CAL_WEAK(HAL_Delay)
void HAL_Delay(uint32_t Delay)
{
    uint32_t tickstart = HAL_GetTick();
    uint32_t wait      = Delay;

    if (wait < HAL_MAX_DELAY) {
        wait += (uint32_t)(uwTickFreq);
    }

    while ((HAL_GetTick() - tickstart) < wait) {
    }
}


VSF_CAL_WEAK(HAL_GetTick)
uint32_t HAL_GetTick(void)
{
    vsf_systimer_tick_t tick = vsf_systimer_get();
    return vsf_systimer_tick_to_ms(tick);
}

HAL_StatusTypeDef HAL_Init(void)
{
// Normally, arch/hal will be initialized when vsf kernel startup,
//  but if kernel is not enabled, user MUST call vsf_arch_init() and vsf_hal_init()
#if VSF_USE_KERNEL != ENABLED
    vsf_arch_init();
    vsf_hal_init();
#endif

// Normally, systimer will be initialized in vsf kernel,
//  but if kernel is not enabled or kernel does not support timer,
//  systimer MUST be initialized here.
#if     (VSF_STHAL_TICK_USE_SYSTIMER == ENABLED)                                \
    &&  ((VSF_USE_KERNEL != ENABLED) || (VSF_KERNEL_CFG_EDA_SUPPORT_TIMER != ENABLED))
#if     (VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER)     \
    ||  (VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_WITH_COMP_TIMER)
    vsf_systimer_init();
    vsf_systimer_start();
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

VSF_CAL_WEAK(HAL_MspInit)
void HAL_MspInit(void)
{
    /*weak*/
}

VSF_CAL_WEAK(HAL_MspDeInit)
void HAL_MspDeInit(void)
{
    /*weak*/
}

uint32_t HAL_GetHalVersion(void)
{
    return __ST_HAL_VERSION;
}
