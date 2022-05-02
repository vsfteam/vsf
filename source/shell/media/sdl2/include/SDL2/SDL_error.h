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

#ifndef __VSF_SDL2_ERROR_H__
#define __VSF_SDL2_ERROR_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "SDL_stdinc.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SDL_CFG_WRAPPER == ENABLED
#define SDL_Error                       VSF_SDL_WRAPPER(SDL_Error)
#define SDL_GetError                    VSF_SDL_WRAPPER(SDL_GetError)
#define SDL_SetError                    VSF_SDL_WRAPPER(SDL_SetError)
#define SDL_ClearError                  VSF_SDL_WRAPPER(SDL_ClearError)
#endif

#define SDL_OutOfMemory()               SDL_Error(SDL_ENOMEN)

/*============================ TYPES =========================================*/

typedef enum SDL_errorcode {
    SDL_ENOMEN,
    SDL_EFREAD,
    SDL_EFWRITE,
    SDL_EFSEEK,
    SDL_UNSUPPORTED,
    SDL_LASTERROR
} SDL_errorcode;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern int SDL_Error(SDL_errorcode code);
extern const char * SDL_GetError(void);
extern int SDL_SetError(const char *fmt, ...);
extern void SDL_ClearError(void);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_ERROR_H__
/* EOF */
