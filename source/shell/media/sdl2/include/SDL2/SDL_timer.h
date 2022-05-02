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

#ifndef __VSF_SDL2_TIMER_H__
#define __VSF_SDL2_TIMER_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "SDL_stdinc.h"

#include "kernel/vsf_kernel.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER != ENABLED
#   error SDL_timer depends on VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SDL_CFG_WRAPPER == ENABLED
#define SDL_AddTimer                    VSF_SDL_WRAPPER(SDL_AddTimer)
#define SDL_RemoveTimer                 VSF_SDL_WRAPPER(SDL_RemoveTimer)
#define SDL_GetTicks                    VSF_SDL_WRAPPER(SDL_GetTicks)
#define SDL_GetTicks64                  VSF_SDL_WRAPPER(SDL_GetTicks64)
#define SDL_GetPerformanceCounter       VSF_SDL_WRAPPER(SDL_GetPerformanceCounter)
#define SDL_GetPerformanceFrequency     VSF_SDL_WRAPPER(SDL_GetPerformanceFrequency)
#define SDL_Delay                       VSF_SDL_WRAPPER(SDL_Delay)
#endif

#define SDL_TICKS_PASSED(__A, __B)      ((int32_t)((__B) - (__A)) <= 0)

/*============================ TYPES =========================================*/

typedef uintptr_t SDL_TimerID;
typedef uint32_t (*SDL_TimerCallback)(uint32_t interval, void *param);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_TimerCallback callback, void *param);
extern SDL_bool SDL_RemoveTimer(SDL_TimerID id);
extern uint32_t SDL_GetTicks(void);
extern uint64_t SDL_GetTicks64(void);
extern uint64_t SDL_GetPerformanceCounter(void);
extern uint64_t SDL_GetPerformanceFrequency(void);
extern void SDL_Delay(uint32_t ms);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_TIMER_H__
/* EOF */
