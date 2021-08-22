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

#ifndef __VSF_LINUX_HTTPD_H__
#define __VSF_LINUX_HTTPD_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/sys/socket.h"
#   include "../../include/netinet/in.h"
#else
#   include <sys/socket.h>
#   include <netinet/in.h>
#endif

#if     defined(__VSF_LINUX_HTTPD_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   define PUBLIC_CONST
#elif   defined(__VSF_LINUX_HTTPD_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   define PUBLIC_CONST
#else
#   define PUBLIC_CONST                                 const
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_LINUX_HTTPD_CFG_SESSION_BUFSIZE
#   define VSF_LINUX_HTTPD_CFG_SESSION_BUFSIZE          1024
#endif
#ifndef VSF_LINUX_HTTPD_CFG_PRIV_SIZE
#   define VSF_LINUX_HTTPD_CFG_PRIV_SIZE                256
#endif

#ifndef VSF_LINUX_HTTPD_CFG_FILESYSTEM
#   define VSF_LINUX_HTTPD_CFG_FILESYSTEM               ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_linux_httpd_request_method_t {
    VSF_LINUX_HTTPD_GET,
    VSF_LINUX_HTTPD_POST,
} vsf_linux_httpd_request_method_t;

typedef enum vsf_linux_httpd_content_type_t {
    VSF_LINUX_HTTPD_TEXT_XML,
} vsf_linux_httpd_content_type_t;

typedef enum vsf_linux_httpd_language_t {
    VSF_LINUX_HTTPD_ENGLISH,
} vsf_linux_httpd_language_t;

typedef enum vsf_linux_https_request_result_t {
    VSF_LINUX_HTTPD_OK = 200,

    VSF_LINUX_HTTPD_MULTIPLE_CHOICES = 300,

    VSF_LINUX_HTTPD_BAD_REQUEST = 400,
    VSF_LINUX_HTTPD_UNAUTHORIZED,
    VSF_LINUX_HTTPD_PAYMENT_REQUIRED,
    VSF_LINUX_HTTPD_FORBIDDEN,
    VSF_LINUX_HTTPD_NOT_FOUND,
    VSF_LINUX_HTTPD_METHOD_NOT_ALLOWED,
    VSF_LINUX_HTTPD_NOT_ACCEPTABLE,
    VSF_LINUX_HTTPD_PROXY_AUTHENTICATION_REQUIRED,
    VSF_LINUX_HTTPD_REQUEST_TIME_OUT,
    VSF_LINUX_HTTPD_CONFLICT,
    VSF_LINUX_HTTPD_GONE,
    VSF_LINUX_HTTPD_LENGTH_REQUIRED,
    VSF_LINUX_HTTPD_PRECONDITION_FAILED,
    VSF_LINUX_HTTPD_REQUEST_ENITIY_TOO_LARGE,
    VSF_LINUX_HTTPD_REQUEST_URI_TOO_LARGE,
    VSF_LINUX_HTTPD_UNSUPPORTED_MEDIA_TYPE,
    VSF_LINUX_HTTPD_REQUESTED_RANGE_NOT_SATISFIABLE,
    VSF_LINUX_HTTPD_EXPECTATION_FAILED,

    VSF_LINUX_HTTPD_INTERNAL_SERVER_ERROR = 500,
    VSF_LINUX_HTTPD_NOT_IMPLEMENT,
    VSF_LINUX_HTTPD_BAD_GATEWAY,
    VSF_LINUX_HTTPD_SERVICE_UNAVAILABLE,
    VSF_LINUX_HTTPD_GATEWAY_TIME_OUT,
    VSF_LINUX_HTTPD_HTTP_VERSION_NOT_SUPPORTED,
} vsf_linux_https_request_result_t;

vsf_dcl_class(vsf_linux_httpd_request_t)
typedef struct vsf_linux_httpd_urihandler_op_t {
    vsf_err_t (*init_fn)(vsf_linux_httpd_request_t *req, vsf_stream_t *in, vsf_stream_t *out);
    vsf_err_t (*fini_fn)(vsf_linux_httpd_request_t *req);
    vsf_err_t (*serve_fn)(vsf_linux_httpd_request_t *req);
} vsf_linux_httpd_urihandler_op_t;

/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
}
#endif

#if VSF_LINUX_HTTPD_CFG_FILESYSTEM == ENABLED
#   include "./urihandler/file/__vsf_linux_urihandler_file.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_linux_httpd_request_t) {
    protected_member(
        vsf_linux_httpd_request_method_t method;
        vsf_linux_httpd_content_type_t content_type;
        vsf_linux_httpd_language_t language;
        uint32_t content_length;
        char *uri;
        char *query;

        vsf_stream_t *stream_in, *stream_out;
        uint16_t result;

        union {
#if VSF_LINUX_HTTPD_CFG_FILESYSTEM == ENABLED
            vsf_linux_httpd_urihandler_file_t file;
#endif
            uint8_t priv[VSF_LINUX_HTTPD_CFG_PRIV_SIZE];
        } urihandler_ctx;
    )
};

typedef enum vsf_linux_httpd_urihandler_match_t {
    VSF_LINUX_HTTPD_URI_MATCH_EXT = 1 << 0,
    VSF_LINUX_HTTPD_URI_MATCH_URI = 1 << 1,
    VSF_LINUX_HTTPD_URI_MATCH_ANY = 1 << 2,
} vsf_linux_httpd_urihandler_match_t;
typedef enum vsf_linux_httpd_urihandler_type_t {
    VSF_LINUX_HTTPD_URI_OP = 0,
    VSF_LINUX_HTTPD_URI_REMAP,
} vsf_linux_httpd_urihandler_type_t;

typedef struct vsf_linux_httpd_urihandler_t {
    vsf_linux_httpd_urihandler_match_t match;
    vsf_linux_httpd_urihandler_type_t type;

    char *ext;
    char *uri;

    union {
        const vsf_linux_httpd_urihandler_op_t *op;
        char *target_uri;
    };
} vsf_linux_httpd_urihandler_t;

vsf_class(vsf_linux_httpd_session_t) {
    private_member(
        vsf_dlist_node_t session_node;
        vsf_linux_httpd_request_t request;

        int fd_socket;
        int fd_stream_out;
        struct sockaddr_in client_addr;

        uint8_t buffer[VSF_LINUX_HTTPD_CFG_SESSION_BUFSIZE];
        uint16_t buffer_pos;
        uint16_t buffer_length;
    )
};

#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
dcl_vsf_pool(vsf_linux_httpd_session_pool)
def_vsf_pool(vsf_linux_httpd_session_pool, vsf_linux_httpd_session_t)
#endif

vsf_class(vsf_linux_httpd_t) {
    public_member(
        PUBLIC_CONST char *root_path;
        PUBLIC_CONST uint16_t port;
        PUBLIC_CONST uint8_t backlog;

        PUBLIC_CONST uint16_t num_of_urihandler;
        PUBLIC_CONST vsf_linux_httpd_urihandler_t *urihandler;
    )
    private_member(
        vsf_dlist_t session_list;
#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
        vsf_pool(vsf_linux_httpd_session_pool) session_pool;
        vsf_pool_item(vsf_linux_httpd_session_pool) sessions[VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE];
#endif
    )
};

#undef PUBLIC_CONST

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_linux_httpd_start(vsf_linux_httpd_t *httpd);

#ifdef __cplusplus
}
#endif

#undef __VSF_LINUX_HTTPD_CLASS_IMPLEMENT
#undef __VSF_LINUX_HTTPD_CLASS_INHERIT__

#endif
/* EOF */
