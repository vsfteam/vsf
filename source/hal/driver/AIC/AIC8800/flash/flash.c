/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#include "./flash.h"
#if VSF_HAL_USE_FLASH == ENABLED

/*============================ MACROS ========================================*/

#undef VSF_AIC8800_FLASH_CFG_PROTECT
#define VSF_AIC8800_FLASH_CFG_PROTECT                   interrupt

#undef VSF_AIC8800_FLASH_BASE_ADDRESS
#define VSF_AIC8800_FLASH_BASE_ADDRESS                  0x8000000

#undef VSF_AIC8800_FLASH_ALIGNMENT_ERASE_SIZE
#define VSF_AIC8800_FLASH_ALIGNMENT_ERASE_SIZE          0x1000

#undef VSF_AIC8800_FLASH_ALIGNMENT_WRITE_SIZE
#define VSF_AIC8800_FLASH_ALIGNMENT_WRITE_SIZE          0x100

/*============================ MACROFIED FUNCTIONS ===========================*/

#undef __aic8800_flash_protect
#define __aic8800_flash_protect                                                 \
    vsf_protect(VSF_AIC8800_FLASH_CFG_PROTECT)

#undef __aic8800_flash_unprotect
#define __aic8800_flash_unprotect                                               \
    vsf_unprotect(VSF_AIC8800_FLASH_CFG_PROTECT)

#undef __aic8800_flash_address_get
#define __aic8800_flash_address_get(__offset)                                   \
    ((unsigned int)(VSF_AIC8800_FLASH_BASE_ADDRESS + (uint32_t)__offset))

#define ROM_APITBL_BASE                         ((unsigned int *)0x00000180UL)

#define ROM_FlashChipSizeGet                                                    \
    ((unsigned int (*)(void))ROM_APITBL_BASE[2])

#define ROM_FlashErase                                                          \
    ((int (*)(unsigned int a4k, unsigned int len))ROM_APITBL_BASE[4])

#define ROM_FlashWrite                                                          \
    ((int (*)(unsigned int adr, unsigned int len, unsigned int buf))ROM_APITBL_BASE[5])

#define ROM_FlashRead                                                           \
    ((int (*)(unsigned int adr, unsigned int len, unsigned int buf))ROM_APITBL_BASE[6])

#define ROM_FlashCacheInvalidRange                                              \
    ((void (*)(unsigned int adr, unsigned int len))ROM_APITBL_BASE[8])

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

vsf_flash_t vsf_flash0 = {
    .cfg = {
        .isr = {
            .handler_fn = NULL,
            .target_ptr = NULL,
        },
    },
    .info = {
        .flash_size = 0,
    },
    .is_enabled = 0,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_flash_init(vsf_flash_t *flash_ptr, flash_cfg_t *cfg_ptr)
{
    flash_ptr->cfg = *cfg_ptr;

    vsf_protect_t org = __aic8800_flash_protect();
    flash_ptr->info.flash_size = ROM_FlashChipSizeGet();
    __aic8800_flash_unprotect(org);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_flash_enable(vsf_flash_t *flash_ptr)
{
    flash_ptr->is_enabled = 1;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_flash_disable(vsf_flash_t *flash_ptr)
{
    flash_ptr->is_enabled = 0;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_flash_erase(  vsf_flash_t *flash_ptr,
                            uint_fast32_t offset,
                            uint_fast32_t size)
{
    uint_fast32_t ret;
    if (!flash_ptr->is_enabled) {
        return VSF_ERR_NOT_READY;
    }
    if (offset + size > flash_ptr->info.flash_size) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    if (    (0 != (offset % VSF_AIC8800_FLASH_ALIGNMENT_ERASE_SIZE))
        ||  (0 != (size % VSF_AIC8800_FLASH_ALIGNMENT_ERASE_SIZE))  ) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    vsf_protect_t org = __aic8800_flash_protect();
    ret = ROM_FlashErase(__aic8800_flash_address_get(offset), size);//offset:4k * n
    __aic8800_flash_unprotect(org);
    ROM_FlashCacheInvalidRange(__aic8800_flash_address_get(offset), size);

    if (NULL != flash_ptr->cfg.isr.handler_fn) {
        flash_ptr->cfg.isr.handler_fn(
                            flash_ptr->cfg.isr.target_ptr,
                            0 == ret ? VSF_FLASH_IRQ_ERASE_MASK : VSF_FLASH_IRQ_ERASE_ERROR_MASK,
                            flash_ptr);
    }
    return VSF_ERR_NONE;
}

vsf_err_t vsf_flash_write(  vsf_flash_t *flash_ptr,
                            uint_fast32_t offset,
                            uint8_t* buffer,
                            uint_fast32_t size)
{
    uint_fast32_t ret;
    if (!flash_ptr->is_enabled) {
        return VSF_ERR_NOT_READY;
    }
    if (offset + size > flash_ptr->info.flash_size) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    if (0 != (offset % VSF_AIC8800_FLASH_ALIGNMENT_WRITE_SIZE)) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    vsf_protect_t org = __aic8800_flash_protect();
    ret = ROM_FlashWrite(__aic8800_flash_address_get(offset), size, (unsigned int)buffer);
    __aic8800_flash_unprotect(org);
    ROM_FlashCacheInvalidRange(__aic8800_flash_address_get(offset), size);

    if (NULL != flash_ptr->cfg.isr.handler_fn) {
        flash_ptr->cfg.isr.handler_fn(
                            flash_ptr->cfg.isr.target_ptr,
                            0 == ret ? VSF_FLASH_IRQ_WRITE_MASK : VSF_FLASH_IRQ_WRITE_ERROR_MASK,
                            flash_ptr);
    }
    return VSF_ERR_NONE;
}

vsf_err_t vsf_flash_read(   vsf_flash_t *flash_ptr,
                            uint_fast32_t offset,
                            uint8_t* buffer,
                            uint_fast32_t size)
{
    uint_fast32_t ret;
    VSF_HAL_ASSERT((NULL != flash_ptr) && (NULL != buffer));
    if (!flash_ptr->is_enabled) {
        return VSF_ERR_NOT_READY;
    }
    if (offset + size > flash_ptr->info.flash_size) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    vsf_protect_t org = __aic8800_flash_protect();
    ret = ROM_FlashRead(__aic8800_flash_address_get(offset), size, (unsigned int)buffer);
    __aic8800_flash_unprotect(org);

    if (NULL != flash_ptr->cfg.isr.handler_fn) {
        flash_ptr->cfg.isr.handler_fn(
                            flash_ptr->cfg.isr.target_ptr,
                            0 == ret ? VSF_FLASH_IRQ_READ_MASK : VSF_FLASH_IRQ_READ_ERROR_MASK,
                            flash_ptr);
    }
    return VSF_ERR_NONE;
}

#endif      // VSF_HAL_USE_FLASH
