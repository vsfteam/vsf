#ifndef __VSF_LINUX_SYS_SOCKET_H__
#define __VSF_LINUX_SYS_SOCKET_H__

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

#define SOMAXCONN               256

#define SCM_RIGHTS              0x01
#define SCM_CREDENTIALS         0x02
#define SCM_SECURITY            0x03

// protocol families
// refer: https://code.woboq.org/gtk/include/bits/socket.h.html
#define PF_UNSPEC               0
#define PF_LOCAL                1
#define PF_UNIX                 PF_LOCAL
#define PF_INET                 2
#define PF_INET6                10
#define PF_NETLINK              16
#define PF_PACKET               17

// standard address families
#define AF_UNSPEC               PF_UNSPEC
#define AF_LOCAL                PF_LOCAL
#define AF_UNIX                 PF_UNIX
#define AF_FILE                 PF_FILE
#define AF_INET                 PF_INET
#define AF_INET6                PF_INET6
#define AF_NETLINK              PF_NETLINK
#define AF_PACKET               PF_PACKET

#define SOL_IP                  IPPROTO_IP
#define SOL_TCP                 IPPROTO_TCP
#define SOL_UDP                 IPPROTO_UDP

#define IP_TOS                  1
#define IP_TTL                  2
#define IP_HDRINCL              3
#define IP_OPTIONS              4

enum sock_type {
    SOCK_STREAM                 = 1,
    SOCK_DGRAM                  = 2,
    SOCK_RAW                    = 3,
    SOCK_RDM                    = 4,
    SOCK_SEQPACKET              = 5,
    SOCK_PACKET                 = 10,
};
#define SOCK_MAX                (SOCK_PACKET + 1)

// flags for socket/socketpair/accept4
#define SOCK_CLOEXEC            O_CLOEXEC
#define SOCK_NONBLOCK           O_NONBLOCK

#define INVALID_SOCKET          -1
#define SOCKET_ERROR            -1

typedef uint32_t                socklen_t;
#define __socklen_t_defined

typedef uint16_t                sa_family_t;
struct sockaddr {
    sa_family_t                 sa_family;
    char                        sa_data[32];
};
struct sockaddr_storage {
    sa_family_t                 ss_family;
    // TODO: need to be larger?
    char                        sa_data[32];
};

struct linger {
    int                         l_onoff;
    int                         l_linger;
};

#if VSF_LINUX_SOCKET_CFG_WRAPPER == ENABLED
#define VSF_LINUX_SOCKET_WRAPPER(__api)     VSF_SHELL_WRAPPER(vsf_linux_socket, __api)

#define setsockopt              VSF_LINUX_SOCKET_WRAPPER(setsockopt)
#define getsockopt              VSF_LINUX_SOCKET_WRAPPER(getsockopt)

#define getpeername             VSF_LINUX_SOCKET_WRAPPER(getpeername)
#define getsockname             VSF_LINUX_SOCKET_WRAPPER(getsockname)

#define accept                  VSF_LINUX_SOCKET_WRAPPER(accept)
#define accept4                 VSF_LINUX_SOCKET_WRAPPER(accept4)
#define bind                    VSF_LINUX_SOCKET_WRAPPER(bind)
#define connect                 VSF_LINUX_SOCKET_WRAPPER(connect)
#define listen                  VSF_LINUX_SOCKET_WRAPPER(listen)
#define recv                    VSF_LINUX_SOCKET_WRAPPER(recv)
#define recvmsg                 VSF_LINUX_SOCKET_WRAPPER(recvmsg)
#define recvfrom                VSF_LINUX_SOCKET_WRAPPER(recvfrom)
#define send                    VSF_LINUX_SOCKET_WRAPPER(send)
#define sendmsg                 VSF_LINUX_SOCKET_WRAPPER(sendmsg)
#define sendto                  VSF_LINUX_SOCKET_WRAPPER(sendto)
#define shutdown                VSF_LINUX_SOCKET_WRAPPER(shutdown)
#define socket                  VSF_LINUX_SOCKET_WRAPPER(socket)
#define socketpair              VSF_LINUX_SOCKET_WRAPPER(socketpair)
#endif

// syscalls

#define __NR_connect            connect
#define __NR_accept             accept
#define __NR_accept4            accept4
#define __NR_bind               bind

// level for sockopt
#define SOL_SOCKET              0xFFFF
// sock options
// refer: https://code.woboq.org/gtk/include/asm-generic/socket.h.html
#define SO_DEBUG                1
#define SO_REUSEADDR            2
#define SO_TYPE                 3
#define SO_ERROR                4
#define SO_DONTROUTE            5
#define SO_BROADCAST            6
#define SO_SNDBUF               7
#define SO_RCVBUF               8
#define SO_KEEPALIVE            9
#define SO_OOBINLINE            10
#define SO_NO_CHECK             11
#define SO_PRIORITY             12
#define SO_LINGER               13
#define SO_BSDCOMPAT            14
#define SO_REUSEPORT            15
// struct ucred is not implemented
//#define SO_PASSCRED             16
//#define SO_PEERCRED             17
#define SO_RCVLOWAT             18
#define SO_SNDLOWAT             19
#define SO_RCVTIMEO             20
#define SO_SNDTIMEO             21
#define SO_BINDTODEVICE         25
#define SO_MAX_PACING_RATE      47
#define SO_NONBLOCK             100

// IP options
#define IP_TOS                  1
#define IP_TTL                  2
#define IP_HDRINCL              3
#define IP_OPTIONS              4
#define IP_PKTINFO              8

// multicast
// in.h
#define IP_MULTICAST_IF         32
#define IP_MULTICAST_TTL        33
#define IP_MULTICAST_LOOP       34
#define IP_ADD_MEMBERSHIP       35
#define IP_DROP_MEMBERSHIP      36
#define IP_MULTICAST_ALL        49
// in6.h
#define IPV6_UNICAST_HOPS       16
#define IPV6_MULTICAST_IF       17
#define IPV6_MULTICAST_HOPS     18
#define IPV6_MULTICAST_LOOP     19
#define IPV6_JOIN_GROUP         20
#define IPV6_ADD_MEMBERSHIP     IPV6_JOIN_GROUP
#define IPV6_LEAVE_GROUP        21
#define IPV6_DROP_MEMBERSHIP    IPV6_LEAVE_GROUP
#define IPV6_V6ONLY             26

// flags for send/recv
enum {
    MSG_OOB                     = 1 << 0,
#define MSG_OOB                 MSG_OOB
    MSG_PEEK                    = 1 << 1,
#define MSG_PEEK                MSG_PEEK
    MSG_DONTROUTE               = 1 << 2,
#define MSG_DONTROUTE           MSG_DONTROUTE
    MSG_CTRUNC                  = 1 << 3,
#define MSG_CTRUNC              MSG_CTRUNC
    MSG_TRUNC                   = 1 << 5,
#define MSG_TRUNC               MSG_TRUNC
    MSG_DONTWAIT                = 1 << 6,
#define MSG_DONTWAIT            MSG_DONTWAIT
    MSG_WAITALL                 = 1 << 8,
#define MSG_WAITALL             MSG_WAITALL
    MSG_NOSIGNAL                = 1 << 14,
#define MSG_NOSIGNAL            MSG_NOSIGNAL
};

struct msghdr {
    void                        *msg_name;
    socklen_t                   msg_namelen;
    struct iovec                *msg_iov;
    size_t                      msg_iovlen;
    void                        *msg_control;
    size_t                      msg_controllen;
    int                         msg_flags;
};

#define CMSG_ALIGN(len)         (((len)+sizeof(long)-1) & ~(sizeof(long)-1))
#define CMSG_SPACE(len)         (sizeof(struct cmsghdr) + CMSG_ALIGN(len))
#define CMSG_LEN(len)           (sizeof(struct cmsghdr) + (len))
#define CMSG_DATA(cmsg)         (void *)((uint8_t *)(cmsg) + sizeof(struct cmsghdr))
#define CMSG_FIRSTHDR(msg)                                                      \
            ((msg)->msg_controllen >= sizeof(struct cmsghdr) ?                  \
                (struct cmsghdr *)(msg)->msg_control                            \
            :   (struct cmsghdr *)NULL)
struct cmsghdr {
    size_t                      cmsg_len;
    int                         cmsg_level;
    int                         cmsg_type;
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
    SHUT_RD                     = 1 << 0,
    SHUT_WR                     = 1 << 1,
    SHUT_RDWR                   = SHUT_RD | SHUT_WR,
};

#if VSF_LINUX_APPLET_USE_SYS_SOCKET == ENABLED
typedef struct vsf_linux_sys_socket_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setsockopt);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getsockopt);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getpeername);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getsockname);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(accept);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(bind);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(connect);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(listen);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(recv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(recvmsg);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(recvfrom);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(send);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sendmsg);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sendto);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(shutdown);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(socket);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(socketpair);

    // in6addr_any and in6addr_loopback belongs to <netinet/in.h>, which has no vplt, so add to socket vplt
    VSF_APPLET_VPLT_ENTRY_VAR_DEF(in6addr_any);
    VSF_APPLET_VPLT_ENTRY_VAR_DEF(in6addr_loopback);
} vsf_linux_sys_socket_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_socket_vplt_t vsf_linux_sys_socket_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_SOCKET_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_SOCKET == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_SOCKET_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_SOCKET_VPLT                                 \
            ((vsf_linux_sys_socket_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_socket_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_SOCKET_VPLT                                 \
            ((vsf_linux_sys_socket_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(__NAME)                               \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_SOCKET_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_SOCKET_IMP(...)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_SOCKET_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_SOCKET_IMP(setsockopt, int, int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(setsockopt)(sockfd, level, optname, optval, optlen);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(getsockopt, int, int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(getsockopt)(sockfd, level, optname, optval, optlen);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(getpeername, int, int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(getpeername)(sockfd, addr, addrlen);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(getsockname, int, int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(getsockname)(sockfd, addr, addrlen);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(accept, int, int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(accept)(sockfd, addr, addrlen);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(bind, int, int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(bind)(sockfd, addr, addrlen);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(connect, int, int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(connect)(sockfd, addr, addrlen);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(listen, int, int sockfd, int backlog) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(listen)(sockfd, backlog);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(recv, ssize_t, int sockfd, void *buffer, size_t length, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(recv)(sockfd, buffer, length, flags);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(recvmsg, ssize_t, int sockfd, struct msghdr *msg, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(recvmsg)(sockfd, msg, flags);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(recvfrom, ssize_t, int sockfd, void *buffer, size_t length, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(recvfrom)(sockfd, buffer, length, flags, src_addr, addrlen);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(send, ssize_t, int sockfd, const void *buffer, size_t length, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(send)(sockfd, buffer, length, flags);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(sendmsg, ssize_t, int sockfd, const struct msghdr *msg, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(sendmsg)(sockfd, msg, flags);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(sendto, ssize_t, int sockfd, const void *buffer, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(sendto)(sockfd, buffer, length, flags, dest_addr, addrlen);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(shutdown, int, int sockfd, int how) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(shutdown)(sockfd, how);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(socket, int, int domain, int type, int protocol) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(socket)(domain, type, protocol);
}
VSF_LINUX_APPLET_SYS_SOCKET_IMP(socketpair, int, int domain, int type, int protocol, int socket_vector[2]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SOCKET_ENTRY(socketpair)(domain, type, protocol, socket_vector);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SOCKET

int setsockopt(int sockfd, int level, int optname, const void *optval,
                    socklen_t optlen);
int getsockopt(int sockfd, int level, int optname, void *optval,
                    socklen_t *optlen);

int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags);
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
