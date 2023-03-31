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
#include "service/vsf_service_cfg.h"

#if VSF_USE_LOADER == ENABLED

#define __VSF_LOADER_CLASS_IMPLEMENT
#include "./vsf_loader.h"

#if VSF_USE_HEAP == ENABLED
#   include "service/heap/vsf_heap.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static void * __vsf_loader_heap_malloc(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, uint32_t size, uint32_t alignment);
static void __vsf_loader_heap_free(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, void *buffer);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_HEAP == ENABLED
const vsf_loader_heap_op_t vsf_loader_default_heap_op = {
    .fn_malloc  = __vsf_loader_heap_malloc,
    .fn_free    = __vsf_loader_heap_free,
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_HEAP == ENABLED
static void * __vsf_loader_heap_malloc(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, uint32_t size, uint32_t alignment)
{
    return vsf_heap_malloc(size);
}

static void __vsf_loader_heap_free(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, void *buffer)
{
    vsf_heap_free(buffer);
}
#endif

uint32_t vsf_loader_stdio_read(vsf_loader_target_t *target, uint32_t offset, void *buffer, uint32_t size)
{
    FILE *f = (FILE *)target->object;
    if (fseek(f, offset, SEEK_SET) != 0) {
        return 0;
    }
    return fread(buffer, 1, size, f);
}

uint32_t vsf_loader_xip_read(vsf_loader_target_t *target, uint32_t offset, void *buffer, uint32_t size)
{
    memcpy(buffer, (void *)(target->object + offset), size);
    return size;
}

WEAK(vsf_loader_link)
void * vsf_loader_link(vsf_loader_t *loader, const char *name)
{
    return NULL;
}

#endif      // VSF_USE_LOADER
