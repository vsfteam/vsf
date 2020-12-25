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

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if VSF_USE_SDL2 == ENABLED && APP_USE_SDL2_DEMO == ENABLED

#include <SDL.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if APP_USE_LINUX_DEMO == ENABLED
int sdl2_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

    // SDL application
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *win = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

#if defined(APP_CFG_SDL2_DEMO_COLOR_RGB565)
    SDL_Texture *texture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STATIC, 34, 34);

    uint16_t block[34 * 34];
    for (int i = 0; i < sizeof(block) / sizeof(block[0]); i++) {
        block[i] = 0xFFFF;
    }
    SDL_UpdateTexture(texture, &((SDL_Rect){
            .x = 0,
            .y = 0,
            .w = 34,
            .h = 34,
        }),
        block, sizeof(block[0]) * 34);
#elif defined(APP_CFG_SDL2_DEMO_COLOR_RGB666)
    SDL_Texture *texture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_RGB666, SDL_TEXTUREACCESS_STATIC, 34, 34);

    uint32_t block[34 * 34];
    for (int i = 0; i < sizeof(block) / sizeof(block[0]); i++) {
        block[i] = 0x3FFFF;
    }
    SDL_UpdateTexture(texture, &((SDL_Rect){
            .x = 0,
            .y = 0,
            .w = 34,
            .h = 34,
        }),
        block, sizeof(block[0]) * 34);
#endif

    uint8_t x = 0, y = 0;
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    srand(vsf_systimer_get_tick());
#else
    srand(0);
#endif
    while (1) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, &((SDL_Rect){
                .x = x,
                .y = y,
                .w = 34,
                .h = 34,
            }));
        x += rand();
        y += rand();
        SDL_RenderPresent(renderer);
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        vsf_teda_set_timer_ms(1000);
        vsf_thread_wfe(VSF_EVT_TIMER);
#endif
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

#endif
