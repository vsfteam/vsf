#ifndef __VSF_LINUX_RTNETLINK_H__
#define __VSF_LINUX_RTNETLINK_H__

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#define RTMGRP_LINK         1
#define RTMGRP_IPV4_IFADDR  0x10

struct ifinfomsg {
    unsigned char           ifi_family;
    unsigned char           __ifi_pad;
    unsigned short          ifi_type;
    int                     ifi_index;
    unsigned                ifi_flags;
    unsigned                ifi_change;
};

enum {
    RTM_NEWLINK	            = 16,
#define RTM_NEWLINK	RTM_NEWLINK
    RTM_DELLINK,
#define RTM_DELLINK	RTM_DELLINK
    RTM_NEWADDR,
#define RTM_NEWADDR RTM_NEWADDR
    RTM_DELADDR,
#define RTM_DELADDR RTM_DELADDR
};

struct rtattr {
    unsigned short          rta_len;
    unsigned short          rta_type;
};

#define RTA_ALIGNTO         4U
#define RTA_ALIGN(len)      (((len) + RTA_ALIGNTO - 1) & ~(RTA_ALIGNTO- 1 ))
#define RTA_OK(rta,len)     ((len) >= (int)sizeof(struct rtattr) && \
                            (rta)->rta_len >= sizeof(struct rtattr) && \
                            (rta)->rta_len <= (len))
#define RTA_NEXT(rta,attrlen)   ((attrlen) -= RTA_ALIGN((rta)->rta_len), \
                                (struct rtattr*)(((char*)(rta)) + RTA_ALIGN((rta)->rta_len)))
#define RTA_LENGTH(len)     (RTA_ALIGN(sizeof(struct rtattr)) + (len))
#define RTA_SPACE(len)      RTA_ALIGN(RTA_LENGTH(len))
#define RTA_DATA(rta)       ((void*)(((char*)(rta)) + RTA_LENGTH(0)))
#define RTA_PAYLOAD(rta)    ((int)((rta)->rta_len) - RTA_LENGTH(0))

#endif      // __VSF_LINUX_RTNETLINK_H__
