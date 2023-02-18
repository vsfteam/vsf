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

#define SOMAXCONN       256

#define SCM_RIGHTS      0x01
#define SCM_CREDENTIALS 0x02
#define SCM_SECURITY    0x03

// protocol families
// refer: https://code.woboq.org/gtk/include/bits/socket.h.html
#define PF_UNSPEC       0
#define PF_LOCAL        1
#define PF_UNIX         PF_LOCAL
#define PF_INET         2
#define PF_INET6        10
#define PF_PACKET       17

// standard address families
#define AF_UNSPEC       PF_UNSPEC
#define AF_LOCAL        PF_LOCAL
#define AF_UNIX         PF_UNIX
#define AF_FILE         PF_FILE
#define AF_INET         PF_INET
#define AF_INET6        PF_INET6
#define AF_PACKET       PF_PACKET

// socket types
// refer: https://code.woboq.org/gtk/include/bits/socket_type.h.html
#define SOCK_STREAM     1
#define SOCK_DGRAM      2

#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1

enum {
    IPPROTO_IP          = 0,
    IPPROTO_TCP         = 6,
    IPPROTO_UDP         = 17,
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
#define recvmsg         VSF_LINUX_SOCKET_WRAPPER(recvmsg)
#define recvfrom        VSF_LINUX_SOCKET_WRAPPER(recvfrom)
#define send            VSF_LINUX_SOCKET_WRAPPER(send)
#define sendmsg         VSF_LINUX_SOCKET_WRAPPER(sendmsg)
#define sendto          VSF_LINUX_SOCKET_WRAPPER(sendto)
#define shutdown        VSF_LINUX_SOCKET_WRAPPER(shutdown)
#define socket          VSF_LINUX_SOCKET_WRAPPER(socket)
#define socketpair      VSF_LINUX_SOCKET_WRAPPER(socketpair)
#endif

// level for sockopt
#define SOL_SOCKET      0xFFFF
// sock options
// refer: https://code.woboq.org/gtk/include/asm-generic/socket.h.html
#define SO_DEBUG        1
#define SO_REUSEADDR    2
#define SO_TYPE         3
#define SO_ERROR        4
#define SO_DONTROUTE    5
#define SO_BROADCAST    6
#define SO_SNDBUF       7
#define SO_RCVBUF       8
#define SO_KEEPALIVE    9
#define SO_OOBINLINE    10
#define SO_NO_CHECK     11
#define SO_PRIORITY     12
#define SO_LINGER       13
#define SO_BSDCOMPAT    14
#define SO_REUSEPORT    15
// struct ucred is not implemented
//#define SO_PASSCRED     16
//#define SO_PEERCRED     17
#define SO_RCVLOWAT     18
#define SO_SNDLOWAT     19
#define SO_RCVTIMEO     20
#define SO_SNDTIMEO     21
#define SO_NONBLOCK     100

// flags for send/recv
enum {
    MSG_OOB             = 1 << 0,
#define MSG_OOB         MSG_OOB
    MSG_PEEK            = 1 << 1,
#define MSG_PEEK        MSG_PEEK
    MSG_WAITALL         = 1 << 2,
#define MSG_WAITALL     MSG_WAITALL
    MSG_NOSIGNAL        = 1 << 3,
#define MSG_NOSIGNAL    MSG_NOSIGNAL
    MSG_DONTWAIT        = 1 << 4,
#define MSG_DONTWAIT    MSG_DONTWAIT
};

struct msghdr {
    void                *msg_name;
    socklen_t           msg_namelen;
    struct iovec        *msg_iov;
    size_t              msg_iovlen;
    void                *msg_control;
    size_t              msg_controllen;
    int                 msg_flags;
};

#define CMSG_ALIGN(len) (((len)+sizeof(long)-1) & ~(sizeof(long)-1))
#define CMSG_SPACE(len) (sizeof(struct cmsghdr) + CMSG_ALIGN(len))
#define CMSG_LEN(len)   (sizeof(struct cmsghdr) + (len))
#define CMSG_DATA(cmsg) (void *)((uint8_t *)(cmsg) + sizeof(struct cmsghdr))
#define CMSG_FIRSTHDR(msg)                                                      \
            ((msg)->msg_controllen >= sizeof(struct cmsghdr) ?                  \
                (struct cmsghdr *)(msg)->msg_control                            \
            :   (struct cmsghdr *)NULL)
struct cmsghdr {
    size_t              cmsg_len;
    int                 cmsg_level;
    int                 cmsg_type;
};

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

static inline struct cmsghdr * CMSG_NXTHDR(struct msghdr *__msg, struct cmsghdr *__cmsg)
{
    struct cmsghdr * __ptr;

    __ptr = (struct cmsghdr*)(((unsigned char *) __cmsg) +  CMSG_ALIGN(__cmsg->cmsg_len));
    if ((unsigned long)((char*)(__ptr+1) - (char *)__msg->msg_control) > __msg->msg_controllen)
        return (struct cmsghdr *)0;

    return __ptr;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

// how for shutdown
enum {
    SHUT_RD             = 1 << 0,
    SHUT_WR             = 1 << 1,
    SHUT_RDWR           = SHUT_RD | SHUT_WR,
};

#if VSF_LINUX_APPLET_USE_SYS_SOCKET == ENABLED
typedef struct vsf_linux_sys_socket_vplt_t {
    vsf_vplt_info_t info;

    int (*setsockopt)(int sockfd, int level, int optname, const void *optval,
                    socklen_t optlen);
    int (*getsockopt)(int sockfd, int level, int optname, void *optval,
                    socklen_t *optlen);

    int (*getpeername)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    int (*getsockname)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

    int (*accept)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    int (*bind)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    int (*connect)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    int (*listen)(int sockfd, int backlog);

    ssize_t (*recv)(int sockfd, void *buffer, size_t length, int flags);
    ssize_t (*recvmsg)(int sockfd, struct msghdr *msg, int flags);
    ssize_t (*recvfrom)(int sockfd, void *buffer, size_t length, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen);
    ssize_t (*send)(int sockfd, const void *message, size_t length, int flags);
    ssize_t (*sendmsg)(int sockfd, const struct msghdr *msg, int flags);
    ssize_t (*sendto)(int sockfd, const void *message, size_t length, int flags,
                    const struct sockaddr *dest_addr, socklen_t addrlen);

    int (*shutdown)(int sockfd, int how);
    int (*socket)(int domain, int type, int protocol);
    int (*socketpair)(int domain, int type, int protocol, int socket_vector[2]);
} vsf_linux_sys_socket_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_socket_vplt_t vsf_linux_sys_socket_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_SOCKET == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_SOCKET_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_SOCKET_VPLT                                 \
            ((vsf_linux_sys_socket_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_socket))
#   else
#       define VSF_LINUX_APPLET_SYS_SOCKET_VPLT                                 \
            ((vsf_linux_sys_socket_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->setsockopt(socket, level, optname, optval, optlen);
}
static inline int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->getsockopt(socket, level, optname, optval, optlen);
}

static inline int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->getpeername(socket, addr, addrlen);
}
static inline int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->getsockname(socket, addr, addrlen);
}

static inline int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->accept(socket, addr, addrlen);
}
static inline int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->bind(socket, addr, addrlen);
}
static inline int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->connect(socket, addr, addrlen);
}
static inline int listen(int sockfd, int backlog) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->listen(socket, backlog);
}

static inline ssize_t recv(int sockfd, void *buffer, size_t length, int flags) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->recv(socket, buffer, length, flags);
}
static inline ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->recvmsg(socket, msg, flags);
}
static inline ssize_t recvfrom(int sockfd, void *buffer, size_t length, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->recvfrom(socket, buffer, length, flags,
                    src_addr, addrlen);
}
static inline ssize_t send(int sockfd, const void *buffer, size_t length, int flags) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->send(socket, buffer, length, flags);
}
static inline ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->sendmsg(socket, msg, flags);
}
static inline ssize_t sendto(int sockfd, const void *buffer, size_t length, int flags,
                    const struct sockaddr *dest_addr, socklen_t addrlen) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->sendto(socket, buffer, length, flags,
                    dest_addr, addrlen);
}

static inline int shutdown(int sockfd, int how) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->shutdown(socket, how);
}
static inline int socket(int domain, int type, int protocol) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->socket(domain, type, protocol);
}
static inline int socketpair(int domain, int type, int protocol, int socket_vector[2]) {
    return VSF_LINUX_APPLET_SYS_SOCKET_VPLT->socketpair(domain, type, protocol, socket_vector);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SOCKET

int setsockopt(int sockfd, int level, int optname, const void *optval,
                    socklen_t optlen);
int getsockopt(int sockfd, int level, int optname, void *optval,
                    socklen_t *optlen);

int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);

ssize_t recv(int sockfd, void *buffer, size_t length, int flags);
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
ssize_t recvfrom(int sockfd, void *buffer, size_t length, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t send(int sockfd, const void *buffer, size_t length, int flags);
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
ssize_t sendto(int sockfd, const void *buffer, size_t length, int flags,
                    const struct sockaddr *dest_addr, socklen_t addrlen);

int shutdown(int sockfd, int how);
int socket(int domain, int type, int protocol);
int socketpair(int domain, int type, int protocol, int socket_vector[2]);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SOCKET

#ifdef __cplusplus
}
#endif

#endif
