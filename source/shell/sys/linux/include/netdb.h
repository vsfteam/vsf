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
#define herror              VSF_LINUX_SOCKET_WRAPPER(herror)
#define hstrerror           VSF_LINUX_SOCKET_WRAPPER(hstrerror)
#endif

extern int * __vsf_linux_h_errno(void);
#define h_errno             (*__vsf_linux_h_errno())

// flags for getnameinfo
#define NI_NAMEREQD         (1 << 0)
#define NI_DGRAM            (1 << 1)
#define NI_NOFQDN           (1 << 2)
#define NI_NUMERICHOST      (1 << 3)
#define NI_NUMERICSERV      (1 << 4)

#if VSF_LINUX_APPLET_USE_NETDB == ENABLED
typedef struct vsf_linux_netdb_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(gethostbyaddr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(gethostbyname);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(gai_strerror);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getnameinfo);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getaddrinfo);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(freeaddrinfo);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(herror);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(hstrerror);
} vsf_linux_netdb_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_netdb_vplt_t vsf_linux_netdb_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_NETDB_LIB__))\
    && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_NETDB == ENABLED

#ifndef VSF_LINUX_APPLET_NETDB_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_NETDB_VPLT                                      \
            ((vsf_linux_netdb_vplt_t *)(VSF_LINUX_APPLET_VPLT->netdb_vplt))
#   else
#       define VSF_LINUX_APPLET_NETDB_VPLT                                      \
            ((vsf_linux_netdb_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_NETDB_ENTRY(__NAME)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_NETDB_VPLT, __NAME)
#define VSF_LINUX_APPLET_NETDB_IMP(...)                                         \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_NETDB_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_NETDB_IMP(gethostbyaddr, struct hostent *, const void *addr, size_t len, int type) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_NETDB_ENTRY(gethostbyaddr)(addr, len, type);
}
VSF_LINUX_APPLET_NETDB_IMP(gethostbyname, struct hostent *, const char *name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_NETDB_ENTRY(gethostbyname)(name);
}
VSF_LINUX_APPLET_NETDB_IMP(gai_strerror, const char *, int errcode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_NETDB_ENTRY(gai_strerror)(errcode);
}
VSF_LINUX_APPLET_NETDB_IMP(getnameinfo, int, const struct sockaddr *addr, socklen_t addrlen, char *host, socklen_t hostlen, char *serv, socklen_t servlen, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_NETDB_ENTRY(getnameinfo)(addr, addrlen, host, hostlen, serv, servlen, flags);
}
VSF_LINUX_APPLET_NETDB_IMP(getaddrinfo, int, const char *name, const char *service, const struct addrinfo *hints, struct addrinfo **pai) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_NETDB_ENTRY(getaddrinfo)(name, service, hints, pai);
}
VSF_LINUX_APPLET_NETDB_IMP(freeaddrinfo, void, struct addrinfo *pai) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_NETDB_ENTRY(freeaddrinfo)(pai);
}
VSF_LINUX_APPLET_NETDB_IMP(herror, void, const char *s) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_NETDB_ENTRY(herror)(s);
}
VSF_LINUX_APPLET_NETDB_IMP(hstrerror, const char *, int err) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_NETDB_ENTRY(hstrerror)(err);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_NETDB

struct hostent * gethostbyaddr(const void *addr, size_t len, int type);
struct hostent * gethostbyname(const char *name);

void setservent(int stayopen);
void endservent(void);
struct servent * getservent(void);
struct servent * getservbyname(const char *name, const char *proto);
struct servent * getservbyport(int port, const char *proto);

const char * gai_strerror(int errcode);

int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                        char *host, socklen_t hostlen,
                        char *serv, socklen_t servlen, int flags);
int getaddrinfo(const char *name, const char *service, const struct addrinfo *hints,
                        struct addrinfo **pai);
void freeaddrinfo(struct addrinfo *pai);

void herror(const char *s);
const char * hstrerror(int err);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_NETDB

#ifdef __cplusplus
}
#endif
#endif
