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

#define MODULE_NAME                             "httpd"

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

// urihandler
#if VSF_LINUX_HTTPD_CFG_FILESYSTEM == ENABLED
#   include "./urihandler/file/__vsf_linux_urihandler_file.inc"
#endif

#if VSF_LINUX_HTTPD_CFG_FILESYSTEM == ENABLED

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

// request

static vsf_err_t __vsf_linux_httpd_parse_request(vsf_linux_httpd_session_t *session)
{
    return VSF_ERR_NOT_READY;
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
        urihandler = __vsf_linux_httpe_parse_uri_in_list(__vsf_linux_httpd_urihandler, sizeof(__vsf_linux_httpd_urihandler), uri);
    }
#endif

    return urihandler;
}

// session
#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
imp_vsf_pool(vsf_linux_httpd_session_pool, vsf_linux_httpd_session_t)
#endif

// this event handler is called in httpd thread, so can use all linux APIs
static void __vsf_linux_httpd_stream_evthandler(void *param, vsf_stream_evt_t evt)
{
    vsf_linux_httpd_session_t *session = param;
    switch (evt) {
    case VSF_STREAM_ON_RX:
        if (VSF_ERR_NONE == __vsf_linux_httpd_parse_request(session)) {
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
                // TODO: error handler
            }

            vsf_linux_fd_t *sfd;
            if (session->request.stream_in != NULL) {
                sfd = vsf_linux_rx_stream(session->request.stream_in);
                if (NULL == sfd) {
                    // TODO: error handler
                }
                session->fd_stream_in = sfd->fd;
            }
            if (session->request.stream_out != NULL) {
                sfd = vsf_linux_tx_stream(session->request.stream_out);
                if (NULL == sfd) {
                    // TODO: error handler
                }
                session->fd_stream_out = sfd->fd;
            }
        }
        break;
    }
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
        // setup context for handling http header
        vsf_mem_stream_t *stream = &session->request.urihandler_ctx.header.stream;
        stream->op = &vsf_mem_stream_op;
        stream->buffer = session->request.buffer;
        stream->size = sizeof(session->request.buffer);
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

#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
    VSF_POOL_PREPARE(vsf_linux_httpd_session_pool, &httpd->session_pool);
    VSF_POOL_ADD_BUFFER(vsf_linux_httpd_session_pool, &httpd->session_pool, &httpd->sessions, sizeof(httpd->sessions));
#endif

    fd_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd_listen < 0) {
        vsf_trace_error(MODULE_NAME ": fail to cerate listen socket.");
        goto __exit;
    }

    host_addr.sin_family = AF_INET;
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    host_addr.sin_port = htons(httpd->port);
    if (bind(fd_listen, (struct sockaddr *)&host_addr, sizeof(host_addr))) {
        vsf_trace_error(MODULE_NAME ": fail to bind socket to port %d.", httpd->port);
        goto __close_fd_and_exit;
    }

    if (listen(fd_listen, httpd->backlog)) {
        vsf_trace_error(MODULE_NAME ": fail to listen.");
        goto __close_fd_and_exit;
    }

    while (true) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_SET(fd_listen, &rfds);
        fd_num = __vsf_linux_httpd_set_fds(httpd, &rfds, &wfds);
        fd_num = select(fd_num, &rfds, &wfds, NULL, NULL);
        if (fd_num < 0) {
            vsf_trace_error(MODULE_NAME ": fail to select.");
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

        __vsf_dlist_foreach_unsafe(vsf_linux_httpd_session_t, session_node, &httpd->session_list) {
            VSF_LINUX_ASSERT(fd_num >= 0);
            if (0 == fd_num) {
                break;
            }

            VSF_LINUX_ASSERT(_->fd_socket >= 0);

            // can write to stream_in or can read from socket, mutual exclusive events
            bool is_socket_readable = FD_ISSET(_->fd_socket, &rfds);
            bool is_stream_writable = (_->fd_stream_in >= 0) && FD_ISSET(_->fd_stream_in, &rfds);
            VSF_LINUX_ASSERT((is_socket_readable && !is_stream_writable) || (!is_socket_readable && is_stream_writable));
            if (is_socket_readable || is_stream_writable) {
                fd_num--;

                VSF_LINUX_ASSERT(_->request.stream_in != NULL);

                uint8_t *ptr;
                uint_fast32_t size = vsf_stream_get_wbuf(_->request.stream_in, &ptr);
                if (size > 0) {
                    ssize_t realsize = read(_->fd_socket, ptr, size);
                    if (realsize < 0) {
                        // TODO: process socket error
                    }
                    vsf_stream_write(_->request.stream_in, NULL, realsize);
                    _->wait_stream_in = false;
                } else {
                    VSF_LINUX_ASSERT(!is_stream_writable);
                    _->wait_stream_in = true;
                }
            }

            // can read from stream_out or can write to socket, mutual exclusive events
            bool is_socket_writable = FD_ISSET(_->fd_socket, &rfds);
            bool is_stream_readable = (_->fd_stream_out >= 0) && FD_ISSET(_->fd_stream_out, &rfds);
            VSF_LINUX_ASSERT((is_socket_writable && !is_stream_readable) || (!is_socket_writable && is_stream_readable));
            if (is_socket_writable || is_stream_readable) {
                fd_num--;

                VSF_LINUX_ASSERT(_->request.stream_out != NULL);

                uint8_t *ptr;
                uint_fast32_t size = vsf_stream_get_rbuf(_->request.stream_out, &ptr);
                if (size > 0) {
                    ssize_t realsize = write(_->fd_socket, ptr, size);
                    if (realsize < 0) {
                        // TODO: process socket error
                    }
                    vsf_stream_read(_->request.stream_out, NULL, realsize);
                    _->wait_stream_out = false;
                } else {
                    VSF_LINUX_ASSERT(!is_stream_readable);
                    _->wait_stream_out = true;
                }
            }
        }
    }

__close_session_fd_and_exit:
    // TODO: close all sessions
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
