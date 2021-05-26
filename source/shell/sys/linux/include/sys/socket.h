#ifndef __VSF_LINUX_SOCKET_H__
#define __VSF_LINUX_SOCKET_H__

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

// standard address families
#define AF_INET         2
#define AF_INET6        10

// protocol families
#define PF_INET         AF_INET
#define PF_INET6        AF_INET6

// socket types
#define SOCK_STREAM	    1
#define SOCK_DGRAM      2

enum {
    IPPROTO_TCP         = 6,
    IPPROTO_UDP         = 17,
};

typedef uint32_t        socklen_t;

typedef uint16_t        sa_family_t;
struct sockaddr {
    sa_family_t         sa_family;
    char                sa_data[14];
};

#if VSF_LINUX_CFG_FAKE_API == ENABLED
#define setsockopt      __vsf_linux_setsockopt
#endif

// level for setsockopt
#define SOL_SOCKET      0xFFFF
// option_name for setsockopt
#define SO_DEBUG        0x0001
#define SO_ACCEPTCONN   0x0002
#define SO_KEEPALIVE    0x0008
#define SO_BROADCAST    0x0020
int setsockopt(int socket, int level, int option_name,
       const void *option_value, socklen_t option_len);

#ifdef __cplusplus
}
#endif

#endif
