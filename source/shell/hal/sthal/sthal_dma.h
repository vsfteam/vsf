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

#ifndef __ST_HAL_DMA_H__
#define __ST_HAL_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include "sthal_def.h"

/*============================ MACROS ========================================*/

#define HAL_DMA_ERROR_NONE          0x00000000U
#define HAL_DMA_ERROR_TE            0x00000001U
#define HAL_DMA_ERROR_FE            0x00000002U
#define HAL_DMA_ERROR_DME           0x00000004U
#define HAL_DMA_ERROR_TIMEOUT       0x00000020U
#define HAL_DMA_ERROR_PARAM         0x00000040U
#define HAL_DMA_ERROR_NO_XFER       0x00000080U
#define HAL_DMA_ERROR_NOT_SUPPORTED 0x00000100U

#define DMA_CHANNEL_0 0x00000000U
#define DMA_CHANNEL_1 0x02000000U
#define DMA_CHANNEL_2 0x04000000U
#define DMA_CHANNEL_3 0x06000000U
#define DMA_CHANNEL_4 0x08000000U
#define DMA_CHANNEL_5 0x0A000000U
#define DMA_CHANNEL_6 0x0C000000U
#define DMA_CHANNEL_7 0x0E000000U
#if defined(DMA_SxCR_CHSEL_3)
#    define DMA_CHANNEL_8  0x10000000U
#    define DMA_CHANNEL_9  0x12000000U
#    define DMA_CHANNEL_10 0x14000000U
#    define DMA_CHANNEL_11 0x16000000U
#    define DMA_CHANNEL_12 0x18000000U
#    define DMA_CHANNEL_13 0x1A000000U
#    define DMA_CHANNEL_14 0x1C000000U
#    define DMA_CHANNEL_15 0x1E000000U
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct {
    uint32_t Channel;
    uint32_t Direction;
    uint32_t PeriphInc;
    uint32_t MemInc;
    uint32_t PeriphDataAlignment;
    uint32_t MemDataAlignment;
    uint32_t Mode;
    uint32_t Priority;
    uint32_t FIFOMode;
    uint32_t FIFOThreshold;
    uint32_t MemBurst;
    uint32_t PeriphBurst;
} DMA_InitTypeDef;

typedef enum {
    HAL_DMA_STATE_RESET   = 0x00U,
    HAL_DMA_STATE_READY   = 0x01U,
    HAL_DMA_STATE_BUSY    = 0x02U,
    HAL_DMA_STATE_TIMEOUT = 0x03U,
    HAL_DMA_STATE_ERROR   = 0x04U,
    HAL_DMA_STATE_ABORT   = 0x05U,
} HAL_DMA_StateTypeDef;

typedef enum {
    HAL_DMA_FULL_TRANSFER = 0x00U,
    HAL_DMA_HALF_TRANSFER = 0x01U
} HAL_DMA_LevelCompleteTypeDef;

typedef enum {
    HAL_DMA_XFER_CPLT_CB_ID       = 0x00U,
    HAL_DMA_XFER_HALFCPLT_CB_ID   = 0x01U,
    HAL_DMA_XFER_M1CPLT_CB_ID     = 0x02U,
    HAL_DMA_XFER_M1HALFCPLT_CB_ID = 0x03U,
    HAL_DMA_XFER_ERROR_CB_ID      = 0x04U,
    HAL_DMA_XFER_ABORT_CB_ID      = 0x05U,
    HAL_DMA_XFER_ALL_CB_ID        = 0x06U
} HAL_DMA_CallbackIDTypeDef;

typedef struct __DMA_HandleTypeDef {
    DMA_Stream_TypeDef       *Instance;
    DMA_InitTypeDef           Init;
    HAL_LockTypeDef           Lock;
    __IO HAL_DMA_StateTypeDef State;
    void                     *Parent;
    void (*XferCpltCallback)(struct __DMA_HandleTypeDef *hdma);
    void (*XferHalfCpltCallback)(struct __DMA_HandleTypeDef *hdma);
    void (*XferM1CpltCallback)(struct __DMA_HandleTypeDef *hdma);
    void (*XferM1HalfCpltCallback)(struct __DMA_HandleTypeDef *hdma);
    void (*XferErrorCallback)(struct __DMA_HandleTypeDef *hdma);
    void (*XferAbortCallback)(struct __DMA_HandleTypeDef *hdma);
    __IO uint32_t ErrorCode;
    uint32_t      StreamBaseAddress;
    uint32_t      StreamIndex;

} DMA_HandleTypeDef;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma);

HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress,
                                uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress,
                                   uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_PollForTransfer(
    DMA_HandleTypeDef *hdma, HAL_DMA_LevelCompleteTypeDef CompleteLevel,
    uint32_t Timeout);
void              HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_CleanCallbacks(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_RegisterCallback(
    DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID,
    void (*pCallback)(DMA_HandleTypeDef *_hdma));
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(
    DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID);

HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma);
uint32_t             HAL_DMA_GetError(DMA_HandleTypeDef *hdma);

#ifdef __cplusplus
}
#endif

#endif
