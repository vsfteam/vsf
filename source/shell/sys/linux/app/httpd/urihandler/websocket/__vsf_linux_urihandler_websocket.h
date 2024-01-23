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

#ifndef __VSF_LINUX_HTTPD_WEBSOCKET_H__
#define __VSF_LINUX_HTTPD_WEBSOCKET_H__

/*============================ INCLUDES ======================================*/

#include "kernel/vsf_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef int (*vsf_linux_httpd_websocket_onopen_t)(vsf_linux_httpd_request_t *req);
typedef void (*vsf_linux_httpd_websocket_onclose_t)(vsf_linux_httpd_request_t *req);
typedef void (*vsf_linux_httpd_websocket_onerror_t)(vsf_linux_httpd_request_t *req);
typedef void (*vsf_linux_httpd_websocket_onmessage_t)(vsf_linux_httpd_request_t *req, uint8_t *buf, uint32_t len);

typedef struct vsf_linux_httpd_urihandler_websocket_t {
    vsf_fifo_stream_t stream_in;
    vsf_fifo_stream_t stream_out;
    bool connected;
} vsf_linux_httpd_urihandler_websocket_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_linux_httpd_urihandler_op_t vsf_linux_httpd_urihandler_websocket_op;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif
#endif
/* EOF */
