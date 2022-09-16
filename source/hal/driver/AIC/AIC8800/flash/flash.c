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

#define VSF_FLASH_CFG_IMP_PREFIX                vsf_hw
#define VSF_FLASH_CFG_IMP_UPCASE_PREFIX         VSF_HW

/*============================ INCLUDES ======================================*/

#include "./flash.h"

#if VSF_HAL_USE_FLASH == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_HW_FLASH_CFG_MULTI_CLASS
#   define VSF_HW_FLASH_CFG_MULTI_CLASS         VSF_FLASH_CFG_MULTI_CLASS
#endif

// only define in source file
#define VSF_FLASH_CFG_REIMPLEMENT_CAPABILITY    ENABLED
#define VSF_FLASH_CFG_BASE_ADDRESS              0x8000000

#define __AIC8800_FLASH_CFG_PROTECT             interrupt

#define __AIC8800_FLASH_ALIGNMENT_ERASE_SIZE    0x1000

#define __AIC8800_FLASH_ALIGNMENT_WRITE_SIZE    0x100

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __aic8800_flash_protect                                                 \
    vsf_protect(__AIC8800_FLASH_CFG_PROTECT)

#define __aic8800_flash_unprotect                                               \
    vsf_unprotect(__AIC8800_FLASH_CFG_PROTECT)

#define __aic8800_flash_address_get(__offset)                                   \
    ((unsigned int)(VSF_FLASH_CFG_BASE_ADDRESS + (uint32_t)__offset))

#define __ROM_APITBL_BASE                         ((unsigned int *)0x00000180UL)

#define __ROM_FlashChipSizeGet                                                  \
    ((unsigned int (*)(void))__ROM_APITBL_BASE[2])

#define __ROM_FlashErase                                                        \
    ((int (*)(unsigned int a4k, unsigned int len))__ROM_APITBL_BASE[4])

#define __ROM_FlashWrite                                                        \
    ((int (*)(unsigned int adr, unsigned int len, unsigned int buf))__ROM_APITBL_BASE[5])

#define __ROM_FlashRead                                                         \
    ((int (*)(unsigned int adr, unsigned int len, unsigned int buf))__ROM_APITBL_BASE[6])

#define __ROM_FlashCacheInvalidRange                                            \
    ((void (*)(unsigned int adr, unsigned int len))__ROM_APITBL_BASE[8])

/*============================ TYPES =========================================*/

typedef struct vsf_hw_flash_t {
#if VSF_HW_FLASH_CFG_MULTI_CLASS == ENABLED
    vsf_flash_t vsf_flash;
#endif
    flash_cfg_t cfg;
    vsf_flash_size_t flash_size;
    bool is_enabled;
} vsf_hw_flash_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_flash_init(vsf_hw_flash_t *hw_flash_ptr, flash_cfg_t *cfg_ptr)
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

vsf_err_t vsf_hw_flash_erase(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_size_t offset, vsf_flash_size_t size)
{
    VSF_HAL_ASSERT(hw_flash_ptr != NULL);

    VSF_HAL_ASSERT(hw_flash_ptr->is_enabled);
    VSF_HAL_ASSERT(offset + size <= hw_flash_ptr->flash_size);
    VSF_HAL_ASSERT(0 == (offset % __AIC8800_FLASH_ALIGNMENT_WRITE_SIZE));
    VSF_HAL_ASSERT(0 == (size % __AIC8800_FLASH_ALIGNMENT_WRITE_SIZE));

    vsf_flash_size_t ret;

    vsf_protect_t org = __aic8800_flash_protect();
        ret = __ROM_FlashErase(__aic8800_flash_address_get(offset), size);//offset:4k * n
    __aic8800_flash_unprotect(org);
    __ROM_FlashCacheInvalidRange(__aic8800_flash_address_get(offset), size);

    if (NULL != hw_flash_ptr->cfg.isr.handler_fn) {
        vsf_flash_irq_mask_t mask = (0 == ret) ? VSF_FLASH_IRQ_ERASE_MASK : VSF_FLASH_IRQ_ERASE_ERROR_MASK;
        hw_flash_ptr->cfg.isr.handler_fn(hw_flash_ptr->cfg.isr.target_ptr, mask, (vsf_flash_t *)hw_flash_ptr);
    }

    return (0 == ret) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

vsf_err_t vsf_hw_flash_write(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_size_t offset, uint8_t* buffer, vsf_flash_size_t size)
{
    vsf_flash_size_t ret;

    VSF_HAL_ASSERT(hw_flash_ptr != NULL);
    VSF_HAL_ASSERT(hw_flash_ptr->is_enabled);
    VSF_HAL_ASSERT(offset + size <= hw_flash_ptr->flash_size);
    VSF_HAL_ASSERT(0 == (offset % __AIC8800_FLASH_ALIGNMENT_WRITE_SIZE));

    vsf_protect_t org = __aic8800_flash_protect();
        ret = __ROM_FlashWrite(__aic8800_flash_address_get(offset), size, (unsigned int)buffer);
    __aic8800_flash_unprotect(org);
    __ROM_FlashCacheInvalidRange(__aic8800_flash_address_get(offset), size);

    if (NULL != hw_flash_ptr->cfg.isr.handler_fn) {
        vsf_flash_irq_mask_t mask = (0 == ret) ? VSF_FLASH_IRQ_WRITE_MASK : VSF_FLASH_IRQ_WRITE_ERROR_MASK;
        hw_flash_ptr->cfg.isr.handler_fn(hw_flash_ptr->cfg.isr.target_ptr, mask, (vsf_flash_t *)hw_flash_ptr);
    }

    return (0 == ret) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

vsf_err_t vsf_hw_flash_read(vsf_hw_flash_t *hw_flash_ptr, vsf_flash_size_t offset, uint8_t* buffer, vsf_flash_size_t size)
{
    vsf_flash_size_t ret;

    VSF_HAL_ASSERT(hw_flash_ptr != NULL);
    VSF_HAL_ASSERT(NULL != buffer);
    VSF_HAL_ASSERT(hw_flash_ptr->is_enabled);
    VSF_HAL_ASSERT(offset + size <= hw_flash_ptr->flash_size);
    VSF_HAL_ASSERT(0 == (offset % __AIC8800_FLASH_ALIGNMENT_WRITE_SIZE));

    vsf_protect_t org = __aic8800_flash_protect();
        ret = __ROM_FlashRead(__aic8800_flash_address_get(offset), size, (unsigned int)buffer);
    __aic8800_flash_unprotect(org);

    if (NULL != hw_flash_ptr->cfg.isr.handler_fn) {
        vsf_flash_irq_mask_t mask = (0 == ret) ? VSF_FLASH_IRQ_READ_MASK : VSF_FLASH_IRQ_READ_ERROR_MASK;
        hw_flash_ptr->cfg.isr.handler_fn(hw_flash_ptr->cfg.isr.target_ptr, mask, (vsf_flash_t *)hw_flash_ptr);
    }

    return (0 == ret) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

flash_capability_t vsf_hw_flash_capability(vsf_hw_flash_t *hw_flash_ptr)
{
    flash_capability_t flash_capability = {
        .base_address = VSF_FLASH_CFG_BASE_ADDRESS,
        .size         = hw_flash_ptr->flash_size,
    };

    return flash_capability;
}

/*============================ INCLUDES ======================================*/

#define VSF_FLASH_CFG_IMP_LV0(__count, __hal_op)                                \
    vsf_hw_flash_t vsf_hw_flash ## __count = {                                  \
        .cfg = {                                                                \
            .isr = {                                                            \
                .handler_fn = NULL,                                             \
                .target_ptr = NULL,                                             \
            },                                                                  \
        },                                                                      \
        .flash_size = 0,                                                        \
        .is_enabled = false,                                                    \
        __hal_op                                                                \
    };
#include "hal/driver/common/flash/flash_template.inc"

#endif      // VSF_HAL_USE_FLASH
