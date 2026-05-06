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
 * Clean-room re-implementation of ESP-IDF public header "esp_lcd_panel_dev.h".
 */

#ifndef __VSF_ESPIDF_ESP_LCD_PANEL_DEV_H__
#define __VSF_ESPIDF_ESP_LCD_PANEL_DEV_H__

/*============================ INCLUDES ======================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "esp_lcd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

typedef struct {
    lcd_rgb_element_order_t  rgb_ele_order;
    lcd_rgb_data_endian_t    data_endian;
    uint32_t                 bits_per_pixel;
    int                      reset_gpio_num;
    void                    *vendor_config;
    struct {
        uint32_t reset_active_high : 1;
    } flags;
} esp_lcd_panel_dev_config_t;

#ifdef __cplusplus
}
#endif

#endif /* __VSF_ESPIDF_ESP_LCD_PANEL_DEV_H__ */
