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

#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SOCKET == ENABLED && VSF_LINUX_SOCKET_USE_INET == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#define __VSF_LINUX_TELNETD_CLASS_IMPLEMENT
#include "./vsf_linux_telnetd.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
$   include "../../include/unistd.h"
#   include "../../include/sys/time.h"
#else
#   include <unistd.h>
#   include <sys/time.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_linux_telnetd_thread(vsf_linux_telnetd_t *telnetd)
{
    telnetd->listener_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (telnetd->listener_sock < 0) {
        vsf_trace_error("telnetd: fail to create socket\n");
        return;
    }

    struct sockaddr_in addr = {
        .sin_family         = AF_INET,
        .sin_port           = htons(telnetd->port),
        .sin_addr.s_addr    = INADDR_ANY,
    };
    if (bind(telnetd->listener_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        vsf_trace_error("telnetd: fail to bind port %d\n", telnetd->port);
        return;
    }

    if (0 == telnetd->backlog) {
        telnetd->backlog = 1;
    }
    if (listen(telnetd->listener_sock, telnetd->backlog) < 0) {
        vsf_trace_error("telnetd: fail to listen socket\n");
        return;
    }

    while (true) {
        socklen_t addr_len = sizeof(addr);
        int client_sock = accept(telnetd->listener_sock, (struct sockaddr *)&addr, &addr_len);
        if (client_sock < 0) {
            vsf_trace_error("telnetd: fail to accept socket\n");
            continue;
        }

        int opt = 1;
        if (setsockopt(client_sock, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
            vsf_trace_error("telnetd: fail to keep alive socket\n");
            goto close_client;
        }

        // implement shell process manually to avoid posix_spawn
        vsf_linux_process_t *process = vsf_linux_create_process(0);
        if (NULL == process) {
            vsf_trace_error("telnetd: fail to create child process\n");
            goto close_client;
        }

        extern int vsh_main(int argc, char *argv[]);
        process->ctx.entry = vsh_main;
        process->ctx.arg.argc = 1;
        process->ctx.arg.argv[0] = "sh";

        extern int __vsf_linux_fd_create_ex(vsf_linux_process_t *process, vsf_linux_fd_t **sfd,
                const vsf_linux_fd_op_t *op, int fd_desired, vsf_linux_fd_priv_t *priv);
        vsf_linux_fd_t *sfd, *sfd_from = vsf_linux_fd_get(client_sock);
        if (STDIN_FILENO != __vsf_linux_fd_create_ex(process, &sfd, sfd_from->op, STDIN_FILENO, sfd_from->priv)) {
            goto delete_process_and_close_client;
        }
        if (STDOUT_FILENO != __vsf_linux_fd_create_ex(process, &sfd, sfd_from->op, STDOUT_FILENO, sfd_from->priv)) {
            goto delete_process_and_close_client;
        }
        if (STDERR_FILENO != __vsf_linux_fd_create_ex(process, &sfd, sfd_from->op, STDERR_FILENO, sfd_from->priv)) {
            goto delete_process_and_close_client;
        }
        vsf_linux_detach_process(process);
        process->shell_process = process;
        vsf_linux_start_process(process);

        close(client_sock);
        continue;
    delete_process_and_close_client:
        vsf_linux_delete_process(process);
    close_client:
        close(client_sock);
    }
}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SOCKET && VSF_LINUX_SOCKET_USE_INET
