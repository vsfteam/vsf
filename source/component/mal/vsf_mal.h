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

#ifndef __VSF_MAL_H__
#define __VSF_MAL_H__

/*! \brief  This module provides MAL(Memory Abstraction Layser) device class.
 *! \author SimonQian
 *! \note   Supports read/write/erase operation in block granularity.
 *!         Enabled by VSF_USE_MAL. Drivers:
 *!         mim_mal(mim: mal in mal) by VSF_MAL_USE_MIM_MAL:
 *!             Used to derive a mal device from partial of another mal device.
 *!         fakefat32_mal by VSF_MAL_USE_FAKEFAT32_MAL, depends on VSF_USE_FS/VSF_FS_USE_MEMFS/VSF_FS_USE_FATFS:
 *!             Generate a vitual fat32 mal device.
 *!         file_mal by VSF_MAL_USE_FILE_MAL, depends on VSF_USE_FS:
 *!             Generate a mal device from a file.
 *!         flash_mal by VSF_MAL_USE_FLASH_MAL, depends on VSF_HAL_USE_FLASH:
 *!             Generate a mal device from partial of the FLASH in HAL.
 *!         mem_mal by VSF_MAL_USE_MEM_MAL:
 *!             Generate a mal device from io-accessable memory.
 *!         mmc_mal by VSF_MAL_USE_MMC_MAL:
 *!             Mal device from a MMC card.
 *!         scsi_mal by VSF_MAL_USE_SCSI_MAL, depnends on VSF_USE_SCSI:
 *!             Mal device from a SCSI device.
 */

/*============================ INCLUDES ======================================*/

#include "./vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED

#include "kernel/vsf_kernel.h"

#if     defined(__VSF_MAL_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_MAL_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use mal
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_dcl_class(vk_mal_t)
#if VSF_USE_SIMPLE_STREAM == ENABLED
vsf_dcl_class(vk_mal_stream_t)
#endif

vsf_dcl_class(vk_mal_drv_t)

typedef enum vsf_mal_op_t {
    VSF_MAL_OP_ERASE,
    VSF_MAL_OP_READ,
    VSF_MAL_OP_WRITE,
} vsf_mal_op_t;

typedef enum vsf_mal_feature_t {
    VSF_MAL_READABLE            = 1 << 0,
    VSF_MAL_WRITABLE            = 1 << 1,
    VSF_MAL_ERASABLE            = 1 << 2,
    VSF_MAL_NON_UNIFIED_BLOCK   = 1 << 3,
    VSF_MAL_LOCAL_BUFFER        = 1 << 4,
} vsf_mal_feature_t;

/**
 \~english
 @brief mal driver

 \~chinese
 @brief mal 驱动
 */
vsf_class(vk_mal_drv_t) {
    protected_member(
        /**
         \~english get block size for a dedicated operation specified by addr/size/operation
         @note must be called after mal is successfully initialized
         \~chinese 根据指定 addr/size/operation 的操作，获得块大小
         @note 必须在正常初始化之后，才能调用
        */
        uint_fast32_t (*blksz)(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
        /**
         \~english get buffer for read/write if VSF_MAL_LOCAL_BUFFER is enabled
         @note must be called after mal is successfully initialized
         \~chinese 如果具备 VSF_MAL_LOCAL_BUFFER 特性的话，可获得本地读写缓冲
         @note 必须在正常初始化之后，才能调用
        */
        bool (*buffer)(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
        uint8_t init_local_size;
        uint8_t fini_local_size;
        uint8_t erase_local_size;
        uint8_t read_local_size;
        uint8_t write_local_size;
        /**
         \~english initialilze mal
         \~chinese 执行初始化操作
        */
        vsf_peda_evthandler_t init;
        /**
         \~english finilize mal
         \~chinese 执行终止化操作
        */
        vsf_peda_evthandler_t fini;
        /**
         \~english erase mal
         \~chinese 执行擦除操作
        */
        vsf_peda_evthandler_t erase;
        /**
         \~english read mal
         \~chinese 执行读取操作
        */
        vsf_peda_evthandler_t read;
        /**
         \~english write mal
         \~chinese 执行写入操作
        */
        vsf_peda_evthandler_t write;
    )
};

/**
 \~english
 @brief mal class

 \~chinese
 @brief mal类
 */
vsf_class(vk_mal_t) {
    public_member(
        /**
         \~english pointer to mal driver
         \~chinese mal 驱动指针
        */
        const vk_mal_drv_t *drv;
        /**
         \~english parameter of mal, type of which is specified by the driver
         \~chinese mal 参数，类型由具体的驱动决定
        */
        void *param;
        /**
         \~english size of mal in byte
         \~chinese mal 字节大小
        */
        uint64_t size;
        /**
         \~english feature of mal, ored by value in vsf_mal_feature_t
         \~chinese mal 特性，由 vsf_mal_feature_t 中的值组成
        */
        uint8_t feature;
    )
};

#if VSF_USE_SIMPLE_STREAM == ENABLED
/**
 \~english
 @brief mal stream class

 \~chinese
 @brief mal 流类
 */
vsf_class(vk_mal_stream_t) {
    public_member(
        /**
         \~english the mal instance
         \~chinese mal 实例
        */
        vk_mal_t *mal;
    )
    protected_member(
        uint64_t addr;
        uint32_t size;
        uint32_t rw_size;
        vsf_stream_t *stream;
        uint32_t cur_size;
        uint8_t *cur_buff;
        vsf_eda_t *cur_eda;
    )
};
#endif

#if defined(__VSF_MAL_CLASS_IMPLEMENT) || defined(__VSF_MAL_CLASS_INHERIT__)
__vsf_component_peda_ifs(vk_mal_init)
__vsf_component_peda_ifs(vk_mal_fini)
__vsf_component_peda_ifs(vk_mal_erase,
    uint64_t addr;
    uint32_t size;
)
__vsf_component_peda_ifs(vk_mal_read,
    uint64_t addr;
    uint32_t size;
    uint8_t *buff;
    uint32_t rsize;
)
__vsf_component_peda_ifs(vk_mal_write,
    uint64_t addr;
    uint32_t size;
    uint8_t *buff;
    uint32_t wsize;
)
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
/**
 \~english reentrant mal class
 @note Normally, a mal instance is not reentrant. Follow the steps to get multiple
        mal instance which is protect with a common mutex.
        1. prepare a common mutex instance for malA
        2. in each task which will asscess malA, implement a vk_reentrant_mal_t instance.
            mal member points to malA, mutex member points to the common mutex in step 1,
          offset member is the byte offset in malA, size member of vk_mal_t is the size of
          the reentrant mal instance.
        3. each vk_reentrant_mal_t instance in each task will be protected by the common mutex.

 \~chinese 可重入 mal 类
 @note 一般而言， mal 实例不是可重入的。按照如下步骤，可得到多个由同一个互斥量保护的可重入 mal 实例。
        1. 为 malA 准备一个互斥量实例
        2. 每个会访问 malA 的任务里，实现一个 vk_reentrant_mal_t 实例。
            mal 成员指向 malA 、 mutex 成员指向第一步中的互斥量实例、 offset 成员是 malA 中的字节偏移、
          vk_mal_t 中的 size 成员是当前可重入 mal 实例的字节大小。
        3. 每个任务里的 vk_reentrant_mal_t 实例，会被同一个 mutex 保护，实现了可重入。

*/
vsf_class(vk_reentrant_mal_t) {
    public_member(
        implement(vk_mal_t)

        vk_mal_t *mal;
        vsf_mutex_t *mutex;
        uint64_t offset;
    )
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
/**
 \~english
 @brief class driver of vk_reentrant_mal_t, used to bet set to the drv member in vk_mal_t

 \~chinese
 @brief vk_reentrant_mal_t 的类驱动，用于设置给 vk_mal_t 的 drv 成员
 */
extern const vk_mal_drv_t vk_reentrant_mal_drv;
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief prepare subcall to initialize a mal instance.
 @note A mal instance MUST be successfully initialized before calling other mal APIs.
        The initialization is done by a subcall, so if vk_mal_init returns VSF_ERR_NONE,
        it does not mean that the initialization is done. It depends on the caller context.
        1. If caller is a stack-owner thread, vk_mal_init will return after subcall is done.
            The result of initialization can be got by vsf_eda_get_return_value, with type
            vsf_err_t.
        2. If caller is not a stack-owner thread, the initializaion subcall will start after
            exiting of current event handler. After the subcall is done, VSF_EVT_RETURN will
            be send to the caller task. And vsf_eda_get_return_value could be called to get
            the result with type vsf_err_t.
 @param[in] pthis: mal instance
 @return vsf_err_t: VSF_ERR_NONE if subcall is ready to start, or a negative error code

 \~chinese
 @brief 准备 mal 初始化子调用
 @note mal 实例必须先初始化，然后才能调用其他 mal 的 API 。
        实际初始化是在子调用里执行的，所以即使 vk_mal_init 返回VSF_ERR_NONE，也不意味着 mal 初始化
        完成。这里根据调用上下文，分 2 种情况：
        1. 调用者是独立堆栈的线程， vk_mal_ini 会在子调用完成后返回。
            初始化结果可以通过 vsf_eda_get_return_value 来得到，类型为 vsf_err_t 。
        2. 调用者是共享堆栈的任务，初始化子调用会在当前事件处理程序返回后，开始执行。执行完成后，
            会向调用任务发送 VSF_EVT_RETURN 事件，在处理这个事件的时候，可以通过
            vsf_eda_get_return_value 来得到初始化清执行结果，类型为 vsf_err_t 。
 @param[in] pthis: mal 实例
 @return vsf_err_t: 如果子调用就绪，则返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vk_mal_init(vk_mal_t *pthis);

/**
 \~english
 @brief prepare subcall to finialize a mal instance.
 @note similar to vk_mal_init.
 @param[in] pthis: mal instance
 @return vsf_err_t: VSF_ERR_NONE if subcall is ready to start, or a negative error code

 \~chinese
 @brief 准备 mal 终止化子调用
 @note 和 vk_mal_init 类似。
 @param[in] pthis: mal 实例
 @return vsf_err_t: 如果子调用就绪，则返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vk_mal_fini(vk_mal_t *pthis);

/**
 \~english
 @brief get the block size of a dedicated operation.
 @param[in] pthis: mal instance
 @param[in] addr: address(starting from 0) to operate
 @param[in] size: size in bytes to operation
 @param[in] op: the operation, one in vsf_mal_op_t
 @return block size in bytes: if 0 means the specified operation is not supported

 \~chinese
 @brief 根据特定操作，获得块大小
 @param[in] pthis: mal 实例
 @param[in] addr: 操作的地址，从 0 开始
 @param[in] size: 操作的字节大小
 @param[in] op: vsf_mal_op_t 里定义的操作
 @return 块字节大小: 如果返回 0 ，表示不支持指定的操作
 */
extern uint_fast32_t vk_mal_blksz(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);

/**
 \~english
 @brief get the local memory of a dedicated operation, if mal instance has VSF_MAL_LOCAL_BUFFER feature.
 @param[in] pthis: mal instance
 @param[in] addr: address(starting from 0) to operate
 @param[in] size: size in bytes to operation
 @param[in] op: the operation, one in vsf_mal_op_t
 @param[out] mem: the memory returned
 @return true: a valid local memory is returned; false: local memory is not supported

 \~chinese
 @brief 根据特定操作，获得本地缓冲大小， mal 实例具备 VSF_MAL_LOCAL_BUFFER 特性的时候，可以获得本地缓冲。
 @param[in] pthis: mal 实例
 @param[in] addr: 操作的地址，从 0 开始
 @param[in] size: 操作的字节大小
 @param[in] op: vsf_mal_op_t 里定义的操作
 @param[out] mem: 返回的本地缓冲
 @return true: 有效的本地缓冲已经返回；false: 不支持本地缓冲
 */
extern bool vk_mal_prepare_buffer(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);

/**
 \~english
 @brief prepare subcall to erase a mal instance.
 @note similar to vk_mal_init.
 @param[in] pthis: mal instance
 @param[in] addr: address(starting from 0) to operate
 @param[in] size: size in bytes to operation
 @return vsf_err_t: VSF_ERR_NONE if subcall is ready to start, or a negative error code

 \~chinese
 @brief 准备 mal 擦除子调用
 @note 和 vk_mal_init 类似。
 @param[in] pthis: mal 实例
 @param[in] addr: 操作的地址，从 0 开始
 @param[in] size: 操作的字节大小
 @return vsf_err_t: 如果子调用就绪，则返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vk_mal_erase(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size);

/**
 \~english
 @brief prepare subcall to read a mal instance.
 @note similar to vk_mal_init.
        The result of subcall from vsf_eda_get_return_value is int32_t type.
        result < 0 means error; result >= 0 means the actual byte size read.
 @param[in] pthis: mal instance
 @param[in] addr: address(starting from 0) to read
 @param[in] size: size in bytes to read
 @param[in] buffer: data buffer for reading
 @return vsf_err_t: VSF_ERR_NONE if subcall is ready to start, or a negative error code

 \~chinese
 @brief 准备 mal 读取子调用
 @note 和 vk_mal_init 类似。
        通过 vsf_eda_get_return_value 得到的子调用返回值类型是int32_t。
        返回值小于 0 表示错误，大于等于 0 表示实际读取的字节数。
 @param[in] pthis: mal 实例
 @param[in] addr: 读取的地址，从 0 开始
 @param[in] size: 读取的字节大小
 @param[in] buffer: 数据缓冲
 @return vsf_err_t: 如果子调用就绪，则返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vk_mal_read(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff);

/**
 \~english
 @brief prepare subcall to write a mal instance.
 @note similar to vk_mal_init.
        The result of subcall from vsf_eda_get_return_value is int32_t type.
        result < 0 means error; result >= 0 means the actual byte size written.
 @param[in] pthis: mal instance
 @param[in] addr: address(starting from 0) to write
 @param[in] size: size in bytes to write
 @param[in] buffer: data buffer for writing
 @return vsf_err_t: VSF_ERR_NONE if subcall is ready to start, or a negative error code

 \~chinese
 @brief 准备 mal 写入子调用
 @note 和 vk_mal_init 类似。
        通过 vsf_eda_get_return_value 得到的子调用返回值类型是int32_t。
        返回值小于 0 表示错误，大于等于 0 表示实际写入的字节数。
 @param[in] pthis: mal 实例
 @param[in] addr: 读取的地址，从 0 开始
 @param[in] size: 读取的字节大小
 @param[in] buffer: 数据缓冲
 @return vsf_err_t: 如果子调用就绪，则返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vk_mal_write(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff);

#if VSF_USE_SIMPLE_STREAM == ENABLED
/**
 \~english
 @brief prepare subcall to read mal instance into a stream.
 @note similar to vk_mal_init.
        stream will be connected/disconnected automatically.
        The result of subcall from vsf_eda_get_return_value is int32_t type.
        result < 0 means error; result >= 0 means the actual byte size read.
 @param[in] pthis: mal_stream instance
 @param[in] addr: address(starting from 0) to read
 @param[in] size: size in bytes to read
 @param[in] stream: stream for reading
 @return vsf_err_t: VSF_ERR_NONE if stream is started, or a negative error code

 \~chinese
 @brief 准备 mal 流读取子调用
 @note 和 vk_mal_init 类似。数据流会自动连接和断开。
        数据流会自动连接和断开。
        通过 vsf_eda_get_return_value 得到的子调用返回值类型是int32_t。
        返回值小于 0 表示错误，大于等于 0 表示实际读取的字节数。
 @param[in] pthis: mal_stream 实例
 @param[in] addr: 读取的地址，从 0 开始
 @param[in] size: 读取的字节大小
 @param[in] stream: 数据流
 @return vsf_err_t: 如果读取数据流子调用就读，则返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vk_mal_read_stream(vk_mal_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream);

/**
 \~english
 @brief prepare subcall to write a stream into mal instance.
 @note similar to vk_mal_init.
        stream will be connected/disconnected automatically.
        The result of subcall from vsf_eda_get_return_value is int32_t type.
        result < 0 means error; result >= 0 means the actual byte size written.
 @param[in] pthis: mal_stream instance
 @param[in] addr: address(starting from 0) to write
 @param[in] size: size in bytes to write
 @param[in] stream: stream for writing
 @return vsf_err_t: VSF_ERR_NONE if stream is started, or a negative error code

 \~chinese
 @brief 准备 mal 流写入子调用
 @note 和 vk_mal_init 类似。
        数据流会自动连接和断开。
        通过 vsf_eda_get_return_value 得到的子调用返回值类型是int32_t。
        返回值小于 0 表示错误，大于等于 0 表示实际写入的字节数。
 @param[in] pthis: mal 实例
 @param[in] addr: 写入的地址，从 0 开始
 @param[in] size: 写入的字节大小
 @param[in] buffer: 数据流
 @return vsf_err_t: 如果写入数据流自调子就读，则返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vk_mal_write_stream(vk_mal_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream);
#endif

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#include "./driver/mim_mal/vsf_mim_mal.h"
#include "./driver/mem_mal/vsf_mem_mal.h"
#include "./driver/fakefat32_mal/vsf_fakefat32_mal.h"
#include "./driver/scsi_mal/vsf_scsi_mal.h"
#include "./driver/file_mal/vsf_file_mal.h"
#include "./driver/flash_mal/vsf_flash_mal.h"
#include "./driver/mmc_mal/vsf_mmc_mal.h"
#include "./driver/cached_mal/vsf_cached_mal.h"

#undef __VSF_MAL_CLASS_IMPLEMENT
#undef __VSF_MAL_CLASS_INHERIT__

#endif      // VSF_USE_MAL
#endif      // __VSF_MAL_H__
