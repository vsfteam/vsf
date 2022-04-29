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

#ifndef __VSF_SDL2_TRHEAD_H__
#define __VSF_SDL2_TRHEAD_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED && VSF_USE_LINUX == ENABLED

#include "kernel/vsf_kernel.h"
#include "SDL_stdinc.h"

#include "begin_code.h"

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef int (*SDL_ThreadFunction)(void *data);

typedef vsf_prio_t SDL_ThreadPriority;
typedef pthread_t SDL_threadID;
typedef int SDL_TLSID;
typedef struct SDL_Thread {
    SDL_threadID thread;
    SDL_ThreadFunction entry;
    void *data;
    char name[sizeof(uintptr_t)];
} SDL_Thread;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern SDL_Thread * SDL_CreateThread(SDL_ThreadFunction fn, const char *name, void *data);
extern SDL_Thread * SDL_CreateThreadWithStackSize(SDL_ThreadFunction fn, const char *name, const size_t stacksize, void *data);
extern const char * SDL_GetThreadName(SDL_Thread *thread);
extern SDL_threadID SDL_ThreadID(void);
extern SDL_threadID SDL_GetThreadID(SDL_Thread * thread);
extern int SDL_SetThreadPriority(SDL_ThreadPriority priority);
extern void SDL_WaitThread(SDL_Thread * thread, int *status);
extern void SDL_DetachThread(SDL_Thread * thread);
extern SDL_TLSID SDL_TLSCreate(void);
extern void * SDL_TLSGet(SDL_TLSID id);
extern int SDL_TLSSet(SDL_TLSID id, const void *value, void (*destructor)(void*));
extern void SDL_TLSCleanup(void);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_TRHEAD_H__
/* EOF */
