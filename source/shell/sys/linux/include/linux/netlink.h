#ifndef __VSF_LINUX_NETLINK_H__
#define __VSF_LINUX_NETLINK_H__

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/socket.h"
#else
#   include <sys/socket.h>
#endif

#define RTMGRP_LINK         1
#define RTMGRP_IPV4_IFADDR  0x10

#define NLMSG_NOOP          0x1
#define NLMSG_ERROR         0x2
#define NLMSG_DONE          0x3
#define NLMSG_OVERRUN       0x4

#define NETLINK_ROUTE       0

struct sockaddr_nl {
    sa_family_t             nl_family;
    uint16_t                nl_pad;
    uint32_t                nl_pid;
    uint32_t                nl_groups;
};

struct nlmsghdr {
    uint32_t                nlmsg_len;
    uint16_t                nlmsg_type;
    uint16_t                nlmsg_flags;
    uint16_t                nlmsg_seq;
    uint16_t                nlmsg_pid;
};

struct nlmsgerr {
    int                     error;
    struct nlmsghdr         msg;
};

#define NLMSG_OK(nlh,len)   ((len) >= (int)sizeof(struct nlmsghdr) && \
                            (nlh)->nlmsg_len >= sizeof(struct nlmsghdr) && \
                            (nlh)->nlmsg_len <= (len))

#endif      // __VSF_LINUX_NETLINK_H__
