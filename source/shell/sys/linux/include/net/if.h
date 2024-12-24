#ifndef __VSF_LINUX_NET_IF_H__
#define __VSF_LINUX_NET_IF_H__

#include "shell/sys/linux/vsf_linux_cfg.h"
#include <linux/sockios.h>

#define IFNAMSIZ                16
#define	IFALIASZ                256

struct ifmap {
    unsigned long               mem_start;
    unsigned long               mem_end;
    unsigned short              base_addr;
    unsigned char               irq;
    unsigned char               dma;
    unsigned char               port;
};

struct ifreq {
    union {
        char                    ifrn_name[IFNAMSIZ];
    } ifr_ifrn;

    union {
        struct sockaddr         ifru_addr;
        struct sockaddr         ifru_dstaddr;
        struct sockaddr         ifru_broadaddr;
        struct sockaddr         ifru_netmask;
        struct sockaddr         ifru_hwaddr;
        short                   ifru_flags;
        int                     ifru_ivalue;
        int                     ifru_mtu;
        struct ifmap            ifru_map;
        char                    ifru_slave[IFNAMSIZ];
        char                    ifru_newname[IFNAMSIZ];
        char                   *ifru_data;
    } ifr_ifru;
};
#define ifr_name                ifr_ifrn.ifrn_name
#define ifr_hwaddr              ifr_ifru.ifru_hwaddr
#define	ifr_addr                ifr_ifru.ifru_addr
#define	ifr_dstaddr             ifr_ifru.ifru_dstaddr
#define	ifr_broadaddr           ifr_ifru.ifru_broadaddr
#define	ifr_netmask             ifr_ifru.ifru_netmask
#define	ifr_flags               ifr_ifru.ifru_flags
#define	ifr_metric              ifr_ifru.ifru_ivalue
#define	ifr_mtu                 ifr_ifru.ifru_mtu
#define ifr_map                 ifr_ifru.ifru_map
#define ifr_slave               ifr_ifru.ifru_slave
#define	ifr_data                ifr_ifru.ifru_data
#define ifr_ifindex             ifr_ifru.ifru_ivalue
#define ifr_bandwidth           ifr_ifru.ifru_ivalue
#define ifr_qlen                ifr_ifru.ifru_ivalue
#define ifr_newname             ifr_ifru.ifru_newname
#define ifr_settings            ifr_ifru.ifru_settings

struct ifconf {
    int                         ifc_len;
    union {
        char                   *ifc_buf;
        struct ifreq           *ifc_req;
    };
};

#if VSF_LINUX_APPLET_USE_NET_IF == ENABLED
typedef struct vsf_linux_net_if_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(if_nametoindex);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(if_indextoname);
} vsf_linux_net_if_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_net_if_vplt_t vsf_linux_net_if_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_NET_IF_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_NET_IF == ENABLED

#ifndef VSF_LINUX_APPLET_NET_IF_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_NET_IF_VPLT                                     \
            ((vsf_linux_net_if_vplt_t *)(VSF_LINUX_APPLET_VPLT->net_if_vplt))
#   else
#       define VSF_LINUX_APPLET_NET_IF_VPLT                                     \
            ((vsf_linux_net_if_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_NET_IF_ENTRY(__NAME)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_NET_IF_VPLT, __NAME)
#define VSF_LINUX_APPLET_NET_IF_IMP(...)                                        \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_NET_IF_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_NET_IF_IMP(if_nametoindex, unsigned int, const char *ifname) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_NET_IF_ENTRY(if_nametoindex)(ifname);
}
VSF_LINUX_APPLET_NET_IF_IMP(if_indextoname, char *, unsigned int ifindex, char *ifname) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_NET_IF_ENTRY(if_indextoname)(ifindex, ifname);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_NET_IF

unsigned int if_nametoindex(const char *ifname);
char * if_indextoname(unsigned int ifindex, char *ifname);

#endif

#endif      // __VSF_LINUX_NET_IF_H__
