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
 * Clean-room re-implementation of ESP-IDF public API "esp_lcd_panel_io.h".
 *
 * In the VSF bridge the IO layer is a thin configuration carrier. On a
 * host target (Windows via wingdi / SDL2) the panel layer wraps the
 * board's existing vk_disp_t and never calls tx_param / tx_color / rx_param.
 * On an MCU target the IO layer's stored config can be consumed by a
 * VSF display driver factory.
 *
 * Baseline: ESP-IDF v5.x public API surface.
 */

#ifndef __VSF_ESPIDF_ESP_LCD_PANEL_IO_H__
#define __VSF_ESPIDF_ESP_LCD_PANEL_IO_H__

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

extern esp_err_t esp_lcd_panel_io_rx_param(
    esp_lcd_panel_io_handle_t io, int lcd_cmd,
    void *param, size_t param_size);

extern esp_err_t esp_lcd_panel_io_tx_param(
    esp_lcd_panel_io_handle_t io, int lcd_cmd,
    const void *param, size_t param_size);

extern esp_err_t esp_lcd_panel_io_tx_color(
    esp_lcd_panel_io_handle_t io, int lcd_cmd,
    const void *color, size_t color_size);

extern esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t io);

extern esp_err_t esp_lcd_panel_io_register_event_callbacks(
    esp_lcd_panel_io_handle_t io,
    const esp_lcd_panel_io_callbacks_t *cbs,
    void *user_ctx);

#ifdef __cplusplus
}
#endif

#endif /* __VSF_ESPIDF_ESP_LCD_PANEL_IO_H__ */
