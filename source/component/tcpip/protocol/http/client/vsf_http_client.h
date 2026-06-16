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
// for vsf_err_t return type of vsf_http_client_init/fini
#include "utilities/vsf_utilities.h"

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
        // len is total bytes to transfer in this call. Implementations MAY
        // short-write/short-read (return value < len); callers use
        // vsf_http_client_write/read to loop. Return <= 0 indicates error.
        // Note on signedness: return type is int while len is size_t. In
        // practice the HTTP request/response buffers used by this library
        // are far below INT_MAX, so the narrowing is always safe; this
        // mirrors the signature convention of mbedtls_ssl_read/write.
        int (*fn_write)(void *param, uint8_t *buf, size_t len);
        int (*fn_read)(void *param, uint8_t *buf, size_t len);

        // Optional. Update the transport's read timeout (ms); semantics of
        // timeout_ms <= 0 is "no timeout / blocking". Both TCP and TLS ports
        // should accept calls before and after a connection is established:
        // upper layers (e.g. esp_http_client_set_timeout_ms) may set the
        // value either way round. Return 0 on success, < 0 on failure.
        // When NULL, vsf_http_client_set_timeout is a no-op (feature absent).
        int (*fn_set_timeout)(void *param, int timeout_ms);

        // Optional. Return the underlying socket fd for integration with
        // select()/poll(). ONLY meaningful when the port runs on top of the
        // VSF linux socket layer (VSF_USE_LINUX == ENABLED); ports on other
        // net backends (bare winsock, raw lwip, custom bio) should leave
        // this field NULL -- callers must then treat "no fd available"
        // (vsf_http_client_get_fd returns -1).
        int (*fn_get_fd)(void *param);
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

        // Working buffer for header assembly / response parsing.
        // Caller may provide their own (static / pre-allocated) buffer by
        // setting both fields before vsf_http_client_init(); otherwise
        // init() allocates VSF_HTTP_CLIENT_CFG_BUFFER_SIZE bytes from the
        // VSF heap and vsf_http_client_fini() releases it.
        //   buffer       == NULL && buffer_size == 0  -> init allocates (CFG_BUFFER_SIZE)
        //   buffer       == NULL && buffer_size  > 0  -> init allocates buffer_size bytes
        //   buffer       != NULL && buffer_size  > 0  -> caller-owned, never freed by fini
        uint8_t *buffer;
        size_t buffer_size;
    )
    private_member(
        size_t cur_size;
        bool is_chunked;
        int cur_chunk_size;
        uint8_t *cur_buffer;
        // true when init() allocated ->buffer from the heap; fini() will free it.
        bool _buffer_owned;
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
// Initialise an http client. Must be called on a zero-initialised handle.
// See the buffer / buffer_size fields in vsf_http_client_t for buffer ownership.
// Returns VSF_ERR_NONE on success, or a negative vsf_err_t on failure
// (currently only VSF_ERR_NOT_ENOUGH_RESOURCES when the internal buffer malloc fails).
extern vsf_err_t vsf_http_client_init(vsf_http_client_t *http);
// Release resources owned by init() (i.e. the heap-allocated buffer if any).
// Does NOT close the transport -- call vsf_http_client_close() first if the
// connection is still open. Safe to call on an init()-failed or already-fini'd handle.
extern void vsf_http_client_fini(vsf_http_client_t *http);
extern int vsf_http_client_request(vsf_http_client_t *http, vsf_http_client_req_t *req);
extern int vsf_http_client_request_with_redirect(vsf_http_client_t *http,
                                                  vsf_http_client_req_t *req,
                                                  int max_redirect);
extern int vsf_http_client_read(vsf_http_client_t *http, uint8_t *buf, size_t len);
extern int vsf_http_client_write(vsf_http_client_t *http, uint8_t *buf, size_t len);

// Update the transport's read timeout (ms). Delegates to op->fn_set_timeout
// when provided; returns VSF_ERR_NOT_SUPPORT if the port lacks this op.
extern vsf_err_t vsf_http_client_set_timeout(vsf_http_client_t *http, int timeout_ms);
// Return underlying socket fd for select()/poll(), or -1 when the port
// does not expose one (see vsf_http_op_t::fn_get_fd for backend requirements).
extern int vsf_http_client_get_fd(vsf_http_client_t *http);

// ---- Stream-style sub-APIs ---------------------------------------------
// vsf_http_client_request() is a composition of the APIs below. They are
// exposed so that upper layers (e.g. esp_http_client compat shim) can drive
// the transfer in discrete stages: connect -> send_header -> write(body) ->
// fetch_headers -> read(body) -> close.

// Establish underlying connection (TCP or TLS), thin wrapper on op->fn_connect.
// Returns 0 on success, non-zero on failure (transport error code is passed through).
extern int vsf_http_client_connect(vsf_http_client_t *http,
                                   const char *host, const char *port);

// Send request line + headers only (no body). Uses req->txdata_len as
// Content-Length when > 0. Caller sends body afterwards via vsf_http_client_write.
// req->header / req->connect_mode use defaults when NULL (same as request()).
// Returns bytes written (> 0) on success, < 0 on failure.
extern int vsf_http_client_send_header(vsf_http_client_t *http,
                                       vsf_http_client_req_t *req);

// Read & parse response headers, populate resp_status / content_length /
// is_chunked / redirect_path; any body bytes already consumed are retained
// in the internal buffer for subsequent vsf_http_client_read().
// Returns 0 on success, <= 0 from underlying fn_read on failure, -1 on overflow.
extern int vsf_http_client_fetch_headers(vsf_http_client_t *http,
                                         vsf_http_client_req_t *req);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_HTTP_CLIENT_H__
