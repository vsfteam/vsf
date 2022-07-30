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

#if     defined(__VSF_LOADER_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_LOADER_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_dcl_class(vsf_loader_t)

/**
 * \~english vsf loader memory attributes
 * @note TODO
 *
 * \~chinese vsf loader 内存属性
 * @note TODO
 */
enum vsf_loader_mem_attr_t {
    VSF_LOADER_MEM_R    = 1 << 0,
    VSF_LOADER_MEM_W    = 1 << 1,
    VSF_LOADER_MEM_X    = 1 << 2,
    VSF_LOADER_MEM_RW   = VSF_LOADER_MEM_R | VSF_LOADER_MEM_W,
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
     @param alignment memory alignment.
     @return memory allocated.

     \~chinese 内存分配函数。
     @param loader 加载器实例, 不能是空指针。
     @param attr 需要分配的内存属性（只读、只写、读写、执行）。
     @param size 需要分配的内存的字节大小。
     @param alignment 内存对其。
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
    void (*fn_free)(vsf_loader_t *loader, vsf_loader_mem_attr_t attr, void *buffer, uint32_t alignment);
} vsf_loader_heap_op_t;

typedef struct vsf_loader_target_t {
    /**
     \~english target, can be fd, pointer, etc.

     \~chinese 目标，可以是文件、指针等等。
    */
    uintptr_t taget;

    /**
     \~english whether xip is supported. For xip, target is the address of image.

     \~chinese 是否支持xip。对于XIP的话，target是镜像地址。
    */
    bool is_xip;

    /**
     \~english function to read taget to memory.
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
    uint32_t (*fn_read)(vsf_loader_t *loader, uint32_t offset, void *buffer, uint32_t size);
} vsf_loader_target_t;

vsf_class(vsf_loader_t) {
    public_member(
        const vsf_loader_heap_op_t *heap_op;
        vsf_loader_target_t *target;
    )

    protected_member(
        void * text;
        void * bss;
        void * data;
        void * rodata;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if defined(__VSF_LOADER_CLASS_IMPLEMENT) || defined(__VSF_LOADER_CLASS_INHERIT__)
/**
 \~english cleanup loader.
 @param loader loader instance, cannot be NULL.
 @return None.

 \~chinese 清理载入器。
 @param loader 载入器实例, 不能是空指针。
 @return 无返回值。
*/
extern void vsf_loader_cleanup(vsf_loader_t *loader);
#endif

#ifdef __cplusplus
}
#endif

#undef __VSF_LOADER_CLASS_IMPLEMENT
#undef __VSF_LOADER_CLASS_INHERIT__

/*============================ INCLUDES ======================================*/

#if VSF_LOADER_USE_ELF == ENABLED
#   include "./elf/vsf_elfloader.h"
#endif

/** @} */   // vsf_loader
/** @} */   // vsf_service

#endif      // VSF_USE_LOADER
#endif      // __VSF_LOADER_H__
