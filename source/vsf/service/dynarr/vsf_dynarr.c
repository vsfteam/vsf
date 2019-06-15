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

#include "service/vsf_service_cfg.h"

#if VSF_USE_DYNARR == ENABLED

#define VSF_DYNARR_IMPLEMENT
#include "./vsf_dynarr.h"
#include "../heap/vsf_heap.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static uint_fast16_t vsf_dynarr_get_buf_num(vsf_dynarr_t *dynarr)
{
    uint_fast16_t buf_num = dynarr->length + (1 << dynarr->item_num_per_buf_bitlen) - 1;
    buf_num >>= dynarr->item_num_per_buf_bitlen;
    return buf_num;
}

static vsf_dynarr_table_t * vsf_dynarr_get_table(vsf_dynarr_t *dynarr, uint_fast16_t buf_idx)
{
    uint_fast16_t table_idx = buf_idx >> dynarr->buf_num_per_table_bitlen;

    if (buf_idx < vsf_dynarr_get_buf_num(dynarr)) {
        __vsf_slist_foreach_unsafe(vsf_dynarr_table_t, table_node, &dynarr->table_list) {
            if (!table_idx--) {
                return _;
            }
        }
    }
    return NULL;
}

uint_fast32_t vsf_dynarr_get_size(vsf_dynarr_t *dynarr)
{
    return dynarr->length;
}

vsf_err_t vsf_dynarr_set_size(vsf_dynarr_t *dynarr, uint_fast32_t size)
{
    uint_fast16_t buf_num_per_table = 1UL << dynarr->buf_num_per_table_bitlen;
    uint_fast16_t item_num_per_buf = 1UL << dynarr->item_num_per_buf_bitlen;

    uint_fast16_t buf_num = (size + item_num_per_buf - 1) >> dynarr->item_num_per_buf_bitlen;
    uint_fast16_t buf_num_orig = (dynarr->length + item_num_per_buf - 1) >> dynarr->item_num_per_buf_bitlen;
    int_fast16_t buf_num_diff = buf_num - buf_num_orig;

    uint_fast32_t buffer_size = buf_num_per_table * dynarr->item_size;
    uint_fast16_t table_size = sizeof(vsf_dynarr_table_t) + sizeof(uint8_t *) * buf_num_per_table;
    int_fast16_t buf_idx;

    vsf_dynarr_table_t *table, *table_tmp;

    if (!buf_num_diff) {
        dynarr->length = size;
        return VSF_ERR_NONE;
    } else if (buf_num_diff > 0) {
        buf_idx = buf_num_orig - 1;
    } else /* if (buf_num_diff < 0) */ {
        buf_idx = buf_num - 1;
    }

    dynarr->length = buf_num_orig * item_num_per_buf;
    table = vsf_dynarr_get_table(dynarr, buf_idx);
    if (buf_idx < 0) {
        buf_idx = buf_num_per_table - 1;
        table = container_of(&dynarr->table_list, vsf_dynarr_table_t, table_node);
    } else {
        buf_idx &= buf_num_per_table - 1;
    }

    if (buf_num_diff > 0) {
        void *buf_new;

        buf_idx++;
        table_tmp = NULL;
        while (buf_num_diff > 0) {
            if (buf_idx == buf_num_per_table) {
                buf_idx = 0;
                table_tmp = (vsf_dynarr_table_t *)vsf_heap_malloc(table_size);
                if (!table_tmp) {
                    return VSF_ERR_NOT_ENOUGH_RESOURCES;
                }
                memset(table_tmp, 0, sizeof(*table_tmp));
            }

            buf_new = vsf_heap_malloc(buffer_size);
            if (!buf_new) {
                if (table_tmp != NULL) {
                    vsf_heap_free(table_tmp);
                }
                return VSF_ERR_NOT_ENOUGH_RESOURCES;
            }

            if (table_tmp != NULL) {
                vsf_slist_set_next(vsf_dynarr_table_t, table_node, &table->table_node, table_tmp);
                table = table_tmp;
                table_tmp = NULL;
            }

            table->buffer[buf_idx++] = buf_new;
            dynarr->length += item_num_per_buf;
            buf_num_diff--;
        }
    } else {
        bool first = true;
        buf_num_diff = -buf_num_diff;
        while (buf_num_diff > 0) {
            if (++buf_idx >= buf_num_per_table) {
                if (first) {
                    first = false;
                    vsf_slist_peek_next(vsf_dynarr_table_t, table_node, table, table_tmp);
                    vsf_slist_init_node(vsf_dynarr_table_t, table_node, table);
                    table = table_tmp;
                }
                vsf_slist_peek_next(vsf_dynarr_table_t, table_node, table, table_tmp);

                for (int i = 0; (buf_num_diff > 0) && (i < buf_num_per_table); i++) {
                    vsf_heap_free(table->buffer[i]);
                    buf_num_diff--;
                }
                vsf_heap_free(table);
                table = table_tmp;
            } else {
                vsf_heap_free(table->buffer[buf_idx]);
                table->buffer[buf_idx] = NULL;
                buf_num_diff--;
            }
        }
    }
    dynarr->length = size;
    return VSF_ERR_NONE;
}

void * vsf_dynarr_get(vsf_dynarr_t *dynarr, uint_fast16_t pos)
{
    if (pos < dynarr->length) {
        vsf_dynarr_table_t *table;
        uint_fast16_t item_idx, buf_idx;

        item_idx = pos & ((1UL << dynarr->item_num_per_buf_bitlen) - 1);
        pos >>= dynarr->item_num_per_buf_bitlen;
        buf_idx = pos & ((1UL << dynarr->buf_num_per_table_bitlen) - 1);

        table = vsf_dynarr_get_table(dynarr, buf_idx);
        if (table != NULL) {
            return (void *)((uint32_t *)table->buffer[buf_idx] +
                ((item_idx * dynarr->item_size) >> 2));
        }
    }
    return NULL;
}

vsf_err_t vsf_dynarr_init(vsf_dynarr_t *dynarr)
{
    dynarr->item_size = (dynarr->item_size + 3) & ~3;
    vsf_slist_init(&dynarr->table_list);
    dynarr->length = 0;
    return VSF_ERR_NONE;
}

void vsf_dynarr_fini(vsf_dynarr_t *dynarr)
{
    uint_fast8_t buf_num_per_table = 1UL << dynarr->buf_num_per_table_bitlen;
    uint_fast8_t item_num_per_buf = 1UL << dynarr->item_num_per_buf_bitlen;
    uint_fast16_t buf_num = (dynarr->length + item_num_per_buf - 1) >> dynarr->item_num_per_buf_bitlen;

    __vsf_slist_foreach_next_unsafe(vsf_dynarr_table_t, table_node, &dynarr->table_list) {
        for (int i = min(buf_num, buf_num_per_table); i > 0; i--) {
            vsf_heap_free(_->buffer[i - 1]);
            buf_num--;
        }
        vsf_heap_free(_);
    }

    vsf_dynarr_init(dynarr);
}

#endif      // VSF_USE_DYNARR
