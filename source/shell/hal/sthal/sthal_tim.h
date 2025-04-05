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

#ifndef __ST_HAL_TIM_H__
#define __ST_HAL_TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include "sthal_def.h"

/*============================ MACROS ========================================*/

// The timer interface of vsf-hal does not currently support the channel feature
#define TIM_CHANNEL_1   0x00000000U
#define TIM_CHANNEL_2   0x00000004U
#define TIM_CHANNEL_3   0x00000008U
#define TIM_CHANNEL_4   0x0000000CU
#define TIM_CHANNEL_ALL 0x0000003CU

#ifdef VSF_TIMER_BASE_COUNTER_UP
#   define TIM_COUNTERMODE_UP              VSF_TIMER_BASE_COUNTER_UP
#else
#   define TIM_COUNTERMODE_UP              0x00000000U
#endif
#ifdef VSF_TIMER_BASE_COUNTER_DOWN
#   define TIM_COUNTERMODE_DOWN            VSF_TIMER_BASE_COUNTER_DOWN
#else
#   define TIM_COUNTERMODE_DOWN            0x00000000U
#endif
#ifdef VSF_TIMER_BASE_COUNTER_CENTERALIGNED1
#   define TIM_COUNTERMODE_CENTERALIGNED1  VSF_TIMER_BASE_COUNTER_CENTERALIGNED1
#else
#   define TIM_COUNTERMODE_CENTERALIGNED1  0x00000000U
#endif
#ifdef VSF_TIMER_BASE_COUNTER_CENTERALIGNED2
#   define TIM_COUNTERMODE_CENTERALIGNED2  VSF_TIMER_BASE_COUNTER_CENTERALIGNED2
#else
#   define TIM_COUNTERMODE_CENTERALIGNED2  0x00000000U
#endif
#ifdef VSF_TIMER_BASE_COUNTER_CENTERALIGNED3
#   define TIM_COUNTERMODE_CENTERALIGNED3  VSF_TIMER_BASE_COUNTER_CENTERALIGNED3
#else
#   define TIM_COUNTERMODE_CENTERALIGNED3  0x00000000U
#endif


#define TIM_CLOCKDIVISION_DIV1              1
#define TIM_CLOCKDIVISION_DIV2              2
#define TIM_CLOCKDIVISION_DIV4              4

#define TIM_AUTORELOAD_PRELOAD_DISABLE     VSF_TIMER_BASE_ONESHOT
#define TIM_AUTORELOAD_PRELOAD_ENABLE      VSF_TIMER_BASE_CONTINUES


// TODO: implement the following macros
#define IS_TIM_INSTANCE(INSTANCE)               1
#define IS_TIM_CCX_INSTANCE(INSTANCE, CHANNEL)  1
#define IS_TIM_DMABURST_INSTANCE(INSTANCE)      1
#define IS_TIM_DMABURST_INSTANCE(INSTANCE)      1
#define IS_TIM_OCXREF_CLEAR_INSTANCE(INSTANCE)  1
#define IS_TIM_SLAVE_INSTANCE(INSTANCE)         1
#define IS_TIM_SYNCHRO_INSTANCE(INSTANCE)       1
#define IS_TIM_BREAK_INSTANCE(INSTANCE)         1
#if 0
#define IS_TIM_CCXN_INSTANCE(INSTANCE, CHANNEL) 1
#define IS_TIM_CLOCKSOURCE_INSTANCE(INSTANCE)   1
#define IS_TIM_CLOCKPOLARITY_INSTANCE(INSTANCE) 1
#define IS_TIM_CLOCKPRESCALER_INSTANCE(INSTANCE)1
#define IS_TIM_CLOCKFILTER_INSTANCE(INSTANCE)   1
#define IS_TIM_CLEARINPUT_SOURCE_INSTANCE(INSTANCE) 1
#define IS_TIM_CLEARINPUT_POLARITY_INSTANCE(INSTANCE) 1
#define IS_TIM_CLEARINPUT_PRESCALER_INSTANCE(INSTANCE) 1
#define IS_TIM_CLEARINPUT_FILTER_INSTANCE(INSTANCE) 1
#define IS_TIM_OSSR_INSTANCE(INSTANCE)          1
#define IS_TIM_OSSI_INSTANCE(INSTANCE)          1
#define IS_TIM_LOCK_LEVEL_INSTANCE(INSTANCE)    1
#define IS_TIM_BREAK_INSTANCE(INSTANCE)         1
#define IS_TIM_BREAK_POLARITY_INSTANCE(INSTANCE)1
#define IS_TIM_AUTOMATIC_OUTPUT_INSTANCE(INSTANCE) 1
#define IS_TIM_TRGO_SOURCE_INSTANCE(INSTANCE)   1
#define IS_TIM_SLAVE_INSTANCE(INSTANCE)         1
#define IS_TIM_SLAVE_MODE_INSTANCE(INSTANCE)    1
#define IS_TIM_TRIGGER_SELECTION_INSTANCE(INSTANCE) 1
#define IS_TIM_DMA_INSTANCE(INSTANCE)           1
#define IS_TIM_DMA_CC_INSTANCE(INSTANCE)        1
#define IS_TIM_DMA_CCX_INSTANCE(INSTANCE, CHANNEL) 1
#define IS_TIM_DMA_COM_INSTANCE(INSTANCE)       1
#define IS_TIM_DMA_TRIGGER_INSTANCE(INSTANCE)   1
#define IS_TIM_DMA_REMAP_INSTANCE(INSTANCE)     1
#define IS_TIM_DMA_CC_REMAP_INSTANCE(INSTANCE)  1
#define IS_TIM_DMA_CCX_REMAP_INSTANCE(INSTANCE, CHANNEL) 1
#define IS_TIM_DMA_COM_REMAP_INSTANCE(INSTANCE) 1
#define IS_TIM_REMAP_INSTANCE(INSTANCE)         1
#define IS_TIM_REMAP_CC_INSTANCE(INSTANCE)      1
#define IS_TIM_REMAP_CCX_INSTANCE(INSTANCE, CHANNEL) 1
#define IS_TIM_REMAP_COM_INSTANCE(INSTANCE)     1
#define IS_TIM_REMAP_DMA_INSTANCE(INSTANCE)     1
#define IS_TIM_REMAP_DMA_CC_INSTANCE(INSTANCE)  1
#define IS_TIM_REMAP_DMA_CCX_INSTANCE(INSTANCE, CHANNEL) 1
#define IS_TIM_REMAP_DMA_COM_INSTANCE(INSTANCE) 1
#endif

#define IS_TIM_COUNTER_MODE(__MODE__)      (((__MODE__) == TIM_COUNTERMODE_UP)              || \
                                            ((__MODE__) == TIM_COUNTERMODE_DOWN)            || \
                                            ((__MODE__) == TIM_COUNTERMODE_CENTERALIGNED1)  || \
                                            ((__MODE__) == TIM_COUNTERMODE_CENTERALIGNED2)  || \
                                            ((__MODE__) == TIM_COUNTERMODE_CENTERALIGNED3))
#define IS_TIM_CLOCKDIVISION_DIV(__DIV__)  (((__DIV__) == TIM_CLOCKDIVISION_DIV1) || \
                                            ((__DIV__) == TIM_CLOCKDIVISION_DIV2) || \
                                            ((__DIV__) == TIM_CLOCKDIVISION_DIV4))
#define IS_TIM_AUTORELOAD_PRELOAD(PRELOAD) (((PRELOAD) == TIM_AUTORELOAD_PRELOAD_DISABLE) || \
                                            ((PRELOAD) == TIM_AUTORELOAD_PRELOAD_ENABLE))

#define IS_TIM_DMA_BASE(BASE)               1
#define IS_TIM_DMA_LENGTH(LENGTH)           1
#define IS_TIM_DMA_SOURCE(SOURCE)           1
#define IS_TIM_EVENT_SOURCE(SOURCE)         1
#define IS_TIM_CLEARINPUT_SOURCE(SOURCE)    1
#define IS_TIM_CLOCKSOURCE(SOURCE)          1
#define IS_TIM_SLAVE_MODE(MODE)             1
#define IS_TIM_TRIGGER_SELECTION(SELECTION) 1
#define IS_TIM_COMMUTATION_EVENT_INSTANCE(INSTANCE) 1
#define IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(SELECTION) 1
#define IS_TIM_TRGO_SOURCE(SOURCE)          1
#define IS_TIM_MSM_STATE(STATE)             1
#define IS_TIM_OSSR_STATE(STATE)            1
#define IS_TIM_OSSI_STATE(STATE)            1
#define IS_TIM_DEADTIME(DTIME)              1
#define IS_TIM_LOCK_LEVEL(LEVEL)            1
#define IS_TIM_BREAK_STATE(STATE)           1
#define IS_TIM_BREAK_POLARITY(POLARITY)     1
#define IS_TIM_AUTOMATIC_OUTPUT_STATE(STATE) 1
#define IS_TIM_REMAP(REMAP, INSTANCE)       1

#define TIM_TS_ITR0                          0x00000000U
#define TIM_TS_ITR1                          0x00000010U
#define TIM_TS_ITR2                          0x00000020U
#define TIM_TS_ITR3                          0x00000030U

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef vsf_timer_t TIM_TypeDef;

typedef struct {
    uint32_t Prescaler;
    uint32_t CounterMode;
    uint32_t Period;
    uint32_t ClockDivision;
    uint32_t RepetitionCounter;
    uint32_t AutoReloadPreload;
} TIM_Base_InitTypeDef;

typedef struct {
    uint32_t OCMode;
    uint32_t Pulse;
    uint32_t OCPolarity;
    uint32_t OCNPolarity;
    uint32_t OCFastMode;
    uint32_t OCIdleState;
    uint32_t OCNIdleState;
} TIM_OC_InitTypeDef;

typedef struct {
    uint32_t OCMode;
    uint32_t Pulse;
    uint32_t OCPolarity;
    uint32_t OCNPolarity;
    uint32_t OCIdleState;
    uint32_t OCNIdleState;
    uint32_t ICPolarity;
    uint32_t ICSelection;
    uint32_t ICFilter;
} TIM_OnePulse_InitTypeDef;

typedef struct {
    uint32_t ICPolarity;
    uint32_t ICSelection;
    uint32_t ICPrescaler;
    uint32_t ICFilter;
} TIM_IC_InitTypeDef;

typedef struct {
    uint32_t EncoderMode;
    uint32_t IC1Polarity;
    uint32_t IC1Selection;
    uint32_t IC1Prescaler;
    uint32_t IC1Filter;
    uint32_t IC2Polarity;
    uint32_t IC2Selection;
    uint32_t IC2Prescaler;
    uint32_t IC2Filter;
} TIM_Encoder_InitTypeDef;

typedef struct {
    uint32_t ClockSource;
    uint32_t ClockPolarity;
    uint32_t ClockPrescaler;
    uint32_t ClockFilter;
} TIM_ClockConfigTypeDef;

typedef struct {
    uint32_t ClearInputState;
    uint32_t ClearInputSource;
    uint32_t ClearInputPolarity;
    uint32_t ClearInputPrescaler;
    uint32_t ClearInputFilter;
} TIM_ClearInputConfigTypeDef;

typedef struct {
    uint32_t MasterOutputTrigger;
    uint32_t MasterSlaveMode;
} TIM_MasterConfigTypeDef;

typedef struct {
    uint32_t SlaveMode;
    uint32_t InputTrigger;
    uint32_t TriggerPolarity;
    uint32_t TriggerPrescaler;
    uint32_t TriggerFilter;

} TIM_SlaveConfigTypeDef;

typedef struct {
    uint32_t OffStateRunMode;
    uint32_t OffStateIDLEMode;
    uint32_t LockLevel;
    uint32_t DeadTime;
    uint32_t BreakState;
    uint32_t BreakPolarity;
    uint32_t BreakFilter;
    uint32_t AutomaticOutput;
} TIM_BreakDeadTimeConfigTypeDef;

typedef enum {
    HAL_TIM_STATE_RESET   = 0x00U,
    HAL_TIM_STATE_READY   = 0x01U,
    HAL_TIM_STATE_BUSY    = 0x02U,
    HAL_TIM_STATE_TIMEOUT = 0x03U,
    HAL_TIM_STATE_ERROR   = 0x04U,
} HAL_TIM_StateTypeDef;

typedef enum {
    HAL_TIM_CHANNEL_STATE_RESET = 0x00U,
    HAL_TIM_CHANNEL_STATE_READY = 0x01U,
    HAL_TIM_CHANNEL_STATE_BUSY  = 0x02U,
} HAL_TIM_ChannelStateTypeDef;

typedef enum {
    HAL_DMA_BURST_STATE_RESET = 0x00U,
    HAL_DMA_BURST_STATE_READY = 0x01U,
    HAL_DMA_BURST_STATE_BUSY  = 0x02U,
} HAL_TIM_DMABurstStateTypeDef;

typedef enum {
    HAL_TIM_ACTIVE_CHANNEL_1       = 0x01U,
    HAL_TIM_ACTIVE_CHANNEL_2       = 0x02U,
    HAL_TIM_ACTIVE_CHANNEL_3       = 0x04U,
    HAL_TIM_ACTIVE_CHANNEL_4       = 0x08U,
    HAL_TIM_ACTIVE_CHANNEL_CLEARED = 0x00U,
} HAL_TIM_ActiveChannel;

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
typedef struct __TIM_HandleTypeDef
#else
typedef struct
#endif
{
    TIM_TypeDef                      *Instance;
    TIM_Base_InitTypeDef              Init;
    HAL_TIM_ActiveChannel             Channel;
    DMA_HandleTypeDef                *hdma[7];
    HAL_LockTypeDef                   Lock;
    volatile HAL_TIM_StateTypeDef         State;
    volatile HAL_TIM_ChannelStateTypeDef  ChannelState[4];
    volatile HAL_TIM_ChannelStateTypeDef  ChannelNState[4];
    volatile HAL_TIM_DMABurstStateTypeDef DMABurstState;

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
    void (*Base_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*Base_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*IC_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*IC_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*OC_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*OC_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*PWM_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*PWM_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*OnePulse_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*OnePulse_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*Encoder_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*Encoder_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*HallSensor_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*HallSensor_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
    void (*PeriodElapsedCallback)(struct __TIM_HandleTypeDef *htim);
    void (*PeriodElapsedHalfCpltCallback)(struct __TIM_HandleTypeDef *htim);
    void (*TriggerCallback)(struct __TIM_HandleTypeDef *htim);
    void (*TriggerHalfCpltCallback)(struct __TIM_HandleTypeDef *htim);
    void (*IC_CaptureCallback)(struct __TIM_HandleTypeDef *htim);
    void (*IC_CaptureHalfCpltCallback)(struct __TIM_HandleTypeDef *htim);
    void (*OC_DelayElapsedCallback)(struct __TIM_HandleTypeDef *htim);
    void (*PWM_PulseFinishedCallback)(struct __TIM_HandleTypeDef *htim);
    void (*PWM_PulseFinishedHalfCpltCallback)(struct __TIM_HandleTypeDef *htim);
    void (*ErrorCallback)(struct __TIM_HandleTypeDef *htim);
    void (*CommutationCallback)(struct __TIM_HandleTypeDef *htim);
    void (*CommutationHalfCpltCallback)(struct __TIM_HandleTypeDef *htim);
    void (*BreakCallback)(struct __TIM_HandleTypeDef *htim);
#endif
} TIM_HandleTypeDef;

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)

typedef enum {
    HAL_TIM_BASE_MSPINIT_CB_ID            = 0x00U,
    HAL_TIM_BASE_MSPDEINIT_CB_ID          = 0x01U,
    HAL_TIM_IC_MSPINIT_CB_ID              = 0x02U,
    HAL_TIM_IC_MSPDEINIT_CB_ID            = 0x03U,
    HAL_TIM_OC_MSPINIT_CB_ID              = 0x04U,
    HAL_TIM_OC_MSPDEINIT_CB_ID            = 0x05U,
    HAL_TIM_PWM_MSPINIT_CB_ID             = 0x06U,
    HAL_TIM_PWM_MSPDEINIT_CB_ID           = 0x07U,
    HAL_TIM_ONE_PULSE_MSPINIT_CB_ID       = 0x08U,
    HAL_TIM_ONE_PULSE_MSPDEINIT_CB_ID     = 0x09U,
    HAL_TIM_ENCODER_MSPINIT_CB_ID         = 0x0AU,
    HAL_TIM_ENCODER_MSPDEINIT_CB_ID       = 0x0BU,
    HAL_TIM_HALL_SENSOR_MSPINIT_CB_ID     = 0x0CU,
    HAL_TIM_HALL_SENSOR_MSPDEINIT_CB_ID   = 0x0DU,
    HAL_TIM_PERIOD_ELAPSED_CB_ID          = 0x0EU,
    HAL_TIM_PERIOD_ELAPSED_HALF_CB_ID     = 0x0FU,
    HAL_TIM_TRIGGER_CB_ID                 = 0x10U,
    HAL_TIM_TRIGGER_HALF_CB_ID            = 0x11U,
    HAL_TIM_IC_CAPTURE_CB_ID              = 0x12U,
    HAL_TIM_IC_CAPTURE_HALF_CB_ID         = 0x13U,
    HAL_TIM_OC_DELAY_ELAPSED_CB_ID        = 0x14U,
    HAL_TIM_PWM_PULSE_FINISHED_CB_ID      = 0x15U,
    HAL_TIM_PWM_PULSE_FINISHED_HALF_CB_ID = 0x16U,
    HAL_TIM_ERROR_CB_ID                   = 0x17U,
    HAL_TIM_COMMUTATION_CB_ID             = 0x18U,
    HAL_TIM_COMMUTATION_HALF_CB_ID        = 0x19U,
    HAL_TIM_BREAK_CB_ID                   = 0x1AU,
} HAL_TIM_CallbackIDTypeDef;
typedef void (*pTIM_CallbackTypeDef)(TIM_HandleTypeDef *htim);
#endif

typedef struct {
    uint32_t IC1Polarity;
    uint32_t IC1Prescaler;
    uint32_t IC1Filter;
    uint32_t Commutation_Delay;

} TIM_HallSensor_InitTypeDef;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/* Time Base functions ********************************************************/
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Start_DMA(TIM_HandleTypeDef *htim,
                                         const uint32_t    *pData,
                                         uint16_t           Length);
HAL_StatusTypeDef HAL_TIM_Base_Stop_DMA(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_OC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_OC_DeInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_OC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_OC_Start_IT(TIM_HandleTypeDef *htim,
                                      uint32_t           Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_OC_Start_DMA(TIM_HandleTypeDef *htim,
                                       uint32_t Channel, const uint32_t *pData,
                                       uint16_t Length);
HAL_StatusTypeDef HAL_TIM_OC_Stop_DMA(TIM_HandleTypeDef *htim,
                                      uint32_t           Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_PWM_DeInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_PWM_Start_IT(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_IT(TIM_HandleTypeDef *htim,
                                      uint32_t           Channel);

HAL_StatusTypeDef HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim,
                                        uint32_t Channel, const uint32_t *pData,
                                        uint16_t Length);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_DMA(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel);
HAL_StatusTypeDef HAL_TIM_IC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_IC_DeInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_IC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_IC_Start_IT(TIM_HandleTypeDef *htim,
                                      uint32_t           Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim,
                                       uint32_t Channel, uint32_t *pData,
                                       uint16_t Length);
HAL_StatusTypeDef HAL_TIM_IC_Stop_DMA(TIM_HandleTypeDef *htim,
                                      uint32_t           Channel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Init(TIM_HandleTypeDef *htim,
                                        uint32_t           OnePulseMode);
HAL_StatusTypeDef HAL_TIM_OnePulse_DeInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_OnePulse_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_OnePulse_Start(TIM_HandleTypeDef *htim,
                                         uint32_t           OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop(TIM_HandleTypeDef *htim,
                                        uint32_t           OutputChannel);

HAL_StatusTypeDef HAL_TIM_OnePulse_Start_IT(TIM_HandleTypeDef *htim,
                                            uint32_t           OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop_IT(TIM_HandleTypeDef *htim,
                                           uint32_t           OutputChannel);
HAL_StatusTypeDef HAL_TIM_Encoder_Init(TIM_HandleTypeDef             *htim,
                                       const TIM_Encoder_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIM_Encoder_DeInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim);
void              HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef *htim,
                                        uint32_t           Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel);

HAL_StatusTypeDef HAL_TIM_Encoder_Start_IT(TIM_HandleTypeDef *htim,
                                           uint32_t           Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_IT(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel);

HAL_StatusTypeDef HAL_TIM_Encoder_Start_DMA(TIM_HandleTypeDef *htim,
                                            uint32_t Channel, uint32_t *pData1,
                                            uint32_t *pData2, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_DMA(TIM_HandleTypeDef *htim,
                                           uint32_t           Channel);

void              HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_OC_ConfigChannel(TIM_HandleTypeDef        *htim,
                                           const TIM_OC_InitTypeDef *sConfig,
                                           uint32_t                  Channel);
HAL_StatusTypeDef HAL_TIM_PWM_ConfigChannel(TIM_HandleTypeDef        *htim,
                                            const TIM_OC_InitTypeDef *sConfig,
                                            uint32_t                  Channel);
HAL_StatusTypeDef HAL_TIM_IC_ConfigChannel(TIM_HandleTypeDef        *htim,
                                           const TIM_IC_InitTypeDef *sConfig,
                                           uint32_t                  Channel);
HAL_StatusTypeDef HAL_TIM_OnePulse_ConfigChannel(
    TIM_HandleTypeDef *htim, TIM_OnePulse_InitTypeDef *sConfig,
    uint32_t OutputChannel, uint32_t InputChannel);
HAL_StatusTypeDef HAL_TIM_ConfigOCrefClear(
    TIM_HandleTypeDef                 *htim,
    const TIM_ClearInputConfigTypeDef *sClearInputConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_ConfigClockSource(
    TIM_HandleTypeDef *htim, const TIM_ClockConfigTypeDef *sClockSourceConfig);
HAL_StatusTypeDef HAL_TIM_ConfigTI1Input(TIM_HandleTypeDef *htim,
                                         uint32_t           TI1_Selection);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro(
    TIM_HandleTypeDef *htim, const TIM_SlaveConfigTypeDef *sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro_IT(
    TIM_HandleTypeDef *htim, const TIM_SlaveConfigTypeDef *sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStart(TIM_HandleTypeDef *htim,
                                              uint32_t        BurstBaseAddress,
                                              uint32_t        BurstRequestSrc,
                                              const uint32_t *BurstBuffer,
                                              uint32_t        BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_MultiWriteStart(TIM_HandleTypeDef *htim,
                                                   uint32_t BurstBaseAddress,
                                                   uint32_t BurstRequestSrc,
                                                   const uint32_t *BurstBuffer,
                                                   uint32_t        BurstLength,
                                                   uint32_t        DataLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStop(TIM_HandleTypeDef *htim,
                                             uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStart(TIM_HandleTypeDef *htim,
                                             uint32_t  BurstBaseAddress,
                                             uint32_t  BurstRequestSrc,
                                             uint32_t *BurstBuffer,
                                             uint32_t  BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_MultiReadStart(TIM_HandleTypeDef *htim,
                                                  uint32_t  BurstBaseAddress,
                                                  uint32_t  BurstRequestSrc,
                                                  uint32_t *BurstBuffer,
                                                  uint32_t  BurstLength,
                                                  uint32_t  DataLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStop(TIM_HandleTypeDef *htim,
                                            uint32_t           BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_GenerateEvent(TIM_HandleTypeDef *htim,
                                        uint32_t           EventSource);
uint32_t          HAL_TIM_ReadCapturedValue(const TIM_HandleTypeDef *htim,
                                            uint32_t                 Channel);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim);

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_TIM_RegisterCallback(TIM_HandleTypeDef        *htim,
                                           HAL_TIM_CallbackIDTypeDef CallbackID,
                                           pTIM_CallbackTypeDef      pCallback);
HAL_StatusTypeDef HAL_TIM_UnRegisterCallback(
    TIM_HandleTypeDef *htim, HAL_TIM_CallbackIDTypeDef CallbackID);
#endif

HAL_TIM_StateTypeDef  HAL_TIM_Base_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef  HAL_TIM_OC_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef  HAL_TIM_PWM_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef  HAL_TIM_IC_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef  HAL_TIM_OnePulse_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef  HAL_TIM_Encoder_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_ActiveChannel HAL_TIM_GetActiveChannel(const TIM_HandleTypeDef *htim);
HAL_TIM_ChannelStateTypeDef HAL_TIM_GetChannelState(
    const TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_TIM_DMABurstStateTypeDef HAL_TIM_DMABurstState(
    const TIM_HandleTypeDef *htim);
void TIM_Base_SetConfig(TIM_TypeDef                *TIMx,
                        const TIM_Base_InitTypeDef *Structure);
void TIM_TI1_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity,
                       uint32_t TIM_ICSelection, uint32_t TIM_ICFilter);
void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, const TIM_OC_InitTypeDef *OC_Config);
void TIM_ETR_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ExtTRGPrescaler,
                       uint32_t TIM_ExtTRGPolarity, uint32_t ExtTRGFilter);
void TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma);
void TIM_DMAError(DMA_HandleTypeDef *hdma);
void TIM_DMACaptureCplt(DMA_HandleTypeDef *hdma);
void TIM_DMACaptureHalfCplt(DMA_HandleTypeDef *hdma);
void TIM_CCxChannelCmd(TIM_TypeDef *TIMx, uint32_t Channel,
                       uint32_t ChannelState);

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
void TIM_ResetCallback(TIM_HandleTypeDef *htim);
#endif

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(
    TIM_HandleTypeDef *htim, const TIM_HallSensor_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim);
void              HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim);
void              HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef *htim,
                                                 uint32_t          *pData,
                                                 uint16_t           Length);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef *htim,
                                      uint32_t           Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef *htim,
                                         uint32_t           Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef *htim,
                                        uint32_t           Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel,
                                          const uint32_t    *pData,
                                          uint16_t           Length);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef *htim,
                                         uint32_t           Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef *htim,
                                       uint32_t           Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef *htim,
                                      uint32_t           Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT(TIM_HandleTypeDef *htim,
                                         uint32_t           Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef *htim,
                                           uint32_t           Channel,
                                           const uint32_t    *pData,
                                           uint16_t           Length);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef *htim,
                                          uint32_t           Channel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef *htim,
                                            uint32_t           OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef *htim,
                                           uint32_t           OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef *htim,
                                               uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef *htim,
                                              uint32_t           OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent(TIM_HandleTypeDef *htim,
                                              uint32_t           InputTrigger,
                                              uint32_t CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_IT(TIM_HandleTypeDef *htim,
                                                 uint32_t InputTrigger,
                                                 uint32_t CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_DMA(TIM_HandleTypeDef *htim,
                                                  uint32_t InputTrigger,
                                                  uint32_t CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(
    TIM_HandleTypeDef *htim, const TIM_MasterConfigTypeDef *sMasterConfig);
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(
    TIM_HandleTypeDef                    *htim,
    const TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig);
HAL_StatusTypeDef    HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim,
                                           uint32_t           Remap);
void                 HAL_TIMEx_CommutCallback(TIM_HandleTypeDef *htim);
void                 HAL_TIMEx_CommutHalfCpltCallback(TIM_HandleTypeDef *htim);
void                 HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(
    const TIM_HandleTypeDef *htim);
HAL_TIM_ChannelStateTypeDef HAL_TIMEx_GetChannelNState(
    const TIM_HandleTypeDef *htim, uint32_t ChannelN);
void TIMEx_DMACommutationCplt(DMA_HandleTypeDef *hdma);
void TIMEx_DMACommutationHalfCplt(DMA_HandleTypeDef *hdma);

#ifdef __cplusplus
}
#endif

#endif
