#ifndef __VSF_LINUX_SYS_SELECT_H__
#define __VSF_LINUX_SYS_SELECT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/time.h"
#   include "../signal.h"
#else
#   include <sys/time.h>
#   include <signal.h>
#endif
// for bitmap
#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#   define select               VSF_LINUX_WRAPPER(select)
#elif defined(__WIN__) && !defined(__VSF_APPLET__)
#   define select               VSF_LINUX_WRAPPER(select)
#   define pselect              VSF_LINUX_WRAPPER(pselect)
#endif

// syscalls

#define __NR_select             select

#ifndef FD_SETSIZE
#   define FD_SETSIZE           1024
#endif
#define FD_ZERO(set)            vsf_bitmap_reset((set)->fds_bits, FD_SETSIZE)
#define FD_SET(fd, set)         vsf_bitmap_set((set)->fds_bits, (fd))
#define FD_CLR(fd, set)         vsf_bitmap_clear((set)->fds_bits, (fd))
#define FD_ISSET(fd, set)       vsf_bitmap_get((set)->fds_bits, (fd))

#define NFDBITS                 (sizeof(fd_mask) << 3)

typedef uintalu_t fd_mask;
typedef struct fd_set {
    fd_mask fds_bits[(FD_SETSIZE + NFDBITS - 1) / NFDBITS];
} fd_set;
//__vsf_declare_bitmap_ex(fd_set, FD_SETSIZE)

#if VSF_LINUX_APPLET_USE_SYS_SELECT == ENABLED
typedef struct vsf_linux_sys_select_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(select);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pselect);
} vsf_linux_sys_select_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_select_vplt_t vsf_linux_sys_select_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_SELECT_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_SELECT == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_SELECT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_SELECT_VPLT                                 \
            ((vsf_linux_sys_select_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_select_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_SELECT_VPLT                                 \
            ((vsf_linux_sys_select_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_SELECT_ENTRY(__NAME)                               \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_SELECT_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_SELECT_IMP(...)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_SELECT_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_SELECT_IMP(select, int, int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SELECT_ENTRY(select)(nfds, readfds, writefds, exceptfds, timeout);
}
VSF_LINUX_APPLET_SYS_SELECT_IMP(pselect, int, int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout_ts, const sigset_t *sigmask) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SELECT_ENTRY(pselect)(nfds, readfds, writefds, exceptfds, timeout_ts, sigmask);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SELECT

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
        struct timeval *timeout);
int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
        const struct timespec *timeout_ts, const sigset_t *sigmask);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SELECT

#ifdef __cplusplus
}
#endif

#endif
