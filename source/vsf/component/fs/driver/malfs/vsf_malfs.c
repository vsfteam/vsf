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

#include "../../vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED && VSF_USE_MALFS == ENABLED

#define VSF_FS_INHERIT
#define VSF_MALFS_IMPLEMENT
#define VSF_MAL_INHERIT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void __vk_malfs_init(__vk_malfs_info_t *info)
{
#if VSF_FS_CFG_SUPPORT_MULTI_THREAD == ENABLED
    vsf_eda_crit_init(&info->crit);
#endif
    __vk_malfs_cache_init(info, &info->cache);
}

static uint8_t * __vk_malfs_get_cache_buff(__vk_malfs_cache_t *cache, __vk_malfs_cache_node_t *node)
{
    int idx = node - cache->nodes;
    uint8_t *buff = (uint8_t *)&cache->nodes[cache->number];
    return &buff[idx * cache->info->block_size];
}

void __vk_malfs_cache_init(__vk_malfs_info_t *info, __vk_malfs_cache_t *cache)
{
    cache->info = info;
#if VSF_FS_CFG_SUPPORT_MULTI_THREAD == ENABLED
    vsf_eda_crit_init(&cache->crit);
#endif
}

static void __vk_malfs_alloc_cache_imp(uintptr_t target, vsf_evt_t evt)
{
    __vk_malfs_cache_t *cache = (__vk_malfs_cache_t *)target;
    __vk_malfs_info_t *info = cache->info;
    __vk_malfs_cache_node_t *nodes = cache->nodes;

    switch (evt) {
    case VSF_EVT_INIT: {
            __vk_malfs_cache_node_t *least_node = nodes, *unalloced_node = NULL;
            for (uint_fast16_t i = 0; i < cache->number; i++) {
                if (nodes[i].is_alloced) {
                    if (nodes[i].block_addr == cache->ctx.block_addr) {
                        cache->ctx.result = &nodes[i];
                        vsf_eda_return();
                        return;
                    }
                    if (nodes[i].access_time_sec < least_node->access_time_sec) {
                        least_node = &nodes[i];
                    }
                } else if (NULL == unalloced_node) {
                    unalloced_node = &nodes[i];
                }
            }
            if (unalloced_node != NULL) {
                unalloced_node->is_alloced = true;
                cache->ctx.result = unalloced_node;
                vsf_eda_return();
                return;
            }

            cache->ctx.result = least_node;
            // missed, get least cache
            // TODO: add bit to indicate the node is not currently used
            if (!least_node->is_dirty) {
                cache->ctx.result = least_node;
                vsf_eda_return();
                return;
            }

            least_node->is_dirty = false;
            __vk_malfs_write(info, least_node->block_addr, 1, __vk_malfs_get_cache_buff(&info->cache, least_node));
            break;
        }
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return();
        break;
    }
}

vsf_err_t __vk_malfs_alloc_cache(__vk_malfs_info_t *info, __vk_malfs_cache_t *cache, uint_fast64_t block_addr)
{
    // TODO: add lock
    cache->ctx.block_addr = block_addr;
    cache->ctx.result = NULL;
    return __vsf_call_eda((uintptr_t)__vk_malfs_alloc_cache_imp, cache);
}

static void __vk_malfs_read_imp(uintptr_t target, vsf_evt_t evt)
{
    __vk_malfs_info_t *info = (__vk_malfs_info_t *)target;
    enum {
        STATE_GET_CACHE,
        STATE_COMMIT_READ,
        STATE_FINISH_READ,
    };

    switch (evt) {
    case VSF_EVT_INIT:
        if (NULL == info->ctx_io.buff) {
            VSF_FS_ASSERT(1 == info->ctx_io.block_num);
            vsf_eda_frame_user_value_set(STATE_GET_CACHE);
            __vk_malfs_alloc_cache(info, &info->cache, info->ctx_io.block_addr);
            return;
        }
        vsf_eda_frame_user_value_set(STATE_COMMIT_READ);
    case VSF_EVT_RETURN: {
            uint_fast8_t state;
            vsf_eda_frame_user_value_get(&state);
            switch (state) {
            case STATE_GET_CACHE:
                VSF_FS_ASSERT(info->cache.ctx.result != NULL);
                info->ctx_io.buff = __vk_malfs_get_cache_buff(&info->cache, info->cache.ctx.result);
                // fall through
            case STATE_COMMIT_READ:
                vsf_eda_frame_user_value_set(STATE_FINISH_READ);
                vk_mal_read(info->mal, info->block_size * info->ctx_io.block_addr,
                            info->block_size * info->ctx_io.block_num, info->ctx_io.buff);
                break;
            case STATE_FINISH_READ:
                info->ctx_io.err = info->mal->result.errcode;
                vsf_eda_return();
                break;
            }
        }
        break;
    }
}

vsf_err_t __vk_malfs_read(__vk_malfs_info_t *info, uint_fast64_t block_addr, uint_fast32_t block_num, uint8_t *buff)
{
    // TODO: add lock and unlock
    info->ctx_io.block_addr = block_addr;
    info->ctx_io.block_num = block_num;
    info->ctx_io.buff = buff;
    return __vsf_call_eda((uintptr_t)__vk_malfs_read_imp, info);
}

vsf_err_t __vk_malfs_write(__vk_malfs_info_t *info, uint_fast64_t block_addr, uint_fast32_t block_num, uint8_t *buff)
{
    // TODO: add lock and unlock
//    info->ctx_io.block_addr = block_addr;
//    info->ctx_io.block_num = block_num;
//    info->ctx_io.buff = buff;
    return vk_mal_write(info->mal, info->block_size * block_addr, info->block_size * block_num, buff);
}

#endif
