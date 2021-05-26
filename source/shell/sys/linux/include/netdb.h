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

struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    socklen_t        ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};

#ifdef __cplusplus
}
#endif
#endif
