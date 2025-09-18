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
#include "sthal.h"

#if defined(HAL_TIM_MODULE_ENABLED) && VSF_HAL_USE_TIMER == ENABLED

/*============================ MACROS ========================================*/

#   ifndef VSF_STHAL_CFG_TIMER_ISR_PRIO
#      define VSF_STHAL_CFG_TIMER_ISR_PRIO vsf_arch_prio_0
#   endif

#   ifndef VSF_STHAL_CFG_TIMER_CLOCK
#      define VSF_STHAL_CFG_TIMER_CLOCK   (10 * 1000 * 1000)
#   endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(HAL_TIM_Base_MspInit)
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_Base_MspDeInit)
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_OC_MspInit)
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_OC_MspDeInit)
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_PWM_MspInit)
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_PWM_MspDeInit)
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_IC_MspInit)
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_IC_MspDeInit)
void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_OnePulse_MspInit)
void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_OnePulse_MspDeInit)
void HAL_TIM_OnePulse_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_Encoder_MspInit)
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_Encoder_MspDeInit)
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_PeriodElapsedCallback)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_PeriodElapsedHalfCpltCallback)
void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_OC_DelayElapsedCallback)
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_IC_CaptureCallback)
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_IC_CaptureHalfCpltCallback)
void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_PWM_PulseFinishedCallback)
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_PWM_PulseFinishedHalfCpltCallback)
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_TriggerCallback)
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_TriggerHalfCpltCallback)
void HAL_TIM_TriggerHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_ErrorCallback)
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_ErrorCallback)
void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_ErrorCallback)
void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_ErrorCallback)
void HAL_TIMEx_CommutCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_ErrorCallback)
void HAL_TIMEx_CommutHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

VSF_CAL_WEAK(HAL_TIM_ErrorCallback)
void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

static void __timer_isr_handler(void *target_ptr, vsf_timer_t *timer_ptr,
                                vsf_timer_irq_mask_t irq_mask)
{
    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)target_ptr;
    if (irq_mask & VSF_TIMER_IRQ_MASK_OVERFLOW) {
        HAL_TIM_PeriodElapsedCallback(htim);
    }
}

HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_INSTANCE(htim->Instance));
    VSF_STHAL_ASSERT(IS_TIM_COUNTER_MODE(htim->Init.CounterMode));
    VSF_STHAL_ASSERT(IS_TIM_CLOCKDIVISION_DIV(htim->Init.ClockDivision));
    VSF_STHAL_ASSERT(IS_TIM_AUTORELOAD_PRELOAD(htim->Init.AutoReloadPreload));

    if (htim->State == HAL_TIM_STATE_RESET) {
        htim->Lock = HAL_UNLOCKED;

#   if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
        TIM_ResetCallback(htim);
        if (htim->Base_MspInitCallback == NULL) {
            htim->Base_MspInitCallback = HAL_TIM_Base_MspInit;
        }
        htim->Base_MspInitCallback(htim);
#   else
        HAL_TIM_Base_MspInit(htim);
#   endif
    }

    htim->State = HAL_TIM_STATE_BUSY;

    vsf_timer_cfg_t cfg = {
        .period = htim->Init.Period,
        .freq   = VSF_STHAL_CFG_TIMER_CLOCK / (htim->Init.Prescaler + 1) / htim->Init.ClockDivision,
        .isr =
            {
                .handler_fn = __timer_isr_handler,
                .target_ptr = htim,
                .prio       = VSF_STHAL_CFG_TIMER_ISR_PRIO,
            },
    };
    vsf_err_t err = vsf_timer_init(timer, &cfg);
    if (VSF_ERR_NONE != err) {
        return HAL_ERROR;
    }

    while (fsm_rt_cpl != vsf_timer_enable(timer));

    vsf_timer_channel_cfg_t channel_cfg = {
        .mode  = VSF_TIMER_CHANNEL_MODE_BASE,
        .pulse = 0,
    };

    if (htim->Channel & HAL_TIM_ACTIVE_CHANNEL_1) {
        err = vsf_timer_channel_config(timer, 0, &channel_cfg);
        if (err != VSF_ERR_NONE) {
            return HAL_ERROR;
        }
    }

    htim->State = HAL_TIM_STATE_READY;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_INSTANCE(htim->Instance));

    htim->State = HAL_TIM_STATE_BUSY;

    vsf_timer_fini(timer);

#   if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
    if (htim->Base_MspDeInitCallback == NULL) {
        htim->Base_MspDeInitCallback = HAL_TIM_Base_MspDeInit;
    }
    htim->Base_MspDeInitCallback(htim);
#   else
    HAL_TIM_Base_MspDeInit(htim);
#   endif

    htim->State = HAL_TIM_STATE_RESET;

    VSF_STHAL_UNLOCK(htim);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_INSTANCE(htim->Instance));

    htim->State = HAL_TIM_STATE_BUSY;

    vsf_err_t err = vsf_timer_channel_start(timer, 0);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }

    htim->State = HAL_TIM_STATE_READY;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_INSTANCE(htim->Instance));

    htim->State = HAL_TIM_STATE_BUSY;

    vsf_err_t err = vsf_timer_channel_stop(timer, 0);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }

    htim->State = HAL_TIM_STATE_READY;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_INSTANCE(htim->Instance));

    vsf_timer_irq_enable(timer, VSF_TIMER_IRQ_MASK_OVERFLOW);

    vsf_err_t err = vsf_timer_channel_start(timer, 0);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_INSTANCE(htim->Instance));

    vsf_err_t err = vsf_timer_channel_stop(timer, 0);
    vsf_timer_irq_disable(timer, VSF_TIMER_IRQ_MASK_OVERFLOW);

    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    } else {
        return HAL_OK;
    }
}

HAL_StatusTypeDef HAL_TIM_Base_Start_DMA(TIM_HandleTypeDef *htim,
                                         const uint32_t *pData, uint16_t Length)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_INSTANCE(htim->Instance));

#   ifndef VSF_TIMER_IRQ_MASK_REQUEST_CPL
    return HAL_ERROR;
#   else
    if ((htim->State == HAL_TIM_STATE_BUSY)) {
        return HAL_BUSY;
    } else if ((htim->State == HAL_TIM_STATE_READY)) {
        if ((pData == NULL) && (Length > 0U)) {
            return HAL_ERROR;
        } else {
            htim->State = HAL_TIM_STATE_BUSY;
        }
    }
    vsf_timer_irq_enable(timer, VSF_TIMER_IRQ_MASK_REQUEST_CPL);
    if (vsf_timer_channel_request_start(timer, 0, pData, Length) !=
        VSF_ERR_NONE) {
        return HAL_ERROR;
    } else {
        return HAL_OK;
    }
#   endif
}

HAL_StatusTypeDef HAL_TIM_Base_Stop_DMA(TIM_HandleTypeDef *htim)
{
#   ifndef VSF_TIMER_IRQ_MASK_REQUEST_CPL
    return HAL_ERROR;
#   else
    vsf_timer_irq_enable(timer, VSF_TIMER_IRQ_MASK_REQUEST_CPL);
    vsf_err_t err = vsf_timer_channel_request_stop(timer, 0);
    htim->State   = HAL_TIM_STATE_READY;
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    } else {
        return HAL_OK;
    }
#   endif
}

HAL_StatusTypeDef HAL_TIM_OC_Init(TIM_HandleTypeDef *htim)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OC_DeInit(TIM_HandleTypeDef *htim)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OC_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OC_Start_DMA(TIM_HandleTypeDef *htim,
                                       uint32_t Channel, const uint32_t *pData,
                                       uint16_t Length)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OC_ConfigChannel(TIM_HandleTypeDef        *htim,
                                           const TIM_OC_InitTypeDef *sConfig,
                                           uint32_t                  Channel)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef *htim,
                                         uint32_t           Channel)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef *htim,
                                        uint32_t           Channel)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel,
                                          const uint32_t    *pData,
                                          uint16_t           Length)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef *htim,
                                         uint32_t           Channel)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_PWM_Init(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_PWM_DeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_PWM_Start_IT(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_PWM_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim,
                                        uint32_t Channel, const uint32_t *pData,
                                        uint16_t Length)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    VSF_UNUSED_PARAM(pData);
    VSF_UNUSED_PARAM(Length);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_PWM_Stop_DMA(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_PWM_ConfigChannel(TIM_HandleTypeDef        *htim,
                                            const TIM_OC_InitTypeDef *sConfig,
                                            uint32_t                  Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(sConfig);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT(TIM_HandleTypeDef *htim,
                                         uint32_t           Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef *htim,
                                           uint32_t           Channel,
                                           const uint32_t    *pData,
                                           uint16_t           Length)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    VSF_UNUSED_PARAM(pData);
    VSF_UNUSED_PARAM(Length);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_IC_Init(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_IC_DeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_IC_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_IC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_IC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_IC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim,
                                       uint32_t Channel, uint32_t *pData,
                                       uint16_t Length)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    VSF_UNUSED_PARAM(pData);
    VSF_UNUSED_PARAM(Length);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_IC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_IC_ConfigChannel(TIM_HandleTypeDef        *htim,
                                           const TIM_IC_InitTypeDef *sConfig,
                                           uint32_t                  Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(sConfig);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_Init(TIM_HandleTypeDef *htim,
                                        uint32_t           OnePulseMode)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(OnePulseMode);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_DeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_Start(TIM_HandleTypeDef *htim,
                                         uint32_t           OutputChannel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(OutputChannel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_Stop(TIM_HandleTypeDef *htim,
                                        uint32_t           OutputChannel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(OutputChannel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_Start_IT(TIM_HandleTypeDef *htim,
                                            uint32_t           OutputChannel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(OutputChannel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_Stop_IT(TIM_HandleTypeDef *htim,
                                           uint32_t           OutputChannel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(OutputChannel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_ConfigChannel(
    TIM_HandleTypeDef *htim, TIM_OnePulse_InitTypeDef *sConfig,
    uint32_t OutputChannel, uint32_t InputChannel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(sConfig);
    VSF_UNUSED_PARAM(OutputChannel);
    VSF_UNUSED_PARAM(InputChannel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef *htim,
                                            uint32_t           OutputChannel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(OutputChannel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef *htim,
                                           uint32_t           OutputChannel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(OutputChannel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef *htim,
                                               uint32_t           OutputChannel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(OutputChannel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef *htim,
                                              uint32_t           OutputChannel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(OutputChannel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Init(TIM_HandleTypeDef             *htim,
                                       const TIM_Encoder_InitTypeDef *sConfig)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(sConfig);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_Encoder_DeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef *htim,
                                        uint32_t           Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Stop(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Start_IT(TIM_HandleTypeDef *htim,
                                           uint32_t           Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Stop_IT(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Start_DMA(TIM_HandleTypeDef *htim,
                                            uint32_t Channel, uint32_t *pData1,
                                            uint32_t *pData2, uint16_t Length)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    VSF_UNUSED_PARAM(pData1);
    VSF_UNUSED_PARAM(pData2);
    VSF_UNUSED_PARAM(Length);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Stop_DMA(TIM_HandleTypeDef *htim,
                                           uint32_t           Channel)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(Channel);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(
    TIM_HandleTypeDef *htim, const TIM_HallSensor_InitTypeDef *sConfig)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(sConfig);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef *htim,
                                                 uint32_t          *pData,
                                                 uint16_t           Length)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(pData);
    VSF_UNUSED_PARAM(Length);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStart(TIM_HandleTypeDef *htim,
                                              uint32_t        BurstBaseAddress,
                                              uint32_t        BurstRequestSrc,
                                              const uint32_t *BurstBuffer,
                                              uint32_t        BurstLength)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(BurstBaseAddress);
    VSF_UNUSED_PARAM(BurstRequestSrc);
    VSF_UNUSED_PARAM(BurstBuffer);
    VSF_UNUSED_PARAM(BurstLength);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_MultiWriteStart(TIM_HandleTypeDef *htim,
                                                   uint32_t BurstBaseAddress,
                                                   uint32_t BurstRequestSrc,
                                                   const uint32_t *BurstBuffer,
                                                   uint32_t        BurstLength,
                                                   uint32_t        DataLength)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(BurstBaseAddress);
    VSF_UNUSED_PARAM(BurstRequestSrc);
    VSF_UNUSED_PARAM(BurstBuffer);
    VSF_UNUSED_PARAM(BurstLength);
    VSF_UNUSED_PARAM(DataLength);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStop(TIM_HandleTypeDef *htim,
                                             uint32_t           BurstRequestSrc)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(BurstRequestSrc);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStart(TIM_HandleTypeDef *htim,
                                             uint32_t  BurstBaseAddress,
                                             uint32_t  BurstRequestSrc,
                                             uint32_t *BurstBuffer,
                                             uint32_t  BurstLength)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(BurstBaseAddress);
    VSF_UNUSED_PARAM(BurstRequestSrc);
    VSF_UNUSED_PARAM(BurstBuffer);
    VSF_UNUSED_PARAM(BurstLength);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_MultiReadStart(TIM_HandleTypeDef *htim,
                                                  uint32_t  BurstBaseAddress,
                                                  uint32_t  BurstRequestSrc,
                                                  uint32_t *BurstBuffer,
                                                  uint32_t  BurstLength,
                                                  uint32_t  DataLength)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(BurstBaseAddress);
    VSF_UNUSED_PARAM(BurstRequestSrc);
    VSF_UNUSED_PARAM(BurstBuffer);
    VSF_UNUSED_PARAM(BurstLength);
    VSF_UNUSED_PARAM(DataLength);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStop(TIM_HandleTypeDef *htim,
                                            uint32_t           BurstRequestSrc)
{
    VSF_UNUSED_PARAM(htim);
    VSF_UNUSED_PARAM(BurstRequestSrc);
    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_GenerateEvent(TIM_HandleTypeDef *htim,
                                        uint32_t           EventSource)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_INSTANCE(htim->Instance));
    VSF_STHAL_ASSERT(IS_TIM_EVENT_SOURCE(EventSource));

    VSF_STHAL_LOCK(htim);

    htim->State = HAL_TIM_STATE_BUSY;

#   if defined(VSF_TIMERL_CTRL_GENERATE_EVENT)
    // TODO: convert EventSource to vsf_timer_event_source_t
    vsf_err_t err = vsf_timer_channel_ctrl(
        timer, VSF_TIMERL_CTRL_GENERATE_EVENT,
        (void *)EventSource);
    VSF_STHAL_ASSERT(err == VSF_ERR_NONE);
#   endif

    htim->State = HAL_TIM_STATE_READY;

    VSF_STHAL_UNLOCK(htim);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_ConfigOCrefClear(
    TIM_HandleTypeDef                 *htim,
    const TIM_ClearInputConfigTypeDef *sClearInputConfig, uint32_t Channel)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_OCXREF_CLEAR_INSTANCE(htim->Instance));
    VSF_STHAL_ASSERT(
        IS_TIM_CLEARINPUT_SOURCE(sClearInputConfig->ClearInputSource));

    VSF_STHAL_LOCK(htim);

#   if defined(VSF_TIMERL_CHANNEL_CTRL_OUPUT_CAPTURE_CLEAR)
    // TODO: convert sClearInputConfig to vsf_timer_clear_input_config_t
    vsf_err_t err = vsf_timer_channel_ctrl(
        timer, VSF_TIMERL_CHANNEL_CTRL_OUPUT_CAPTURE_CLEAR,
        (void *)sClearInputConfig);
    VSF_STHAL_ASSERT(err == VSF_ERR_NONE);
#   endif

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_ConfigClockSource(
    TIM_HandleTypeDef *htim, const TIM_ClockConfigTypeDef *sClockSourceConfig)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);

    VSF_STHAL_LOCK(htim);

    htim->State = HAL_TIM_STATE_BUSY;

    VSF_STHAL_ASSERT(IS_TIM_CLOCKSOURCE(sClockSourceConfig->ClockSource));

#   if defined(VSF_TIMERL_CTRL_CLOCK_SOURCE)
    // TODO: convert sClockSourceConfig to vsf_timer_clock_source_t
    vsf_err_t err = vsf_timer_channel_ctrl(timer, VSF_TIMERL_CTRL_CLOCK_SOURCE,
                                           (void *)sClockSourceConfig);
    VSF_STHAL_ASSERT(err == VSF_ERR_NONE);
#   endif

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_ConfigTI1Input(TIM_HandleTypeDef *htim,
                                         uint32_t           TI1_Selection)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);

    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro(
    TIM_HandleTypeDef *htim, const TIM_SlaveConfigTypeDef *sSlaveConfig)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_SLAVE_INSTANCE(htim->Instance));
    VSF_STHAL_ASSERT(IS_TIM_SLAVE_MODE(sSlaveConfig->SlaveMode));
    VSF_STHAL_ASSERT(IS_TIM_TRIGGER_SELECTION(sSlaveConfig->InputTrigger));

    VSF_STHAL_LOCK(htim);

#   if defined(VSF_TIMERL_CTRL_SLAVE_SYNC)
    // TODO: convert sSlaveConfig to vsf_timer_slave_config_t
    vsf_err_t err = vsf_timer_channel_ctrl(timer, VSF_TIMERL_CTRL_SLAVE_SYNC,
                                           (void *)sSlaveConfig);
    VSF_STHAL_ASSERT(err == VSF_ERR_NONE);
#   endif

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro_IT(
    TIM_HandleTypeDef *htim, const TIM_SlaveConfigTypeDef *sSlaveConfig)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);

    VSF_STHAL_ASSERT(IS_TIM_SLAVE_INSTANCE(htim->Instance));
    VSF_STHAL_ASSERT(IS_TIM_SLAVE_MODE(sSlaveConfig->SlaveMode));
    VSF_STHAL_ASSERT(IS_TIM_TRIGGER_SELECTION(sSlaveConfig->InputTrigger));

#   if defined(VSF_TIMERL_CTRL_SLAVE_SYNC)
    // TODO: convert sSlaveConfig to vsf_timer_slave_config_t
    // TODO: enable slave sync interrupt
    vsf_err_t err = vsf_timer_channel_ctrl(timer, VSF_TIMERL_CTRL_SLAVE_SYNC,
                                           (void *)sSlaveConfig);
    VSF_STHAL_ASSERT(err == VSF_ERR_NONE);
#   endif

    return HAL_OK;
}

uint32_t HAL_TIM_ReadCapturedValue(const TIM_HandleTypeDef *htim,
                                   uint32_t                 Channel)
{
    VSF_STHAL_ASSERT(htim != NULL);

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    uint32_t tmpreg = 0U;

#   if defined(VSF_TIMER_CHANNEL_CTRL_READ_CAPTURE_VALUE)
    vsf_err_t err = vsf_timer_channel_ctrl(
        timer, VSF_TIMER_CHANNEL_CTRL_READ_CAPTURE_VALUE, (void *)tmpreg);
    VSF_STHAL_ASSERT(err == VSF_ERR_NONE);
#   endif

    return tmpreg;
}

#   if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)

HAL_StatusTypeDef HAL_TIM_RegisterCallback(TIM_HandleTypeDef        *htim,
                                           HAL_TIM_CallbackIDTypeDef CallbackID,
                                           pTIM_CallbackTypeDef      pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {
        return HAL_ERROR;
    }

    if (htim->State == HAL_TIM_STATE_READY) {
        switch (CallbackID) {
        case HAL_TIM_BASE_MSPINIT_CB_ID:
            htim->Base_MspInitCallback = pCallback;
            break;
        case HAL_TIM_BASE_MSPDEINIT_CB_ID:
            htim->Base_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_IC_MSPINIT_CB_ID:
            htim->IC_MspInitCallback = pCallback;
            break;
        case HAL_TIM_IC_MSPDEINIT_CB_ID:
            htim->IC_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_OC_MSPINIT_CB_ID:
            htim->OC_MspInitCallback = pCallback;
            break;
        case HAL_TIM_OC_MSPDEINIT_CB_ID:
            htim->OC_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_PWM_MSPINIT_CB_ID:
            htim->PWM_MspInitCallback = pCallback;
            break;
        case HAL_TIM_PWM_MSPDEINIT_CB_ID:
            htim->PWM_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_ONE_PULSE_MSPINIT_CB_ID:
            htim->OnePulse_MspInitCallback = pCallback;
            break;
        case HAL_TIM_ONE_PULSE_MSPDEINIT_CB_ID:
            htim->OnePulse_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_ENCODER_MSPINIT_CB_ID:
            htim->Encoder_MspInitCallback = pCallback;
            break;
        case HAL_TIM_ENCODER_MSPDEINIT_CB_ID:
            htim->Encoder_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_HALL_SENSOR_MSPINIT_CB_ID:
            htim->HallSensor_MspInitCallback = pCallback;
            break;
        case HAL_TIM_HALL_SENSOR_MSPDEINIT_CB_ID:
            htim->HallSensor_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_PERIOD_ELAPSED_CB_ID:
            htim->PeriodElapsedCallback = pCallback;
            break;
        case HAL_TIM_PERIOD_ELAPSED_HALF_CB_ID:
            htim->PeriodElapsedHalfCpltCallback = pCallback;
            break;
        case HAL_TIM_TRIGGER_CB_ID:
            htim->TriggerCallback = pCallback;
            break;
        case HAL_TIM_TRIGGER_HALF_CB_ID:
            htim->TriggerHalfCpltCallback = pCallback;
            break;
        case HAL_TIM_IC_CAPTURE_CB_ID:
            htim->IC_CaptureCallback = pCallback;
            break;
        case HAL_TIM_IC_CAPTURE_HALF_CB_ID:
            htim->IC_CaptureHalfCpltCallback = pCallback;
            break;
        case HAL_TIM_OC_DELAY_ELAPSED_CB_ID:
            htim->OC_DelayElapsedCallback = pCallback;
            break;
        case HAL_TIM_PWM_PULSE_FINISHED_CB_ID:
            htim->PWM_PulseFinishedCallback = pCallback;
            break;
        case HAL_TIM_PWM_PULSE_FINISHED_HALF_CB_ID:
            htim->PWM_PulseFinishedHalfCpltCallback = pCallback;
            break;
        case HAL_TIM_ERROR_CB_ID:
            htim->ErrorCallback = pCallback;
            break;
        case HAL_TIM_COMMUTATION_CB_ID:
            htim->CommutationCallback = pCallback;
            break;
        case HAL_TIM_COMMUTATION_HALF_CB_ID:
            htim->CommutationHalfCpltCallback = pCallback;
            break;
        case HAL_TIM_BREAK_CB_ID:
            htim->BreakCallback = pCallback;
            break;
        default:
            status = HAL_ERROR;
            break;
        }
    } else if (htim->State == HAL_TIM_STATE_RESET) {
        switch (CallbackID) {
        case HAL_TIM_BASE_MSPINIT_CB_ID:
            htim->Base_MspInitCallback = pCallback;
            break;
        case HAL_TIM_BASE_MSPDEINIT_CB_ID:
            htim->Base_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_IC_MSPINIT_CB_ID:
            htim->IC_MspInitCallback = pCallback;
            break;
        case HAL_TIM_IC_MSPDEINIT_CB_ID:
            htim->IC_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_OC_MSPINIT_CB_ID:
            htim->OC_MspInitCallback = pCallback;
            break;
        case HAL_TIM_OC_MSPDEINIT_CB_ID:
            htim->OC_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_PWM_MSPINIT_CB_ID:
            htim->PWM_MspInitCallback = pCallback;
            break;
        case HAL_TIM_PWM_MSPDEINIT_CB_ID:
            htim->PWM_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_ONE_PULSE_MSPINIT_CB_ID:
            htim->OnePulse_MspInitCallback = pCallback;
            break;
        case HAL_TIM_ONE_PULSE_MSPDEINIT_CB_ID:
            htim->OnePulse_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_ENCODER_MSPINIT_CB_ID:
            htim->Encoder_MspInitCallback = pCallback;
            break;
        case HAL_TIM_ENCODER_MSPDEINIT_CB_ID:
            htim->Encoder_MspDeInitCallback = pCallback;
            break;
        case HAL_TIM_HALL_SENSOR_MSPINIT_CB_ID:
            htim->HallSensor_MspInitCallback = pCallback;
            break;
        case HAL_TIM_HALL_SENSOR_MSPDEINIT_CB_ID:
            htim->HallSensor_MspDeInitCallback = pCallback;
            break;
        default:
            status = HAL_ERROR;
            break;
        }
    } else {
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef HAL_TIM_UnRegisterCallback(
    TIM_HandleTypeDef *htim, HAL_TIM_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (htim->State == HAL_TIM_STATE_READY) {
        switch (CallbackID) {
        case HAL_TIM_BASE_MSPINIT_CB_ID:
            htim->Base_MspInitCallback = HAL_TIM_Base_MspInit;
            break;
        case HAL_TIM_BASE_MSPDEINIT_CB_ID:
            htim->Base_MspDeInitCallback = HAL_TIM_Base_MspDeInit;
            break;
        case HAL_TIM_IC_MSPINIT_CB_ID:
            htim->IC_MspInitCallback = HAL_TIM_IC_MspInit;
            break;
        case HAL_TIM_IC_MSPDEINIT_CB_ID:
            htim->IC_MspDeInitCallback = HAL_TIM_IC_MspDeInit;
            break;
        case HAL_TIM_OC_MSPINIT_CB_ID:
            htim->OC_MspInitCallback = HAL_TIM_OC_MspInit;
            break;
        case HAL_TIM_OC_MSPDEINIT_CB_ID:
            htim->OC_MspDeInitCallback = HAL_TIM_OC_MspDeInit;
            break;
        case HAL_TIM_PWM_MSPINIT_CB_ID:
            htim->PWM_MspInitCallback = HAL_TIM_PWM_MspInit;
            break;
        case HAL_TIM_PWM_MSPDEINIT_CB_ID:
            htim->PWM_MspDeInitCallback = HAL_TIM_PWM_MspDeInit;
            break;
        case HAL_TIM_ONE_PULSE_MSPINIT_CB_ID:
            htim->OnePulse_MspInitCallback = HAL_TIM_OnePulse_MspInit;
            break;
        case HAL_TIM_ONE_PULSE_MSPDEINIT_CB_ID:
            htim->OnePulse_MspDeInitCallback = HAL_TIM_OnePulse_MspDeInit;
            break;
        case HAL_TIM_ENCODER_MSPINIT_CB_ID:
            htim->Encoder_MspInitCallback = HAL_TIM_Encoder_MspInit;
            break;
        case HAL_TIM_ENCODER_MSPDEINIT_CB_ID:
            htim->Encoder_MspDeInitCallback = HAL_TIM_Encoder_MspDeInit;
            break;
        case HAL_TIM_HALL_SENSOR_MSPINIT_CB_ID:
            htim->HallSensor_MspInitCallback = HAL_TIMEx_HallSensor_MspInit;
            break;
        case HAL_TIM_HALL_SENSOR_MSPDEINIT_CB_ID:
            htim->HallSensor_MspDeInitCallback = HAL_TIMEx_HallSensor_MspDeInit;
            break;
        case HAL_TIM_PERIOD_ELAPSED_CB_ID:
            htim->PeriodElapsedCallback = HAL_TIM_PeriodElapsedCallback;
            break;
        case HAL_TIM_PERIOD_ELAPSED_HALF_CB_ID:
            htim->PeriodElapsedHalfCpltCallback =
                HAL_TIM_PeriodElapsedHalfCpltCallback;
            break;
        case HAL_TIM_TRIGGER_CB_ID:
            htim->TriggerCallback = HAL_TIM_TriggerCallback;
            break;
        case HAL_TIM_TRIGGER_HALF_CB_ID:
            htim->TriggerHalfCpltCallback = HAL_TIM_TriggerHalfCpltCallback;
            break;
        case HAL_TIM_IC_CAPTURE_CB_ID:
            htim->IC_CaptureCallback = HAL_TIM_IC_CaptureCallback;
            break;
        case HAL_TIM_IC_CAPTURE_HALF_CB_ID:
            htim->IC_CaptureHalfCpltCallback =
                HAL_TIM_IC_CaptureHalfCpltCallback;
            break;
        case HAL_TIM_OC_DELAY_ELAPSED_CB_ID:
            htim->OC_DelayElapsedCallback = HAL_TIM_OC_DelayElapsedCallback;
            break;
        case HAL_TIM_PWM_PULSE_FINISHED_CB_ID:
            htim->PWM_PulseFinishedCallback = HAL_TIM_PWM_PulseFinishedCallback;
            break;
        case HAL_TIM_PWM_PULSE_FINISHED_HALF_CB_ID:
            htim->PWM_PulseFinishedHalfCpltCallback =
                HAL_TIM_PWM_PulseFinishedHalfCpltCallback;
            break;
        case HAL_TIM_ERROR_CB_ID:
            htim->ErrorCallback = HAL_TIM_ErrorCallback;
            break;
        case HAL_TIM_COMMUTATION_CB_ID:
            htim->CommutationCallback = HAL_TIMEx_CommutCallback;
            break;
        case HAL_TIM_COMMUTATION_HALF_CB_ID:
            htim->CommutationHalfCpltCallback =
                HAL_TIMEx_CommutHalfCpltCallback;
            break;
        case HAL_TIM_BREAK_CB_ID:
            htim->BreakCallback = HAL_TIMEx_BreakCallback;
            break;
        default:
            status = HAL_ERROR;
            break;
        }
    } else if (htim->State == HAL_TIM_STATE_RESET) {
        switch (CallbackID) {
        case HAL_TIM_BASE_MSPINIT_CB_ID:
            htim->Base_MspInitCallback = HAL_TIM_Base_MspInit;
            break;
        case HAL_TIM_BASE_MSPDEINIT_CB_ID:
            htim->Base_MspDeInitCallback = HAL_TIM_Base_MspDeInit;
            break;
        case HAL_TIM_IC_MSPINIT_CB_ID:
            htim->IC_MspInitCallback = HAL_TIM_IC_MspInit;
            break;
        case HAL_TIM_IC_MSPDEINIT_CB_ID:
            htim->IC_MspDeInitCallback = HAL_TIM_IC_MspDeInit;
            break;
        case HAL_TIM_OC_MSPINIT_CB_ID:
            htim->OC_MspInitCallback = HAL_TIM_OC_MspInit;
            break;
        case HAL_TIM_OC_MSPDEINIT_CB_ID:
            htim->OC_MspDeInitCallback = HAL_TIM_OC_MspDeInit;
            break;
        case HAL_TIM_PWM_MSPINIT_CB_ID:
            htim->PWM_MspInitCallback = HAL_TIM_PWM_MspInit;
            break;
        case HAL_TIM_PWM_MSPDEINIT_CB_ID:
            htim->PWM_MspDeInitCallback = HAL_TIM_PWM_MspDeInit;
            break;
        case HAL_TIM_ONE_PULSE_MSPINIT_CB_ID:
            htim->OnePulse_MspInitCallback = HAL_TIM_OnePulse_MspInit;
            break;
        case HAL_TIM_ONE_PULSE_MSPDEINIT_CB_ID:
            htim->OnePulse_MspDeInitCallback = HAL_TIM_OnePulse_MspDeInit;
            break;
        case HAL_TIM_ENCODER_MSPINIT_CB_ID:
            htim->Encoder_MspInitCallback = HAL_TIM_Encoder_MspInit;
            break;
        case HAL_TIM_ENCODER_MSPDEINIT_CB_ID:
            htim->Encoder_MspDeInitCallback = HAL_TIM_Encoder_MspDeInit;
            break;
        case HAL_TIM_HALL_SENSOR_MSPINIT_CB_ID:
            htim->HallSensor_MspInitCallback = HAL_TIMEx_HallSensor_MspInit;
            break;
        case HAL_TIM_HALL_SENSOR_MSPDEINIT_CB_ID:
            htim->HallSensor_MspDeInitCallback = HAL_TIMEx_HallSensor_MspDeInit;
            break;
        default:
            status = HAL_ERROR;
            break;
        }
    } else {

        status = HAL_ERROR;
    }

    return status;
}
#   endif

HAL_TIM_StateTypeDef HAL_TIM_Base_GetState(const TIM_HandleTypeDef *htim)
{
    VSF_STHAL_ASSERT(htim != NULL);
    return htim->State;
}

HAL_TIM_StateTypeDef HAL_TIM_OC_GetState(const TIM_HandleTypeDef *htim)
{
    VSF_STHAL_ASSERT(htim != NULL);
    return htim->State;
}

HAL_TIM_StateTypeDef HAL_TIM_PWM_GetState(const TIM_HandleTypeDef *htim)
{
    VSF_STHAL_ASSERT(htim != NULL);
    return htim->State;
}

HAL_TIM_StateTypeDef HAL_TIM_IC_GetState(const TIM_HandleTypeDef *htim)
{
    VSF_STHAL_ASSERT(htim != NULL);
    return htim->State;
}

HAL_TIM_StateTypeDef HAL_TIM_OnePulse_GetState(const TIM_HandleTypeDef *htim)
{
    VSF_STHAL_ASSERT(htim != NULL);
    return htim->State;
}

HAL_TIM_StateTypeDef HAL_TIM_Encoder_GetState(const TIM_HandleTypeDef *htim)
{
    VSF_STHAL_ASSERT(htim != NULL);
    return htim->State;
}

HAL_TIM_ActiveChannel HAL_TIM_GetActiveChannel(const TIM_HandleTypeDef *htim)
{
    VSF_STHAL_ASSERT(htim != NULL);
    return htim->Channel;
}

HAL_TIM_ChannelStateTypeDef HAL_TIM_GetChannelState(
    const TIM_HandleTypeDef *htim, uint32_t Channel)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    vsf_timer_status_t status = vsf_timer_status(timer);

#   if defined(VSF_TIMER_STATUS_BUSY_MASK) &&                                  \
       defined(VSF_TIMER_STATUS_BUSY_OFFSET)
    if (((status.value & VSF_TIMER_STATUS_BUSY_MASK) &
         (1 << (VSF_TIMER_STATUS_BUSY_OFFSET + Channel)))) {
        return HAL_TIM_CHANNEL_STATE_BUSY;
    }
#   endif

#   if defined(VSF_TIMER_STATUS_INIT_OFFSET) &&                                \
       defined(VSF_TIMER_STATUS_INIT_MASK)
    if (((status.value & VSF_TIMER_STATUS_INIT_MASK) &
         (1 << (VSF_TIMER_STATUS_BUSY_OFFSET + Channel)))) {
        return HAL_TIM_CHANNEL_STATE_RESET;
    } else {
        return HAL_TIM_CHANNEL_STATE_READY;
    }
#   else
    return HAL_TIM_CHANNEL_STATE_READY;
#   endif
}

HAL_TIM_ChannelStateTypeDef HAL_TIMEx_GetChannelNState(
    const TIM_HandleTypeDef *htim, uint32_t ChannelN)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    vsf_timer_status_t status = vsf_timer_status(timer);

#   if defined(VSF_TIMER_STATUS_N_BUSY_MASK) &&                                \
       defined(VSF_TIMER_STATUS_N_BUSY_OFFSET)
    if (((status.value & VSF_TIMER_STATUS_N_BUSY_MASK) &
         (1 << (VSF_TIMER_STATUS_N_BUSY_OFFSET + ChannelN)))) {
        return HAL_TIM_CHANNEL_STATE_BUSY;
    }
#   endif

#   if defined(VSF_TIMER_STATUS_N_INIT_OFFSET) &&                              \
       defined(VSF_TIMER_STATUS_N_INIT_MASK)
    if (((status.value & VSF_TIMER_STATUS_N_INIT_MASK) &
         (1 << (VSF_TIMER_STATUS_N_BUSY_OFFSET + ChannelN)))) {
        return HAL_TIM_CHANNEL_STATE_RESET;
    } else {
        return HAL_TIM_CHANNEL_STATE_READY;
    }
#   else
    return HAL_TIM_CHANNEL_STATE_READY;
#   endif
}

HAL_TIM_DMABurstStateTypeDef HAL_TIM_DMABurstState(
    const TIM_HandleTypeDef *htim)
{
    VSF_STHAL_ASSERT(htim != NULL);

    return htim->DMABurstState;
}

void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim) {}

#   if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
void TIM_ResetCallback(TIM_HandleTypeDef *htim)
{

    htim->PeriodElapsedCallback         = HAL_TIM_PeriodElapsedCallback;
    htim->PeriodElapsedHalfCpltCallback = HAL_TIM_PeriodElapsedHalfCpltCallback;
    htim->TriggerCallback               = HAL_TIM_TriggerCallback;
    htim->TriggerHalfCpltCallback       = HAL_TIM_TriggerHalfCpltCallback;
    htim->IC_CaptureCallback            = HAL_TIM_IC_CaptureCallback;
    htim->IC_CaptureHalfCpltCallback    = HAL_TIM_IC_CaptureHalfCpltCallback;
    htim->OC_DelayElapsedCallback       = HAL_TIM_OC_DelayElapsedCallback;
    htim->PWM_PulseFinishedCallback     = HAL_TIM_PWM_PulseFinishedCallback;
    htim->PWM_PulseFinishedHalfCpltCallback =
        HAL_TIM_PWM_PulseFinishedHalfCpltCallback;
    htim->ErrorCallback               = HAL_TIM_ErrorCallback;
    htim->CommutationCallback         = HAL_TIMEx_CommutCallback;
    htim->CommutationHalfCpltCallback = HAL_TIMEx_CommutHalfCpltCallback;
    htim->BreakCallback               = HAL_TIMEx_BreakCallback;
}
#   endif

HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent(TIM_HandleTypeDef *htim,
                                              uint32_t           InputTrigger,
                                              uint32_t CommutationSource)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_COMMUTATION_EVENT_INSTANCE(htim->Instance));
    VSF_STHAL_ASSERT(IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(InputTrigger));

    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_IT(TIM_HandleTypeDef *htim,
                                                 uint32_t InputTrigger,
                                                 uint32_t CommutationSource)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_COMMUTATION_EVENT_INSTANCE(htim->Instance));
    VSF_STHAL_ASSERT(IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(InputTrigger));

    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_DMA(TIM_HandleTypeDef *htim,
                                                  uint32_t InputTrigger,
                                                  uint32_t CommutationSource)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);
    VSF_STHAL_ASSERT(IS_TIM_COMMUTATION_EVENT_INSTANCE(htim->Instance));
    VSF_STHAL_ASSERT(IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(InputTrigger));

    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(
    TIM_HandleTypeDef *htim, const TIM_MasterConfigTypeDef *sMasterConfig)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);

    VSF_STHAL_ASSERT(IS_TIM_SYNCHRO_INSTANCE(htim->Instance));
    VSF_STHAL_ASSERT(IS_TIM_TRGO_SOURCE(sMasterConfig->MasterOutputTrigger));
    VSF_STHAL_ASSERT(IS_TIM_MSM_STATE(sMasterConfig->MasterSlaveMode));

    return HAL_ERROR;
}

HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(
    TIM_HandleTypeDef                    *htim,
    const TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);

    VSF_STHAL_ASSERT(IS_TIM_BREAK_INSTANCE(htim->Instance));
    VSF_STHAL_ASSERT(IS_TIM_OSSR_STATE(sBreakDeadTimeConfig->OffStateRunMode));
    VSF_STHAL_ASSERT(IS_TIM_OSSI_STATE(sBreakDeadTimeConfig->OffStateIDLEMode));
    VSF_STHAL_ASSERT(IS_TIM_LOCK_LEVEL(sBreakDeadTimeConfig->LockLevel));
    VSF_STHAL_ASSERT(IS_TIM_DEADTIME(sBreakDeadTimeConfig->DeadTime));
    VSF_STHAL_ASSERT(IS_TIM_BREAK_STATE(sBreakDeadTimeConfig->BreakState));
    VSF_STHAL_ASSERT(
        IS_TIM_BREAK_POLARITY(sBreakDeadTimeConfig->BreakPolarity));
    VSF_STHAL_ASSERT(
        IS_TIM_AUTOMATIC_OUTPUT_STATE(sBreakDeadTimeConfig->AutomaticOutput));

#   ifdef VSF_TIMER_CTRL_BREAK_DEAD_TIME
    // TODO: need daed time support in vsf_template_timer.h
    VSF_STHAL_ASSERT(IS_TIM_REMAP(htim->Instance, Remap));
    VSF_STHAL_LOCK(htim);
    vsf_err_t err = vsf_timer_ctrl(timer, VSF_TIMER_CTRL_BREAK_DEAD_TIME,
                                   (void *)sBreakDeadTimeConfig);
    VSF_STHAL_UNLOCK(htim);
    if (err < 0) {
        return HAL_ERROR;
    } else {
        return HAL_OK;
    }
#   else
    return HAL_ERROR;
#   endif
}

HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim, uint32_t Remap)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    vsf_timer_t *timer = (vsf_timer_t *)htim->Instance;
    VSF_STHAL_ASSERT(timer != NULL);

#   ifdef VSF_TIMER_CTRL_REMAP
    VSF_STHAL_ASSERT(IS_TIM_REMAP(htim->Instance, Remap));
    VSF_STHAL_LOCK(htim);
    vsf_err_t err = vsf_timer_ctrl(timer, VSF_TIMER_CTRL_REMAP, (void *)Remap);
    VSF_STHAL_UNLOCK(htim);

    if (err < 0) {
        return HAL_ERROR;
    } else {
        return HAL_OK;
    }
#   else
    return HAL_ERROR;
#   endif
}

HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(
    const TIM_HandleTypeDef *htim)
{
    return htim->State;
}

#endif
