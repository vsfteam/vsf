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

#ifndef VSF_LINUX_CFG_SUPPORT_SHM
#   define VSF_LINUX_CFG_SUPPORT_SHM            ENABLED
#endif
#ifndef VSF_LINUX_CFG_SUPPORT_SEM
#   define VSF_LINUX_CFG_SUPPORT_SEM            ENABLED
#endif

#ifndef VSF_LINUX_CFG_PLS_NUM
#   define VSF_LINUX_CFG_PLS_NUM                8
#endif

#ifndef VSF_LINUX_CFG_TLS_NUM
#   define VSF_LINUX_CFG_TLS_NUM                8
#endif

#ifndef VSF_LINUX_CFG_FUTEX_NUM
#   define VSF_LINUX_CFG_FUTEX_NUM              8
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

#ifndef VSF_LINUX_CFG_ATEXIT_NUM
#   define VSF_LINUX_CFG_ATEXIT_NUM             32
#endif

#ifndef VSF_LINUX_CFG_PEOCESS_HEAP_SIZE
#   define VSF_LINUX_CFG_PEOCESS_HEAP_SIZE      0
#endif

#ifndef VSF_LINUX_CFG_HOSTNAME
#   define VSF_LINUX_CFG_HOSTNAME               "vsf"
#endif

#ifndef VSF_LINUX_USE_EPOLL
#   define VSF_LINUX_USE_EPOLL                  ENABLED
#endif

#ifndef VSF_LINUX_USE_VFORK
#   define VSF_LINUX_USE_VFORK                  ENABLED
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

#ifndef VSF_LINUX_USE_BUSYBOX
#   define VSF_LINUX_USE_BUSYBOX                ENABLED
#endif

#ifndef VSF_LINUX_USE_SOCKET
#   define VSF_LINUX_USE_SOCKET                 ENABLED
#endif

#ifndef VSF_LINUX_CFG_MAX_PTY
#   define VSF_LINUX_CFG_MAX_PTY                9
#endif
#if VSF_LINUX_CFG_MAX_PTY > 9
#   error currently max for VSF_LINUX_CFG_MAX_PTY is 9
#endif

#if VSF_LINUX_USE_SOCKET == ENABLED
#   ifndef VSF_LINUX_SOCKET_USE_UNIX
#       define VSF_LINUX_SOCKET_USE_UNIX        ENABLED
#   endif
#   ifndef VSF_LINUX_SOCKET_USE_INET
#       if VSF_USE_LWIP == ENABLED
#           define VSF_LINUX_SOCKET_USE_INET    ENABLED
#       elif (defined(__WIN__) || defined(__LINUX__) || defined(__linux__) || defined(__MACOS__)) && !defined(__VSF_APPLET__)
#           define VSF_LINUX_SOCKET_USE_INET    ENABLED
#           define VSF_LINUX_SOCKET_CFG_WRAPPER ENABLED
#       endif
#   endif
#endif

#ifdef __WIN__
// __VSF_LINUX_LIBC_STDIO_WRAPPER_SNPRINTF MUST be enabled on windows to avoid confliction
#   define __VSF_LINUX_LIBC_STDIO_WRAPPER_SNPRINTF    ENABLED
#endif

// VSF_LINUX_CFG_STDIO_FALLBACK is used while user want to access stdio outside linux environment
#ifndef VSF_LINUX_CFG_STDIO_FALLBACK
#   define VSF_LINUX_CFG_STDIO_FALLBACK         ENABLED
#endif

// to use simple libc
//  1. enable VSF_LINUX_USE_SIMPLE_LIBC
//  2. add "shell/sys/linux/include/simple_libc to include path
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
#   ifndef VSF_LINUX_APPLET_USE_SCHED
#       define VSF_LINUX_APPLET_USE_SCHED       ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SPAWN
#       define VSF_LINUX_APPLET_USE_SPAWN       ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_PTY
#       define VSF_LINUX_APPLET_USE_PTY         ENABLED
#   endif

#   if VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED
#       ifndef VSF_LINUX_LIBC_USE_ENVIRON
#           define VSF_LINUX_LIBC_USE_ENVIRON   ENABLED
#       endif
#       ifndef VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR
#           define VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR                 ENABLED
#       endif
#       ifndef VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER
#           define VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER    DISABLED
#       endif
#       ifndef VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_QUIET
#           define VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_QUIET               ENABLED
#       endif
#       ifndef VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_MAX
#           define VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_MAX                 DISABLED
#       endif
#   endif
#   ifndef VSF_LINUX_USE_GETOPT
#       define VSF_LINUX_USE_GETOPT             ENABLED
#   endif
#endif

// do not check VSF_USE_APPLET
#if VSF_LINUX_USE_APPLET == ENABLED
#   ifndef VSF_LINUX_APPLET_USE_DLFCN
#       define VSF_LINUX_APPLET_USE_DLFCN       ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_GLOB
#       define VSF_LINUX_APPLET_USE_GLOB        ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_MNTENT
#       define VSF_LINUX_APPLET_USE_MNTENT      ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SIGNAL
#       define VSF_LINUX_APPLET_USE_SIGNAL      ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_PWD
#       define VSF_LINUX_APPLET_USE_PWD         ENABLED
#   endif

#   ifndef VSF_LINUX_APPLET_USE_SYS_EPOLL
#       define VSF_LINUX_APPLET_USE_SYS_EPOLL   ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_EVENTFD
#       define VSF_LINUX_APPLET_USE_SYS_EVENTFD ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_RANDOM
#       define VSF_LINUX_APPLET_USE_SYS_RANDOM  ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_SELECT
#       define VSF_LINUX_APPLET_USE_SYS_SELECT  ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_SHM
#       define VSF_LINUX_APPLET_USE_SYS_SHM     ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_STAT
#       define VSF_LINUX_APPLET_USE_SYS_STAT    ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_TIME
#       define VSF_LINUX_APPLET_USE_SYS_TIME    ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_TIMEX
#       define VSF_LINUX_APPLET_USE_SYS_TIMEX   ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_UTSNAME
#       define VSF_LINUX_APPLET_USE_SYS_UTSNAME ENABLED
#   endif
#   if !defined(VSF_LINUX_APPLET_USE_SYS_SOCKET) && VSF_LINUX_USE_SOCKET == ENABLED
#       define VSF_LINUX_APPLET_USE_SYS_SOCKET  ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_WAIT
#       define VSF_LINUX_APPLET_USE_SYS_WAIT    ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_SENDFILE
#       define VSF_LINUX_APPLET_USE_SYS_SENDFILE    ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_REBOOT
#       define VSF_LINUX_APPLET_USE_SYS_REBOOT  ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_FILE
#       define VSF_LINUX_APPLET_USE_SYS_FILE    DISABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_PRCTL
#       define VSF_LINUX_APPLET_USE_SYS_PRCTL   ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_EVENT
#       define VSF_LINUX_APPLET_USE_SYS_EVENT   DISABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_SEM
#       define VSF_LINUX_APPLET_USE_SYS_SEM     ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_SIGNALFD
#       define VSF_LINUX_APPLET_USE_SYS_SIGNALFD    DISABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_TIMES
#       define VSF_LINUX_APPLET_USE_SYS_TIMES   ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_STATFS
#       define VSF_LINUX_APPLET_USE_SYS_STATFS  ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_CAPABILITY
#       define VSF_LINUX_APPLET_USE_SYS_CAPABILITY  ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_SYSMACROS
#       define VSF_LINUX_APPLET_USE_SYS_SYSMACROS   ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_STATVFS
#       define VSF_LINUX_APPLET_USE_SYS_STATVFS ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_MMAN
#       define VSF_LINUX_APPLET_USE_SYS_MMAN    ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_RESOURCE
#       define VSF_LINUX_APPLET_USE_SYS_RESOURCE    ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_INFO
#       define VSF_LINUX_APPLET_USE_SYS_INFO    ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SYS_IOCTL
#       define VSF_LINUX_APPLET_USE_SYS_IOCTL   ENABLED
#   endif

#   ifndef VSF_LINUX_APPLET_USE_UNISTD
#       define VSF_LINUX_APPLET_USE_UNISTD      ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_PTHREAD
#       define VSF_LINUX_APPLET_USE_PTHREAD     ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_POLL
#       define VSF_LINUX_APPLET_USE_POLL        ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_SEMAPHORE
#       define VSF_LINUX_APPLET_USE_SEMAPHORE   ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_DIRENT
#       define VSF_LINUX_APPLET_USE_DIRENT      ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_FCNTL
#       define VSF_LINUX_APPLET_USE_FCNTL       ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_IFADDRS
#       define VSF_LINUX_APPLET_USE_IFADDRS     ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_ARPA_INET
#       define VSF_LINUX_APPLET_USE_ARPA_INET   ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_NETDB
#       define VSF_LINUX_APPLET_USE_NETDB       ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_NET_IF
#       define VSF_LINUX_APPLET_USE_NET_IF      ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_GRP
#       define VSF_LINUX_APPLET_USE_GRP         ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_TERMIOS
#       define VSF_LINUX_APPLET_USE_TERMIOS     ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_ERRNO
#       define VSF_LINUX_APPLET_USE_ERRNO       ENABLED
#   endif

#   if !defined(VSF_LINUX_APPLET_USE_LIBUSB) && VSF_LINUX_USE_LIBUSB == ENABLED
#       define VSF_LINUX_APPLET_USE_LIBUSB      ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_LIBGEN
#       define VSF_LINUX_APPLET_USE_LIBGEN      ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_FINDPROG
#       define VSF_LINUX_APPLET_USE_FINDPROG    ENABLED
#   endif
#   ifndef VSF_LINUX_APPLET_USE_FNMATCH
#       define VSF_LINUX_APPLET_USE_FNMATCH     ENABLED
#   endif

#   ifndef VSF_LINUX_APPLET_USE_LIBC
#       define VSF_LINUX_APPLET_USE_LIBC        ENABLED
#   endif
#   if VSF_LINUX_APPLET_USE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#       if !defined(SF_LINUX_APPLET_USE_LIBC_STDIO) && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED
#           define VSF_LINUX_APPLET_USE_LIBC_STDIO  ENABLED
#       endif
#       if !defined(VSF_LINUX_APPLET_USE_LIBC_STDLIB) && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED
#           define VSF_LINUX_APPLET_USE_LIBC_STDLIB ENABLED
#       endif
#       if !defined(VSF_LINUX_APPLET_USE_LIBC_STRING) && VSF_LINUX_USE_SIMPLE_STRING == ENABLED
#           define VSF_LINUX_APPLET_USE_LIBC_STRING ENABLED
#       endif
#       if !defined(VSF_LINUX_APPLET_USE_LIBC_CTYPE) && VSF_LINUX_USE_SIMPLE_CTYPE == ENABLED
#           define VSF_LINUX_APPLET_USE_LIBC_CTYPE  ENABLED
#       endif
#       if !defined(VSF_LINUX_APPLET_USE_LIBC_TIME) && VSF_LINUX_USE_SIMPLE_TIME == ENABLED
#           define VSF_LINUX_APPLET_USE_LIBC_TIME   ENABLED
#       endif
#       ifndef VSF_LINUX_APPLET_USE_LIBC_SETJMP
#           define VSF_LINUX_APPLET_USE_LIBC_SETJMP ENABLED
#       endif
#       ifndef VSF_LINUX_APPLET_USE_LIBC_MATH
#           define VSF_LINUX_APPLET_USE_LIBC_MATH   ENABLED
#       endif
#       if (VSF_LINUX_APPLET_USE_LIBC_MATH == ENABLED) && !defined(VSF_LINUX_APPLET_LIBC_MATH_LONG_DOULBE)
#           define VSF_LINUX_APPLET_LIBC_MATH_LONG_DOULBE   ENABLED
#       endif
#   endif

#   ifndef VSF_LINUX_APPLET_VPLT
#       if VSF_USE_APPLET == ENABLED
#           define VSF_LINUX_APPLET_VPLT                                        \
                ((vsf_linux_vplt_t *)(VSF_APPLET_VPLT->linux_vplt))
#       else
#           define VSF_LINUX_APPLET_VPLT                                        \
                ((vsf_linux_vplt_t *)vsf_vplt((void *)0))
#       endif
#   endif

typedef struct vsf_linux_vplt_t {
    vsf_vplt_info_t info;

    // vsf APIs, MUST be compatible with vsf_vplt_t
    void *applet_vplt;
    void *utilities_vplt;
    void *arch_vplt;
    void *hal_vplt;
    void *service_vplt;
    void *kernel_vplt;
    void *component_vplt;
    void *arch_abi_vplt;
    void *dynamic_vplt;
    void *compiler_vplt;

    // fundmental, vsf APIs for linux
    void *fundmental_vplt;

    // libc
    void *libc_stdio_vplt;
    void *libc_stdlib_vplt;
    void *libc_string_vplt;
    void *libc_ctype_vplt;
    void *libc_time_vplt;
    void *libc_setjmp_vplt;
    void *libc_assert_vplt;
    void *libc_math_vplt;
    void *libc_res0_vplt;
    void *libc_res1_vplt;
    void *libc_res2_vplt;
    void *libc_res3_vplt;
    void *libc_res4_vplt;
    void *libc_res5_vplt;
    void *libc_res6_vplt;
    void *libc_res7_vplt;

    // sys
    void *sys_epoll_vplt;
    void *sys_select_vplt;
    void *sys_time_vplt;
    void *sys_wait_vplt;
    void *sys_eventfd_vplt;
    void *sys_stat_vplt;
    void *sys_mman_vplt;
    void *sys_utsname_vplt;
    void *sys_shm_vplt;
    void *sys_mount_vplt;
    void *sys_syscall_vplt;
    void *sys_socket_vplt;
    void *sys_ipc_vplt;
    void *sys_syslog_vplt;
    void *sys_random_vplt;
    void *sys_sendfile_vplt;
    void *sys_reboot_vplt;
    void *sys_file_vplt;
    void *sys_prctl_vplt;
    void *sys_event_vplt;
    void *sys_statfs_vplt;
    void *sys_capability_vplt;
    void *sys_timex_vplt;
    void *sys_sysmacros_vplt;
    void *sys_statvfs_vplt;
    void *sys_resource_vplt;
    void *sys_times_vplt;
    void *sys_info_vplt;
    void *sys_sem_vplt;
    void *sys_ioctl_vplt;
    void *sys_res0_vplt;
    void *sys_res1_vplt;
    void *sys_res2_vplt;
    void *sys_res3_vplt;
    void *sys_res4_vplt;
    void *sys_res5_vplt;
    void *sys_res6_vplt;

    // unix
    void *unistd_vplt;
    void *errno_vplt;
    void *signal_vplt;
    void *pthread_vplt;
    void *poll_vplt;
    void *semaphore_vplt;
    void *fcntl_vplt;
    void *dirent_vplt;
    void *spawn_vplt;
    void *termios_vplt;
    void *netdb_vplt;
    void *langinfo_vplt;
    void *syslog_vplt;
    void *sched_vplt;
    void *ifaddrs_vplt;
    void *arpa_inet_vplt;
    void *dlfcn_vplt;
    void *glob_vplt;
    void *mntent_vplt;
    void *pwd_vplt;
    void *findprog_vplt;
    void *fnmatch_vplt;
    void *grp_vplt;
    void *net_if_vplt;
    void *pty_vplt;
    void *unix_res1_vplt;
    void *unix_res2_vplt;
    void *unix_res3_vplt;
    void *unix_res4_vplt;
    void *unix_res5_vplt;
    void *unix_res6_vplt;
    void *unix_res7_vplt;

    // built-in libraries
    void *libusb_vplt;
    void *libgen_vplt;

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