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

// for file operation in vsf_loader_stdio_read
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_ARCH_PROVIDE_EXE == ENABLED
static void * __vsf_loader_arch_exe_alloc(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, uint32_t size, uint32_t alignment);
static void __vsf_loader_arch_exe_free(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, void *buffer);
#elif VSF_USE_HEAP == ENABLED
static void * __vsf_loader_heap_malloc(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, uint32_t size, uint32_t alignment);
static void __vsf_loader_heap_free(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, void *buffer);
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_loader_heap_op_t vsf_loader_default_heap_op = {
#if VSF_ARCH_PROVIDE_EXE == ENABLED
    .fn_malloc  = __vsf_loader_arch_exe_alloc,
    .fn_free    = __vsf_loader_arch_exe_free,
#elif VSF_USE_HEAP == ENABLED
    .fn_malloc  = __vsf_loader_heap_malloc,
    .fn_free    = __vsf_loader_heap_free,
#else
#   warning no valid allocator
    .fn_malloc  = NULL,
    .fn_free    = NULL,
#endif
};

/*============================ IMPLEMENTATION ================================*/

#if VSF_ARCH_PROVIDE_EXE == ENABLED
static void * __vsf_loader_arch_exe_alloc(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, uint32_t size, uint32_t alignment)
{
    return vsf_arch_alloc_exe(size);
}

static void __vsf_loader_arch_exe_free(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, void *buffer)
{
    vsf_arch_free_exe(buffer);
}
#elif VSF_USE_HEAP == ENABLED
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

VSF_CAL_WEAK(vsf_loader_link)
void * vsf_loader_link(vsf_loader_t *loader, const char *name)
{
    return NULL;
}

int vsf_loader_load(vsf_loader_t *loader, vsf_loader_target_t *target)
{
    VSF_SERVICE_ASSERT((loader != NULL) && (loader->op != NULL) && (loader->op->fn_load != NULL));
    return loader->op->fn_load(loader, target);
}

void vsf_loader_cleanup(vsf_loader_t *loader)
{
    VSF_SERVICE_ASSERT((loader != NULL) && (loader->op != NULL) && (loader->op->fn_cleanup != NULL));
    loader->op->fn_cleanup(loader);
}

int vsf_loader_call_init_array(vsf_loader_t *loader)
{
    VSF_SERVICE_ASSERT((loader != NULL) && (loader->op != NULL) && (loader->op->fn_call_init_array != NULL));
    return loader->op->fn_call_init_array(loader);
}

void vsf_loader_call_fini_array(vsf_loader_t *loader)
{
    VSF_SERVICE_ASSERT((loader != NULL) && (loader->op != NULL) && (loader->op->fn_call_fini_array != NULL));
    loader->op->fn_call_fini_array(loader);
}

void * vsf_loader_remap(vsf_loader_t *loader, void *vaddr)
{
    VSF_SERVICE_ASSERT((loader != NULL) && (loader->op != NULL));
    if (NULL == loader->op->fn_remap) {
        return NULL;
    }
    return loader->op->fn_remap(loader, vaddr);
}

#endif      // VSF_USE_LOADER
