/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_DISTBUS_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_DISTBUS_SERVICE_TYPE_ROLE       0x8000
#define VSF_DISTBUS_SERVICE_TYPE_MASTER     VSF_DISTBUS_SERVICE_TYPE_ROLE
#define VSF_DISTBUS_SERVICE_TYPE_SLAVE      0x0000

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vsf_distbus_t)
dcl_simple_class(vsf_distbus_service_t)
dcl_simple_class(vsf_distbus_trans_t)

typedef bool (*vsf_distbus_transhandler_t)( vsf_distbus_t *bus,
                                            vsf_distbus_service_t *service,
                                            vsf_distbus_trans_t *trans);
typedef struct vsf_distbus_transheader_t {
    uint32_t                                datalen;
    uint32_t                                hash_header;
    uint32_t                                hash_data;
    uint16_t                                addr;
    uint16_t                                flag;
} vsf_distbus_transheader_t;
def_simple_class(vsf_distbus_trans_t) {
    private_member(
        union {
            uint32_t                        pos;
            vsf_slist_node_t                node;
        };
    )
    public_member(
        uint32_t                            buffer_size;
        vsf_distbus_transheader_t           header[0];
    )
};

typedef struct vsf_distbus_service_info_t {
    uint32_t                                mtu;
    uint16_t                                type;
    uint8_t                                 addr_range;
    uint8_t                                 flag;
    vsf_distbus_transhandler_t              handler;
} vsf_distbus_service_info_t;

def_simple_class(vsf_distbus_service_t) {
    private_member(
        vsf_slist_node_t                    service_node;
    )
    public_member(
        uint16_t                            addr_start;
        uint16_t                            addr_end;
        const vsf_distbus_service_info_t    *info;
    )
};

typedef struct vsf_distbuf_drv_t {
    uint_fast32_t (*send)(uint8_t *buffer, uint_fast32_t size);
    uint_fast32_t (*recv)(uint8_t *buffer, uint_fast32_t size, void (*)(uint8_t *buffer, uint_fast32_t size));
} vsf_distbuf_drv_t;

def_simple_class(vsf_distbus_t) {
    private_member(
        const vsf_distbuf_drv_t             *drv;
        vsf_distbus_trans_t                 *trans_rx;
        vsf_distbus_trans_t                 *trans_tx;
        vsf_slist_t                         service_list;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __VSF_DISTBUS_CLASS_INHERIT__
extern vsf_distbus_trans_t * vsf_distbus_alloc_trans(vsf_distbus_t *distbus, uint_fast32_t size);
extern void vsf_distbus_free_trans(vsf_distbus_t *distbus, vsf_distbus_trans_t *trans);
// after transact is sent, it will be freed automatically
extern vsf_err_t vsf_distbus_send_trans(vsf_distbus_t *distbus, vsf_distbus_trans_t *trans);
#endif

extern vsf_err_t vsf_distbuf_init(vsf_distbus_t *distbus, const vsf_distbuf_drv_t *drv);

#ifdef __cplusplus
}
#endif

#undef __VSF_DISTBUS_CLASS_IMPLEMENT
#undef __VSF_DISTBUS_CLASS_INHERIT__

#endif      // VSF_USE_DISTBUS
#endif      // __VSF_DISTBUS_H__
