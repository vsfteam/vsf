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

#ifndef __VSF_SDL2_LOG_H__
#define __VSF_SDL2_LOG_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

// for vsf_trace
#include "service/vsf_service.h"
#include "SDL_stdinc.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define SDL_LogMessage(__category, __prio, __fmt, ...)                          \
                                        __SDL_LogMessage(__prio, (__category), (__fmt), ##__VA_ARGS__)
#define SDL_Log(...)                    __SDL_LogMessage(SDL_LOG_PRIORITY_INFO, 0, __VA_ARGS__)
#define SDL_LogInfo(...)                __SDL_LogMessage(SDL_LOG_PRIORITY_INFO, __VA_ARGS__)
#define SDL_LogError(...)               __SDL_LogMessage(SDL_LOG_PRIORITY_ERROR, __VA_ARGS__)
#define SDL_LogCritical(...)            __SDL_LogMessage(SDL_LOG_PRIORITY_ERROR, __VA_ARGS__)
#define SDL_LogDebug(...)               __SDL_LogMessage(SDL_LOG_PRIORITY_DEBUG, __VA_ARGS__)
#define SDL_LogWarn(...)                __SDL_LogMessage(SDL_LOG_PRIORITY_WARN, __VA_ARGS__)
#define SDL_LogVerbose(...)             __SDL_LogMessage(SDL_LOG_PRIORITY_INFO, __VA_ARGS__)

/*============================ TYPES =========================================*/

typedef enum SDL_LogCategory {
    SDL_LOG_CATEGORY_APPLICATION,
    SDL_LOG_CATEGORY_ERROR,
    SDL_LOG_CATEGORY_ASSERT,
    SDL_LOG_CATEGORY_SYSTEM,
    SDL_LOG_CATEGORY_AUDIO,
    SDL_LOG_CATEGORY_VIDEO,
    SDL_LOG_CATEGORY_RENDER,
    SDL_LOG_CATEGORY_INPUT,
    SDL_LOG_CATEGORY_TEST,
} SDL_LogCategory;

typedef enum SDL_LogPriority {
    SDL_LOG_PRIORITY_VERBOSE            = VSF_TRACE_INFO,
    SDL_LOG_PRIORITY_DEBUG              = VSF_TRACE_DEBUG,
    SDL_LOG_PRIORITY_INFO               = VSF_TRACE_INFO,
    SDL_LOG_PRIORITY_WARN               = VSF_TRACE_WARNING,
    SDL_LOG_PRIORITY_ERROR              = VSF_TRACE_ERROR,
    SDL_LOG_PRIORITY_CRITICAL           = VSF_TRACE_ERROR,
} SDL_LogPriority;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void __SDL_LogMessage(SDL_LogPriority priority, int category, const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_LOG_H__
/* EOF */
