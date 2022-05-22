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
#define AF_UNIX         1
#define AF_INET         2
#define AF_INET6        10
#define AF_PACKET       17

// protocol families
#define PF_UNSPEC       0
#define PF_UNIX         AF_UNIX
#define PF_INET         AF_INET
#define PF_INET6        AF_INET6
#define PF_PACKET       AF_PACKET

// socket types
#define SOCK_STREAM	    1
#define SOCK_DGRAM      2

#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1

enum {
    IPPROTO_IP          = 0,
    IPPROTO_TCP         = 6,
    IPPROTO_UDP         = 17,
};

enum {
    MSG_NOSIGNAL        = 0,
};

typedef uint32_t        socklen_t;
#define __socklen_t_defined

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

struct linger {
    int                 l_onoff;
    int                 l_linger;
};

#if VSF_LINUX_SOCKET_CFG_WRAPPER == ENABLED
#define VSF_LINUX_SOCKET_WRAPPER(__api)     VSF_SHELL_WRAPPER(vsf_linux_socket, __api)

#define setsockopt      VSF_LINUX_SOCKET_WRAPPER(setsockopt)
#define getsockopt      VSF_LINUX_SOCKET_WRAPPER(getsockopt)

#define getpeername     VSF_LINUX_SOCKET_WRAPPER(getpeername)
#define getsockname     VSF_LINUX_SOCKET_WRAPPER(getsockname)

#define accept          VSF_LINUX_SOCKET_WRAPPER(accept)
#define bind            VSF_LINUX_SOCKET_WRAPPER(bind)
#define connect         VSF_LINUX_SOCKET_WRAPPER(connect)
#define listen          VSF_LINUX_SOCKET_WRAPPER(listen)
#define recv            VSF_LINUX_SOCKET_WRAPPER(recv)
#define recvfrom        VSF_LINUX_SOCKET_WRAPPER(recvfrom)
#define send            VSF_LINUX_SOCKET_WRAPPER(send)
#define sendto          VSF_LINUX_SOCKET_WRAPPER(sendto)
#define shutdown        VSF_LINUX_SOCKET_WRAPPER(shutdown)
#define socket          VSF_LINUX_SOCKET_WRAPPER(socket)
#define socketpair      VSF_LINUX_SOCKET_WRAPPER(socketpair)
#endif

// level for sockopt
#define SOL_SOCKET      0xFFFF
// sock options
#define SO_DEBUG        1
#define SO_REUSEADDR    2
#define SO_ACCEPTCONN   3
#define SO_KEEPALIVE    4
#define SO_BROADCAST    5
#define SO_TYPE         6
#define SO_ERROR        7
#define SO_SNDBUF       8
#define SO_RCVBUF       9
#define SO_RCVTIMEO     10
#define SO_SNDTIMEO     11
#define SO_NONBLOCK     12
#define SO_LINGER       13
#define SO_PRIORITY     14

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
