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

#include "./vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED && VSF_USE_LINUX == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#include "./include/SDL2/SDL.h"
#include <unistd.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static void * __SDL_Thread_Entry(void *param)
{
    SDL_Thread *thread = param;
    int ret = thread->entry(thread->data);
    free(thread);
    return (void *)ret;
}

SDL_Thread * SDL_CreateThreadWithStackSize(SDL_ThreadFunction fn, const char *name, const size_t stacksize, void *data)
{
    size_t namelen = strlen(name) + 1;
    SDL_Thread *thread;

    if (namelen < sizeof(thread->name)) {
        namelen = 0;
    } else {
        namelen = namelen - sizeof(thread->name);
    }
    thread = malloc(sizeof(SDL_Thread) + namelen);
    if (thread != NULL) {
        const pthread_attr_t attr = {
            .stacksize = stacksize,
        };
        int ret = pthread_create(&thread->thread, &attr, __SDL_Thread_Entry, thread);
        if (ret != 0) {
            free(thread);
            thread = NULL;
        } else {
            thread->entry = fn;
            thread->data = data;
            strcpy(thread->name, name);
        }
    }
    return thread;
}

SDL_Thread * SDL_CreateThread(SDL_ThreadFunction fn, const char *name, void *data)
{
    return SDL_CreateThreadWithStackSize(fn, name, 0, data);
}

const char * SDL_GetThreadName(SDL_Thread *thread)
{
    return thread->name;
}

SDL_threadID SDL_ThreadID(void)
{
    return pthread_self();
}

SDL_threadID SDL_GetThreadID(SDL_Thread * thread)
{
    return thread->thread;
}

int SDL_SetThreadPriority(SDL_ThreadPriority priority)
{
    vsf_thread_set_priority(priority);
    return 0;
}

void SDL_WaitThread(SDL_Thread * thread, int *status)
{
    pthread_join(thread->thread, (void **)&status);
}

void SDL_DetachThread(SDL_Thread * thread)
{
    pthread_detach(thread->thread);
}

SDL_TLSID SDL_TLSCreate(void)
{
    return vsf_linux_tls_alloc(NULL);
}

void * SDL_TLSGet(SDL_TLSID id)
{
    return vsf_linux_tls_get(id);
}

int SDL_TLSSet(SDL_TLSID id, const void *value, void (*destructor)(void*))
{
    vsf_linux_localstorage_t *tls = vsf_linux_tls_get(id);
    if (tls != NULL) {
        tls->data = (void *)value;
        tls->destructor = destructor;
        return 0;
    }
    return -1;
}

void SDL_TLSCleanup(void)
{
    // tls will be freed on thread exit automatically,
    //  so do nothing here
}

#endif      // VSF_USE_SDL2
