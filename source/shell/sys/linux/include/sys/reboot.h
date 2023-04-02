#ifndef __VSF_LINUX_REBOOT_H__
#define __VSF_LINUX_REBOOT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define reboot                  VSF_LINUX_WRAPPER(reboot)
#endif

#define RB_AUTOBOOT             0x01234567
#define RB_HALT_SYSTEM          0xcdef0123
#define RB_ENABLE_CAD           0x89abcdef
#define RB_DISABLE_CAD          0
#define RB_POWER_OFF            0x4321fedc
#define RB_SW_SUSPEND           0xd000fce2
#define RB_KEXEC                0x45584543

#if VSF_LINUX_APPLET_USE_SYS_REBOOT == ENABLED
typedef struct vsf_linux_sys_reboot_vplt_t {
    vsf_vplt_info_t info;

    int (*reboot)(int howto);
} vsf_linux_sys_reboot_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_reboot_vplt_t vsf_linux_sys_reboot_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_REBOOT == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_REBOOT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_REBOOT_VPLT                                 \
            ((vsf_linux_sys_reboot_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_reboot_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_REBOOT_VPLT                                 \
            ((vsf_linux_sys_reboot_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

int reboot(int howto) {
    return VSF_LINUX_APPLET_SYS_REBOOT_VPLT->reboot(howto);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_REBOOT

int reboot(int howto);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_REBOOT

#ifdef __cplusplus
}
#endif

#endif
