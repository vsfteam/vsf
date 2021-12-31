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

#ifndef __VSF_USBH_DL1X5_H__
#define __VSF_USBH_DL1X5_H__

/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_DL1X5 == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct edid_t {
    uint8_t         header[8];
    struct edid_vendor_product_id_t {
        uint16_t    manufacturer;
        uint16_t    product;
        uint32_t    serial;
        uint8_t     week;
        uint8_t     year;
    } PACKED        product;
    struct edid_vendor_version_t {
        uint8_t     version;
        uint8_t     revision;
    } PACKED        version;
    struct edid_basic_param_t {
        uint8_t     video_input;
        uint8_t     h_size_cm;
        uint8_t     v_size_cm;
        uint8_t     gamma;
        uint8_t     dpms;
    } PACKED        basic;
    struct edid_color_t {
        uint8_t     red_green_low;
        uint8_t     bluw_white_low;
        uint8_t     red_x;
        uint8_t     red_y;
        uint8_t     green_x;
        uint8_t     green_y;
        uint8_t     blue_x;
        uint8_t     blue_y;
        uint8_t     white_x;
        uint8_t     white_y;
    } PACKED        color;
    struct edid_established_timings_t {
        uint16_t    timings;
        uint8_t     reserved_timings;
    } PACKED established_timings;
    struct edid_standard_timings_t {
        uint16_t    timing_id[8];
    } PACKED standard_timings;
    struct edid_detailed_timing_t {
        uint16_t    pixel_clock_10khz;
        uint8_t     h_active_low;
        uint8_t     h_blanking_low;
        uint8_t     h_blanking_high         : 4;
        uint8_t     h_active_high           : 4;
        uint8_t     v_active_low;
        uint8_t     v_blanking_low;
        uint8_t     v_blanking_high         : 4;
        uint8_t     v_active_high           : 4;
        uint8_t     h_sync_offset_low;
        uint8_t     h_sync_width_low;
        uint8_t     v_sync_width_low        : 4;
        uint8_t     v_sync_offset_low       : 4;
        uint8_t     v_sync_width_high       : 2;
        uint8_t     v_sync_offset_high      : 2;
        uint8_t     h_sync_width_high       : 2;
        uint8_t     h_sync_offset_high      : 2;
        uint8_t     h_image_size_mm_low;
        uint8_t     v_image_size_mm_low;
        uint8_t     v_image_size_mm_high    : 4;
        uint8_t     h_image_size_mm_high    : 4;
        uint8_t     h_border;
        uint8_t     v_border;
        uint8_t     stereo_mode             : 1;
        uint8_t     h_sync_positive         : 1;
        uint8_t     v_sync_positive         : 1;
        uint8_t     seperate_sync           : 2;
        uint8_t     stereo                  : 2;
        uint8_t     interlaced              : 1;
    } PACKED detailed_timings[4];
    uint8_t         extension_flag;
    uint8_t         checksum;
} PACKED edid_t;

typedef struct vga_timing_t {
    uint16_t        pixel_clock_10khz;
    struct {
        uint16_t    active;
        uint16_t    front_porch;
        uint16_t    sync;
        uint16_t    back_porch;
        uint8_t     sync_positive;
    } h, v;
} vga_timing_t;

typedef void (*vk_usbh_dl1x5_callback_t)(void *dev, vsf_err_t err);

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_class_drv_t vk_usbh_dl1x5_drv;

/*============================ PROTOTYPES ====================================*/

extern vga_timing_t * vk_usbh_dl1x5_get_timing(void *dev);

#if !(VSF_USE_UI == ENABLED && VSF_DISP_USE_DL1X5 == ENABLED)
extern void vk_usbh_dl1x5_set_callback(void *dev, vk_usbh_dl1x5_callback_t callback);

extern vsf_err_t vk_usbh_dl1x5_commit(void *dev);
// pixel_num in range [1, 256]
extern bool vk_usbh_dl1x5_fill_color(void *dev, uint_fast32_t gram_pixel_addr,
            uint_fast16_t pixel_num, uint16_t color_rgb565);
extern bool vk_usbh_dl1x5_fill_buf(void *dev, uint_fast32_t gram_pixel_addr,
            uint_fast16_t pixel_num, uint16_t *color_rgb565_ptr);
#endif

#ifdef __cplusplus
}
#endif

#endif
#endif
