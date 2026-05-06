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
 * VSF extension to the ESP-IDF LCD panel API.
 *
 * esp_lcd_new_panel_from_vsf_disp() creates an esp_lcd_panel_handle_t
 * that wraps a board-provided vk_disp_t.  The board owns the display
 * hardware lifecycle (SPI / I2C / GPIO / init sequence); the espidf
 * panel layer is a thin consumer.
 */

#ifndef __VSF_ESPIDF_ESP_LCD_PANEL_VSF_H__
#define __VSF_ESPIDF_ESP_LCD_PANEL_VSF_H__

/*============================ INCLUDES ======================================*/

#include "esp_err.h"
#include "esp_lcd_types.h"
#include "esp_lcd_panel_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

typedef struct vk_disp_t vk_disp_t;

/*============================ PROTOTYPES ====================================*/

extern esp_err_t esp_lcd_new_panel_from_vsf_disp(
    vk_disp_t                          *disp,
    const esp_lcd_panel_dev_config_t   *panel_dev_config,
    esp_lcd_panel_handle_t             *ret_panel);

#ifdef __cplusplus
}
#endif

#endif /* __VSF_ESPIDF_ESP_LCD_PANEL_VSF_H__ */
