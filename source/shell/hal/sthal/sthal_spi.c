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

#if defined(HAL_SPI_MODULE_ENABLED) && VSF_HAL_USE_SPI == ENABLED

#include "sthal_internal.h"

/*============================ MACROS ========================================*/

#   ifndef SPI_DEFAULT_TIMEOUT
#      define SPI_DEFAULT_TIMEOUT             100U
#   endif

#   ifndef SPI_BSY_FLAG_WORKAROUND_TIMEOUT
#      define SPI_BSY_FLAG_WORKAROUND_TIMEOUT 1000U
#   endif

#   ifndef VSF_STHAL_CFG_SPI_DEFAULT_CLOCK
#      define VSF_STHAL_CFG_SPI_DEFAULT_CLOCK (100 * 1000 * 1000)
#   endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static bool __spi_fifo_transfer_continue(SPI_HandleTypeDef *hspi);

/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(HAL_SPI_MspInit)
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

VSF_CAL_WEAK(HAL_SPI_MspDeInit)
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

VSF_CAL_WEAK(HAL_SPI_TxCpltCallback)
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

VSF_CAL_WEAK(HAL_SPI_RxCpltCallback)
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

VSF_CAL_WEAK(HAL_SPI_TxRxCpltCallback)
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

VSF_CAL_WEAK(HAL_SPI_TxHalfCpltCallback)
void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

VSF_CAL_WEAK(HAL_SPI_RxHalfCpltCallback)
void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

VSF_CAL_WEAK(HAL_SPI_TxRxHalfCpltCallback)
void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

VSF_CAL_WEAK(HAL_SPI_ErrorCallback)
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

VSF_CAL_WEAK(HAL_SPI_AbortCpltCallback)
void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi)
{
    VSF_UNUSED_PARAM(hspi);
}

#   if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
HAL_StatusTypeDef HAL_SPI_RegisterCallback(SPI_HandleTypeDef        *hspi,
                                           HAL_SPI_CallbackIDTypeDef CallbackID,
                                           pSPI_CallbackTypeDef      pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;
    if (pCallback == NULL) {
        hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
        return HAL_ERROR;
    }

    VSF_STHAL_LOCK(hspi);
    if (HAL_SPI_STATE_READY == hspi->State) {
        switch (CallbackID) {
        case HAL_SPI_TX_COMPLETE_CB_ID:
            hspi->TxCpltCallback = pCallback;
            break;
        case HAL_SPI_RX_COMPLETE_CB_ID:
            hspi->RxCpltCallback = pCallback;
            break;
        case HAL_SPI_TX_RX_COMPLETE_CB_ID:
            hspi->TxRxCpltCallback = pCallback;
            break;
        case HAL_SPI_TX_HALF_COMPLETE_CB_ID:
            hspi->TxHalfCpltCallback = pCallback;
            break;
        case HAL_SPI_RX_HALF_COMPLETE_CB_ID:
            hspi->RxHalfCpltCallback = pCallback;
            break;
        case HAL_SPI_TX_RX_HALF_COMPLETE_CB_ID:
            hspi->TxRxHalfCpltCallback = pCallback;
            break;
        case HAL_SPI_ERROR_CB_ID:
            hspi->ErrorCallback = pCallback;
            break;
        case HAL_SPI_ABORT_CB_ID:
            hspi->AbortCpltCallback = pCallback;
            break;
        case HAL_SPI_MSPINIT_CB_ID:
            hspi->MspInitCallback = pCallback;
            break;
        case HAL_SPI_MSPDEINIT_CB_ID:
            hspi->MspDeInitCallback = pCallback;
            break;
        default:
            hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (HAL_SPI_STATE_RESET == hspi->State) {
        switch (CallbackID) {
        case HAL_SPI_MSPINIT_CB_ID:
            hspi->MspInitCallback = pCallback;
            break;
        case HAL_SPI_MSPDEINIT_CB_ID:
            hspi->MspDeInitCallback = pCallback;
            break;
        default:
            hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hspi);
    return status;
}

HAL_StatusTypeDef HAL_SPI_UnRegisterCallback(
    SPI_HandleTypeDef *hspi, HAL_SPI_CallbackIDTypeDef CallbackID)
{
    HAL_StatusTypeDef status = HAL_OK;
    VSF_STHAL_LOCK(hspi);
    if (HAL_SPI_STATE_READY == hspi->State) {
        switch (CallbackID) {
        case HAL_SPI_TX_COMPLETE_CB_ID:
            hspi->TxCpltCallback = HAL_SPI_TxCpltCallback;
            break;
        case HAL_SPI_RX_COMPLETE_CB_ID:
            hspi->RxCpltCallback = HAL_SPI_RxCpltCallback;
            break;
        case HAL_SPI_TX_RX_COMPLETE_CB_ID:
            hspi->TxRxCpltCallback = HAL_SPI_TxRxCpltCallback;
            break;
        case HAL_SPI_TX_HALF_COMPLETE_CB_ID:
            hspi->TxHalfCpltCallback = HAL_SPI_TxHalfCpltCallback;
            break;
        case HAL_SPI_RX_HALF_COMPLETE_CB_ID:
            hspi->RxHalfCpltCallback = HAL_SPI_RxHalfCpltCallback;
            break;
        case HAL_SPI_TX_RX_HALF_COMPLETE_CB_ID:
            hspi->TxRxHalfCpltCallback = HAL_SPI_TxRxHalfCpltCallback;
            break;
        case HAL_SPI_ERROR_CB_ID:
            hspi->ErrorCallback = HAL_SPI_ErrorCallback;
            break;
        case HAL_SPI_ABORT_CB_ID:
            hspi->AbortCpltCallback = HAL_SPI_AbortCpltCallback;
            break;
        case HAL_SPI_MSPINIT_CB_ID:
            hspi->MspInitCallback = HAL_SPI_MspInit;
            break;
        case HAL_SPI_MSPDEINIT_CB_ID:
            hspi->MspDeInitCallback = HAL_SPI_MspDeInit;
            break;
        default:
            hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else if (HAL_SPI_STATE_RESET == hspi->State) {
        switch (CallbackID) {
        case HAL_SPI_MSPINIT_CB_ID:
            hspi->MspInitCallback = HAL_SPI_MspInit;
            break;
        case HAL_SPI_MSPDEINIT_CB_ID:
            hspi->MspDeInitCallback = HAL_SPI_MspDeInit;
            break;
        default:
            hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
            status = HAL_ERROR;
            break;
        }
    } else {
        hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hspi);
    return status;
}
#   endif

bool __spi_is_idle(vsf_spi_t *spi, uint32_t Timeout, uint32_t Tickstart)
{
    while (1) {
        vsf_spi_status_t status = vsf_spi_status(spi);
        if (!status.is_busy) {
            break;
        }
        if (__sthal_check_timeout(Tickstart, Timeout)) {
            return HAL_TIMEOUT;
        }
    }

    return HAL_OK;
}

static void __spi_isr_handler(void *target_ptr, vsf_spi_t *spi_ptr,
                              vsf_spi_irq_mask_t irq_mask)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)target_ptr;
    VSF_STHAL_ASSERT(hspi != NULL);

    if (irq_mask & VSF_SPI_IRQ_MASK_TX_CPL) {
        if (hspi->State == HAL_SPI_STATE_BUSY_TX) {
            vsf_spi_irq_disable(spi_ptr, VSF_SPI_IRQ_MASK_TX_CPL);
            //hspi->State = HAL_SPI_STATE_READY;

#   if USE_HAL_SPI_REGISTER_CALLBACKS == 1U
            hspi->TxCpltCallback(hspi);
#   else
            HAL_SPI_TxCpltCallback(hspi);
#   endif
        }
    }

    if (irq_mask & VSF_SPI_IRQ_MASK_CPL) {
        if (hspi->State == HAL_SPI_STATE_BUSY_TX_RX) {
            hspi->State = HAL_SPI_STATE_READY;
#   if USE_HAL_SPI_REGISTER_CALLBACKS == 1U
            hspi->TxRxCpltCallback(hspi);
#   else
            HAL_SPI_TxRxCpltCallback(hspi);
#   endif
        } else if (hspi->State == HAL_SPI_STATE_BUSY_RX) {
            hspi->State = HAL_SPI_STATE_READY;

#   if USE_HAL_SPI_REGISTER_CALLBACKS == 1U
            hspi->RxCpltCallback(hspi);
#   else
            HAL_SPI_RxCpltCallback(hspi);
#   endif
        }
    }

    if (irq_mask & VSF_SPI_IRQ_MASK_TX) {
        if (!__spi_fifo_transfer_continue(hspi)) {
            vsf_spi_irq_disable(spi_ptr, VSF_SPI_IRQ_MASK_TX);
            if (hspi->State == HAL_SPI_STATE_BUSY_TX) {
#   if USE_HAL_SPI_REGISTER_CALLBACKS == 1U
                hspi->TxCpltCallback(hspi);
#   else
                HAL_SPI_TxCpltCallback(hspi);
#   endif
            }
        }
    }

    if (irq_mask & VSF_SPI_IRQ_MASK_RX) {
        if (!__spi_fifo_transfer_continue(hspi)) {
            vsf_spi_irq_disable(spi_ptr,
                                VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX);

            if (hspi->State == HAL_SPI_STATE_BUSY_RX) {
                hspi->State = HAL_SPI_STATE_READY;
#   if USE_HAL_SPI_REGISTER_CALLBACKS == 1U
                hspi->RxCpltCallback(hspi);
#   else
                HAL_SPI_RxCpltCallback(hspi);
#   endif
            } else if (hspi->State == HAL_SPI_STATE_BUSY_TX_RX) {
                hspi->State = HAL_SPI_STATE_READY;
#   if USE_HAL_SPI_REGISTER_CALLBACKS == 1U
                hspi->TxRxCpltCallback(hspi);
#   else
                HAL_SPI_TxRxCpltCallback(hspi);
#   endif
            }
        }
    }
}

HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }
    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

    VSF_STHAL_ASSERT(IS_SPI_ALL_INSTANCE(hspi->Instance));
    VSF_STHAL_ASSERT(IS_SPI_MODE(hspi->Init.Mode));
    VSF_STHAL_ASSERT(IS_SPI_DIRECTION(hspi->Init.Direction));
    VSF_STHAL_ASSERT(IS_SPI_DATASIZE(hspi->Init.DataSize));
    VSF_STHAL_ASSERT(IS_SPI_NSS(hspi->Init.NSS));
    VSF_STHAL_ASSERT(IS_SPI_BAUDRATE_PRESCALER(hspi->Init.BaudRatePrescaler));
    VSF_STHAL_ASSERT(IS_SPI_FIRST_BIT(hspi->Init.FirstBit));
    VSF_STHAL_ASSERT(IS_SPI_TIMODE(hspi->Init.TIMode));

    if (hspi->Init.TIMode == SPI_TIMODE_DISABLE) {
        //VSF_STHAL_ASSERT(IS_SPI_CPOL(hspi->Init.CLKPolarity));
        //VSF_STHAL_ASSERT(IS_SPI_CPHA(hspi->Init.CLKPhase));
    }
#   if (USE_SPI_CRC != 0U)
    VSF_STHAL_ASSERT(IS_SPI_CRC_CALCULATION(hspi->Init.CRCCalculation));
    if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE) {
        VSF_STHAL_ASSERT(IS_SPI_CRC_POLYNOMIAL(hspi->Init.CRCPolynomial));
    }
#   else
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
#   endif

    if (hspi->State == HAL_SPI_STATE_RESET) {
        hspi->Lock = HAL_UNLOCKED;
#   if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
        hspi->TxCpltCallback       = HAL_SPI_TxCpltCallback;
        hspi->RxCpltCallback       = HAL_SPI_RxCpltCallback;
        hspi->TxRxCpltCallback     = HAL_SPI_TxRxCpltCallback;
        hspi->TxHalfCpltCallback   = HAL_SPI_TxHalfCpltCallback;
        hspi->RxHalfCpltCallback   = HAL_SPI_RxHalfCpltCallback;
        hspi->TxRxHalfCpltCallback = HAL_SPI_TxRxHalfCpltCallback;
        hspi->ErrorCallback        = HAL_SPI_ErrorCallback;
        hspi->AbortCpltCallback    = HAL_SPI_AbortCpltCallback;
        if (hspi->MspInitCallback == NULL) {
            hspi->MspInitCallback = HAL_SPI_MspInit;
        }
        hspi->MspInitCallback(hspi);
#   else
        HAL_SPI_MspInit(hspi);
#   endif
    }
    hspi->State = HAL_SPI_STATE_BUSY;

    uint32_t div =
        (hspi->Init.BaudRatePrescaler == 0) ? 1 : hspi->Init.BaudRatePrescaler;

    vsf_spi_cfg_t cfg = {
        .mode = hspi->Init.Mode | hspi->Init.Direction | hspi->Init.DataSize |
                hspi->Init.NSS | hspi->Init.FirstBit | hspi->Init.TIMode,
        .clock_hz = VSF_STHAL_CFG_SPI_DEFAULT_CLOCK / div,
        .isr =
            {
                .handler_fn = __spi_isr_handler,
                .target_ptr = hspi,
                .prio       = vsf_arch_prio_0,
            },
    };
    vsf_err_t err = vsf_spi_init(spi, &cfg);
    if (err != VSF_ERR_NONE) {
        return HAL_ERROR;
    }
    while (fsm_rt_cpl != vsf_spi_enable(spi));

    hspi->ErrorCode = HAL_SPI_ERROR_NONE;
    hspi->State     = HAL_SPI_STATE_READY;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    VSF_STHAL_ASSERT(IS_SPI_ALL_INSTANCE(hspi->Instance));

    hspi->State = HAL_SPI_STATE_BUSY;

    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);
    vsf_spi_fini(spi);

#   if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
    if (hspi->MspDeInitCallback == NULL) {
        hspi->MspDeInitCallback = HAL_SPI_MspDeInit;
    }
    hspi->MspDeInitCallback(hspi);
#   else
    HAL_SPI_MspDeInit(hspi);
#   endif

    hspi->ErrorCode = HAL_SPI_ERROR_NONE;
    hspi->State     = HAL_SPI_STATE_RESET;

    VSF_STHAL_UNLOCK(hspi);

    return HAL_OK;
}

static HAL_StatusTypeDef __spi_transfer_prepare(
    SPI_HandleTypeDef *hspi, HAL_SPI_StateTypeDef State, uint8_t *pTxBuffPtr,
    uint16_t TxXferSize, uint8_t *pRxBuffPtr, uint16_t RxXferSize)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    VSF_STHAL_ASSERT(IS_SPI_DIRECTION_2LINES_OR_1LINE(hspi->Init.Direction));

    VSF_STHAL_LOCK(hspi);

    if (hspi->State != HAL_SPI_STATE_READY) {
        hspi->State = HAL_SPI_STATE_READY;
        VSF_STHAL_UNLOCK(hspi);
        return HAL_BUSY;
    }

    hspi->State     = State;
    hspi->ErrorCode = HAL_SPI_ERROR_NONE;

    hspi->pTxBuffPtr  = pTxBuffPtr;
    hspi->TxXferSize  = TxXferSize;
    hspi->TxXferCount = TxXferSize;

    hspi->pRxBuffPtr  = pRxBuffPtr;
    hspi->RxXferSize  = RxXferSize;
    hspi->RxXferCount = RxXferSize;

    hspi->TxISR = NULL;
    hspi->RxISR = NULL;

    return HAL_OK;
}

static bool __spi_fifo_transfer_continue(SPI_HandleTypeDef *hspi)
{
    vsf_spi_t    *spi        = (vsf_spi_t *)hspi->Instance;
    uint_fast32_t out_offset = hspi->TxXferSize - hspi->TxXferCount;
    uint_fast32_t in_offset  = hspi->RxXferSize - hspi->RxXferCount;
    uint_fast32_t count      = vsf_max(hspi->TxXferSize, hspi->RxXferSize);

    vsf_spi_fifo_transfer(spi, (void *)hspi->pTxBuffPtr, &out_offset,
                          (void *)hspi->pRxBuffPtr, &in_offset, count);

    if (hspi->pTxBuffPtr != NULL) {
        hspi->TxXferCount = hspi->TxXferSize - out_offset;
    }
    if (hspi->pRxBuffPtr != NULL) {
        hspi->RxXferCount = hspi->RxXferSize - in_offset;
    }

    return (hspi->TxXferCount > 0U) || (hspi->RxXferCount > 0U);
}

static HAL_StatusTypeDef __spi_transfer_with_timeout(SPI_HandleTypeDef *hspi,
                                                     uint32_t           Timeout)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    VSF_STHAL_ASSERT(IS_SPI_ALL_INSTANCE(hspi->Instance));

    uint32_t tickstart = HAL_GetTick();

    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;

    while ((hspi->TxXferCount > 0U) || (hspi->RxXferCount > 0U)) {
        __spi_fifo_transfer_continue(hspi);

        if (__sthal_check_timeout(tickstart, Timeout)) {
            hspi->State = HAL_SPI_STATE_READY;
            VSF_STHAL_UNLOCK(hspi);
            return HAL_TIMEOUT;
        }
    }

    HAL_StatusTypeDef status = __spi_is_idle(spi, Timeout, tickstart);
    if (status != HAL_OK) {
        hspi->ErrorCode = HAL_SPI_ERROR_FLAG;
    }
    hspi->State = HAL_SPI_STATE_READY;
    VSF_STHAL_UNLOCK(hspi);

    return status;
}

HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                   uint16_t Size, uint32_t Timeout)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    if ((pData == NULL) || (Size == 0U)) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_ERROR;
    }

    if (__spi_transfer_prepare(hspi, HAL_SPI_STATE_BUSY_TX, pData, Size, NULL,
                               0) != HAL_OK) {
        hspi->State = HAL_SPI_STATE_READY;
        VSF_STHAL_UNLOCK(hspi);
        return HAL_ERROR;
    }

    return __spi_transfer_with_timeout(hspi, Timeout);
}

HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                  uint16_t Size, uint32_t Timeout)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    if ((pData == NULL) || (Size == 0U)) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_ERROR;
    }

    if (__spi_transfer_prepare(hspi, HAL_SPI_STATE_BUSY_RX, NULL, 0, pData,
                               Size) != HAL_OK) {
        hspi->State = HAL_SPI_STATE_READY;
        VSF_STHAL_UNLOCK(hspi);
        return HAL_ERROR;
    }

    return __spi_transfer_with_timeout(hspi, Timeout);
}

HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi,
                                          uint8_t *pTxData, uint8_t *pRxData,
                                          uint16_t Size, uint32_t Timeout)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    VSF_STHAL_ASSERT(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

    if (!((hspi->State == HAL_SPI_STATE_READY) ||
          ((hspi->Init.Mode == SPI_MODE_MASTER) &&
           (hspi->Init.Direction == SPI_DIRECTION_2LINES) &&
           (hspi->State == HAL_SPI_STATE_BUSY_RX)))) {
        hspi->State = HAL_SPI_STATE_READY;
        VSF_STHAL_UNLOCK(hspi);
        return HAL_BUSY;
    }

    if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U)) {
        hspi->State = HAL_SPI_STATE_READY;
        VSF_STHAL_UNLOCK(hspi);
        return HAL_ERROR;
    }

    if (hspi->State != HAL_SPI_STATE_BUSY_RX) {
        hspi->State = HAL_SPI_STATE_READY;
    }

    if (__spi_transfer_prepare(hspi, hspi->State, pTxData, Size, pRxData,
                               Size) != HAL_OK) {
        hspi->State = HAL_SPI_STATE_READY;
        VSF_STHAL_UNLOCK(hspi);
        return HAL_ERROR;
    }

    return __spi_transfer_with_timeout(hspi, Timeout);
}

HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                      uint16_t Size)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }
    if ((pData == NULL) || (Size == 0U)) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_ERROR;
    }
    if (hspi->State != HAL_SPI_STATE_READY) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_BUSY;
    }

    VSF_STHAL_ASSERT(IS_SPI_ALL_INSTANCE(hspi->Instance));
    VSF_STHAL_ASSERT(IS_SPI_DIRECTION_2LINES_OR_1LINE(hspi->Init.Direction));

    HAL_StatusTypeDef status = __spi_transfer_prepare(
        hspi, HAL_SPI_STATE_BUSY_TX, pData, Size, NULL, 0);
    if (status != HAL_OK) {
        VSF_STHAL_UNLOCK(hspi);
        return status;
    }

    __spi_fifo_transfer_continue(hspi);

    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;
    vsf_spi_irq_enable(spi, VSF_SPI_IRQ_MASK_TX);

    VSF_STHAL_UNLOCK(hspi);
    return status;
}

HAL_StatusTypeDef HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                     uint16_t Size)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    VSF_STHAL_ASSERT(IS_SPI_ALL_INSTANCE(hspi->Instance));

    if (hspi->State != HAL_SPI_STATE_READY) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_BUSY;
    }
    if ((pData == NULL) || (Size == 0U)) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = __spi_transfer_prepare(
        hspi, HAL_SPI_STATE_BUSY_RX, NULL, 0, pData, Size);
    if (status != HAL_OK) {
        VSF_STHAL_UNLOCK(hspi);
        return status;
    }

    __spi_fifo_transfer_continue(hspi);

    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;
    vsf_spi_irq_enable(spi, VSF_SPI_IRQ_MASK_RX | VSF_SPI_IRQ_MASK_ERR);

    VSF_STHAL_UNLOCK(hspi);
    return status;
}

HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi,
                                             uint8_t *pTxData, uint8_t *pRxData,
                                             uint16_t Size)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    VSF_STHAL_ASSERT(IS_SPI_ALL_INSTANCE(hspi->Instance));
    VSF_STHAL_ASSERT(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

    if (!((hspi->State == HAL_SPI_STATE_READY) ||
          ((hspi->Init.Mode == SPI_MODE_MASTER) &&
           (hspi->Init.Direction == SPI_DIRECTION_2LINES) &&
           (hspi->State == HAL_SPI_STATE_BUSY_RX)))) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_BUSY;
    }
    if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U)) {
        return HAL_ERROR;
    }

    HAL_SPI_StateTypeDef State;
    if (hspi->State == HAL_SPI_STATE_BUSY_RX) {
        hspi->State = HAL_SPI_STATE_READY;
        State       = HAL_SPI_STATE_BUSY_RX;
    } else {
        State = HAL_SPI_STATE_BUSY_TX_RX;
    }

    HAL_StatusTypeDef status =
        __spi_transfer_prepare(hspi, State, pTxData, Size, pRxData, Size);
    if (status != HAL_OK) {
        VSF_STHAL_UNLOCK(hspi);
        return status;
    }

    __spi_fifo_transfer_continue(hspi);

    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;
    vsf_spi_irq_enable(spi, VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX |
                                VSF_SPI_IRQ_MASK_ERR);

    VSF_STHAL_UNLOCK(hspi);
    return status;
}

HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                       uint16_t Size)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    VSF_STHAL_ASSERT(IS_SPI_ALL_INSTANCE(hspi->Instance));
    VSF_STHAL_ASSERT(IS_SPI_DIRECTION_2LINES_OR_1LINE(hspi->Init.Direction));

    if (hspi->State != HAL_SPI_STATE_READY) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_BUSY;
    }

    if ((pData == NULL) || (Size == 0U)) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = __spi_transfer_prepare(
        hspi, HAL_SPI_STATE_BUSY_TX, pData, Size, NULL, 0);
    if (status != HAL_OK) {
        VSF_STHAL_UNLOCK(hspi);
        return status;
    }

    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;

    vsf_spi_irq_enable(spi, VSF_SPI_IRQ_MASK_TX_CPL);
    vsf_err_t err = vsf_spi_request_transfer(spi, (void *)pData, NULL, Size);
    if (err != VSF_ERR_NONE) {
        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hspi);

    return status;
}

HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                      uint16_t Size)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    if (hspi->State != HAL_SPI_STATE_READY) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_BUSY;
    }
    if ((pData == NULL) || (Size == 0U)) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = __spi_transfer_prepare(
        hspi, HAL_SPI_STATE_BUSY_RX, NULL, 0, pData, Size);
    if (status != HAL_OK) {
        VSF_STHAL_UNLOCK(hspi);
        return status;
    }

    VSF_STHAL_ASSERT(IS_SPI_ALL_INSTANCE(hspi->Instance));
    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;

    vsf_spi_irq_enable(spi, VSF_SPI_IRQ_MASK_RX_CPL | VSF_SPI_IRQ_MASK_ERR);

    vsf_err_t err = vsf_spi_request_transfer(spi, NULL, (void *)pData, Size);
    if (err != VSF_ERR_NONE) {
        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hspi);
    return status;
}

HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi,
                                              uint8_t           *pTxData,
                                              uint8_t *pRxData, uint16_t Size)
{
    if (hspi == NULL) {
        return HAL_ERROR;
    }

    VSF_STHAL_ASSERT(IS_SPI_ALL_INSTANCE(hspi->Instance));
    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;

    if (!((hspi->State == HAL_SPI_STATE_READY) ||
          ((hspi->Init.Mode == SPI_MODE_MASTER) &&
           (hspi->Init.Direction == SPI_DIRECTION_2LINES) &&
           (hspi->State == HAL_SPI_STATE_BUSY_RX)))) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_BUSY;
    }
    if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U)) {
        VSF_STHAL_UNLOCK(hspi);
        return HAL_ERROR;
    }

    HAL_SPI_StateTypeDef State;
    if (hspi->State == HAL_SPI_STATE_BUSY_RX) {
        hspi->State = HAL_SPI_STATE_READY;
        State       = HAL_SPI_STATE_BUSY_RX;
    } else {
        State = HAL_SPI_STATE_BUSY_TX_RX;
    }

    HAL_StatusTypeDef status =
        __spi_transfer_prepare(hspi, State, pTxData, Size, pRxData, Size);
    if (status != HAL_OK) {
        VSF_STHAL_UNLOCK(hspi);
        return status;
    }

    vsf_spi_irq_enable(spi, VSF_SPI_IRQ_MASK_RX_CPL | VSF_SPI_IRQ_MASK_ERR);

    vsf_err_t err =
        vsf_spi_request_transfer(spi, (void *)pTxData, (void *)pRxData, Size);
    if (err != VSF_ERR_NONE) {
        hspi->State = HAL_SPI_STATE_READY;
        status      = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hspi);
    return status;
}

HAL_StatusTypeDef HAL_SPI_Abort(SPI_HandleTypeDef *hspi)
{
    HAL_StatusTypeDef status;
    VSF_STHAL_ASSERT(hspi != NULL);
    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

    status = HAL_OK;

    vsf_spi_irq_disable(spi, VSF_SPI_IRQ_ALL_BITS_MASK);

    vsf_err_t err = vsf_spi_cancel_transfer(spi);
    if (err < VSF_ERR_NONE) {
        return HAL_ERROR;
    }

    hspi->RxXferCount = 0U;
    hspi->TxXferCount = 0U;
    if (hspi->ErrorCode == HAL_SPI_ERROR_ABORT) {
        status = HAL_ERROR;
    } else {
        hspi->ErrorCode = HAL_SPI_ERROR_NONE;
    }
    hspi->State = HAL_SPI_STATE_READY;

    return status;
}

HAL_StatusTypeDef HAL_SPI_Abort_IT(SPI_HandleTypeDef *hspi)
{
    VSF_STHAL_ASSERT(hspi != NULL);
    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

    vsf_spi_irq_disable(spi, VSF_SPI_IRQ_ALL_BITS_MASK);

    vsf_err_t err = vsf_spi_cancel_transfer(spi);
    if (err < VSF_ERR_NONE) {
        return HAL_ERROR;
    } else if (err == VSF_ERR_NOT_READY) {
        return HAL_OK;
    }

    hspi->RxXferCount        = 0U;
    hspi->TxXferCount        = 0U;
    HAL_StatusTypeDef status = HAL_OK;
    if (hspi->ErrorCode == HAL_SPI_ERROR_ABORT) {
        status = HAL_ERROR;
    } else {
        hspi->ErrorCode = HAL_SPI_ERROR_NONE;
    }
    hspi->State = HAL_SPI_STATE_READY;

#   if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
    hspi->AbortCpltCallback(hspi);
#   else
    HAL_SPI_AbortCpltCallback(hspi);
#   endif

    return status;
}

HAL_StatusTypeDef HAL_SPI_DMAPause(SPI_HandleTypeDef *hspi)
{
    VSF_STHAL_ASSERT(hspi != NULL);
    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

#   ifdef VSF_SPI_CTRL_REQUEST_PAUSE
    vsf_spi_ctrl(spi, VSF_SPI_CTRL_REQUEST_PAUSE);
    return HAL_OK;
#   else
    return HAL_ERROR;
#   endif
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_DMAResume(SPI_HandleTypeDef *hspi)
{
    VSF_STHAL_ASSERT(hspi != NULL);
    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

#   ifdef VSF_SPI_CTRL_REQUEST_RESUME
    vsf_spi_ctrl(spi, VSF_SPI_CTRL_REQUEST_RESUME);
    return HAL_OK;
#   else
    return HAL_ERROR;
#   endif
}

HAL_StatusTypeDef HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi)
{
    VSF_STHAL_ASSERT(hspi != NULL);
    vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

    vsf_err_t err = vsf_spi_cancel_transfer(spi);
    if (err < VSF_ERR_NONE) {
        return HAL_ERROR;
    }

    hspi->State = HAL_SPI_STATE_READY;
    return HAL_OK;
}

void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi)
{
    VSF_STHAL_ASSERT(hspi != NULL);
    VSF_UNUSED_PARAM(hspi);
}

HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef *hspi)
{
    VSF_STHAL_ASSERT(hspi != NULL);

    if (hspi->State != HAL_SPI_STATE_READY) {
        VSF_STHAL_ASSERT(IS_SPI_ALL_INSTANCE(hspi->Instance));
        vsf_spi_t *spi = (vsf_spi_t *)hspi->Instance;
        vsf_spi_status_t status = vsf_spi_status(spi);
        if (!status.is_busy) {
            hspi->State = HAL_SPI_STATE_READY;
        }
    }

    return hspi->State;
}

uint32_t HAL_SPI_GetError(SPI_HandleTypeDef *hspi)
{
    VSF_STHAL_ASSERT(hspi != NULL);
    return hspi->ErrorCode;
}

#endif
