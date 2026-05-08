/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
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

/*
 * esp_heap_caps_init.h -> vsf_heap instance bridge.
 *
 * Maintains a dynamic singly-linked list of registered {caps, heap, buffer}
 * entries, mirroring ESP-IDF's SLIST_FOREACH over registered_heaps.
 */

#define __VSF_HEAP_CLASS_IMPLEMENT

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_HEAP_CAPS == ENABLED

#include "esp_heap_caps_init.h"
#include "esp_heap_caps.h"
#include "../vsf_espidf.h"
#include "service/heap/vsf_heap.h"
#include "kernel/vsf_kernel.h"

#include <string.h>

struct vsf_espidf_multi_heap {
    implement(vsf_heap_t)
    vsf_dlist_t   __freelist[20];
};

typedef struct heap_reg_entry {
    struct heap_reg_entry           *next;
    uint32_t                         caps;
    intptr_t                         start;
    intptr_t                         end;
    struct vsf_espidf_multi_heap      mh;
} heap_reg_entry_t;

static heap_reg_entry_t *__vsf_espidf_heap_reg;

void heap_caps_init(void)
{
}

void heap_caps_enable_nonos_stack_heaps(void)
{
}

static esp_err_t __heap_caps_add_region_internal(const uint32_t *caps, intptr_t start, intptr_t end)
{
    if (start >= end) {
        return ESP_ERR_INVALID_ARG;
    }

    heap_reg_entry_t *ent;
    ent = (heap_reg_entry_t *)vsf_heap_malloc(sizeof(*ent));
    if (ent == NULL) {
        return ESP_ERR_NO_MEM;
    }
    memset(ent, 0, sizeof(*ent));

    ent->start = start;
    ent->end   = end;
    ent->caps  = (caps != NULL) ? caps[0] : MALLOC_CAP_DEFAULT;

    ent->mh.freelist     = &ent->mh.__freelist[0];
    ent->mh.freelist_num = dimof(ent->mh.__freelist);
    __vsf_heap_init(&ent->mh.use_as__vsf_heap_t);
    __vsf_heap_add_buffer(&ent->mh.use_as__vsf_heap_t, (uint8_t *)start, (uint_fast32_t)(end - start));

    vsf_protect_t orig = vsf_protect_sched();
        ent->next = __vsf_espidf_heap_reg;
        __vsf_espidf_heap_reg = ent;
    vsf_unprotect_sched(orig);

    return ESP_OK;
}

esp_err_t heap_caps_add_region(intptr_t start, intptr_t end)
{
    return __heap_caps_add_region_internal(NULL, start, end);
}

esp_err_t heap_caps_add_region_with_caps(const uint32_t caps[], intptr_t start, intptr_t end)
{
    if (caps == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return __heap_caps_add_region_internal(caps, start, end);
}

#endif      // VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_HEAP_CAPS
