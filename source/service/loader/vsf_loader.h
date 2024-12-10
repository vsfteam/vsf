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

/**
 * \file vsf_loader.h
 * \brief vsf loader support
 *
 * provides a application loader implementation
 */

/** @ingroup vsf_service
 *  @{
 */

/** @defgroup vsf_loader vsf loader
 *  @{
 */

#ifndef __VSF_LOADER_H__
#define __VSF_LOADER_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"
#include "utilities/vsf_utilities.h"

#if VSF_USE_LOADER == ENABLED

#undef __VSF_LOADER_PUBLIC_CONST__
#if     defined(__VSF_LOADER_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#   define __VSF_LOADER_PUBLIC_CONST__
#elif   defined(__VSF_LOADER_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#   define __VSF_LOADER_PUBLIC_CONST__
#else
#   define __VSF_LOADER_PUBLIC_CONST__              const
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_loader_read(__target, __offset, __ptr, __size)                      \
        (__target)->fn_read((__target), (__offset), (__ptr), (__size))

#define vsf_loader_malloc(__loader, __attr, __size, __align)                    \
        (__loader)->heap_op->fn_malloc((vsf_loader_t *)(__loader), (__attr), (__size), (__align))
#define vsf_loader_free(__loader, __attr, __ptr)                                \
        (__loader)->heap_op->fn_free((vsf_loader_t *)(__loader), (__attr), (__ptr))

/*============================ TYPES =========================================*/

vsf_dcl_class(vsf_loader_t)

/**
 * \~english vsf loader memory attributes
 * @note TODO
 *
 * \~chinese vsf loader 内存属性
 * @note TODO
 */
typedef enum vsf_loader_mem_attr_t {
    VSF_LOADER_MEM_R    = 1 << 0,
    VSF_LOADER_MEM_W    = 1 << 1,
    VSF_LOADER_MEM_X    = 1 << 2,
    VSF_LOADER_MEM_RW   = VSF_LOADER_MEM_R | VSF_LOADER_MEM_W,
    VSF_LOADER_MEM_RWX  = VSF_LOADER_MEM_X | VSF_LOADER_MEM_RW,
} vsf_loader_mem_attr_t;

/**
 * \~english vsf loader heap functions
 * @note TODO
 *
 * \~chinese vsf loader 栈函数
 * @note TODO
 */
typedef struct vsf_loader_heap_op_t {
    /**
     \~english function to allocate memory buffer.
     @param loader loader instance, cannot be NULL.
     @param attr attribution of the memory(R/W/X/RW).
     @param size byte size of memory to allocate.
     @param alignment memory alignment: 2**alignment.
     @return memory allocated.

     \~chinese 内存分配函数。
     @param loader 加载器实例, 不能是空指针。
     @param attr 需要分配的内存属性（只读、只写、读写、执行）。
     @param size 需要分配的内存的字节大小。
     @param alignment 内存对其：2**alignment。
     @return 分配的内存。
    */
    void * (*fn_malloc)(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, uint32_t size, uint32_t alignment);

    /**
     \~english function to free allocated memory buffer.
     @param loader loader instance, cannot be NULL.
     @param attr attribution of the memory(R/W/X/RW).
     @param buffer memory to be freed.
     @return None.

     \~chinese 内存分配函数。
     @param loader 加载器实例, 不能是空指针。
     @param attr 需要分配的内存属性（只读、只写、读写、执行）。
     @param buffer 需要释放的内存。
     @return 无返回值。
    */
    void (*fn_free)(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, void *buffer);
} vsf_loader_heap_op_t;

typedef struct vsf_loader_target_t vsf_loader_target_t;
struct vsf_loader_target_t {
    /**
     \~english target object, can be fd, pointer, etc.

     \~chinese 目标，可以是文件、指针等等。
    */
    uintptr_t object;

    /**
     \~english whether xip is supported. Whether to use xip is detected in loader.
                If support_xip is true, target should be the address of the image.

     \~chinese 是否支持 xip。是否能够使用 xip 会在加载的时候自动检测。
                如果 support_xip 是 true 的话， target 必须是镜像地址。
    */
    bool support_xip;

    /**
     \~english function to read target to memory.
     @param loader loader instance, cannot be NULL.
     @param offset offset in target.
     @param buffer memory buffer for the read data.
     @param size memory size to read.
     @return actual size read.

     \~chinese 目标读取函数。
     @param loader 加载器实例, 不能是空指针。
     @param offset 目标中的偏移.
     @param buffer 读取数据的缓冲.
     @param size 读取的数据大小.
     @return 实际读取的大小。
    */
    uint32_t (*fn_read)(vsf_loader_target_t *target, uint32_t offset, void *buffer, uint32_t size);
};

typedef struct vsf_loader_op_t {
    int (*fn_load)(vsf_loader_t *loader, vsf_loader_target_t *target);
    void (*fn_cleanup)(vsf_loader_t *loader);
    int (*fn_call_init_array)(vsf_loader_t *loader);
    void (*fn_call_fini_array)(vsf_loader_t *loader);
    void * (*fn_remap)(vsf_loader_t *loader, void *vaddr);
} vsf_loader_op_t;


vsf_class(vsf_loader_t) {
    public_member(
        const vsf_loader_heap_op_t *heap_op;
        const vsf_loader_op_t *op;

        __VSF_LOADER_PUBLIC_CONST__ void *static_base;
        __VSF_LOADER_PUBLIC_CONST__ void *entry;
        __VSF_LOADER_PUBLIC_CONST__ void *vplt_out;

        void *vplt;
        void * (*alloc_vplt)(int num);
        void (*free_vplt)(void *vplt);
    )
    protected_member(
        vsf_loader_target_t *target;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_HEAP == ENABLED
/**
 \~english default heap op.
 @note alignment is not supported.

 \~chinese 默认heap操作。
 @note 不支持对其。
*/
extern const vsf_loader_heap_op_t vsf_loader_default_heap_op;
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english read function for xip target, set to fn_read member of vsf_loader_target_t.
 @param target loader target, object of target MUST be the address of image.
 @param offset offset in image.
 @param buffer buffer to hold the data read.
 @param size size to read.
 @return the actual data size read.

 \~chinese xip目标的读取函数，设置给vsf_loader_target_t的fn_read参数。
 @param target 载入目标，target的object参数必须是镜像地址。
 @param offset 镜像中的偏移
 @param buffer 读取缓存。
 @param size 读取大小。
 @return 实际读取大小。
*/
extern uint32_t vsf_loader_xip_read(vsf_loader_target_t *target, uint32_t offset, void *buffer, uint32_t size);

/**
 \~english read function for xip target, set to fn_read member of vsf_loader_target_t.
 @param target loader target, object of target MUST be the file opened with FILE * type.
 @param offset offset in image.
 @param buffer buffer to hold the data read.
 @param size size to read.
 @return the actual data size read.

 \~chinese xip目标的读取函数，设置给vsf_loader_target_t的fn_read参数。
 @param target 载入目标，target的object参数必须是已经打开的 FILE *f。
 @param offset 镜像中的偏移
 @param buffer 读取缓存。
 @param size 读取大小。
 @return 实际读取大小。
*/
extern uint32_t vsf_loader_stdio_read(vsf_loader_target_t *target, uint32_t offset, void *buffer, uint32_t size);

#if defined(__VSF_LOADER_CLASS_IMPLEMENT) || defined(__VSF_LOADER_CLASS_INHERIT__)
extern void * vsf_loader_link(vsf_loader_t *loader, const char *name);
#endif

extern int vsf_loader_load(vsf_loader_t *loader, vsf_loader_target_t *target);
extern void vsf_loader_cleanup(vsf_loader_t *loader);
extern int vsf_loader_call_init_array(vsf_loader_t *loader);
extern void vsf_loader_call_fini_array(vsf_loader_t *loader);
extern void * vsf_loader_remap(vsf_loader_t *loader, void *vaddr);

#ifdef __cplusplus
}
#endif

#undef __VSF_LOADER_CLASS_IMPLEMENT
#undef __VSF_LOADER_CLASS_INHERIT__

/*============================ INCLUDES ======================================*/

#if VSF_LOADER_USE_ELF == ENABLED
#   include "./elf/vsf_elfloader.h"
#endif
#if VSF_LOADER_USE_PE == ENABLED
#   include "./pe/vsf_peloader.h"
#endif

/** @} */   // vsf_loader
/** @} */   // vsf_service

#endif      // VSF_USE_LOADER
#endif      // __VSF_LOADER_H__
