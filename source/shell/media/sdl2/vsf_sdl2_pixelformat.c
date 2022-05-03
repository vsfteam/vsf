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

#if VSF_USE_SDL2 == ENABLED

#include "./include/SDL2/SDL.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct {
    uint32_t color;
    SDL_PixelFormat format;
} static const __vsf_sdl2_color[] = {
    {
        .color              = SDL_PIXELFORMAT_PALETTE,
        .format             = {
            .format         = SDL_PIXELFORMAT_PALETTE,
            .BitsPerPixel   = 8,
            .BytesPerPixel  = 1,
        },
    },
    {
        .color              = SDL_PIXELFORMAT_ARGB8888,
        .format             = {
            .format         = SDL_PIXELFORMAT_ARGB8888,
            .BitsPerPixel   = 32,
            .BytesPerPixel  = 4,
            .Rmask          = 0x00FF0000,
            .Gmask          = 0x0000FF00,
            .Bmask          = 0x000000FF,
            .Amask          = 0xFF000000,
            .Rshift         = 16,
            .Gshift         = 8,
            .Bshift         = 0,
            .Ashift         = 24,
            .Rloss          = 0,
            .Gloss          = 0,
            .Bloss          = 0,
            .Aloss          = 0,
        },
    },
    {
        .color              = SDL_PIXELFORMAT_ABGR8888,
        .format             = {
            .format         = SDL_PIXELFORMAT_ABGR8888,
            .BitsPerPixel   = 32,
            .BytesPerPixel  = 4,
            .Rmask          = 0x000000FF,
            .Gmask          = 0x0000FF00,
            .Bmask          = 0x00FF0000,
            .Amask          = 0xFF000000,
            .Rshift         = 16,
            .Gshift         = 8,
            .Bshift         = 0,
            .Ashift         = 24,
            .Rloss          = 0,
            .Gloss          = 0,
            .Bloss          = 0,
            .Aloss          = 0,
        },
    },
    {
        .color              = SDL_PIXELFORMAT_RGBA8888,
        .format             = {
            .format         = SDL_PIXELFORMAT_RGBA8888,
            .BitsPerPixel   = 32,
            .BytesPerPixel  = 4,
            .Rmask          = 0xFF000000,
            .Gmask          = 0x00FF0000,
            .Bmask          = 0x0000FF00,
            .Amask          = 0x000000FF,
            .Rshift         = 24,
            .Gshift         = 16,
            .Bshift         = 8,
            .Ashift         = 0,
            .Rloss          = 0,
            .Gloss          = 0,
            .Bloss          = 0,
            .Aloss          = 0,
        },
    },
    {
        .color              = SDL_PIXELFORMAT_RGB565,
        .format             = {
            .format         = SDL_PIXELFORMAT_RGB565,
            .BitsPerPixel   = 16,
            .BytesPerPixel  = 2,
            .Rmask          = 0xF800,
            .Gmask          = 0x07E0,
            .Bmask          = 0x001F,
            .Rshift         = 11,
            .Gshift         = 5,
            .Bshift         = 0,
            .Rloss          = 3,
            .Gloss          = 2,
            .Bloss          = 3,
        },
    },
    {
        .color              = SDL_PIXELFORMAT_RGB888,
        .format             = {
            .format         = SDL_PIXELFORMAT_RGB888,
            .BitsPerPixel   = 32,
            .BytesPerPixel  = 4,
            .Rmask          = 0x00FF0000,
            .Gmask          = 0x0000FF00,
            .Bmask          = 0x000000FF,
            .Rshift         = 16,
            .Gshift         = 8,
            .Bshift         = 0,
            .Rloss          = 0,
            .Gloss          = 0,
            .Bloss          = 0,
        },
    },
    {
        .color              = SDL_PIXELFORMAT_RGB24,
        .format             = {
            .format         = SDL_PIXELFORMAT_RGB24,
            .BitsPerPixel   = 24,
            .BytesPerPixel  = 3,
            .Rmask          = 0x00FF0000,
            .Gmask          = 0x0000FF00,
            .Bmask          = 0x000000FF,
            .Rshift         = 16,
            .Gshift         = 8,
            .Bshift         = 0,
            .Rloss          = 0,
            .Gloss          = 0,
            .Bloss          = 0,
        },
    },
    {
        .color              = SDL_PIXELFORMAT_RGB332,
        .format             = {
            .format         = SDL_PIXELFORMAT_RGB332,
            .BitsPerPixel   = 8,
            .BytesPerPixel  = 1,
            .Rmask          = 0xE0,
            .Gmask          = 0x1C,
            .Bmask          = 0x3,
            .Rshift         = 5,
            .Gshift         = 2,
            .Bshift         = 0,
            .Rloss          = 5,
            .Gloss          = 5,
            .Bloss          = 6,
        },
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

uint32_t __SDL_GetColorFromMask(uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
    for (uint_fast16_t i = 0; i < dimof(__vsf_sdl2_color); i++) {
        if (    __vsf_sdl2_color[i].format.Rmask == Rmask
            &&  __vsf_sdl2_color[i].format.Gmask == Gmask
            &&  __vsf_sdl2_color[i].format.Bmask == Bmask
            &&  __vsf_sdl2_color[i].format.Amask == Amask) {
            return __vsf_sdl2_color[i].color;
        }
    }

    int_fast8_t pixel_bitlen = vsf_msb32(Rmask | Gmask | Bmask | Amask);
    if (pixel_bitlen < 0) {
        return SDL_PIXELFORMAT_UNKNOWN;
    }

    uint_fast8_t pixel_bytelen = (++pixel_bitlen + 7) >> 3;
    return VSF_DISP_COLOR_VALUE(SDL_PIXELFORMAT_BYMASK_IDX, pixel_bitlen, pixel_bytelen, Amask != 0);
}

const SDL_PixelFormat * __SDL_GetFormatFromColor(uint32_t color)
{
    for (uint_fast16_t i = 0; i < dimof(__vsf_sdl2_color); i++) {
        if (__vsf_sdl2_color[i].color == color) {
            return &__vsf_sdl2_color[i].format;
        }
    }
    return NULL;
}

void __SDL_InitFormatMask(SDL_PixelFormat *format, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
    uint_fast8_t tmp8;
    format->Rmask = Rmask;
    format->Gmask = Gmask;
    format->Bmask = Bmask;
    format->Amask = Amask;

    tmp8 = vsf_clz32(Rmask);
    Rmask <<= tmp8;
    format->Rloss = vsf_clz32(~Rmask);
    format->Rshift = 32 - (tmp8 + format->Rloss);
    format->Rloss = 8 - format->Rloss;

    tmp8 = vsf_clz32(Gmask);
    Gmask <<= tmp8;
    format->Gloss = vsf_clz32(~Gmask);
    format->Gshift = 32 - (tmp8 + format->Gloss);
    format->Gloss = 8 - format->Gloss;

    tmp8 = vsf_clz32(Bmask);
    Bmask <<= tmp8;
    format->Bloss = vsf_clz32(~Bmask);
    format->Bshift = 32 - (tmp8 + format->Bloss);
    format->Bloss = 8 - format->Bloss;

    tmp8 = vsf_clz32(Amask);
    Amask <<= tmp8;
    format->Aloss = vsf_clz32(~Amask);
    format->Ashift = 32 - (tmp8 + format->Aloss);
    format->Aloss = 8 - format->Aloss;
}

#endif      // VSF_USE_SDL2
