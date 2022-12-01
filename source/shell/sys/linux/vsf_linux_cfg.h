/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

//! \note do not move this pre-processor statement to other places
#include "../../../vsf_cfg.h"

#ifndef __VSF_LINUX_CFG_H__
#define __VSF_LINUX_CFG_H__

#include "shell/vsf_shell_wrapper.h"

/*============================ MACROS ========================================*/

#ifndef VSF_LINUX_ASSERT
#   define VSF_LINUX_ASSERT                     VSF_ASSERT
#endif

#define VSF_LINUX_WRAPPER(__api)                VSF_SHELL_WRAPPER(vsf_linux, __api)
#define VSF_LINUX_SOCKET_WRAPPER(__api)         VSF_SHELL_WRAPPER(vsf_linux_socket, __api)

#ifndef VSF_LINUX_CFG_FD_BITMAP_SIZE
#   define VSF_LINUX_CFG_FD_BITMAP_SIZE         32
#endif

#ifndef VSF_LINUX_CFG_SHM_NUM
#   define VSF_LINUX_CFG_SHM_NUM                32
#endif

#ifndef VSF_LINUX_CFG_PLS_NUM
#   define VSF_LINUX_CFG_PLS_NUM                8
#endif

#ifndef VSF_LINUX_CFG_TLS_NUM
#   define VSF_LINUX_CFG_TLS_NUM                8
#endif

#ifndef VSF_LINUX_CFG_PRIO_LOWEST
#   define VSF_LINUX_CFG_PRIO_LOWEST            vsf_prio_0
#endif

#ifndef VSF_LINUX_CFG_PRIO_HIGHEST
#   define VSF_LINUX_CFG_PRIO_HIGHEST           vsf_prio_0
#endif

#ifndef VSF_LINUX_CFG_SUPPORT_SIG
#   define VSF_LINUX_CFG_SUPPORT_SIG            ENABLED
#endif
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
#   ifndef VSF_LINUX_CFG_PRIO_SIGNAL
// Note that VSF_LINUX_CFG_PRIO_SIGNAL SHOULD be higher than priority of normal
//  linux thread, VSF_LINUX_CFG_PRIO_HIGHEST should be defined to higher priority
#       define VSF_LINUX_CFG_PRIO_SIGNAL        VSF_LINUX_CFG_PRIO_HIGHEST
#   endif
#endif

#ifndef VSF_LINUX_CFG_HOSTNAME
#   define VSF_LINUX_CFG_HOSTNAME               "vsf"
#endif

#ifndef VSF_LINUX_USE_EPOLL
#   define VSF_LINUX_USE_EPOLL                  ENABLED
#endif

#ifndef VSF_LINUX_CFG_BIN_PATH
#   define VSF_LINUX_CFG_BIN_PATH               "/bin"
#endif
#ifndef VSF_LINUX_CFG_FW_PATH
#   define VSF_LINUX_CFG_FW_PATH                "/lib/firmware"
#endif

#ifndef VSF_LINUX_USE_TERMINFO
#   define VSF_LINUX_USE_TERMINFO               ENABLED
#endif
#if VSF_LINUX_USE_TERMINFO == ENABLED
#   ifndef VSF_LINUX_CFG_TERMINFO_PATH
#       define VSF_LINUX_CFG_TERMINFO_PATH      "/usr/share/terminfo"
#   endif
#   ifndef VSF_LINUX_CFG_TERMINFO_TYPE
// A terminfo file will be needed for a dedicated type.
//  Convert the real file by bin2array, and add a the path to the include path.
// There is some terminfo files copied from ubuntu, to use them,
//  add vsf/source/shell/sys/linux/kernel/fs/vfs/files/terminfo to include path.
#       define VSF_LINUX_CFG_TERMINFO_TYPE      "vt100"
#   endif
#endif

#ifndef VSF_LINUX_USE_BUSYBOX
#   define VSF_LINUX_USE_BUSYBOX                ENABLED
#endif

#ifndef VSF_LINUX_USE_SOCKET
#   define VSF_LINUX_USE_SOCKET                 ENABLED
#endif

#if VSF_LINUX_USE_SOCKET == ENABLED
#   ifndef VSF_LINUX_SOCKET_USE_UNIX
#       define VSF_LINUX_SOCKET_USE_UNIX        ENABLED
#   endif
#   ifndef VSF_LINUX_SOCKET_USE_INET
#       if VSF_USE_LWIP == ENABLED
#           define VSF_LINUX_SOCKET_USE_INET    ENABLED
#       elif (defined(__WIN__) || defined(__LINUX__) || defined(__linux__) || defined(__MACOS__))
#           define VSF_LINUX_SOCKET_USE_INET    ENABLED
#           define VSF_LINUX_SOCKET_CFG_WRAPPER ENABLED
#       endif
#   endif
#endif

// to use simple libc
//  1. enable VSF_LINUX_USE_SIMPLE_LIBC
//  2. add "shell/sys/linux/include/simple_libc to include path
//  IMPORTANT: c++ is not supported if simple libc enabled
#if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   ifndef VSF_LINUX_USE_SIMPLE_STDIO
#       define VSF_LINUX_USE_SIMPLE_STDIO       ENABLED
#   endif
#   ifndef VSF_LINUX_USE_SIMPLE_STRING
#       define VSF_LINUX_USE_SIMPLE_STRING      ENABLED
#   endif
#   ifndef VSF_LINUX_USE_SIMPLE_TIME
#       define VSF_LINUX_USE_SIMPLE_TIME        ENABLED
#   endif
#   ifndef VSF_LINUX_USE_SIMPLE_STDLIB
#       define VSF_LINUX_USE_SIMPLE_STDLIB      ENABLED
#   endif
#   ifndef VSF_LINUX_USE_SIMPLE_CTYPE
#       define VSF_LINUX_USE_SIMPLE_CTYPE       ENABLED
#   endif

#   if VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED
#       ifndef VSF_LINUX_LIBC_USE_ENVIRON
#           define VSF_LINUX_LIBC_USE_ENVIRON   ENABLED
#       endif
#       ifndef VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR
#           define VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR DISABLED
#       endif
#       if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
#           ifndef VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH
#               define VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH   0
#           endif
#       endif
#   endif
#   ifndef VSF_LINUX_USE_GETOPT
#       define VSF_LINUX_USE_GETOPT             ENABLED
#   endif
#endif

#if VSF_LINUX_APPLET_USE_LIBC == ENABLED
#   ifndef VSF_LINUX_APPLET_USE_LIBC_STDIO
#       define VSF_LINUX_APPLET_USE_LIBC_STDIO  ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_LIBC_STDLIB
#       define VSF_LINUX_APPLET_USE_LIBC_STDLIB ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_LIBC_STRING
#       define VSF_LINUX_APPLET_USE_LIBC_STRING ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_LIBC_TIME
#       define VSF_LINUX_APPLET_USE_LIBC_TIME   ENABLED
#   endif
#endif

#if VSF_LINUX_USE_APPLET == ENABLED
#   ifndef VSF_LINUX_APPLET_VPLT
#       if VSF_USE_APPLET == ENABLED
#           define VSF_LINUX_APPLET_VPLT                                        \
                ((vsf_linux_vplt_t *)(VSF_APPLET_VPLT->linux))
#       else
#           define VSF_LINUX_APPLET_VPLT                                        \
                ((vsf_linux_vplt_t *)vsf_vplt((void *)0))
#       endif
#   endif

typedef struct vsf_linux_vplt_t {
    vsf_vplt_info_t info;

    // fundmental, vsf APIs for linux
    void *fundmental;

    // libc
    void *libc_stdio;
    void *libc_stdlib;
    void *libc_string;
    void *libc_time;
    void *libc_setjmp;
    void *libc_assert;
    void *libc_math;
    void *libc_res0;
    void *libc_res1;
    void *libc_res2;
    void *libc_res3;
    void *libc_res4;
    void *libc_res5;
    void *libc_res6;
    void *libc_res7;

    // sys
    void *sys_epoll;
    void *sys_select;
    void *sys_time;
    void *sys_wait;
    void *sys_eventfd;
    void *sys_stat;
    void *sys_mman;
    void *sys_utsname;
    void *sys_shm;
    void *sys_mount;
    void *sys_syscall;
    void *sys_socket;
    void *sys_ipc;
    void *sys_syslog;
    void *sys_random;
    void *sys_res0;
    void *sys_res1;
    void *sys_res2;
    void *sys_res3;
    void *sys_res4;
    void *sys_res5;
    void *sys_res6;
    void *sys_res7;

    // unix
    void *unistd;
    void *signal;
    void *pthread;
    void *poll;
    void *semaphore;
    void *fcntl;
    void *dirent;
    void *spawn;
    void *termios;
    void *netdb;
    void *langinfo;
    void *syslog;
    void *sched;
    void *ifaddrs;
    void *arpa_inet;
    void *res0;
    void *res1;
    void *res2;
    void *res3;
    void *res4;
    void *res5;
    void *res6;
    void *res7;

    // libraries
    void *libusb;
    void *libgen;
    void *libgetopt;
    void *libsdl2;
    void *libncurses;
    void *libcurl;

    // for compatibility, new entries added below
} vsf_linux_vplt_t;

#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_vplt_t vsf_linux_vplt;
#   endif
#endif

#ifdef __VSF_APPLET__

#   if  (VSF_LINUX_LIBC_CFG_WRAPPER == ENABLED)                                 \
    ||  (VSF_LINUX_CFG_WRAPPER == ENABLED)                                      \
    ||  (VSF_LINUX_LIBUSB_CFG_WRAPPER == ENABLED)                               \
    ||  (VSF_LINUX_SOCKET_CFG_WRAPPER == ENABLED)
#       error wrappers are not supported in applet.
#   endif

#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */