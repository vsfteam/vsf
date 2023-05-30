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

#include "../../vsf_mal_cfg.h"
#include "hal/driver/driver.h"

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_HW_FLASH_MAL == ENABLED && VSF_HAL_USE_FLASH == ENABLED

#define __VSF_MAL_CLASS_INHERIT__
#define __VSF_HW_FLASH_MAL_CLASS_IMPLEMENT

#include "../../vsf_mal.h"
#include "./vsf_hw_flash_mal.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_FLASH_MAL_CFG_ISR_PRIO
#   define VSF_HW_FLASH_MAL_CFG_ISR_PRIO            vsf_arch_prio_1
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_hw_flash_evt_t {
    VSF_EVT_FLASH_ERASE_CPL = VSF_EVT_USER,
    VSF_EVT_FLASH_WRITE_CPL,
    VSF_EVT_FLASH_READ_CPL,
    VSF_EVT_FLASH_ERASE_ERR,
    VSF_EVT_FLASH_WRITE_ERR,
    VSF_EVT_FLASH_READ_ERR,
} vsf_hw_flash_evt_t;

/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vk_hw_flash_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static bool __vk_hw_flash_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
dcl_vsf_peda_methods(static, __vk_hw_flash_mal_init)
dcl_vsf_peda_methods(static, __vk_hw_flash_mal_fini)
dcl_vsf_peda_methods(static, __vk_hw_flash_mal_read)
dcl_vsf_peda_methods(static, __vk_hw_flash_mal_write)
dcl_vsf_peda_methods(static, __vk_hw_flash_mal_erase)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_mal_drv_t vk_hw_flash_mal_drv = {
    .blksz          = __vk_hw_flash_mal_blksz,
    .buffer         = __vk_hw_flash_mal_buffer,
    .init           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_hw_flash_mal_init),
    .fini           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_hw_flash_mal_fini),
    .read           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_hw_flash_mal_read),
    .write          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_hw_flash_mal_write),
    .erase          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_hw_flash_mal_erase),
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __vk_hw_flash_mal_blksz(vk_mal_t *mal, uint_fast64_t addr,
                uint_fast32_t size, vsf_mal_op_t op)
{
    vk_hw_flash_mal_t *pthis = (vk_hw_flash_mal_t *)mal;
    return pthis->cap.erase_sector_size;
}

static bool __vk_hw_flash_mal_buffer(vk_mal_t *mal, uint_fast64_t addr,
                uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem)
{
    mem->buffer = 0;
    mem->size = 0;
    return false;
}

static void __hw_flash_isrhandler(void *target_ptr, vsf_flash_t *flash_ptr, vsf_flash_irq_mask_t mask)
{
    vk_hw_flash_mal_t *pthis = (vk_hw_flash_mal_t *)target_ptr;
    VSF_ASSERT(pthis != NULL);
    VSF_ASSERT(pthis->cur != NULL);
    VSF_MAL_ASSERT((mask & ~VSF_FLASH_IRQ_ALL_BITS_MASK) == 0);

    if (mask & VSF_FLASH_IRQ_ERASE_ERROR_MASK) {
        vsf_eda_post_evt(pthis->cur, VSF_FLASH_IRQ_ERASE_ERROR_MASK);
    } else if (mask & VSF_FLASH_IRQ_ERASE_MASK) {
        vsf_eda_post_evt(pthis->cur, VSF_EVT_FLASH_ERASE_CPL);
    }

    if (mask & VSF_FLASH_IRQ_WRITE_ERROR_MASK) {
        vsf_eda_post_evt(pthis->cur, VSF_FLASH_IRQ_WRITE_ERROR_MASK);
    } else if (mask & VSF_FLASH_IRQ_WRITE_MASK) {
        vsf_eda_post_evt(pthis->cur, VSF_EVT_FLASH_WRITE_CPL);
    }

    if (mask & VSF_FLASH_IRQ_READ_ERROR_MASK) {
        vsf_eda_post_evt(pthis->cur, VSF_FLASH_IRQ_READ_ERROR_MASK);
    } else if (mask & VSF_FLASH_IRQ_READ_MASK) {
        vsf_eda_post_evt(pthis->cur, VSF_EVT_FLASH_READ_CPL);
    }
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_hw_flash_mal_init, vk_mal_init)
{
    vsf_peda_begin();
    vk_hw_flash_mal_t *pthis = (vk_hw_flash_mal_t *)&vsf_this;
    VSF_MAL_ASSERT(pthis != NULL);
    VSF_MAL_ASSERT(pthis->flash != NULL);
    vsf_err_t err;
    vsf_flash_cfg_t cfg;
    vsf_flash_irq_mask_t irq_mask;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->cap = vsf_hw_flash_capability(pthis->flash);
        irq_mask = VSF_FLASH_IRQ_ERASE_MASK | VSF_FLASH_IRQ_WRITE_MASK | VSF_FLASH_IRQ_READ_MASK;
        if ((pthis->cap.irq_mask & irq_mask) != irq_mask) {
            err = VSF_ERR_NOT_SUPPORT;
        } else {
            pthis->cur = NULL;
            cfg.isr.handler_fn = __hw_flash_isrhandler;
            cfg.isr.target_ptr = pthis;
            cfg.isr.prio = VSF_HW_FLASH_MAL_CFG_ISR_PRIO;
            err = vsf_hw_flash_init(pthis->flash, &cfg);
            if (err == VSF_ERR_NONE) {
                while (vsf_hw_flash_enable(pthis->flash) != fsm_rt_cpl);
                vsf_hw_flash_irq_enable(pthis->flash, VSF_FLASH_IRQ_ALL_BITS_MASK);
            }
        }
        vsf_eda_return(err);
        break;

    default:
        VSF_MAL_ASSERT(0);
        break;
    }

    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_hw_flash_mal_fini, vk_mal_fini)
{
    vsf_peda_begin();
    vk_hw_flash_mal_t *pthis = (vk_hw_flash_mal_t *)&vsf_this;
    VSF_MAL_ASSERT(pthis != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_hw_flash_irq_disable(pthis->flash, VSF_FLASH_IRQ_ALL_BITS_MASK);
        while (vsf_hw_flash_disable(pthis->flash) != fsm_rt_cpl);
        vsf_hw_flash_fini(pthis->flash);
        vsf_eda_return(VSF_ERR_NONE);
        break;

    default:
        VSF_MAL_ASSERT(0);
        break;
    }

    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_hw_flash_mal_erase, vk_mal_erase)
{
    vsf_peda_begin();
    vk_hw_flash_mal_t *pthis = (vk_hw_flash_mal_t *)&vsf_this;
    VSF_MAL_ASSERT(pthis != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->cur = vsf_eda_get_cur();
        VSF_MAL_ASSERT((vsf_local.addr + vsf_local.size) <= pthis->cap.max_size);
        if (VSF_ERR_NONE == vsf_hw_flash_erase_multi_sector(pthis->flash, vsf_local.addr, vsf_local.size)) {
            break;
        }
        // fall through

    case VSF_EVT_FLASH_ERASE_ERR:
        vsf_eda_return(-1);
        break;

    case VSF_EVT_FLASH_ERASE_CPL:
        vsf_eda_return(vsf_local.size);
        break;

    default:
        VSF_MAL_ASSERT(0);
        break;
    }

    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_hw_flash_mal_read, vk_mal_read)
{
    vsf_peda_begin();
    vk_hw_flash_mal_t *pthis = (vk_hw_flash_mal_t *)&vsf_this;
    VSF_MAL_ASSERT(pthis != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->cur = vsf_eda_get_cur();
        VSF_MAL_ASSERT((vsf_local.size > 0) && ((vsf_local.addr + vsf_local.size) <= pthis->cap.max_size));
        if (VSF_ERR_NONE == vsf_hw_flash_read_multi_sector(pthis->flash, vsf_local.addr, vsf_local.buff, vsf_local.size)) {
            break;
        }
        // fall through

    case VSF_EVT_FLASH_READ_ERR:
        vsf_eda_return(-1);
        break;

    case VSF_EVT_FLASH_READ_CPL:
        vsf_eda_return(vsf_local.size);
        break;

    default:
        VSF_MAL_ASSERT(0);
        break;
    }

    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_hw_flash_mal_write, vk_mal_write)
{
    vsf_peda_begin();
    vk_hw_flash_mal_t *pthis = (vk_hw_flash_mal_t *)&vsf_this;
    VSF_MAL_ASSERT(pthis != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->cur = vsf_eda_get_cur();
        VSF_MAL_ASSERT((vsf_local.size > 0) && ((vsf_local.addr + vsf_local.size) <= pthis->cap.max_size));
        if (VSF_ERR_NONE == vsf_hw_flash_erase_multi_sector(pthis->flash, vsf_local.addr, vsf_local.size)) {
            break;
        }
        // fall through

    case VSF_EVT_FLASH_ERASE_ERR:
        vsf_eda_return(-1);
        break;

    case VSF_EVT_FLASH_ERASE_CPL:
        if (VSF_ERR_NONE == vsf_hw_flash_write_multi_sector(pthis->flash, vsf_local.addr, vsf_local.buff, vsf_local.size)) {
            break;
        }
        // fall through

    case VSF_EVT_FLASH_WRITE_ERR:
        vsf_eda_return(-1);
        break;

    case VSF_EVT_FLASH_WRITE_CPL:
        vsf_eda_return(vsf_local.size);
        break;

    default:
        VSF_MAL_ASSERT(0);
        break;
    }

    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#endif
