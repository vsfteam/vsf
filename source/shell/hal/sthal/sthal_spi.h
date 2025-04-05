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

#ifndef __ST_HAL_SPI_H__
#define __ST_HAL_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include "sthal_def.h"

/*============================ MACROS ========================================*/

#define HAL_SPI_ERROR_NONE  (0x00000000U)
#define HAL_SPI_ERROR_MODF  (0x00000001U)
#define HAL_SPI_ERROR_CRC   (0x00000002U)
#define HAL_SPI_ERROR_OVR   (0x00000004U)
#define HAL_SPI_ERROR_FRE   (0x00000008U)
#define HAL_SPI_ERROR_DMA   (0x00000010U)
#define HAL_SPI_ERROR_FLAG  (0x00000020U)
#define HAL_SPI_ERROR_ABORT (0x00000040U)
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
#    define HAL_SPI_ERROR_INVALID_CALLBACK (0x00000080U)
#endif

#define SPI_MODE_SLAVE      VSF_SPI_SLAVE
#define SPI_MODE_MASTER     VSF_SPI_MASTER
#define SPI_DATASIZE_8BIT   VSF_SPI_DATASIZE_8
#define SPI_DATASIZE_16BIT  VSF_SPI_DATASIZE_16
#define SPI_POLARITY_LOW    VSF_SPI_CPOL_LOW
#define SPI_POLARITY_HIGH   VSF_SPI_CPOL_HIGH
#define SPI_PHASE_1EDGE     VSF_SPI_CPHA_LOW
#define SPI_PHASE_2EDGE     VSF_SPI_CPHA_HIGH
#define SPI_FIRSTBIT_MSB    VSF_SPI_MSB_FIRST
#define SPI_FIRSTBIT_LSB    VSF_SPI_LSB_FIRST
#define SPI_NSS_SOFT        VSF_SPI_CS_SOFTWARE_MODE
#define SPI_NSS_HARD_OUTPUT VSF_SPI_CS_HARDWARE_MODE
#ifdef VSF_SPI_CS_HARDWARE_INPUT_MODE
#    define SPI_NSS_HARD_INPUT VSF_SPI_CS_HARDWARE_INPUT_MODE
#else
#    define SPI_NSS_HARD_INPUT 0
#endif
#ifdef VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX
#    define SPI_DIRECTION_2LINES VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX
#else
#    define SPI_DIRECTION_2LINES 0
#endif

#ifdef VSF_SPI_DATALINE_2_LINE_RX_ONLY
#    define SPI_DIRECTION_2LINES_RXONLY VSF_SPI_DATALINE_2_LINE_RX_ONLY
#else
#    define SPI_DIRECTION_2LINES_RXONLY 0
#endif

#ifdef VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX
#    define SPI_DIRECTION_1LINE VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX
#else
#    define SPI_DIRECTION_1LINE 0
#endif
#    define SPI_BAUDRATEPRESCALER_2 2
#    define SPI_BAUDRATEPRESCALER_4 4
#    define SPI_BAUDRATEPRESCALER_8 8
#    define SPI_BAUDRATEPRESCALER_16 16
#    define SPI_BAUDRATEPRESCALER_32 32
#    define SPI_BAUDRATEPRESCALER_64 64
#    define SPI_BAUDRATEPRESCALER_128 128
#    define SPI_BAUDRATEPRESCALER_256 256
#ifdef VSF_SPI_TI_MODE_DISABLE
#    define SPI_TIMODE_DISABLE VSF_SPI_TI_MODE_DISABLE
#else
#    define SPI_TIMODE_DISABLE (0x00000000U)
#endif
#ifdef VSF_SPI_TI_MMOE_ENABLE
#    define SPI_TIMODE_ENABLE VSF_SPI_TI_MMOE_ENABLE
#else
#    define SPI_TIMODE_ENABLE (0x00000000U)
#endif
#ifdef VSF_SPI_CRCCALCULATION_DISABLE
#    define SPI_CRCCALCULATION_DISABLE VSF_SPI_CRCCALCULATION_DISABLE
#else
#    define SPI_CRCCALCULATION_DISABLE (0x00000000U)
#endif

// TODO: need vsf-hal support
#define IS_SPI_ALL_INSTANCE(INSTANCE) 1
#define IS_SPI_MODE(__MODE__)                                                  \
    (((__MODE__) == SPI_MODE_SLAVE) || ((__MODE__) == SPI_MODE_MASTER))

#define IS_SPI_DIRECTION(__MODE__)                                             \
    (((__MODE__) == SPI_DIRECTION_2LINES) ||                                   \
     ((__MODE__) == SPI_DIRECTION_2LINES_RXONLY) ||                            \
     ((__MODE__) == SPI_DIRECTION_1LINE))
#define IS_SPI_DIRECTION_2LINES(__MODE__) ((__MODE__) == SPI_DIRECTION_2LINES)
#define IS_SPI_DIRECTION_2LINES_OR_1LINE(__MODE__)                             \
    (((__MODE__) == SPI_DIRECTION_2LINES) ||                                   \
     ((__MODE__) == SPI_DIRECTION_1LINE))
#define IS_SPI_DATASIZE(__DATASIZE__)                                          \
    (((__DATASIZE__) == SPI_DATASIZE_16BIT) ||                                 \
     ((__DATASIZE__) == SPI_DATASIZE_8BIT))
#define IS_SPI_CPOL(__CPOL__)                                                  \
    (((__CPOL__) == SPI_POLARITY_LOW) || ((__CPOL__) == SPI_POLARITY_HIGH))
#define IS_SPI_CPHA(__CPHA__)                                                  \
    (((__CPHA__) == SPI_PHASE_1EDGE) || ((__CPHA__) == SPI_PHASE_2EDGE))
#define IS_SPI_NSS(__NSS__)                                                    \
    (((__NSS__) == SPI_NSS_SOFT) || ((__NSS__) == SPI_NSS_HARD_INPUT) ||       \
     ((__NSS__) == SPI_NSS_HARD_OUTPUT))
#define IS_SPI_BAUDRATE_PRESCALER(__PRESCALER__)                               \
    (((__PRESCALER__) == SPI_BAUDRATEPRESCALER_2) ||                           \
     ((__PRESCALER__) == SPI_BAUDRATEPRESCALER_4) ||                           \
     ((__PRESCALER__) == SPI_BAUDRATEPRESCALER_8) ||                           \
     ((__PRESCALER__) == SPI_BAUDRATEPRESCALER_16) ||                          \
     ((__PRESCALER__) == SPI_BAUDRATEPRESCALER_32) ||                          \
     ((__PRESCALER__) == SPI_BAUDRATEPRESCALER_64) ||                          \
     ((__PRESCALER__) == SPI_BAUDRATEPRESCALER_128) ||                         \
     ((__PRESCALER__) == SPI_BAUDRATEPRESCALER_256))
#define IS_SPI_FIRST_BIT(__BIT__)                                              \
    (((__BIT__) == SPI_FIRSTBIT_MSB) || ((__BIT__) == SPI_FIRSTBIT_LSB))
#define IS_SPI_TIMODE(__MODE__)                                                \
    (((__MODE__) == SPI_TIMODE_DISABLE) || ((__MODE__) == SPI_TIMODE_ENABLE))
#define IS_SPI_CRC_CALCULATION(__CALCULATION__)                                \
    (((__CALCULATION__) == SPI_CRCCALCULATION_DISABLE) ||                      \
     ((__CALCULATION__) == SPI_CRCCALCULATION_ENABLE))
#define IS_SPI_CRC_POLYNOMIAL(__POLYNOMIAL__)                                  \
    (((__POLYNOMIAL__) >= 0x1U) && ((__POLYNOMIAL__) <= 0xFFFFU) &&            \
     (((__POLYNOMIAL__) & 0x1U) != 0U))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef vsf_spi_t SPI_TypeDef;

typedef struct {
    uint32_t Mode;
    uint32_t Direction;
    uint32_t DataSize;
    uint32_t CLKPolarity;
    uint32_t CLKPhase;
    uint32_t NSS;
    uint32_t BaudRatePrescaler;
    uint32_t FirstBit;
    uint32_t TIMode;
    uint32_t CRCCalculation;
    uint32_t CRCPolynomial;
} SPI_InitTypeDef;

typedef enum {
    HAL_SPI_STATE_RESET      = 0x00U,
    HAL_SPI_STATE_READY      = 0x01U,
    HAL_SPI_STATE_BUSY       = 0x02U,
    HAL_SPI_STATE_BUSY_TX    = 0x03U,
    HAL_SPI_STATE_BUSY_RX    = 0x04U,
    HAL_SPI_STATE_BUSY_TX_RX = 0x05U,
    HAL_SPI_STATE_ERROR      = 0x06U,
    HAL_SPI_STATE_ABORT      = 0x07U,
} HAL_SPI_StateTypeDef;

typedef struct __SPI_HandleTypeDef {
    SPI_TypeDef      *Instance;
    SPI_InitTypeDef   Init;
    uint8_t          *pTxBuffPtr;
    uint16_t          TxXferSize;
    volatile uint16_t TxXferCount;
    uint8_t          *pRxBuffPtr;
    uint16_t          RxXferSize;
    volatile uint16_t RxXferCount;
    void (*RxISR)(struct __SPI_HandleTypeDef *hspi);
    void (*TxISR)(struct __SPI_HandleTypeDef *hspi);
    DMA_HandleTypeDef            *hdmatx;
    DMA_HandleTypeDef            *hdmarx;
    HAL_LockTypeDef               Lock;
    volatile HAL_SPI_StateTypeDef State;
    volatile uint32_t             ErrorCode;
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
    void (*TxCpltCallback)(struct __SPI_HandleTypeDef *hspi);
    void (*RxCpltCallback)(struct __SPI_HandleTypeDef *hspi);
    void (*TxRxCpltCallback)(struct __SPI_HandleTypeDef *hspi);
    void (*TxHalfCpltCallback)(struct __SPI_HandleTypeDef *hspi);
    void (*RxHalfCpltCallback)(struct __SPI_HandleTypeDef *hspi);
    void (*TxRxHalfCpltCallback)(struct __SPI_HandleTypeDef *hspi);
    void (*ErrorCallback)(struct __SPI_HandleTypeDef *hspi);
    void (*AbortCpltCallback)(struct __SPI_HandleTypeDef *hspi);
    void (*MspInitCallback)(struct __SPI_HandleTypeDef *hspi);
    void (*MspDeInitCallback)(struct __SPI_HandleTypeDef *hspi);

#endif
} SPI_HandleTypeDef;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
typedef enum {
    HAL_SPI_TX_COMPLETE_CB_ID         = 0x00U,
    HAL_SPI_RX_COMPLETE_CB_ID         = 0x01U,
    HAL_SPI_TX_RX_COMPLETE_CB_ID      = 0x02U,
    HAL_SPI_TX_HALF_COMPLETE_CB_ID    = 0x03U,
    HAL_SPI_RX_HALF_COMPLETE_CB_ID    = 0x04U,
    HAL_SPI_TX_RX_HALF_COMPLETE_CB_ID = 0x05U,
    HAL_SPI_ERROR_CB_ID               = 0x06U,
    HAL_SPI_ABORT_CB_ID               = 0x07U,
    HAL_SPI_MSPINIT_CB_ID             = 0x08U,
    HAL_SPI_MSPDEINIT_CB_ID           = 0x09U

} HAL_SPI_CallbackIDTypeDef;
typedef void (*pSPI_CallbackTypeDef)(SPI_HandleTypeDef *hspi);
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi);
void              HAL_SPI_MspInit(SPI_HandleTypeDef *hspi);
void              HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
HAL_StatusTypeDef HAL_SPI_RegisterCallback(SPI_HandleTypeDef        *hspi,
                                           HAL_SPI_CallbackIDTypeDef CallbackID,
                                           pSPI_CallbackTypeDef      pCallback);
HAL_StatusTypeDef HAL_SPI_UnRegisterCallback(
    SPI_HandleTypeDef *hspi, HAL_SPI_CallbackIDTypeDef CallbackID);
#endif

HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                   uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                  uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi,
                                          uint8_t *pTxData, uint8_t *pRxData,
                                          uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                      uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                     uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi,
                                             uint8_t *pTxData, uint8_t *pRxData,
                                             uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                       uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                      uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi,
                                              uint8_t           *pTxData,
                                              uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_DMAPause(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DMAResume(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_Abort(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_Abort_IT(SPI_HandleTypeDef *hspi);
void              HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi);
void              HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
void              HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void              HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
void              HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void              HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void              HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void              HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi);
void              HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi);
HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef *hspi);
uint32_t             HAL_SPI_GetError(SPI_HandleTypeDef *hspi);

#ifdef __cplusplus
}
#endif

#endif
