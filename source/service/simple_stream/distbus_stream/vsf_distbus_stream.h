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

#ifndef __VSF_DISTBUS_STREAM_H__
#define __VSF_DISTBUS_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED && VSF_USE_DISTBUS == ENABLED

#include "service/distbus/vsf_distbus.h"

#if     defined(__VSF_DISTBUS_STREAM_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_DISTBUS_STREAM_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define __VSF_DISTBUS_STREAM_INIT(__NAME, __DISTBUS, __MTU, __IS_TX, __BUF_SIZE, __HANDLER) \
            .op                 = &vsf_distbus_stream_op,                       \
            .buf_size           = (__BUF_SIZE),                                 \
            .is_tx              = (__IS_TX),                                    \
            .distbus            = (__DISTBUS),                                  \
            .mtu                = (__MTU),                                      \
            .addr_range         = VSF_DISTBUS_STREAM_ADDR_RANGE,                \
            .handler            = (__HANDLER),                                  \
            .info               = (const vsf_distbus_service_info_t *)          \
                                    &(__NAME).use_as__vsf_distbus_service_info_t,
#define VSF_DISTBUS_STREAM_INIT(__NAME, __DISTBUS, __MTU, __IS_TX, __BUF_SIZE, __HANDLER)\
            __VSF_DISTBUS_STREAM_INIT(__NAME, (__DISTBUS), (__MTU), (__IS_TX), (__BUF_SIZE), (__HANDLER))

#define __describe_distbus_stream(__name, __distbus, __mtu, __is_tx, __buf_size, __handler) \
            vsf_distbus_stream_t __name = {                                     \
                VSF_DISTBUS_STREAM_INIT(__name, (__distbus), (__mtu), (__is_tx), (__buf_size), (__handler))\
            };

#define describe_distbus_stream_tx(__name, __distbus, __mtu, __buf_size, __handler)\
            __describe_distbus_stream(__name, (__distbus), (__mtu), true, (__buf_size), (__handler))
#define describe_distbus_stream_rx(__name, __distbus, __mtu, __handler)         \
            __describe_distbus_stream(__name, (__distbus), (__mtu), false, 0, (__handler))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_DISTBUS_STREAM_CMD_BUF_SIZE,
    VSF_DISTBUS_STREAM_CMD_DATA,
    VSF_DISTBUS_STREAM_CMD_SIZE,
    VSF_DISTBUS_STREAM_ADDR_RANGE,
};

vsf_class(vsf_distbus_stream_t) {
    public_member(
        implement(vsf_stream_t)
        implement(vsf_distbus_service_info_t)
        implement(vsf_distbus_service_t)
        vsf_distbus_t *distbus;
        uint32_t buf_size;
        // is distbus_stream tx terminal
        bool is_tx;
    )
    private_member(
        uint32_t data_size;
        union {
            struct {
                vsf_distbus_msg_t *msg;
            };
            struct {
                vsf_slist_queue_t msgq;
            };
        };
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_stream_op_t vsf_distbus_stream_op;

/*============================ PROTOTYPES ====================================*/

extern void vsf_distbus_stream_register_service(vsf_distbus_stream_t *distbus_stream);

#if     defined(__VSF_DISTBUS_STREAM_CLASS_IMPLEMENT)                           \
    ||  defined(__VSF_DISTBUS_STREAM_CLASS_INHERIT__)
bool __vsf_distbus_stream_msghandler(vsf_distbus_t *bus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg);
#endif

#ifdef __cplusplus
}
#endif

#undef __VSF_DISTBUS_STREAM_CLASS_IMPLEMENT
#undef __VSF_DISTBUS_STREAM_CLASS_INHERIT__

#endif      // VSF_USE_SIMPLE_STREAM && VSF_USE_DISTBUS
#endif      // __VSF_DISTBUS_STREAM_H__
