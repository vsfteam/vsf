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
#ifndef __VSF_DISP_SDL2_H__
#define __VSF_DISP_SDL2_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_SDL2 == ENABLED

#include "hal/vsf_hal.h"

#if     defined(__VSF_DISP_SDL2_CLASS_IMPLEMENT)
#   undef __VSF_DISP_SDL2_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if !defined(__CPU_X86__) && !defined(__CPU_X64__)
#   error "vsf_disp_sdl2 ONLY support x86/64"
#endif

// if VSF_DISP_USE_SDL2 is used, SDL header files are not visible in vsf header files
// so define main to SDL_main for SDL
#if     defined(__WIN__)
#   define main                     SDL_main
#elif   defined(__LINUX__)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_disp_sdl2_t)

def_simple_class(vk_disp_sdl2_t) {
    public_member(
        implement(vk_disp_t)
        char                        * title;
        uint8_t                     amplifier;
        uint8_t                     flush_delay_ms;
    )

    private_member(
        vsf_arch_irq_thread_t       event_thread;
        vsf_arch_irq_thread_t       flush_thread;
        vsf_arch_irq_request_t      flush_request;
        vk_disp_area_t              area;
        void                        *disp_buff;

#if 0
        // SDL_XXXX not visible in header files
        SDL_Window                  *window;
        SDL_Renderer                *renderer;
        SDL_Texture                 *texture;
#else
        void                        *window;
        void                        *renderer;
        void                        *texture;
#endif
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_disp_drv_t vk_disp_drv_sdl2;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_SDL2_H__
