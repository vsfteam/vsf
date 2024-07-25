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

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_CACHED_MAL == ENABLED

#define __VSF_MAL_CLASS_INHERIT__
#define __VSF_CACHED_MAL_CLASS_IMPLEMENT

#include "../../vsf_mal.h"
#include "./vsf_cached_mal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vk_cached_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static bool __vk_cached_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
dcl_vsf_peda_methods(static, __vk_cached_mal_init)
dcl_vsf_peda_methods(static, __vk_cached_mal_fini)
dcl_vsf_peda_methods(static, __vk_cached_mal_read)
dcl_vsf_peda_methods(static, __vk_cached_mal_write)
dcl_vsf_peda_methods(static, __vk_cached_mal_erase)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_mal_drv_t vk_cached_mal_drv = {
    .blksz              = __vk_cached_mal_blksz,
    .buffer             = __vk_cached_mal_buffer,
    .fini_local_size    = 4,
    .read_local_size    = 4,
    .write_local_size   = 8,
    .init               = (vsf_peda_evthandler_t)vsf_peda_func(__vk_cached_mal_init),
    .fini               = (vsf_peda_evthandler_t)vsf_peda_func(__vk_cached_mal_fini),
    .read               = (vsf_peda_evthandler_t)vsf_peda_func(__vk_cached_mal_read),
    .write              = (vsf_peda_evthandler_t)vsf_peda_func(__vk_cached_mal_write),
    .erase              = (vsf_peda_evthandler_t)vsf_peda_func(__vk_cached_mal_erase),
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __vk_cached_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op)
{
    vk_cached_mal_t *pthis = (vk_cached_mal_t *)mal;
    return vk_mal_blksz(pthis->host_mal, addr, size, op);
}

static bool __vk_cached_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem)
{
    vk_cached_mal_t *pthis = (vk_cached_mal_t *)mal;
    return vk_mal_prepare_buffer(pthis->host_mal, addr, size, op, mem);
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_cached_mal_init, vk_mal_init)
{
    vsf_peda_begin();
    vk_cached_mal_t *pthis = (vk_cached_mal_t *)&vsf_this;
    pthis->cache = NULL;
    pthis->cache_size = vk_mal_blksz(pthis->host_mal, 0, 0, VSF_MAL_OP_ERASE);
    pthis->cache_valid = false;
    if (pthis->cache_size != 0) {
        pthis->cache = vsf_heap_malloc(pthis->cache_size);
        if (NULL == pthis->cache) {
            vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
            return;
        }
    }
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_cached_mal_fini, vk_mal_fini,
    bool is_sync_erasing;
)
{
    vsf_peda_begin();
    vk_cached_mal_t *pthis = (vk_cached_mal_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        if (pthis->cache_valid) {
            vsf_local.is_sync_erasing = true;
            vk_mal_erase(pthis->host_mal, pthis->cache_addr, pthis->cache_size);
        } else {
            vsf_eda_return(VSF_ERR_NONE);
        }
        break;
    case VSF_EVT_RETURN:
        if (vsf_local.is_sync_erasing) {
            vsf_local.is_sync_erasing = false;
            vk_mal_write(pthis->host_mal, pthis->cache_addr, pthis->cache_size, pthis->cache);
            break;
        }

        vsf_heap_free(pthis->cache);
        pthis->cache = NULL;
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_cached_mal_read, vk_mal_read,
    uint32_t remain_size;
)
{
    vsf_peda_begin();

    vk_cached_mal_t *pthis = (vk_cached_mal_t *)&vsf_this;
    uint64_t cur_addr = vsf_local.addr;
    uint32_t cur_size = vsf_local.remain_size;
    uint8_t *cur_buff = vsf_local.buff;

    switch (evt) {
    case VSF_EVT_INIT:
        cur_size = vsf_local.remain_size = vsf_local.size;
        if (!pthis->cache_valid) {
            vk_mal_read(pthis->host_mal, cur_addr, cur_size, cur_buff);
            break;
        }

        // fall through
    case VSF_EVT_RETURN:
        if (!pthis->cache_valid || (0 == cur_size)) {
            vsf_eda_return(vsf_local.size);
            break;
        }

        if (    (cur_addr < pthis->cache_addr)
            ||  (cur_addr >= pthis->cache_addr + pthis->cache_size)) {
            if (pthis->cache_addr > cur_addr) {
                cur_size = vsf_min(cur_size, pthis->cache_addr - cur_addr);
            }

            vsf_local.addr += cur_size;
            vsf_local.buff += cur_size;
            vsf_local.remain_size -= cur_size;
            vk_mal_read(pthis->host_mal, cur_addr, cur_size, cur_buff);
            break;
        } else {
            uint32_t offset = cur_addr - pthis->cache_addr;
            cur_size = vsf_min(cur_size, pthis->cache_size - offset);
            memcpy(cur_buff, pthis->cache + offset, cur_size);
            vsf_local.remain_size -= cur_size;
            cur_addr += cur_size;
            cur_buff += cur_size;

            cur_size = vsf_local.remain_size;
            if (cur_size > 0) {
                vsf_local.remain_size = 0;
                vk_mal_read(pthis->host_mal, cur_addr, cur_size, cur_buff);
                break;
            } else {
                vsf_eda_return(vsf_local.size);
                break;
            }
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_cached_mal_write, vk_mal_write,
    uint32_t remain_size;
    bool is_sync_erasing;
)
{
    vsf_peda_begin();
    vk_cached_mal_t *pthis = (vk_cached_mal_t *)&vsf_this;
    uint64_t cur_addr = vsf_local.addr;
    uint32_t cur_size = vsf_local.remain_size;
    uint8_t *cur_buff = vsf_local.buff;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.is_sync_erasing = false;
        cur_size = vsf_local.remain_size = vsf_local.size;
        if (NULL == pthis->cache) {
            vk_mal_write(pthis->host_mal, cur_addr, cur_size, cur_buff);
            break;
        }
        // fall through
    case VSF_EVT_RETURN:
        if (vsf_local.is_sync_erasing) {
            vsf_local.is_sync_erasing = false;
            pthis->cache_valid = false;
            vk_mal_write(pthis->host_mal, pthis->cache_addr, pthis->cache_size, pthis->cache);
            break;
        }
        if ((NULL == pthis->cache) || (0 == cur_size)) {
            vsf_eda_return(vsf_local.size);
            break;
        }

        if (pthis->cache_valid) {
            if (cur_addr == pthis->cache_addr + pthis->cache_pos) {
                cur_size = vsf_min(cur_size, pthis->cache_size - pthis->cache_pos);
                memcpy(pthis->cache + pthis->cache_pos, cur_buff, cur_size);
                pthis->cache_pos += cur_size;
                vsf_local.addr += cur_size;
                vsf_local.buff += cur_size;
                vsf_local.remain_size -= cur_size;

                if (pthis->cache_pos == pthis->cache_size) {
                sync_cache:
                    vsf_local.is_sync_erasing = true;
                    vk_mal_erase(pthis->host_mal, pthis->cache_addr, pthis->cache_size);
                } else {
                    vsf_eda_return(vsf_local.size);
                }
                break;
            } else {
                goto sync_cache;
            }
        } else if (!(cur_addr % pthis->cache_size) && (cur_size >= pthis->cache_size)) {
            cur_size = pthis->cache_size;
            vsf_local.addr += cur_size;
            vsf_local.buff += cur_size;
            vsf_local.remain_size -= cur_size;
            vsf_local.is_sync_erasing = true;
            pthis->cache_addr = cur_addr;
            memcpy(pthis->cache, cur_buff, cur_size);
            vk_mal_erase(pthis->host_mal, pthis->cache_addr, cur_size);
        } else {
            uint64_t aligned_addr = cur_addr & ~(pthis->cache_size - 1);
            pthis->cache_valid = true;
            pthis->cache_pos = cur_addr - aligned_addr;
            pthis->cache_addr = aligned_addr;
            vk_mal_read(pthis->host_mal, aligned_addr, pthis->cache_size, pthis->cache);
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_cached_mal_erase, vk_mal_erase)
{
    vsf_peda_begin();
    vk_cached_mal_t *pthis = (vk_cached_mal_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        if (pthis->cache_valid && (vsf_local.addr == pthis->cache_addr) && (pthis->cache != NULL)) {
            pthis->cache_valid = false;
        }
        vk_mal_erase(pthis->host_mal, vsf_local.addr, vsf_local.size);
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
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
