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

#ifndef __ST_HAL_RTC_H__
#define __ST_HAL_RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HAL_RTC_MODULE_ENABLED) && VSF_HAL_USE_RTC == ENABLED

/*============================ INCLUDES ======================================*/

#include "sthal_def.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef vsf_rtc_t RTC_TypeDef;

typedef enum {
    HAL_RTC_STATE_RESET   = 0x00U,
    HAL_RTC_STATE_READY   = 0x01U,
    HAL_RTC_STATE_BUSY    = 0x02U,
    HAL_RTC_STATE_TIMEOUT = 0x03U,
    HAL_RTC_STATE_ERROR   = 0x04U,
} HAL_RTCStateTypeDef;

typedef struct {
    uint32_t HourFormat;
    uint32_t AsynchPrediv;
    uint32_t SynchPrediv;
    uint32_t OutPut;
    uint32_t OutPutPolarity;
    uint32_t OutPutType;
} RTC_InitTypeDef;

typedef struct {
    uint8_t  Hours;
    uint8_t  Minutes;
    uint8_t  Seconds;
    uint8_t  TimeFormat;
    uint32_t SubSeconds;
    uint32_t SecondFraction;
    uint32_t DayLightSaving;
    uint32_t StoreOperation;
} RTC_TimeTypeDef;

typedef struct {
    uint8_t WeekDay;
    uint8_t Month;
    uint8_t Date;
    uint8_t Year;
} RTC_DateTypeDef;

typedef struct {
    RTC_TimeTypeDef AlarmTime;
    uint32_t        AlarmMask;
    uint32_t        AlarmSubSecondMask;
    uint32_t        AlarmDateWeekDaySel;
    uint8_t         AlarmDateWeekDay;
    uint32_t        Alarm;
} RTC_AlarmTypeDef;

#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
typedef struct __RTC_HandleTypeDef {
#else
typedef struct {
#endif
    RTC_TypeDef             *Instance;
    RTC_InitTypeDef          Init;
    HAL_LockTypeDef          Lock;
    volatile HAL_RTCStateTypeDef State;
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    void (*AlarmAEventCallback)(struct __RTC_HandleTypeDef *hrtc);
    void (*AlarmBEventCallback)(struct __RTC_HandleTypeDef *hrtc);
    void (*TimeStampEventCallback)(struct __RTC_HandleTypeDef *hrtc);
    void (*WakeUpTimerEventCallback)(struct __RTC_HandleTypeDef *hrtc);
    void (*Tamper1EventCallback)(struct __RTC_HandleTypeDef *hrtc);
#    if defined(RTC_TAMPER2_SUPPORT)
    void (*Tamper2EventCallback)(struct __RTC_HandleTypeDef *hrtc);
#    endif
    void (*MspInitCallback)(struct __RTC_HandleTypeDef *hrtc);
    void (*MspDeInitCallback)(struct __RTC_HandleTypeDef *hrtc);
#endif
} RTC_HandleTypeDef;

#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
typedef enum {
    HAL_RTC_ALARM_A_EVENT_CB_ID     = 0x00U,
    HAL_RTC_ALARM_B_EVENT_CB_ID     = 0x01U,
    HAL_RTC_TIMESTAMP_EVENT_CB_ID   = 0x02U,
    HAL_RTC_WAKEUPTIMER_EVENT_CB_ID = 0x03U,
    HAL_RTC_TAMPER1_EVENT_CB_ID     = 0x04U,
#    if defined(RTC_TAMPER2_SUPPORT)
    HAL_RTC_TAMPER2_EVENT_CB_ID = 0x05U,
#    endif
    HAL_RTC_MSPINIT_CB_ID   = 0x0EU,
    HAL_RTC_MSPDEINIT_CB_ID = 0x0FU,
} HAL_RTC_CallbackIDTypeDef;

typedef void (*pRTC_CallbackTypeDef)(RTC_HandleTypeDef *hrtc);
#endif

typedef struct {
    uint32_t Tamper;
    uint32_t PinSelection;
    uint32_t Trigger;
    uint32_t Filter;
    uint32_t SamplingFrequency;
    uint32_t PrechargeDuration;
    uint32_t TamperPullUp;
    uint32_t TimeStampOnTamperDetection;
} RTC_TamperTypeDef;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_RTC_Init(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTC_DeInit(RTC_HandleTypeDef *hrtc);
void              HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void              HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);

#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_RTC_RegisterCallback(RTC_HandleTypeDef        *hrtc,
                                           HAL_RTC_CallbackIDTypeDef CallbackID,
                                           pRTC_CallbackTypeDef      pCallback);
HAL_StatusTypeDef HAL_RTC_UnRegisterCallback(
    RTC_HandleTypeDef *hrtc, HAL_RTC_CallbackIDTypeDef CallbackID);
#endif

HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc,
                                  RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc,
                                  RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc,
                                  RTC_DateTypeDef *sDate, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc,
                                  RTC_DateTypeDef *sDate, uint32_t Format);

HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc,
                                   RTC_AlarmTypeDef *sAlarm, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_SetAlarm_IT(RTC_HandleTypeDef *hrtc,
                                      RTC_AlarmTypeDef  *sAlarm,
                                      uint32_t           Format);
HAL_StatusTypeDef HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc,
                                          uint32_t           Alarm);
HAL_StatusTypeDef HAL_RTC_GetAlarm(RTC_HandleTypeDef *hrtc,
                                   RTC_AlarmTypeDef *sAlarm, uint32_t Alarm,
                                   uint32_t Format);
void              HAL_RTC_AlarmIRQHandler(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTC_PollForAlarmAEvent(RTC_HandleTypeDef *hrtc,
                                             uint32_t           Timeout);
void              HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);

HAL_StatusTypeDef HAL_RTC_WaitForSynchro(RTC_HandleTypeDef *hrtc);

void     HAL_RTC_DST_Add1Hour(RTC_HandleTypeDef *hrtc);
void     HAL_RTC_DST_Sub1Hour(RTC_HandleTypeDef *hrtc);
void     HAL_RTC_DST_SetStoreOperation(RTC_HandleTypeDef *hrtc);
void     HAL_RTC_DST_ClearStoreOperation(RTC_HandleTypeDef *hrtc);
uint32_t HAL_RTC_DST_ReadStoreOperation(RTC_HandleTypeDef *hrtc);

HAL_RTCStateTypeDef HAL_RTC_GetState(RTC_HandleTypeDef *hrtc);

HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef *hrtc);
uint8_t           RTC_ByteToBcd2(uint8_t number);
uint8_t           RTC_Bcd2ToByte(uint8_t number);

HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp(RTC_HandleTypeDef *hrtc,
                                         uint32_t           RTC_TimeStampEdge,
                                         uint32_t           RTC_TimeStampPin);
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp_IT(RTC_HandleTypeDef *hrtc,
                                            uint32_t RTC_TimeStampEdge,
                                            uint32_t RTC_TimeStampPin);
HAL_StatusTypeDef HAL_RTCEx_DeactivateTimeStamp(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_GetTimeStamp(RTC_HandleTypeDef *hrtc,
                                         RTC_TimeTypeDef   *sTimeStamp,
                                         RTC_DateTypeDef   *sTimeStampDate,
                                         uint32_t           Format);

HAL_StatusTypeDef HAL_RTCEx_SetTamper(RTC_HandleTypeDef *hrtc,
                                      RTC_TamperTypeDef *sTamper);
HAL_StatusTypeDef HAL_RTCEx_SetTamper_IT(RTC_HandleTypeDef *hrtc,
                                         RTC_TamperTypeDef *sTamper);
HAL_StatusTypeDef HAL_RTCEx_DeactivateTamper(RTC_HandleTypeDef *hrtc,
                                             uint32_t           Tamper);
void              HAL_RTCEx_TamperTimeStampIRQHandler(RTC_HandleTypeDef *hrtc);

void HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef *hrtc);
#if defined(RTC_TAMPER2_SUPPORT)
void HAL_RTCEx_Tamper2EventCallback(RTC_HandleTypeDef *hrtc);
#endif /* RTC_TAMPER2_SUPPORT */
void              HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForTimeStampEvent(RTC_HandleTypeDef *hrtc,
                                                  uint32_t           Timeout);
HAL_StatusTypeDef HAL_RTCEx_PollForTamper1Event(RTC_HandleTypeDef *hrtc,
                                                uint32_t           Timeout);
#if defined(RTC_TAMPER2_SUPPORT)
HAL_StatusTypeDef HAL_RTCEx_PollForTamper2Event(RTC_HandleTypeDef *hrtc,
                                                uint32_t           Timeout);
#endif /* RTC_TAMPER2_SUPPORT */

HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer(RTC_HandleTypeDef *hrtc,
                                           uint32_t           WakeUpCounter,
                                           uint32_t           WakeUpClock);
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer_IT(RTC_HandleTypeDef *hrtc,
                                              uint32_t           WakeUpCounter,
                                              uint32_t           WakeUpClock);
HAL_StatusTypeDef HAL_RTCEx_DeactivateWakeUpTimer(RTC_HandleTypeDef *hrtc);
uint32_t          HAL_RTCEx_GetWakeUpTimer(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_WakeUpTimerIRQHandler(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForWakeUpTimerEvent(RTC_HandleTypeDef *hrtc,
                                                    uint32_t           Timeout);
void     HAL_RTCEx_BKUPWrite(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister,
                             uint32_t Data);
uint32_t HAL_RTCEx_BKUPRead(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister);

HAL_StatusTypeDef HAL_RTCEx_SetCoarseCalib(RTC_HandleTypeDef *hrtc,
                                           uint32_t CalibSign, uint32_t Value);
HAL_StatusTypeDef HAL_RTCEx_DeactivateCoarseCalib(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_SetSmoothCalib(
    RTC_HandleTypeDef *hrtc, uint32_t SmoothCalibPeriod,
    uint32_t SmoothCalibPlusPulses, uint32_t SmoothCalibMinusPulsesValue);
HAL_StatusTypeDef HAL_RTCEx_SetSynchroShift(RTC_HandleTypeDef *hrtc,
                                            uint32_t           ShiftAdd1S,
                                            uint32_t           ShiftSubFS);
HAL_StatusTypeDef HAL_RTCEx_SetCalibrationOutPut(RTC_HandleTypeDef *hrtc,
                                                 uint32_t CalibOutput);
HAL_StatusTypeDef HAL_RTCEx_DeactivateCalibrationOutPut(
    RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_SetRefClock(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DeactivateRefClock(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_EnableBypassShadow(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DisableBypassShadow(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForAlarmBEvent(RTC_HandleTypeDef *hrtc,
                                               uint32_t           Timeout);

#endif /* defined(HAL_RTC_MODULE_ENABLED) && VSF_HAL_USE_RTC == ENABLED */

#ifdef __cplusplus
}
#endif

#endif
