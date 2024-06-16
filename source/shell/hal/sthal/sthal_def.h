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
#include <stdint.h>

/*============================ MACROS ========================================*/

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
#elif defined(__GNUC__) && !defined(__CC_ARM)
#    ifndef __weak
#        define __weak __attribute__((weak))
#    endif
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
