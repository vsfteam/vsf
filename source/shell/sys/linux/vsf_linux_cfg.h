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
#       define VSF_LINUX_CFG_PRIO_SIGNAL        VSF_LINUX_CFG_PRIO_HIGHEST
#   endif
#endif

#ifndef VSF_LINUX_CFG_HOSTNAME
#   define VSF_LINUX_CFG_HOSTNAME               "vsf"
#endif

#ifndef VSF_LINUX_CFG_BIN_PATH
#   define VSF_LINUX_CFG_BIN_PATH               "/bin"
#endif
#ifndef VSF_LINUX_USE_TERMIOS
#   define VSF_LINUX_USE_TERMIOS                ENABLED
#endif
#if VSF_LINUX_USE_TERMIOS == ENABLED
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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */