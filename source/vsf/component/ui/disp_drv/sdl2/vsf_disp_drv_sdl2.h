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
#ifndef __VSF_DISP_DRV_SDL2_H___
#define __VSF_DISP_DRV_SDL2_H___

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_DISP_DRV_SDL2 == ENABLED

#include "hal/vsf_hal.h"
#include "SDL.h"

#if     defined(__VSF_DISP_SDL2_CLASS_IMPLEMENT)
#   undef __VSF_DISP_SDL2_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(__VSF_DISP_SDL2_CLASS_INHERIT)
#   undef __VSF_DISP_SDL2_CLASS_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if !defined(__CPU_X86__) && !defined(__CPU_X64__)
#   error "libusb_hcd ONLY support x86/64"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_disp_sdl2_t)

def_simple_class(vk_disp_sdl2_t) {
    implement(vk_disp_t);

    public_member(
        uint8_t                     amplifier;
        uint8_t                     flush_delay_ms;
    )

    private_member(
        vsf_arch_irq_thread_t       event_thread;
        vsf_arch_irq_thread_t       flush_thread;
        vsf_arch_irq_request_t      flush_request;
        vk_disp_area_t              area;
        void                        *disp_buff;

        SDL_Window                  *window;
        SDL_Renderer                *renderer;
        SDL_Texture                 *texture;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_disp_drv_t vk_disp_drv_sdl2;

/*============================ PROTOTYPES ====================================*/

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_DRV_SDL2_H___
