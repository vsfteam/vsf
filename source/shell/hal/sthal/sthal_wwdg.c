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

#include "hal/vsf_hal.h"
#include "./sthal.h"

#if defined(HAL_WWDG_MODULE_ENABLED) && VSF_HAL_USE_WDT == ENABLED

/*============================ MACROS ========================================*/

#   ifndef VSF_STHAL_CFG_WWDT_ISR_PRIO
#      define VSF_STHAL_CFG_WWDT_ISR_PRIO vsf_arch_prio_0
#   endif

#   ifndef VSF_STHAL_CFG_WWDT_CLOCK
#      define VSF_STHAL_CFG_WWDT_CLOCK   (40 * 1000)
#   endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(HAL_WWDG_MspInit)
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
    VSF_STHAL_ASSERT(hwwdg != NULL);
    VSF_UNUSED_PARAM(hwwdg);
}

VSF_CAL_WEAK(HAL_WWDG_EarlyWakeupCallback)
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    VSF_STHAL_ASSERT(hwwdg != NULL);
    VSF_UNUSED_PARAM(hwwdg);
}

static void __wdt_isr_handler(void *target_ptr, vsf_wdt_t *wdt_ptr)
{
    VSF_STHAL_ASSERT(target_ptr != NULL);
    VSF_STHAL_ASSERT(wdt_ptr != NULL);
    WWDG_HandleTypeDef *hwwdg = (WWDG_HandleTypeDef *)target_ptr;

    if (hwwdg->Init.EWIMode == WWDG_EWI_ENABLE) {
#   if (USE_HAL_WWDG_REGISTER_CALLBACKS == 1)
        hwwdg->EwiCallback(hwwdg);
#   else
        HAL_WWDG_EarlyWakeupCallback(hwwdg);
#   endif
    }
}

HAL_StatusTypeDef HAL_WWDG_Init(WWDG_HandleTypeDef *hwwdg)
{
    if (hwwdg == NULL) {
        return HAL_ERROR;
    }

    vsf_wdt_t *wdt = (vsf_wdt_t *)hwwdg->Instance;
    VSF_STHAL_ASSERT(wdt != NULL);

    VSF_STHAL_ASSERT(IS_WWDG_ALL_INSTANCE(hwwdg->Instance));
    VSF_STHAL_ASSERT(IS_WWDG_PRESCALER(hwwdg->Init.Prescaler));
    VSF_STHAL_ASSERT(IS_WWDG_WINDOW(hwwdg->Init.Window));
    VSF_STHAL_ASSERT(IS_WWDG_COUNTER(hwwdg->Init.Counter));
    VSF_STHAL_ASSERT(IS_WWDG_EWI_MODE(hwwdg->Init.EWIMode));

#   if (USE_HAL_WWDG_REGISTER_CALLBACKS == 1)
    if (hwwdg->EwiCallback == NULL) {
        hwwdg->EwiCallback = HAL_WWDG_EarlyWakeupCallback;
    }
    if (hwwdg->MspInitCallback == NULL) {
        hwwdg->MspInitCallback = HAL_WWDG_MspInit;
    }
    hwwdg->MspInitCallback(hwwdg);
#   else
    HAL_WWDG_MspInit(hwwdg);
#   endif

    vsf_wdt_capability_t cap = vsf_wdt_capability(wdt);

    uint32_t prescaler = hwwdg->Init.Prescaler;
    uint32_t max_ms = 1000 * hwwdg->Init.Counter / (VSF_STHAL_CFG_WWDT_CLOCK / prescaler);
    uint32_t min_ms;
    if (cap.support_min_timeout) {
        min_ms = 1000 * hwwdg->Init.Window / (VSF_STHAL_CFG_WWDT_CLOCK / prescaler);
    } else {
        min_ms = 0;
    }

    if ((hwwdg->Init.EWIMode == WWDG_EWI_ENABLE) && !cap.support_early_wakeup) {
        return HAL_ERROR;
    }
    vsf_wdt_mode_t mode = hwwdg->Init.EWIMode;
    if (cap.support_reset_soc) {
        mode |= VSF_WDT_MODE_RESET_SOC;
    } else if (cap.support_reset_cpu) {
        mode |= VSF_WDT_MODE_RESET_CPU;
    } else {
        return HAL_ERROR;
    }

    vsf_wdt_cfg_t  cfg  = {
        .mode   = mode,
        .max_ms = max_ms,
        .min_ms = min_ms,
        .isr    = {
            .handler_fn = __wdt_isr_handler,
            .target_ptr = hwwdg,
            .prio       = VSF_STHAL_CFG_WWDT_ISR_PRIO
        }
    };

    vsf_err_t err = vsf_wdt_init(wdt, &cfg);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }
    while (fsm_rt_cpl != vsf_wdt_enable(wdt));

    return HAL_OK;
}

#   if (USE_HAL_WWDG_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_WWDG_RegisterCallback(
    WWDG_HandleTypeDef *hwwdg, HAL_WWDG_CallbackIDTypeDef CallbackID,
    pWWDG_CallbackTypeDef pCallback)
{
    VSF_STHAL_ASSERT(hwwdg != NULL);
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {
        status = HAL_ERROR;
    } else {
        switch (CallbackID) {
        case HAL_WWDG_EWI_CB_ID:
            hwwdg->EwiCallback = pCallback;
            break;
        case HAL_WWDG_MSPINIT_CB_ID:
            hwwdg->MspInitCallback = pCallback;
            break;
        default:
            status = HAL_ERROR;
            break;
        }
    }

    return status;
}

HAL_StatusTypeDef HAL_WWDG_UnRegisterCallback(
    WWDG_HandleTypeDef *hwwdg, HAL_WWDG_CallbackIDTypeDef CallbackID)
{
    VSF_STHAL_ASSERT(hwwdg != NULL);
    HAL_StatusTypeDef status = HAL_OK;
    switch (CallbackID) {
    case HAL_WWDG_EWI_CB_ID:
        hwwdg->EwiCallback = HAL_WWDG_EarlyWakeupCallback;
        break;
    case HAL_WWDG_MSPINIT_CB_ID:
        hwwdg->MspInitCallback = HAL_WWDG_MspInit;
        break;
    default:
        status = HAL_ERROR;
        break;
    }

    return status;
}
#   endif

HAL_StatusTypeDef HAL_WWDG_Refresh(WWDG_HandleTypeDef *hwwdg)
{
    if (hwwdg == NULL) {
        return HAL_ERROR;
    }
    vsf_wdt_t *wdt = (vsf_wdt_t *)hwwdg->Instance;
    VSF_STHAL_ASSERT(wdt != NULL);

    vsf_wdt_feed(wdt);

    return HAL_OK;
}

// It should not be called by user
void HAL_WWDG_IRQHandler(WWDG_HandleTypeDef *hwwdg)
{
    VSF_STHAL_ASSERT(hwwdg != NULL);
}

#endif
