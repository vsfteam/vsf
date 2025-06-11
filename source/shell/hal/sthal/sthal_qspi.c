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

#if defined(HAL_QSPI_MODULE_ENABLED) && defined(VSF_SPI_CTRL_QSPI_ENABLE)

#   include "sthal_internal.h"

/*============================ MACROS ========================================*/

#   define QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE 0x00000000U
#   define QSPI_FUNCTIONAL_MODE_INDIRECT_READ  0x00000001U
#   define QSPI_FUNCTIONAL_MODE_AUTO_POLLING   0x00000002U
#   define QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED  0x00000003U

#   define IS_QSPI_FUNCTIONAL_MODE(MODE)                                       \
       (((MODE) == QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE) ||                     \
        ((MODE) == QSPI_FUNCTIONAL_MODE_INDIRECT_READ) ||                      \
        ((MODE) == QSPI_FUNCTIONAL_MODE_AUTO_POLLING) ||                       \
        ((MODE) == QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED))

#   ifndef VSF_STHAL_CFG_QSPI_DEFAULT_CLOCK
#      define VSF_STHAL_CFG_QSPI_DEFAULT_CLOCK (100 * 1000 * 1000)
#   endif

#   ifndef VSF_SPI_CTRL_QSPI_CMD_PHASE_ENABLE
#      error "VSF_SPI_CTRL_QSPI_CMD_PHASE_ENABLE is not defined"
#   endif
#   ifndef VSF_SPI_CTRL_QSPI_CMD_PHASE_DISABLE
#      error "VSF_SPI_CTRL_QSPI_CMD_PHASE_DISABLE is not defined"
#   endif
#   ifndef VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_LINE_MODE
#      error "VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_LINE_MODE is not defined"
#   endif
#   ifndef VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_LINE_MODE
#      error "VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_LINE_MODE is not defined"
#   endif
#   ifndef VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_VALUE
#      error "VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_VALUE is not defined"
#   endif

#   ifndef VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_ENABLE
#      error "VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_ENABLE is not defined"
#   endif
#   ifndef VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_DISABLE
#      error "VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_DISABLE is not defined"
#   endif
#   ifndef VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_GET_LINE_MODE
#      error "VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_GET_LINE_MODE is not defined"
#   endif
#   ifndef VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_LINE_MODE
#      error "VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_LINE_MODE is not defined"
#   endif
#   ifndef VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_SIZE
#      error "VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_SIZE is not defined"
#   endif

#   ifndef VSF_SPI_CTRL_QSPI_DATA_PHASE_GET_LINE_MODE
#      error "VSF_SPI_CTRL_QSPI_DATA_PHASE_GET_LINE_MODE is not defined"
#   endif
#   ifndef VSF_SPI_CTRL_QSPI_DATA_PHASE_SET_LINE_MODE
#      error "VSF_SPI_CTRL_QSPI_DATA_PHASE_SET_LINE_MODE is not defined"
#   endif

#   ifndef VSF_SPI_CTRL_QSPI_TRANSFER_SET_MODE
#      error "VSF_SPI_CTRL_QSPI_TRANSFER_SET_MODE is not defined"
#   endif
#   ifndef VSF_SPI_CTRL_QSPI_TRANSFER_GET_MODE
#      error "VSF_SPI_CTRL_QSPI_TRANSFER_GET_MODE is not defined"
#   endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

/*============================ PROTOTYPES ====================================*/

static void              QSPI_DMARxCplt(DMA_HandleTypeDef *hdma);
static void              QSPI_DMATxCplt(DMA_HandleTypeDef *hdma);
static void              QSPI_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void              QSPI_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void              QSPI_DMAError(DMA_HandleTypeDef *hdma);
static void              QSPI_DMAAbortCplt(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef QSPI_WaitFlagStateUntilTimeout(
    QSPI_HandleTypeDef *hqspi, uint32_t Flag, FlagStatus State,
    uint32_t Tickstart, uint32_t Timeout);
static HAL_StatusTypeDef QSPI_WaitFlagStateUntilTimeout_CPUCycle(
    QSPI_HandleTypeDef *hqspi, uint32_t Flag, FlagStatus State,
    uint32_t Timeout);
static void QSPI_Config(QSPI_HandleTypeDef *hqspi, QSPI_CommandTypeDef *cmd,
                        uint32_t FunctionalMode);

/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(HAL_QSPI_MspInit)
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

VSF_CAL_WEAK(HAL_QSPI_MspDeInit)
void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

VSF_CAL_WEAK(HAL_QSPI_ErrorCallback)
void HAL_QSPI_ErrorCallback(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

VSF_CAL_WEAK(HAL_QSPI_AbortCpltCallback)
void HAL_QSPI_AbortCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

VSF_CAL_WEAK(HAL_QSPI_CmdCpltCallback)
void HAL_QSPI_CmdCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

VSF_CAL_WEAK(HAL_QSPI_RxCpltCallback)
void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

VSF_CAL_WEAK(HAL_QSPI_TxCpltCallback)
void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

VSF_CAL_WEAK(HAL_QSPI_RxHalfCpltCallback)
void HAL_QSPI_RxHalfCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

VSF_CAL_WEAK(HAL_QSPI_TxHalfCpltCallback)
void HAL_QSPI_TxHalfCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

VSF_CAL_WEAK(HAL_QSPI_FifoThresholdCallback)
void HAL_QSPI_FifoThresholdCallback(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

VSF_CAL_WEAK(HAL_QSPI_StatusMatchCallback)
void HAL_QSPI_StatusMatchCallback(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

VSF_CAL_WEAK(HAL_QSPI_TimeOutCallback)
void HAL_QSPI_TimeOutCallback(QSPI_HandleTypeDef *hqspi)
{
    VSF_UNUSED_PARAM(hqspi);
}

#   if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_QSPI_RegisterCallback(
    QSPI_HandleTypeDef *hqspi, HAL_QSPI_CallbackIDTypeDef CallbackId,
    pQSPI_CallbackTypeDef pCallback)
{
    HAL_StatusTypeDef status = HAL_OK;

    if (pCallback == NULL) {

        hqspi->ErrorCode |= HAL_QSPI_ERROR_INVALID_CALLBACK;
        return HAL_ERROR;
    }

    VSF_STHAL_LOCK(hqspi);

    if (hqspi->State == HAL_QSPI_STATE_READY) {
        switch (CallbackId) {
        case HAL_QSPI_ERROR_CB_ID:
            hqspi->ErrorCallback = pCallback;
            break;
        case HAL_QSPI_ABORT_CB_ID:
            hqspi->AbortCpltCallback = pCallback;
            break;
        case HAL_QSPI_FIFO_THRESHOLD_CB_ID:
            hqspi->FifoThresholdCallback = pCallback;
            break;
        case HAL_QSPI_CMD_CPLT_CB_ID:
            hqspi->CmdCpltCallback = pCallback;
            break;
        case HAL_QSPI_RX_CPLT_CB_ID:
            hqspi->RxCpltCallback = pCallback;
            break;
        case HAL_QSPI_TX_CPLT_CB_ID:
            hqspi->TxCpltCallback = pCallback;
            break;
        case HAL_QSPI_RX_HALF_CPLT_CB_ID:
            hqspi->RxHalfCpltCallback = pCallback;
            break;
        case HAL_QSPI_TX_HALF_CPLT_CB_ID:
            hqspi->TxHalfCpltCallback = pCallback;
            break;
        case HAL_QSPI_STATUS_MATCH_CB_ID:
            hqspi->StatusMatchCallback = pCallback;
            break;
        case HAL_QSPI_TIMEOUT_CB_ID:
            hqspi->TimeOutCallback = pCallback;
            break;
        case HAL_QSPI_MSP_INIT_CB_ID:
            hqspi->MspInitCallback = pCallback;
            break;
        case HAL_QSPI_MSP_DEINIT_CB_ID:
            hqspi->MspDeInitCallback = pCallback;
            break;
        default:

            hqspi->ErrorCode |= HAL_QSPI_ERROR_INVALID_CALLBACK;

            status = HAL_ERROR;
            break;
        }
    } else if (hqspi->State == HAL_QSPI_STATE_RESET) {
        switch (CallbackId) {
        case HAL_QSPI_MSP_INIT_CB_ID:
            hqspi->MspInitCallback = pCallback;
            break;
        case HAL_QSPI_MSP_DEINIT_CB_ID:
            hqspi->MspDeInitCallback = pCallback;
            break;
        default:

            hqspi->ErrorCode |= HAL_QSPI_ERROR_INVALID_CALLBACK;

            status = HAL_ERROR;
            break;
        }
    } else {

        hqspi->ErrorCode |= HAL_QSPI_ERROR_INVALID_CALLBACK;

        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hqspi);
    return status;
}

HAL_StatusTypeDef HAL_QSPI_UnRegisterCallback(
    QSPI_HandleTypeDef *hqspi, HAL_QSPI_CallbackIDTypeDef CallbackId)
{
    HAL_StatusTypeDef status = HAL_OK;

    VSF_STHAL_LOCK(hqspi);

    if (hqspi->State == HAL_QSPI_STATE_READY) {
        switch (CallbackId) {
        case HAL_QSPI_ERROR_CB_ID:
            hqspi->ErrorCallback = HAL_QSPI_ErrorCallback;
            break;
        case HAL_QSPI_ABORT_CB_ID:
            hqspi->AbortCpltCallback = HAL_QSPI_AbortCpltCallback;
            break;
        case HAL_QSPI_FIFO_THRESHOLD_CB_ID:
            hqspi->FifoThresholdCallback = HAL_QSPI_FifoThresholdCallback;
            break;
        case HAL_QSPI_CMD_CPLT_CB_ID:
            hqspi->CmdCpltCallback = HAL_QSPI_CmdCpltCallback;
            break;
        case HAL_QSPI_RX_CPLT_CB_ID:
            hqspi->RxCpltCallback = HAL_QSPI_RxCpltCallback;
            break;
        case HAL_QSPI_TX_CPLT_CB_ID:
            hqspi->TxCpltCallback = HAL_QSPI_TxCpltCallback;
            break;
        case HAL_QSPI_RX_HALF_CPLT_CB_ID:
            hqspi->RxHalfCpltCallback = HAL_QSPI_RxHalfCpltCallback;
            break;
        case HAL_QSPI_TX_HALF_CPLT_CB_ID:
            hqspi->TxHalfCpltCallback = HAL_QSPI_TxHalfCpltCallback;
            break;
        case HAL_QSPI_STATUS_MATCH_CB_ID:
            hqspi->StatusMatchCallback = HAL_QSPI_StatusMatchCallback;
            break;
        case HAL_QSPI_TIMEOUT_CB_ID:
            hqspi->TimeOutCallback = HAL_QSPI_TimeOutCallback;
            break;
        case HAL_QSPI_MSP_INIT_CB_ID:
            hqspi->MspInitCallback = HAL_QSPI_MspInit;
            break;
        case HAL_QSPI_MSP_DEINIT_CB_ID:
            hqspi->MspDeInitCallback = HAL_QSPI_MspDeInit;
            break;
        default:

            hqspi->ErrorCode |= HAL_QSPI_ERROR_INVALID_CALLBACK;

            status = HAL_ERROR;
            break;
        }
    } else if (hqspi->State == HAL_QSPI_STATE_RESET) {
        switch (CallbackId) {
        case HAL_QSPI_MSP_INIT_CB_ID:
            hqspi->MspInitCallback = HAL_QSPI_MspInit;
            break;
        case HAL_QSPI_MSP_DEINIT_CB_ID:
            hqspi->MspDeInitCallback = HAL_QSPI_MspDeInit;
            break;
        default:

            hqspi->ErrorCode |= HAL_QSPI_ERROR_INVALID_CALLBACK;

            status = HAL_ERROR;
            break;
        }
    } else {

        hqspi->ErrorCode |= HAL_QSPI_ERROR_INVALID_CALLBACK;

        status = HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hqspi);
    return status;
}
#   endif

static bool __spi_fifo_transfer_continue(QSPI_HandleTypeDef *hqspi)
{
    vsf_qspi_t     *spi = (vsf_qspi_t *)hqspi->Instance;
    uint_fast32_t *out_offset_ptr;
    uint_fast32_t *in_offset_ptr;
    uint_fast32_t  out_offset;
    uint_fast32_t  in_offset;
    uint_fast32_t  count = vsf_max(hqspi->TxXferSize, hqspi->RxXferSize);
    if (hqspi->pTxBuffPtr != NULL) {
        out_offset     = hqspi->TxXferSize - hqspi->TxXferCount;
        out_offset_ptr = &out_offset;
    } else {
        out_offset_ptr = NULL;
    }
    if (hqspi->pRxBuffPtr != NULL) {
        in_offset     = hqspi->RxXferSize - hqspi->RxXferCount;
        in_offset_ptr = &in_offset;
    } else {
        in_offset_ptr = NULL;
    }

    // 因为我们合并了 tx 和 rx 的 fifo 传输，所以在这里需要使用 vsf_spi_fifo_transfer
    vsf_spi_fifo_transfer(spi, (void *)hqspi->pTxBuffPtr, out_offset_ptr,
                          (void *)hqspi->pRxBuffPtr, in_offset_ptr, count);

    if (hqspi->pTxBuffPtr != NULL) {
        hqspi->TxXferCount = hqspi->TxXferSize - out_offset;
    }
    if (hqspi->pRxBuffPtr != NULL) {
        hqspi->RxXferCount = hqspi->RxXferSize - in_offset;
    }

    return (hqspi->TxXferCount > 0U) || (hqspi->RxXferCount > 0U);
}

static void __qspi_isr_handler(void *target_ptr, vsf_qspi_t *spi_ptr,
                               vsf_qspi_irq_mask_t irq_mask)
{

    QSPI_HandleTypeDef *hqspi = (QSPI_HandleTypeDef *)target_ptr;
    VSF_STHAL_ASSERT(hqspi != NULL);

    if (irq_mask & VSF_SPI_IRQ_MASK_TX_CPL) {
        if (hqspi->State == HAL_QSPI_STATE_BUSY_INDIRECT_TX) {
            vsf_qspi_irq_disable(spi_ptr, VSF_SPI_IRQ_MASK_TX_CPL);
            hqspi->State = HAL_QSPI_STATE_READY;
#   if USE_HAL_QSPI_REGISTER_CALLBACKS == 1U
            hqspi->TxCpltCallback(hqspi);
#   else
            HAL_QSPI_TxCpltCallback(hqspi);
#   endif
        }
    }

    if (irq_mask & VSF_SPI_IRQ_MASK_CPL) {
        if (hqspi->State == HAL_QSPI_STATE_BUSY_INDIRECT_RX) {
            hqspi->State = HAL_QSPI_STATE_READY;
#   if USE_HAL_QSPI_REGISTER_CALLBACKS == 1U
            hqspi->RxCpltCallback(hqspi);
#   else
            HAL_QSPI_RxCpltCallback(hqspi);
#   endif
        }
    }

    if (irq_mask & VSF_SPI_IRQ_MASK_TX) {
        if (!__spi_fifo_transfer_continue(hqspi)) {
            vsf_qspi_irq_disable(spi_ptr, VSF_SPI_IRQ_MASK_TX);
            if (hqspi->State == HAL_QSPI_STATE_BUSY_INDIRECT_TX) {
                hqspi->State = HAL_QSPI_STATE_READY;
#   if USE_HAL_SPI_REGISTER_CALLBACKS == 1U
                hqspi->TxCpltCallback(hqspi);
#   else
                HAL_QSPI_TxCpltCallback(hqspi);
#   endif
            }
        }
    }

    if (irq_mask & VSF_SPI_IRQ_MASK_RX) {
        if (!__spi_fifo_transfer_continue(hqspi)) {
            vsf_qspi_irq_disable(spi_ptr, VSF_SPI_IRQ_MASK_RX);
            if (hqspi->State == HAL_QSPI_STATE_BUSY_INDIRECT_RX) {
                hqspi->State = HAL_QSPI_STATE_READY;
#   if USE_HAL_QSPI_REGISTER_CALLBACKS == 1U
                hqspi->RxCpltCallback(hqspi);
#   else
                HAL_QSPI_RxCpltCallback(hqspi);
#   endif
            }
        }
    }
}

HAL_StatusTypeDef HAL_QSPI_Init(QSPI_HandleTypeDef *hqspi)
{
    if (hqspi == NULL) {
        return HAL_ERROR;
    }
    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

    if (hqspi->State == HAL_QSPI_STATE_RESET) {
        VSF_STHAL_UNLOCK(hqspi);
#   if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
        hqspi->ErrorCallback         = HAL_QSPI_ErrorCallback;
        hqspi->AbortCpltCallback     = HAL_QSPI_AbortCpltCallback;
        hqspi->FifoThresholdCallback = HAL_QSPI_FifoThresholdCallback;
        hqspi->CmdCpltCallback       = HAL_QSPI_CmdCpltCallback;
        hqspi->RxCpltCallback        = HAL_QSPI_RxCpltCallback;
        hqspi->TxCpltCallback        = HAL_QSPI_TxCpltCallback;
        hqspi->RxHalfCpltCallback    = HAL_QSPI_RxHalfCpltCallback;
        hqspi->TxHalfCpltCallback    = HAL_QSPI_TxHalfCpltCallback;
        hqspi->StatusMatchCallback   = HAL_QSPI_StatusMatchCallback;
        hqspi->TimeOutCallback       = HAL_QSPI_TimeOutCallback;
        if (hqspi->MspInitCallback == NULL) {
            hqspi->MspInitCallback = HAL_QSPI_MspInit;
        }
        hqspi->MspInitCallback(hqspi);
#   else
        HAL_QSPI_MspInit(hqspi);
#   endif
    }
    hqspi->State = HAL_QSPI_STATE_BUSY;

    uint32_t div =
        VSF_STHAL_CFG_QSPI_DEFAULT_CLOCK /
        (hqspi->Init.ClockPrescaler ? hqspi->Init.ClockPrescaler : 1);

    vsf_qspi_mode_t mode = VSF_SPI_MASTER | VSF_SPI_DATASIZE_8;

    mode |= hqspi->Init.FifoThreshold;
    mode |= hqspi->Init.SampleShifting;
    mode |= hqspi->Init.ChipSelectHighTime;
    mode |= hqspi->Init.ClockMode;
    mode |= hqspi->Init.FlashID;
    mode |= hqspi->Init.DualFlash;

    vsf_qspi_cfg_t cfg = {
        .mode     = mode,
        .clock_hz = div,
        .isr =
            {
                .handler_fn = __qspi_isr_handler,
                .target_ptr = hqspi,
                .prio       = vsf_arch_prio_0,
            },
    };
    vsf_err_t err = vsf_qspi_init(spi, &cfg);
    if (err != VSF_ERR_NONE) {
        hqspi->ErrorCode |= HAL_QSPI_ERROR_INVALID_PARAM;
        hqspi->State = HAL_QSPI_STATE_READY;
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_ERROR;
    }
    while (fsm_rt_cpl != vsf_qspi_enable(spi));

#   ifdef VSF_SPI_CTRL_QSPI_QSPI_FLASH_SIZE_SET
    uint32_t flash_size = hqspi->Init.FlashSize;
    err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_QSPI_FLASH_SIZE_SET, &flash_size);
    if (err != VSF_ERR_NONE) {
        hqspi->ErrorCode |= HAL_QSPI_ERROR_INVALID_PARAM;
        hqspi->State = HAL_QSPI_STATE_READY;
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_ERROR;
    }
#   endif

    err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_ENABLE, NULL);
    if (err != VSF_ERR_NONE) {
        hqspi->ErrorCode |= HAL_QSPI_ERROR_INVALID_PARAM;
        hqspi->State = HAL_QSPI_STATE_READY;
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_ERROR;
    }

    hqspi->ErrorCode = HAL_QSPI_ERROR_NONE;
    hqspi->State     = HAL_QSPI_STATE_READY;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_QSPI_DeInit(QSPI_HandleTypeDef *hqspi)
{
    if (hqspi == NULL) {
        return HAL_ERROR;
    }

    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

    hqspi->State = HAL_QSPI_STATE_BUSY;

    vsf_qspi_fini(spi);

#   if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
    if (hqspi->MspDeInitCallback == NULL) {
        hqspi->MspDeInitCallback = HAL_QSPI_MspDeInit;
    }
    hqspi->MspDeInitCallback(hqspi);
#   else
    HAL_QSPI_MspDeInit(hqspi);
#   endif

    hqspi->ErrorCode = HAL_QSPI_ERROR_NONE;
    hqspi->State     = HAL_QSPI_STATE_RESET;

    VSF_STHAL_UNLOCK(hqspi);

    return HAL_OK;
}

static HAL_StatusTypeDef __HAL_QSPI_Command(QSPI_HandleTypeDef  *hqspi,
                                            QSPI_CommandTypeDef *cmd,
                                            uint32_t             Timeout)
{
    HAL_StatusTypeDef status = HAL_OK;
    vsf_err_t         err;

    VSF_STHAL_ASSERT(IS_QSPI_INSTRUCTION_MODE(cmd->InstructionMode));
    if (cmd->InstructionMode != QSPI_INSTRUCTION_NONE) {
        VSF_STHAL_ASSERT(IS_QSPI_INSTRUCTION(cmd->Instruction));
    }

    VSF_STHAL_ASSERT(IS_QSPI_ADDRESS_MODE(cmd->AddressMode));
    if (cmd->AddressMode != QSPI_ADDRESS_NONE) {
        VSF_STHAL_ASSERT(IS_QSPI_ADDRESS_SIZE(cmd->AddressSize));
    }

    VSF_STHAL_ASSERT(IS_QSPI_ALTERNATE_BYTES_MODE(cmd->AlternateByteMode));
    VSF_STHAL_ASSERT(IS_QSPI_DUMMY_CYCLES(cmd->DummyCycles));
    VSF_STHAL_ASSERT(IS_QSPI_DATA_MODE(cmd->DataMode));
    VSF_STHAL_ASSERT(IS_QSPI_DDR_MODE(cmd->DdrMode));
    VSF_STHAL_ASSERT(IS_QSPI_DDR_HHC(cmd->DdrHoldHalfCycle));
    VSF_STHAL_ASSERT(IS_QSPI_SIOO_MODE(cmd->SIOOMode));

    VSF_STHAL_LOCK(hqspi);
    if (hqspi->State != HAL_QSPI_STATE_READY) {
        status = HAL_BUSY;
        VSF_STHAL_UNLOCK(hqspi);
        return status;
    }

    hqspi->ErrorCode = HAL_QSPI_ERROR_NONE;
    hqspi->State     = HAL_QSPI_STATE_BUSY;

    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);
    uint32_t tickstart = HAL_GetTick();

    status = __spi_is_idle(spi, Timeout, tickstart);
    if (status != HAL_OK) {
        hqspi->State = HAL_QSPI_STATE_READY;
        VSF_STHAL_UNLOCK(hqspi);
        return status;
    }

    // Data Phase
    // Data phase is the last part of the transfer, but other configurations
    // might depend on the data phase configuration, such as in Dual / Quad IO
    // modes, whether different line mode configurations for commands and
    // addresses are supported
    uint32_t data_phase;
    err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_DATA_PHASE_GET_LINE_MODE,
                       &data_phase);
    VSF_ASSERT(err == VSF_ERR_NONE);
    if (data_phase != cmd->DataMode) {
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_DATA_PHASE_SET_LINE_MODE,
                           &cmd->DataMode);
        VSF_ASSERT(err == VSF_ERR_NONE);
    }

    // Command (Instruction) Phase
    if (cmd->InstructionMode == QSPI_INSTRUCTION_NONE) {
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_CMD_PHASE_DISABLE, NULL);
        VSF_ASSERT(err == VSF_ERR_NONE);
    } else {
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_CMD_PHASE_ENABLE, NULL);
        VSF_ASSERT(err == VSF_ERR_NONE);

        uint32_t inst_mode;
        switch (cmd->InstructionMode) {
        case QSPI_INSTRUCTION_1_LINE:
            inst_mode = VSF_SPI_CTRL_QSPI_CMD_SINGLE;
            break;
        case QSPI_INSTRUCTION_2_LINES:
            inst_mode = VSF_SPI_CTRL_QSPI_CMD_DUAL;
            break;
        case QSPI_INSTRUCTION_4_LINES:
            inst_mode = VSF_SPI_CTRL_QSPI_CMD_QUAD;
            break;
        default:
            VSF_ASSERT(0);
            break;
        }

        uint32_t cmd_line_mode;
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_CMD_PHASE_GET_LINE_MODE,
                           &cmd_line_mode);
        VSF_ASSERT(err == VSF_ERR_NONE);

        if (cmd_line_mode != inst_mode) {
            err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_LINE_MODE,
                               &inst_mode);
            VSF_ASSERT(err == VSF_ERR_NONE);
        }

        uint8_t instruction = cmd->Instruction;
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_CMD_PHASE_SET_VALUE,
                           &instruction);
        VSF_ASSERT(err == VSF_ERR_NONE);
    }

    // Address Phase
    if (cmd->AddressMode == QSPI_ADDRESS_NONE) {
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_DISABLE, NULL);
        VSF_ASSERT(err == VSF_ERR_NONE);
    } else {
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_ENABLE, NULL);
        VSF_ASSERT(err == VSF_ERR_NONE);

        uint32_t addr_line_mode = 0;
        switch (cmd->AddressMode) {
        case QSPI_ADDRESS_1_LINE:
            addr_line_mode = VSF_SPI_CTRL_QSPI_ADDRESS_SINGLE;
            break;
        case QSPI_ADDRESS_2_LINES:
            addr_line_mode = VSF_SPI_CTRL_QSPI_ADDRESS_DUAL;
            break;
        case QSPI_ADDRESS_4_LINES:
            addr_line_mode = VSF_SPI_CTRL_QSPI_ADDRESS_QUAD;
            break;
        }

        uint32_t addr_line_mode_current;
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_GET_LINE_MODE,
                           &addr_line_mode_current);
        VSF_ASSERT(err == VSF_ERR_NONE);
        if (addr_line_mode_current != addr_line_mode) {
            err =
                vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_LINE_MODE,
                             &addr_line_mode);
            VSF_ASSERT(err == VSF_ERR_NONE);
        }

        uint32_t addr = cmd->Address;
        err           = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_ADDRESS_SET, &addr);
        VSF_ASSERT(err == VSF_ERR_NONE);

        uint32_t address_size_mode = cmd->AddressSize;
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_ADDRESS_PHASE_SET_SIZE,
                           &address_size_mode);
        VSF_ASSERT(err == VSF_ERR_NONE);
    }

    // Alternate Bytes Phase
#   ifdef VSF_SPI_CTRL_QSPI_QSPI_ALATERNATE_BYTES_MODE
    if (cmd->AlternateByteMode == QSPI_ALTERNATE_BYTES_NONE) {
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_QSPI_ALTERNATE_BYTES_DISABLE,
                           NULL);
        VSF_ASSERT(err == VSF_ERR_NONE);
    } else {
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_QSPI_ALTERNATE_BYTES_ENABLE,
                           NULL);
        VSF_ASSERT(err == VSF_ERR_NONE);
    }
    // TODO: add more alternate bytes mode
    cmd->AlternateByteMode;
    cmd->AlternateBytes;
    cmd->AlternateBytesSize;
#   endif

    // Dummy Cycles Phase
#   if defined(VSF_SPI_CTRL_QSPI_DUMMY_PHASE_ENABLE) &&                        \
       defined(VSF_SPI_CTRL_QSPI_DUMMY_PHASE_DISABLE) &&                       \
       defined(VSF_SPI_CTRL_QSPI_DUMMY_PHASE_SET_CYCLES)
    hqspi->DummyCycles = cmd->DummyCycles;
    if (cmd->DummyCycles != 0) {
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_DUMMY_PHASE_ENABLE, NULL);
        VSF_ASSERT(err == VSF_ERR_NONE);

        uint8_t dummy = cmd->DummyCycles;
        err =
            vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_DUMMY_PHASE_SET_CYCLES, &dummy);
        VSF_ASSERT(err == VSF_ERR_NONE);
    } else {
        err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_DUMMY_PHASE_DISABLE, NULL);
        VSF_ASSERT(err == VSF_ERR_NONE);
    }
#   endif

    // Miscellaneous
#   ifdef VSF_SPI_CTRL_QSPI_QSPI_DRR_MODE
    uint8_t dummy = cmd->DdrHoldHalfCycle;
    if (cmd->DdrMode == QSPI_DDR_MODE_ENABLE) {
        err =
            vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_QSPI_DRR_MODE_ENABLED, &dummy);
        VSF_ASSERT(err == VSF_ERR_NONE);

#      ifdef VSF_SPI_CTRL_QSPI_QSPI_DDR_HOLD_HALF_CYCLE
        if (cmd->DdrHoldHalfCycle == QSPI_DDR_HHC_ENABLE) {
            err =
                vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_QSPI_DDR_HHC_ENABLED, NULL);
            VSF_ASSERT(err == VSF_ERR_NONE);
        } else {
            err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_QSPI_DDR_HHC_DISABLED,
                               NULL);
            VSF_ASSERT(err == VSF_ERR_NONE);
        }
#      endif
    } else {
        err =
            vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_QSPI_DRR_MODE_DISABLED, &dummy);
        VSF_ASSERT(err == VSF_ERR_NONE);
    }
#   endif

    hqspi->NbData = cmd->NbData;

    return status;
}

static bool __HAL_QSPI_Command_need_other_transfer(QSPI_CommandTypeDef *cmd)
{
    VSF_STHAL_ASSERT(cmd != NULL);

    if (cmd->NbData != 0) {
        return false;
    }
    if (cmd->InstructionMode == QSPI_INSTRUCTION_NONE) {
        return false;
    }
    return true;
}

HAL_StatusTypeDef HAL_QSPI_Command(QSPI_HandleTypeDef  *hqspi,
                                   QSPI_CommandTypeDef *cmd, uint32_t Timeout)
{
    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);
    HAL_StatusTypeDef status;

    status = __HAL_QSPI_Command(hqspi, cmd, Timeout);
    if (status != HAL_OK) {
        return status;
    }

    if (__HAL_QSPI_Command_need_other_transfer(cmd)) {
        vsf_spi_fifo_transfer(spi, NULL, NULL, NULL, NULL, 0);
    }

    hqspi->State = HAL_QSPI_STATE_READY;
    VSF_STHAL_UNLOCK(hqspi);

    return status;
}

HAL_StatusTypeDef HAL_QSPI_Command_IT(QSPI_HandleTypeDef  *hqspi,
                                      QSPI_CommandTypeDef *cmd)
{
    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);
    HAL_StatusTypeDef status;

    status = __HAL_QSPI_Command(hqspi, cmd, 0);
    if (status != HAL_OK) {
        return status;
    }

    /// TODO: enable interrupt
    if (__HAL_QSPI_Command_need_other_transfer(cmd)) {
        vsf_spi_fifo_transfer(spi, NULL, NULL, NULL, NULL, 0);
    }

    hqspi->State = HAL_QSPI_STATE_READY;
    VSF_STHAL_UNLOCK(hqspi);

#   if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
    hqspi->CmdCpltCallback(hqspi);
#   else
    HAL_QSPI_CmdCpltCallback(hqspi);
#   endif

    return status;
}

static HAL_StatusTypeDef __spi_transfer_with_timeout(QSPI_HandleTypeDef *hqspi,
                                                     uint32_t Timeout)
{
    if (hqspi == NULL) {
        return HAL_ERROR;
    }

    VSF_STHAL_ASSERT(IS_SPI_ALL_INSTANCE(hqspi->Instance));

    uint32_t tickstart = HAL_GetTick();

    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;

    while ((hqspi->TxXferCount > 0U) || (hqspi->RxXferCount > 0U)) {
        __spi_fifo_transfer_continue(hqspi);

        if (__sthal_check_timeout(tickstart, Timeout)) {
            hqspi->State = HAL_SPI_STATE_READY;
            VSF_STHAL_UNLOCK(hqspi);
            return HAL_TIMEOUT;
        }
    }

    HAL_StatusTypeDef status = __spi_is_idle(spi, Timeout, tickstart);
    if (status != HAL_OK) {
        hqspi->ErrorCode = HAL_SPI_ERROR_FLAG;
    }
    hqspi->State = HAL_SPI_STATE_READY;
    VSF_STHAL_UNLOCK(hqspi);

    return status;
}

static HAL_StatusTypeDef __qspi_prepare(QSPI_HandleTypeDef   *hqspi,
                                        HAL_QSPI_StateTypeDef state,
                                        bool is_transmit, uint8_t *pTxBuffPtr,
                                        uint16_t TxXferSize,
                                        uint8_t *pRxBuffPtr,
                                        uint16_t RxXferSize)
{
    VSF_STHAL_LOCK(hqspi);

    if (hqspi->State != HAL_QSPI_STATE_READY) {
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_BUSY;
    }

    if (hqspi->NbData == 0) {
        hqspi->ErrorCode |= HAL_QSPI_ERROR_INVALID_PARAM;
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_ERROR;
    }

    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

    if (hqspi->DummyCycles > 0) {
        uint32_t trans_mode;
        if (is_transmit) {
            trans_mode = VSF_SPI_CTRL_QSPI_TRANSFER_MODE_DUMMY_WRITE;
        } else {
            trans_mode = VSF_SPI_CTRL_QSPI_TRANSFER_MODE_DUMMY_READ;
        }
        vsf_err_t err =
            vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_TRANSFER_SET_MODE, &trans_mode);
        VSF_ASSERT(err == VSF_ERR_NONE);
    }

    hqspi->ErrorCode   = HAL_QSPI_ERROR_NONE;
    hqspi->State       = state;
    hqspi->pTxBuffPtr  = pTxBuffPtr;
    hqspi->TxXferSize  = TxXferSize;
    hqspi->TxXferCount = TxXferSize;
    hqspi->pRxBuffPtr  = pRxBuffPtr;
    hqspi->RxXferSize  = RxXferSize;
    hqspi->RxXferCount = RxXferSize;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_QSPI_Transmit(QSPI_HandleTypeDef *hqspi, uint8_t *pData,
                                    uint32_t Timeout)
{
    if (hqspi == NULL) {
        return HAL_ERROR;
    }

    if (pData == NULL) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status =
        __qspi_prepare(hqspi, HAL_QSPI_STATE_BUSY_INDIRECT_TX, true, pData,
                       hqspi->NbData, NULL, 0);
    if (status != HAL_OK) {
        return status;
    }

    return __spi_transfer_with_timeout(hqspi, Timeout);
}

HAL_StatusTypeDef HAL_QSPI_Receive(QSPI_HandleTypeDef *hqspi, uint8_t *pData,
                                   uint32_t Timeout)
{
    if (hqspi == NULL) {
        return HAL_ERROR;
    }
    if (pData == NULL) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status =
        __qspi_prepare(hqspi, HAL_QSPI_STATE_BUSY_INDIRECT_RX, false, NULL, 0,
                       pData, hqspi->NbData);
    if (status != HAL_OK) {
        return status;
    }

    return __spi_transfer_with_timeout(hqspi, Timeout);
}

HAL_StatusTypeDef HAL_QSPI_Transmit_IT(QSPI_HandleTypeDef *hqspi,
                                       uint8_t            *pData)
{
    if (hqspi == NULL) {
        return HAL_ERROR;
    }
    if (pData == NULL) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status =
        __qspi_prepare(hqspi, HAL_QSPI_STATE_BUSY_INDIRECT_TX, true, pData,
                       hqspi->NbData, NULL, 0);
    if (status != HAL_OK) {
        return status;
    }

    __spi_fifo_transfer_continue(hqspi);

    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    vsf_qspi_irq_enable(spi, VSF_SPI_IRQ_MASK_TX);

    VSF_STHAL_UNLOCK(hqspi);

    return status;
}

HAL_StatusTypeDef HAL_QSPI_Receive_IT(QSPI_HandleTypeDef *hqspi, uint8_t *pData)
{

    if (hqspi == NULL) {
        return HAL_ERROR;
    }
    if (pData == NULL) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status =
        __qspi_prepare(hqspi, HAL_QSPI_STATE_BUSY_INDIRECT_RX, false, NULL, 0,
                       pData, hqspi->NbData);
    if (status != HAL_OK) {
        return status;
    }

    __spi_fifo_transfer_continue(hqspi);

    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    vsf_qspi_irq_enable(spi, VSF_SPI_IRQ_MASK_RX | VSF_SPI_IRQ_MASK_ERR);

    VSF_STHAL_UNLOCK(hqspi);

    return status;
}

HAL_StatusTypeDef HAL_QSPI_Transmit_DMA(QSPI_HandleTypeDef *hqspi,
                                        uint8_t            *pData)
{
    if (hqspi == NULL) {
        return HAL_ERROR;
    }
    if (pData == NULL) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status =
        __qspi_prepare(hqspi, HAL_QSPI_STATE_BUSY_INDIRECT_TX, true, pData,
                       hqspi->NbData, NULL, 0);
    if (status != HAL_OK) {
        return status;
    }

    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

    vsf_qspi_irq_enable(spi, VSF_SPI_IRQ_MASK_TX_CPL);
    vsf_err_t err =
        vsf_qspi_request_tx(spi, (void *)pData, hqspi->NbData);
    if (err != VSF_ERR_NONE) {
        hqspi->ErrorCode |= HAL_QSPI_ERROR_TRANSFER;
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hqspi);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_QSPI_Receive_DMA(QSPI_HandleTypeDef *hqspi,
                                       uint8_t            *pData)
{
    if (hqspi == NULL) {
        return HAL_ERROR;
    }
    if (pData == NULL) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status =
        __qspi_prepare(hqspi, HAL_QSPI_STATE_BUSY_INDIRECT_RX, false, NULL, 0,
                       pData, hqspi->NbData);
    if (status != HAL_OK) {
        return status;
    }

    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

    vsf_qspi_irq_enable(spi, VSF_SPI_IRQ_MASK_CPL);
    vsf_err_t err =
        vsf_qspi_request_rx(spi, (void *)pData, hqspi->NbData);
    if (err != VSF_ERR_NONE) {
        hqspi->ErrorCode |= HAL_QSPI_ERROR_TRANSFER;
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_ERROR;
    }

    VSF_STHAL_UNLOCK(hqspi);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_QSPI_AutoPolling(QSPI_HandleTypeDef      *hqspi,
                                       QSPI_CommandTypeDef     *cmd,
                                       QSPI_AutoPollingTypeDef *cfg,
                                       uint32_t                 Timeout)
{
#   ifdef VSF_SPI_CTRL_QSPI_QSPI_AUTO_POLLING
    HAL_StatusTypeDef status;
    uint32_t          tickstart = HAL_GetTick();

    VSF_STHAL_ASSERT(IS_QSPI_INSTRUCTION_MODE(cmd->InstructionMode));
    if (cmd->InstructionMode != QSPI_INSTRUCTION_NONE) {
        VSF_STHAL_ASSERT(IS_QSPI_INSTRUCTION(cmd->Instruction));
    }

    VSF_STHAL_ASSERT(IS_QSPI_ADDRESS_MODE(cmd->AddressMode));
    if (cmd->AddressMode != QSPI_ADDRESS_NONE) {
        VSF_STHAL_ASSERT(IS_QSPI_ADDRESS_SIZE(cmd->AddressSize));
    }

    VSF_STHAL_ASSERT(IS_QSPI_ALTERNATE_BYTES_MODE(cmd->AlternateByteMode));
    if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE) {
        VSF_STHAL_ASSERT(IS_QSPI_ALTERNATE_BYTES_SIZE(cmd->AlternateBytesSize));
    }

    VSF_STHAL_ASSERT(IS_QSPI_DUMMY_CYCLES(cmd->DummyCycles));
    VSF_STHAL_ASSERT(IS_QSPI_DATA_MODE(cmd->DataMode));
    VSF_STHAL_ASSERT(IS_QSPI_DDR_MODE(cmd->DdrMode));
    VSF_STHAL_ASSERT(IS_QSPI_DDR_HHC(cmd->DdrHoldHalfCycle));
    VSF_STHAL_ASSERT(IS_QSPI_SIOO_MODE(cmd->SIOOMode));
    VSF_STHAL_ASSERT(IS_QSPI_INTERVAL(cfg->Interval));
    VSF_STHAL_ASSERT(IS_QSPI_STATUS_BYTES_SIZE(cfg->StatusBytesSize));
    VSF_STHAL_ASSERT(IS_QSPI_MATCH_MODE(cfg->MatchMode));

    VSF_STHAL_LOCK(hqspi);

    if (hqspi->State != HAL_QSPI_STATE_READY) {
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_BUSY;
    }

    hqspi->ErrorCode = HAL_QSPI_ERROR_NONE;
    hqspi->State     = HAL_QSPI_STATE_BUSY_AUTO_POLLING;

    // TODO: add auto polling cfg
    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);
    vsf_err_t err =
        vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_QSPI_AUTO_POLLING, NULL);

    hqspi->State = HAL_QSPI_STATE_READY;
    VSF_STHAL_UNLOCK(hqspi);

    return HAL_OK;
#   else
    return HAL_ERROR;
#   endif
}

HAL_StatusTypeDef HAL_QSPI_AutoPolling_IT(QSPI_HandleTypeDef      *hqspi,
                                          QSPI_CommandTypeDef     *cmd,
                                          QSPI_AutoPollingTypeDef *cfg)
{
#   ifdef VSF_SPI_CTRL_QSPI_QSPI_AUTO_POLLING
    VSF_STHAL_ASSERT(IS_QSPI_INSTRUCTION_MODE(cmd->InstructionMode));
    if (cmd->InstructionMode != QSPI_INSTRUCTION_NONE) {
        VSF_STHAL_ASSERT(IS_QSPI_INSTRUCTION(cmd->Instruction));
    }

    VSF_STHAL_ASSERT(IS_QSPI_ADDRESS_MODE(cmd->AddressMode));
    if (cmd->AddressMode != QSPI_ADDRESS_NONE) {
        VSF_STHAL_ASSERT(IS_QSPI_ADDRESS_SIZE(cmd->AddressSize));
    }

    VSF_STHAL_ASSERT(IS_QSPI_ALTERNATE_BYTES_MODE(cmd->AlternateByteMode));
    if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE) {
        VSF_STHAL_ASSERT(IS_QSPI_ALTERNATE_BYTES_SIZE(cmd->AlternateBytesSize));
    }

    VSF_STHAL_ASSERT(IS_QSPI_DUMMY_CYCLES(cmd->DummyCycles));
    VSF_STHAL_ASSERT(IS_QSPI_DATA_MODE(cmd->DataMode));
    VSF_STHAL_ASSERT(IS_QSPI_DDR_MODE(cmd->DdrMode));
    VSF_STHAL_ASSERT(IS_QSPI_DDR_HHC(cmd->DdrHoldHalfCycle));
    VSF_STHAL_ASSERT(IS_QSPI_SIOO_MODE(cmd->SIOOMode));
    VSF_STHAL_ASSERT(IS_QSPI_INTERVAL(cfg->Interval));
    VSF_STHAL_ASSERT(IS_QSPI_STATUS_BYTES_SIZE(cfg->StatusBytesSize));
    VSF_STHAL_ASSERT(IS_QSPI_MATCH_MODE(cfg->MatchMode));
    VSF_STHAL_ASSERT(IS_QSPI_AUTOMATIC_STOP(cfg->AutomaticStop));
    VSF_STHAL_LOCK(hqspi);

    if (hqspi->State != HAL_QSPI_STATE_READY) {
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_BUSY;
    }

    hqspi->ErrorCode = HAL_QSPI_ERROR_NONE;
    hqspi->State     = HAL_QSPI_STATE_BUSY_AUTO_POLLING;

    // TODO: add auto polling cfg
    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);
    vsf_err_t err =
        vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_QSPI_AUTO_POLLING, NULL);

    hqspi->State = HAL_QSPI_STATE_READY;
    VSF_STHAL_UNLOCK(hqspi);
    return HAL_OK;
#   else
    return HAL_ERROR;
#   endif
}

HAL_StatusTypeDef HAL_QSPI_MemoryMapped(QSPI_HandleTypeDef       *hqspi,
                                        QSPI_CommandTypeDef      *cmd,
                                        QSPI_MemoryMappedTypeDef *cfg)
{
#   ifdef VSF_SPI_CTRL_QSPI_QSPI_MEMORY_MAP_SET
    HAL_StatusTypeDef status;
    uint32_t          tickstart = HAL_GetTick();

    VSF_STHAL_ASSERT(IS_QSPI_INSTRUCTION_MODE(cmd->InstructionMode));
    if (cmd->InstructionMode != QSPI_INSTRUCTION_NONE) {
        VSF_STHAL_ASSERT(IS_QSPI_INSTRUCTION(cmd->Instruction));
    }

    VSF_STHAL_ASSERT(IS_QSPI_ADDRESS_MODE(cmd->AddressMode));
    if (cmd->AddressMode != QSPI_ADDRESS_NONE) {
        VSF_STHAL_ASSERT(IS_QSPI_ADDRESS_SIZE(cmd->AddressSize));
    }

    VSF_STHAL_ASSERT(IS_QSPI_ALTERNATE_BYTES_MODE(cmd->AlternateByteMode));
    if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE) {
        VSF_STHAL_ASSERT(IS_QSPI_ALTERNATE_BYTES_SIZE(cmd->AlternateBytesSize));
    }

    VSF_STHAL_ASSERT(IS_QSPI_DUMMY_CYCLES(cmd->DummyCycles));
    VSF_STHAL_ASSERT(IS_QSPI_DATA_MODE(cmd->DataMode));
    VSF_STHAL_ASSERT(IS_QSPI_DDR_MODE(cmd->DdrMode));
    VSF_STHAL_ASSERT(IS_QSPI_DDR_HHC(cmd->DdrHoldHalfCycle));
    VSF_STHAL_ASSERT(IS_QSPI_SIOO_MODE(cmd->SIOOMode));
    VSF_STHAL_ASSERT(IS_QSPI_TIMEOUT_ACTIVATION(cfg->TimeOutActivation));

    if (hqspi->State != HAL_QSPI_STATE_READY) {
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_BUSY;
    }

    // TODO: add memory map
    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);
    uint32_t *map = {
        // TODO
    };
    vsf_err_t err =
        vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_QSPI_MEMORY_MAP_SET, &map);

    hqspi->ErrorCode = HAL_QSPI_ERROR_NONE;
    hqspi->State     = HAL_QSPI_STATE_BUSY_AUTO_POLLING;

    hqspi->State = HAL_QSPI_STATE_READY;
    VSF_STHAL_UNLOCK(hqspi);
    return HAL_OK;
#   else
    return HAL_ERROR;
#   endif
}

HAL_QSPI_StateTypeDef HAL_QSPI_GetState(const QSPI_HandleTypeDef *hqspi)
{
    VSF_STHAL_ASSERT(hqspi != NULL);

    return hqspi->State;
}

uint32_t HAL_QSPI_GetError(const QSPI_HandleTypeDef *hqspi)
{
    VSF_STHAL_ASSERT(hqspi != NULL);

    return hqspi->ErrorCode;
}

HAL_StatusTypeDef HAL_QSPI_Abort(QSPI_HandleTypeDef *hqspi)
{
    HAL_StatusTypeDef status;
    VSF_STHAL_ASSERT(hqspi != NULL);
    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

    status = HAL_OK;

    vsf_qspi_irq_disable(spi, VSF_SPI_IRQ_ALL_BITS_MASK);

    vsf_err_t err = vsf_qspi_cancel_transfer(spi);
    if (err < VSF_ERR_NONE) {
        return HAL_ERROR;
    }

    hqspi->RxXferCount = 0U;
    hqspi->TxXferCount = 0U;
    if (hqspi->ErrorCode == HAL_QSPI_ERROR_DMA) {
        status = HAL_ERROR;
    } else {
        hqspi->ErrorCode = HAL_QSPI_ERROR_NONE;
    }
    hqspi->State = HAL_QSPI_STATE_READY;

    return status;
}

HAL_StatusTypeDef HAL_QSPI_Abort_IT(QSPI_HandleTypeDef *hqspi)
{
    VSF_STHAL_ASSERT(hqspi != NULL);
    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);

    vsf_qspi_irq_disable(spi, VSF_SPI_IRQ_ALL_BITS_MASK);

    vsf_err_t err = vsf_qspi_cancel_transfer(spi);
    if (err < VSF_ERR_NONE) {
        return HAL_ERROR;
    } else if (err == VSF_ERR_NOT_READY) {
        return HAL_OK;
    }

    hqspi->RxXferCount       = 0U;
    hqspi->TxXferCount       = 0U;
    HAL_StatusTypeDef status = HAL_OK;
    if (hqspi->ErrorCode == HAL_QSPI_ERROR_DMA) {
        status = HAL_ERROR;
    } else {
        hqspi->ErrorCode = HAL_QSPI_ERROR_NONE;
    }
    hqspi->State = HAL_QSPI_STATE_READY;

#   if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
    hqspi->AbortCpltCallback(hqspi);
#   else
    HAL_QSPI_AbortCpltCallback(hqspi);
#   endif

    return HAL_OK;
}

void HAL_QSPI_SetTimeout(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
    VSF_STHAL_ASSERT(hqspi != NULL);

    hqspi->Timeout = Timeout;
}

HAL_StatusTypeDef HAL_QSPI_SetFifoThreshold(QSPI_HandleTypeDef *hqspi,
                                            uint32_t            Threshold)
{
    HAL_StatusTypeDef status = HAL_OK;

    VSF_STHAL_LOCK(hqspi);

    if (hqspi->State != HAL_QSPI_STATE_READY) {
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_BUSY;
    }

#   ifdef VSF_SPI_CTRL_QSPI_FIFO_THRESHOLD_SET
    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);
    vsf_err_t err = vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_FIFO_THRESHOLD_SET,
                                 &hqspi->Init.FifoThreshold);
    if (err != VSF_ERR_NONE) {
        status = HAL_ERROR;
    } else {
        hqspi->Init.FifoThreshold = HAL_QSPI_GetFifoThreshold(hqspi);
    }
#   else

    VSF_UNUSED_PARAM(hqspi);
    VSF_UNUSED_PARAM(Threshold);
    VSF_STHAL_ASSERT(false);
    status = HAL_ERROR;
#   endif

    VSF_STHAL_UNLOCK(hqspi);

    return status;
}

uint32_t HAL_QSPI_GetFifoThreshold(const QSPI_HandleTypeDef *hqspi)
{
#   ifdef VSF_SPI_CTRL_QSPI_FIFO_THRESHOLD_GET
    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);
    uint32_t  threshold = 0;
    vsf_err_t err =
        vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_FIFO_THRESHOLD_GET, &threshold);
    if (err != VSF_ERR_NONE) {
        return 0;
    }
    return threshold;
#   else
    VSF_UNUSED_PARAM(hqspi);
    VSF_STHAL_ASSERT(false);
    return 0;
#   endif
}

HAL_StatusTypeDef HAL_QSPI_SetFlashID(QSPI_HandleTypeDef *hqspi,
                                      uint32_t            FlashID)
{
    HAL_StatusTypeDef status = HAL_OK;
    VSF_STHAL_ASSERT(IS_QSPI_FLASH_ID(FlashID));

    VSF_STHAL_LOCK(hqspi);

    if (hqspi->State != HAL_QSPI_STATE_READY) {
        VSF_STHAL_UNLOCK(hqspi);
        return HAL_BUSY;
    }

#   ifdef VSF_SPI_CTRL_QSPI_QSPI_FLASH_ID_SET
    vsf_qspi_t *spi = (vsf_qspi_t *)hqspi->Instance;
    VSF_STHAL_ASSERT(spi != NULL);
    vsf_err_t err =
        vsf_qspi_ctrl(spi, VSF_SPI_CTRL_QSPI_QSPI_FLASH_ID_SET, &FlashID);
    if (err != VSF_ERR_NONE) {
        status = HAL_ERROR;
    } else {
        hqspi->Init.FlashID = FlashID;
    }
#   else
    VSF_UNUSED_PARAM(hqspi);
    VSF_UNUSED_PARAM(FlashID);
    VSF_STHAL_ASSERT(false);
    status = HAL_ERROR;
#   endif

    VSF_STHAL_UNLOCK(hqspi);
    return status;
}

void HAL_QSPI_IRQHandler(QSPI_HandleTypeDef *hqspi)
{
    VSF_STHAL_ASSERT(hqspi != NULL);
    VSF_UNUSED_PARAM(hqspi);
}

#endif
