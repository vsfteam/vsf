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

#include "vsf.h"

#if APP_USE_SOCKET_DEMO == ENABLED && APP_USE_LINUX_DEMO == ENABLED && VSF_USE_TCPIP == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

int socket_main(int argc, char *argv[])
{
    const char *host = "www.baidu.com";

    vk_socket_win_t socket = {
        .op     = &vk_socket_win_op,
    };
    vk_netdrv_addr_t addr;

    vk_socket_win_init();

    vk_socket_set_default_op((vk_socket_op_t *)&vk_socket_win_op);
    vk_dns_gethostbyname(host, &addr);
    printf("ip for %s is %d.%d.%d.%d\r\n", host,
                addr.addr_buf[0], addr.addr_buf[1],
                addr.addr_buf[2], addr.addr_buf[3]);

    vk_socket_open(&socket.use_as__vk_socket_t, VSF_SOCKET_AF_INET, VSF_SOCKET_IPPROTO_TCP);
    vk_socket_close(&socket.use_as__vk_socket_t);

    vk_socket_win_fini();

    return 0;
}

#endif
