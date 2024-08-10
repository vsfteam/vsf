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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_FLASH == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${FLASH_IP}_FLASH_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${FLASH_IP}_FLASH_CLASS_INHERIT__ before including vsf_hal.h
 */

#include "hal/vsf_hal.h"

#include "../vendor/Inc/stm32h7xx.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_FLASH_PROG_WORD_CNT
#   error VSF_HW_FLASH_PROG_WORD_CNT MUST be defined in device.h
#endif

/*\note VSF_HW_FLASH_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${FLASH_IP}_FLASH_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_FLASH_CFG_MULTI_CLASS
#   define VSF_HW_FLASH_CFG_MULTI_CLASS         VSF_FLASH_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_FLASH_CFG_IMP_PREFIX                vsf_hw
#define VSF_FLASH_CFG_IMP_UPCASE_PREFIX         VSF_HW
// HW end

#define VSF_HW_FLASH_SR_IRQ_MASK                0x0FEF0000

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct {
    __IO uint32_t KEYR;
    uint32_t      RESERVED0;
    __IO uint32_t CR;
    __IO uint32_t SR;
    __IO uint32_t CCR;
    uint32_t      RESERVED1[4];
    __IO uint32_t PRAR_CUR;
    __IO uint32_t PRAR_PRG;
    __IO uint32_t SCAR_CUR;
    __IO uint32_t SCAR_PRG;
    __IO uint32_t WPSN_CUR;
    __IO uint32_t WPSN_PRG;
    uint32_t      RESERVED2[4];
    __IO uint32_t CRCCR;
    __IO uint32_t CRCSADD;
    __IO uint32_t CRCEADD;
    __IO uint32_t CRCDATA;
    __IO uint32_t ECC_FA;
} FLASH_BANK_TypeDef;

// HW
typedef struct VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) {
#if VSF_HW_FLASH_CFG_MULTI_CLASS == ENABLED
    vsf_flash_t             vsf_flash;
#endif
    uint32_t                addr;
    FLASH_BANK_TypeDef      *reg;

    vsf_flash_irq_mask_t    irq_mask;
    bool                    is_multi;
    uint8_t                 erase0_write1;
    vsf_flash_isr_t         isr;

    vsf_flash_size_t        offset_orig;
    vsf_flash_size_t        size_orig;
    vsf_flash_size_t        offset_cur;
    vsf_flash_size_t        size_cur;
    uint8_t                 *buffer;
} VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t);
// HW end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw flash only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

static void VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_lock)(
    FLASH_BANK_TypeDef *reg
) {
    reg->CR |= FLASH_CR_LOCK;
}

static void VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_unlock)(
    FLASH_BANK_TypeDef *reg
) {
    reg->KEYR = 0x45670123U;
    reg->KEYR = 0xCDEF89ABU;
}

// HW
vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_init)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    // configure according to cfg_ptr
    flash_ptr->isr = cfg_ptr->isr;
    if (flash_ptr->isr.handler_fn != NULL) {
        NVIC_SetPriority(VSF_HW_FLASH_IRQN, (uint32_t)flash_ptr->isr.prio);
        NVIC_EnableIRQ(VSF_HW_FLASH_IRQN);
    } else {
        vsf_trace_warning("gd32h7xx_flash: irq not enabled, erase/write operation will not complete!!!");
        NVIC_DisableIRQ(VSF_HW_FLASH_IRQN);
    }
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_fini)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
}

fsm_rt_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_enable)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_disable)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_enable)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);

    FLASH_BANK_TypeDef *reg = flash_ptr->reg;
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_unlock)(reg);

    flash_ptr->irq_mask = irq_mask;
    irq_mask &= __VSF_HW_FLASH_IRQ_MASK;
    if (irq_mask & VSF_FLASH_IRQ_WRITE_MASK) {
        irq_mask &= ~VSF_FLASH_IRQ_WRITE_MASK;
        irq_mask |= FLASH_CR_EOPIE;
    }
    reg->CR |= irq_mask;

    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_lock)(reg);
}

void VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_disable)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);

    FLASH_BANK_TypeDef *reg = flash_ptr->reg;
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_unlock)(reg);

    flash_ptr->irq_mask &= ~irq_mask;
    irq_mask &= __VSF_HW_FLASH_IRQ_MASK;
    if (irq_mask & VSF_FLASH_IRQ_WRITE_MASK) {
        irq_mask &= ~VSF_FLASH_IRQ_WRITE_MASK;
        irq_mask |= FLASH_CR_EOPIE;
    }
    reg->CR &= ~irq_mask;

    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_lock)(reg);
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_one_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(!(offset & ((1 << 17) - 1)));
    VSF_HAL_ASSERT((offset >> 17) <= 7);

    FLASH_BANK_TypeDef *reg = flash_ptr->reg;
    uint32_t sector = (offset >> 17) << FLASH_CR_SNB_Pos;
    flash_ptr->erase0_write1 = 0;

    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_unlock)(reg);
#if defined (FLASH_CR_PSIZE)
    reg->CR &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);
    reg->CR |= FLASH_CR_SER | FLASH_CR_PSIZE | FLASH_CR_START | FLASH_CR_LOCK | sector;
#else
    reg->CR &= ~(FLASH_CR_SNB);
    reg->CR |= FLASH_CR_SER | FLASH_CR_START | FLASH_CR_LOCK | sector;
#endif

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(!(offset & ((1 << 17) - 1)));
    VSF_HAL_ASSERT(!(size & ((1 << 17) - 1)));

    flash_ptr->offset_orig = flash_ptr->offset_cur = offset;
    flash_ptr->size_orig = flash_ptr->size_cur = size;
    return VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_one_sector)(
        flash_ptr, offset);
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    uint8_t *buffer,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(NULL != buffer);
    VSF_HAL_ASSERT(!(offset & ((VSF_HW_FLASH_PROG_WORD_CNT << 2) - 1)));
    VSF_HAL_ASSERT(!(size & ((VSF_HW_FLASH_PROG_WORD_CNT << 2) - 1)));

    FLASH_BANK_TypeDef *reg = flash_ptr->reg;
    flash_ptr->erase0_write1 = 1;
    flash_ptr->offset_orig = flash_ptr->offset_cur = offset;
    flash_ptr->size_orig = flash_ptr->size_cur = size;
    flash_ptr->buffer = buffer;
    uint32_t *buffer32 = (uint32_t *)buffer;
    uint32_t *addr = (uint32_t *)(flash_ptr->addr + offset);

    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_unlock)(reg);
        reg->CR |= FLASH_CR_PG;
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_lock)(reg);

    __ISB();
    __DSB();

    for (uint_fast8_t i = 0; i < VSF_HW_FLASH_PROG_WORD_CNT; i++, addr++, buffer32++) {
        *addr = *buffer32;
    }

    __ISB();
    __DSB();

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_one_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    uint8_t* buffer,
    vsf_flash_size_t size
) {
    return VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_multi_sector)(
        flash_ptr, offset, buffer, size);
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_read_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    uint8_t *buffer,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(NULL != buffer);

    memcpy(buffer, (void *)(flash_ptr->addr + offset), size);

    vsf_flash_isr_t *isr_ptr = &flash_ptr->isr;
    if (    (flash_ptr->irq_mask & VSF_FLASH_IRQ_READ_MASK)
        &&  (isr_ptr->handler_fn != NULL)) {
        flash_ptr->isr.handler_fn(isr_ptr->target_ptr, (vsf_flash_t *)flash_ptr, VSF_FLASH_IRQ_READ_MASK);
    }
    return VSF_ERR_NONE;
}

vsf_flash_status_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_status)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    return (vsf_flash_status_t) {
        .is_busy          = 0,
    };
}

vsf_flash_capability_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_capability)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    return (vsf_flash_capability_t) {
        .irq_mask                  = VSF_FLASH_IRQ_ALL_BITS_MASK,
        .base_address              = flash_ptr->addr,
        .max_size                  = 1024 * 1024,
        .erase_sector_size         = 128 * 1024,
        .write_sector_size         = 256,
        .none_sector_aligned_write = 0,
        .none_sector_aligned_read  = 1,
    };
}

static void VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_irqhandler)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    VSF_HAL_ASSERT(NULL != flash_ptr);

    FLASH_BANK_TypeDef *reg = flash_ptr->reg;
    vsf_flash_irq_mask_t irq_mask = reg->SR;
    vsf_flash_isr_t *isr_ptr = &flash_ptr->isr;

    reg->CCR |= irq_mask & VSF_HW_FLASH_SR_IRQ_MASK;
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_unlock)(reg);
        reg->CR &= ~(FLASH_CR_PG | FLASH_CR_SER);
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_lock)(reg);

    if (irq_mask & __VSF_HW_FLASH_ERR_IRQ_MASK) {
        flash_ptr->size_cur = 0;
    } else {
        // no error
        VSF_HAL_ASSERT(irq_mask & FLASH_SR_EOP);
        if (flash_ptr->erase0_write1) {
            VSF_HAL_ASSERT(flash_ptr->size_cur >= (VSF_HW_FLASH_PROG_WORD_CNT << 2));
            flash_ptr->size_cur -= VSF_HW_FLASH_PROG_WORD_CNT << 2;
            flash_ptr->offset_cur += VSF_HW_FLASH_PROG_WORD_CNT << 2;
            flash_ptr->buffer += VSF_HW_FLASH_PROG_WORD_CNT << 2;
            if (flash_ptr->size_cur > 0) {
                uint32_t *addr = (uint32_t *)(flash_ptr->addr + flash_ptr->offset_cur);

                VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_unlock)(reg);
                    reg->CR |= FLASH_CR_PG;
                VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_lock)(reg);

                __ISB();
                __DSB();

                uint32_t *buffer32 = (uint32_t *)flash_ptr->buffer;
                for (uint_fast8_t i = 0; i < VSF_HW_FLASH_PROG_WORD_CNT; i++, addr++, buffer32++) {
                    *addr = *buffer32;
                }

                __ISB();
                __DSB();

                return;
            }
            irq_mask = VSF_FLASH_IRQ_WRITE_MASK;
        } else {
            if (flash_ptr->size_cur > 0) {
                VSF_HAL_ASSERT(flash_ptr->size_cur >= (128 * 1024));
                flash_ptr->size_cur -= 128 * 1024;
                flash_ptr->offset_cur += 128 * 1024;
            }
            if (flash_ptr->size_cur > 0) {
                VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_one_sector)(
                    flash_ptr, flash_ptr->offset_cur);
                return;
            }
        }
    }

    SCB_CleanInvalidateDCache_by_Addr((void *)(flash_ptr->addr + flash_ptr->offset_orig), flash_ptr->size_orig);
    SCB_InvalidateICache_by_Addr((void *)(flash_ptr->addr + flash_ptr->offset_orig), flash_ptr->size_orig);
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_flash_t *)flash_ptr, irq_mask);
    }
}
// HW end

VSF_CAL_ROOT void VSF_MCONNECT(VSF_FLASH_CFG_IMP_UPCASE_PREFIX, _FLASH, _IRQHandler)(void)
{
    uintptr_t ctx = vsf_hal_irq_enter();

#if VSF_HW_FLASH_COUNT > 1
#   ifndef DUAL_BANK
#       error wrong flash configurations in device.h
#   endif
    if (FLASH->SR1 & (vsf_hw_flash0.irq_mask & VSF_HW_FLASH_SR_IRQ_MASK)) {
        VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_irqhandler)(
            &VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash0)
        );
    } else if (FLASH->SR2 & (vsf_hw_flash1.irq_mask & VSF_HW_FLASH_SR_IRQ_MASK)) {
        VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_irqhandler)(
            &VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash1)
        );
    }
#else
    VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_irqhandler)(
        &VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash0)
    );
#endif

    vsf_hal_irq_leave(ctx);
}

/*============================ INCLUDES ======================================*/

/*\note TODO: add comments about template configurations below:
 *  VSF_FLASH_CFG_ERASE_ALL_TEMPLATE
 *  VSF_FLASH_CFG_ERASE_ONE_SECTOR_TEMPLATE
 *  VSF_FLASH_CFG_ERASE_MULTI_SECTOR_TEMPLATE
 *  VSF_FLASH_CFG_WRITE_ONE_SECTOR_TEMPLATE
 *  VSF_FLASH_CFG_WRITE_MULTI_SECTOR_TEMPLATE
 *  VSF_FLASH_CFG_READ_ONE_SECTOR_TEMPLATE
 *  VSF_FLASH_CFG_READ_MULTI_SECTOR_TEMPLATE
 */

// only define in source file
#define VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#define VSF_FLASH_CFG_ERASE_ALL_TEMPLATE            ENABLED
#define VSF_FLASH_CFG_READ_ONE_SECTOR_TEMPLATE      ENABLED

#define VSF_FLASH_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t)                            \
        VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash ## __IDX) = {             \
        .addr               = VSF_MCONNECT(VSF_FLASH_CFG_IMP_UPCASE_PREFIX, _FLASH, __IDX, _ADDR),\
        .reg                = (FLASH_BANK_TypeDef *)VSF_MCONNECT(VSF_FLASH_CFG_IMP_UPCASE_PREFIX, _FLASH, __IDX, _REG_BASE),\
        __HAL_OP                                                                \
    };
#include "hal/driver/common/flash/flash_template.inc"

#endif      // VSF_HAL_USE_FLASH
