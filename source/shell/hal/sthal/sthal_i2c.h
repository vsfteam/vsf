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

#ifndef __ST_HAL_I2C_H__
#define __ST_HAL_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include "sthal_def.h"

/*============================ MACROS ========================================*/

#define HAL_I2C_ERROR_NONE      0x00000000U
#define HAL_I2C_ERROR_BERR      0x00000001U
#define HAL_I2C_ERROR_ARLO      0x00000002U
#define HAL_I2C_ERROR_AF        0x00000004U
#define HAL_I2C_ERROR_OVR       0x00000008U
#define HAL_I2C_ERROR_DMA       0x00000010U
#define HAL_I2C_ERROR_TIMEOUT   0x00000020U
#define HAL_I2C_ERROR_SIZE      0x00000040U
#define HAL_I2C_ERROR_DMA_PARAM 0x00000080U
#define HAL_I2C_WRONG_START     0x00000200U

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
#    define HAL_I2C_ERROR_INVALID_CALLBACK 0x00000100U
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
#    define __HAL_I2C_RESET_HANDLE_STATE(__HANDLE__)                           \
        do {                                                                   \
            (__HANDLE__)->State             = HAL_I2C_STATE_RESET;             \
            (__HANDLE__)->MspInitCallback   = NULL;                            \
            (__HANDLE__)->MspDeInitCallback = NULL;                            \
        } while (0)
#else
#    define __HAL_I2C_RESET_HANDLE_STATE(__HANDLE__)                           \
        ((__HANDLE__)->State = HAL_I2C_STATE_RESET)
#endif

/*============================ TYPES =========================================*/

typedef vsf_i2c_t I2C_TypeDef;

typedef enum {
    HAL_I2C_STATE_RESET          = 0x00U,
    HAL_I2C_STATE_READY          = 0x20U,
    HAL_I2C_STATE_BUSY           = 0x24U,
    HAL_I2C_STATE_BUSY_TX        = 0x21U,
    HAL_I2C_STATE_BUSY_RX        = 0x22U,
    HAL_I2C_STATE_LISTEN         = 0x28U,
    HAL_I2C_STATE_BUSY_TX_LISTEN = 0x29U,
    HAL_I2C_STATE_BUSY_RX_LISTEN = 0x2AU,
    HAL_I2C_STATE_ABORT          = 0x60U,
    HAL_I2C_STATE_TIMEOUT        = 0xA0U,
    HAL_I2C_STATE_ERROR          = 0xE0U,
} HAL_I2C_StateTypeDef;

typedef enum {
    HAL_I2C_MODE_NONE   = 0x00U,
    HAL_I2C_MODE_MASTER = 0x10U,
    HAL_I2C_MODE_SLAVE  = 0x20U,
    HAL_I2C_MODE_MEM    = 0x40U,
} HAL_I2C_ModeTypeDef;

typedef struct {
    uint32_t ClockSpeed;
    uint32_t DutyCycle;
    uint32_t OwnAddress1;
    uint32_t AddressingMode;
    uint32_t DualAddressMode;
    uint32_t OwnAddress2;
    uint32_t GeneralCallMode;
    uint32_t NoStretchMode;
} I2C_InitTypeDef;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
typedef struct __I2C_HandleTypeDef {
#else
typedef struct {
#endif
    I2C_TypeDef              *Instance;
    I2C_InitTypeDef           Init;
    uint8_t                  *pBuffPtr;
    uint16_t                  XferSize;
    volatile uint16_t             XferCount;
    volatile uint32_t             XferOptions;
    volatile uint32_t             PreviousState;
    DMA_HandleTypeDef        *hdmatx;
    DMA_HandleTypeDef        *hdmarx;
    HAL_LockTypeDef           Lock;
    volatile HAL_I2C_StateTypeDef State;
    volatile HAL_I2C_ModeTypeDef  Mode;
    volatile uint32_t             ErrorCode;
    volatile uint32_t             Devaddress;
    volatile uint32_t             Memaddress;
    volatile uint32_t             MemaddSize;
    volatile uint32_t             EventCount;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
    void (*MasterTxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
    void (*MasterRxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
    void (*SlaveTxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
    void (*SlaveRxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
    void (*ListenCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
    void (*MemTxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
    void (*MemRxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
    void (*ErrorCallback)(struct __I2C_HandleTypeDef *hi2c);
    void (*AbortCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
    void (*AddrCallback)(struct __I2C_HandleTypeDef *hi2c,
                         uint8_t TransferDirection, uint16_t AddrMatchCode);
    void (*MspInitCallback)(struct __I2C_HandleTypeDef *hi2c);
    void (*MspDeInitCallback)(struct __I2C_HandleTypeDef *hi2c);
#endif
} I2C_HandleTypeDef;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
typedef enum {
    HAL_I2C_MASTER_TX_COMPLETE_CB_ID = 0x00U,
    HAL_I2C_MASTER_RX_COMPLETE_CB_ID = 0x01U,
    HAL_I2C_SLAVE_TX_COMPLETE_CB_ID  = 0x02U,
    HAL_I2C_SLAVE_RX_COMPLETE_CB_ID  = 0x03U,
    HAL_I2C_LISTEN_COMPLETE_CB_ID    = 0x04U,
    HAL_I2C_MEM_TX_COMPLETE_CB_ID    = 0x05U,
    HAL_I2C_MEM_RX_COMPLETE_CB_ID    = 0x06U,
    HAL_I2C_ERROR_CB_ID              = 0x07U,
    HAL_I2C_ABORT_CB_ID              = 0x08U,

    HAL_I2C_MSPINIT_CB_ID   = 0x09U,
    HAL_I2C_MSPDEINIT_CB_ID = 0x0AU,
} HAL_I2C_CallbackIDTypeDef;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c);
void              HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c);
void              HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c);

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
typedef void (*pI2C_CallbackTypeDef)(I2C_HandleTypeDef *hi2c);
typedef void (*pI2C_AddrCallbackTypeDef)(I2C_HandleTypeDef *hi2c,
                                         uint8_t            TransferDirection,
                                         uint16_t           AddrMatchCode);
HAL_StatusTypeDef HAL_I2C_RegisterCallback(I2C_HandleTypeDef        *hi2c,
                                           HAL_I2C_CallbackIDTypeDef CallbackID,
                                           pI2C_CallbackTypeDef      pCallback);
HAL_StatusTypeDef HAL_I2C_UnRegisterCallback(
    I2C_HandleTypeDef *hi2c, HAL_I2C_CallbackIDTypeDef CallbackID);

HAL_StatusTypeDef HAL_I2C_RegisterAddrCallback(
    I2C_HandleTypeDef *hi2c, pI2C_AddrCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_I2C_UnRegisterAddrCallback(I2C_HandleTypeDef *hi2c);
#endif

HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c,
                                          uint16_t DevAddress, uint8_t *pData,
                                          uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c,
                                         uint16_t DevAddress, uint8_t *pData,
                                         uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c,
                                         uint8_t *pData, uint16_t Size,
                                         uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData,
                                        uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c,
                                    uint16_t DevAddress, uint16_t MemAddress,
                                    uint16_t MemAddSize, uint8_t *pData,
                                    uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                   uint16_t MemAddress, uint16_t MemAddSize,
                                   uint8_t *pData, uint16_t Size,
                                   uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c,
                                        uint16_t DevAddress, uint32_t Trials,
                                        uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                             uint16_t           DevAddress,
                                             uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c,
                                            uint16_t DevAddress, uint8_t *pData,
                                            uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                            uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Receive_IT(I2C_HandleTypeDef *hi2c,
                                           uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Write_IT(I2C_HandleTypeDef *hi2c,
                                       uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData,
                                       uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *hi2c,
                                      uint16_t DevAddress, uint16_t MemAddress,
                                      uint16_t MemAddSize, uint8_t *pData,
                                      uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                                 uint16_t           DevAddress,
                                                 uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_IT(I2C_HandleTypeDef *hi2c,
                                                uint16_t           DevAddress,
                                                uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                                uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_IT(I2C_HandleTypeDef *hi2c,
                                               uint8_t *pData, uint16_t Size,
                                               uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_EnableListen_IT(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DisableListen_IT(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_Master_Abort_IT(I2C_HandleTypeDef *hi2c,
                                          uint16_t           DevAddress);
HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                              uint16_t           DevAddress,
                                              uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                             uint16_t           DevAddress,
                                             uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                             uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                            uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c,
                                        uint16_t           DevAddress,
                                        uint16_t           MemAddress,
                                        uint16_t MemAddSize, uint8_t *pData,
                                        uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c,
                                       uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData,
                                       uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                                  uint16_t           DevAddress,
                                                  uint8_t *pData, uint16_t Size,
                                                  uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                                 uint16_t           DevAddress,
                                                 uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                                 uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                                uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions);
void              HAL_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c);
void              HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c);
void              HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
void              HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);
void              HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
void              HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection,
                          uint16_t AddrMatchCode);
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c);
HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c);
HAL_I2C_ModeTypeDef  HAL_I2C_GetMode(I2C_HandleTypeDef *hi2c);
uint32_t             HAL_I2C_GetError(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef    HAL_I2CEx_ConfigAnalogFilter(I2C_HandleTypeDef *hi2c,
                                                  uint32_t           AnalogFilter);
HAL_StatusTypeDef    HAL_I2CEx_ConfigDigitalFilter(I2C_HandleTypeDef *hi2c,
                                                   uint32_t DigitalFilter);

#ifdef __cplusplus
}
#endif

#endif
