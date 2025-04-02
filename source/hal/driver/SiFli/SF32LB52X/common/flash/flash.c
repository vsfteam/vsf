/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#include "../driver.h"

#if VSF_HAL_USE_FLASH == ENABLED

#include "service/vsf_service.h"

#include "bf0_hal.h"
#include "sifli_bbm.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_FLASH_CFG_MULTI_CLASS
#   define VSF_HW_FLASH_CFG_MULTI_CLASS             VSF_FLASH_CFG_MULTI_CLASS
#endif

#define __VSF_HW_FLASH_CFG_PROTECT                  interrupt
#define __VSF_HW_FLASH_USE_DMA                      DISABLED

// Boot source
#define BOOT_FROM_SIP_PUYA                          1
#define BOOT_FROM_SIP_GD                            2
#define BOOT_FROM_NOR                               3
#define BOOT_FROM_NAND                              4
#define BOOT_FROM_SD                                5
#define BOOT_FROM_EMMC                              6

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_hw_flash_protect                                                  \
    vsf_protect(__VSF_HW_FLASH_CFG_PROTECT)

#define __vsf_hw_flash_unprotect                                                \
    vsf_unprotect(__VSF_HW_FLASH_CFG_PROTECT)

/*============================ TYPES =========================================*/

typedef struct vsf_hw_flash_t {
#if VSF_HW_FLASH_CFG_MULTI_CLASS == ENABLED
    vsf_flash_t vsf_flash;
#endif
    vsf_flash_cfg_t cfg;
    bool is_enabled;
    vsf_flash_irq_mask_t irq_mask;

    QSPI_FLASH_CTX_T flash_ctx;
#if __VSF_HW_FLASH_USE_DMA == ENABLED
    DMA_HandleTypeDef flash_dma_handle;
#endif
} vsf_hw_flash_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern uint32_t flash_get_freq(int clk_module, uint16_t clk_div, uint8_t hcpu);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_flash_init(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(hw_flash_ptr != NULL);

    if (cfg_ptr != NULL) {
        hw_flash_ptr->cfg = *cfg_ptr;
    }

    memset(&hw_flash_ptr->flash_ctx, 0, sizeof(hw_flash_ptr->flash_ctx));
#if __VSF_HW_FLASH_USE_DMA == ENABLED
    memset(&hw_flash_ptr->flash_dma_handle, 0, sizeof(hw_flash_ptr->flash_dma_handle));
#endif

    int flash_mod;
    qspi_configure_t qspi_cfg;
#if __VSF_HW_FLASH_USE_DMA == ENABLED
    struct dma_config flash_dma = { 0 };
#endif

    switch (HAL_Get_backup(RTC_BACKUP_BOOTOPT)) {
    case BOOT_FROM_SIP_PUYA:
    case BOOT_FROM_SIP_GD:
        qspi_cfg.Instance = FLASH1;
        qspi_cfg.line = 2;
        qspi_cfg.base = FLASH_BASE_ADDR;
        qspi_cfg.SpiMode = SPI_MODE_NOR;
        qspi_cfg.msize = 4;

#if __VSF_HW_FLASH_USE_DMA == ENABLED
        flash_dma.Instance = DMA1_Channel1;
        flash_dma.dma_irq_prio = 0;
        flash_dma.dma_irq = DMAC1_CH1_IRQn;
        flash_dma.request = DMA_REQUEST_0;
#endif

        flash_mod = RCC_CLK_MOD_FLASH1;
        break;
#ifdef HAL_USE_NAND
    case BOOT_FROM_NAND:
        qspi_cfg.SpiMode = SPI_MODE_NAND;
        goto __flash_init;
#endif
    case BOOT_FROM_NOR:
        qspi_cfg.SpiMode = SPI_MODE_NOR;
#ifdef HAL_USE_NAND
    __flash_init:
#endif
        qspi_cfg.Instance = FLASH2;
        qspi_cfg.line = 2;
        qspi_cfg.base = FLASH2_BASE_ADDR;
        qspi_cfg.msize = 32;

#if __VSF_HW_FLASH_USE_DMA == ENABLED
        flash_dma.Instance = DMA1_Channel2;
        flash_dma.dma_irq_prio = 0;
        flash_dma.dma_irq = DMAC1_CH2_IRQn;
        flash_dma.request = DMA_REQUEST_1;
#endif

        flash_mod = RCC_CLK_MOD_FLASH2;
        break;
    case BOOT_FROM_SD:
    case BOOT_FROM_EMMC:
#ifndef HAL_USE_NAND
    case BOOT_FROM_NAND:
#endif
    default:
        return VSF_ERR_NOT_SUPPORT;
    }

    uint32_t div = HAL_RCC_HCPU_GetDLL2Freq() / (576 * 100 * 1000);
    hw_flash_ptr->flash_ctx.handle.freq = flash_get_freq(flash_mod, div, 1);
    hw_flash_ptr->flash_ctx.handle.buf_mode = 0;

    HAL_RCC_HCPU_ClockSelect(flash_mod, RCC_CLK_FLASH_DLL2);

    vsf_protect_t org = __vsf_hw_flash_protect();
        HAL_StatusTypeDef res = HAL_FLASH_Init(&hw_flash_ptr->flash_ctx, &qspi_cfg,
#if __VSF_HW_FLASH_USE_DMA == ENABLED
                &hw_flash_ptr->flash_dma_handle, &flash_dma,
#else
                NULL, NULL,
#endif
                div);
    __vsf_hw_flash_unprotect(org);

    if (res == HAL_OK) {
        vsf_trace_debug("flash: devid 0x%08X, %d@0x%08X %d" VSF_TRACE_CFG_LINEEND,
            hw_flash_ptr->flash_ctx.dev_id,
            hw_flash_ptr->flash_ctx.handle.size,
            hw_flash_ptr->flash_ctx.handle.base,
            hw_flash_ptr->flash_ctx.handle.ctable->erase_base_size);
    }

    return (res == HAL_OK) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

void vsf_hw_flash_fini(vsf_hw_flash_t *hw_flash_ptr)
{
    VSF_HAL_ASSERT(hw_flash_ptr != NULL);
}

fsm_rt_t vsf_hw_flash_enable(vsf_hw_flash_t *hw_flash_ptr)
{
    VSF_HAL_ASSERT(hw_flash_ptr != NULL);

    hw_flash_ptr->is_enabled = true;

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_flash_disable(vsf_hw_flash_t *hw_flash_ptr)
{
    VSF_HAL_ASSERT(hw_flash_ptr != NULL);

    hw_flash_ptr->is_enabled = false;

    return fsm_rt_cpl;
}

void vsf_hw_flash_irq_enable(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(hw_flash_ptr != NULL);
    VSF_HAL_ASSERT((irq_mask & ~VSF_FLASH_IRQ_ALL_BITS_MASK) == 0);

    hw_flash_ptr->irq_mask |= irq_mask;
}

void vsf_hw_flash_irq_disable(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(hw_flash_ptr != NULL);
    VSF_HAL_ASSERT((irq_mask & ~VSF_FLASH_IRQ_ALL_BITS_MASK) == 0);

    hw_flash_ptr->irq_mask &= ~irq_mask;
}

vsf_err_t vsf_hw_flash_erase_multi_sector(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_size_t offset, vsf_flash_size_t size)
{
    int ret = -1;

    VSF_HAL_ASSERT(hw_flash_ptr != NULL);
    VSF_HAL_ASSERT(hw_flash_ptr->is_enabled);
    VSF_HAL_ASSERT(offset + size <= hw_flash_ptr->flash_ctx.handle.size);

    uint32_t block_size = QSPI_NOR_SECT_SIZE << hw_flash_ptr->flash_ctx.handle.dualFlash;

    vsf_flash_size_t cur_offset = offset, remain_size = size;
    vsf_protect_t org = __vsf_hw_flash_protect();
        if (hw_flash_ptr->flash_ctx.flash_mode == 0) {
            // nor
            VSF_HAL_ASSERT(IS_ALIGNED(block_size, remain_size));
            VSF_HAL_ASSERT(IS_ALIGNED(block_size, cur_offset));

            if ((0 == cur_offset) && (remain_size == hw_flash_ptr->flash_ctx.handle.size)) {
                ret = HAL_QSPIEX_CHIP_ERASE(&hw_flash_ptr->flash_ctx.handle);
            } else {
                uint32_t block64_size = QSPI_NOR_BLK64_SIZE << hw_flash_ptr->flash_ctx.handle.dualFlash;
                if (IS_ALIGNED(block64_size, cur_offset) && (remain_size >= block64_size)) {
                    while (remain_size >= block64_size) {
                        ret = HAL_QSPIEX_BLK64_ERASE(&hw_flash_ptr->flash_ctx.handle, cur_offset);
                        if (ret != 0) { break; }
                        remain_size -= block64_size;
                        cur_offset += block64_size;
                    }
                }
                while (remain_size > 0) {
                    ret = HAL_QSPIEX_SECT_ERASE(&hw_flash_ptr->flash_ctx.handle, cur_offset);
                    if (ret != 0) { break; }
                    remain_size -= block_size;
                    cur_offset += block_size;
                }
            }
        } else if (hw_flash_ptr->flash_ctx.flash_mode == 1) {
            // nand, TODO:
        } else {
            VSF_HAL_ASSERT(false);
        }
    __vsf_hw_flash_unprotect(org);

    offset += hw_flash_ptr->flash_ctx.handle.base;
    SCB_CleanInvalidateDCache_by_Addr((void *)offset, size);
    SCB_InvalidateICache_by_Addr((void *)offset, size);
    if (NULL != hw_flash_ptr->cfg.isr.handler_fn) {
        vsf_flash_irq_mask_t mask = (0 == ret) ? VSF_FLASH_IRQ_ERASE_MASK : VSF_FLASH_IRQ_ERASE_ERROR_MASK;
        if (hw_flash_ptr->irq_mask & mask) {
            hw_flash_ptr->cfg.isr.handler_fn(hw_flash_ptr->cfg.isr.target_ptr, (vsf_flash_t *)hw_flash_ptr, mask);
        }
    }

    return (0 == ret) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

vsf_err_t vsf_hw_flash_write_multi_sector(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_size_t offset, uint8_t* buffer, vsf_flash_size_t size)
{
    int ret = -1;

    VSF_HAL_ASSERT(hw_flash_ptr != NULL);
    VSF_HAL_ASSERT(hw_flash_ptr->is_enabled);
    VSF_HAL_ASSERT(offset + size <= hw_flash_ptr->flash_ctx.handle.size);

    uint32_t block_size = QSPI_NOR_PAGE_SIZE << hw_flash_ptr->flash_ctx.handle.dualFlash;

    vsf_flash_size_t cur_offset = offset, remain_size = size;
    vsf_protect_t org = __vsf_hw_flash_protect();
        if (hw_flash_ptr->flash_ctx.flash_mode == 0) {
            // nor
            VSF_HAL_ASSERT(IS_ALIGNED(block_size, remain_size));
            VSF_HAL_ASSERT(IS_ALIGNED(block_size, cur_offset));

            while (remain_size > 0) {
                ret = HAL_QSPIEX_WRITE_PAGE(&hw_flash_ptr->flash_ctx.handle, cur_offset, buffer, block_size);
                if (ret != block_size) { break; }
                remain_size -= block_size;
                cur_offset += block_size;
                buffer += block_size;
            }
        } else if (hw_flash_ptr->flash_ctx.flash_mode == 1) {
            // nand, TODO:
        } else {
            VSF_HAL_ASSERT(false);
        }
    __vsf_hw_flash_unprotect(org);

    offset += hw_flash_ptr->flash_ctx.handle.base;
    SCB_CleanInvalidateDCache_by_Addr((void *)offset, size);
    SCB_InvalidateICache_by_Addr((void *)offset, size);
    if (NULL != hw_flash_ptr->cfg.isr.handler_fn) {
        vsf_flash_irq_mask_t mask = (block_size == ret) ? VSF_FLASH_IRQ_WRITE_MASK : VSF_FLASH_IRQ_WRITE_ERROR_MASK;
        if (hw_flash_ptr->irq_mask & mask) {
            hw_flash_ptr->cfg.isr.handler_fn(hw_flash_ptr->cfg.isr.target_ptr, (vsf_flash_t *)hw_flash_ptr, mask);
        }
    }

    return (block_size == ret) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

vsf_err_t vsf_hw_flash_read_multi_sector(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_size_t offset, uint8_t* buffer, vsf_flash_size_t size)
{
    VSF_HAL_ASSERT(hw_flash_ptr != NULL);
    VSF_HAL_ASSERT(NULL != buffer);
    VSF_HAL_ASSERT(hw_flash_ptr->is_enabled);
    VSF_HAL_ASSERT(offset + size <= hw_flash_ptr->flash_ctx.handle.size);

    memcpy(buffer, (void *)(hw_flash_ptr->flash_ctx.handle.base + offset), size);

    if ((NULL != hw_flash_ptr->cfg.isr.handler_fn) && (hw_flash_ptr->irq_mask & VSF_FLASH_IRQ_READ_MASK)) {
        hw_flash_ptr->cfg.isr.handler_fn(hw_flash_ptr->cfg.isr.target_ptr,
                (vsf_flash_t *)hw_flash_ptr, VSF_FLASH_IRQ_READ_MASK);
    }

    return VSF_ERR_NONE;
}

vsf_flash_status_t vsf_hw_flash_status(vsf_hw_flash_t *hw_flash_ptr)
{
    vsf_flash_status_t flash_status = {
        .is_busy          = 0,
    };

    return flash_status;
}

vsf_flash_capability_t vsf_hw_flash_capability(vsf_hw_flash_t *hw_flash_ptr)
{
    vsf_flash_capability_t flash_capability = {
        .irq_mask              = VSF_FLASH_IRQ_ALL_BITS_MASK,
        .base_address          = hw_flash_ptr->flash_ctx.handle.base,
        .max_size              = hw_flash_ptr->flash_ctx.handle.size,
        .none_sector_aligned_write = 0,
        .none_sector_aligned_read  = 1,
    };

    if (hw_flash_ptr->flash_ctx.flash_mode == 0) {
        // nor
        flash_capability.erase_sector_size = QSPI_NOR_SECT_SIZE;
        flash_capability.write_sector_size = QSPI_NOR_PAGE_SIZE;
    } else if (hw_flash_ptr->flash_ctx.flash_mode == 1) {
        // nand, TODO:
    } else {
        VSF_HAL_ASSERT(false);
    }

    return flash_capability;
}

/*============================ INCLUDES ======================================*/

// only define in source file
#define VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#define VSF_FLASH_CFG_ERASE_ALL_TEMPLATE            ENABLED
#define VSF_FLASH_CFG_ERASE_ONE_SECTOR_TEMPLATE     ENABLED
#define VSF_FLASH_CFG_WRITE_ONE_SECTOR_TEMPLATE     ENABLED
#define VSF_FLASH_CFG_READ_ONE_SECTOR_TEMPLATE      ENABLED
#define VSF_FLASH_CFG_IMP_PREFIX                    vsf_hw
#define VSF_FLASH_CFG_IMP_UPCASE_PREFIX             VSF_HW
#define VSF_FLASH_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    vsf_hw_flash_t vsf_hw_flash ## __IDX = {                                    \
        .cfg = {                                                                \
            .isr = {                                                            \
                .handler_fn = NULL,                                             \
                .target_ptr = NULL,                                             \
            },                                                                  \
        },                                                                      \
        .is_enabled = false,                                                    \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/flash/flash_template.inc"

#endif      // VSF_HAL_USE_FLASH
