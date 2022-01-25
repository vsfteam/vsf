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
 * \file vsf_simple_stream.h
 * \brief vsf simple stream
 *
 * provides a simple stream implementation
 */

/** @ingroup vsf_service
 *  @{
 */

/** @defgroup vsf_simple_stream vsf simple stream
 *  @{
 */

#ifndef __VSF_SIMPLE_STREAM_H__
#define __VSF_SIMPLE_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"
#include "utilities/vsf_utilities.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED

#if     defined(__VSF_SIMPLE_STREAM_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_SIMPLE_STREAM_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/**
 *  \~english
 *  vsf stream ticktock configuration, can be ENABLED or DISABLE.
 *  Default value is ENABLED, it can be predefined in vsf_usr_cfg*.h
 *
 *  \~chinese
 *  vsf stream ticktock 配置, 可以是 ENABLED 或者 DISABLE。
 *  默认值是ENABLED，可以在 vsf_usr_cfg*.h 里预定义
 */
#ifndef VSF_STREAM_CFG_TICKTOCK
#   define VSF_STREAM_CFG_TICKTOCK                      ENABLED
#endif

/**
 *  \~english
 *  vsf stream threshold configuration, can be ENABLED or DISABLE.
 *  Default value is ENABLED, it can be predefined in vsf_usr_cfg*.h
 *
 *  \~chinese
 *  vsf stream阈值配置, 可以是 ENABLED 或者 DISABLE。
 *  默认值是ENABLED，可以在 vsf_usr_cfg*.h 里预定义
 */
#ifndef VSF_STREAM_CFG_THRESHOLD
#   define VSF_STREAM_CFG_THRESHOLD                     ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

// VSF_STREAM_XXX in upper case is compatible with all stream classes
#define VSF_STREAM_INIT(__stream)                       vsf_stream_init((vsf_stream_t *)(__stream))
#define VSF_STREAM_FINI(__stream)                       vsf_stream_fini((vsf_stream_t *)(__stream))
#define VSF_STREAM_WRITE(__stream, __buf, __size)       vsf_stream_write((vsf_stream_t *)(__stream), (__buf), (__size))
#define VSF_STREAM_READ(__stream, __buf, __size)        vsf_stream_read((vsf_stream_t *)(__stream), (__buf), (__size))
#define VSF_STREAM_GET_BUFF_SIZE(__stream)              vsf_stream_get_buff_size((vsf_stream_t *)(__stream))
#define VSF_STREAM_GET_DATA_SIZE(__stream)              vsf_stream_get_data_size((vsf_stream_t *)(__stream))
#define VSF_STREAM_GET_FREE_SIZE(__stream)              vsf_stream_get_free_size((vsf_stream_t *)(__stream))
#define VSF_STREAM_GET_WBUF(__stream, p)                vsf_stream_get_wbuf((vsf_stream_t *)(__stream), (p))
#define VSF_STREAM_GET_RBUF(__stream, p)                vsf_stream_get_rbuf((vsf_stream_t *)(__stream), (p))
#define VSF_STREAM_CONNECT_RX(__stream)                 vsf_stream_connect_rx((vsf_stream_t *)(__stream))
#define VSF_STREAM_CONNECT_TX(__stream)                 vsf_stream_connect_tx((vsf_stream_t *)(__stream))
#define VSF_STREAM_DISCONNECT_RX(__stream)              vsf_stream_disconnect_rx((vsf_stream_t *)(__stream))
#define VSF_STREAM_DISCONNECT_TX(__stream)              vsf_stream_disconnect_tx((vsf_stream_t *)(__stream))
#define VSF_STREAM_IS_RX_CONNECTED(__stream)            vsf_stream_is_rx_connected((vsf_stream_t *)(__stream))
#define VSF_STREAM_IS_TX_CONNECTED(__stream)            vsf_stream_is_tx_connected((vsf_stream_t *)(__stream))
#if VSF_STREAM_CFG_THRESHOLD == ENABLED
// set threshold CAN ONLY be called after stream_init
#define VSF_STREAM_SET_RX_THRESHOLD(__stream, __thres)  vsf_stream_set_rx_threshold((vsf_stream_t *)(__stream), (__thres))
#define VSF_STREAM_SET_TX_THRESHOLD(__stream, __thres)  vsf_stream_set_tx_threshold((vsf_stream_t *)(__stream), (__thres))
#endif

/*============================ TYPES =========================================*/

vsf_dcl_class(vsf_stream_t)
vsf_dcl_class(vsf_stream_terminal_t)
vsf_dcl_class(vsf_stream_op_t)

/**
 *  \~english
 *  vsf stream event
 *
 *  \~chinese
 *  vsf stream 事件
 */
typedef enum vsf_stream_evt_t {
    VSF_STREAM_ON_CONNECT,                  /**< stream on connect event    */
    VSF_STREAM_ON_DISCONNECT,               /**< stream on disconnect event */
    VSF_STREAM_ON_IN,                       /**< stream on in event         */
    VSF_STREAM_ON_RX = VSF_STREAM_ON_IN,    /**< stream on rx event         */
    VSF_STREAM_ON_OUT,                      /**< stream on out event        */
    VSF_STREAM_ON_TX = VSF_STREAM_ON_OUT,   /**< stream on tx event         */
} vsf_stream_evt_t;

/**
 * \~english vsf steam operating functions
 * @note TODO
 *
 * \~chinese vsf steam 操作函数
 * @note TODO
 */
vsf_class(vsf_stream_op_t) {
    protected_member(
        /**
         \~english initialization function, must be called before other API.
         @param stream stream instance, cannot be NULL.
         @return None.

         \~chinese 初始化函数，必须在其他API之前调用。
         @param stream 流实例, 不能是空指针
         @return 无返回值。
         */
        void (*init)(vsf_stream_t *stream);
        void (*fini)(vsf_stream_t *stream);
        // for read/write, if buffer->buffer is NULL,
        //         then do dummy read/write of buffer->size

        /**
         \~english write function, must be called after init.
         @param stream stream instance, cannot be NULL.
         @param buf buffer pointer, poin to a buffer or NULL
         @param size write size(bytes), if buffer is NULL, then do dummy write.

         \~chinese 初始化函数，必须在其他API之前调用。
         @param stream 流实例, 不能是空指针
         @param buf 缓冲指针, 指向一块缓冲或者是NULL
         @param size 写大小(字节单位), 如果缓冲区是NULL, 将写给定大小的虚假数据
         */
        uint_fast32_t (*write)(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);

        /**
         \~english read function, must be called after init.
         @param stream stream instance, cannot be NULL.
         @param buf buffer pointer, poin to a buffer or NULL
         @param size read size(bytes), if buffer is NULL, then do dummy read.

         \~chinese 初始化函数，必须在其他API之前调用。
         @param stream 流实例, 不能是空指针
         @param buf 缓冲指针, 指向一块缓冲或者是NULL
         @param size 读大小(字节单位), 如果缓冲区是NULL, 将读给定大小的虚假数据
         */
        uint_fast32_t (*read)(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
        uint_fast32_t (*get_buff_length)(vsf_stream_t *stream);
        uint_fast32_t (*get_data_length)(vsf_stream_t *stream);
        uint_fast32_t (*get_avail_length)(vsf_stream_t *stream);
        // get consequent buffer for read/write
        uint_fast32_t (*get_wbuf)(vsf_stream_t *stream, uint8_t **ptr);
        uint_fast32_t (*get_rbuf)(vsf_stream_t *stream, uint8_t **ptr);
    )
};

vsf_class(vsf_stream_terminal_t) {

    public_member(
        void *param;
        void (*evthandler)(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt);
    )

    private_member(
#if VSF_STREAM_CFG_THRESHOLD == ENABLED
        uint32_t threshold;
#endif
        bool ready;
        bool data_notified;
    )
};

vsf_class(vsf_stream_t) {

    public_member(
        vsf_stream_op_t const *op;
#if VSF_STREAM_CFG_TICKTOCK == ENABLED
        bool is_ticktock_read;
        bool is_ticktock_write;
#endif
    )

    protected_member(
        union {
            struct {
                vsf_stream_terminal_t tx;
                vsf_stream_terminal_t rx;
            };
            vsf_stream_terminal_t terminal[2];
        };
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 * \~english initialization function, must be called before other API.
 * @param stream stream instance, cannot be NULL.
 *
 * \~chinese 初始化函数，必须在其他API之前调用。
 * @param stream 流实例, 不能是空指针
 */
extern vsf_err_t vsf_stream_init(vsf_stream_t *stream);
extern vsf_err_t vsf_stream_fini(vsf_stream_t *stream);
/**
 * \~english read function, must be called after init.
 * @param stream stream instance, cannot be NULL.
 * @param buf buffer pointer, poin to a buffer or NULL
 * @param size read size(bytes), if buffer is NULL, then do dummy read.
 *
 * \~chinese 初始化函数，必须在其他API之前调用。
 * @param stream 流实例, 不能是空指针
 * @param buf 缓冲指针, 指向一块缓冲或者是NULL
 * @param size 读大小(字节单位), 如果缓冲区是NULL, 将读给定大小的虚假数据
 */
extern uint_fast32_t vsf_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
extern uint_fast32_t vsf_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
#if VSF_STREAM_CFG_THRESHOLD == ENABLED
// set threshold CAN ONLY be called after stream_init
extern void vsf_stream_set_tx_threshold(vsf_stream_t *stream, uint_fast32_t threshold);
extern void vsf_stream_set_rx_threshold(vsf_stream_t *stream, uint_fast32_t threshold);
#endif
extern uint_fast32_t vsf_stream_get_buff_size(vsf_stream_t *stream);
extern uint_fast32_t vsf_stream_get_data_size(vsf_stream_t *stream);
extern uint_fast32_t vsf_stream_get_free_size(vsf_stream_t *stream);
extern uint_fast32_t vsf_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr);
extern uint_fast32_t vsf_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr);
extern void vsf_stream_connect_rx(vsf_stream_t *stream);
extern void vsf_stream_connect_tx(vsf_stream_t *stream);
extern void vsf_stream_disconnect_rx(vsf_stream_t *stream);
extern void vsf_stream_disconnect_tx(vsf_stream_t *stream);
extern bool vsf_stream_is_rx_connected(vsf_stream_t *stream);
extern bool vsf_stream_is_tx_connected(vsf_stream_t *stream);

#if     defined(__VSF_SIMPLE_STREAM_CLASS_IMPLEMENT)                            \
    ||  defined(__VSF_SIMPLE_STREAM_CLASS_INHERIT__)
extern void __vsf_stream_on_read(vsf_stream_t *stream);
extern void __vsf_stream_on_write(vsf_stream_t *stream);
#endif

#ifdef __cplusplus
}
#endif

#undef __VSF_SIMPLE_STREAM_CLASS_IMPLEMENT
#undef __VSF_SIMPLE_STREAM_CLASS_INHERIT__

/*============================ INCLUDES ======================================*/

#include "./fifo_stream/vsf_fifo_stream.h"
#include "./mem_stream/vsf_mem_stream.h"
#include "./block_stream/vsf_block_stream.h"
#include "./distbus_stream/vsf_distbus_stream.h"
#include "./queue_stream/vsf_queue_stream.h"

/** @} */   // vsf_simple_stream
/** @} */   // vsf_service

#endif      // VSF_USE_SIMPLE_STREAM
#endif      // __VSF_SIMPLE_STREAM_H__
