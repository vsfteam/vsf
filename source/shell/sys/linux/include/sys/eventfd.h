#ifndef __VSF_LINUX_EVENTFD_H__
#define __VSF_LINUX_EVENTFD_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "../fcntl.h"
#else
#   include <sys/types.h>
#   include <fcntl.h>
#endif
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define eventfd                 VSF_LINUX_WRAPPER(eventfd)
#define eventfd_read            VSF_LINUX_WRAPPER(eventfd_read)
#define eventfd_write           VSF_LINUX_WRAPPER(eventfd_write)
#endif

#define EFD_SEMAPHORE           (1 << 0)
#define EFD_CLOEXEC             O_CLOEXEC
#define EFD_NONBLOCK            O_NONBLOCK

typedef uint64_t                eventfd_t;

#if VSF_LINUX_APPLET_USE_SYS_EVENTFD == ENABLED
typedef struct vsf_linux_sys_eventfd_vplt_t {
    vsf_vplt_info_t info;
} vsf_linux_sys_eventfd_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_eventfd_vplt_t vsf_linux_sys_eventfd_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_EVENTFD == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_EVENTFD_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_EVENTFD_VPLT                                \
            ((vsf_linux_sys_eventfd_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_eventfs))
#   else
#       define VSF_LINUX_APPLET_SYS_EVENTFD_VPLT                                \
            ((vsf_linux_sys_eventfd_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_EVENTFD

int eventfd(int count, int flags);
int eventfd_read(int fd, eventfd_t *value);
int eventfd_write(int fd, eventfd_t value);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_EVENTFD

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_EVENTFD_H__
