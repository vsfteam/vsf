/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
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
 ****************************************************************************/

/*
 * Clean-room re-implementation of ESP-IDF public API "esp_lcd_types.h".
 *
 * Baseline: ESP-IDF v5.x public API surface.
 */

#ifndef __VSF_ESPIDF_ESP_LCD_TYPES_H__
#define __VSF_ESPIDF_ESP_LCD_TYPES_H__

/*============================ INCLUDES ======================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

typedef struct esp_lcd_panel_io_t *esp_lcd_panel_io_handle_t;  /* IO handle  */
typedef struct esp_lcd_panel_t    *esp_lcd_panel_handle_t;      /* Panel handle */

typedef enum {
    LCD_RGB_ELEMENT_ORDER_RGB,
    LCD_RGB_ELEMENT_ORDER_BGR,
} lcd_rgb_element_order_t;

typedef enum {
    LCD_RGB_DATA_ENDIAN_BIG    = 0,
    LCD_RGB_DATA_ENDIAN_LITTLE = 1,
} lcd_rgb_data_endian_t;

typedef enum {
    LCD_CLOCK_SOURCE_NONE = 0,
} lcd_clock_source_t;

typedef struct {
    uint32_t pclk_hz;
    uint32_t h_res;
    uint32_t v_res;
    uint32_t hsync_pulse_width;
    uint32_t hsync_back_porch;
    uint32_t hsync_front_porch;
    uint32_t vsync_pulse_width;
    uint32_t vsync_back_porch;
    uint32_t vsync_front_porch;
    struct {
        uint32_t hsync_idle_low  : 1;
        uint32_t vsync_idle_low  : 1;
        uint32_t de_idle_high    : 1;
        uint32_t pclk_active_neg : 1;
        uint32_t pclk_idle_high  : 1;
    } flags;
} esp_lcd_rgb_timing_t;

enum {
    LCD_COLOR_FMT_RGB565   = 0,
    LCD_COLOR_FMT_RGB888   = 1,
    LCD_COLOR_FMT_GRAY8    = 2,
};

typedef uint8_t lcd_color_format_t;

typedef enum {
    LCD_COLOR_RANGE_LIMIT = 0,
    LCD_COLOR_RANGE_FULL  = 1,
} lcd_color_range_t;

typedef enum {
    LCD_YUV_CONV_STD_BT601 = 0,
    LCD_YUV_CONV_STD_BT709 = 1,
} lcd_yuv_conv_std_t;

typedef struct {
    uint32_t h_size;
    uint32_t v_size;
    uint32_t hsync_pulse_width;
    uint32_t hsync_back_porch;
    uint32_t hsync_front_porch;
    uint32_t vsync_pulse_width;
    uint32_t vsync_back_porch;
    uint32_t vsync_front_porch;
} esp_lcd_video_timing_t;

typedef struct {
    esp_lcd_panel_io_handle_t    io_handle;
    void                        *user_ctx;
} esp_lcd_panel_io_event_data_t;

typedef bool (*esp_lcd_panel_io_color_trans_done_cb_t)(
    esp_lcd_panel_io_handle_t    panel_io,
    esp_lcd_panel_io_event_data_t *edata,
    void                        *user_ctx
);

typedef struct {
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done;
} esp_lcd_panel_io_callbacks_t;

#ifdef __cplusplus
}
#endif

#endif /* __VSF_ESPIDF_ESP_LCD_TYPES_H__ */
