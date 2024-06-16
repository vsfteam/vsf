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

#ifdef HAL_TIM_MODULE_ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim)
{
    if (htim == NULL) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

__weak void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Start_DMA(TIM_HandleTypeDef *htim,
                                         const uint32_t *pData, uint16_t Length)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Stop_DMA(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OC_Init(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OC_DeInit(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

__weak void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

HAL_StatusTypeDef HAL_TIM_OC_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OC_Start_DMA(TIM_HandleTypeDef *htim,
                                       uint32_t Channel, const uint32_t *pData,
                                       uint16_t Length)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_Init(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_DeInit(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

__weak void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_Start_IT(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim,
                                        uint32_t Channel, const uint32_t *pData,
                                        uint16_t Length)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_Stop_DMA(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_IC_Init(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_IC_DeInit(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

__weak void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

HAL_StatusTypeDef HAL_TIM_IC_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_IC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_IC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_IC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim,
                                       uint32_t Channel, uint32_t *pData,
                                       uint16_t Length)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_IC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_Init(TIM_HandleTypeDef *htim,
                                        uint32_t           OnePulseMode)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_DeInit(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

__weak void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_OnePulse_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

HAL_StatusTypeDef HAL_TIM_OnePulse_Start(TIM_HandleTypeDef *htim,
                                         uint32_t           OutputChannel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_Stop(TIM_HandleTypeDef *htim,
                                        uint32_t           OutputChannel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_Start_IT(TIM_HandleTypeDef *htim,
                                            uint32_t           OutputChannel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_Stop_IT(TIM_HandleTypeDef *htim,
                                           uint32_t           OutputChannel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Init(TIM_HandleTypeDef             *htim,
                                       const TIM_Encoder_InitTypeDef *sConfig)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Encoder_DeInit(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

__weak void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef *htim,
                                        uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Stop(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Start_IT(TIM_HandleTypeDef *htim,
                                           uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Stop_IT(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Start_DMA(TIM_HandleTypeDef *htim,
                                            uint32_t Channel, uint32_t *pData1,
                                            uint32_t *pData2, uint16_t Length)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Stop_DMA(TIM_HandleTypeDef *htim,
                                           uint32_t           Channel)
{
    return HAL_OK;
}

void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim) {}

HAL_StatusTypeDef HAL_TIM_OC_ConfigChannel(TIM_HandleTypeDef        *htim,
                                           const TIM_OC_InitTypeDef *sConfig,
                                           uint32_t                  Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_IC_ConfigChannel(TIM_HandleTypeDef        *htim,
                                           const TIM_IC_InitTypeDef *sConfig,
                                           uint32_t                  Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_ConfigChannel(TIM_HandleTypeDef        *htim,
                                            const TIM_OC_InitTypeDef *sConfig,
                                            uint32_t                  Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_OnePulse_ConfigChannel(
    TIM_HandleTypeDef *htim, TIM_OnePulse_InitTypeDef *sConfig,
    uint32_t OutputChannel, uint32_t InputChannel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStart(TIM_HandleTypeDef *htim,
                                              uint32_t        BurstBaseAddress,
                                              uint32_t        BurstRequestSrc,
                                              const uint32_t *BurstBuffer,
                                              uint32_t        BurstLength)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_MultiWriteStart(TIM_HandleTypeDef *htim,
                                                   uint32_t BurstBaseAddress,
                                                   uint32_t BurstRequestSrc,
                                                   const uint32_t *BurstBuffer,
                                                   uint32_t        BurstLength,
                                                   uint32_t        DataLength)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStop(TIM_HandleTypeDef *htim,
                                             uint32_t           BurstRequestSrc)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStart(TIM_HandleTypeDef *htim,
                                             uint32_t  BurstBaseAddress,
                                             uint32_t  BurstRequestSrc,
                                             uint32_t *BurstBuffer,
                                             uint32_t  BurstLength)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_MultiReadStart(TIM_HandleTypeDef *htim,
                                                  uint32_t  BurstBaseAddress,
                                                  uint32_t  BurstRequestSrc,
                                                  uint32_t *BurstBuffer,
                                                  uint32_t  BurstLength,
                                                  uint32_t  DataLength)
{
    HAL_StatusTypeDef status = HAL_OK;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStop(TIM_HandleTypeDef *htim,
                                            uint32_t           BurstRequestSrc)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_GenerateEvent(TIM_HandleTypeDef *htim,
                                        uint32_t           EventSource)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_ConfigOCrefClear(
    TIM_HandleTypeDef                 *htim,
    const TIM_ClearInputConfigTypeDef *sClearInputConfig, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_ConfigClockSource(
    TIM_HandleTypeDef *htim, const TIM_ClockConfigTypeDef *sClockSourceConfig)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_ConfigTI1Input(TIM_HandleTypeDef *htim,
                                         uint32_t           TI1_Selection)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro(
    TIM_HandleTypeDef *htim, const TIM_SlaveConfigTypeDef *sSlaveConfig)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro_IT(
    TIM_HandleTypeDef *htim, const TIM_SlaveConfigTypeDef *sSlaveConfig)
{
    return HAL_OK;
}

uint32_t HAL_TIM_ReadCapturedValue(const TIM_HandleTypeDef *htim,
                                   uint32_t                 Channel)
{
    uint32_t tmpreg = 0U;
    switch (Channel) {
    case TIM_CHANNEL_1: {
        break;
    }
    case TIM_CHANNEL_2: {
        break;
    }

    case TIM_CHANNEL_3: {
        break;
    }

    case TIM_CHANNEL_4: {
        break;
    }

    default:
        break;
    }

    return tmpreg;
}

__weak void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_TriggerHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

__weak void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
{
    VSF_UNUSED_PARAM(htim);
}

#    if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)

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
#    endif

HAL_TIM_StateTypeDef HAL_TIM_Base_GetState(const TIM_HandleTypeDef *htim)
{
    return htim->State;
}

HAL_TIM_StateTypeDef HAL_TIM_OC_GetState(const TIM_HandleTypeDef *htim)
{
    return htim->State;
}

HAL_TIM_StateTypeDef HAL_TIM_PWM_GetState(const TIM_HandleTypeDef *htim)
{
    return htim->State;
}

HAL_TIM_StateTypeDef HAL_TIM_IC_GetState(const TIM_HandleTypeDef *htim)
{
    return htim->State;
}

HAL_TIM_StateTypeDef HAL_TIM_OnePulse_GetState(const TIM_HandleTypeDef *htim)
{
    return htim->State;
}

HAL_TIM_StateTypeDef HAL_TIM_Encoder_GetState(const TIM_HandleTypeDef *htim)
{
    return htim->State;
}

HAL_TIM_ActiveChannel HAL_TIM_GetActiveChannel(const TIM_HandleTypeDef *htim)
{
    return htim->Channel;
}

HAL_TIM_ChannelStateTypeDef HAL_TIM_GetChannelState(
    const TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_TIM_CHANNEL_STATE_READY;
}

HAL_TIM_DMABurstStateTypeDef HAL_TIM_DMABurstState(
    const TIM_HandleTypeDef *htim)
{
    return htim->DMABurstState;
}

void TIM_DMAError(DMA_HandleTypeDef *hdma) {}

void TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma) {}

void TIM_DMACaptureCplt(DMA_HandleTypeDef *hdma) {}

void TIM_DMACaptureHalfCplt(DMA_HandleTypeDef *hdma) {}

void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, const TIM_OC_InitTypeDef *OC_Config)
{
}

void TIM_TI1_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity,
                       uint32_t TIM_ICSelection, uint32_t TIM_ICFilter)
{
}

void TIM_ETR_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ExtTRGPrescaler,
                       uint32_t TIM_ExtTRGPolarity, uint32_t ExtTRGFilter)
{
}

void TIM_CCxChannelCmd(TIM_TypeDef *TIMx, uint32_t Channel,
                       uint32_t ChannelState)
{
}

#    if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
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
#    endif

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(
    TIM_HandleTypeDef *htim, const TIM_HallSensor_InitTypeDef *sConfig)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim) {}

void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim) {}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef *htim,
                                                 uint32_t          *pData,
                                                 uint16_t           Length)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef *htim)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef *htim,
                                         uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef *htim,
                                        uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel,
                                          const uint32_t    *pData,
                                          uint16_t           Length)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef *htim,
                                         uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT(TIM_HandleTypeDef *htim,
                                         uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef *htim,
                                           uint32_t           Channel,
                                           const uint32_t    *pData,
                                           uint16_t           Length)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef *htim,
                                            uint32_t           OutputChannel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef *htim,
                                           uint32_t           OutputChannel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef *htim,
                                               uint32_t           OutputChannel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef *htim,
                                              uint32_t           OutputChannel)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent(TIM_HandleTypeDef *htim,
                                              uint32_t           InputTrigger,
                                              uint32_t CommutationSource)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_IT(TIM_HandleTypeDef *htim,
                                                 uint32_t InputTrigger,
                                                 uint32_t CommutationSource)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_DMA(TIM_HandleTypeDef *htim,
                                                  uint32_t InputTrigger,
                                                  uint32_t CommutationSource)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(
    TIM_HandleTypeDef *htim, const TIM_MasterConfigTypeDef *sMasterConfig)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(
    TIM_HandleTypeDef                    *htim,
    const TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim, uint32_t Remap)
{
    return HAL_OK;
}

void HAL_TIMEx_CommutCallback(TIM_HandleTypeDef *htim) {}

void HAL_TIMEx_CommutHalfCpltCallback(TIM_HandleTypeDef *htim) {}

void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim) {}

HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(
    const TIM_HandleTypeDef *htim)
{
    return HAL_TIM_STATE_READY;
}

HAL_TIM_ChannelStateTypeDef HAL_TIMEx_GetChannelNState(
    const TIM_HandleTypeDef *htim, uint32_t ChannelN)
{
    return HAL_TIM_CHANNEL_STATE_READY;
}

void TIMEx_DMACommutationCplt(DMA_HandleTypeDef *hdma) {}

void TIMEx_DMACommutationHalfCplt(DMA_HandleTypeDef *hdma) {}

#endif
