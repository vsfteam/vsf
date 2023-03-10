/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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


//! \note User Level Application Configuration

#ifndef __VSF_USR_CFG_H__
#define __VSF_USR_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define VSF_SYSTIMER_FREQ                               (0ul)

#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED

// kernel
#define VSF_OS_CFG_PRIORITY_NUM                         1
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED

// services and components
#define VSF_USE_TRACE                                   ENABLED
#define VSF_USE_FIFO                                    ENABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED
#define VSF_USE_HEAP                                    ENABLED

#define VSF_USE_MAL                                     ENABLED
#   define VSF_MAL_USE_MEM_MAL                          ENABLED
#   define VSF_MAL_USE_FILE_MAL                         ENABLED
#define VSF_USE_FS                                      ENABLED
#   define VSF_FS_USE_MEMFS                             ENABLED
#   define VSF_FS_USE_FATFS                             ENABLED

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_KERNEL_CFG_THREAD_STACK_LARGE            ENABLED
#   define VSF_LINUX_CFG_STACKSIZE                      (4 * 1024)
#   define VSF_LINUX_USE_SIMPLE_LIBC                    ENABLED
#       define VSF_LINUX_LIBC_CFG_CPP                   ENABLED
#       define VSF_LINUX_USE_SIMPLE_STDIO               ENABLED
#       define VSF_LINUX_USE_SIMPLE_STRING              ENABLED
#       define VSF_LINUX_USE_SIMPLE_TIME                ENABLED
#       define VSF_LINUX_USE_SIMPLE_STDLIB              ENABLED
#           define VSF_LINUX_SIMPLE_STDLIB_USE_STRTOXX  ENABLED
#       define VSF_LINUX_USE_SIMPLE_CTYPE               ENABLED
#       define VSF_USE_SIMPLE_SSCANF                    ENABLED
#       define VSF_USE_SIMPLE_SPRINTF                   ENABLED
#           define VSF_SIMPLE_SPRINTF_SUPPORT_FLOAT     ENABLED
#       define VSF_USE_SIMPLE_SWPRINTF                  ENABLED
#   define VSF_USE_POSIX                                ENABLED
#   define VSF_LINUX_USE_BUSYBOX                        ENABLED
#   define VSF_LINUX_USE_SOCKET                         ENABLED
#       define VSF_LINUX_SOCKET_USE_UNIX                ENABLED
#       define VSF_LINUX_SOCKET_USE_INET                VSF_USE_TCPIP
#   define VSF_LINUX_USE_DEVFS                          ENABLED
#       define VSF_LINUX_DEVFS_USE_RAND                 ENABLED
#   define VSF_LINUX_CFG_FD_BITMAP_SIZE                 256
#   define VSF_LINUX_USE_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_LINUX_CFG_PLS_NUM                        16
#   define VSF_LINUX_CFG_TLS_NUM                        64
#   if VSF_USE_LOADER == ENABLED
#       define VSF_LINUX_USE_APPLET                     ENABLED
#           define VSF_LINUX_APPLET_USE_LIBC            ENABLED
#           define VSF_LINUX_APPLET_USE_UNISTD          ENABLED
#           define VSF_LINUX_APPLET_USE_PTHREAD         ENABLED
#           define VSF_LINUX_APPLET_USE_LIBUSB          ENABLED
#           define VSF_LINUX_APPLET_USE_SEMAPHORE       ENABLED
#           define VSF_LINUX_APPLET_USE_POLL            ENABLED
#           define VSF_LINUX_APPLET_USE_NETDB           ENABLED
#           define VSF_LINUX_APPLET_USE_LIBGEN          ENABLED
#           define VSF_LINUX_APPLET_USE_IFADDRS         ENABLED
#           define VSF_LINUX_APPLET_USE_LIBGETOPT       ENABLED
#           define VSF_LINUX_APPLET_USE_FCNTL           ENABLED
#           define VSF_LINUX_APPLET_USE_DIRENT          ENABLED
#           define VSF_LINUX_APPLET_USE_SYS_EPOLL       ENABLED
#           define VSF_LINUX_APPLET_USE_SYS_EVENTFD     ENABLED
#           define VSF_LINUX_APPLET_USE_SYS_RANDOM      ENABLED
#           define VSF_LINUX_APPLET_USE_SYS_SELECT      ENABLED
#           define VSF_LINUX_APPLET_USE_SYS_SHM         ENABLED
#           define VSF_LINUX_APPLET_USE_SYS_STAT        ENABLED
#           define VSF_LINUX_APPLET_USE_SYS_TIME        ENABLED
#           define VSF_LINUX_APPLET_USE_SYS_UTSNAME     ENABLED
#           define VSF_LINUX_APPLET_USE_SYS_WAIT        ENABLED
#           define VSF_LINUX_APPLET_USE_SYS_SOCKET      ENABLED
#   endif

// APP configuration
#define APP_USE_LINUX_DEMO                              ENABLED

// linux init scripts
#define VSF_LINUX_CFG_INIT_SCRIPTS                                              \
            "echo \"vsf build on " __DATE__ "\"",

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#endif
/* EOF */
