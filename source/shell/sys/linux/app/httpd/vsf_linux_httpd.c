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

// session
#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
imp_vsf_pool(vsf_linux_httpd_session_pool, vsf_linux_httpd_session_t)
#endif

static vsf_linux_httpd_session_t * __vsf_linux_httpd_session_new(vsf_linux_httpd_t *httpd)
{
    vsf_linux_httpd_session_t *session;
#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
    session = VSF_POOL_ALLOC(vsf_linux_httpd_session_pool, &httpd->session_pool);
#else
    session = malloc(sizeof(vsf_linux_httpd_session_t));
#endif

    if (session != NULL) {
        vsf_dlist_init_node(vsf_linux_httpd_session_t, session_node, session);
        session->fd_socket = session->fd_stream_out = -1;
        session->request.urihandler = NULL;
        vsf_dlist_add_to_head(vsf_linux_httpd_session_t, session_node, &httpd->session_list, session);
    }
    return session;
}

// httpd
static int __vsf_linux_httpd_set_fds(vsf_linux_httpd_t *httpd, fd_set *rset, fd_set *wset)
{
    int fd_max = -1;
    __vsf_dlist_foreach_unsafe(vsf_linux_httpd_session_t, session_node, &httpd->session_list) {
        FD_SET(_->fd_socket, rset);
        if (_->request.urihandler != NULL) {
            FD_SET(_->fd_socket, wset);
        }
        if (_->fd_socket > fd_max) {
            fd_max = _->fd_socket;
        }

        if (_->fd_stream_out >= 0) {
            if (_->fd_stream_out > fd_max) {
                fd_max = _->fd_stream_out;
            }
            FD_SET(_->fd_stream_out, rset);
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
            if (FD_ISSET(_->fd_socket, &rfds)) {
                fd_num--;

                // can read from socket
                if (NULL == _->request.urihandler) {
                    // urihandler not set, recive header
                } else {
                    // urihandler set, 
                    if (NULL == _->request.stream_in) {
                        VSF_LINUX_ASSERT(false);
                        // TODO: read all data out
                        continue;
                    } else {
                        // read socket data and write to stream_in
                    }
                }
            }
            if (FD_ISSET(_->fd_socket, &wfds)) {
                fd_num--;

                // can write to socket
            }
            if ((_->fd_stream_out >= 0) && FD_ISSET(_->fd_stream_out, &rfds)) {
                fd_num--;

                // can read from stream_out
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
