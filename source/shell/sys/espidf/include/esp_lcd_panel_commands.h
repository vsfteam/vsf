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
 * Clean-room re-implementation of ESP-IDF public header
 * "esp_lcd_panel_commands.h" -- Standard MIPI DCS command macros.
 */

#ifndef __VSF_ESPIDF_ESP_LCD_PANEL_COMMANDS_H__
#define __VSF_ESPIDF_ESP_LCD_PANEL_COMMANDS_H__

#define LCD_CMD_NOP          0x00
#define LCD_CMD_SWRESET      0x01
#define LCD_CMD_RDDID        0x04
#define LCD_CMD_RDDST        0x09
#define LCD_CMD_RDDPM        0x0A
#define LCD_CMD_RDD_MADCTL   0x0B
#define LCD_CMD_RDD_COLMOD   0x0C
#define LCD_CMD_RDDIM        0x0D
#define LCD_CMD_RDDSM        0x0E
#define LCD_CMD_RDDSR        0x0F
#define LCD_CMD_SLPIN        0x10
#define LCD_CMD_SLPOUT       0x11
#define LCD_CMD_PTLON        0x12
#define LCD_CMD_NORON        0x13
#define LCD_CMD_INVOFF       0x20
#define LCD_CMD_INVON        0x21
#define LCD_CMD_GAMSET       0x26
#define LCD_CMD_DISPOFF      0x28
#define LCD_CMD_DISPON       0x29
#define LCD_CMD_CASET        0x2A
#define LCD_CMD_RASET        0x2B
#define LCD_CMD_RAMWR        0x2C
#define LCD_CMD_RAMRD        0x2E
#define LCD_CMD_PTLAR        0x30
#define LCD_CMD_VSCRDEF      0x33
#define LCD_CMD_TEOFF        0x34
#define LCD_CMD_TEON         0x35
#define LCD_CMD_MADCTL       0x36
#define LCD_CMD_VSCSAD       0x37
#define LCD_CMD_IDMOFF       0x38
#define LCD_CMD_IDMON        0x39
#define LCD_CMD_COLMOD       0x3A
#define LCD_CMD_RAMWRC       0x3C
#define LCD_CMD_RAMRDC       0x3E
#define LCD_CMD_STE          0x44
#define LCD_CMD_GDCAN        0x45
#define LCD_CMD_WRDISBV      0x51
#define LCD_CMD_RDDISBV      0x52

#define LCD_CMD_MH_BIT       (1 << 2)
#define LCD_CMD_BGR_BIT      (1 << 3)
#define LCD_CMD_ML_BIT       (1 << 4)
#define LCD_CMD_MV_BIT       (1 << 5)
#define LCD_CMD_MX_BIT       (1 << 6)
#define LCD_CMD_MY_BIT       (1 << 7)

#endif /* __VSF_ESPIDF_ESP_LCD_PANEL_COMMANDS_H__ */
