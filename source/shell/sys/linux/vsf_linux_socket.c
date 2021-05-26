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

#include "./vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./include/unistd.h"
#   include "./include/arpa/inet.h"
#   include "./include/netdb.h"
#else
#   include <unistd.h>
#   include <arpa/inet.h>
#   include <netdb.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

// arpa/inet.h
in_addr_t inet_addr(const char *cp)
{
}

in_addr_t inet_lnaof(struct in_addr in)
{
}

struct in_addr inet_makeaddr(in_addr_t net, in_addr_t lna)
{
}

in_addr_t inet_netof(struct in_addr in)
{
}

in_addr_t inet_network(const char *cp)
{
}

char * inet_ntoa(struct in_addr in)
{
}

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
}

// socket
int setsockopt(int socket, int level, int option_name, const void *option_value,
                    socklen_t option_len)
{
}

int accept(int socket, struct sockaddr *address, socklen_t *address_len)
{
}

int bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
}

int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
}

int getpeername(int socket, struct sockaddr *address, socklen_t *address_len)
{
}

int getsockname(int socket, struct sockaddr *address, socklen_t *address_len)
{
}

int getsockopt(int socket, int level, int option_name, void *option_value,
                    socklen_t *option_len)
{
}

int listen(int socket, int backlog)
{
}

ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
}

ssize_t recvfrom(int socket, void *buffer, size_t length, int flags,
                    struct sockaddr *address, socklen_t *address_len)
{
}

ssize_t send(int socket, const void *message, size_t length, int flags)
{
}

ssize_t sendto(int socket, const void *message, size_t length, int flags,
                    const struct sockaddr *dest_addr, socklen_t dest_len)
{
}

int shutdown(int socket, int how)
{
}

int socket(int domain, int type, int protocol)
{
}

int socketpair(int domain, int type, int protocol, int socket_vector[2])
{
}

// netdb
struct hostent * gethostbyaddr(const void *addr, size_t len, int type)
{
}

struct hostent * gethostbyname(const char *name)
{
}

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints,
                        struct addrinfo **res)
{
}

void freeaddrinfo(struct addrinfo *res)
{
}

#endif
