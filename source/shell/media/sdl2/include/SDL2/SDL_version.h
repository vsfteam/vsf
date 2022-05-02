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

#ifndef __VSF_SDL2_VERSION_H__
#define __VSF_SDL2_VERSION_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "SDL_stdinc.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define SDL_MAJOR_VERSION                   2
#define SDL_MINOR_VERSION                   0
#define SDL_PATCHLEVEL                      7

#define SDL_VERSIONNUM(__X, __Y, __Z)       ((__X) * 1000 + (__Y) * 100 + (__Z))
#define SDL_COMPILEDVERSION                                                     \
            SDL_VERSIONNUM(SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL)
#define SDL_VERSION_ATLEAST(__X, __Y, __Z)                                      \
            (SDL_COMPILEDVERSION >= SDL_VERSIONNUM(__X, __Y, __Z))

#define SDL_VERSION(x)                                                          \
            do {                                                                \
                (x)->major = SDL_MAJOR_VERSION;                                 \
                (x)->minor = SDL_MINOR_VERSION;                                 \
                (x)->patch = SDL_PATCHLEVEL;                                    \
            } while (0);



#if VSF_SDL_CFG_WRAPPER == ENABLED
#define SDL_Linked_Version                  VSF_SDL_WRAPPER(SDL_Linked_Version)
#define SDL_GetVersion                      VSF_SDL_WRAPPER(SDL_GetVersion)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct SDL_version {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} SDL_version;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern const SDL_version * SDL_Linked_Version(void);
extern void SDL_GetVersion(SDL_version *ver);


#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_VERSION_H__
/* EOF */
