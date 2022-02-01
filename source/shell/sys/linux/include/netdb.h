#ifndef __VSF_LINUX_NETDB_H__
#define __VSF_LINUX_NETDB_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/socket.h"
#else
#   include <sys/socket.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ai_flags
#define AI_PASSIVE          0x0001
#define AI_CANONNAME        0x0002
#define AI_NUMERICHOST      0x0004
#define AI_V4MAPPED         0x0008
#define AI_ALL              0x0010
#define AI_ADDRCONFIG       0x0020
#define AI_NUMERICSERV      0x0400

#define NI_MAXHOST          1025
#define NI_MAXSERV          32

// error value for getaddrinfo
#define EAI_BADFLAGS        -1
#define EAI_NONAME          -2
#define EAI_AGAIN           -3
#define EAI_FAIL            -4
#define EAI_FAMILY          -6
#define EAI_SOCKTYPE        -7
#define EAI_SERVICE         -8
#define EAI_MEMORY          -10
#define EAI_SYSTEM          -11
#define EAI_EVERFLOW        -12

struct addrinfo {
    int                     ai_flags;
    int                     ai_family;
    int                     ai_socktype;
    int                     ai_protocol;
    socklen_t               ai_addrlen;
    struct sockaddr        *ai_addr;
    char                   *ai_canonname;
    struct addrinfo        *ai_next;
};

struct hostent {
    char                   *h_name;
    char                  **h_aliases;
    int                     h_addrtype;
    int                     h_length;
    char                  **h_addr_list;
#define h_addr              h_addr_list[0]
};

struct netent {
    char                   *n_name;
    char                  **n_aliases;
    int                     n_addrtype;
    uint32_t                n_net;
};

struct protoent {
    char                   *p_name;
    char                  **p_aliases;
    int                     p_proto;
};

struct servent {
    char                   *s_name;
    char                  **s_aliases;
    int                     s_port;
    char                   *s_proto;
};

#if VSF_LINUX_SOCKET_CFG_WRAPPER == ENABLED
#define getnameinfo         VSF_LINUX_SOCKET_WRAPPER(getnameinfo)
#define gethostbyaddr       VSF_LINUX_SOCKET_WRAPPER(gethostbyaddr)
#define gai_strerror        VSF_LINUX_SOCKET_WRAPPER(gai_strerror)
#define gethostbyname       VSF_LINUX_SOCKET_WRAPPER(gethostbyname)
#define getaddrinfo         VSF_LINUX_SOCKET_WRAPPER(getaddrinfo)
#define freeaddrinfo        VSF_LINUX_SOCKET_WRAPPER(freeaddrinfo)
#endif

extern int * __vsf_linux_h_errno(void);
#define h_errno             (*__vsf_linux_h_errno())

struct hostent * gethostbyaddr(const void *addr, size_t len, int type);
struct hostent * gethostbyname(const char *name);

const char * gai_strerror(int errcode);

// flags for getnameinfo
#define NI_NAMEREQD         (1 << 0)
#define NI_DGRAM            (1 << 1)
#define NI_NOFQDN           (1 << 2)
#define NI_NUMERICHOST      (1 << 3)
#define NI_NUMERICSERV      (1 << 4)
int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                        char *host, socklen_t hostlen,
                        char *serv, socklen_t servlen, int flags);
int getaddrinfo(const char *name, const char *service, const struct addrinfo *hints,
                        struct addrinfo **pai);
void freeaddrinfo(struct addrinfo *ai);

#ifdef __cplusplus
}
#endif
#endif
