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

/*============================ MACROS ========================================*/

#ifndef VSF_HW_FLASH_CFG_MULTI_CLASS
#   define VSF_HW_FLASH_CFG_MULTI_CLASS             VSF_FLASH_CFG_MULTI_CLASS
#endif

#define __AIC8800_FLASH_CFG_PROTECT                 interrupt

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __aic8800_flash_protect                                                 \
    vsf_protect(__AIC8800_FLASH_CFG_PROTECT)

#define __aic8800_flash_unprotect                                               \
    vsf_unprotect(__AIC8800_FLASH_CFG_PROTECT)

#define __aic8800_flash_address_get(__offset)                                   \
    ((unsigned int)(VSF_HW_FLASH_CFG_BASE_ADDRESS + (uint32_t)__offset))

/*============================ TYPES =========================================*/

typedef struct vsf_hw_flash_t {
#if VSF_HW_FLASH_CFG_MULTI_CLASS == ENABLED
    vsf_flash_t vsf_flash;
#endif
    vsf_flash_cfg_t cfg;
    vsf_flash_size_t flash_size;
    bool is_enabled;
    vsf_flash_irq_mask_t irq_mask;
} vsf_hw_flash_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_flash_init(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(hw_flash_ptr != NULL);

    if (cfg_ptr != NULL) {
        hw_flash_ptr->cfg = *cfg_ptr;
    }

    vsf_protect_t org = __aic8800_flash_protect();
        hw_flash_ptr->flash_size = __ROM_FlashChipSizeGet();
    __aic8800_flash_unprotect(org);

    VSF_HAL_ASSERT(hw_flash_ptr->flash_size > 0);

    return VSF_ERR_NONE;
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
    vsf_flash_size_t ret;

    VSF_HAL_ASSERT(hw_flash_ptr != NULL);

    VSF_HAL_ASSERT(hw_flash_ptr->is_enabled);
    VSF_HAL_ASSERT(offset + size <= hw_flash_ptr->flash_size);
    VSF_HAL_ASSERT(0 == (offset % VSF_HW_FLASH_CFG_ERASE_SECTORE_SIZE));
    VSF_HAL_ASSERT(0 == (size % VSF_HW_FLASH_CFG_ERASE_SECTORE_SIZE));

    vsf_protect_t org = __aic8800_flash_protect();
        ret = __ROM_FlashErase(__aic8800_flash_address_get(offset), size);//offset:4k * n
    __aic8800_flash_unprotect(org);
    __ROM_FlashCacheInvalidRange(__aic8800_flash_address_get(offset), size);

    if (NULL != hw_flash_ptr->cfg.isr.handler_fn) {
        vsf_flash_irq_mask_t mask = (0 == ret) ? VSF_FLASH_IRQ_ERASE_MASK : VSF_FLASH_IRQ_ERASE_ERROR_MASK;
        hw_flash_ptr->cfg.isr.handler_fn(hw_flash_ptr->cfg.isr.target_ptr, (vsf_flash_t *)hw_flash_ptr, mask);
    }

    return (0 == ret) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

vsf_err_t vsf_hw_flash_write_multi_sector(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_size_t offset, uint8_t* buffer, vsf_flash_size_t size)
{
    vsf_flash_size_t ret;

    VSF_HAL_ASSERT(hw_flash_ptr != NULL);
    VSF_HAL_ASSERT(hw_flash_ptr->is_enabled);
    VSF_HAL_ASSERT(offset + size <= hw_flash_ptr->flash_size);
    VSF_HAL_ASSERT(0 == (offset % VSF_HW_FLASH_CFG_WRITE_SECTORE_SIZE));

    vsf_protect_t org = __aic8800_flash_protect();
        ret = __ROM_FlashWrite(__aic8800_flash_address_get(offset), size, (unsigned int)buffer);
    __aic8800_flash_unprotect(org);
    __ROM_FlashCacheInvalidRange(__aic8800_flash_address_get(offset), size);

    if (NULL != hw_flash_ptr->cfg.isr.handler_fn) {
        vsf_flash_irq_mask_t mask = (0 == ret) ? VSF_FLASH_IRQ_WRITE_MASK : VSF_FLASH_IRQ_WRITE_ERROR_MASK;
        hw_flash_ptr->cfg.isr.handler_fn(hw_flash_ptr->cfg.isr.target_ptr, (vsf_flash_t *)hw_flash_ptr, mask);
    }

    return (0 == ret) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

vsf_err_t vsf_hw_flash_read_multi_sector(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_size_t offset, uint8_t* buffer, vsf_flash_size_t size)
{
    vsf_flash_size_t ret;

    VSF_HAL_ASSERT(hw_flash_ptr != NULL);
    VSF_HAL_ASSERT(NULL != buffer);
    VSF_HAL_ASSERT(hw_flash_ptr->is_enabled);
    VSF_HAL_ASSERT(offset + size <= hw_flash_ptr->flash_size);

    vsf_protect_t org = __aic8800_flash_protect();
        ret = __ROM_FlashRead(__aic8800_flash_address_get(offset), size, (unsigned int)buffer);
    __aic8800_flash_unprotect(org);

    if (NULL != hw_flash_ptr->cfg.isr.handler_fn) {
        vsf_flash_irq_mask_t mask = (0 == ret) ? VSF_FLASH_IRQ_READ_MASK : VSF_FLASH_IRQ_READ_ERROR_MASK;
        hw_flash_ptr->cfg.isr.handler_fn(hw_flash_ptr->cfg.isr.target_ptr, (vsf_flash_t *)hw_flash_ptr, mask);
    }

    return (0 == ret) ? VSF_ERR_NONE : VSF_ERR_FAIL;
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
        .base_address          = VSF_HW_FLASH_CFG_BASE_ADDRESS,
        .max_size              = hw_flash_ptr->flash_size,
        .erase_sector_size     = VSF_HW_FLASH_CFG_ERASE_SECTORE_SIZE,
        .write_sector_size     = VSF_HW_FLASH_CFG_WRITE_SECTORE_SIZE,
        .can_write_any_address = 0,
        .can_read_any_address  = 1,
    };

    return flash_capability;
}

/*============================ INCLUDES ======================================*/

// only define in source file
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
        .flash_size = 0,                                                        \
        .is_enabled = false,                                                    \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/flash/flash_template.inc"

#endif      // VSF_HAL_USE_FLASH
