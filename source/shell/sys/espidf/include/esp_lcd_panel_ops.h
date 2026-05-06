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
 * Clean-room re-implementation of ESP-IDF public API "esp_lcd_panel_ops.h".
 *
 * Baseline: ESP-IDF v5.x public API surface.
 */

#ifndef __VSF_ESPIDF_ESP_LCD_PANEL_OPS_H__
#define __VSF_ESPIDF_ESP_LCD_PANEL_OPS_H__

/*============================ INCLUDES ======================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_lcd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ PROTOTYPES ====================================*/

extern esp_err_t esp_lcd_panel_reset(esp_lcd_panel_handle_t panel);
extern esp_err_t esp_lcd_panel_init(esp_lcd_panel_handle_t panel);
extern esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t panel);
extern esp_err_t esp_lcd_panel_draw_bitmap(
    esp_lcd_panel_handle_t panel, int x_start, int y_start,
    int x_end, int y_end, const void *color_data);
extern esp_err_t esp_lcd_panel_mirror(
    esp_lcd_panel_handle_t panel, bool mirror_x, bool mirror_y);
extern esp_err_t esp_lcd_panel_swap_xy(
    esp_lcd_panel_handle_t panel, bool swap_axes);
extern esp_err_t esp_lcd_panel_set_gap(
    esp_lcd_panel_handle_t panel, int x_gap, int y_gap);
extern esp_err_t esp_lcd_panel_invert_color(
    esp_lcd_panel_handle_t panel, bool invert_color_data);
extern esp_err_t esp_lcd_panel_disp_on_off(
    esp_lcd_panel_handle_t panel, bool on_off);
extern esp_err_t esp_lcd_panel_disp_sleep(
    esp_lcd_panel_handle_t panel, bool sleep);
extern esp_err_t esp_lcd_panel_set_brightness(
    esp_lcd_panel_handle_t panel, int brightness);
extern esp_err_t esp_lcd_panel_draw_bitmap_2d(
    esp_lcd_panel_handle_t panel, int x_start, int y_start,
    int x_end, int y_end, const void *src_data,
    size_t src_x_size, size_t src_y_size,
    int src_x_start, int src_y_start,
    int src_x_end, int src_y_end);

#ifdef __cplusplus
}
#endif

#endif /* __VSF_ESPIDF_ESP_LCD_PANEL_OPS_H__ */
