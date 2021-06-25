/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#ifndef __VSF_SDL2_RWOPS_H__
#define __VSF_SDL2_RWOPS_H__

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
#define SDL_RWFromFile                  VSF_SDL_WRAPPER(SDL_RWFromFile)
#define SDL_RWsize                      VSF_SDL_WRAPPER(SDL_RWsize)
#define SDL_RWclose                     VSF_SDL_WRAPPER(SDL_RWclose)
#define SDL_RWseek                      VSF_SDL_WRAPPER(SDL_RWseek)
#define SDL_RWtell                      VSF_SDL_WRAPPER(SDL_RWtell)
#define SDL_RWread                      VSF_SDL_WRAPPER(SDL_RWread)
#define SDL_RWwrite                     VSF_SDL_WRAPPER(SDL_RWwrite)
#endif

/*============================ TYPES =========================================*/

enum {
    RW_SEEK_SET                         = SEEK_SET,
    RW_SEEK_CUR                         = SEEK_CUR,
    RW_SEEK_END                         = SEEK_END,
};
typedef FILE SDL_RWops;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern SDL_RWops * SDL_RWFromFile(const char * file, const char * mode);
extern int64_t SDL_RWsize(SDL_RWops * context);
extern int SDL_RWclose(SDL_RWops * context);
extern int64_t SDL_RWseek(SDL_RWops * context, int64_t offset, int whence);
extern int64_t SDL_RWtell(SDL_RWops * context);
extern size_t SDL_RWread(SDL_RWops * context, void * ptr, size_t size, size_t maxnum);
extern size_t SDL_RWwrite(SDL_RWops * context, const void * ptr, size_t size, size_t num);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_RWOPS_H__
/* EOF */
