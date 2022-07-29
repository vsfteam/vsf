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

#include "./usrapp_ui_common.h"

#if     VSF_USE_UI == ENABLED                                                   \
    &&  (   VSF_DISP_USE_SDL2 == ENABLED || VSF_DISP_USE_FB == ENABLED          \
        ||  VSF_DISP_USE_DL1X5 == ENABLED || VSF_DISP_USE_MIPI_LCD == ENABLED   \
        ||  VSF_DISP_USE_WINGDI == ENABLED || VSF_DISP_USE_USBD_UVC == ENABLED)

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

#if VSF_DISP_USE_SDL2 == ENABLED
#   ifndef APP_DISP_SDL2_TITLE
#       define APP_DISP_SDL2_TITLE              "title"
#   endif
#   ifndef APP_DISP_SDL2_AMPLIFIER
#       define APP_DISP_SDL2_AMPLIFIER          1
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_DISP_USE_FB == ENABLED
#   ifdef __F1C100S__
static const f1cx00s_fb_param_t __fb_param = {
    // TODO: fix according to hw
    .lcd_type                   = F1C100S_LCD_TYPE_RGB,
    .color_format               = 9 << 8,        // color 32-bpp(Padding:8/R:8/G:8/B:8)
    .timing.rgb                 = {
        .pixel_clock_hz         = APP_DISP_FB_CFG_PIXEL_CLOCK,
        .h_front_porch          = APP_DISP_FB_CFG_H_FP,
        .h_back_porch           = APP_DISP_FB_CFG_H_BP,
        .h_sync                 = APP_DISP_FB_CFG_H_SYNC,
        .v_front_porch          = APP_DISP_FB_CFG_V_FP,
        .v_back_porch           = APP_DISP_FB_CFG_V_BP,
        .v_sync                 = APP_DISP_FB_CFG_V_SYNC,
        .h_sync_active          = false,
        .v_sync_active          = false,
        .den_active             = true,
        .clk_active             = true,
    },
};
static f1cx00s_fb_t __fb = {
    .param                      = &__fb_param,
    .width                      = APP_DISP_FB_WIDTH,
    .height                     = APP_DISP_FB_HEIGHT,
    .pixel_bit_size             = vsf_disp_get_pixel_format_bitsize(APP_DISP_FB_COLOR),
    .pixel_byte_size            = vsf_disp_get_pixel_format_bytesize(APP_DISP_FB_COLOR),
};
#   define APP_DISP_FB_PARAM    &__fb
#   endif

extern const vk_disp_fb_drv_t APP_DISP_FB_DRV;
#endif

/*============================ GLOBAL VARIABLES ==============================*/

usrapp_ui_common_t usrapp_ui_common = {
#if VSF_DISP_USE_SDL2 == ENABLED
    .disp                       = &usrapp_ui_common.disp_sdl2.use_as__vk_disp_t,
    .disp_sdl2                  = {
        .param                  = {
            .height             = APP_DISP_SDL2_HEIGHT,
            .width              = APP_DISP_SDL2_WIDTH,
            .drv                = &vk_disp_drv_sdl2,
            .color              = APP_DISP_SDL2_COLOR,
        },
        .title                  = APP_DISP_SDL2_TITLE,
        .amplifier              = APP_DISP_SDL2_AMPLIFIER,
    },
#elif VSF_DISP_USE_FB == ENABLED
    .disp                       = &usrapp_ui_common.disp_fb.use_as__vk_disp_t,
    .disp_fb                    = {
        .param                  = {
            .height             = APP_DISP_FB_HEIGHT,
            .width              = APP_DISP_FB_WIDTH,
            .drv                = &vk_disp_drv_fb,
            .color              = APP_DISP_FB_COLOR,
        },
        .fb                     = {
#   ifdef APP_DISP_FB_BUFFER
            .buffer             = (void *)APP_DISP_FB_BUFFER,
#   endif
            .drv                = &APP_DISP_FB_DRV,
#   ifdef APP_DISP_FB_PARAM
            .param              = APP_DISP_FB_PARAM,
#   endif
            .size               = vsf_disp_get_pixel_format_bytesize(APP_DISP_FB_COLOR) * APP_DISP_FB_WIDTH * APP_DISP_FB_HEIGHT,
            .num                = APP_DISP_FB_NUM,
            .pixel_byte_size    = vsf_disp_get_pixel_format_bytesize(APP_DISP_FB_COLOR),
        },
    },
#elif VSF_DISP_USE_MIPI_LCD == ENABLED
    .disp                       = &usrapp_ui_common.disp_mipi_lcd.use_as__vk_disp_t,
    .disp_mipi_lcd              = {
        .param                  = {
            .height             = APP_DISP_DEMO_HEIGHT,
            .width              = APP_DISP_DEMO_WIDTH,
            .drv                = &vk_disp_drv_mipi_lcd,
            .color              = APP_DISP_DEMO_COLOR,
        },
        .spi                    = APP_DISP_DEMO_SPI,
        .reset                  = {
            .gpio               = APP_DISP_DEMO_RESET_GPIO,
            .pin_mask           = APP_DISP_DEMO_RESET_PIN_MASK,
        },
        .dcx                    = {
            .gpio               = APP_DISP_DEMO_DCX_GPIO,
            .pin_mask           = APP_DISP_DEMO_DCX_PIN_MASK,
        },
        .clock_hz               = APP_DISP_DEMO_CLOCK_HZ,
        .init_seq               = (const uint8_t [])APP_DISP_DEMO_SEQ,
        .init_seq_len           = sizeof((const uint8_t [])APP_DISP_DEMO_SEQ),
    },
#elif VSF_DISP_USE_WINGDI == ENABLED
    .disp                       = &usrapp_ui_common.disp_wingdi.use_as__vk_disp_t,
    .disp_wingdi                = {
        .param                  = {
            .height             = APP_DISP_WINGDI_HEIGHT,
            .width              = APP_DISP_WINGDI_WIDTH,
            .drv                = &vk_disp_drv_wingdi,
            .color              = APP_DISP_WINGDI_COLOR,
        },
    },
#elif VSF_DISP_USE_USBD_UVC == ENABLED
    .disp                       = &usrapp_ui_common.disp_usbd_uvc.use_as__vk_disp_t,
    .disp_usbd_uvc                = {
        .param                  = {
            .height             = APP_DISP_USBD_UVC_HEIGHT,
            .width              = APP_DISP_USBD_UVC_WIDTH,
            .drv                = &vk_disp_drv_usbd_uvc,
            .color              = APP_DISP_USBD_UVC_COLOR,
        },
        .uvc                    = NULL,
    },
#else
    .disp                       = NULL,
#endif

#if VSF_USE_LVGL == ENABLED
#   ifdef APP_LVGL_DEMO_CFG_PIXEL_BUFFER_PTR
    .lvgl.color                 = (lv_color_t *)(APP_LVGL_DEMO_CFG_PIXEL_BUFFER_PTR),
#   endif
#endif
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_DL1X5 == ENABLED
void vsf_dl1x5_on_new_disp(vk_disp_t *disp)
{
    vsf_trace_info("ui: switch default display to dl1x5" VSF_TRACE_CFG_LINEEND);
    usrapp_ui_common.disp = disp;
}
#endif

#endif      // VSF_USE_UI
/* EOF */