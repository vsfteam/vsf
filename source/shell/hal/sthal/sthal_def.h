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

#define VSF_STHAL_LOCK(__HANDLE__)                                             \
    do {                                                                       \
        if ((__HANDLE__)->Lock == HAL_LOCKED) {                                \
            return HAL_BUSY;                                                   \
        } else {                                                               \
            (__HANDLE__)->Lock = HAL_LOCKED;                                   \
        }                                                                      \
    } while (0U)

#define VSF_STHAL_UNLOCK(__HANDLE__)                                           \
    do {                                                                       \
        (__HANDLE__)->Lock = HAL_UNLOCKED;                                     \
    } while (0U)

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   ifndef VSF_STHAL_ASSERT
#      define VSF_STHAL_ASSERT(__CON) VSF_ASSERT(__CON)
#   endif
#else
#   ifndef VSF_STHAL_ASSERT
#      define VSF_STHAL_ASSERT(...) VSF_ASSERT(__VA_ARGS__)
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    HAL_OK      = 0x00U,
    HAL_ERROR   = 0x01U,
    HAL_BUSY    = 0x02U,
    HAL_TIMEOUT = 0x03U,
} HAL_StatusTypeDef;

typedef enum {
    HAL_UNLOCKED = 0x00U,
    HAL_LOCKED   = 0x01U,
} HAL_LockTypeDef;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
