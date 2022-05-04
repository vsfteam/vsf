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

#ifndef __VSF_SDL2_AUDIO_H__
#define __VSF_SDL2_AUDIO_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "SDL_stdinc.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define SDL_MIX_MAXVOLUME                   128

#define SDL_AUDIO_ALLOW_FREQUENCY_CHANGE    0x00000001
#define SDL_AUDIO_ALLOW_FORMAT_CHANGE       0x00000002
#define SDL_AUDIO_ALLOW_CHANNELS_CHANGE     0x00000004
#define SDL_AUDIO_ALLOW_SAMPLES_CHANGE      0x00000008
#define SDL_AUDIO_ALLOW_ANY_CHANGE          (SDL_AUDIO_ALLOW_FREQUENCY_CHANGE   \
        | SDL_AUDIO_ALLOW_FORMAT_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE       \
        | SDL_AUDIO_ALLOW_SAMPLES_CHANGE)

#define SDL_AUDIO_MASK_BITSIZE              0xFF
#define SDL_AUDIO_BITSIZE(__FORMAT)         ((__FORMAT) & SDL_AUDIO_MASK_BITSIZE)
#define SDL_AUDIO_MASK_DATATYPE             (1 << 8)
#define SDL_AUDIO_ISFLOAT(__FORMAT)         ((__FORMAT) & SDL_AUDIO_MASK_DATATYPE)
#define SDL_AUDIO_ISINT(__FORMAT)           (!SDL_AUDIO_ISFLOAT(__FORMAT))
#define SDL_AUDIO_MASK_ENDIAN               (1 << 12)
#define SDL_AUDIO_ISBIGENDIAN(__FORMAT)     ((__FORMAT) & SDL_AUDIO_MASK_ENDIAN)
#define SDL_AUDIO_ISLITTLEENDIAN(__FORMAT)  (!SDL_AUDIO_ISBIGENDIAN(__FORMAT))
#define SDL_AUDIO_MASK_SIGNED               (1 << 15)
#define SDL_AUDIO_ISSIGNED(__FORMAT)        ((__FORMAT) & SDL_AUDIO_MASK_SIGNED)
#define SDL_AUDIO_ISUNSIGNED(__FORMAT)      (!SDL_AUDIO_ISSIGNED(__FORMAT))

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SDL_CFG_WRAPPER == ENABLED
#define SDL_OpenAudio                       VSF_SDL_WRAPPER(SDL_OpenAudio)
#define SDL_PauseAudio                      VSF_SDL_WRAPPER(SDL_PauseAudio)
#define SDL_GetAudioStatus                  VSF_SDL_WRAPPER(SDL_GetAudioStatus)
#define SDL_CloseAudio                      VSF_SDL_WRAPPER(SDL_CloseAudio)
#define SDL_LockAudio                       VSF_SDL_WRAPPER(SDL_LockAudio)
#define SDL_UnlockAudio                     VSF_SDL_WRAPPER(SDL_UnlockAudio)
#endif

/*============================ TYPES =========================================*/

typedef enum SDL_AudioStatus {
    SDL_AUDIO_STOPPED = 0,
    SDL_AUDIO_PLAYING,
    SDL_AUDIO_PAUSED,
} SDL_AudioStatus;
typedef uint8_t SDL_AudioFormat;
typedef void (*SDL_AudioCallback)(void * userdata, uint8_t * stream, int len);
enum {
    AUDIO_8                     = 0x0008,
    AUDIO_16                    = 0x0010,
    AUDIO_32                    = 0x0020,

    // LSB is default
    AUDIO_LSB                   = 0x0000,
    AUDIO_MSB                   = 0x4000,
    AUDIO_SIGNED                = 0x8000,
    AUDIO_UNSIGNED              = 0x0000,
    AUDIO_FLOAT                 = 0x0100,

    AUDIO_U8                    = AUDIO_8 | AUDIO_UNSIGNED,
    AUDIO_S8                    = AUDIO_8 | AUDIO_SIGNED,
    AUDIO_U16                   = AUDIO_16 | AUDIO_UNSIGNED,
    AUDIO_S16                   = AUDIO_16 | AUDIO_SIGNED,
    AUDIO_F16                   = AUDIO_16 | AUDIO_FLOAT,
    AUDIO_U32                   = AUDIO_32 | AUDIO_UNSIGNED,
    AUDIO_S32                   = AUDIO_32 | AUDIO_SIGNED,
    AUDIO_F32                   = AUDIO_32 | AUDIO_FLOAT,

    AUDIO_U16LSB                = AUDIO_U16 | AUDIO_LSB,
    AUDIO_U16MSB                = AUDIO_U16 | AUDIO_MSB,
    AUDIO_S16LSB                = AUDIO_S16 | AUDIO_LSB,
    AUDIO_S16MSB                = AUDIO_S16 | AUDIO_MSB,
    AUDIO_U32LSB                = AUDIO_U32 | AUDIO_LSB,
    AUDIO_U32MSB                = AUDIO_U32 | AUDIO_MSB,
    AUDIO_S32LSB                = AUDIO_S32 | AUDIO_LSB,
    AUDIO_S32MSB                = AUDIO_S32 | AUDIO_MSB,
    AUDIO_F32LSB                = AUDIO_F32 | AUDIO_UNSIGNED,
    AUDIO_F32MSB                = AUDIO_F32 | AUDIO_SIGNED,

#if __BYTE_ORDER == __BIG_ENDIAN
    AUDIO_U16SYS                = AUDIO_U16MSB,
    AUDIO_S16SYS                = AUDIO_S16MSB,
    AUDIO_S32SYS                = AUDIO_S32MSB,
    AUDIO_F32SYS                = AUDIO_F32MSB,
#else
    AUDIO_U16SYS                = AUDIO_U16LSB,
    AUDIO_S16SYS                = AUDIO_S16LSB,
    AUDIO_S32SYS                = AUDIO_S32LSB,
    AUDIO_F32SYS                = AUDIO_F32LSB,
#endif
};
typedef struct SDL_AudioSpec {
    uint32_t freq;
    SDL_AudioFormat format;
    uint8_t channels;
    uint8_t silence;
    uint16_t samples;
    uint32_t size;
    SDL_AudioCallback callback;
    void * userdata;
} SDL_AudioSpec;

typedef uintptr_t SDL_AudioDeviceID;

typedef struct SDL_AudioCVT {
    int needed;
    SDL_AudioFormat src_format;
    SDL_AudioFormat dst_format;
    uint8_t *buf;
    int len;
    int len_cvt;
    int len_mult;
} SDL_AudioCVT;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern int SDL_OpenAudio(SDL_AudioSpec * desired, SDL_AudioSpec * obtained);
extern void SDL_PauseAudio(int pause_on);
extern SDL_AudioStatus SDL_GetAudioStatus(void);
extern void SDL_CloseAudio(void);
extern void SDL_LockAudio(void);
extern void SDL_UnlockAudio(void);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_AUDIO_H__
/* EOF */
