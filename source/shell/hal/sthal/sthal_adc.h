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

#ifndef __ST_ADC_H__
#define __ST_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HAL_ADC_MODULE_ENABLED) && VSF_HAL_USE_ADC == ENABLED

/*============================ INCLUDES ======================================*/

#include "sthal_def.h"

/*============================ MACROS ========================================*/

#define HAL_ADC_STATE_RESET           0x00000000U
#define HAL_ADC_STATE_READY           0x00000001U
#define HAL_ADC_STATE_BUSY_INTERNAL   0x00000002U
#define HAL_ADC_STATE_TIMEOUT         0x00000004U
#define HAL_ADC_STATE_ERROR_INTERNAL  0x00000010U
#define HAL_ADC_STATE_ERROR_CONFIG    0x00000020U
#define HAL_ADC_STATE_ERROR_DMA       0x00000040U
#define HAL_ADC_STATE_REG_BUSY        0x00000100U
#define HAL_ADC_STATE_REG_EOC         0x00000200U
#define HAL_ADC_STATE_REG_OVR         0x00000400U
#define HAL_ADC_STATE_INJ_BUSY        0x00001000U
#define HAL_ADC_STATE_INJ_EOC         0x00002000U
#define HAL_ADC_STATE_AWD1            0x00010000U
#define HAL_ADC_STATE_AWD2            0x00020000U
#define HAL_ADC_STATE_AWD3            0x00040000U
#define HAL_ADC_STATE_MULTIMODE_SLAVE 0x00100000U

#define HAL_ADC_ERROR_NONE     0x00U
#define HAL_ADC_ERROR_INTERNAL 0x01U
#define HAL_ADC_ERROR_OVR      0x02U
#define HAL_ADC_ERROR_DMA      0x04U
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
#    define HAL_ADC_ERROR_INVALID_CALLBACK (0x10U)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef vsf_adc_t ADC_TypeDef;

typedef struct {
    uint32_t        ClockPrescaler;
    uint32_t        Resolution;
    uint32_t        DataAlign;
    uint32_t        ScanConvMode;
    uint32_t        EOCSelection;
    FunctionalState ContinuousConvMode;
    uint32_t        NbrOfConversion;
    FunctionalState DiscontinuousConvMode;
    uint32_t        NbrOfDiscConversion;
    uint32_t        ExternalTrigConv;
    uint32_t        ExternalTrigConvEdge;
    FunctionalState DMAContinuousRequests;
} ADC_InitTypeDef;

typedef struct {
    uint32_t Channel;
    uint32_t Rank;
    uint32_t SamplingTime;
    uint32_t Offset;
} ADC_ChannelConfTypeDef;

typedef struct {
    uint32_t        WatchdogMode;
    uint32_t        HighThreshold;
    uint32_t        LowThreshold;
    uint32_t        Channel;
    FunctionalState ITMode;
    uint32_t        WatchdogNumber;
} ADC_AnalogWDGConfTypeDef;

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
typedef struct __ADC_HandleTypeDef
#else
typedef struct
#endif
{
    ADC_TypeDef       *Instance;
    ADC_InitTypeDef    Init;
    volatile uint32_t      NbrOfCurrentConversionRank;
    DMA_HandleTypeDef *DMA_Handle;
    HAL_LockTypeDef    Lock;
    volatile uint32_t      State;
    volatile uint32_t      ErrorCode;
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
    void (*ConvCpltCallback)(struct __ADC_HandleTypeDef *hadc);
    void (*ConvHalfCpltCallback)(struct __ADC_HandleTypeDef *hadc);
    void (*LevelOutOfWindowCallback)(struct __ADC_HandleTypeDef *hadc);
    void (*ErrorCallback)(struct __ADC_HandleTypeDef *hadc);
    void (*InjectedConvCpltCallback)(struct __ADC_HandleTypeDef *hadc);
    void (*MspInitCallback)(struct __ADC_HandleTypeDef *hadc);
    void (*MspDeInitCallback)(struct __ADC_HandleTypeDef *hadc);
#endif
} ADC_HandleTypeDef;

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
typedef enum {
    HAL_ADC_CONVERSION_COMPLETE_CB_ID     = 0x00U,
    HAL_ADC_CONVERSION_HALF_CB_ID         = 0x01U,
    HAL_ADC_LEVEL_OUT_OF_WINDOW_1_CB_ID   = 0x02U,
    HAL_ADC_ERROR_CB_ID                   = 0x03U,
    HAL_ADC_INJ_CONVERSION_COMPLETE_CB_ID = 0x04U,
    HAL_ADC_MSPINIT_CB_ID                 = 0x05U,
    HAL_ADC_MSPDEINIT_CB_ID               = 0x06U,
} HAL_ADC_CallbackIDTypeDef;
typedef void (*pADC_CallbackTypeDef)(ADC_HandleTypeDef *hadc);
#endif

typedef struct {
    uint32_t        InjectedChannel;
    uint32_t        InjectedRank;
    uint32_t        InjectedSamplingTime;
    uint32_t        InjectedOffset;
    uint32_t        InjectedNbrOfConversion;
    FunctionalState InjectedDiscontinuousConvMode;
    FunctionalState AutoInjectedConv;
    uint32_t        ExternalTrigInjecConv;
    uint32_t        ExternalTrigInjecConvEdge;
} ADC_InjectionConfTypeDef;

typedef struct {
    uint32_t Mode;
    uint32_t DMAAccessMode;
    uint32_t TwoSamplingDelay;
} ADC_MultiModeTypeDef;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef *hadc);
void              HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);
void              HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc);

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_ADC_RegisterCallback(ADC_HandleTypeDef        *hadc,
                                           HAL_ADC_CallbackIDTypeDef CallbackID,
                                           pADC_CallbackTypeDef      pCallback);
HAL_StatusTypeDef HAL_ADC_UnRegisterCallback(
    ADC_HandleTypeDef *hadc, HAL_ADC_CallbackIDTypeDef CallbackID);
#endif

HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *hadc,
                                            uint32_t           Timeout);
HAL_StatusTypeDef HAL_ADC_PollForEvent(ADC_HandleTypeDef *hadc,
                                       uint32_t EventType, uint32_t Timeout);
HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef *hadc);
void              HAL_ADC_IRQHandler(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef *hadc, uint32_t *pData,
                                    uint32_t Length);
HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef *hadc);
uint32_t          HAL_ADC_GetValue(ADC_HandleTypeDef *hadc);
void              HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void              HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc);
void              HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc);
void              HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef      *hadc,
                                        ADC_ChannelConfTypeDef *sConfig);
HAL_StatusTypeDef HAL_ADC_AnalogWDGConfig(
    ADC_HandleTypeDef *hadc, ADC_AnalogWDGConfTypeDef *AnalogWDGConfig);
uint32_t          HAL_ADC_GetState(ADC_HandleTypeDef *hadc);
uint32_t          HAL_ADC_GetError(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedStart(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedStop(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedPollForConversion(ADC_HandleTypeDef *hadc,
                                                      uint32_t Timeout);
HAL_StatusTypeDef HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedStop_IT(ADC_HandleTypeDef *hadc);
uint32_t          HAL_ADCEx_InjectedGetValue(ADC_HandleTypeDef *hadc,
                                             uint32_t           InjectedRank);
HAL_StatusTypeDef HAL_ADCEx_MultiModeStart_DMA(ADC_HandleTypeDef *hadc,
                                               uint32_t          *pData,
                                               uint32_t           Length);
HAL_StatusTypeDef HAL_ADCEx_MultiModeStop_DMA(ADC_HandleTypeDef *hadc);
uint32_t          HAL_ADCEx_MultiModeGetValue(ADC_HandleTypeDef *hadc);
void              HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(
    ADC_HandleTypeDef *hadc, ADC_InjectionConfTypeDef *sConfigInjected);
HAL_StatusTypeDef HAL_ADCEx_MultiModeConfigChannel(
    ADC_HandleTypeDef *hadc, ADC_MultiModeTypeDef *multimode);

#endif /* defined(HAL_ADC_MODULE_ENABLED) && VSF_HAL_USE_ADC == ENABLED */

#ifdef __cplusplus
}
#endif

#endif /*__ST_ADC_H */
