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

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) {
#if VSF_HW_FLASH_CFG_MULTI_CLASS == ENABLED
    vsf_flash_t                                 vsf_flash;
#endif
    flash_type                                  *reg;
    uint32_t                                    addr;
    vsf_flash_isr_t                             isr;
    uint8_t                                     *cur_buffer;
    vsf_flash_size_t                            size;
    vsf_flash_irq_mask_t                        irq_mask;
    vsf_flash_size_t                            cur_addr;
    vsf_flash_size_t                            cur_size;
    uint8_t                                     irqn;
    uint8_t                                     erase0_write1;
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

static void VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_lock)(
    flash_type *reg
) {
    reg->ctrl_bit.oplk = 1;
}

static void VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_unlock)(
    flash_type *reg
) {
    reg->unlock = FLASH_UNLOCK_KEY1;
    reg->unlock = FLASH_UNLOCK_KEY2;
}

// HW
vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_init)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    flash_ptr->isr = cfg_ptr->isr;
    if (cfg_ptr->isr.handler_fn != NULL) {
        NVIC_SetPriority(flash_ptr->irqn, (uint32_t)flash_ptr->isr.prio);
        NVIC_EnableIRQ(flash_ptr->irqn);
    } else {
        vsf_trace_warning("vsf_hw_flash: irq not enabled, erase/write operation will not complete!!!");
        NVIC_DisableIRQ(flash_ptr->irqn);
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
    flash_ptr->irq_mask |= irq_mask;
}

void VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_disable)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    flash_ptr->irq_mask &= ~irq_mask;
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    uint8_t sector_size_bits = (flash_ptr->size == (256 * 1024)) ? 11 : 10;
    VSF_HAL_ASSERT(!(offset & ((1 << sector_size_bits) - 1)));
    VSF_HAL_ASSERT(!(size & ((1 << sector_size_bits) - 1)));

    flash_type *reg = flash_ptr->reg;
    if (reg->sts_bit.obf) {
        return VSF_ERR_BUSY;
    }

    flash_ptr->cur_size = size;
    flash_ptr->cur_addr = offset;
    flash_ptr->erase0_write1 = 0;

    VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_unlock)(reg);
    if (flash_ptr->irq_mask & (VSF_FLASH_IRQ_ERASE_MASK | VSF_FLASH_IRQ_WRITE_MASK)) {
        reg->ctrl_bit.odfie = 1;
    }
    if (flash_ptr->irq_mask & (VSF_FLASH_IRQ_ERASE_MASK | VSF_FLASH_IRQ_WRITE_MASK)) {
        reg->ctrl_bit.errie = 1;
    }
    reg->addr = flash_ptr->cur_addr + flash_ptr->addr;
    reg->ctrl |= (1 << 1) | (1 << 6);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    uint8_t *buffer,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(NULL != buffer);
    VSF_HAL_ASSERT(!(offset & ((1 << 8) - 1)));
    VSF_HAL_ASSERT(!(size & ((1 << 8) - 1)));

    flash_type *reg = flash_ptr->reg;
    if (reg->sts_bit.obf) {
        return VSF_ERR_BUSY;
    }

    flash_ptr->cur_size = size;
    flash_ptr->cur_addr = offset;
    flash_ptr->cur_buffer = buffer;
    flash_ptr->erase0_write1 = 1;

    VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_unlock)(reg);
    if (flash_ptr->irq_mask & (VSF_FLASH_IRQ_ERASE_MASK | VSF_FLASH_IRQ_WRITE_MASK)) {
        reg->ctrl_bit.odfie = 1;
    }
    if (flash_ptr->irq_mask & (VSF_FLASH_IRQ_ERASE_MASK | VSF_FLASH_IRQ_WRITE_MASK)) {
        reg->ctrl_bit.errie = 1;
    }
    reg->ctrl_bit.fprgm = 1;
    *(uint32_t *)flash_ptr->cur_addr = get_unaligned_le32(flash_ptr->cur_buffer);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_read_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    uint8_t *buffer,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(NULL != buffer);

    memcpy(buffer, (const void *)(flash_ptr->addr + offset), size);
    vsf_flash_isr_t *isr_ptr = &flash_ptr->isr;
    if ((flash_ptr->irq_mask & VSF_FLASH_IRQ_READ_MASK) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_flash_t *)flash_ptr, VSF_FLASH_IRQ_READ_MASK);
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

vsf_flash_irq_mask_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_clear)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != flash_ptr);

    flash_type *reg = flash_ptr->reg;
    vsf_flash_irq_mask_t irq_mask_out = reg->sts;
    reg->sts = irq_mask_out;
    if (irq_mask_out & (1 << 5)) {
        irq_mask_out |= 1 << ((flash_ptr->erase0_write1 << 1) + 1);
    }
    irq_mask_out &= flash_ptr->irq_mask;
    return irq_mask_out;
}

static void VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_irqhandler)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    VSF_HAL_ASSERT(NULL != flash_ptr);

    flash_type *reg = flash_ptr->reg;
    vsf_flash_irq_mask_t irq_mask = VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_clear)(flash_ptr, VSF_FLASH_IRQ_ALL_BITS_MASK);

    reg->ctrl &= ~3;
    if (irq_mask & (VSF_FLASH_IRQ_ERASE_ERROR_MASK | VSF_FLASH_IRQ_WRITE_ERROR_MASK)) {
        flash_ptr->cur_size = 0;
    } else if (irq_mask & VSF_FLASH_IRQ_WRITE_MASK) {
        if (flash_ptr->cur_size > 0) {
            flash_ptr->cur_size -= 4;
            flash_ptr->cur_addr += 4;
            flash_ptr->cur_buffer += 4;
        }
        if (flash_ptr->cur_size > 0) {
            reg->ctrl_bit.fprgm = 1;
            *(uint32_t *)flash_ptr->cur_addr = get_unaligned_le32(flash_ptr->cur_buffer);
            return;
        }
    } else if (irq_mask & VSF_FLASH_IRQ_ERASE_MASK) {
        vsf_flash_size_t sector_size = flash_ptr->size == (256 * 1024) ? 2048 : 1024;
        if (flash_ptr->cur_size > 0) {
            flash_ptr->cur_size -= sector_size;
            flash_ptr->cur_addr += sector_size;
        }
        if (flash_ptr->cur_size > 0) {
            reg->addr = flash_ptr->cur_addr + flash_ptr->addr;
            reg->ctrl |= (1 << 1) | (1 << 6);
            return;
        }
    }

    reg->ctrl_bit.odfie = 0;
    reg->ctrl_bit.errie = 0;
    VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_lock)(reg);

    vsf_flash_isr_t *isr_ptr = &flash_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_flash_t *)flash_ptr, irq_mask);
    }
}

/*\note Implementation of APIs below is optional, because there is default implementation in flash_template.inc.
 *      VSF_FLASH_CFG_REIMPLEMENT_API_XXXX can be defined to ENABLED to re-write the default implementation for better performance.
 *
 *      The list of APIs and configuration:
 *      VSF_FLASH_CFG_REIMPLEMENT_API_GET_CONFIGURATION for flash_get_configuration.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 *      VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY for flash_capability.
 *          Default implementation will use macros below to initialize capability structure:
 *              VSF_FLASH_CFG_CAPABILITY_IRQ_MASK (default: VSF_FLASH_IRQ_ALL_BITS_MASK)
 *              VSF_FLASH_CFG_CAPABILITY_BASE_ADDRESS (default: 0)
 *              VSF_FLASH_CFG_CAPABILITY_MAX_SIZE (default: error if not defined)
 *              VSF_FLASH_CFG_CAPABILITY_ERASE_SECTORE_SIZE (default: 4096)
 *              VSF_FLASH_CFG_CAPABILITY_WRITE_SECTORE_SIZE (default: 512)
 *              VSF_FLASH_CFG_CAPABILITY_NONE_SECTOR_ALIGNED_WRITE (default: 0)
 *              VSF_FLASH_CFG_CAPABILITY_NONE_SECTOR_ALIGNED_READ (default: 1)
 */

vsf_flash_capability_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_capability)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    return (vsf_flash_capability_t) {
        .irq_mask                   = VSF_FLASH_IRQ_ALL_BITS_MASK,
        .base_address               = flash_ptr->addr,
        .max_size                   = flash_ptr->size,
        .erase_sector_size          = flash_ptr->size == (256 * 1024) ? 2048 : 1024,
        .write_sector_size          = 256,
        .none_sector_aligned_write  = 0,
        .none_sector_aligned_read   = 1,
    };
}
// HW end

/*============================ INCLUDES ======================================*/

/*\note TODO: add comments about template configurations below:
 *  VSF_FLASH_CFG_ERASE_ALL_TEMPLATE                implement erase_all by erase_multi_sector in template
 *  VSF_FLASH_CFG_ERASE_ONE_SECTOR_TEMPLATE         implement erase_one_sector by erase_multi_sector in template
 *  VSF_FLASH_CFG_WRITE_ONE_SECTOR_TEMPLATE         implement write_one_sector by write_multi_sector in template
 *  VSF_FLASH_CFG_READ_ONE_SECTOR_TEMPLATE          implement read_one_sector by read_multi_sector in template
 */

// only define in source file
#define VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY        ENABLED
#define VSF_FLASH_CFG_REIMPLEMENT_API_IRQ_CLEAR         ENABLED
#define VSF_FLASH_CFG_ERASE_ALL_TEMPLATE                ENABLED
#define VSF_FLASH_CFG_ERASE_ONE_SECTOR_TEMPLATE         ENABLED
#define VSF_FLASH_CFG_WRITE_ONE_SECTOR_TEMPLATE         ENABLED
#define VSF_FLASH_CFG_READ_ONE_SECTOR_TEMPLATE          ENABLED

#define VSF_FLASH_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t)                            \
        VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash ## __IDX) = {             \
        .reg                = VSF_MCONNECT(VSF_FLASH_CFG_IMP_UPCASE_PREFIX, _FLASH, __IDX, _REG),\
        .addr               = VSF_MCONNECT(VSF_FLASH_CFG_IMP_UPCASE_PREFIX, _FLASH, __IDX, _ADDR),\
        .size               = VSF_MCONNECT(VSF_FLASH_CFG_IMP_UPCASE_PREFIX, _FLASH, __IDX, _SIZE),\
        .irqn               = VSF_MCONNECT(VSF_FLASH_CFG_IMP_UPCASE_PREFIX, _FLASH, __IDX, _IRQN),\
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_MCONNECT(VSF_HW_INTERRUPT, VSF_MCONNECT(VSF_FLASH_CFG_IMP_UPCASE_PREFIX, _FLASH, __IDX, _IRQN))(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_irqhandler)(          \
            &VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash, __IDX)              \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/flash/flash_template.inc"

#endif      // VSF_HAL_USE_FLASH
