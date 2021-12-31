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

#ifndef __VSF_SDL2_ENDIAN_H__
#define __VSF_SDL2_ENDIAN_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "hal/arch/vsf_arch.h"
#include "SDL_stdinc.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define SDL_BYTEORDER                   __BYTE_ORDER
#define SDL_LIL_ENDIAN                  __LITTLE_ENDIAN
#define SDL_BIG_ENDIAN                  __BIG_ENDIAN




#define SDL_Swap16                      bswap_16
#define SDL_Swap32                      bswap_32
#define SDL_Swap64                      bswap_64

#define SDL_SwapLE16                    cpu_to_le16
#define SDL_SwapLE32                    cpu_to_le32
#define SDL_SwapLE64                    cpu_to_le64
#define SDL_SwapBE16                    cpu_to_be16
#define SDL_SwapBE32                    cpu_to_be32
#define SDL_SwapBE64                    cpu_to_be64

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_ENDIAN_H__
/* EOF */
