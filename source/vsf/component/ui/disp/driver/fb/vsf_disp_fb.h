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
#ifndef __VSF_DISP_FRAME_BUFFER_H__
#define __VSF_DISP_FRAME_BUFFER_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_FB == ENABLED

#include "kernel/vsf_kernel.h"
#include "hal/vsf_hal.h"

#if     defined(__VSF_DISP_FB_CLASS_IMPLEMENT)
#   undef __VSF_DISP_FB_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_disp_fb_t)

def_simple_class(vk_disp_fb_t) {
    public_member(
        implement(vk_disp_t)

        struct {
            // user can provide the frame buffer here
            //  if NULL, frame buffer will be allocated from heap
            // disp_fb will provide the buffer as current user buffer
            void            *buffer;

            const i_fb_t    *drv;
            void            *param;

            uint32_t        size;
            uint8_t         num;
            uint8_t         pixel_byte_size;
        } fb;
    )

    private_member(
        void                *fb_buffer;
        uint8_t             cur_fb_buffer;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_disp_drv_t vk_disp_drv_fb;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI && VSF_DISP_USE_FB
#endif  // __VSF_DISP_FRAME_BUFFER_H__
