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

#ifndef __VSF_HTTP_CLIENT_H__
#define __VSF_HTTP_CLIENT_H__

/*============================ INCLUDES ======================================*/

#include "component/vsf_component_cfg.h"

#if VSF_USE_MBEDTLS == ENABLED
// DO NOT include mbedtls_tls_session.h here, because it will undefine _WIN32 for VSF,
//  but some windows based environment will need this
//#   include "component/3rd-party/mbedtls/extension/tls_session/mbedtls_tls_session.h"
#endif

#if     defined(__VSF_HTTP_CLIENT_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HTTP_CLIENT_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_HTTP_CLIENT_CFG_BUFFER_SIZE
#   define VSF_HTTP_CLIENT_CFG_BUFFER_SIZE      4096
#endif

#ifndef VSF_HTTP_CLIENT_CFG_USER_AGENT
// for some website, not all user agent are accepted, so use fake curl here
#   define VSF_HTTP_CLIENT_CFG_USER_AGENT       "curl/8.5.0"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_declare_class(vsf_http_client_t)

vsf_class(vsf_http_op_t) {
    protected_member(
        int (*fn_connect)(void *param, const char *host, const char *port);
        void (*fn_close)(void *param);
        int (*fn_write)(void *param, uint8_t *buf, uint16_t len);
        int (*fn_read)(void *param, uint8_t *buf, uint16_t len);
    )
};

typedef struct vsf_http_client_req_t vsf_http_client_req_t;
struct vsf_http_client_req_t {
    const char *host;
    const char *port;
    const char *verb;
    const char *connect_mode;

    char *header;
    char *path;

    uint8_t *txdata;
    size_t txdata_len;

    // IMPORTANT: line is NULL terminated string ending with \r
    void (*on_response_header)(vsf_http_client_t *http, vsf_http_client_req_t *req, char *line);
};

vsf_class(vsf_http_client_t) {
    public_member(
        const vsf_http_op_t *op;
        void *param;
        int resp_status;
        int content_length;
        char *redirect_path;
    )
    private_member(
        uint16_t cur_size;
        bool is_chunked;
        int cur_chunk_size;
        uint8_t *cur_buffer;
        uint8_t buffer[VSF_HTTP_CLIENT_CFG_BUFFER_SIZE];
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_MBEDTLS == ENABLED
// to use mbedtls_http_op, user source code(.c) MUST include
//  "component/3rd-party/mbedtls/extension/tls_session/mbedtls_tls_session.h",
//  and set mbedtls_session_t instance to param of vsf_http_client_t.
extern const vsf_http_op_t vsf_mbedtls_http_op;
#endif

/*============================ PROTOTYPES ====================================*/

extern void vsf_http_client_close(vsf_http_client_t *http);
extern void vsf_http_client_init(vsf_http_client_t *http);
extern int vsf_http_client_request(vsf_http_client_t *http, vsf_http_client_req_t *req);
extern int vsf_http_client_read(vsf_http_client_t *http, uint8_t *buf, uint16_t len);
extern int vsf_http_client_write(vsf_http_client_t *http, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_HTTP_CLIENT_H__
