#ifndef __VSF_LINUX_SYS_EVENT_H__
#define __VSF_LINUX_SYS_EVENT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define kqueue              VSF_LINUX_WRAPPER(kqueue)
#define kevent              VSF_LINUX_WRAPPER(kevent)
#endif

#define EV_SET(__kevp, __ident, __filter, __flags, __fflags, __data, __udata)   \
        do {                                                                    \
            (__kevp)->ident = (__ident);                                        \
            (__kevp)->filter = (__filter);                                      \
            (__kevp)->flags = (__flags);                                        \
            (__kevp)->fflags = (__fflags);                                      \
            (__kevp)->data = (__data);                                          \
            (__kevp)->udata = (__udata);                                        \
        } while (0)

struct kevent {
    uintptr_t               ident;
    short                   filter;
    u_short                 flags;
    u_int                   fflags;
    intptr_t                data;
    void                    *udata;
};

// filter
#define EVFILT_READ         -1
#define EVFILT_WRITE        -2
#define EVFILT_AIO          -3
#define EVFILT_VNODE        -4
#define EVFILT_PROC         -5
#define EVFILT_SIGNAL       -6
#define EVFILT_TIMER        -7
#define EVFILT_FS           -8
#define EVFILT_SYSCOUNT	    8

// actions
#define EV_ADD              (1 << 0)
#define EV_DELETE           (1 << 1)
#define EV_ENABLE           (1 << 2)
#define EV_DISABLE          (1 << 3)
// flags
#define EV_ONSHOT           (1 << 4)
#define EV_CLEAR            (1 << 5)

#define EV_SYSFLAGS         (0xF << 12)
#define EV_EOF              (0x8 << 12)
#define EV_ERROR            (0x4 << 12)

// flags for EVFILT_VNODE
#define NOTE_DELETE         (1 << 0)
#define NOTE_WRITE          (1 << 1)
#define NOTE_EXTEND         (1 << 2)
#define NOTE_ATTRIB         (1 << 3)
#define NOTE_LINK           (1 << 4)
#define NOTE_RENAME         (1 << 5)
#define NOTE_REVOKE         (1 << 6)

// flags for EVFILT_PROC
// flags for EVFILT_NETDEV

#if VSF_LINUX_APPLET_USE_SYS_EVENT == ENABLED
typedef struct vsf_linux_sys_event_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(kqueue);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(kevent);
} vsf_linux_sys_event_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_event_vplt_t vsf_linux_sys_event_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_EVENT == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_EVENT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_EVENT_VPLT                                  \
            ((vsf_linux_sys_event_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_event_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_EVENT_VPLT                                  \
            ((vsf_linux_sys_event_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_EVENT_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_EVENT_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_EVENT_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_EVENT_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_EVENT_IMP(kqueue, int, void) {
    return VSF_LINUX_APPLET_SYS_EVENT_ENTRY(kqueue)();
}
VSF_LINUX_APPLET_SYS_EVENT_IMP(kevent, int, int kq, const struct kevent *changelist, int nchanges, struct kevent *eventlist, int nevents, const struct timespec *timeout) {
    return VSF_LINUX_APPLET_SYS_EVENT_ENTRY(kevent)(kq, changelist, nchanges, eventlist, nevents, timeout);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_EVENT

int kqueue(void);
int kevent(int kq, const struct kevent *changelist, int nchanges,
        struct kevent *eventlist, int nevents,
        const struct timespec *timeout);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_EVENT

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_SYS_EVENT_H__
