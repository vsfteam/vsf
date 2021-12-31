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


//! \note User Level Application Configuration

#ifndef __VSF_USR_CFG_H__
#define __VSF_USR_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

// arch/hal/os configurations
// enable the 4 configurations below to use simple implementation for x86
//  simple implementation will not use ThreadSuspend and ThreadResume and has better CPU usage
//  **** but preempt is not supported ****
//  priority configurations are dependent on MACROs below, so put them here(at top)
//#define VSF_ARCH_PRI_NUM                                1
//#define VSF_ARCH_SWI_NUM                                0
//#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED
//#define __VSF_X86_WIN_SINGLE_PRIORITY

//#define __WIN7__

#define VSF_SYSTIMER_FREQ                               (0ul)
#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED

// application configuration
#define APP_USE_TGUI_DEMO                               ENABLED
#define APP_USE_TGUI_DESIGNER_DEMO                      DISABLED
#define APP_USE_FREETYPE_DEMO                           ENABLED
#define USRAPP_CFG_STDIO_EN                             ENABLED
#define USRAPP_CFG_FAKEFAT32                            ENABLED

// compiler configurations
#if __IS_COMPILER_LLVM__
#   pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#   pragma clang diagnostic ignored "-Wmicrosoft-include"
#   pragma clang diagnostic ignored "-Winconsistent-dllimport"
#   pragma clang diagnostic ignored "-Wimplicit-function-declaration"
#endif
#ifdef __CPU_X64__
#   error x64 is currently not supported
#endif
#define VSF_ASSERT(...)                                 assert(__VA_ARGS__)
//#define VSF_ASSERT(...)

// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                0x2000000
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   12      // 4K alignment

#define VSF_USE_INPUT                                   ENABLED
#define VSF_USE_UI                                      ENABLED
#define VSF_USE_MAL                                     ENABLED
#   define VSF_MAL_USE_FAKEFAT32_MAL                    ENABLED
#define VSF_USE_FS                                      ENABLED
#   define VSF_FS_USE_MEMFS                             ENABLED
#   define VSF_FS_USE_FATFS                             ENABLED
#define VSF_USE_TRACE                                   ENABLED
#define VSF_USE_FIFO                                    ENABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED

#define VSF_DISP_USE_SDL2                               ENABLED
#   define VSF_DISP_SDL2_CFG_INCLUDE                    "lib\SDL2\include\SDL.h"
#   define VSF_DISP_SDL2_CFG_MOUSE_AS_TOUCHSCREEN       ENABLED
#   ifdef __VSF_X86_WIN_SINGLE_PRIORITY
#       define VSF_DISP_SDL2_CFG_HW_PRIORITY            vsf_arch_prio_0
#   else
#       define VSF_DISP_SDL2_CFG_HW_PRIORITY            vsf_arch_prio_1
#   endif
#   define APP_DISP_SDL2_HEIGHT                         768
#   define APP_DISP_SDL2_WIDTH                          1024
#   define APP_DISP_SDL2_TITLE                          "vsf_screen"
#   define APP_DISP_SDL2_COLOR                          VSF_DISP_COLOR_RGB565
#   define APP_DISP_SDL2_AMPLIFIER                      1

// configuration check
#ifdef __WIN7__
// console for win7 does not support color by default
#   define VSF_TRACE_CFG_COLOR_EN                       DISABLED
#   define VSH_HAS_COLOR                                0
#   define VSF_WINUSB_CFG_WIN7                          ENABLED
#else
#    define VSF_TRACE_CFG_COLOR_EN                      ENABLED
#endif

#if APP_USE_TGUI_DEMO == ENABLED
#   define APP_CFG_TGUI_RES_DIR                         "./winfs_root/ui/"
#endif

/*----------------------------------------------------------------------------*
 * Regarget Weak interface                                                    *
 *----------------------------------------------------------------------------*/

#define WEAK_VSF_KERNEL_ERR_REPORT
#define WEAK___POST_VSF_KERNEL_INIT
#define WEAK_VSF_SYSTIMER_EVTHANDLER
#define WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR
#define WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR
#define WEAK_VSF_DRIVER_INIT
#define WEAK_VSF_HEAP_MALLOC_ALIGNED

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#if APP_USE_TGUI_DEMO == ENABLED
#   include "vsf_tgui_cfg.h"
#endif

#endif
/* EOF */
