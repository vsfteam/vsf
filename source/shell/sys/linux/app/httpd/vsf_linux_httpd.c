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

/*============================ INCLUDES ======================================*/

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SOCKET == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_LINUX_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h
#   include "../../include/arpa/inet.h"
#   include "../../include/pthread.h"
#   include "../../include/sys/select.h"
#else
#   include <unistd.h>
#   include <arpa/inet.h>
#   include <pthread.h>
#   include <sys/select.h>
#endif

#define __VSF_LINUX_HTTPD_CLASS_IMPLEMENT
#include "./vsf_linux_httpd.h"

/*============================ MACROS ========================================*/

#if VSF_USE_SIMPLE_STREAM != ENABLED
#   error VSF_USE_SIMPLE_STREAM is needed
#endif

#define MODULE_NAME                             "httpd"

#if VSF_LINUX_HTTPD_CFG_REQUEST_BUFSIZE < 1024
#   warning VSF_LINUX_HTTPD_CFG_REQUEST_BUFSIZE MUST be large enough to hold all \
        http request header and http response header.
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __vsf_linux_httpd_response_t {
    vsf_linux_https_response_t response;
    const char *str;
} __vsf_linux_httpd_response_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const char * __vsf_linux_httpd_method[VSF_LINUX_HTTPD_METHOD_NUM + 1] = {
    [VSF_LINUX_HTTPD_GET]                   = "GET ",
    [VSF_LINUX_HTTPD_POST]                  = "POST ",
    // last NULL terminator
    [VSF_LINUX_HTTPD_METHOD_NUM]            = NULL,
};

static const __vsf_linux_httpd_response_t __vsf_linux_httpd_response[] = {
#define __vsf_linux_httpd_def_response(__response, __str)                       \
    {   .response = (__response),   .str = (__str) }

    __vsf_linux_httpd_def_response(VSF_LINUX_HTTPD_OK, "OK"),
    __vsf_linux_httpd_def_response(VSF_LINUX_HTTPD_BAD_REQUEST, "Bad Request"),
    __vsf_linux_httpd_def_response(VSF_LINUX_HTTPD_NOT_IMPLEMENT, "Not Implemented"),
};

// valid vsf_linux_httpd_content_type_t starts from 1, so minus 1 as index
static const char * __vsf_linux_httpd_content_type[VSF_LINUX_HTTPD_CONTENT_NUM] = {
    [VSF_LINUX_HTTPD_CONTENT_TEXT_XML - 1]  = "text/xml",
};

// valid vsf_linux_httpd_charset_t starts from 1, so minus 1 as index
static const char * __vsf_linux_httpd_charset[VSF_LINUX_HTTPD_CHARSET_NUM] = {
    [VSF_LINUX_HTTPD_CHARSET_UTF8 - 1]      = "utf8",
};

/*============================ IMPLEMENTATION ================================*/

// urihandler
#if VSF_LINUX_HTTPD_CFG_FILESYSTEM == ENABLED

/*============================ MACROS ========================================*/

#if VSF_LINUX_HTTPD_CFG_FILESYSTEM == ENABLED
#   include "./urihandler/file/__vsf_linux_urihandler_file.inc"
#endif

/*============================ MACROS ========================================*/

#define __VSF_LINUX_HTTPD_USE_DEFAULT_URIHANDLER

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_linux_httpd_urihandler_t __vsf_linux_httpd_urihandler[] = {
#if VSF_LINUX_HTTPD_CFG_FILESYSTEM == ENABLED
    {
        .match          = VSF_LINUX_HTTPD_URI_MATCH_ANY,
        .type           = VSF_LINUX_HTTPD_URI_OP,
        .op             = &__vsf_linux_httpd_urihandler_file_op,
    },
#endif
};

#endif

// response

const char * __vsf_linux_httpd_get_response_str(vsf_linux_https_response_t response)
{
    for (int i = 0; i < dimof(__vsf_linux_httpd_response); i++) {
        if (__vsf_linux_httpd_response[i].response == response) {
            return __vsf_linux_httpd_response[i].str;
        }
    }
    // if assert here, add corresponding response to __vsf_linux_httpd_response
    VSF_LINUX_ASSERT(false);
    return "UNKNOWN";
}

const char * __vsf_linux_httpd_get_content_type_str(vsf_linux_httpd_content_type_t type)
{
    // skip VSF_LINUX_HTTPD_CONTEXT_INVALID
    if (    (type > VSF_LINUX_HTTPD_CONTEXT_INVALID)
        &&  (type <= dimof(__vsf_linux_httpd_content_type))) {
        return __vsf_linux_httpd_content_type[type - 1];
    }
    // if assert here, add corresponding content_type to __vsf_linux_httpd_content_type
    VSF_LINUX_ASSERT(false);
    return "UNKNOWN";
}

const char * __vsf_linux_httpd_get_charset_str(vsf_linux_httpd_charset_t charset)
{
    // skip VSF_LINUX_HTTPD_CHARSET_INVALID
    if (    (charset > VSF_LINUX_HTTPD_CHARSET_INVALID)
        &&  (charset <= dimof(__vsf_linux_httpd_charset))) {
        return __vsf_linux_httpd_charset[charset - 1];
    }
    // if assert here, add corresponding charset to __vsf_linux_httpd_charset
    VSF_LINUX_ASSERT(false);
    return "UNKNOWN";
}

// request

static vsf_err_t __vsf_linux_httpd_parse_request(vsf_linux_httpd_request_t *request)
{
    // add NULL terminator to request->buffer
    char *cur_ptr = (char *)request->buffer, *end_ptr, *tmp_ptr, *tmp_end_ptr;
    uint_fast32_t size = vsf_stream_get_data_size(request->stream_in);

    // one more byte has already been reserved
    VSF_LINUX_ASSERT(size <= (sizeof(request->buffer) - 1));
    // append last NULL terminator, so no need to check overflow later
    cur_ptr[size] = '\0';
    end_ptr = strstr((const char *)cur_ptr, "\r\n\r\n");
    if (NULL == end_ptr) {
        return VSF_ERR_NOT_READY;
    }
    end_ptr += 4;
    size = end_ptr - cur_ptr;

    vsf_trace_debug(MODULE_NAME ": parse http request." VSF_TRACE_CFG_LINEEND);

    // http request header ready, parse into request
    // 1. http request method/uri/query/protocol
    // 1.1 http request method
    for (int i = 0; i < dimof(__vsf_linux_httpd_method); i++) {
        if (NULL == __vsf_linux_httpd_method[i]) {
        __not_implement:
            request->response = VSF_LINUX_HTTPD_NOT_IMPLEMENT;
            return VSF_ERR_FAIL;
        }
        if (!strcasecmp((const char *)cur_ptr, __vsf_linux_httpd_method[i])) {
            cur_ptr += strlen(__vsf_linux_httpd_method[i]);
            while (' ' == *cur_ptr) {
                cur_ptr++;
            }
            request->method = i;
            break;
        }
    }
    // 1.2 http request uri
    request->uri = cur_ptr;
    cur_ptr = strchr((const char *)cur_ptr, ' ');
    if (NULL == cur_ptr) {
    __bad_request:
        request->response = VSF_LINUX_HTTPD_NOT_IMPLEMENT;
        return VSF_ERR_FAIL;
    }
    // 1.3 http uri query
    tmp_ptr = strchr((const char *)request->uri, '?');
    if (tmp_ptr != NULL) {
        *tmp_ptr++ = '\0';
        while (' ' == *tmp_ptr) {
            tmp_ptr++;
        }
    }
    request->query = tmp_ptr;
    cur_ptr = strchr((const char *)cur_ptr, ' ');
    if (NULL == cur_ptr) {
        goto __bad_request;
    }
    *cur_ptr++ = '\0';
    while (' ' == *cur_ptr) {
        cur_ptr++;
    }
    // 1.4 http request protocol
    if (!strcasecmp((const char *)cur_ptr, "HTTP/1.")) {
        goto __not_implement;
    }
    cur_ptr = strstr((const char *)cur_ptr, "\r\n");
    if (NULL == cur_ptr) {
        goto __bad_request;
    }
    cur_ptr += 2;

    // 2. htte request header
    while (cur_ptr < end_ptr) {
        tmp_end_ptr = strstr((const char *)cur_ptr, "\r\n");
        if (tmp_end_ptr == cur_ptr) {
            // end of request
            cur_ptr += 2;
            break;
        } else if (NULL == tmp_end_ptr) {
            goto __bad_request;
        }
        *tmp_end_ptr = '\0';
        tmp_end_ptr += 2;

        vsf_trace_debug(MODULE_NAME ": %S" VSF_TRACE_CFG_LINEEND, cur_ptr);

        tmp_ptr = cur_ptr;
        cur_ptr = strchr((const char *)cur_ptr, ':');
        if (NULL == cur_ptr) {
            goto __bad_request;
        }
        while (' ' == *cur_ptr) {
            cur_ptr++;
        }

        if (!strcasecmp((const char *)tmp_ptr, "Content-Length")) {
            request->content_length = atoi(cur_ptr);
        } else if (!strcasecmp((const char *)tmp_ptr, "Range")) {
            goto __not_implement;
        } else if (!strcasecmp((const char *)tmp_ptr, "Connection")) {
            if (!strcasecmp((const char *)cur_ptr, "close")) {
                // no need to set to false, becasue it's default value
            } else if (!strcasecmp((const char *)cur_ptr, "Keep-Alive")) {
                request->keep_alive = true;
            } else {
                goto __bad_request;
            }
        } else if (!strcasecmp((const char *)tmp_ptr, "Content-Type")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Host")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "User-Agent")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Cookie")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Accept")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Accept-Encoding")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Accept-Charset")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Accept-Language")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Refer")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Authorization")) {
        }

        cur_ptr = tmp_end_ptr;
    }

    // read http request header out of stream_in
    vsf_stream_read(request->stream_in, NULL, size);
    return VSF_ERR_NONE;
}

static vsf_linux_httpd_urihandler_t * __vsf_linux_httpe_parse_uri_in_list(
                    vsf_linux_httpd_urihandler_t *urihandler,
                    uint_fast16_t num_of_handler,
                    char *uri)
{
    char *ext = strrchr(uri, '.');
    if (ext != NULL) {
        ext++;
    }

    for (uint_fast16_t i = 0; i < num_of_handler; i++, urihandler++) {
        if (    ((urihandler->match & VSF_LINUX_HTTPD_URI_MATCH_EXT) && (ext != NULL) && !strcmp(ext, urihandler->ext))
            ||  ((urihandler->match & VSF_LINUX_HTTPD_URI_MATCH_URI) && !strcmp(uri, urihandler->uri))
            ||  (urihandler->match & VSF_LINUX_HTTPD_URI_MATCH_ANY)) {
            return urihandler;
        }
    }
    return NULL;
}

static vsf_linux_httpd_urihandler_t * __vsf_linux_httpd_parse_uri(vsf_linux_httpd_session_t *session, char *uri)
{
    vsf_linux_httpd_t *httpd = session->httpd;
    vsf_linux_httpd_urihandler_t * urihandler = __vsf_linux_httpe_parse_uri_in_list(httpd->urihandler, httpd->num_of_urihandler, uri);

#ifdef __VSF_LINUX_HTTPD_USE_DEFAULT_URIHANDLER
    if (NULL == urihandler) {
        urihandler = __vsf_linux_httpe_parse_uri_in_list((vsf_linux_httpd_urihandler_t *)__vsf_linux_httpd_urihandler, dimof(__vsf_linux_httpd_urihandler), uri);
    }
#endif

    return urihandler;
}

// session
#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
imp_vsf_pool(vsf_linux_httpd_session_pool, vsf_linux_httpd_session_t)
#endif

static void __vsf_linux_httpd_stream_evthandler(void *param, vsf_stream_evt_t evt)
{
    vsf_linux_httpd_session_t *session = param;
    vsf_err_t err;

    switch (evt) {
    case VSF_STREAM_ON_RX:
        // stream_write is called in httpd thread, so can use all linux APIs here
        err = __vsf_linux_httpd_parse_request(&session->request);
        if (err < 0) {
            // TODO: return request->response
        } else if (VSF_ERR_NONE == err) {
            // request parsed, close stream_in(note that there maybe data in stream_in)
            uint8_t *ptr;
            uint_fast32_t size = vsf_stream_get_rbuf(session->request.stream_in, &ptr);

            VSF_LINUX_ASSERT(session->fd_stream_out >= 0);
            close(session->fd_stream_in);
            session->fd_stream_out = -1;
            session->request.stream_in = NULL;
            session->wait_stream_in = false;

            // find a suitable urihandler, and pass ptr/size to urihandler init
            vsf_linux_httpd_urihandler_t *urihandler;
            char *uri = session->request.uri;
            while (true) {
                urihandler = __vsf_linux_httpd_parse_uri(session, uri);
                if (NULL == urihandler) {
                    vsf_trace_error(MODULE_NAME ": fail to parse uri." VSF_TRACE_CFG_LINEEND);
                    // TODO: error handler
                }
                if (VSF_LINUX_HTTPD_URI_REMAP == urihandler->type) {
                    uri = urihandler->target_uri;
                    continue;
                }
                break;
            }

            session->request.urihandler = urihandler;
            if (VSF_ERR_NONE != urihandler->op->init_fn(&session->request, ptr, size)) {
                vsf_trace_error(MODULE_NAME ": fail to initialize urihandler." VSF_TRACE_CFG_LINEEND);
                // TODO: error handler
            }

            vsf_linux_fd_t *sfd;
            vsf_stream_t *stream;

            stream = session->request.stream_in;
            if (stream != NULL) {
                sfd = vsf_linux_rx_stream(stream);
                if (NULL == sfd) {
                    vsf_trace_error(MODULE_NAME ": fail to create fd for stream_in." VSF_TRACE_CFG_LINEEND);
                    // TODO: error handler
                }
                session->fd_stream_in = sfd->fd;
            }

            stream = session->request.stream_out;
            VSF_LINUX_ASSERT(stream != NULL);
            sfd = vsf_linux_tx_stream(stream);
            if (NULL == sfd) {
                vsf_trace_error(MODULE_NAME ": fail to create fd for stream_out." VSF_TRACE_CFG_LINEEND);
                // TODO: error handler
            }
            session->fd_stream_out = sfd->fd;

            // write response header to stream_out
#if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED
            // code here uses simple_libc FILE, which is vsf_linux_fd_t
            FILE *f = (FILE *)sfd;
            fprintf(f, "HTTP/1.1 %d %s\r\nServer: vsf_linux_httpd\r\n",
                session->request.response,
                __vsf_linux_httpd_get_response_str(session->request.response));
            if (session->request.content_type != VSF_LINUX_HTTPD_CONTEXT_INVALID) {
                fprintf(f, "Content-Type: %s",
                    __vsf_linux_httpd_get_content_type_str(session->request.content_type));
                if (session->request.charset != VSF_LINUX_HTTPD_CHARSET_INVALID) {
                    fprintf(f, "; charset=%s",
                        __vsf_linux_httpd_get_charset_str(session->request.charset));
                }
                fprintf(f, "\r\n");
            }
            fprintf(f, "Content-Length: %d\r\n", session->request.content_length);
            // TODO: add other headers here
            fprintf(f, "\r\n");
#else
# error TODO: add implementation without stdio in simple_libc
#endif

            if (VSF_ERR_NONE != urihandler->op->serve_fn(&session->request)) {
                vsf_trace_error(MODULE_NAME ": fail to serve urihandler." VSF_TRACE_CFG_LINEEND);
                // SHOULD not error here because init_fn succeeded.
                VSF_LINUX_ASSERT(false);
            }
        }
        break;
    }
}

static void __vsf_linux_httpd_session_delete(vsf_linux_httpd_t *httpd, vsf_linux_httpd_session_t *session)
{
    if (session->fd_stream_in >= 0) {
        close(session->fd_stream_in);
    }
    if (session->fd_stream_out >= 0) {
        close(session->fd_stream_out);
    }
    if (session->fd_socket >= 0) {
        close(session->fd_socket);
    }

    vsf_dlist_remove(vsf_linux_httpd_session_t, session_node, &httpd->session_list, session);
#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
    VSF_POOL_FREE(vsf_linux_httpd_session_pool, &httpd->session_pool, session);
#else
    free(session);
#endif
}

static vsf_linux_httpd_session_t * __vsf_linux_httpd_session_new(vsf_linux_httpd_t *httpd)
{
    vsf_linux_httpd_session_t *session;
#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
    session = VSF_POOL_ALLOC(vsf_linux_httpd_session_pool, &httpd->session_pool);
#else
    session = malloc(sizeof(vsf_linux_httpd_session_t));
#endif

    if (session != NULL) {
        memset(session, 0, sizeof(*session));

        // setup context for handling http header
        vsf_mem_stream_t *stream = &session->request.urihandler_ctx.header.stream;
        stream->op = &vsf_mem_stream_op;
        stream->buffer = session->request.buffer;
        // reserve one byte for NULL terminator
        stream->size = sizeof(session->request.buffer) - 1;
        stream->align = 0;
        VSF_STREAM_INIT(stream);

        vsf_linux_fd_t *sfd = vsf_linux_rx_stream(&stream->use_as__vsf_stream_t);
        if (NULL == sfd) {
            free(session);
            return NULL;
        }

        stream->rx.evthandler = __vsf_linux_httpd_stream_evthandler;
        stream->rx.param = session;
        VSF_STREAM_CONNECT_RX(stream);

        vsf_dlist_init_node(vsf_linux_httpd_session_t, session_node, session);
        session->fd_socket = session->fd_stream_out = -1;
        session->wait_stream_out = session->wait_stream_in = false;
        session->request.urihandler = NULL;
        session->request.stream_out = NULL;
        session->request.stream_in = &stream->use_as__vsf_stream_t;
        session->fd_stream_in = sfd->fd;
        session->httpd = httpd;
        vsf_dlist_add_to_head(vsf_linux_httpd_session_t, session_node, &httpd->session_list, session);
    }
    return session;
}

// httpd
static int __vsf_linux_httpd_set_fds(vsf_linux_httpd_t *httpd, fd_set *rset, fd_set *wset)
{
    int fd_max = -1;
    __vsf_dlist_foreach_unsafe(vsf_linux_httpd_session_t, session_node, &httpd->session_list) {
        if (_->request.stream_out != NULL) {
            VSF_LINUX_ASSERT(_->fd_stream_out >= 0);

            if (!_->wait_stream_out) {
                if (_->fd_socket > fd_max) {
                    fd_max = _->fd_socket;
                }
                FD_SET(_->fd_socket, wset);
            } else {
                if (_->fd_stream_out > fd_max) {
                    fd_max = _->fd_stream_out;
                }
                FD_SET(_->fd_stream_out, rset);
            }
        }
        if (_->request.stream_in != NULL) {
            VSF_LINUX_ASSERT(_->fd_stream_in >= 0);

            if (!_->wait_stream_in) {
                if (_->fd_socket > fd_max) {
                    fd_max = _->fd_socket;
                }
                FD_SET(_->fd_socket, rset);
            } else {
                if (_->fd_stream_in > fd_max) {
                    fd_max = _->fd_stream_in;
                }
                FD_SET(_->fd_stream_in, wset);
            }
        }
    }
    return fd_max;
}

static void * __vsf_linux_httpd_thread(void *param)
{
    vsf_linux_httpd_t *httpd = param;
    int fd_listen, fd_num, fd_socket;
    fd_set rfds, wfds;
    struct sockaddr_in host_addr;
    vsf_stream_t *stream;

#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
    VSF_POOL_PREPARE(vsf_linux_httpd_session_pool, &httpd->session_pool);
    VSF_POOL_ADD_BUFFER(vsf_linux_httpd_session_pool, &httpd->session_pool, &httpd->sessions, sizeof(httpd->sessions));
#endif

    fd_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd_listen < 0) {
        vsf_trace_error(MODULE_NAME ": fail to cerate listen socket." VSF_TRACE_CFG_LINEEND);
        goto __exit;
    }

    host_addr.sin_family = AF_INET;
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    host_addr.sin_port = htons(httpd->port);
    if (bind(fd_listen, (struct sockaddr *)&host_addr, sizeof(host_addr))) {
        vsf_trace_error(MODULE_NAME ": fail to bind socket to port %d." VSF_TRACE_CFG_LINEEND, httpd->port);
        goto __close_fd_and_exit;
    }

    if (listen(fd_listen, httpd->backlog)) {
        vsf_trace_error(MODULE_NAME ": fail to listen." VSF_TRACE_CFG_LINEEND);
        goto __close_fd_and_exit;
    }

    while (true) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_SET(fd_listen, &rfds);
        fd_num = __vsf_linux_httpd_set_fds(httpd, &rfds, &wfds);
        fd_num = max(fd_num, fd_listen) + 1;
        fd_num = select(fd_num, &rfds, &wfds, NULL, NULL);
        if (fd_num < 0) {
            vsf_trace_error(MODULE_NAME ": fail to select." VSF_TRACE_CFG_LINEEND);
            goto __close_session_fd_and_exit;
        } else if (0 == fd_num) {
            continue;
        }

        if (FD_ISSET(fd_listen, &rfds)) {
            FD_CLR(fd_listen, &rfds);
            fd_num--;

            struct sockaddr_in client_addr;
            socklen_t client_addr_len;
            fd_socket = accept(fd_listen, (struct sockaddr *)&client_addr, &client_addr_len);
            if (fd_socket >= 0) {
                struct vsf_linux_httpd_session_t *session = __vsf_linux_httpd_session_new(httpd);
                if (NULL == session) {
                    close(fd_socket);
                } else {
                    session->client_addr = client_addr;
                    session->fd_socket = fd_socket;
                }
            }
        }

        __vsf_dlist_foreach_next_unsafe(vsf_linux_httpd_session_t, session_node, &httpd->session_list) {
            VSF_LINUX_ASSERT(fd_num >= 0);
            if (0 == fd_num) {
                break;
            }

            VSF_LINUX_ASSERT(_->fd_socket >= 0);

            // can write to stream_in or can read from socket, mutual exclusive events
            bool is_socket_readable = FD_ISSET(_->fd_socket, &rfds);
            bool is_stream_writable = (_->fd_stream_in >= 0) && FD_ISSET(_->fd_stream_in, &rfds);
            stream = _->request.stream_in;
            VSF_LINUX_ASSERT((is_socket_readable && !is_stream_writable) || (!is_socket_readable && is_stream_writable));
            if (is_socket_readable || is_stream_writable) {
                fd_num--;

                VSF_LINUX_ASSERT(stream != NULL);

                uint8_t *ptr;
                uint_fast32_t size = vsf_stream_get_wbuf(stream, &ptr);
                if (size > 0) {
                    ssize_t realsize = read(_->fd_socket, ptr, size);
                    if (realsize < 0) {
                        vsf_trace_error(MODULE_NAME ": fail to read socket." VSF_TRACE_CFG_LINEEND);
                        // TODO: process socket error
                    }
                    vsf_stream_write(stream, NULL, realsize);
                    _->wait_stream_in = false;
                } else {
                    VSF_LINUX_ASSERT(!is_stream_writable);
                    _->wait_stream_in = true;
                }
            }

            // can read from stream_out or can write to socket, mutual exclusive events
            bool is_socket_writable = FD_ISSET(_->fd_socket, &rfds);
            bool is_stream_readable = (_->fd_stream_out >= 0) && FD_ISSET(_->fd_stream_out, &rfds);
            stream = _->request.stream_out;
            VSF_LINUX_ASSERT((is_socket_writable && !is_stream_readable) || (!is_socket_writable && is_stream_readable));
            if (is_socket_writable || is_stream_readable) {
                fd_num--;

                VSF_LINUX_ASSERT(stream != NULL);

                uint8_t *ptr;
                uint_fast32_t size = vsf_stream_get_rbuf(stream, &ptr);
                if (size > 0) {
                    ssize_t realsize = write(_->fd_socket, ptr, size);
                    if (realsize < 0) {
                        vsf_trace_error(MODULE_NAME ": fail to write socket." VSF_TRACE_CFG_LINEEND);
                        // TODO: process socket error
                    }
                    vsf_stream_read(stream, NULL, realsize);
                    _->wait_stream_out = false;
                } else {
                    VSF_LINUX_ASSERT(!is_stream_readable);

                    if (vsf_stream_is_tx_connected(stream)) {
                        _->wait_stream_out = true;
                    } else {
                        // tx side of stream_out is disconnected, session end
                        _->request.urihandler->op->fini_fn(&_->request);
                        __vsf_linux_httpd_session_delete(httpd, _);
                    }
                }
            }
        }
    }

__close_session_fd_and_exit:
    __vsf_dlist_foreach_next_unsafe(vsf_linux_httpd_session_t, session_node, &httpd->session_list) {
        if (_->request.urihandler != NULL) {
            _->request.urihandler->op->fini_fn(&_->request);
        }
        __vsf_linux_httpd_session_delete(httpd, _);
    }
__close_fd_and_exit:
    if (fd_listen >= 0) {
        close(fd_listen);
    }
__exit:
    return NULL;
}

vsf_err_t vsf_linux_httpd_start(vsf_linux_httpd_t *httpd)
{
    return 0 == pthread_create(NULL, NULL, __vsf_linux_httpd_thread, httpd) ?
                VSF_ERR_NONE : VSF_ERR_FAIL;
}

#endif
