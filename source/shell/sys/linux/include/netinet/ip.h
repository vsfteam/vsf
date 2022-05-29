#ifndef __VSF_LINUX_NETINET_IP_H__
#define __VSF_LINUX_NETINET_IP_H__

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

#define SOL_IP              IPPROTO_IP
// ip options
// refer: https://code.woboq.org/gtk/include/bits/in.h.html
#define IP_TOS              1

#define IPTOS_TOS_MASK      0x1E
#define IPTOS_TOS(__TOS)    ((__TOS) & IPTOS_TOS_MASK)
#define	IPTOS_LOWDELAY      0x10
#define	IPTOS_THROUGHPUT    0x08
#define	IPTOS_RELIABILITY   0x04
#define	IPTOS_MINCOST       0x02

#ifdef __cplusplus
}
#endif

#endif
