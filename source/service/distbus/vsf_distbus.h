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

#ifndef __VSF_DISTBUS_H__
#define __VSF_DISTBUS_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"

#if VSF_USE_DISTBUS == ENABLED

#include "utilities/vsf_utilities.h"

#if     defined(__VSF_DISTBUS_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_DISTBUS_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_DISTBUS_CFG_DEBUG == ENABLED
#   define __vsf_distbus_trace(...)                     vsf_trace_debug(__VA_ARGS__)
#   define __vsf_distbus_trace_buffer(__ptr, __size)    vsf_trace_buffer(VSF_TRACE_DEBUG, (__ptr), (__size))
#else
#   define __vsf_distbus_trace(...)
#   define __vsf_distbus_trace_buffer(__ptr, __size)
#endif

/*============================ TYPES =========================================*/

vsf_dcl_class(vsf_distbus_t)
vsf_dcl_class(vsf_distbus_service_t)
vsf_dcl_class(vsf_distbus_msg_t)

// returns true to hold msg
typedef bool (*vsf_distbus_msghandler_t)(   vsf_distbus_t *bus,
                                            vsf_distbus_service_t *service,
                                            vsf_distbus_msg_t *msg);
typedef struct vsf_distbus_msgheader_t {
    uint32_t                                datalen;
    uint32_t                                hash_header;
    uint32_t                                hash_data;
    uint16_t                                addr;
    uint16_t                                flag;
} vsf_distbus_msgheader_t;
vsf_class(vsf_distbus_msg_t) {
    protected_member(
        uint32_t                            pos;
        vsf_slist_node_t                    node;
    )
    public_member(
        vsf_distbus_msgheader_t             header;
    )
};

typedef struct vsf_distbus_service_info_t {
    uint32_t                                mtu;
    uint16_t                                type;
    uint8_t                                 addr_range;
    uint8_t                                 flag;
    vsf_distbus_msghandler_t                handler;
} vsf_distbus_service_info_t;

vsf_class(vsf_distbus_service_t) {
    private_member(
        vsf_slist_node_t                    node;
    )
    public_member(
        uint16_t                            addr_start;
        const vsf_distbus_service_info_t    *info;
    )
};

typedef struct vsf_distbus_bus_op_t {
    bool (*init)(void *p, void (*on_inited)(void *p));
    // returns true if sent
    bool (*send)(uint8_t *buffer, uint_fast32_t size, void *p, void (*on_sent)(void *p));
    // returns true if received
    bool (*recv)(uint8_t *buffer, uint_fast32_t size, void *p, void (*on_recv)(void *p));
} vsf_distbus_bus_op_t;

typedef struct vsf_distbus_mem_op_t {
    // dynamic allocation of message SHOULD be implemented as pool, becasue maybe
    //  it will be called in interrupt
    void * (*alloc_msg)(uint_fast32_t size);
    void (*free_msg)(void *msg);
} vsf_distbus_mem_op_t;

typedef void (*vsf_distbus_on_error_t)(vsf_distbus_t *distbus);
typedef struct vsf_distbus_op_t {
    vsf_distbus_bus_op_t                    bus;
    vsf_distbus_mem_op_t                    mem;
    vsf_distbus_on_error_t                  on_error;
} vsf_distbus_op_t;

vsf_class(vsf_distbus_t) {
    public_member(
        const vsf_distbus_op_t              op;
        void (*on_connected)(vsf_distbus_t *distbus);
    )
    private_member(
        uint32_t                            mtu;
        vsf_distbus_msg_t                   *msg_rx;
        vsf_distbus_msg_t                   *msg_tx;
        vsf_slist_t                         msg_tx_list;
        vsf_slist_t                         service_list;
        uint16_t                            cur_addr;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __VSF_DISTBUS_CLASS_INHERIT__
// size is data size of message(excluding header)
extern vsf_distbus_msg_t * vsf_distbus_alloc_msg(vsf_distbus_t *distbus, uint_fast32_t size, uint8_t **buf);
// if user hold msg by returning trun in vsf_distbus_msghandler_t, user MUST free msg manually
extern void vsf_distbus_free_msg(vsf_distbus_t *distbus, vsf_distbus_msg_t *msg);
// after message is sent, it will be freed automatically
extern void vsf_distbus_send_msg(vsf_distbus_t *distbus, vsf_distbus_service_t *service, vsf_distbus_msg_t *msg);
#endif

extern vsf_err_t vsf_distbus_init(vsf_distbus_t *distbus);
extern void vsf_distbus_register_service(vsf_distbus_t *distbus, vsf_distbus_service_t *service);

#ifdef __cplusplus
}
#endif

#undef __VSF_DISTBUS_CLASS_IMPLEMENT
#undef __VSF_DISTBUS_CLASS_INHERIT__

#endif      // VSF_USE_DISTBUS
#endif      // __VSF_DISTBUS_H__
