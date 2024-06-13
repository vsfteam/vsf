/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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

#ifndef __ST_HAL_DEF
#define __ST_HAL_DEF

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include <stddef.h>

/*============================ MACROS ========================================*/

#if !defined(UNUSED)
#    define UNUSED(X) (void)X
#endif

#define HAL_MAX_DELAY 0xFFFFFFFFU

#define HAL_IS_BIT_SET(REG, BIT) (((REG) & (BIT)) == (BIT))
#define HAL_IS_BIT_CLR(REG, BIT) (((REG) & (BIT)) == 0U)

#define __HAL_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = 0U)

#if (USE_RTOS == 1U)
#    error "USE_RTOS should be 0 in the current HAL release"
#else
#    define __HAL_LOCK(__HANDLE__)                                             \
        do {                                                                   \
            if ((__HANDLE__)->Lock == HAL_LOCKED) {                            \
                return HAL_BUSY;                                               \
            } else {                                                           \
                (__HANDLE__)->Lock = HAL_LOCKED;                               \
            }                                                                  \
        } while (0U)

#    define __HAL_UNLOCK(__HANDLE__)                                           \
        do {                                                                   \
            (__HANDLE__)->Lock = HAL_UNLOCKED;                                 \
        } while (0U)
#endif

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#    ifndef __weak
#        define __weak __attribute__((weak))
#    endif
#    ifndef __packed
#        define __packed __attribute__((packed))
#    endif
#elif defined(__GNUC__) && !defined(__CC_ARM)
#    ifndef __weak
#        define __weak __attribute__((weak))
#    endif
#    ifndef __packed
#        define __packed __attribute__((__packed__))
#    endif
#endif

/* Macro to get variable aligned on 4-bytes, for __ICCARM__ the directive
 * "#pragma data_alignment=4" must be used instead */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#    ifndef __ALIGN_BEGIN
#        define __ALIGN_BEGIN
#    endif
#    ifndef __ALIGN_END
#        define __ALIGN_END __attribute__((aligned(4)))
#    endif
#elif defined(__GNUC__) && !defined(__CC_ARM)
#    ifndef __ALIGN_END
#        define __ALIGN_END __attribute__((aligned(4)))
#    endif
#    ifndef __ALIGN_BEGIN
#        define __ALIGN_BEGIN
#    endif
#else
#    ifndef __ALIGN_END
#        define __ALIGN_END
#    endif
#    ifndef __ALIGN_BEGIN
#        if defined(__CC_ARM) /* ARM Compiler V5*/
#            define __ALIGN_BEGIN __align(4)
#        elif defined(__ICCARM__)
#            define __ALIGN_BEGIN
#        endif
#    endif
#endif

#if defined(__CC_ARM) ||                                                       \
    (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
/* ARM Compiler V4/V5 and V6
   --------------------------
   RAM functions are defined using the toolchain options.
   Functions that are executed in RAM should reside in a separate source module.
   Using the 'Options for File' dialog you can simply change the 'Code / Const'
   area of a module to a memory space in physical RAM.
   Available memory areas are declared in the 'Target' tab of the 'Options for
   Target' dialog.
*/
#    define __RAM_FUNC

#elif defined(__ICCARM__)
/* ICCARM Compiler
   ---------------
   RAM functions are defined using a specific toolchain keyword "__ramfunc".
*/
#    define __RAM_FUNC __ramfunc

#elif defined(__GNUC__)
/* GNU Compiler
   ------------
  RAM functions are defined using a specific toolchain attribute
   "__attribute__((section(".RamFunc")))".
*/
#    define __RAM_FUNC __attribute__((section(".RamFunc")))

#endif

#if defined(__CC_ARM) ||                                                       \
    (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) ||              \
    defined(__GNUC__)
/* ARM V4/V5 and V6 & GNU Compiler
   -------------------------------
*/
#    define __NOINLINE __attribute__((noinline))

#elif defined(__ICCARM__)
/* ICCARM Compiler
   ---------------
*/
#    define __NOINLINE _Pragma("optimize = no_inline")

#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    HAL_OK      = 0x00U,
    HAL_ERROR   = 0x01U,
    HAL_BUSY    = 0x02U,
    HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

typedef enum {
    HAL_UNLOCKED = 0x00U,
    HAL_LOCKED   = 0x01U
} HAL_LockTypeDef;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
