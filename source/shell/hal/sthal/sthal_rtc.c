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

#ifdef HAL_RTC_MODULE_ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

HAL_StatusTypeDef HAL_RTC_Init(RTC_HandleTypeDef *hrtc)
{
    HAL_StatusTypeDef status = HAL_ERROR;
    if (hrtc == NULL) {
        return HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef HAL_RTC_DeInit(RTC_HandleTypeDef *hrtc)
{
    HAL_StatusTypeDef status = HAL_ERROR;
    return status;
}

#    if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)

HAL_StatusTypeDef HAL_RTC_RegisterCallback(RTC_HandleTypeDef        *hrtc,
                                           HAL_RTC_CallbackIDTypeDef CallbackID,
                                           pRTC_CallbackTypeDef      pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {
        return HAL_ERROR;
    }

    VSF_STHAL_LOCK(hrtc);
    if (HAL_RTC_STATE_READY == hrtc->State) {
        switch (CallbackID) {
        case HAL_RTC_ALARM_A_EVENT_CB_ID:
            hrtc->AlarmAEventCallback = pCallback;
            break;
        case HAL_RTC_ALARM_B_EVENT_CB_ID:
            hrtc->AlarmBEventCallback = pCallback;
            break;
        case HAL_RTC_TIMESTAMP_EVENT_CB_ID:
            hrtc->TimeStampEventCallback = pCallback;
            break;
        case HAL_RTC_WAKEUPTIMER_EVENT_CB_ID:
            hrtc->WakeUpTimerEventCallback = pCallback;
            break;
        case HAL_RTC_TAMPER1_EVENT_CB_ID:
            hrtc->Tamper1EventCallback = pCallback;
            break;

#        if defined(RTC_TAMPER2_SUPPORT)
        case HAL_RTC_TAMPER2_EVENT_CB_ID:
            hrtc->Tamper2EventCallback = pCallback;
            break;
#        endif

        case HAL_RTC_MSPINIT_CB_ID:
            hrtc->MspInitCallback = pCallback;
            break;
        case HAL_RTC_MSPDEINIT_CB_ID:
            hrtc->MspDeInitCallback = pCallback;
            break;
        default:

            status = HAL_ERROR;
            break;
        }
    } else if (HAL_RTC_STATE_RESET == hrtc->State) {
        switch (CallbackID) {
        case HAL_RTC_MSPINIT_CB_ID:
            hrtc->MspInitCallback = pCallback;
            break;
        case HAL_RTC_MSPDEINIT_CB_ID:
            hrtc->MspDeInitCallback = pCallback;
            break;
        default:
            status = HAL_ERROR;
            break;
        }
    } else {
        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hrtc);
    return status;
}

HAL_StatusTypeDef HAL_RTC_UnRegisterCallback(
    RTC_HandleTypeDef *hrtc, HAL_RTC_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;
    VSF_STHAL_LOCK(hrtc);
    if (HAL_RTC_STATE_READY == hrtc->State) {
        switch (CallbackID) {
        case HAL_RTC_ALARM_A_EVENT_CB_ID:
            hrtc->AlarmAEventCallback = HAL_RTC_AlarmAEventCallback;
            break;
        case HAL_RTC_ALARM_B_EVENT_CB_ID:
            hrtc->AlarmBEventCallback = HAL_RTCEx_AlarmBEventCallback;
            break;
        case HAL_RTC_TIMESTAMP_EVENT_CB_ID:
            hrtc->TimeStampEventCallback = HAL_RTCEx_TimeStampEventCallback;
            break;
        case HAL_RTC_WAKEUPTIMER_EVENT_CB_ID:
            hrtc->WakeUpTimerEventCallback = HAL_RTCEx_WakeUpTimerEventCallback;
            break;
        case HAL_RTC_TAMPER1_EVENT_CB_ID:
            hrtc->Tamper1EventCallback = HAL_RTCEx_Tamper1EventCallback;
            break;

#        if defined(RTC_TAMPER2_SUPPORT)
        case HAL_RTC_TAMPER2_EVENT_CB_ID:
            hrtc->Tamper2EventCallback = HAL_RTCEx_Tamper2EventCallback;
            break;
#        endif

        case HAL_RTC_MSPINIT_CB_ID:
            hrtc->MspInitCallback = HAL_RTC_MspInit;
            break;
        case HAL_RTC_MSPDEINIT_CB_ID:
            hrtc->MspDeInitCallback = HAL_RTC_MspDeInit;
            break;
        default:
            status = HAL_ERROR;
            break;
        }
    } else if (HAL_RTC_STATE_RESET == hrtc->State) {
        switch (CallbackID) {
        case HAL_RTC_MSPINIT_CB_ID:
            hrtc->MspInitCallback = HAL_RTC_MspInit;
            break;
        case HAL_RTC_MSPDEINIT_CB_ID:
            hrtc->MspDeInitCallback = HAL_RTC_MspDeInit;
            break;
        default:
            status = HAL_ERROR;
            break;
        }
    } else {
        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hrtc);
    return status;
}
#    endif

VSF_CAL_WEAK(HAL_RTC_MspInit)
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    VSF_UNUSED_PARAM(hrtc);
}

VSF_CAL_WEAK(HAL_RTC_MspDeInit)
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
    VSF_UNUSED_PARAM(hrtc);
}

HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc,
                                  RTC_TimeTypeDef *sTime, uint32_t Format)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc,
                                  RTC_TimeTypeDef *sTime, uint32_t Format)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc,
                                  RTC_DateTypeDef *sDate, uint32_t Format)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc,
                                  RTC_DateTypeDef *sDate, uint32_t Format)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc,
                                   RTC_AlarmTypeDef *sAlarm, uint32_t Format)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTC_SetAlarm_IT(RTC_HandleTypeDef *hrtc,
                                      RTC_AlarmTypeDef *sAlarm, uint32_t Format)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc,
                                          uint32_t           Alarm)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTC_GetAlarm(RTC_HandleTypeDef *hrtc,
                                   RTC_AlarmTypeDef *sAlarm, uint32_t Alarm,
                                   uint32_t Format)
{
    return HAL_OK;
}

void HAL_RTC_AlarmIRQHandler(RTC_HandleTypeDef *hrtc) {}

VSF_CAL_WEAK(HAL_RTC_AlarmAEventCallback)
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    VSF_UNUSED_PARAM(hrtc);
}

HAL_StatusTypeDef HAL_RTC_PollForAlarmAEvent(RTC_HandleTypeDef *hrtc,
                                             uint32_t           Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTC_WaitForSynchro(RTC_HandleTypeDef *hrtc)
{
    return HAL_OK;
}

void HAL_RTC_DST_Add1Hour(RTC_HandleTypeDef *hrtc) {}

void HAL_RTC_DST_Sub1Hour(RTC_HandleTypeDef *hrtc) {}

void HAL_RTC_DST_SetStoreOperation(RTC_HandleTypeDef *hrtc) {}

void HAL_RTC_DST_ClearStoreOperation(RTC_HandleTypeDef *hrtc) {}

uint32_t HAL_RTC_DST_ReadStoreOperation(RTC_HandleTypeDef *hrtc)
{
    return 0;
}

HAL_RTCStateTypeDef HAL_RTC_GetState(RTC_HandleTypeDef *hrtc)
{
    return HAL_RTC_STATE_READY;
}

HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef *hrtc)
{
    return HAL_OK;
}

HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef *hrtc)
{
    return HAL_OK;
}

uint8_t RTC_ByteToBcd2(uint8_t number)
{
    uint32_t bcdhigh = 0U;
    while (number >= 10U) {
        bcdhigh++;
        number -= 10U;
    }

    return ((uint8_t)(bcdhigh << 4U) | number);
}

uint8_t RTC_Bcd2ToByte(uint8_t number)
{
    uint32_t tens = 0U;
    tens          = (((uint32_t)number & 0xF0U) >> 4U) * 10U;
    return (uint8_t)(tens + ((uint32_t)number & 0x0FU));
}

HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp(RTC_HandleTypeDef *hrtc,
                                         uint32_t           RTC_TimeStampEdge,
                                         uint32_t           RTC_TimeStampPin)
{
    return HAL_OK;
}
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp_IT(RTC_HandleTypeDef *hrtc,
                                            uint32_t RTC_TimeStampEdge,
                                            uint32_t RTC_TimeStampPin)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_DeactivateTimeStamp(RTC_HandleTypeDef *hrtc)

{
    return HAL_OK;
}
HAL_StatusTypeDef HAL_RTCEx_GetTimeStamp(RTC_HandleTypeDef *hrtc,
                                         RTC_TimeTypeDef   *sTimeStamp,
                                         RTC_DateTypeDef   *sTimeStampDate,
                                         uint32_t           Format)

{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_SetTamper(RTC_HandleTypeDef *hrtc,
                                      RTC_TamperTypeDef *sTamper)

{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_SetTamper_IT(RTC_HandleTypeDef *hrtc,
                                         RTC_TamperTypeDef *sTamper)

{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_DeactivateTamper(RTC_HandleTypeDef *hrtc,
                                             uint32_t           Tamper)

{
    return HAL_OK;
}

void HAL_RTCEx_TamperTimeStampIRQHandler(RTC_HandleTypeDef *hrtc) {}

void HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef *hrtc) {}

#    if defined(RTC_TAMPER2_SUPPORT)
void HAL_RTCEx_Tamper2EventCallback(RTC_HandleTypeDef *hrtc) {}
#    endif /* RTC_TAMPER2_SUPPORT */
void HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc) {}

HAL_StatusTypeDef HAL_RTCEx_PollForTimeStampEvent(RTC_HandleTypeDef *hrtc,
                                                  uint32_t           Timeout)

{
    return HAL_OK;
}
HAL_StatusTypeDef HAL_RTCEx_PollForTamper1Event(RTC_HandleTypeDef *hrtc,
                                                uint32_t           Timeout)

{
    return HAL_OK;
}
#    if defined(RTC_TAMPER2_SUPPORT)
HAL_StatusTypeDef HAL_RTCEx_PollForTamper2Event(RTC_HandleTypeDef *hrtc,
                                                uint32_t           Timeout)
{
}
#    endif /* RTC_TAMPER2_SUPPORT */

HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer(RTC_HandleTypeDef *hrtc,
                                           uint32_t           WakeUpCounter,
                                           uint32_t           WakeUpClock)

{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer_IT(RTC_HandleTypeDef *hrtc,
                                              uint32_t           WakeUpCounter,
                                              uint32_t           WakeUpClock)

{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_DeactivateWakeUpTimer(RTC_HandleTypeDef *hrtc)

{
    return HAL_OK;
}

uint32_t HAL_RTCEx_GetWakeUpTimer(RTC_HandleTypeDef *hrtc)
{
    return 0;
}

void HAL_RTCEx_WakeUpTimerIRQHandler(RTC_HandleTypeDef *hrtc) {}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc) {}

HAL_StatusTypeDef HAL_RTCEx_PollForWakeUpTimerEvent(RTC_HandleTypeDef *hrtc,
                                                    uint32_t           Timeout)

{
    return HAL_OK;
}

void HAL_RTCEx_BKUPWrite(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister,
                         uint32_t Data)
{
}

uint32_t HAL_RTCEx_BKUPRead(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister)
{
    return 0;
}

HAL_StatusTypeDef HAL_RTCEx_SetCoarseCalib(RTC_HandleTypeDef *hrtc,
                                           uint32_t CalibSign, uint32_t Value)

{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_DeactivateCoarseCalib(RTC_HandleTypeDef *hrtc)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_SetSmoothCalib(RTC_HandleTypeDef *hrtc,
                                           uint32_t           SmoothCalibPeriod,
                                           uint32_t SmoothCalibPlusPulses,
                                           uint32_t SmoothCalibMinusPulsesValue)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_SetSynchroShift(RTC_HandleTypeDef *hrtc,
                                            uint32_t           ShiftAdd1S,
                                            uint32_t           ShiftSubFS)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_SetCalibrationOutPut(RTC_HandleTypeDef *hrtc,
                                                 uint32_t           CalibOutput)

{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_DeactivateCalibrationOutPut(
    RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_SetRefClock(RTC_HandleTypeDef *hrtc)

{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_DeactivateRefClock(RTC_HandleTypeDef *hrtc)

{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_EnableBypassShadow(RTC_HandleTypeDef *hrtc)

{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_RTCEx_DisableBypassShadow(RTC_HandleTypeDef *hrtc)

{
    return HAL_OK;
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc) {}

HAL_StatusTypeDef HAL_RTCEx_PollForAlarmBEvent(RTC_HandleTypeDef *hrtc,
                                               uint32_t           Timeout)
{
    return HAL_OK;
}

#endif
