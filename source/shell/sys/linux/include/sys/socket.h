#ifndef __VSF_LINUX_SOCKET_H__
#define __VSF_LINUX_SOCKET_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif
// for stdint.h
#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

// standard address families
#define AF_UNSPEC       0
#define AF_INET         2
#define AF_INET6        10

// protocol families
#define PF_UNSPEC       0
#define PF_INET         AF_INET
#define PF_INET6        AF_INET6

// socket types
#define SOCK_STREAM	    1
#define SOCK_DGRAM      2

#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1

enum {
    IPPROTO_TCP         = 6,
    IPPROTO_UDP         = 17,
};

typedef uint32_t        socklen_t;

typedef uint16_t        sa_family_t;
struct sockaddr {
    sa_family_t         sa_family;
    char                sa_data[14];
};
struct sockaddr_storage {
    sa_family_t         ss_family;
    // TODO: need to be larger?
    char                sa_data[14];
};

#if VSF_LINUX_SOCKET_CFG_WRAPPER == ENABLED
#define setsockopt      __vsf_linux_setsockopt
#define getsockopt      __vsf_linux_getsockopt

#define getpeername     __vsf_linux_getpeername
#define getsockname     __vsf_linux_getsockname

#define accept          __vsf_linux_accept
#define bind            __vsf_linux_bind
#define connect         __vsf_linux_connect
#define listen          __vsf_linux_listen
#define recv            __vsf_linux_recv
#define recvfrom        __vsf_linux_recvfrom
#define send            __vsf_linux_send
#define sendto          __vsf_linux_sendto
#define shutdown        __vsf_linux_shutdown
#define socket          __vsf_linux_socket
#define socketpair      __vsf_linux_socketpair
#endif

// level for setsockopt
#define SOL_SOCKET      0xFFFF
// option_name for setsockopt
enum {
    SO_DEBUG            = 1,
    SO_REUSEADDR,
    SO_ACCEPTCONN,
    SO_KEEPALIVE,
    SO_BROADCAST,

    SO_TYPE,
    SO_ERROR,
    SO_SNDBUF,
    SO_RCVBUF,
};
int setsockopt(int socket, int level, int optname, const void *optval,
                    socklen_t optlen);
int getsockopt(int socket, int level, int optname, void *optval,
                    socklen_t *optlen);

int getpeername(int socket, struct sockaddr *addr, socklen_t *addrlen);
int getsockname(int socket, struct sockaddr *addr, socklen_t *addrlen);

int accept(int socket, struct sockaddr *addr, socklen_t *addrlen);
int bind(int socket, const struct sockaddr *addr, socklen_t addrlen);
int connect(int socket, const struct sockaddr *addr, socklen_t addrlen);
int listen(int socket, int backlog);

// flags
enum {
    MSG_OOB             = 1 << 0,
    MSG_PEEK            = 1 << 1,
    MSG_WAITALL         = 1 << 2,
};
ssize_t recv(int socket, void *buffer, size_t length, int flags);
ssize_t recvfrom(int socket, void *buffer, size_t length, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t send(int socket, const void *message, size_t length, int flags);
ssize_t sendto(int socket, const void *message, size_t length, int flags,
                    const struct sockaddr *dest_addr, socklen_t addrlen);
// how for shutdown
enum {
    SHUT_RD             = 1 << 0,
    SHUT_WR             = 1 << 1,
    SHUT_RDWR           = SHUT_RD | SHUT_WR,
};
int shutdown(int socket, int how);
int socket(int domain, int type, int protocol);
int socketpair(int domain, int type, int protocol, int socket_vector[2]);

#ifdef __cplusplus
}
#endif

#endif
