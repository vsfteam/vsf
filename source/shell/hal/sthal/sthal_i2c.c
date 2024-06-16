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

#ifdef HAL_I2C_MODULE_ENABLED

/*============================ MACROS ========================================*/

#    define I2C_TIMEOUT_FLAG      35U
#    define I2C_TIMEOUT_BUSY_FLAG 25U
#    define I2C_TIMEOUT_STOP_FLAG 5U
#    define I2C_NO_OPTION_FRAME   0xFFFF0000U

#    define I2C_DUTYCYCLE_2          0x00000000U
#    define I2C_DUTYCYCLE_16_9       0 /*I2C_CCR_DUTY*/
#    define I2C_ADDRESSINGMODE_7BIT  0x00004000U
#    define I2C_ADDRESSINGMODE_10BIT (/*I2C_OAR1_ADDMODE*/ | 0x00004000U)
#    define I2C_DUALADDRESS_DISABLE  0x00000000U
#    define I2C_DUALADDRESS_ENABLE   0 /*I2C_OAR2_ENDUAL*/
#    define I2C_GENERALCALL_DISABLE  0x00000000U
#    define I2C_GENERALCALL_ENABLE   0 /*I2C_CR1_ENGC*/
#    define I2C_NOSTRETCH_DISABLE    0x00000000U
#    define I2C_NOSTRETCH_ENABLE     0 /*I2C_CR1_NOSTRETCH*/
#    define I2C_MEMADD_SIZE_8BIT     0x00000001U
#    define I2C_MEMADD_SIZE_16BIT    0x00000010U
#    define I2C_DIRECTION_RECEIVE    0x00000000U
#    define I2C_DIRECTION_TRANSMIT   0x00000001U
#    define I2C_FIRST_FRAME          0x00000001U
#    define I2C_FIRST_AND_NEXT_FRAME 0x00000002U
#    define I2C_NEXT_FRAME           0x00000004U
#    define I2C_FIRST_AND_LAST_FRAME 0x00000008U
#    define I2C_LAST_FRAME_NO_STOP   0x00000010U
#    define I2C_LAST_FRAME           0x00000020U
#    define I2C_OTHER_FRAME          (0x00AA0000U)
#    define I2C_OTHER_AND_LAST_FRAME (0xAA000000U)
#    define I2C_IT_BUF               0 /*I2C_CR2_ITBUFEN*/
#    define I2C_IT_EVT               0 /*I2C_CR2_ITEVTEN*/
#    define I2C_IT_ERR               0 /*I2C_CR2_ITERREN*/
#    define I2C_FLAG_OVR             0x00010800U
#    define I2C_FLAG_AF              0x00010400U
#    define I2C_FLAG_ARLO            0x00010200U
#    define I2C_FLAG_BERR            0x00010100U
#    define I2C_FLAG_TXE             0x00010080U
#    define I2C_FLAG_RXNE            0x00010040U
#    define I2C_FLAG_STOPF           0x00010010U
#    define I2C_FLAG_ADD10           0x00010008U
#    define I2C_FLAG_BTF             0x00010004U
#    define I2C_FLAG_ADDR            0x00010002U
#    define I2C_FLAG_SB              0x00010001U
#    define I2C_FLAG_DUALF           0x00100080U
#    define I2C_FLAG_GENCALL         0x00100010U
#    define I2C_FLAG_TRA             0x00100004U
#    define I2C_FLAG_BUSY            0x00100002U
#    define I2C_FLAG_MSL             0x00100001U

#    define I2C_FLAG_MASK              0x0000FFFFU
#    define I2C_MIN_PCLK_FREQ_STANDARD 2000000U
#    define I2C_MIN_PCLK_FREQ_FAST     4000000U

#    define I2C_STATE_MSK                                                      \
        ((uint32_t)((uint32_t)((uint32_t)HAL_I2C_STATE_BUSY_TX |               \
                               (uint32_t)HAL_I2C_STATE_BUSY_RX) &              \
                    (uint32_t)(~((uint32_t)HAL_I2C_STATE_READY))))
#    define I2C_STATE_NONE ((uint32_t)(HAL_I2C_MODE_NONE))
#    define I2C_STATE_MASTER_BUSY_TX                                           \
        ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_TX & I2C_STATE_MSK) |        \
                    (uint32_t)HAL_I2C_MODE_MASTER))
#    define I2C_STATE_MASTER_BUSY_RX                                           \
        ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_RX & I2C_STATE_MSK) |        \
                    (uint32_t)HAL_I2C_MODE_MASTER))
#    define I2C_STATE_SLAVE_BUSY_TX                                            \
        ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_TX & I2C_STATE_MSK) |        \
                    (uint32_t)HAL_I2C_MODE_SLAVE))
#    define I2C_STATE_SLAVE_BUSY_RX                                            \
        ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_RX & I2C_STATE_MSK) |        \
                    (uint32_t)HAL_I2C_MODE_SLAVE))

/*============================ MACROFIED FUNCTIONS ===========================*/

#    define I2C_GET_DMA_REMAIN_DATA(__HANDLE__)                                \
        __HAL_DMA_GET_COUNTER(__HANDLE__)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c)
{
    uint32_t freqrange;
    uint32_t pclk1;
    if (hi2c == NULL) {
        return HAL_ERROR;
    }

    if (hi2c->State == HAL_I2C_STATE_RESET) {

        hi2c->Lock = HAL_UNLOCKED;

#    if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
        hi2c->MasterTxCpltCallback = HAL_I2C_MasterTxCpltCallback;
        hi2c->MasterRxCpltCallback = HAL_I2C_MasterRxCpltCallback;
        hi2c->SlaveTxCpltCallback  = HAL_I2C_SlaveTxCpltCallback;
        hi2c->SlaveRxCpltCallback  = HAL_I2C_SlaveRxCpltCallback;
        hi2c->ListenCpltCallback   = HAL_I2C_ListenCpltCallback;
        hi2c->MemTxCpltCallback    = HAL_I2C_MemTxCpltCallback;
        hi2c->MemRxCpltCallback    = HAL_I2C_MemRxCpltCallback;
        hi2c->ErrorCallback        = HAL_I2C_ErrorCallback;
        hi2c->AbortCpltCallback    = HAL_I2C_AbortCpltCallback;
        hi2c->AddrCallback         = HAL_I2C_AddrCallback;
        if (hi2c->MspInitCallback == NULL) {
            hi2c->MspInitCallback = HAL_I2C_MspInit;
        }

        hi2c->MspInitCallback(hi2c);
#    else
        HAL_I2C_MspInit(hi2c);
#    endif
    }

    hi2c->State         = HAL_I2C_STATE_BUSY;
    hi2c->ErrorCode     = HAL_I2C_ERROR_NONE;
    hi2c->State         = HAL_I2C_STATE_READY;
    hi2c->PreviousState = I2C_STATE_NONE;
    hi2c->Mode          = HAL_I2C_MODE_NONE;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c)
{

    if (hi2c == NULL) {
        return HAL_ERROR;
    }

    hi2c->State = HAL_I2C_STATE_BUSY;
    //__HAL_I2C_DISABLE(hi2c);

#    if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
    if (hi2c->MspDeInitCallback == NULL) {
        hi2c->MspDeInitCallback = HAL_I2C_MspDeInit;
    }

    hi2c->MspDeInitCallback(hi2c);
#    else
    HAL_I2C_MspDeInit(hi2c);
#    endif

    hi2c->ErrorCode     = HAL_I2C_ERROR_NONE;
    hi2c->State         = HAL_I2C_STATE_RESET;
    hi2c->PreviousState = I2C_STATE_NONE;
    hi2c->Mode          = HAL_I2C_MODE_NONE;
    __HAL_UNLOCK(hi2c);
    return HAL_OK;
}

__weak void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

__weak void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

#    if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_I2C_RegisterCallback(I2C_HandleTypeDef        *hi2c,
                                           HAL_I2C_CallbackIDTypeDef CallbackID,
                                           pI2C_CallbackTypeDef      pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {
        hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
        return HAL_ERROR;
    }
    __HAL_LOCK(hi2c);
    if (HAL_I2C_STATE_READY == hi2c->State) {
        switch (CallbackID) {
        case HAL_I2C_MASTER_TX_COMPLETE_CB_ID:
            hi2c->MasterTxCpltCallback = pCallback;
            break;
        case HAL_I2C_MASTER_RX_COMPLETE_CB_ID:
            hi2c->MasterRxCpltCallback = pCallback;
            break;
        case HAL_I2C_SLAVE_TX_COMPLETE_CB_ID:
            hi2c->SlaveTxCpltCallback = pCallback;
            break;
        case HAL_I2C_SLAVE_RX_COMPLETE_CB_ID:
            hi2c->SlaveRxCpltCallback = pCallback;
            break;
        case HAL_I2C_LISTEN_COMPLETE_CB_ID:
            hi2c->ListenCpltCallback = pCallback;
            break;
        case HAL_I2C_MEM_TX_COMPLETE_CB_ID:
            hi2c->MemTxCpltCallback = pCallback;
            break;
        case HAL_I2C_MEM_RX_COMPLETE_CB_ID:
            hi2c->MemRxCpltCallback = pCallback;
            break;
        case HAL_I2C_ERROR_CB_ID:
            hi2c->ErrorCallback = pCallback;
            break;
        case HAL_I2C_ABORT_CB_ID:
            hi2c->AbortCpltCallback = pCallback;
            break;
        case HAL_I2C_MSPINIT_CB_ID:
            hi2c->MspInitCallback = pCallback;
            break;
        case HAL_I2C_MSPDEINIT_CB_ID:
            hi2c->MspDeInitCallback = pCallback;
            break;
        default:
            hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (HAL_I2C_STATE_RESET == hi2c->State) {
        switch (CallbackID) {
        case HAL_I2C_MSPINIT_CB_ID:
            hi2c->MspInitCallback = pCallback;
            break;
        case HAL_I2C_MSPDEINIT_CB_ID:
            hi2c->MspDeInitCallback = pCallback;
            break;
        default:
            hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    __HAL_UNLOCK(hi2c);
    return status;
}

HAL_StatusTypeDef HAL_I2C_UnRegisterCallback(
    I2C_HandleTypeDef *hi2c, HAL_I2C_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;
    __HAL_LOCK(hi2c);
    if (HAL_I2C_STATE_READY == hi2c->State) {
        switch (CallbackID) {
        case HAL_I2C_MASTER_TX_COMPLETE_CB_ID:
            hi2c->MasterTxCpltCallback = HAL_I2C_MasterTxCpltCallback;
            break;
        case HAL_I2C_MASTER_RX_COMPLETE_CB_ID:
            hi2c->MasterRxCpltCallback = HAL_I2C_MasterRxCpltCallback;
            break;
        case HAL_I2C_SLAVE_TX_COMPLETE_CB_ID:
            hi2c->SlaveTxCpltCallback = HAL_I2C_SlaveTxCpltCallback;
            break;
        case HAL_I2C_SLAVE_RX_COMPLETE_CB_ID:
            hi2c->SlaveRxCpltCallback = HAL_I2C_SlaveRxCpltCallback;
            break;
        case HAL_I2C_LISTEN_COMPLETE_CB_ID:
            hi2c->ListenCpltCallback = HAL_I2C_ListenCpltCallback;
            break;
        case HAL_I2C_MEM_TX_COMPLETE_CB_ID:
            hi2c->MemTxCpltCallback = HAL_I2C_MemTxCpltCallback;
            break;
        case HAL_I2C_MEM_RX_COMPLETE_CB_ID:
            hi2c->MemRxCpltCallback = HAL_I2C_MemRxCpltCallback;
            break;
        case HAL_I2C_ERROR_CB_ID:
            hi2c->ErrorCallback = HAL_I2C_ErrorCallback;
            break;
        case HAL_I2C_ABORT_CB_ID:
            hi2c->AbortCpltCallback = HAL_I2C_AbortCpltCallback;
            break;
        case HAL_I2C_MSPINIT_CB_ID:
            hi2c->MspInitCallback = HAL_I2C_MspInit;
            break;
        case HAL_I2C_MSPDEINIT_CB_ID:
            hi2c->MspDeInitCallback = HAL_I2C_MspDeInit;
            break;
        default:
            hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (HAL_I2C_STATE_RESET == hi2c->State) {
        switch (CallbackID) {
        case HAL_I2C_MSPINIT_CB_ID:
            hi2c->MspInitCallback = HAL_I2C_MspInit;
            break;
        case HAL_I2C_MSPDEINIT_CB_ID:
            hi2c->MspDeInitCallback = HAL_I2C_MspDeInit;
            break;
        default:
            hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    __HAL_UNLOCK(hi2c);
    return status;
}

HAL_StatusTypeDef HAL_I2C_RegisterAddrCallback(
    I2C_HandleTypeDef *hi2c, pI2C_AddrCallbackTypeDef pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {
        hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
        return HAL_ERROR;
    }
    __HAL_LOCK(hi2c);
    if (HAL_I2C_STATE_READY == hi2c->State) {
        hi2c->AddrCallback = pCallback;
    } else {
        hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    __HAL_UNLOCK(hi2c);
    return status;
}

HAL_StatusTypeDef HAL_I2C_UnRegisterAddrCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef status = HAL_OK;
    __HAL_LOCK(hi2c);
    if (HAL_I2C_STATE_READY == hi2c->State) {
        hi2c->AddrCallback = HAL_I2C_AddrCallback;
    } else {
        hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    __HAL_UNLOCK(hi2c);
    return status;
}
#    endif

HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c,
                                          uint16_t DevAddress, uint8_t *pData,
                                          uint16_t Size, uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c,
                                         uint16_t DevAddress, uint8_t *pData,
                                         uint16_t Size, uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c,
                                         uint8_t *pData, uint16_t Size,
                                         uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData,
                                        uint16_t Size, uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                             uint16_t           DevAddress,
                                             uint8_t *pData, uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c,
                                            uint16_t DevAddress, uint8_t *pData,
                                            uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                            uint8_t *pData, uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Receive_IT(I2C_HandleTypeDef *hi2c,
                                           uint8_t *pData, uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                              uint16_t           DevAddress,
                                              uint8_t *pData, uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                             uint16_t           DevAddress,
                                             uint8_t *pData, uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                             uint8_t *pData, uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                            uint8_t *pData, uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c,
                                    uint16_t DevAddress, uint16_t MemAddress,
                                    uint16_t MemAddSize, uint8_t *pData,
                                    uint16_t Size, uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                   uint16_t MemAddress, uint16_t MemAddSize,
                                   uint8_t *pData, uint16_t Size,
                                   uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Write_IT(I2C_HandleTypeDef *hi2c,
                                       uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData,
                                       uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *hi2c,
                                      uint16_t DevAddress, uint16_t MemAddress,
                                      uint16_t MemAddSize, uint8_t *pData,
                                      uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c,
                                        uint16_t           DevAddress,
                                        uint16_t           MemAddress,
                                        uint16_t MemAddSize, uint8_t *pData,
                                        uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c,
                                       uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData,
                                       uint16_t Size)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c,
                                        uint16_t DevAddress, uint32_t Trials,
                                        uint32_t Timeout)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                                 uint16_t           DevAddress,
                                                 uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                                  uint16_t           DevAddress,
                                                  uint8_t *pData, uint16_t Size,
                                                  uint32_t XferOptions)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_IT(I2C_HandleTypeDef *hi2c,
                                                uint16_t           DevAddress,
                                                uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                                 uint16_t           DevAddress,
                                                 uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                                uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                                 uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_IT(I2C_HandleTypeDef *hi2c,
                                               uint8_t *pData, uint16_t Size,
                                               uint32_t XferOptions)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                                uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_EnableListen_IT(I2C_HandleTypeDef *hi2c)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_DisableListen_IT(I2C_HandleTypeDef *hi2c)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Abort_IT(I2C_HandleTypeDef *hi2c,
                                          uint16_t           DevAddress)
{
    return HAL_OK;
}

void HAL_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c) {}

void HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c) {}

__weak void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

__weak void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

__weak void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

__weak void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

__weak void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c,
                                 uint8_t            TransferDirection,
                                 uint16_t           AddrMatchCode)
{

    VSF_UNUSED_PARAM(hi2c);
    VSF_UNUSED_PARAM(TransferDirection);
    VSF_UNUSED_PARAM(AddrMatchCode);
}

__weak void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{

    VSF_UNUSED_PARAM(hi2c);
}

__weak void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{

    VSF_UNUSED_PARAM(hi2c);
}

__weak void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{

    VSF_UNUSED_PARAM(hi2c);
}

__weak void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

__weak void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c)
{
    return hi2c->State;
}

HAL_I2C_ModeTypeDef HAL_I2C_GetMode(I2C_HandleTypeDef *hi2c)
{
    return hi2c->Mode;
}

uint32_t HAL_I2C_GetError(I2C_HandleTypeDef *hi2c)
{
    return hi2c->ErrorCode;
}

HAL_StatusTypeDef HAL_I2CEx_ConfigAnalogFilter(I2C_HandleTypeDef *hi2c,
                                               uint32_t           AnalogFilter)
{
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2CEx_ConfigDigitalFilter(I2C_HandleTypeDef *hi2c,
                                                uint32_t DigitalFilter)
{
    return HAL_OK;
}

#endif
