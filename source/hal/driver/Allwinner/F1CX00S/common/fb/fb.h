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

#ifndef __HAL_DRIVER_ALLWINNER_F1CX00S_FRAMEBUFFER_H__
#define __HAL_DRIVER_ALLWINNER_F1CX00S_FRAMEBUFFER_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"
#include "utilities/vsf_utilities.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct f1cx00s_fb_rgb_timing_t {
    uint32_t                    pixel_clock_hz;
    uint16_t                    h_front_porch;
    uint16_t                    h_back_porch;
    uint16_t                    h_sync;
    uint16_t                    v_front_porch;
    uint16_t                    v_back_porch;
    uint16_t                    v_sync;
    bool                        h_sync_active;
    bool                        v_sync_active;
    bool                        den_active;
    bool                        clk_active;
} f1cx00s_fb_rgb_timing_t;

typedef struct f1cx00s_fb_cpu_timing_t {
    uint32_t                    pixel_clock_hz;
} f1cx00s_fb_cpu_timing_t;

typedef struct f1cx00s_fb_tve_timing_t {
    uint32_t                    pixel_clock_hz;
} f1cx00s_fb_tve_timing_t;

typedef struct f1cx00s_fb_param_t {
    enum {
        F1C100S_LCD_TYPE_RGB,
        F1C100S_LCD_TYPE_CPU,
        F1C100S_LCD_TYPE_TV_OUT,
    } lcd_type;
    // refer to DEBE_LAY_ATT_CTRL1 register
    uint16_t                    color_format;
    union {
        f1cx00s_fb_rgb_timing_t rgb;
        f1cx00s_fb_cpu_timing_t cpu;
        f1cx00s_fb_tve_timing_t tv_out;
    } timing;
} f1cx00s_fb_param_t;

typedef struct f1cx00s_fb_t {
    const f1cx00s_fb_param_t *  param;
    uint32_t                    width;
    uint32_t                    height;
    uint8_t                     pixel_bit_size;
    uint8_t                     pixel_byte_size;
} f1cx00s_fb_t;

// TODO: move to interface in hal, lv0 interface
def_interface(i_fb_t)
    vsf_err_t (*Init)(void *fb, void *initial_pixel_buffer);
    vsf_err_t (*Fini)(void *fb);
    vsf_err_t (*Present)(void *fb, void *pixel_buffer);
end_def_interface(i_fb_t)

/*============================ GLOBAL VARIABLES ==============================*/

extern const i_fb_t VSF_FB;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t f1cx00s_fb_init(f1cx00s_fb_t *fb, void *initial_pixel_buffer);
extern vsf_err_t f1cx00s_fb_fini(f1cx00s_fb_t *fb);
extern vsf_err_t f1cx00s_fb_present(f1cx00s_fb_t *fb, void *pixel_buffer);

#endif
/* EOF */
