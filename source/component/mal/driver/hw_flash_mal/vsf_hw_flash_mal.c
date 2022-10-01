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

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_HW_FLASH_MAL == ENABLED && VSF_HAL_USE_FLASH == ENABLED

#define __VSF_MAL_CLASS_INHERIT__
#define __VSF_HW_FLASH_MAL_CLASS_IMPLEMENT

#include "../../vsf_mal.h"
#include "./vsf_hw_flash_mal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vk_hw_flash_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static bool __vk_hw_flash_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
dcl_vsf_peda_methods(static, __vk_hw_flash_mal_init)
dcl_vsf_peda_methods(static, __vk_hw_flash_mal_fini)
dcl_vsf_peda_methods(static, __vk_hw_flash_mal_read)
dcl_vsf_peda_methods(static, __vk_hw_flash_mal_write)

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

    // TODO: interrupt mode
    vsf_err_t err = vsf_hw_flash_init(pthis->flash, NULL);
    if (err == VSF_ERR_NONE) {
        while (vsf_hw_flash_enable(pthis->flash) != fsm_rt_cpl);
        pthis->cap = vsf_hw_flash_capability(pthis->flash);
    }

    vsf_eda_return(err);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_hw_flash_mal_fini, vk_mal_fini)
{
    vsf_peda_begin();
    vk_hw_flash_mal_t *pthis = (vk_hw_flash_mal_t *)&vsf_this;
    VSF_UNUSED_PARAM(pthis);
    VSF_MAL_ASSERT(pthis != NULL);

    while (vsf_hw_flash_disable(pthis->flash) != fsm_rt_cpl);

    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_hw_flash_mal_erase, vk_mal_erase)
{
    vsf_peda_begin();
    vk_hw_flash_mal_t *pthis = (vk_hw_flash_mal_t *)&vsf_this;
    vsf_flash_size_t offset;
    vsf_flash_size_t size;

    VSF_MAL_ASSERT(pthis != NULL);
    offset = vsf_local.addr;
    size = vsf_local.size;
    VSF_MAL_ASSERT((offset + size) <= pthis->cap.max_size);

    vsf_err_t err;
    if (size == 0) {
        err = vsf_hw_flash_erase_all(pthis->flash);
    } else {
        err = vsf_hw_flash_erase_multi_sector(pthis->flash, offset, size);
    }
    if (err != VSF_ERR_NONE) {
        size = (vsf_flash_size_t)-1;
    }

    vsf_eda_return(size);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_hw_flash_mal_read, vk_mal_read)
{
    vsf_peda_begin();
    vk_hw_flash_mal_t *pthis = (vk_hw_flash_mal_t *)&vsf_this;
    vsf_flash_size_t offset;
    vsf_flash_size_t size;

    VSF_MAL_ASSERT(pthis != NULL);
    offset = vsf_local.addr;
    size = vsf_local.size;
    VSF_MAL_ASSERT((size > 0) && ((offset + size) <= pthis->cap.max_size));
    VSF_MAL_ASSERT(pthis->flash != NULL);

    if (VSF_ERR_NONE != vsf_hw_flash_read_multi_sector(pthis->flash, offset, vsf_local.buff, size)) {
        size = (vsf_flash_size_t)-1;
    }

    vsf_eda_return(size);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_hw_flash_mal_write, vk_mal_write)
{
    vsf_peda_begin();
    vk_hw_flash_mal_t *pthis = (vk_hw_flash_mal_t *)&vsf_this;
    vsf_flash_size_t offset;
    vsf_flash_size_t size;

    VSF_MAL_ASSERT(pthis != NULL);
    offset = vsf_local.addr;
    size = vsf_local.size;
    VSF_MAL_ASSERT(size > 0);
    VSF_MAL_ASSERT((offset + size) <= pthis->cap.max_size);

    {
        vsf_err_t err;
        if (size == 0) {
            err = vsf_hw_flash_erase_all(pthis->flash);
        } else {
            err = vsf_hw_flash_erase_multi_sector(pthis->flash, offset, size);
        }
        if (err != VSF_ERR_NONE) {
            size = (vsf_flash_size_t)-1;
        } else {

            if (VSF_ERR_NONE != vsf_hw_flash_write_multi_sector(pthis->flash, offset, vsf_local.buff, size)) {
                size = (vsf_flash_size_t)-1;
            }

        }
    }

    vsf_eda_return(size);
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#endif
