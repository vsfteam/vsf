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

/*============================ INCLUDES ======================================*/

#include "component/vsf_component_cfg.h"

#if VSF_USE_TCPIP == ENABLED

// for vsf_min
#include "utilities/vsf_utilities.h"
// for vsf_heap_malloc / vsf_heap_free used by default buffer allocation
#include "service/vsf_service.h"

#define __VSF_HTTP_CLIENT_CLASS_IMPLEMENT
#include "./vsf_http_client.h"

#if VSF_USE_MBEDTLS == ENABLED
#   include "component/3rd-party/mbedtls/extension/tls_session/mbedtls_tls_session.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

//#define vsf_http_trace(...)                 printf(__VA_ARGS__)
#ifndef vsf_http_trace
#   define vsf_http_trace(...)
#endif

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_MBEDTLS == ENABLED
// Cast function pointers from mbedtls_session_{read,write,...} (int return,
// mbedtls_session_t* param) to vsf_http_op_t's (void* param). Layout-compatible
// on every ABI VSF targets; matches the convention used elsewhere in VSF.
const vsf_http_op_t vsf_mbedtls_http_op = {
    .fn_connect     = (int (*)(void *, const char *, const char *))mbedtls_session_connect,
    .fn_close       = (void (*)(void *))mbedtls_session_close,
    .fn_write       = (int (*)(void *, uint8_t *, size_t))mbedtls_session_write,
    .fn_read        = (int (*)(void *, uint8_t *, size_t))mbedtls_session_read,
    .fn_set_timeout = (int (*)(void *, int))mbedtls_session_set_timeout,
#if VSF_USE_LINUX == ENABLED
    // fd returned by mbedtls net context is only select()/poll()-compatible
    // when mbedtls runs on top of the VSF linux socket layer. On bare winsock
    // / raw lwip / custom bio backends, leaving fn_get_fd NULL tells
    // vsf_http_client_get_fd to return -1 ("no fd available").
    .fn_get_fd      = (int (*)(void *))mbedtls_session_get_fd,
#endif
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(strnchr)
char * strnchr(const char *s, size_t n, int c)
{
    do {
        if (*s == c) {
            return (char*)s;
        }
    } while (--n && *s++);
    return NULL;
}

vsf_err_t vsf_http_client_init(vsf_http_client_t *http)
{
    http->cur_size       = 0;
    http->cur_chunk_size = 0;
    http->cur_buffer     = NULL;
    http->is_chunked     = false;
    // NOTE: do NOT reset _buffer_owned here. init() is idempotent on an
    // already-initialised handle (e.g. reused across several transfers);
    // clearing the flag would leak a heap-allocated buffer on the 2nd call.
    // _buffer_owned is written only when we actually allocate below.
    //
    // The caller MUST zero-initialise vsf_http_client_t before the FIRST
    // init(); otherwise buffer/buffer_size/_buffer_owned hold indeterminate
    // values and any branch taken below is undefined behaviour.

    // Buffer ownership rules (see vsf_http_client_t in header):
    //   user-provided buffer -> keep as-is (buffer != NULL)
    //   NULL + size == 0     -> allocate CFG_BUFFER_SIZE bytes
    //   NULL + size  > 0     -> allocate requested size
    if (NULL == http->buffer) {
        size_t sz = http->buffer_size > 0 ? http->buffer_size
                                          : (size_t)VSF_HTTP_CLIENT_CFG_BUFFER_SIZE;
        http->buffer = (uint8_t *)vsf_heap_malloc(sz);
        if (NULL == http->buffer) {
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }
        http->buffer_size   = sz;
        http->_buffer_owned = true;
    } else if (0 == http->buffer_size) {
        // defensive: caller forgot to set size; refuse rather than silently overflow
        return VSF_ERR_INVALID_PARAMETER;
    }
    return VSF_ERR_NONE;
}

void vsf_http_client_fini(vsf_http_client_t *http)
{
    if (http->_buffer_owned && http->buffer != NULL) {
        vsf_heap_free(http->buffer);
        http->buffer       = NULL;
        http->buffer_size  = 0;
        http->_buffer_owned = false;
    }
}

void vsf_http_client_close(vsf_http_client_t *http)
{
    http->op->fn_close(http->param);
}

int vsf_http_client_connect(vsf_http_client_t *http, const char *host, const char *port)
{
    return http->op->fn_connect(http->param, host, port);
}

vsf_err_t vsf_http_client_set_timeout(vsf_http_client_t *http, int timeout_ms)
{
    if (NULL == http->op->fn_set_timeout) {
        return VSF_ERR_NOT_SUPPORT;
    }
    return http->op->fn_set_timeout(http->param, timeout_ms) == 0
           ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

int vsf_http_client_get_fd(vsf_http_client_t *http)
{
    if (NULL == http->op->fn_get_fd) {
        return -1;
    }
    return http->op->fn_get_fd(http->param);
}

int vsf_http_client_send_header(vsf_http_client_t *http, vsf_http_client_req_t *req)
{
    int header_len, written;

    if (NULL == req->header) {
        req->header = (char *)"Accept: */*\r\n";
    }
    if (NULL == req->connect_mode) {
        req->connect_mode = "close";
    }

    if ((req->txdata_len > 0) && (req->txdata != NULL)) {
        header_len = sprintf((char *)http->buffer, "\
%s %s HTTP/1.1\r\n\
Host: %s\r\n\
User-Agent: %s\r\n\
Connection: %s\r\n\
Content-Length: %d\r\n\
%s\
\r\n", NULL == req->verb ? "POST" : req->verb, req->path, req->host, VSF_HTTP_CLIENT_CFG_USER_AGENT, req->connect_mode, (int)req->txdata_len, req->header);
    } else {
        header_len = sprintf((char *)http->buffer, "\
%s %s HTTP/1.1\r\n\
Host: %s\r\n\
User-Agent: %s\r\n\
Connection: %s\r\n\
%s\
\r\n", NULL == req->verb ? "GET" : req->verb, req->path, req->host, VSF_HTTP_CLIENT_CFG_USER_AGENT, req->connect_mode, req->header);
    }
    vsf_http_trace("http request:\n%s", http->buffer);

    // Fix: loop-write via vsf_http_client_write to handle transport-layer short-write.
    written = vsf_http_client_write(http, http->buffer, (size_t)header_len);
    if (written < header_len) {
        return written < 0 ? written : -1;
    }
    return written;
}

int vsf_http_client_fetch_headers(vsf_http_client_t *http, vsf_http_client_req_t *req)
{
    int result;

    // Fix: reset state once before the read_more loop. The original code
    // reset content_length/is_chunked on every re-read, which lost already
    // parsed header fields when response headers span multiple fn_read()
    // calls (e.g. long Set-Cookie, TCP fragmentation).
    http->redirect_path  = NULL;
    http->content_length = 0;
    http->is_chunked     = false;

    vsf_http_trace("http response:\n");
read_more:
    if (http->cur_size >= http->buffer_size) {
        vsf_http_client_close(http);
        return -1;
    }
    result = http->op->fn_read(http->param, http->buffer + http->cur_size, http->buffer_size - http->cur_size);
    if (result <= 0) {
        return result;
    }
    http->cur_size = (size_t)result + http->cur_size;
    http->cur_buffer = http->buffer;

    char *tmp, *line;
    while (http->cur_size > 0) {
        tmp = strnchr((const char *)http->cur_buffer, http->cur_size, '\n');
        if (NULL == tmp) {
            memcpy(http->buffer, http->cur_buffer, http->cur_size);
            goto read_more;
        }
        line = (char *)http->cur_buffer;
        *tmp++ = '\0';
        vsf_http_trace("%s\n", line);
        http->cur_size -= tmp - line;
        http->cur_buffer = (uint8_t *)tmp;

        if (strstr(line, "HTTP/1.") != NULL) {
            line += sizeof("HTTP/1.x ") - 1;
            http->resp_status = atoi(line);
            continue;
        }
        if (strstr(line, "Content-Length:")) {
            line += sizeof("Content-Length:") - 1;
            while (*line && isspace(*line)) { line++; }

            http->content_length = atoi(line);
            continue;
        }
        if (strstr(line, "Transfer-Encoding:")) {
            line += sizeof("Transfer-Encoding:") - 1;
            while (*line && isspace(*line)) { line++; }

            if (strstr(line, "chunked")) {
                http->is_chunked = true;
                http->cur_chunk_size = 0;
            }
            continue;
        }
        if (    ((301 == http->resp_status) || (302 == http->resp_status))
            &&  strstr(line, "Location:")) {
            line += sizeof("Location:") - 1;
            while (*line && isspace(*line)) { line++; }

            // 1. remove last \r
            if (line[strlen(line) - 1] == '\r') {
                line[strlen(line) - 1] = '\0';
            }
            // 2. remove possible '/'
            // TODO: confirm with VSF team whether stripping a trailing '/'
            //       on Location is intentional; RFC 3986 treats "/" and ""
            //       as distinct paths. Kept as-is to preserve legacy behavior.
            if (line[strlen(line) - 1] == '/') {
                line[strlen(line) - 1] = '\0';
            }
            http->redirect_path = line;
            continue;
        }
        if (req->on_response_header != NULL) {
            req->on_response_header(http, req, line);
        }
        if (*line == '\0' || *line == '\r') {
            break;
        }
    }
    return 0;
}

// Backward-compatible wrapper: combines connect/send_header/(body)/fetch_headers.
// Behavior matches the original monolithic implementation except for the
// two fixes noted above (short-write on body, state reset in fetch_headers).
int vsf_http_client_request(vsf_http_client_t *http, vsf_http_client_req_t *req)
{
    int result;

    result = vsf_http_client_connect(http, req->host, req->port);
    if (result != 0) {
        return result;
    }

    result = vsf_http_client_send_header(http, req);
    if (result < 0) {
        return result;
    }

    if ((req->txdata_len > 0) && (req->txdata != NULL)) {
        // Fix: use loop-write wrapper; original code called op->fn_write once
        // and ignored short-writes, silently truncating bodies larger than
        // the transport's single-call capacity (e.g. TLS record size).
        int written = vsf_http_client_write(http, req->txdata, req->txdata_len);
        if ((size_t)written < req->txdata_len) {
            return written < 0 ? written : -1;
        }
    }

    return vsf_http_client_fetch_headers(http, req);
}

// Parse a redirect URL into heap-allocated host/port/path strings.
// Supports:
//   - absolute URL: scheme://[user:pass@]host[:port]/path[?query][#fragment]
//   - path-absolute relative URL: /path[?query][#fragment]
// Returns 0 on success, -1 on failure.
// On success, caller must free *host_out, *port_out, *path_out with vsf_heap_free.
// For relative URLs, *host_out and *port_out are NULL, only *path_out is set.
static int __vsf_http_client_parse_redirect_url(const char *url,
                                                char **host_out,
                                                char **port_out,
                                                char **path_out)
{
    const char *p = strstr(url, "://");
    if (p != NULL) {
        const char *scheme = url;
        size_t scheme_len = p - url;
        p += sizeof("://") - 1;

        // skip user:pass@
        const char *at = strchr(p, '@');
        if (at != NULL) {
            p = at + 1;
        }

        // host
        const char *host_end = p;
        while (    *host_end
               &&  *host_end != ':'
               &&  *host_end != '/'
               &&  *host_end != '?'
               &&  *host_end != '#') {
            host_end++;
        }
        size_t host_len = host_end - p;
        char *host = (char *)vsf_heap_malloc(host_len + 1);
        if (NULL == host) {
            return -1;
        }
        memcpy(host, p, host_len);
        host[host_len] = '\0';

        // port
        char *port;
        if (*host_end == ':') {
            const char *port_start = host_end + 1;
            const char *port_end = port_start;
            while (    *port_end
                   &&  *port_end != '/'
                   &&  *port_end != '?'
                   &&  *port_end != '#') {
                port_end++;
            }
            size_t port_len = port_end - port_start;
            port = (char *)vsf_heap_malloc(port_len + 1);
            if (NULL == port) {
                vsf_heap_free(host);
                return -1;
            }
            memcpy(port, port_start, port_len);
            port[port_len] = '\0';
            p = port_end;
        } else {
            port = (char *)vsf_heap_malloc(4);
            if (NULL == port) {
                vsf_heap_free(host);
                return -1;
            }
            if (    (scheme_len == 5)
                &&  (scheme[0] == 'h' || scheme[0] == 'H')
                &&  (scheme[1] == 't' || scheme[1] == 'T')
                &&  (scheme[2] == 't' || scheme[2] == 'T')
                &&  (scheme[3] == 'p' || scheme[3] == 'P')
                &&  (scheme[4] == 's' || scheme[4] == 'S')) {
                strcpy(port, "443");
            } else {
                strcpy(port, "80");
            }
            p = host_end;
        }

        // path
        char *path;
        if ((*p == '\0') || (*p == '?') || (*p == '#')) {
            path = (char *)vsf_heap_malloc(2);
            if (NULL == path) {
                vsf_heap_free(host);
                vsf_heap_free(port);
                return -1;
            }
            strcpy(path, "/");
        } else {
            const char *path_end = p;
            while (*path_end && *path_end != '#') {
                path_end++;
            }
            size_t path_len = path_end - p;
            path = (char *)vsf_heap_malloc(path_len + 1);
            if (NULL == path) {
                vsf_heap_free(host);
                vsf_heap_free(port);
                return -1;
            }
            memcpy(path, p, path_len);
            path[path_len] = '\0';
        }

        *host_out = host;
        *port_out = port;
        *path_out = path;
        return 0;
    } else if (url[0] == '/') {
        // path-absolute relative URL
        const char *frag = strchr(url, '#');
        size_t len = frag ? (size_t)(frag - url) : strlen(url);
        char *path = (char *)vsf_heap_malloc(len + 1);
        if (NULL == path) {
            return -1;
        }
        memcpy(path, url, len);
        path[len] = '\0';
        *host_out = NULL;
        *port_out = NULL;
        *path_out = path;
        return 0;
    }

    return -1;
}

int vsf_http_client_request_with_redirect(vsf_http_client_t *http,
                                          vsf_http_client_req_t *req,
                                          int max_redirect)
{
    vsf_http_client_req_t cur_req = *req;
    int redirect_count = 0;
    int result;
    char *host = NULL;
    char *port = NULL;
    char *path = NULL;


    while (1) {
        result = vsf_http_client_request(http, &cur_req);
        if (result < 0) {
            goto do_exit;
        }

        int status = http->resp_status;
        if (    (status != 301) && (status != 302)
            &&  (status != 303) && (status != 307)
            &&  (status != 308)) {
            break;
        }

        if ((max_redirect <= 0) || (redirect_count >= max_redirect)) {
            result = -1;
            goto do_exit;
        }
        if (http->redirect_path == NULL) {
            result = -1;
            goto do_exit;
        }

        // 303 See Other: change method to GET and drop body
        if (status == 303) {
            cur_req.verb = "GET";
            cur_req.txdata = NULL;
            cur_req.txdata_len = 0;
        }

        char *new_host = NULL, *new_port = NULL, *new_path = NULL;
        if (__vsf_http_client_parse_redirect_url(http->redirect_path,
                                                  &new_host, &new_port, &new_path) < 0) {
            result = -1;
            goto do_exit;
        }

        // Always close the current connection before following a redirect.
        // The 3xx response body, if any, is discarded; reusing the buffer
        // without clearing http->cur_size would corrupt the next response.
        vsf_http_client_close(http);
        http->cur_size = 0;
        http->cur_buffer = NULL;

        char *old_host = host;
        char *old_port = port;
        char *old_path = path;
        if ((new_host != NULL) && (new_port != NULL)) {
            host = new_host;
            port = new_port;
        } else {
            // relative URL: keep current host/port
            vsf_heap_free(new_host);
            vsf_heap_free(new_port);
        }
        path = new_path;
        vsf_heap_free(old_host);
        vsf_heap_free(old_port);
        vsf_heap_free(old_path);

        cur_req.host = host;
        cur_req.port = port;
        cur_req.path = path;

        redirect_count++;
    }

do_exit:
    vsf_heap_free(host);
    vsf_heap_free(port);
    vsf_heap_free(path);
    return result;
}


int vsf_http_client_read(vsf_http_client_t *http, uint8_t *buf, size_t len)
{
    int result = 0;
    size_t cur_size;
again:
    while (len && http->cur_size) {
        if (http->is_chunked) {
            if (http->cur_chunk_size < 0) {
                if (http->cur_size < -http->cur_chunk_size) {
                    http->cur_chunk_size += http->cur_size;
                    http->cur_size = 0;
                    goto do_read;
                }
                http->cur_size += http->cur_chunk_size;
                http->cur_buffer -= http->cur_chunk_size;
                http->cur_chunk_size = 0;
                continue;
            } else if (http->cur_chunk_size == 0) {
                char *tmp = strnchr((const char *)http->cur_buffer, http->cur_size, '\n');
                if (NULL == tmp) {
                    memcpy(http->buffer, http->cur_buffer, http->cur_size);
                    http->cur_buffer = http->buffer;
                    goto do_read;
                }
                http->cur_chunk_size = strtoul((const char *)http->cur_buffer, NULL, 16);
                cur_size = ++tmp - (char *)http->cur_buffer;
                http->cur_buffer += cur_size;
                http->cur_size -= cur_size;
                if (0 == http->cur_chunk_size) {
                    http->cur_chunk_size = -2;
                    return result;
                }
            }
            cur_size = vsf_min(len, http->cur_size);
            cur_size = vsf_min(cur_size, http->cur_chunk_size);
        } else {
            cur_size = vsf_min(len, http->cur_size);
        }
        memcpy(buf, http->cur_buffer, cur_size);
        if (http->is_chunked) {
            http->cur_chunk_size -= cur_size;
            if (!http->cur_chunk_size) {
                http->cur_chunk_size = -2;
            }
        }
        len -= cur_size;
        buf += cur_size;
        http->cur_size -= cur_size;
        http->cur_buffer += cur_size;
        result += cur_size;
    }
do_read:
    if (len > 0) {
        int rxlen = http->op->fn_read(http->param, http->buffer + http->cur_size, http->buffer_size - http->cur_size);
        if (rxlen <= 0) {
            return result;
        }
        http->cur_size = rxlen;
        http->cur_buffer = http->buffer;
        goto again;
    }
    return result;
}

int vsf_http_client_write(vsf_http_client_t *http, uint8_t *buf, size_t len)
{
    size_t remaining = len;
    int cur_txlen;

    // NOTE: len is size_t but fn_write/this function return int. In the
    // HTTP use case the total body is bounded well below INT_MAX (typical
    // buffers are a few KB to a few MB), so the implicit size_t -> int
    // narrowing at the return site is always safe. No clamping is added:
    // following the same convention as mbedtls_session_read/write.
    while (remaining > 0) {
        cur_txlen = http->op->fn_write(http->param, buf, remaining);
        if (cur_txlen <= 0) {
            break;
        }
        remaining -= (size_t)cur_txlen;
        buf += cur_txlen;
    }
    return (int)(len - remaining);
}

#endif
