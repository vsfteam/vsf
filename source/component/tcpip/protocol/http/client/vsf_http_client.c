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

#define __VSF_HTTP_CLIENT_CLASS_IMPLEMENT
#include "./vsf_http_client.h"

#if VSF_USE_MBEDTLS == ENABLED
#   include "component/3rd-party/mbedtls/extension/tls_session/mbedtls_tls_session.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

//#define vsf_http_trace(...)           printf(__VA_ARGS__)
#ifndef vsf_http_trace
#   define vsf_http_trace(...)
#endif

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_MBEDTLS == ENABLED
const vsf_http_op_t vsf_mbedtls_http_op = {
    .fn_connect     = (int (*)(void *, const char *, const char *))mbedtls_session_connect,
    .fn_close       = (void (*)(void *))mbedtls_session_close,
    .fn_write       = (int (*)(void *, uint8_t *, uint16_t))mbedtls_session_write,
    .fn_read        = (int (*)(void *, uint8_t *, uint16_t))mbedtls_session_read,
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

void vsf_http_client_init(vsf_http_client_t *http)
{
    http->cur_size = 0;
}

void vsf_http_client_close(vsf_http_client_t *http)
{
    http->op->fn_close(http->param);
}

int vsf_http_client_request(vsf_http_client_t *http, vsf_http_client_req_t *req)
{
    int result = http->op->fn_connect(http->param, req->host, req->port);
    if (result != 0) {
        return result;
    }

    if (NULL == req->header) {
        req->header = (char *)"Accept: */*\r\n";
    }

    if ((req->txdata_len > 0) && (req->txdata != NULL)) {
        result = sprintf((char *)http->buffer, "\
%s %s HTTP/1.1\r\n\
Host: %s\r\n\
User-Agent: %s\r\n\
Connection: close\r\n\
Content-Length: %d\r\n\
%s\
\r\n", NULL == req->verb ? "POST" : req->verb, req->path, req->host, "vsf", (int)req->txdata_len, req->txdata);
        vsf_http_trace("http request:\n%s", http->buffer);
    } else {
        result = sprintf((char *)http->buffer, "\
%s %s HTTP/1.1\r\n\
Host: %s\r\n\
User-Agent: %s\r\n\
Connection: close\r\n\
%s\
\r\n", NULL == req->verb ? "GET" : req->verb, req->path, req->host, "vsf", req->header);
        vsf_http_trace("http request:\n%s", http->buffer);
    }
    result = http->op->fn_write(http->param, http->buffer, result);
    if (result < 0) {
        return result;
    }

    if ((req->txdata_len > 0) && (req->txdata != NULL)) {
        result = http->op->fn_write(http->param, req->txdata, req->txdata_len);
        if (result < 0) {
            return result;
        }
    }

    vsf_http_trace("http response:\n");
read_more:
    if (http->cur_size >= sizeof(http->buffer)) {
        vsf_http_client_close(http);
        return -1;
    }
    result = http->op->fn_read(http->param, http->buffer + http->cur_size, sizeof(http->buffer) - http->cur_size);
    if (result <= 0) {
        return result;
    }
    http->cur_size = result + http->cur_size;
    http->cur_buffer = http->buffer;
    http->content_length = 0;
    http->is_chunked = false;

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
            line += sizeof("Content-Length:") - 1;
            while (*line && isspace(*line)) { line++; }

            if (strstr(line, "chunked")) {
                http->is_chunked = true;
                http->cur_chunk_size = 0;
            }
            continue;
        }
        if (*line == '\0' || *line == '\r') {
            break;
        }
    }
    return 0;
}

int vsf_http_client_read(vsf_http_client_t *http, uint8_t *buf, uint16_t len)
{
    int result = 0;
    uint16_t cur_size;
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
        int rxlen = http->op->fn_read(http->param, http->buffer + http->cur_size, sizeof(http->buffer) - http->cur_size);
        if (rxlen <= 0) {
            return result;
        }
        http->cur_size = rxlen;
        http->cur_buffer = http->buffer;
        goto again;
    }
    return result;
}

#endif
