#ifndef __VSF_LINUX_NETPACKET_PACKET_H__
#define __VSF_LINUX_NETPACKET_PACKET_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct sockaddr_ll {
    unsigned short  sll_family;
    unsigned short  sll_protocol;
    int             sll_ifindex;
    unsigned short  sll_hatype;
    unsigned char   sll_pkttype;
    unsigned char   sll_halen;
    unsigned char   sll_addr[8];
};

#ifdef __cplusplus
}
#endif

#endif
