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
#ifndef __VSF_DISP_MIPI_LCD_H__
#define __VSF_DISP_MIPI_LCD_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_MIPI_LCD == ENABLED

#include "kernel/vsf_kernel.h"
#include "hal/vsf_hal.h"

#if defined(__VSF_DISP_MIPI_LCD_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_DISP_MIPI_LCD_WRITE(__CMD, __PARAM_LEN, ...)                       \
            (__CMD), (__PARAM_LEN),  ##__VA_ARGS__

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_SPI != ENABLED
#   error "need VSF_HAL_USE_SPI"
#endif

#ifndef VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET
#   define VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET  ENABLED
#endif

#ifndef VSF_DISP_MIPI_LCD_USING_VSF_GPIO
#   define VSF_DISP_MIPI_LCD_USING_VSF_GPIO         ENABLED
#endif

#ifndef VSF_DISP_MIPI_LCD_CFG_WIDTH
#   define VSF_DISP_MIPI_LCD_CFG_WIDTH              240
#endif

#ifndef VSF_DISP_MIPI_LCD_CFG_HEIGTH
#   define VSF_DISP_MIPI_LCD_CFG_HEIGTH             320
#endif

#define VSF_DISP_MIPI_LCD_SPI_8BITS_MODE            0
#define VSF_DISP_MIPI_LCD_SPI_9BITS_MODE            1

#ifndef VSF_DISP_MIPI_LCD_SPI_MODE
#   define  VSF_DISP_MIPI_LCD_SPI_MODE              VSF_DISP_MIPI_LCD_SPI_8BITS_MODE
#endif

#if (VSF_DISP_MIPI_LCD_SPI_MODE != VSF_DISP_MIPI_LCD_SPI_8BITS_MODE) && \
    (VSF_DISP_MIPI_LCD_SPI_MODE != VSF_DISP_MIPI_LCD_SPI_9BITS_MODE)
#   error "vsf_disp_mipi_lcd: error spi mode"
#endif

#if VSF_DISP_MIPI_LCD_SPI_MODE ==  VSF_DISP_MIPI_LCD_SPI_9BITS_MODE
#   error "TODO: support 9bit spi mode"
#endif

#define VSF_DISP_MIPI_LCD_REFRESH_SEQ_LEN       \
    (1 + 1 + 4) + (1 + 1 + 4) + (1 + 1 + 4 + 4)

// MIPI Display Command Set
#define MIPI_DCS_NOP                     0x00
#define MIPI_DCS_SOFT_RESET              0x01
#define MIPI_DCS_GET_COMPRESSION_MODE    0x03
#define MIPI_DCS_GET_DISPLAY_ID          0x04
#define MIPI_DCS_GET_ERROR_COUNT_ON_DSI  0x05
#define MIPI_DCS_GET_RED_CHANNEL         0x06
#define MIPI_DCS_GET_GREEN_CHANNEL       0x07
#define MIPI_DCS_GET_BLUE_CHANNEL        0x08
#define MIPI_DCS_GET_DISPLAY_STATUS      0x09
#define MIPI_DCS_GET_POWER_MODE          0x0A
#define MIPI_DCS_GET_ADDRESS_MODE        0x0B
#define MIPI_DCS_GET_PIXEL_FORMAT        0x0C
#define MIPI_DCS_GET_DISPLAY_MODE        0x0D
#define MIPI_DCS_GET_SIGNAL_MODE         0x0E
#define MIPI_DCS_GET_DIAGNOSTIC_RESULT   0x0F
#define MIPI_DCS_ENTER_SLEEP_MODE        0x10
#define MIPI_DCS_EXIT_SLEEP_MODE         0x11
#define MIPI_DCS_ENTER_PARTIAL_MODE      0x12
#define MIPI_DCS_ENTER_NORMAL_MODE       0x13
#define MIPI_DCS_GET_IMAGE_CHECKSUM_RGB  0x14
#define MIPI_DCS_GET_IMAGE_CHECKSUM_CT   0x15
#define MIPI_DCS_EXIT_INVERT_MODE        0x20
#define MIPI_DCS_ENTER_INVERT_MODE       0x21
#define MIPI_DCS_SET_GAMMA_CURVE         0x26
#define MIPI_DCS_SET_DISPLAY_OFF         0x28
#define MIPI_DCS_SET_DISPLAY_ON          0x29
#define MIPI_DCS_SET_COLUMN_ADDRESS      0x2A
#define MIPI_DCS_SET_PAGE_ADDRESS        0x2B
#define MIPI_DCS_WRITE_MEMORY_START      0x2C
#define MIPI_DCS_WRITE_LUT               0x2D
#define MIPI_DCS_READ_MEMORY_START       0x2E
#define MIPI_DCS_SET_PARTIAL_ROWS        0x30
#define MIPI_DCS_SET_PARTIAL_COLUMNS     0x31
#define MIPI_DCS_SET_SCROLL_AREA         0x33
#define MIPI_DCS_SET_TEAR_OFF            0x34
#define MIPI_DCS_SET_TEAR_ON             0x35
#define MIPI_DCS_SET_ADDRESS_MODE        0x36
#define MIPI_DCS_SET_SCROLL_START        0x37
#define MIPI_DCS_EXIT_IDLE_MODE          0x38
#define MIPI_DCS_ENTER_IDLE_MODE         0x39
#define MIPI_DCS_SET_PIXEL_FORMAT        0x3A
#define MIPI_DCS_WRITE_MEMORY_CONTINUE   0x3C
#define MIPI_DCS_SET_3D_CONTROL          0x3D
#define MIPI_DCS_READ_MEMORY_CONTINUE    0x3E
#define MIPI_DCS_GET_3D_CONTROL          0x3F
#define MIPI_DCS_SET_VSYNC_TIMING        0x40
#define MIPI_DCS_SET_TEAR_SCANLINE       0x44
#define MIPI_DCS_GET_SCANLINE            0x45
#define MIPI_DCS_SET_DISPLAY_BRIGHTNESS  0x51
#define MIPI_DCS_GET_DISPLAY_BRIGHTNESS  0x52
#define MIPI_DCS_WRITE_CONTROL_DISPLAY   0x53
#define MIPI_DCS_GET_CONTROL_DISPLAY     0x54
#define MIPI_DCS_WRITE_POWER_SAVE        0x55
#define MIPI_DCS_GET_POWER_SAVE          0x56
#define MIPI_DCS_SET_CABC_MIN_BRIGHTNESS 0x5E
#define MIPI_DCS_GET_CABC_MIN_BRIGHTNESS 0x5F
#define MIPI_DCS_READ_DDB_START          0xA1
#define MIPI_DCS_READ_PPS_START          0xA2
#define MIPI_DCS_READ_DDB_CONTINUE       0xA8
#define MIPI_DCS_READ_PPS_CONTINUE       0xA9

// reg: PIXEL_FORAMT high 4 bits
#define MIPI_PIXEL_FORMAT_DPI_3BITS      (1 << 4)
#define MIPI_PIXEL_FORMAT_DPI_8BITS      (2 << 4)
#define MIPI_PIXEL_FORMAT_DPI_12BITS     (3 << 4)
#define MIPI_PIXEL_FORMAT_DPI_16BITS     (5 << 4)
#define MIPI_PIXEL_FORMAT_DPI_18BITS     (6 << 4)
#define MIPI_PIXEL_FORMAT_DPI_24BITS     (7 << 4)
// reg: PIXEL_FORAMT low 4 bits
#define MIPI_PIXEL_FORMAT_DBI_3BITS      (1)
#define MIPI_PIXEL_FORMAT_DBI_8BITS      (2)
#define MIPI_PIXEL_FORMAT_DBI_12BITS     (3)
#define MIPI_PIXEL_FORMAT_DBI_16BITS     (5)
#define MIPI_PIXEL_FORMAT_DBI_18BITS     (6)
#define MIPI_PIXEL_FORMAT_DBI_24BITS     (7)

#ifndef MIPI_LCD_PIXEL_FORMAT_VALUE
#   define MIPI_LCD_PIXEL_FORMAT_VALUE  (MIPI_PIXEL_FORMAT_DPI_16BITS | MIPI_PIXEL_FORMAT_DBI_16BITS)
#endif

// These 3 bits control the direction from the MPU to memory write/read.
#define GRAM_WRITE_TOP_TO_BOTTOM        (0 << 7)
#define GRAM_WRITE_BOTTOM_TO_TOP        (1 << 7)
#define GRAM_WRITE_LEFT_TO_RIGHT        (0 << 6)
#define GRAM_WRITE_RIGHT_TO_LEFT        (1 << 6)
#define GRAM_WRITE_NORMAL_MODE          (0 << 5)
#define GRAM_WRITE_ROW_COLUMN_EXCHANGE  (1 << 5)

// LCD vertical refresh direction control.
#define LCD_REFRESH_TOP_TO_BOTTOM       (0 << 4)
#define LCD_REFRESH_BOTTOM_TO_TOP       (1 << 4)
#define LCD_REFRESH_RGB_ORDER           (0 << 3)
#define LCD_REFRESH_BGR_ORDER           (1 << 3)
#define LCD_REFRESH_LEFT_TO_RIGHT       (0 << 2)
#define LCD_REFRESH_RIGHT_TO_LEFT       (1 << 2)
#ifndef MIPI_LCD_ADDRESS_MODE_VALUE
#   define MIPI_LCD_ADDRESS_MODE_VALUE  (GRAM_WRITE_TOP_TO_BOTTOM | GRAM_WRITE_LEFT_TO_RIGHT | GRAM_WRITE_NORMAL_MODE)
#endif

#define TEAR_OUTPUT_V_BLANKING_ONLY             0
#define TEAR_OUTPUT_V_BLANKING_AND_H_BLANKING   1
#ifndef MIPI_LCD_TEAR_VALUE
#   define MIPI_LCD_TEAR_VALUE                  (TEAR_OUTPUT_V_BLANKING_ONLY)
#endif

#ifndef VSF_DISP_MIPI_LCD_DEFAUT_INIT
#   define VSF_DISP_MIPI_LCD_DEFAUT_INIT \
        VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_SET_ADDRESS_MODE,  1, MIPI_LCD_ADDRESS_MODE_VALUE /*0x40*/), \
        VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_SET_PIXEL_FORMAT,  1, MIPI_LCD_PIXEL_FORMAT_VALUE /*0x55*/), \
        VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_SET_DISPLAY_ON,    0), \
        VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_SET_TEAR_ON,       1, 0x00), /* Tearing Effect Line ON, for V-Blanking information only */
#endif

#ifndef VSF_DISP_MIPI_LCD_S6D05A1_BASE
#   define VSF_DISP_MIPI_LCD_S6D05A1_BASE \
        VSF_DISP_MIPI_LCD_WRITE(0xF0,  2, 0x5A, 0x5A), /*PASSWD1*/ \
        VSF_DISP_MIPI_LCD_WRITE(0xF1,  2, 0x5A, 0x5A), /*PASSWD2*/ \
        VSF_DISP_MIPI_LCD_WRITE(0xF2, 19, 0x3B, 0x40, 0x03, 0x04, 0x02, 0x08, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x40, 0x08, 0x08, 0x08, 0x08), /*DISCTL*/ \
        VSF_DISP_MIPI_LCD_WRITE(0xF4, 14, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x03, 0x00, 0x70, 0x03), /*PWRCTL*/ \
        VSF_DISP_MIPI_LCD_WRITE(0xF5, 12, 0x00, 0x54, 0x73, 0x00, 0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x53, 0x71), /*VCMCTL*/ \
        VSF_DISP_MIPI_LCD_WRITE(0xF6,  8, 0x04, 0x00, 0x08, 0x03, 0x01, 0x00, 0x01, 0x00), /*SRCCTL*/ \
        VSF_DISP_MIPI_LCD_WRITE(0xF7,  5, 0x48, 0x80, 0x10, 0x02, 0x00), /*IFCTL*/ \
        VSF_DISP_MIPI_LCD_WRITE(0xF8,  2, 0x11, 0x00), /*PANELCTL*/ \
        VSF_DISP_MIPI_LCD_WRITE(0xF9,  1, 0x27), /*GAMMASEL*/ \
        VSF_DISP_MIPI_LCD_WRITE(0xFA, 20, 0x0B, 0x0B, 0x0F, 0x26, 0x2A, 0x30, 0x33, 0x12, 0x1F, 0x25, 0x31, 0x30, 0x24, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x3F), /*PGAMMACTL*/
#endif

#ifndef VSF_DISP_MIPI_LCD_S6D05A1_EXTEND
#   define VSF_DISP_MIPI_LCD_S6D05A1_EXTEND
#endif

#define VSF_DISP_MIPI_LCD_S6D05A1  { VSF_DISP_MIPI_LCD_S6D05A1_BASE VSF_DISP_MIPI_LCD_DEFAUT_INIT VSF_DISP_MIPI_LCD_S6D05A1_EXTEND }

#ifndef VSF_DISP_MIPI_LCD_ILI9488_BASE
#   define VSF_DISP_MIPI_LCD_ILI9488_BASE \
        VSF_DISP_MIPI_LCD_WRITE(0xE0, 15, 0x00, 0x07, 0x0f, 0x0D, 0x1B, 0x0A, 0x3c, 0x78, 0x4A, 0x07, 0x0E, 0x09, 0x1B, 0x1e, 0x0f), /* Positive Gamma Control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xE1, 15, 0x00, 0x22, 0x24, 0x06, 0x12, 0x07, 0x36, 0x47, 0x47, 0x06, 0x0a, 0x07, 0x30, 0x37, 0x0f), /* Negative Gamma Control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xC0, 2,  0x10, 0x10),                /* Power Control 1 */ \
        VSF_DISP_MIPI_LCD_WRITE(0xC1, 1,  0x41),                      /* Power Control 2 */ \
        VSF_DISP_MIPI_LCD_WRITE(0xC5, 3,  0x00, 0x22, 0x80),          /* VCOM Control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xB0, 1,  0x00),                      /* Interface Mode Contro */ \
        VSF_DISP_MIPI_LCD_WRITE(0xB1, 2,  0x60, 0x11),                /* frame rate control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xB4, 1,  0x02),                      /* Display Inversion Control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xB6, 2,  0x02, 0x02),                /* Display Function Control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xB7, 1,  0x06),                      /* Entry Mode Set */ \
        VSF_DISP_MIPI_LCD_WRITE(0xE9, 1,  0x00),                      /* Set Image Function: 24bit data bus */ \
        VSF_DISP_MIPI_LCD_WRITE(0XF7, 4,  0xA9, 0x51, 0x2C, 0x82),    /* Adjust Control 3 */
#endif

#ifndef VSF_DISP_MIPI_LCD_ILI9488_EXTEND
#   define VSF_DISP_MIPI_LCD_ILI9488_EXTEND
#endif

#define VSF_DISP_MIPI_LCD_ILI9488  { VSF_DISP_MIPI_LCD_ILI9488_BASE VSF_DISP_MIPI_LCD_DEFAUT_INIT VSF_DISP_MIPI_LCD_ILI9488_EXTEND }

#ifndef VSF_DISP_MIPI_LCD_ILI9341_BASE
#   define VSF_DISP_MIPI_LCD_ILI9341_BASE \
        VSF_DISP_MIPI_LCD_WRITE(0xCF,  3, 0x00, 0xC1, 0X30), \
        VSF_DISP_MIPI_LCD_WRITE(0xED,  4, 0x64, 0x03, 0X12, 0X81), \
        VSF_DISP_MIPI_LCD_WRITE(0xE8,  3, 0x85, 0x10, 0x7A), \
        VSF_DISP_MIPI_LCD_WRITE(0xCB,  5, 0x39, 0x2C, 0x00, 0x34, 0x02), \
        VSF_DISP_MIPI_LCD_WRITE(0xF7,  1, 0x20), \
        VSF_DISP_MIPI_LCD_WRITE(0xEA,  2, 0x00, 0x00), \
        VSF_DISP_MIPI_LCD_WRITE(0xC0,  1, 0x1B),                    /* Power Control 1 */ \
        VSF_DISP_MIPI_LCD_WRITE(0xC1,  1, 0x01),                    /* Power Control 2 */ \
        VSF_DISP_MIPI_LCD_WRITE(0xC5,  2, 0x30, 0x30),              /* VCOM Control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xC7,  1, 0XB7),                    /* VCOM Control 2 */ \
        VSF_DISP_MIPI_LCD_WRITE(0xB1,  2, 0x01, 0x1B),              /* frame rate control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xB6,  2, 0x02, 0x82),              /* Display Function Control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xB5,  4, 0x5F, 0x5F, 0x3F, 0x3F),  /* Blanking Porch Control, VFP/VBP/0x00/HBP */ \
        VSF_DISP_MIPI_LCD_WRITE(0xF2,  1, 0x00), \
        VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_SET_GAMMA_CURVE,  1, 0x01),    /* Gamma Set */  \
        VSF_DISP_MIPI_LCD_WRITE(0xE0, 15, 0x0F, 0x2A, 0x28, 0x08, 0x0E, 0x08, 0x54, 0XA9, 0x43, 0x0A, 0x0F, 0x00, 0x00, 0x00, 0x00), /* Positive Gamma Control */ \
        VSF_DISP_MIPI_LCD_WRITE(0XE1, 15, 0x00, 0x15, 0x17, 0x07, 0x11, 0x06, 0x2B, 0x56, 0x3C, 0x05, 0x10, 0x0F, 0x3F, 0x3F, 0x0F), /* Negative Gamma Control */
#endif

#ifndef VSF_DISP_MIPI_LCD_ILI9341_EXTEND
#   define VSF_DISP_MIPI_LCD_ILI9341_EXTEND
#endif

#define VSF_DISP_MIPI_LCD_ILI9341  { VSF_DISP_MIPI_LCD_ILI9341_BASE VSF_DISP_MIPI_LCD_DEFAUT_INIT VSF_DISP_MIPI_LCD_ILI9341_EXTEND }

#ifndef VSF_DISP_MIPI_LCD_ST7789V_BASE
#   define VSF_DISP_MIPI_LCD_ST7789V_BASE \
        VSF_DISP_MIPI_LCD_WRITE(0xB1,  3, 0x40, 0x7F, 0x14), /* RGB Interface Control */\
        VSF_DISP_MIPI_LCD_WRITE(0xB2,  5, 0x0C, 0x0C, 0x00, 0x33, 0x33), /* Porch Setting */\
        VSF_DISP_MIPI_LCD_WRITE(0xB7,  1, 0x35), /* Gate Control */\
        VSF_DISP_MIPI_LCD_WRITE(0xBB,  1, 0x20), /* VCOM Setting */\
        VSF_DISP_MIPI_LCD_WRITE(0xC0,  1, 0x2C), /* LCM Control  */\
        VSF_DISP_MIPI_LCD_WRITE(0xC2,  1, 0x01), /* VDV and VRH Command Enable */\
        VSF_DISP_MIPI_LCD_WRITE(0xC3,  1, 0x0B), /* VRH Set */\
        VSF_DISP_MIPI_LCD_WRITE(0xC4,  1, 0x20), /* VDV Set */\
        VSF_DISP_MIPI_LCD_WRITE(0xC6,  1, 0x1F), /* Frame Rate Control in Normal Mode, 1F: 39 */\
        VSF_DISP_MIPI_LCD_WRITE(0xD0,  2, 0xA4, 0xA1), /* Power Control 1 */\
        VSF_DISP_MIPI_LCD_WRITE(0xE0, 14, 0xD0, 0x03, 0x09, 0x0E, 0x11, 0x3D, 0x47, 0x55, 0x53, 0x1A, 0x16, 0x14, 0x1F, 0x22), /* Positive Voltage Gamma Control */\
        VSF_DISP_MIPI_LCD_WRITE(0xE1, 14, 0xD0, 0x02, 0x08, 0x0D, 0x12, 0x2C, 0x43, 0x55, 0x53, 0x1E, 0x1B, 0x19, 0x20, 0x22), /* Negative Voltage Gamma Control */
#endif

#ifndef VSF_DISP_MIPI_LCD_ST7789V_EXTEND
#   define VSF_DISP_MIPI_LCD_ST7789V_EXTEND
#endif

#define VSF_DISP_MIPI_LCD_ST7789V  { VSF_DISP_MIPI_LCD_ST7789V_BASE VSF_DISP_MIPI_LCD_DEFAUT_INIT VSF_DISP_MIPI_LCD_ST7789V_EXTEND }


#ifndef VSF_DISP_MIPI_LCD_ST7796S_BASE
#   define VSF_DISP_MIPI_LCD_ST7796S_BASE \
        VSF_DISP_MIPI_LCD_WRITE(0xF0,  1, 0xC3), /* Command Set Control, C3h enable command 2 part I */ \
        VSF_DISP_MIPI_LCD_WRITE(0xF0,  1, 0x96), /* Command Set Control, 96h enable command 2 part II */ \
        VSF_DISP_MIPI_LCD_WRITE(0xE8,  8, 0x40, 0x82, 0x07, 0x18, 0x27, 0x0A, 0xB6, 0x33), /* Display Output Ctrl Adjust */ \
        VSF_DISP_MIPI_LCD_WRITE(0xC5,  1, 0x27), /* VCOM Control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xC2,  1, 0xA7), /* Power Control 3 */ \
        VSF_DISP_MIPI_LCD_WRITE(0xE0, 14, 0xF0, 0x01, 0x06, 0x0F, 0x12, 0x1D, 0x36, 0x54, 0x44, 0x0C, 0x18, 0x16, 0x13, 0x15), /* Positive Gamma Control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xE1, 14, 0xF0, 0x01, 0x05, 0x0A, 0x0B, 0x07, 0x32, 0x44, 0x44, 0x0C, 0x18, 0x17, 0x13, 0x16), /* Negative Gamma Control */ \
        VSF_DISP_MIPI_LCD_WRITE(0xB5,  4, 0xFF, 0xFF, 0x00, 0x04), /* Blanking Porch Control, VFP/VBP/0x00/HBP */ \
        VSF_DISP_MIPI_LCD_WRITE(0xB1,  2, 0x00, 0x10), /* Blanking Porch Control, VFP/VBP/0x00/HBP */ \
        VSF_DISP_MIPI_LCD_WRITE(0xF0,  1, 0x3C), /* Command Set Control, 3Ch disable command 2 part I */ \
        VSF_DISP_MIPI_LCD_WRITE(0xF0,  1, 0x69), /* Command Set Control, 69h disable command 2 part II */
#endif

#ifndef VSF_DISP_MIPI_LCD_ST7796S_EXTEND
#   define VSF_DISP_MIPI_LCD_ST7796S_EXTEND
#endif

#define VSF_DISP_MIPI_LCD_ST7796S  { VSF_DISP_MIPI_LCD_ST7796S_BASE VSF_DISP_MIPI_LCD_DEFAUT_INIT VSF_DISP_MIPI_LCD_ST7796S_EXTEND }

/*============================ TYPES =========================================*/

enum {
    MIPI_LCD_STATE_INIT = 0,
    MIPI_LCD_STATE_HW_RESET_WAIT_CPL,
    MIPI_LCD_STATE_INIT_SEQ_EXIT_SLEEP,
    MIPI_LCD_STATE_INIT_CPL,
};

vsf_class(vk_disp_mipi_lcd_t) {
    public_member(
        implement(vk_disp_t)
        vsf_spi_t       *spi;
        uint32_t         clock_hz;
        const uint8_t   *init_seq;
        const uint16_t   init_seq_len;
#if VSF_DISP_MIPI_LCD_USING_VSF_GPIO == ENABLED
        struct {
            vsf_gpio_t * gpio;
            uint32_t     pin_mask;
        } reset, dcx;
#endif
    )

    private_member(
        vsf_teda_t           teda;
        vsf_eda_evthandler_t evthandler;

        vk_disp_area_t      area;
        uint8_t             *cur_buffer;

        uint8_t             refresh_seq[VSF_DISP_MIPI_LCD_REFRESH_SEQ_LEN];
#if VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
        uint8_t             reset_state : 1;
#endif

        struct {
            const uint8_t   *buf;
            uint16_t        max_cnt;
            uint16_t        cur_cnt;
        } seq;

        struct {
            uint8_t         cmd;
            uint32_t        param_size;
            void            *param_buffer;
        } cmd;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_disp_drv_t vk_disp_drv_mipi_lcd;

/*============================ PROTOTYPES ====================================*/

extern void vk_disp_mipi_tearing_effect_line_ready(vk_disp_mipi_lcd_t *disp_mipi_lcd);

#if VSF_DISP_MIPI_LCD_SPI_MODE == VSF_DISP_MIPI_LCD_SPI_8BITS_MODE
extern void vk_disp_mipi_lcd_dcx_write(vk_disp_mipi_lcd_t *disp_mipi_lcd, bool state);
#endif

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_MIPI_SPI_LCD_H__
