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

#if defined(HAL_I2C_MODULE_ENABLED) && VSF_HAL_USE_I2C == ENABLED

#include "sthal_internal.h"

/*============================ MACROS ========================================*/

#   define I2C_STATE_MSK                                                       \
       ((uint32_t)((uint32_t)((uint32_t)HAL_I2C_STATE_BUSY_TX |                \
                              (uint32_t)HAL_I2C_STATE_BUSY_RX) &               \
                   (uint32_t)(~((uint32_t)HAL_I2C_STATE_READY))))
#   define I2C_STATE_NONE ((uint32_t)(HAL_I2C_MODE_NONE))
#   define I2C_STATE_MASTER_BUSY_TX                                            \
       ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_TX & I2C_STATE_MSK) |         \
                   (uint32_t)HAL_I2C_MODE_MASTER))
#   define I2C_STATE_MASTER_BUSY_RX                                            \
       ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_RX & I2C_STATE_MSK) |         \
                   (uint32_t)HAL_I2C_MODE_MASTER))
#   define I2C_STATE_SLAVE_BUSY_TX                                             \
       ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_TX & I2C_STATE_MSK) |         \
                   (uint32_t)HAL_I2C_MODE_SLAVE))
#   define I2C_STATE_SLAVE_BUSY_RX                                             \
       ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_RX & I2C_STATE_MSK) |         \
                   (uint32_t)HAL_I2C_MODE_SLAVE))

#   define I2C_TIMEOUT_FLAG      35U
#   define I2C_TIMEOUT_BUSY_FLAG 25U
#   define I2C_NO_OPTION_FRAME   0xFFFF0000U

/*============================ MACROFIED FUNCTIONS ===========================*/

#   define I2C_GET_DMA_REMAIN_DATA(__HANDLE__) __HAL_DMA_GET_COUNTER(__HANDLE__)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static bool __i2c_master_fifo_transnfer_continue(I2C_HandleTypeDef *hi2c);
static bool __i2c_slave_fifo_transnfer_continue(I2C_HandleTypeDef *hi2c,
                                                bool               is_transmit);

/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(HAL_I2C_MspInit)
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

VSF_CAL_WEAK(HAL_I2C_MspDeInit)
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

VSF_CAL_WEAK(HAL_I2C_MasterTxCpltCallback)
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

VSF_CAL_WEAK(HAL_I2C_MasterRxCpltCallback)
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

VSF_CAL_WEAK(HAL_I2C_SlaveTxCpltCallback)
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

VSF_CAL_WEAK(HAL_I2C_SlaveRxCpltCallback)
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

VSF_CAL_WEAK(HAL_I2C_AddrCallback)
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection,
                          uint16_t AddrMatchCode)
{

    VSF_UNUSED_PARAM(hi2c);
    VSF_UNUSED_PARAM(TransferDirection);
    VSF_UNUSED_PARAM(AddrMatchCode);
}

VSF_CAL_WEAK(HAL_I2C_ListenCpltCallback)
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{

    VSF_UNUSED_PARAM(hi2c);
}

VSF_CAL_WEAK(HAL_I2C_MemTxCpltCallback)
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{

    VSF_UNUSED_PARAM(hi2c);
}

VSF_CAL_WEAK(HAL_I2C_MemRxCpltCallback)
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{

    VSF_UNUSED_PARAM(hi2c);
}

VSF_CAL_WEAK(HAL_I2C_ErrorCallback)
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

VSF_CAL_WEAK(HAL_I2C_AbortCpltCallback)
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
    VSF_UNUSED_PARAM(hi2c);
}

#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_I2C_RegisterCallback(I2C_HandleTypeDef        *hi2c,
                                           HAL_I2C_CallbackIDTypeDef CallbackID,
                                           pI2C_CallbackTypeDef      pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {
        hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
        return HAL_ERROR;
    }
    VSF_STHAL_LOCK(hi2c);
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

    VSF_STHAL_UNLOCK(hi2c);
    return status;
}

HAL_StatusTypeDef HAL_I2C_UnRegisterCallback(
    I2C_HandleTypeDef *hi2c, HAL_I2C_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;
    VSF_STHAL_LOCK(hi2c);
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

    VSF_STHAL_UNLOCK(hi2c);
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
    VSF_STHAL_LOCK(hi2c);
    if (HAL_I2C_STATE_READY == hi2c->State) {
        hi2c->AddrCallback = pCallback;
    } else {
        hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hi2c);
    return status;
}

HAL_StatusTypeDef HAL_I2C_UnRegisterAddrCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef status = HAL_OK;
    VSF_STHAL_LOCK(hi2c);
    if (HAL_I2C_STATE_READY == hi2c->State) {
        hi2c->AddrCallback = HAL_I2C_AddrCallback;
    } else {
        hi2c->ErrorCode |= HAL_I2C_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hi2c);
    return status;
}
#   endif

static void __i2c_isr_handler(void *target_ptr, vsf_i2c_t *i2c_ptr,
                              vsf_i2c_irq_mask_t irq_mask)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)target_ptr;
    VSF_STHAL_ASSERT(hi2c != NULL);

    if (irq_mask &
        (VSF_I2C_IRQ_MASK_MASTER_TX | VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT)) {
        if ((hi2c->State == HAL_I2C_STATE_BUSY_TX) ||
            ((hi2c->Mode == HAL_I2C_MODE_MEM) &&
             (hi2c->State == HAL_I2C_STATE_BUSY_RX))) {
            if (!__i2c_master_fifo_transnfer_continue(hi2c)) {
                vsf_i2c_irq_disable(i2c_ptr,
                                    VSF_I2C_IRQ_MASK_MASTER_TX |
                                        VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT);

                if ((hi2c->XferOptions != I2C_FIRST_AND_LAST_FRAME) &&
                    (hi2c->XferOptions != I2C_LAST_FRAME) &&
                    (hi2c->XferOptions != I2C_NO_OPTION_FRAME)) {
                    hi2c->PreviousState = I2C_STATE_MASTER_BUSY_TX;
                } else {
                    hi2c->PreviousState = I2C_STATE_NONE;
                }
                hi2c->State = HAL_I2C_STATE_READY;
                if (hi2c->Mode == HAL_I2C_MODE_MASTER) {
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
                    hi2c->MasterTxCpltCallback(hi2c);
#   else
                    HAL_I2C_MasterTxCpltCallback(hi2c);
#   endif
                } else if (hi2c->Mode == HAL_I2C_MODE_MEM) {
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
                    hi2c->MemTxCpltCallback(hi2c);
#   else
                    HAL_I2C_MemTxCpltCallback(hi2c);
#   endif
                } else {
                    VSF_STHAL_ASSERT(false);
                }
            }
        }
    }

    if (irq_mask &
        (VSF_I2C_IRQ_MASK_MASTER_RX | VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT)) {
        if (hi2c->State == HAL_I2C_STATE_BUSY_RX) {
            if (!__i2c_master_fifo_transnfer_continue(hi2c)) {
                vsf_i2c_irq_disable(i2c_ptr,
                                    VSF_I2C_IRQ_MASK_MASTER_RX |
                                        VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT);
                hi2c->State = HAL_I2C_STATE_READY;
                if (hi2c->Mode == HAL_I2C_MODE_MASTER) {
                    if ((hi2c->XferOptions == I2C_FIRST_AND_LAST_FRAME) ||
                        (hi2c->XferOptions == I2C_LAST_FRAME)) {
                        hi2c->PreviousState = I2C_STATE_NONE;
                    } else {
                        hi2c->PreviousState = I2C_STATE_MASTER_BUSY_RX;
                    }
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
                    hi2c->MasterRxCpltCallback(hi2c);
#   else
                    HAL_I2C_MasterRxCpltCallback(hi2c);
#   endif
                } else if (hi2c->Mode == HAL_I2C_MODE_MEM) {
                    hi2c->PreviousState = I2C_STATE_NONE;
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
                    hi2c->MemRxCpltCallback(hi2c);
#   else
                    HAL_I2C_MemRxCpltCallback(hi2c);
#   endif
                } else {
                    VSF_STHAL_ASSERT(false);
                }
            }
        }
    }

    if (irq_mask & VSF_I2C_IRQ_MASK_SLAVE_TX) {
        if (hi2c->State == HAL_I2C_STATE_BUSY_TX) {
            if (!__i2c_slave_fifo_transnfer_continue(hi2c, true)) {
                vsf_i2c_irq_disable(i2c_ptr, VSF_I2C_IRQ_MASK_SLAVE_TX);

                hi2c->State = HAL_I2C_STATE_READY;
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
                hi2c->SlaveTxCpltCallback(hi2c);
#   else
                HAL_I2C_SlaveTxCpltCallback(hi2c);
#   endif
            }
        }
    }

    if (irq_mask & VSF_I2C_IRQ_MASK_SLAVE_RX) {
        if (hi2c->State == HAL_I2C_STATE_BUSY_RX) {
            if (!__i2c_slave_fifo_transnfer_continue(hi2c, false)) {
                vsf_i2c_irq_disable(i2c_ptr, VSF_I2C_IRQ_MASK_SLAVE_RX);

                hi2c->State = HAL_I2C_STATE_READY;
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
                hi2c->SlaveRxCpltCallback(hi2c);
#   else
                HAL_I2C_SlaveRxCpltCallback(hi2c);
#   endif
            }
        }
    }

    if (irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
        vsf_i2c_irq_disable(i2c_ptr,
                            VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE |
                                VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST |
                                VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK |
                                VSF_I2C_IRQ_MASK_MASTER_ERR);
        HAL_I2C_StateTypeDef State = hi2c->State;
        hi2c->State                = HAL_I2C_STATE_READY;
        if (hi2c->Mode == HAL_I2C_MODE_MASTER) {
            if (State == HAL_I2C_STATE_BUSY_TX) {
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
                hi2c->MasterTxCpltCallback(hi2c);
#   else
                HAL_I2C_MasterTxCpltCallback(hi2c);
#   endif
            } else if (State == HAL_I2C_STATE_BUSY_RX) {
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
                hi2c->MasterRxCpltCallback(hi2c);
#   else
                HAL_I2C_MasterRxCpltCallback(hi2c);
#   endif
            }
        } else if (hi2c->Mode == HAL_I2C_MODE_MEM) {
            if (State == HAL_I2C_STATE_BUSY_TX) {
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
                hi2c->MemTxCpltCallback(hi2c);
#   else
                HAL_I2C_MemTxCpltCallback(hi2c);
#   endif
            } else if (State == HAL_I2C_STATE_BUSY_RX) {
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
                hi2c->MemRxCpltCallback(hi2c);
#   else
                HAL_I2C_MemRxCpltCallback(hi2c);
#   endif
            }
        } else {
            VSF_STHAL_ASSERT(false);
        }
    }

    if (irq_mask & VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE) {
        vsf_i2c_irq_disable(i2c_ptr, VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE);
        HAL_I2C_StateTypeDef State = hi2c->State;
        hi2c->State                = HAL_I2C_STATE_READY;

        if (State == HAL_I2C_STATE_BUSY_TX) {
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
            hi2c->SlaveTxCpltCallback(hi2c);
#   else
            HAL_I2C_SlaveTxCpltCallback(hi2c);
#   endif
        } else if (State == HAL_I2C_STATE_BUSY_RX) {
#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
            hi2c->SlaveRxCpltCallback(hi2c);
#   else
            HAL_I2C_SlaveRxCpltCallback(hi2c);
#   endif
        }
    }
}

static HAL_StatusTypeDef __i2c_check_busy_status(I2C_HandleTypeDef *hi2c,
                                                 uint32_t           Tickstart,
                                                 uint32_t           Timeout)
{
    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    do {
        vsf_i2c_status_t status = vsf_i2c_status(i2c_ptr);
        if (!status.is_busy) {
            break;
        }

        if (__sthal_check_timeout(Tickstart, Timeout)) {
            hi2c->PreviousState = I2C_STATE_NONE;
            hi2c->State         = HAL_I2C_STATE_READY;
            hi2c->Mode          = HAL_I2C_MODE_NONE;
            hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
            VSF_STHAL_UNLOCK(hi2c);
            return HAL_ERROR;
        }
    } while (1);

    return HAL_OK;
}

static vsf_i2c_cmd_t __i2c_get_cmd(I2C_HandleTypeDef *hi2c)
{
    vsf_i2c_cmd_t cmd = 0;
    if ((hi2c->XferOptions == I2C_FIRST_AND_LAST_FRAME) ||
        (hi2c->XferOptions == I2C_FIRST_FRAME) ||
        (hi2c->XferOptions == I2C_NO_OPTION_FRAME) ||
        (hi2c->XferOptions == I2C_OTHER_FRAME) ||
        (hi2c->XferOptions == I2C_OTHER_AND_LAST_FRAME)) {
        cmd |= VSF_I2C_CMD_START;
    } else if ((hi2c->PreviousState == I2C_STATE_MASTER_BUSY_RX) &&
               (hi2c->State == I2C_STATE_MASTER_BUSY_TX)) {
        cmd |= VSF_I2C_CMD_RESTART;
    } else if ((hi2c->PreviousState == I2C_STATE_MASTER_BUSY_TX) &&
               (hi2c->State == I2C_STATE_MASTER_BUSY_RX)) {
        cmd |= VSF_I2C_CMD_RESTART;
    }

    if (hi2c->State == HAL_I2C_STATE_BUSY_TX) {
        cmd |= VSF_I2C_CMD_WRITE;
    } else if (hi2c->State == HAL_I2C_STATE_BUSY_RX) {
        cmd |= VSF_I2C_CMD_READ;
    } else {
        VSF_STHAL_ASSERT(false);
    }

    if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT) {
        cmd |= VSF_I2C_CMD_7_BITS;
    } else {
        cmd |= VSF_I2C_CMD_10_BITS;
    }

    if ((hi2c->XferOptions == I2C_FIRST_AND_LAST_FRAME) ||
        (hi2c->XferOptions == I2C_LAST_FRAME) ||
        (hi2c->XferOptions == I2C_NO_OPTION_FRAME)) {
        cmd |= VSF_I2C_CMD_STOP;
    }

    return cmd;
}

static vsf_i2c_cmd_t __i2c_seq_master_transimit_get_cmd(I2C_HandleTypeDef *hi2c,
                                                        uint32_t XferOptions)
{
    vsf_i2c_cmd_t cmd = VSF_I2C_CMD_WRITE;
    if ((XferOptions == I2C_FIRST_AND_LAST_FRAME) ||
        (XferOptions == I2C_FIRST_FRAME) ||
        (XferOptions == I2C_NO_OPTION_FRAME)) {
        cmd |= VSF_I2C_CMD_START;
    } else if ((hi2c->PreviousState == I2C_STATE_MASTER_BUSY_RX)) {
        cmd |= VSF_I2C_CMD_RESTART;
    }

    if ((XferOptions == I2C_FIRST_AND_LAST_FRAME) ||
        (XferOptions == I2C_LAST_FRAME) ||
        (XferOptions == I2C_NO_OPTION_FRAME)) {
        cmd |= VSF_I2C_CMD_STOP;
    }

    if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT) {
        cmd |= VSF_I2C_CMD_7_BITS;
    } else {
        cmd |= VSF_I2C_CMD_10_BITS;
    }

    return cmd;
}

static vsf_i2c_cmd_t __i2c_seq_master_receive_get_cmd(I2C_HandleTypeDef *hi2c,
                                                      uint32_t XferOptions)
{
    vsf_i2c_cmd_t cmd = VSF_I2C_CMD_READ;
    if ((XferOptions == I2C_FIRST_AND_LAST_FRAME) ||
        (XferOptions == I2C_FIRST_FRAME) ||
        (XferOptions == I2C_NO_OPTION_FRAME)) {
        cmd |= VSF_I2C_CMD_START;
    } else if ((hi2c->PreviousState == I2C_STATE_MASTER_BUSY_TX)) {
        cmd |= VSF_I2C_CMD_RESTART;
    }

    if ((XferOptions == I2C_FIRST_AND_LAST_FRAME) ||
        (XferOptions == I2C_LAST_FRAME) ||
        (XferOptions == I2C_NO_OPTION_FRAME)) {
        cmd |= VSF_I2C_CMD_STOP;
    }

    if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT) {
        cmd |= VSF_I2C_CMD_7_BITS;
    } else {
        cmd |= VSF_I2C_CMD_10_BITS;
    }

    return cmd;
}

static vsf_err_t __i2c_switch_mode(I2C_HandleTypeDef  *hi2c,
                                   HAL_I2C_ModeTypeDef mode_type)
{
    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

#   if defined(VSF_I2C_CTRL_GET_MODE) && defined(VSF_I2C_CTRL_SET_MODE)
    int       mode     = (mode_type == HAL_I2C_MODE_SLAVE) ? 0 : 1;
    int       cur_mode = 0;
    vsf_err_t err = vsf_i2c_ctrl(i2c_ptr, VSF_I2C_CTRL_GET_MODE, &cur_mode);
    if (err != VSF_ERR_NONE) {
        return err;
    }
    if (mode != cur_mode) {
        err = vsf_i2c_ctrl(i2c_ptr, VSF_I2C_CTRL_SET_MODE, &mode);
        if (err != VSF_ERR_NONE) {
            return err;
        }

        uint16_t address;
        if (hi2c->Init.DualAddressMode != I2C_DUALADDRESS_DISABLE) {
            address = hi2c->Init.OwnAddress2;
        } else {
            address = hi2c->Init.OwnAddress1;
        }
        err = vsf_i2c_ctrl(i2c_ptr, VSF_I2C_CTRL_SLAVE_SET_ADDRESS, &address);
        if (err != VSF_ERR_NONE) {
            return err;
        }
    }
    return VSF_ERR_NONE;
#   else
    return VSF_ERR_NOT_SUPPORT;
#   endif
}

static HAL_StatusTypeDef __i2c_transfer_prepare(
    I2C_HandleTypeDef *hi2c, HAL_I2C_StateTypeDef State,
    HAL_I2C_ModeTypeDef mode, uint16_t DevAddress, uint16_t MemAddress,
    uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t XferOptions,
    uint32_t Tickstart, uint32_t Timeout)
{
    if (hi2c == NULL) {
        return HAL_ERROR;
    }

    if (hi2c->State == HAL_I2C_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }

        VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
        vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
        VSF_STHAL_ASSERT(i2c_ptr != NULL);

        if ((Timeout != 0) &&
            __i2c_check_busy_status(hi2c, Tickstart, Timeout) != HAL_OK) {
            return HAL_BUSY;
        }

        VSF_STHAL_LOCK(hi2c);

        hi2c->State       = State;
        hi2c->Mode        = mode;
        hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;
        hi2c->pBuffPtr    = pData;
        hi2c->XferCount   = Size;
        hi2c->XferSize    = hi2c->XferCount;
        hi2c->Devaddress  = DevAddress;
        hi2c->Memaddress  = MemAddress;
        hi2c->MemaddSize  = MemAddSize;
        hi2c->XferOptions = XferOptions;
        hi2c->EventCount  = 0;

        if (__i2c_switch_mode(hi2c, mode) != VSF_ERR_NONE) {
            return HAL_ERROR;
        }

        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

static void __i2c_transfer_process(I2C_HandleTypeDef *hi2c,
                                   uint32_t PreviousState, uint32_t ErrorCode)
{
    hi2c->PreviousState = PreviousState;
    hi2c->State         = HAL_I2C_STATE_READY;
    hi2c->Mode          = HAL_I2C_MODE_NONE;
    hi2c->ErrorCode     = ErrorCode;

    VSF_STHAL_UNLOCK(hi2c);
}

static bool __i2c_check_timeout(I2C_HandleTypeDef *hi2c, uint32_t Tickstart,
                                uint32_t timeout)
{
    if (__sthal_check_timeout(Tickstart, timeout)) {
        hi2c->State = HAL_I2C_STATE_READY;
        VSF_STHAL_UNLOCK(hi2c);
        return true;
    } else {
        return false;
    }
}

static bool __i2c_master_fifo_transnfer(I2C_HandleTypeDef *hi2c,
                                        vsf_i2c_cmd_t      cmd,
                                        vsf_i2c_cmd_t     *current_cmd)
{
    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    if (hi2c->Mode == HAL_I2C_MODE_MEM) {
        if (hi2c->EventCount != hi2c->MemaddSize) {
            vsf_i2c_cmd_t memaddr_cmd =
                VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS;
            vsf_i2c_cmd_t memaddr_current_cmd = 0;
            vsf_i2c_master_fifo_transfer(
                i2c_ptr, hi2c->Devaddress, memaddr_cmd, hi2c->MemaddSize,
                (uint8_t *)&hi2c->Memaddress, &memaddr_current_cmd,
                (uint_fast16_t *)&hi2c->EventCount);

            if (hi2c->EventCount != hi2c->MemaddSize) {
                return true;
            }
        }
    }

    uint_fast16_t offset = 0;
    if (((hi2c->XferCount > 0) && (hi2c->pBuffPtr != NULL)) ||
        (cmd != *current_cmd)) {
        vsf_i2c_master_fifo_transfer(i2c_ptr, hi2c->Devaddress, cmd,
                                     hi2c->XferCount, hi2c->pBuffPtr,
                                     current_cmd, &offset);

        hi2c->XferCount -= offset;
        hi2c->XferSize -= offset;
        hi2c->pBuffPtr += offset;
    }

    return hi2c->XferCount != 0;
}

static bool __i2c_master_fifo_transnfer_continue(I2C_HandleTypeDef *hi2c)
{
    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    vsf_i2c_cmd_t cmd         = __i2c_get_cmd(hi2c);
    vsf_i2c_cmd_t current_cmd = 0;

    return __i2c_master_fifo_transnfer(hi2c, cmd, &current_cmd);
}

static HAL_StatusTypeDef __i2c_master_fifo_transnfer_with_timeout(
    I2C_HandleTypeDef *hi2c, vsf_i2c_cmd_t cmd, uint32_t Tickstart,
    uint32_t Timeout)
{
    vsf_i2c_cmd_t current_cmd = 0;

    while ((hi2c->XferCount > 0) || (cmd != current_cmd)) {
        __i2c_master_fifo_transnfer(hi2c, cmd, &current_cmd);

        if (__i2c_check_timeout(hi2c, Tickstart, Timeout)) {
            return HAL_TIMEOUT;
        }
    }

    return HAL_OK;
}

static bool __i2c_slave_fifo_transnfer_continue(I2C_HandleTypeDef *hi2c,
                                                bool               is_transmit)
{
    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    if (hi2c->XferCount != 0) {
        uint_fast16_t cnt = vsf_i2c_slave_fifo_transfer(
            i2c_ptr, is_transmit, hi2c->XferCount, hi2c->pBuffPtr);

        hi2c->XferCount -= cnt;
        hi2c->XferSize -= cnt;
        hi2c->pBuffPtr += cnt;
    }

    if (hi2c->XferCount == 0) {
        return false;
    } else {
        return true;
    }
}

static HAL_StatusTypeDef __hal_i2c_master_transfer(
    I2C_HandleTypeDef *hi2c, HAL_I2C_StateTypeDef State, uint16_t DevAddress,
    uint8_t *pData, uint16_t Size, uint32_t Tickstart, uint32_t Timeout)
{
    HAL_StatusTypeDef status = __i2c_transfer_prepare(
        hi2c, State, HAL_I2C_MODE_MASTER, DevAddress, 0, 0, pData, Size,
        I2C_NO_OPTION_FRAME, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    vsf_i2c_cmd_t cmd = __i2c_get_cmd(hi2c) | VSF_I2C_CMD_STOP;
    status =
        __i2c_master_fifo_transnfer_with_timeout(hi2c, cmd, Tickstart, Timeout);

    __i2c_transfer_process(hi2c, I2C_STATE_NONE, HAL_I2C_ERROR_NONE);

    return status;
}

static HAL_StatusTypeDef __i2c_master_transfer_it(
    I2C_HandleTypeDef *hi2c, HAL_I2C_StateTypeDef State,
    HAL_I2C_ModeTypeDef mode, vsf_i2c_cmd_t cmd, vsf_i2c_irq_mask_t irq_mask,
    uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize,
    uint8_t *pData, uint16_t Size, uint32_t XferOptions, uint32_t Tickstart,
    uint32_t Timeout)
{
    HAL_StatusTypeDef status = __i2c_transfer_prepare(
        hi2c, State, mode, DevAddress, MemAddress, MemAddSize, pData, Size,
        XferOptions, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    vsf_i2c_cmd_t current_cmd = 0;
    __i2c_master_fifo_transnfer(hi2c, cmd, &current_cmd);

    VSF_STHAL_UNLOCK(hi2c);
    vsf_i2c_irq_enable(i2c_ptr, irq_mask);

    return HAL_OK;
}

static void __i2c_call_slave_fifo_transnfer(I2C_HandleTypeDef *hi2c,
                                            bool               is_transmit)
{
    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    uint16_t offset = vsf_i2c_slave_fifo_transfer(
        i2c_ptr, is_transmit, hi2c->XferCount, hi2c->pBuffPtr);
    hi2c->XferCount -= offset;
    hi2c->XferSize -= offset;
    hi2c->pBuffPtr += offset;
}

static HAL_StatusTypeDef __i2c_slave_fifo_transnfer(I2C_HandleTypeDef   *hi2c,
                                                    HAL_I2C_StateTypeDef State,
                                                    uint8_t             *pData,
                                                    uint16_t             Size,
                                                    uint32_t Timeout)
{
    uint32_t          Tickstart = HAL_GetTick();
    HAL_StatusTypeDef status =
        __i2c_transfer_prepare(hi2c, State, HAL_I2C_MODE_SLAVE, 0, 0, 0, pData,
                               Size, I2C_NO_OPTION_FRAME, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    while (hi2c->XferCount > 0) {
        __i2c_call_slave_fifo_transnfer(hi2c, true);

        if (__sthal_check_timeout(Tickstart, Timeout)) {
            hi2c->State = HAL_I2C_STATE_READY;
            VSF_STHAL_UNLOCK(hi2c);
            return HAL_TIMEOUT;
        }
    }

    if (__i2c_check_busy_status(hi2c, Tickstart, Timeout) != HAL_OK) {
        return HAL_ERROR;
    }

    __i2c_transfer_process(hi2c, I2C_STATE_NONE, HAL_I2C_ERROR_NONE);

    return HAL_OK;
}

static HAL_StatusTypeDef __i2c_slave_fifo_transfer_it(
    I2C_HandleTypeDef *hi2c, HAL_I2C_StateTypeDef State, uint8_t *pData,
    uint16_t Size, uint32_t XferOptions, vsf_i2c_irq_mask_t irq_mask)
{
    HAL_StatusTypeDef status =
        __i2c_transfer_prepare(hi2c, State, HAL_I2C_MODE_SLAVE, 0, 0, 0, pData,
                               Size, XferOptions, 0, 0);
    if (status != HAL_OK) {
        return status;
    }

    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    __i2c_call_slave_fifo_transnfer(hi2c, (State == HAL_I2C_STATE_BUSY_TX));

    vsf_i2c_irq_enable(i2c_ptr, irq_mask);

    VSF_STHAL_UNLOCK(hi2c);

    return status;
}

static HAL_StatusTypeDef __hal_i2c_master_request(
    I2C_HandleTypeDef *hi2c, HAL_I2C_StateTypeDef State, vsf_i2c_cmd_t cmd,
    uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions)
{

    HAL_StatusTypeDef status =
        __i2c_transfer_prepare(hi2c, State, HAL_I2C_MODE_MASTER, DevAddress, 0,
                               0, pData, Size, XferOptions, 0, 0);
    if (status != HAL_OK) {
        return status;
    }

    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    vsf_i2c_irq_enable(i2c_ptr, VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE |
                                    VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST |
                                    VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK |
                                    VSF_I2C_IRQ_MASK_MASTER_ERR);

    vsf_err_t err = vsf_i2c_master_request(i2c_ptr, hi2c->Devaddress, cmd,
                                           hi2c->XferCount, hi2c->pBuffPtr);
    VSF_STHAL_UNLOCK(hi2c);
    if (err != VSF_ERR_NONE) {
        hi2c->ErrorCode |= HAL_I2C_ERROR_DMA;
        return HAL_ERROR;
    } else {
        return HAL_OK;
    }
}

static HAL_StatusTypeDef __hal_i2c_slave_request(I2C_HandleTypeDef   *hi2c,
                                                 HAL_I2C_StateTypeDef State,
                                                 vsf_i2c_cmd_t        cmd,
                                                 bool     transmit_or_receive,
                                                 uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions)
{
    HAL_StatusTypeDef status =
        __i2c_transfer_prepare(hi2c, State, HAL_I2C_MODE_SLAVE, 0, 0, 0, pData,
                               Size, XferOptions, 0, 0);
    if (status != HAL_OK) {
        return status;
    }

    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    vsf_i2c_irq_enable(i2c_ptr, VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE);

    vsf_err_t err = vsf_i2c_slave_request(i2c_ptr, transmit_or_receive, Size,
                                          hi2c->pBuffPtr);
    VSF_STHAL_UNLOCK(hi2c);
    if (err != VSF_ERR_NONE) {
        hi2c->ErrorCode |= HAL_I2C_ERROR_DMA;
        return HAL_ERROR;
    } else {
        return HAL_OK;
    }
}

HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == NULL) {
        return HAL_ERROR;
    }
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(i2c_ptr));
    VSF_STHAL_ASSERT(IS_I2C_CLOCK_SPEED(hi2c->Init.ClockSpeed));
    VSF_STHAL_ASSERT(IS_I2C_DUTY_CYCLE(hi2c->Init.DutyCycle));
    VSF_STHAL_ASSERT(IS_I2C_OWN_ADDRESS1(hi2c->Init.OwnAddress1));
    VSF_STHAL_ASSERT(IS_I2C_ADDRESSING_MODE(hi2c->Init.AddressingMode));
    VSF_STHAL_ASSERT(IS_I2C_DUAL_ADDRESS(hi2c->Init.DualAddressMode));
    VSF_STHAL_ASSERT(IS_I2C_OWN_ADDRESS2(hi2c->Init.OwnAddress2));
    VSF_STHAL_ASSERT(IS_I2C_GENERAL_CALL(hi2c->Init.GeneralCallMode));
    VSF_STHAL_ASSERT(IS_I2C_NO_STRETCH(hi2c->Init.NoStretchMode));

    if (hi2c->State == HAL_I2C_STATE_RESET) {
        hi2c->Lock = HAL_UNLOCKED;

#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
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
#   else
        HAL_I2C_MspInit(hi2c);
#   endif
    }

    hi2c->State = HAL_I2C_STATE_BUSY;

    // TODO: dual address mode
    // TODO: switch master or slave mode

    vsf_i2c_mode_t mode = VSF_I2C_MODE_MASTER | hi2c->Init.DutyCycle |
                          hi2c->Init.AddressingMode |
                          hi2c->Init.DualAddressMode |
                          hi2c->Init.GeneralCallMode | hi2c->Init.NoStretchMode;

    if (hi2c->Init.ClockSpeed <= 100000) {
        mode |= VSF_I2C_SPEED_STANDARD_MODE;
    } else if (hi2c->Init.ClockSpeed <= 400000) {
        mode |= VSF_I2C_SPEED_FAST_MODE;
    } else if (hi2c->Init.ClockSpeed <= 1000000) {
        mode |= VSF_I2C_SPEED_FAST_MODE_PLUS;
    } else {
        mode |= VSF_I2C_SPEED_HIGH_SPEED_MODE;
    }

    if (hi2c->Init.DualAddressMode != I2C_DUALADDRESS_DISABLE) {
        // TODO: check dual address
    }

    if (hi2c->Init.GeneralCallMode != I2C_GENERALCALL_DISABLE) {
        // TODO:
    }

    if (hi2c->Init.NoStretchMode != I2C_NOSTRETCH_DISABLE) {
        // TODO:
    }

    vsf_i2c_cfg_t cfg = {
        .mode       = mode,
        .clock_hz   = hi2c->Init.ClockSpeed,
        .slave_addr = hi2c->Init.OwnAddress1,
        .isr =
            {
                .handler_fn = __i2c_isr_handler,
                .target_ptr = hi2c,
                .prio       = vsf_arch_prio_0,
            },
    };
    vsf_err_t err = vsf_i2c_init(i2c_ptr, &cfg);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }
    while (fsm_rt_cpl != vsf_i2c_enable(i2c_ptr));

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

    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    hi2c->State = HAL_I2C_STATE_BUSY;

    vsf_i2c_fini(i2c_ptr);

#   if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
    if (hi2c->MspDeInitCallback == NULL) {
        hi2c->MspDeInitCallback = HAL_I2C_MspDeInit;
    }
    hi2c->MspDeInitCallback(hi2c);
#   else
    HAL_I2C_MspDeInit(hi2c);
#   endif

    hi2c->ErrorCode     = HAL_I2C_ERROR_NONE;
    hi2c->State         = HAL_I2C_STATE_RESET;
    hi2c->PreviousState = I2C_STATE_NONE;
    hi2c->Mode          = HAL_I2C_MODE_NONE;

    VSF_STHAL_UNLOCK(hi2c);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c,
                                          uint16_t DevAddress, uint8_t *pData,
                                          uint16_t Size, uint32_t Timeout)
{
    uint32_t Tickstart = HAL_GetTick();
    return __hal_i2c_master_transfer(hi2c, HAL_I2C_STATE_BUSY_TX, DevAddress,
                                     pData, Size, Tickstart, Timeout);
}

HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c,
                                         uint16_t DevAddress, uint8_t *pData,
                                         uint16_t Size, uint32_t Timeout)
{
    uint32_t Tickstart = HAL_GetTick();
    return __hal_i2c_master_transfer(hi2c, HAL_I2C_STATE_BUSY_RX, DevAddress,
                                     pData, Size, Tickstart, Timeout);
}

HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                             uint16_t           DevAddress,
                                             uint8_t *pData, uint16_t Size)
{
    vsf_i2c_cmd_t cmd =
        VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP;
    if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT) {
        cmd |= VSF_I2C_CMD_7_BITS;
    } else {
        cmd |= VSF_I2C_CMD_10_BITS;
    }

    uint32_t Tickstart = HAL_GetTick();
    uint32_t Timeout   = I2C_TIMEOUT_BUSY_FLAG;

    return __i2c_master_transfer_it(
        hi2c, HAL_I2C_STATE_BUSY_TX, HAL_I2C_MODE_MASTER, cmd,
        VSF_I2C_IRQ_MASK_MASTER_TX | VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT,
        DevAddress, 0, 0, pData, Size, I2C_NO_OPTION_FRAME, Tickstart, Timeout);
}

HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c,
                                            uint16_t DevAddress, uint8_t *pData,
                                            uint16_t Size)
{
    vsf_i2c_cmd_t cmd = VSF_I2C_CMD_START | VSF_I2C_CMD_READ | VSF_I2C_CMD_STOP;

    if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT) {
        cmd |= VSF_I2C_CMD_7_BITS;
    } else {
        cmd |= VSF_I2C_CMD_10_BITS;
    }

    uint32_t Tickstart = HAL_GetTick();
    uint32_t Timeout   = I2C_TIMEOUT_BUSY_FLAG;

    return __i2c_master_transfer_it(
        hi2c, HAL_I2C_STATE_BUSY_RX, HAL_I2C_MODE_MASTER, cmd,
        VSF_I2C_IRQ_MASK_MASTER_RX | VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT,
        DevAddress, 0, 0, pData, Size, I2C_NO_OPTION_FRAME, Tickstart, Timeout);
}

HAL_StatusTypeDef HAL_I2C_Mem_Write_IT(I2C_HandleTypeDef *hi2c,
                                       uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData,
                                       uint16_t Size)
{
    vsf_i2c_cmd_t cmd = VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE;

    if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT) {
        cmd |= VSF_I2C_CMD_7_BITS;
    } else {
        cmd |= VSF_I2C_CMD_10_BITS;
    }

    uint32_t Tickstart = HAL_GetTick();
    uint32_t Timeout   = I2C_TIMEOUT_BUSY_FLAG;

    return __i2c_master_transfer_it(
        hi2c, HAL_I2C_STATE_BUSY_TX, HAL_I2C_MODE_MEM, cmd,
        VSF_I2C_IRQ_MASK_MASTER_TX | VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT,
        DevAddress, MemAddress, MemAddSize, pData, Size, I2C_NO_OPTION_FRAME,
        Tickstart, Timeout);
}

HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *hi2c,
                                      uint16_t DevAddress, uint16_t MemAddress,
                                      uint16_t MemAddSize, uint8_t *pData,
                                      uint16_t Size)
{
    vsf_i2c_cmd_t cmd = VSF_I2C_CMD_START | VSF_I2C_CMD_READ;

    if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT) {
        cmd |= VSF_I2C_CMD_7_BITS;
    } else {
        cmd |= VSF_I2C_CMD_10_BITS;
    }

    uint32_t Tickstart = HAL_GetTick();
    uint32_t Timeout   = I2C_TIMEOUT_BUSY_FLAG;

    return __i2c_master_transfer_it(
        hi2c, HAL_I2C_STATE_BUSY_RX, HAL_I2C_MODE_MEM, cmd,
        VSF_I2C_IRQ_MASK_MASTER_RX | VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT,
        DevAddress, MemAddress, MemAddSize, pData, Size, I2C_NO_OPTION_FRAME,
        Tickstart, Timeout);
}

HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                                 uint16_t           DevAddress,
                                                 uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions)
{
    vsf_i2c_capability_t cap = vsf_i2c_capability(hi2c->Instance);
    if (!cap.support_no_stop) {
        return HAL_ERROR;
    }

    vsf_i2c_cmd_t cmd = __i2c_seq_master_transimit_get_cmd(hi2c, XferOptions);

    vsf_i2c_irq_mask_t irq_mask = VSF_I2C_IRQ_MASK_MASTER_TX;
    if (cmd & VSF_I2C_CMD_STOP) {
        irq_mask |= VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT;
    }

    uint32_t Tickstart = HAL_GetTick();
    uint32_t Timeout;
    if (cmd & VSF_I2C_CMD_START) {
        Timeout = I2C_TIMEOUT_BUSY_FLAG;
    } else {
        Timeout = 0;
    }

    return __i2c_master_transfer_it(
        hi2c, HAL_I2C_STATE_BUSY_TX, HAL_I2C_MODE_MASTER, cmd, irq_mask,
        DevAddress, 0, 0, pData, Size, XferOptions, Tickstart, Timeout);
}

HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_IT(I2C_HandleTypeDef *hi2c,
                                                uint16_t           DevAddress,
                                                uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions)
{
    vsf_i2c_capability_t cap = vsf_i2c_capability(hi2c->Instance);
    if (!cap.support_no_stop) {
        return HAL_ERROR;
    }

    vsf_i2c_cmd_t cmd = __i2c_seq_master_receive_get_cmd(hi2c, XferOptions);
    vsf_i2c_irq_mask_t irq_mask = VSF_I2C_IRQ_MASK_MASTER_RX;
    if (cmd & VSF_I2C_CMD_STOP) {
        irq_mask |= VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT;
    }

    uint32_t Tickstart = HAL_GetTick();
    uint32_t Timeout;
    if (cmd & VSF_I2C_CMD_START) {
        Timeout = I2C_TIMEOUT_BUSY_FLAG;
    } else {
        Timeout = 0;
    }

    return __i2c_master_transfer_it(
        hi2c, HAL_I2C_STATE_BUSY_RX, HAL_I2C_MODE_MASTER, cmd, irq_mask,
        DevAddress, 0, 0, pData, Size, XferOptions, Tickstart, Timeout);
}

HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c,
                                         uint8_t *pData, uint16_t Size,
                                         uint32_t Timeout)
{
    return __i2c_slave_fifo_transnfer(hi2c, HAL_I2C_STATE_BUSY_TX, pData, Size,
                                      Timeout);
}

HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData,
                                        uint16_t Size, uint32_t Timeout)
{
    return __i2c_slave_fifo_transnfer(hi2c, HAL_I2C_STATE_BUSY_RX, pData, Size,
                                      Timeout);
}

HAL_StatusTypeDef HAL_I2C_Slave_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                            uint8_t *pData, uint16_t Size)
{
    return __i2c_slave_fifo_transfer_it(hi2c, HAL_I2C_STATE_BUSY_TX, pData,
                                        Size, I2C_NO_OPTION_FRAME,
                                        VSF_I2C_IRQ_MASK_SLAVE_TX);
}

HAL_StatusTypeDef HAL_I2C_Slave_Receive_IT(I2C_HandleTypeDef *hi2c,
                                           uint8_t *pData, uint16_t Size)
{
    return __i2c_slave_fifo_transfer_it(hi2c, HAL_I2C_STATE_BUSY_RX, pData,
                                        Size, I2C_NO_OPTION_FRAME,
                                        VSF_I2C_IRQ_MASK_SLAVE_RX);
}

HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c,
                                                uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions)
{
#   if defined(VSF_I2C_IRQ_MASK_ENABLE_LISTEN) &&                              \
       defined(VSF_I2C_IRQ_MASK_DISABLE_LISTEN)
    if (((uint32_t)hi2c->State & (uint32_t)HAL_I2C_STATE_LISTEN) ==
        (uint32_t)HAL_I2C_STATE_LISTEN) {
        hi2c->State = HAL_I2C_STATE_READY;
    }

    return __i2c_slave_fifo_transfer_it(hi2c, HAL_I2C_STATE_BUSY_TX_LISTEN,
                                        pData, Size, XferOptions,
                                        VSF_I2C_IRQ_MASK_SLAVE_TX);
#   else
    return HAL_ERROR;
#   endif
}

HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_IT(I2C_HandleTypeDef *hi2c,
                                               uint8_t *pData, uint16_t Size,
                                               uint32_t XferOptions)
{
#   if defined(VSF_I2C_IRQ_MASK_ENABLE_LISTEN) &&                              \
       defined(VSF_I2C_IRQ_MASK_DISABLE_LISTEN)
    if (((uint32_t)hi2c->State & (uint32_t)HAL_I2C_STATE_LISTEN) ==
        (uint32_t)HAL_I2C_STATE_LISTEN) {
        hi2c->State = HAL_I2C_STATE_READY;
    }

    return __i2c_slave_fifo_transfer_it(hi2c, HAL_I2C_STATE_BUSY_RX_LISTEN,
                                        pData, Size, XferOptions,
                                        VSF_I2C_IRQ_MASK_SLAVE_RX);
#   else
    return HAL_ERROR;
#   endif
}

HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                              uint16_t           DevAddress,
                                              uint8_t *pData, uint16_t Size)
{
    return __hal_i2c_master_request(
        hi2c, HAL_I2C_STATE_BUSY_TX,
        VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP, DevAddress,
        pData, Size, I2C_NO_OPTION_FRAME);
}

HAL_StatusTypeDef HAL_I2C_Master_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                             uint16_t           DevAddress,
                                             uint8_t *pData, uint16_t Size)
{
    return __hal_i2c_master_request(
        hi2c, HAL_I2C_STATE_BUSY_RX,
        VSF_I2C_CMD_READ | VSF_I2C_CMD_START | VSF_I2C_CMD_STOP, DevAddress,
        pData, Size, I2C_NO_OPTION_FRAME);
}

HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                                  uint16_t           DevAddress,
                                                  uint8_t *pData, uint16_t Size,
                                                  uint32_t XferOptions)
{
    vsf_i2c_capability_t cap = vsf_i2c_capability(hi2c->Instance);
    if (!cap.support_no_stop) {
        return HAL_ERROR;
    }

    vsf_i2c_cmd_t cmd = __i2c_seq_master_transimit_get_cmd(hi2c, XferOptions);

    return __hal_i2c_master_request(hi2c, HAL_I2C_STATE_BUSY_TX, cmd,
                                    DevAddress, pData, Size, XferOptions);
}

HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                                 uint16_t           DevAddress,
                                                 uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions)
{
    vsf_i2c_capability_t cap = vsf_i2c_capability(hi2c->Instance);
    if (!cap.support_no_stop) {
        return HAL_ERROR;
    }

    vsf_i2c_cmd_t cmd = __i2c_seq_master_receive_get_cmd(hi2c, XferOptions);

    return __hal_i2c_master_request(hi2c, HAL_I2C_STATE_BUSY_RX, cmd,
                                    DevAddress, pData, Size, XferOptions);
}

HAL_StatusTypeDef HAL_I2C_Slave_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                             uint8_t *pData, uint16_t Size)
{
    return __hal_i2c_slave_request(hi2c, HAL_I2C_STATE_BUSY_TX,
                                   VSF_I2C_CMD_WRITE, true, pData, Size,
                                   I2C_NO_OPTION_FRAME);
}

HAL_StatusTypeDef HAL_I2C_Slave_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                            uint8_t *pData, uint16_t Size)
{
    return __hal_i2c_slave_request(hi2c, HAL_I2C_STATE_BUSY_RX,
                                   VSF_I2C_CMD_READ, false, pData, Size,
                                   I2C_NO_OPTION_FRAME);
}

HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c,
                                                 uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions)
{
#   if defined(VSF_I2C_IRQ_MASK_ENABLE_LISTEN) &&                              \
       defined(VSF_I2C_IRQ_MASK_DISABLE_LISTEN)
    if (((uint32_t)hi2c->State & (uint32_t)HAL_I2C_STATE_LISTEN) ==
        (uint32_t)HAL_I2C_STATE_LISTEN) {
        hi2c->State = HAL_I2C_STATE_READY;
    }

    return __hal_i2c_slave_request(hi2c, HAL_I2C_STATE_BUSY_TX_LISTEN,
                                   VSF_I2C_CMD_WRITE, true, pData, Size,
                                   XferOptions);
#   else
    return HAL_ERROR;
#   endif
}

HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c,
                                                uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions)
{
#   if defined(VSF_I2C_IRQ_MASK_ENABLE_LISTEN) &&                              \
       defined(VSF_I2C_IRQ_MASK_DISABLE_LISTEN)
    if (((uint32_t)hi2c->State & (uint32_t)HAL_I2C_STATE_LISTEN) ==
        (uint32_t)HAL_I2C_STATE_LISTEN) {
        hi2c->State = HAL_I2C_STATE_READY;
    }

    return __hal_i2c_slave_request(hi2c, HAL_I2C_STATE_BUSY_RX_LISTEN,
                                   VSF_I2C_CMD_READ, false, pData, Size,
                                   XferOptions);
#   else
    return HAL_ERROR;
#   endif
}

HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c,
                                    uint16_t DevAddress, uint16_t MemAddress,
                                    uint16_t MemAddSize, uint8_t *pData,
                                    uint16_t Size, uint32_t Timeout)
{
    vsf_i2c_cmd_t     cmd;
    uint32_t          Tickstart = HAL_GetTick();
    HAL_StatusTypeDef status;

    status = __i2c_transfer_prepare(
        hi2c, HAL_I2C_STATE_BUSY_TX, HAL_I2C_MODE_MEM, DevAddress, MemAddress,
        MemAddSize, pData, Size, I2C_NO_OPTION_FRAME, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    cmd = VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS | VSF_I2C_CMD_STOP;
    status =
        __i2c_master_fifo_transnfer_with_timeout(hi2c, cmd, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    __i2c_transfer_process(hi2c, I2C_STATE_NONE, HAL_I2C_ERROR_NONE);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                   uint16_t MemAddress, uint16_t MemAddSize,
                                   uint8_t *pData, uint16_t Size,
                                   uint32_t Timeout)
{
    vsf_i2c_cmd_t     cmd;
    HAL_StatusTypeDef status;
    uint32_t          Tickstart = HAL_GetTick();

    status = __i2c_transfer_prepare(
        hi2c, HAL_I2C_STATE_BUSY_RX, HAL_I2C_MODE_MEM, DevAddress, MemAddress,
        MemAddSize, pData, Size, I2C_NO_OPTION_FRAME, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    cmd = VSF_I2C_CMD_RESTART | VSF_I2C_CMD_READ | VSF_I2C_CMD_7_BITS |
          VSF_I2C_CMD_STOP;
    status =
        __i2c_master_fifo_transnfer_with_timeout(hi2c, cmd, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    if (__i2c_check_busy_status(hi2c, Tickstart, Timeout) != HAL_OK) {
        return HAL_ERROR;
    }

    __i2c_transfer_process(hi2c, I2C_STATE_NONE, HAL_I2C_ERROR_NONE);

    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c,
                                        uint16_t           DevAddress,
                                        uint16_t           MemAddress,
                                        uint16_t MemAddSize, uint8_t *pData,
                                        uint16_t Size)
{
    vsf_i2c_cmd_t     cmd;
    HAL_StatusTypeDef status;
    uint32_t          Timeout   = I2C_TIMEOUT_FLAG;
    uint32_t          Tickstart = HAL_GetTick();

    status = __i2c_transfer_prepare(
        hi2c, HAL_I2C_STATE_BUSY_TX, HAL_I2C_MODE_MEM, DevAddress, MemAddress,
        MemAddSize, pData, Size, I2C_NO_OPTION_FRAME, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    hi2c->XferCount = 0;
    cmd = VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS;
    status =
        __i2c_master_fifo_transnfer_with_timeout(hi2c, cmd, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    hi2c->XferCount = Size;

    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    vsf_i2c_irq_enable(i2c_ptr, VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE |
                                    VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST |
                                    VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK |
                                    VSF_I2C_IRQ_MASK_MASTER_ERR);

    cmd = VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS | VSF_I2C_CMD_STOP;

    vsf_err_t err = vsf_i2c_master_request(i2c_ptr, hi2c->Devaddress, cmd, Size,
                                           hi2c->pBuffPtr);
    VSF_STHAL_UNLOCK(hi2c);
    if (err != VSF_ERR_NONE) {
        hi2c->ErrorCode |= HAL_I2C_ERROR_DMA;
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c,
                                       uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData,
                                       uint16_t Size)
{
    vsf_i2c_cmd_t     cmd;
    HAL_StatusTypeDef status;
    uint32_t          Timeout   = I2C_TIMEOUT_FLAG;
    uint32_t          Tickstart = HAL_GetTick();

    status = __i2c_transfer_prepare(
        hi2c, HAL_I2C_STATE_BUSY_RX, HAL_I2C_MODE_MEM, DevAddress, MemAddress,
        MemAddSize, pData, Size, I2C_NO_OPTION_FRAME, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    hi2c->XferCount = 0;
    cmd = VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_7_BITS;
    status =
        __i2c_master_fifo_transnfer_with_timeout(hi2c, cmd, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    hi2c->XferCount = Size;

    status = __i2c_check_busy_status(hi2c, Tickstart, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    vsf_i2c_irq_enable(i2c_ptr, VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE |
                                    VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST |
                                    VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK |
                                    VSF_I2C_IRQ_MASK_MASTER_ERR);

    cmd = VSF_I2C_CMD_RESTART | VSF_I2C_CMD_READ | VSF_I2C_CMD_STOP;

    vsf_err_t err = vsf_i2c_master_request(i2c_ptr, hi2c->Devaddress, cmd, hi2c->XferCount, hi2c->pBuffPtr);
    VSF_STHAL_UNLOCK(hi2c);
    if (err != VSF_ERR_NONE) {
        hi2c->ErrorCode |= HAL_I2C_ERROR_DMA;
        return HAL_ERROR;
    } else {
        return HAL_OK;
    }
}

HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c,
                                        uint16_t DevAddress, uint32_t Trials,
                                        uint32_t Timeout)
{
    // 检查输入参数有效性
    if (hi2c == NULL) {
        return HAL_ERROR;
    }

    uint32_t Tickstart = HAL_GetTick();

    uint32_t      i = 0;
    vsf_i2c_cmd_t cmd =
        VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP;
    vsf_i2c_cmd_t current_cmd = 0;

    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    uint_fast16_t offset = 0;
    while (i < Trials) {
        while (cmd != current_cmd) {
            vsf_i2c_master_fifo_transfer(i2c_ptr, hi2c->Devaddress, cmd, 0,
                                         NULL, &current_cmd, &offset);

            if (__i2c_check_timeout(hi2c, Tickstart, Timeout)) {
                return HAL_ERROR;
            } else if (__i2c_check_timeout(hi2c, Tickstart,
                                           I2C_TIMEOUT_BUSY_FLAG)) {
                i++;
            }
        }
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_EnableListen_IT(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == NULL) {
        return HAL_ERROR;
    }

#   if defined(VSF_I2C_IRQ_MASK_ENABLE_LISTEN)
    if (hi2c->State == HAL_I2C_STATE_READY) {
        hi2c->State = HAL_I2C_STATE_LISTEN;

        VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
        vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
        VSF_STHAL_ASSERT(i2c_ptr != NULL);

        vsf_i2c_irq_enable(i2c_ptr, VSF_I2C_IRQ_MASK_ENABLE_LISTEN);
        return HAL_OK;
    }
    return HAL_OK;
#   else
    return HAL_ERROR;
#   endif
}

HAL_StatusTypeDef HAL_I2C_DisableListen_IT(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == NULL) {
        return HAL_ERROR;
    }

#   if defined(VSF_I2C_IRQ_MASK_DISABLE_LISTEN)
    if (hi2c->State == HAL_I2C_STATE_LISTEN) {
        hi2c->PreviousState = (hi2c->State & I2C_STATE_MSK) | hi2c->Mode;
        hi2c->State         = HAL_I2C_STATE_READY;
        hi2c->Mode          = HAL_I2C_MODE_NONE;

        VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
        vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
        VSF_STHAL_ASSERT(i2c_ptr != NULL);

        vsf_i2c_irq_disable(i2c_ptr, VSF_I2C_IRQ_MASK_DISABLE_LISTEN);
        return HAL_ERROR;
    }
    return HAL_OK;
#   else
    return HAL_ERROR;
#   endif
}

HAL_StatusTypeDef HAL_I2C_Master_Abort_IT(I2C_HandleTypeDef *hi2c,
                                          uint16_t           DevAddress)
{
    if (hi2c == NULL) {
        return HAL_ERROR;
    }

#   ifdef VSF_I2C_CTRL_MASTER_ABORT
    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    vsf_err_t err =
        vsf_i2c_control(i2c_ptr, VSF_I2C_CTRL_MASTER_ABORT, (void *)DevAddress);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }
    return HAL_OK;
#   else
    return HAL_ERROR;
#   endif
}

void HAL_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c)
{
    VSF_STHAL_ASSERT(hi2c != NULL);
    VSF_UNUSED_PARAM(hi2c);
}

void HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c)
{
    VSF_STHAL_ASSERT(hi2c != NULL);
    VSF_UNUSED_PARAM(hi2c);
}

HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c)
{
    VSF_STHAL_ASSERT(hi2c != NULL);

    if (hi2c->State != HAL_I2C_STATE_READY) {
        VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
        vsf_i2c_t       *i2c    = (vsf_i2c_t *)hi2c->Instance;
        vsf_i2c_status_t status = vsf_i2c_status(i2c);
        if (!status.is_busy) {
            hi2c->State = HAL_I2C_STATE_READY;
        }
    }

    return hi2c->State;
}

HAL_I2C_ModeTypeDef HAL_I2C_GetMode(I2C_HandleTypeDef *hi2c)
{
    VSF_STHAL_ASSERT(hi2c != NULL);
    return hi2c->Mode;
}

uint32_t HAL_I2C_GetError(I2C_HandleTypeDef *hi2c)
{
    VSF_STHAL_ASSERT(hi2c != NULL);
    return hi2c->ErrorCode;
}

HAL_StatusTypeDef HAL_I2CEx_ConfigAnalogFilter(I2C_HandleTypeDef *hi2c,
                                               uint32_t           AnalogFilter)
{
    if (hi2c == NULL) {
        return HAL_ERROR;
    }

#   ifdef VSF_I2C_CTRL_ANALOG_FILTER
    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    vsf_err_t err = vsf_i2c_control(i2c_ptr, VSF_I2C_CTRL_ANALOG_FILTER,
                                    (void *)AnalogFilter);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }
#   endif

    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2CEx_ConfigDigitalFilter(I2C_HandleTypeDef *hi2c,
                                                uint32_t DigitalFilter)
{
    if (hi2c == NULL) {
        return HAL_ERROR;
    }

#   ifdef VSF_I2C_CTRL_DIGITAL_FILTER
    VSF_STHAL_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    vsf_i2c_t *i2c_ptr = (vsf_i2c_t *)hi2c->Instance;
    VSF_STHAL_ASSERT(i2c_ptr != NULL);

    vsf_err_t err = vsf_i2c_control(i2c_ptr, VSF_I2C_CTRL_DIGITAL_FILTER,
                                    (void *)DigitalFilter);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }
#   endif

    return HAL_OK;
}

#endif
