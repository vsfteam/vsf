#ifndef __VSF_LINUX_PTY_H__
#define __VSF_LINUX_PTY_H__

#include "shell/sys/linux/vsf_linux_cfg.h"
#include "kernel/vsf_kernel.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#else
#   include <sys/types.h>
#endif
// for struct termios
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define openpty         VSF_LINUX_WRAPPER(openpty)
#define login_tty       VSF_LINUX_WRAPPER(login_tty)
#endif

#if VSF_LINUX_APPLET_USE_PTY == ENABLED
typedef struct vsf_linux_pty_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(openpty);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(login_tty);
} vsf_linux_pty_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_pty_vplt_t vsf_linux_pty_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_PTY_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_PTY == ENABLED

#ifndef VSF_LINUX_APPLET_PTY_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_PTY_VPLT                                        \
            ((vsf_linux_pty_vplt_t *)(VSF_LINUX_APPLET_VPLT->pty_vplt))
#   else
#       define VSF_LINUX_APPLET_PTY_VPLT                                        \
            ((vsf_linux_pty_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_PTY_ENTRY(__NAME)                                      \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_PTY_VPLT, __NAME)
#define VSF_LINUX_APPLET_PTY_IMP(...)                                           \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_PTY_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_PTY_IMP(openpty, int, int *amaster, int *aslave, char *name,
                const struct termios *termp,
                const struct winsize *winp) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTY_ENTRY(openpty)(amaster, aslave, name, termp, winp);
}
VSF_LINUX_APPLET_PTY_IMP(login_tty, int, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PTY_ENTRY(login_tty)(fd);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_PTY

int openpty(int *amaster, int *aslave, char *name,
                const struct termios *termp,
                const struct winsize *winp);
int login_tty(int fd);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_PTY

#define forkpty(__amaster, __name, __termp, __winp)         ({                  \
    int VSF_MACRO_SAFE_NAME(master);                                            \
    int VSF_MACRO_SAFE_NAME(slave);                                             \
    int VSF_MACRO_SAFE_NAME(pid);                                               \
    int VSF_MACRO_SAFE_NAME(result) = -1;                                       \
    if (!openpty(&VSF_MACRO_SAFE_NAME(master), &VSF_MACRO_SAFE_NAME(slave), __name, __termp, __winp)) {\
        VSF_MACRO_SAFE_NAME(pid) = vfork();                                     \
        switch (VSF_MACRO_SAFE_NAME(pid)) {                                     \
        case -1:                                                                \
            close(VSF_MACRO_SAFE_NAME(master));                                 \
            close(VSF_MACRO_SAFE_NAME(slave));                                  \
            break;                                                              \
        case 0:                                                                 \
            close(VSF_MACRO_SAFE_NAME(master));                                 \
            if (login_tty(VSF_MACRO_SAFE_NAME(slave))) {                        \
                _exit(1);                                                       \
            }                                                                   \
            VSF_MACRO_SAFE_NAME(result) = 0;                                    \
            break;                                                              \
        default:                                                                \
            *(__amaster) = VSF_MACRO_SAFE_NAME(master);                         \
            close(VSF_MACRO_SAFE_NAME(slave));                                  \
            VSF_MACRO_SAFE_NAME(result) = VSF_MACRO_SAFE_NAME(pid);             \
            break;                                                              \
        }                                                                       \
    }                                                                           \
    VSF_MACRO_SAFE_NAME(result);                                                \
})

#ifdef __cplusplus
}
#endif

#endif
