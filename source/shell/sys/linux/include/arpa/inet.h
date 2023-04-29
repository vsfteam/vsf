#ifndef __VSF_LINUX_INET_H__
#define __VSF_LINUX_INET_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../netinet/in.h"
#else
#   include <netinet/in.h>
#endif

// for endian APIs
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_SOCKET_CFG_WRAPPER == ENABLED
#define inet_addr           VSF_LINUX_SOCKET_WRAPPER(inet_addr)
#define inet_lnaof          VSF_LINUX_SOCKET_WRAPPER(inet_lnaof)
#define inet_makeaddr       VSF_LINUX_SOCKET_WRAPPER(inet_makeaddr)
#define inet_netof          VSF_LINUX_SOCKET_WRAPPER(inet_netof)
#define inet_network        VSF_LINUX_SOCKET_WRAPPER(inet_network)
#define inet_aton           VSF_LINUX_SOCKET_WRAPPER(inet_aton)
#define inet_ntoa           VSF_LINUX_SOCKET_WRAPPER(inet_ntoa)
#define inet_pton           VSF_LINUX_SOCKET_WRAPPER(inet_pton)
#define inet_ntop           VSF_LINUX_SOCKET_WRAPPER(inet_ntop)
#endif

#define htonl               cpu_to_be32
#define htons               cpu_to_be16
#define ntohl               be32_to_cpu
#define ntohs               be16_to_cpu

#if VSF_LINUX_APPLET_USE_ARPA_INET == ENABLED
typedef struct vsf_linux_arpa_inet_vplt_t {
    vsf_vplt_info_t info;
} vsf_linux_arpa_inet_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_arpa_inet_vplt_t vsf_linux_arpa_inet_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_ARPA_INET_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_ARPA_INET == ENABLED

#ifndef VSF_LINUX_APPLET_ARPA_INET_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_ARPA_INET_VPLT                                  \
            ((vsf_linux_termios_vplt_t *)(VSF_LINUX_APPLET_VPLT->arpa_inet))
#   else
#       define VSF_LINUX_APPLET_ARPA_INET_VPLT                                  \
            ((vsf_linux_termios_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_ARPA_INET_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_ARPA_INET_VPLT, __NAME)
#define VSF_LINUX_APPLET_ARPA_INET_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_ARPA_INET_VPLT, __VA_ARGS__)

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_ARPA_INET

in_addr_t inet_addr(const char *cp);
in_addr_t inet_lnaof(struct in_addr in);
struct in_addr inet_makeaddr(in_addr_t net, in_addr_t lna);
in_addr_t inet_netof(struct in_addr in);
in_addr_t inet_network(const char *cp);
int inet_aton(const char *cp, struct in_addr *addr);
char * inet_ntoa(struct in_addr in);
int inet_pton(int af, const char *src, void *dst);
const char * inet_ntop(int af, const void *src, char *dst, socklen_t size);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_ARPA_INET

#ifdef __cplusplus
}
#endif

#endif
