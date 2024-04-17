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

// stream adapter

#define __VSF_STREAM_ADAPTER_INIT(__STREAM_TX, __STREAM_RX, __THRESHOLD_TX, __THRESHOLD_RX)\
            .stream_tx          = (vsf_stream_t *)(__STREAM_TX),                \
            .stream_rx          = (vsf_stream_t *)(__STREAM_RX),                \
            .threshold_tx       = (__THRESHOLD_TX),                             \
            .threshold_rx       = (__THRESHOLD_RX),
#define VSF_STREAM_ADAPTER_INIT0(__STREAM_TX, __STREAM_RX)                      \
            __VSF_STREAM_ADAPTER_INIT((__STREAM_TX), (__STREAM_RX), 0, 0)
#define VSF_STREAM_ADAPTER_INIT1(__STREAM_TX, __STREAM_RX, __THRESHOLD_TX)      \
            __VSF_STREAM_ADAPTER_INIT((__STREAM_TX), (__STREAM_RX), (__THRESHOLD_TX), 0)
#define VSF_STREAM_ADAPTER_INIT2(__STREAM_TX, __STREAM_RX, __THRESHOLD_TX, __THRESHOLD_RX)\
            __VSF_STREAM_ADAPTER_INIT((__STREAM_TX), (__STREAM_RX), (__THRESHOLD_TX), (__THRESHOLD_RX))
// prototype: VSF_STREAM_ADAPTER_INIT(__STREAM_TX, __STREAM_RX, __THRESHOLD_TX = 0, __THRESHOLD_RX = 0)
#define VSF_STREAM_ADAPTER_INIT(__STREAM_TX, __STREAM_RX, ...)                  \
            __PLOOC_EVAL(VSF_STREAM_ADAPTER_INIT, __VA_ARGS__)((__STREAM_TX), (__STREAM_RX), ##__VA_ARGS__)

#define __declare_stream_adapter(__name)                                        \
            vsf_dcl_class(vsf_stream_adapter_t)

#define declare_stream_adapter(__name)                                          \
            __declare_stream_adapter(__name)
#define dcl_stream_adapter(__name)                                              \
            declare_stream_adapter(__name)

#define __describe_stream_adapter(__name, __stream_tx, __stream_rx, __threshold_tx, __threshold_rx)\
            vsf_stream_adapter_t __name = {                                     \
                VSF_STREAM_ADAPTER_INIT((__stream_tx), (__stream_rx), (__threshold_tx), (__threshold_rx))\
            };

#define describe_stream_adapter0(__name, __stream_tx, __stream_rx)              \
            __describe_stream_adapter(__name, (__stream_tx), (__stream_rx), 0, 0)
#define describe_stream_adapter1(__name, __stream_tx, __stream_rx, __threshold_tx)\
            __describe_stream_adapter(__name, (__stream_tx), (__stream_rx), (__threshold_tx), 0)
#define describe_stream_adapter2(__name, __stream_tx, __stream_rx, __threshold_tx, __threshold_rx) \
            __describe_stream_adapter(__name, (__stream_tx), (__stream_rx), (__threshold_tx), __threshold_rx)
// prototype: describe_stream_adapter(__name, __stream_tx, __stream_rx, __threshold_tx = 0, __threshold_rx = 0)
#define describe_stream_adapter(__name, __stream_tx, __stream_rx, ...)          \
            __PLOOC_EVAL(describe_stream_adapter, __VA_ARGS__)(__name, (__stream_tx), (__stream_rx), ##__VA_ARGS__)

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
         \~english initialization function, must be called before other APIs.
         @param stream stream instance, cannot be NULL.
         @return None.

         \~chinese 初始化函数，必须在其他API之前调用。
         @param stream 流实例, 不能是空指针。
         @return 无返回值。
         */
        void (*init)(vsf_stream_t *stream);
        void (*fini)(vsf_stream_t *stream);

        /**
         \~english write function, must be called after init.
         @param stream stream instance, cannot be NULL.
         @param buf buffer pointer, point to a buffer or NULL
         @param size write size(bytes), if buffer is NULL, then increase stream size.

         \~chinese 初始化函数，必须在其他API之前调用。
         @param stream 流实例, 不能是空指针。
         @param buf 缓冲指针, 指向一块缓冲或者是NULL。
         @param size 写大小(字节单位), 如果缓冲区是NULL, 将只增大流中的数据缓冲大小
         */
        uint_fast32_t (*write)(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);

        /**
         \~english read function, must be called after init.
         @param stream stream instance, cannot be NULL.
         @param buf buffer pointer, point to a buffer or NULL
         @param size read size(bytes), if buffer is NULL, then decrease stream size

         \~chinese 初始化函数，必须在其他API之前调用。
         @param stream 流实例, 不能是空指针。
         @param buf 缓冲指针, 指向一块缓冲或者是NULL。
         @param size 读大小(字节单位), 如果缓冲区是NULL, 将只减小流中的数据缓冲大小
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

/**
 * \~english vsf steam terminal class, each stream has a tx terminal and a rx terminal
 * @note TODO
 *
 * \~chinese vsf 流终端类, 每个流都有收发 2 个终端
 * @note TODO
 */
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

/**
 * \~english vsf steam class
 * @note TODO
 *
 * \~chinese vsf 流类
 * @note TODO
 */
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

/**
 * \~english vsf steam adapter class, used to connect 2 streams
 * @note stream terminal connection: stream_tx.rx <==> stream_rx.tx
 *       data flow: stream_tx ==> adapter ==> stream_rx
 *          IMPORTANT: tx of stream_tx and rx of stream_rx can not be pre-empted by each other
 *
 * \~chinese vsf 流适配器类, 用于连接 2 个流
 * @note 流终端连接方式: stream_tx.rx <==> stream_rx.tx(stream_tx => stream_rx)
 *       数据流: stream_tx ==> adapter ==> stream_rx
 *          重要: stream_tx 的发送端和 stream_rx 的接收端不能互相抢占
 */
vsf_class(vsf_stream_adapter_t) {
    public_member(
        vsf_stream_t *stream_tx;
        vsf_stream_t *stream_rx;
        uint32_t threshold_tx;      // threshold of data_size of stream_tx
        uint32_t threshold_rx;      // threshold of free_size of stream_rx
        // user can read data from stream_tx and write to stream_rx in on_data callback,
        //  and return the actual size write to stream_rx
        uint_fast32_t (*on_data)(vsf_stream_adapter_t *adapter, uint_fast32_t tx_data_size, uint_fast32_t rx_free_size);
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 * \~english initialization function, must be called before other APIs.
 * @param stream stream instance, cannot be NULL.
 *
 * \~chinese 初始化函数，必须在其他API之前调用。
 * @param stream 流实例, 不能是空指针。
 */
extern vsf_err_t vsf_stream_init(vsf_stream_t *stream);
extern vsf_err_t vsf_stream_fini(vsf_stream_t *stream);
/**
 \~english write function, must be called after init.
 @param stream stream instance, cannot be NULL.
 @param buf buffer pointer, point to a buffer or NULL
 @param size write size(bytes), if buffer is NULL, then increase stream size.

 \~chinese 初始化函数，必须在其他API之前调用。
 @param stream 流实例, 不能是空指针。
 @param buf 缓冲指针, 指向一块缓冲或者是NULL。
 @param size 写大小(字节单位), 如果缓冲区是NULL, 将只增大流中的数据缓冲大小
*/
extern uint_fast32_t vsf_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
/**
 \~english read function, must be called after init.
 @param stream stream instance, cannot be NULL.
 @param buf buffer pointer, point to a buffer or NULL
 @param size read size(bytes), if buffer is NULL, then decrease stream size

 \~chinese 初始化函数，必须在其他API之前调用。
 @param stream 流实例, 不能是空指针。
 @param buf 缓冲指针, 指向一块缓冲或者是NULL。
 @param size 读大小(字节单位), 如果缓冲区是NULL, 将只减小流中的数据缓冲大小
*/
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

// vsf_stream_adapter_evthandler is only for classes derived from vsf_stream_adapter_t
//  IMPORTANT: vsf_stream_adapter_evthandler for stream_tx and stream_rx can not be pre-empted by each other
extern uint_fast32_t vsf_stream_adapter_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt);
extern void vsf_stream_adapter_init(vsf_stream_adapter_t *adapter);

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
