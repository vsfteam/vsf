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

#ifndef __VSF_SDL2_MUTEX_H__
#define __VSF_SDL2_MUTEX_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "kernel/vsf_kernel.h"
#include "SDL_stdinc.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SDL_CFG_WRAPPER == ENABLED
#define SDL_CreateMutex                 VSF_SDL_WRAPPER(SDL_CreateMutex)
#define SDL_DestroyMutex                VSF_SDL_WRAPPER(SDL_DestroyMutex)
#define SDL_TryLockMutex                VSF_SDL_WRAPPER(SDL_TryLockMutex)
#define SDL_LockMutex                   VSF_SDL_WRAPPER(SDL_LockMutex)
#define SDL_UnlockMutex                 VSF_SDL_WRAPPER(SDL_UnlockMutex)
#endif

/*============================ TYPES =========================================*/

typedef vsf_mutex_t SDL_mutex;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern SDL_mutex* SDL_CreateMutex(void);
extern void SDL_DestroyMutex(SDL_mutex * mutex);
extern int SDL_TryLockMutex(SDL_mutex * mutex);
extern int SDL_LockMutex(SDL_mutex * mutex);
extern int SDL_UnlockMutex(SDL_mutex * mutex);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_MUTEX_H__
/* EOF */
