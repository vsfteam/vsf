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
#ifndef __VSF_DISP_MIPI_SPI_LCD_H__
#define __VSF_DISP_MIPI_SPI_LCD_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_MIPI_SPI_LCD == ENABLED

#include "kernel/vsf_kernel.h"
#include "hal/vsf_hal.h"

#if defined(__VSF_DISP_MIPI_SPI_LCD_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO: add scrolling demo

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_SPI != ENABLED
#   error "need VSF_HAL_USE_SPI"
#endif

#ifndef VSF_DISP_MIPI_SPI_LCD_SUPPORT_HARDWARE_RESET
#   define VSF_DISP_MIPI_SPI_LCD_SUPPORT_HARDWARE_RESET     ENABLED
#endif

#ifndef VSF_DISP_MIPI_SPI_LCD_USING_VSF_GPIO
#   define VSF_DISP_MIPI_SPI_LCD_USING_VSF_GPIO             ENABLED
#endif

#define VSF_DISP_MIPI_SPI_LCD_SPI_8BITS_MODE                0
#define VSF_DISP_MIPI_SPI_LCD_SPI_9BITS_MODE                1

#ifndef VSF_DISP_MIPI_SPI_LCD_SPI_MODE
#   define  VSF_DISP_MIPI_SPI_LCD_SPI_MODE                  VSF_DISP_MIPI_SPI_LCD_SPI_8BITS_MODE
#endif

#if     (VSF_DISP_MIPI_SPI_LCD_SPI_MODE != VSF_DISP_MIPI_SPI_LCD_SPI_8BITS_MODE)\
    &&  (VSF_DISP_MIPI_SPI_LCD_SPI_MODE != VSF_DISP_MIPI_SPI_LCD_SPI_9BITS_MODE)
#   error "vsf_disp_mipi_spi_lcd: error spi mode"
#endif

#if VSF_DISP_MIPI_SPI_LCD_SPI_MODE ==  VSF_DISP_MIPI_SPI_LCD_SPI_9BITS_MODE
#   error "TODO: support 9bit spi mode"
#endif

// address
#define MIPI_MODE_X_FLIP                                MIPI_DCS_COLUME_ADDRESS_RIGHT_TO_LEFT
#define MIPI_MODE_Y_FLIP                                MIPI_DCS_PAGE_ADDRESS_BOTTOM_TO_TOP
#define MIPI_MODE_RGB                                   MIPI_DCS_DEVICE_REFRESH_RGB
#define MIPI_MODE_BGR                                   MIPI_DCS_DEVICE_REFRESH_BGR
// pixel format
// pixel bitlen in [3, 8, 12, 16, 18, 24]
#define MIPI_PIXEL_FORMAT_BITLEN(__BITLEN)              MIPI_DCS_PIXEL_FORMAT_DBI_BITS(__BITLEN)
// soft reset
#define MIPI_SOFT_RESET                                 MIPI_DCS_CMD_SOFT_RESET
// sleep sleep
#define MIPI_ENTER_IDLE                                 MIPI_DCS_CMD_HEX_CODE_ENTER_IDLE_MODE
// exit sleep
#define MIPI_EXIT_IDLE                                  MIPI_DCS_CMD_HEX_CODE_EXIT_IDLE_MODE
// sleep sleep
#define MIPI_ENTER_SLEEP                                MIPI_DCS_CMD_ENTER_SLEEP_MODE
// exit sleep
#define MIPI_EXIT_SLEEP                                 MIPI_DCS_CMD_EXIT_SLEEP_MODE
// the part of display
#define MIPI_ENTER_PARTIAL_MODE                         MIPI_DCS_CMD_ENTER_PARTIAL_MODE
// the whole of display
#define MIPI_EXIT_PARTIAL_MODE                          MIPI_DCS_CMD_ENTER_NORMAL_MODE
// inverted colors
#define MIPI_ENTER_INVERT_MODE                          MIPI_DCS_CMD_HEX_CODE_ENTER_INVERT_MODE
// normal colors
#define MIPI_EXIT_INVERT_MODE                           MIPI_DCS_CMD_HEX_CODE_EXIT_INVERT_MODE
// tearing effect output pin on
#define MIPI_TEAR_PIN_ON                                MIPI_DCS_CMD_SET_TEAR_ON
// tearing effect output off
#define MIPI_TEAR_PIN_OFF                               MIPI_DCS_CMD_SET_TEAR_OFF

/**
 \~chinese
 @def VSF_DISP_MIPI_SPI_LCD_INIT(__LCD_SEQ, __PIXEL_FORMAT, __MODE, ...)
 @brief 一个简化 LCD 配置的宏
 @param[in] __LCD_SEQ: 包含一堆预定义命令的宏, 增加新的 LCD 支持应该定义类似的命令，部分现在提供的预定义命令：
            VSF_DISP_MIPI_SPI_LCD_S6D05A1_BASE, VSF_DISP_MIPI_SPI_LCD_ILI9488_BASE, VSF_DISP_MIPI_SPI_LCD_ILI9341_BASE, ...

 @param[in] __PIXEL_FORMAT: LCD 像素位宽，可选值包括： MIPI_PIXEL_FORMAT_BITLEN([3, 8, 12, 16, 18, 24])

 @param[in] __MODE: LCD 的颜色和地址顺序，可以使用简化配置宏
                        MIPI_MODE_X_FLIP, MIPI_MODE_Y_FLIP, MIPI_MODE_RGB, MIPI_MODE_BGR，
                    也可以使用如下完整命令，需要注意，不是所有 LCD 都支持以下所有配置：
                        MIPI_DCS_PAGE_ADDRESS_TOP_TO_BOTTOM
                        MIPI_DCS_PAGE_ADDRESS_BOTTOM_TO_TOP
                        MIPI_DCS_COLUME_ADDRESS_LEFT_TO_RIGHT
                        MIPI_DCS_COLUME_ADDRESS_RIGHT_TO_LEFT
                        MIPI_DCS_PAGE_COLUMN_NORMAL_ORDER
                        MIPI_DCS_PAGE_COLUMN_REVERSE_ORDER
                        MIPI_DCS_DEVICE_REFRESH_TOP_TO_BOTTOM
                        MIPI_DCS_DEVICE_REFRESH_BOTTOM_TO_TOP
                        MIPI_DCS_DEVICE_REFRESH_RGB
                        MIPI_DCS_DEVICE_REFRESH_BGR
                        MIPI_DCS_LCD_REFRESH_LEFT_TO_RIGHT
                        MIPI_DCS_LCD_REFRESH_RIGHT_TO_LEFT
                        MIPI_DCS_FLIP_HORIZONTAL_NORMAL
                        MIPI_DCS_FLIP_HORIZONTAL_FLIPPED

 @param[in] 可选参数，有三种方式填充可选参数
    1. 使用简化命令配置通用寄存器，例如 MIPI_SOFT_RESET, MIPI_ENTER_IDLE, MIPI_EXIT_IDLE, MIPI_EXIT_IDLE, ...
    2. 使用完整命令配置通用寄存器，每一个通用寄存器都有相应的宏，可以参考头文件 vsf_disp_mip_lcd_dcs.h，例子：
         MIPI_DCS_CMD_SET_TEAR_SCANLINE(0x10),  // turns on the display module’s Tearing Effect output signal
                                                // on the TE signal line when the displaymodule reaches line 16
    3. 使用宏 VSF_DISP_MIPI_LCD_WRITE() to 配置部分 LCD 专有寄存器，例如
       VSF_DISP_MIPI_LCD_WRITE(0xB1,  2, 0x00, 0x10),   //  for ST7796S, set 0x81 register with 2 byte parameter

  以下是一些简单的例子：

  // ST7789V with RGB565
  VSF_DISP_MIPI_SPI_LCD_INITSEQ(
    VSF_DISP_MIPI_SPI_LCD_ST7789V_BASE,
    MIPI_PIXEL_FORMAT_BITLEN(16),
    MIPI_MODE_RGB
  )

  // ST7789V with RGB565, tearing effect line on
  VSF_DISP_MIPI_SPI_LCD_INITSEQ(
    VSF_DISP_MIPI_SPI_LCD_ST7789V_BASE,
    MIPI_PIXEL_FORMAT_BITLEN(16),
    MIPI_MODE_RGB,
    MIPI_TEAR_PIN_ON
  )

  // ST7796S with BGR888, X FLIP
  VSF_DISP_MIPI_SPI_LCD_INITSEQ(
    VSF_DISP_MIPI_SPI_LCD_ST7796S_BASE,
    MIPI_PIXEL_FORMAT_BITLEN(24),
    MIPI_MODE_RGB | MIPI_MODE_X_FLIP,
    // Blanking Porch Control
    VSF_DISP_MIPI_LCD_WRITE(0xB1,  2, 0x00, 0x10),
  )
 */
#define VSF_DISP_MIPI_SPI_LCD_INITSEQ(__LCD_SEQ, __PIXEL_FORMAT, __MODE, ...)   \
    __LCD_SEQ,                                                                  \
    MIPI_DCS_CMD_SET_ADDRESS_MODE(__MODE),                                      \
    MIPI_DCS_CMD_SET_PIXEL_FORMAT(__PIXEL_FORMAT),                              \
    MIPI_DCS_CMD_SET_DISPLAY_ON,                                                \
    ##__VA_ARGS__

#define VSF_DISP_MIPI_SPI_LCD_S6D05A1_BASE                                      \
    VSF_DISP_MIPI_LCD_WRITE(0xF0,  2, 0x5A, 0x5A), /*PASSWD1*/                  \
    VSF_DISP_MIPI_LCD_WRITE(0xF1,  2, 0x5A, 0x5A), /*PASSWD2*/                  \
    VSF_DISP_MIPI_LCD_WRITE(0xF2, 19, 0x3B, 0x40, 0x03, 0x04, 0x02, 0x08, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x40, 0x08, 0x08, 0x08, 0x08), /*DISCTL*/ \
    VSF_DISP_MIPI_LCD_WRITE(0xF4, 14, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x03, 0x00, 0x70, 0x03), /*PWRCTL*/ \
    VSF_DISP_MIPI_LCD_WRITE(0xF5, 12, 0x00, 0x54, 0x73, 0x00, 0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x53, 0x71), /*VCMCTL*/ \
    VSF_DISP_MIPI_LCD_WRITE(0xF6,  8, 0x04, 0x00, 0x08, 0x03, 0x01, 0x00, 0x01, 0x00), /*SRCCTL*/ \
    VSF_DISP_MIPI_LCD_WRITE(0xF7,  5, 0x48, 0x80, 0x10, 0x02, 0x00), /*IFCTL*/  \
    VSF_DISP_MIPI_LCD_WRITE(0xF8,  2, 0x11, 0x00), /*PANELCTL*/                 \
    VSF_DISP_MIPI_LCD_WRITE(0xF9,  1, 0x27), /*GAMMASEL*/                       \
    VSF_DISP_MIPI_LCD_WRITE(0xFA, 20, 0x0B, 0x0B, 0x0F, 0x26, 0x2A, 0x30, 0x33, 0x12, 0x1F, 0x25, 0x31, 0x30, 0x24, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x3F) /*PGAMMACTL*/

#define VSF_DISP_MIPI_SPI_LCD_ILI9488_BASE                                      \
    VSF_DISP_MIPI_LCD_WRITE(0xE0, 15, 0x00, 0x07, 0x0f, 0x0D, 0x1B, 0x0A, 0x3c, 0x78, 0x4A, 0x07, 0x0E, 0x09, 0x1B, 0x1e, 0x0f), /* Positive Gamma Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xE1, 15, 0x00, 0x22, 0x24, 0x06, 0x12, 0x07, 0x36, 0x47, 0x47, 0x06, 0x0a, 0x07, 0x30, 0x37, 0x0f), /* Negative Gamma Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xC0, 2,  0x10, 0x10),                /* Power Control 1 */ \
    VSF_DISP_MIPI_LCD_WRITE(0xC1, 1,  0x41),                      /* Power Control 2 */ \
    VSF_DISP_MIPI_LCD_WRITE(0xC5, 3,  0x00, 0x22, 0x80),          /* VCOM Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB0, 1,  0x00),                      /* Interface Mode Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB1, 2,  0x60, 0x11),                /* frame rate control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB4, 1,  0x02),                      /* Display Inversion Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB6, 2,  0x02, 0x02),                /* Display Function Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB7, 1,  0x06),                      /* Entry Mode Set */ \
    VSF_DISP_MIPI_LCD_WRITE(0xE9, 1,  0x00),                      /* Set Image Function: 24bit data bus */ \
    VSF_DISP_MIPI_LCD_WRITE(0xF7, 4,  0xA9, 0x51, 0x2C, 0x82)     /* Adjust Control 3 */

#define VSF_DISP_MIPI_SPI_LCD_ILI9341_BASE                                      \
    VSF_DISP_MIPI_LCD_WRITE(0xCF,  3, 0x00, 0xC1, 0x30),                        \
    VSF_DISP_MIPI_LCD_WRITE(0xED,  4, 0x64, 0x03, 0x12, 0x81),                  \
    VSF_DISP_MIPI_LCD_WRITE(0xE8,  3, 0x85, 0x10, 0x7A),                        \
    VSF_DISP_MIPI_LCD_WRITE(0xCB,  5, 0x39, 0x2C, 0x00, 0x34, 0x02),            \
    VSF_DISP_MIPI_LCD_WRITE(0xF7,  1, 0x20),                                    \
    VSF_DISP_MIPI_LCD_WRITE(0xEA,  2, 0x00, 0x00),                              \
    VSF_DISP_MIPI_LCD_WRITE(0xC0,  1, 0x1B),                    /* Power Control 1 */ \
    VSF_DISP_MIPI_LCD_WRITE(0xC1,  1, 0x01),                    /* Power Control 2 */ \
    VSF_DISP_MIPI_LCD_WRITE(0xC5,  2, 0x30, 0x30),              /* VCOM Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xC7,  1, 0xB7),                    /* VCOM Control 2 */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB1,  2, 0x01, 0x1B),              /* frame rate control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB6,  2, 0x02, 0x82),              /* Display Function Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB5,  4, 0x5F, 0x5F, 0x3F, 0x3F),  /* Blanking Porch Control, VFP/VBP/0x00/HBP */ \
    VSF_DISP_MIPI_LCD_WRITE(0xF2,  1, 0x00),                                    \
    VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_GAMMA_CURVE,  1, 0x01),    /* Gamma Set */  \
    VSF_DISP_MIPI_LCD_WRITE(0xE0, 15, 0x0F, 0x2A, 0x28, 0x08, 0x0E, 0x08, 0x54, 0xA9, 0x43, 0x0A, 0x0F, 0x00, 0x00, 0x00, 0x00), /* Positive Gamma Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xE1, 15, 0x00, 0x15, 0x17, 0x07, 0x11, 0x06, 0x2B, 0x56, 0x3C, 0x05, 0x10, 0x0F, 0x3F, 0x3F, 0x0F)  /* Negative Gamma Control */

#define VSF_DISP_MIPI_SPI_LCD_ST7789V_BASE                                      \
    VSF_DISP_MIPI_LCD_WRITE(0xB1,  3, 0x40, 0x7F, 0x14), /* RGB Interface Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB2,  5, 0x0C, 0x0C, 0x00, 0x33, 0x33), /* Porch Setting */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB7,  1, 0x35), /* Gate Control */                 \
    VSF_DISP_MIPI_LCD_WRITE(0xBB,  1, 0x20), /* VCOM Setting */                 \
    VSF_DISP_MIPI_LCD_WRITE(0xC0,  1, 0x2C), /* LCM Control  */                 \
    VSF_DISP_MIPI_LCD_WRITE(0xC2,  1, 0x01), /* VDV and VRH Command Enable */   \
    VSF_DISP_MIPI_LCD_WRITE(0xC3,  1, 0x0B), /* VRH Set */                      \
    VSF_DISP_MIPI_LCD_WRITE(0xC4,  1, 0x20), /* VDV Set */                      \
    VSF_DISP_MIPI_LCD_WRITE(0xC6,  1, 0x1F), /* Frame Rate Control in Normal Mode, 1F: 39 */ \
    VSF_DISP_MIPI_LCD_WRITE(0xD0,  2, 0xA4, 0xA1), /* Power Control 1 */        \
    VSF_DISP_MIPI_LCD_WRITE(0xE0, 14, 0xD0, 0x03, 0x09, 0x0E, 0x11, 0x3D, 0x47, 0x55, 0x53, 0x1A, 0x16, 0x14, 0x1F, 0x22), /* Positive Voltage Gamma Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xE1, 14, 0xD0, 0x02, 0x08, 0x0D, 0x12, 0x2C, 0x43, 0x55, 0x53, 0x1E, 0x1B, 0x19, 0x20, 0x22)  /* Negative Voltage Gamma Control */

#define VSF_DISP_ST7789V_SPI_INITSEQ(__MODE, __PIXEL_FORMAT, ...)               \
    VSF_DISP_MIPI_SPI_LCD_ST7789V_BASE,                                         \
    MIPI_DCS_CMD_SET_ADDRESS_MODE(__MODE),                                      \
    MIPI_DCS_CMD_SET_PIXEL_FORMAT(__PIXEL_FORMAT),                              \
    MIPI_DCS_CMD_SET_DISPLAY_ON,                                                \
    ##__VA_ARGS__

#define VSF_DISP_MIPI_SPI_LCD_ST7796S_BASE                                      \
    VSF_DISP_MIPI_LCD_WRITE(0xF0,  1, 0xC3), /* Command Set Control, C3h enable command 2 part I */ \
    VSF_DISP_MIPI_LCD_WRITE(0xF0,  1, 0x96), /* Command Set Control, 96h enable command 2 part II */ \
    VSF_DISP_MIPI_LCD_WRITE(0xE8,  8, 0x40, 0x82, 0x07, 0x18, 0x27, 0x0A, 0xB6, 0x33), /* Display Output Ctrl Adjust */ \
    VSF_DISP_MIPI_LCD_WRITE(0xC5,  1, 0x27), /* VCOM Control */                 \
    VSF_DISP_MIPI_LCD_WRITE(0xC2,  1, 0xA7), /* Power Control 3 */              \
    VSF_DISP_MIPI_LCD_WRITE(0xE0, 14, 0xF0, 0x01, 0x06, 0x0F, 0x12, 0x1D, 0x36, 0x54, 0x44, 0x0C, 0x18, 0x16, 0x13, 0x15), /* Positive Gamma Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xE1, 14, 0xF0, 0x01, 0x05, 0x0A, 0x0B, 0x07, 0x32, 0x44, 0x44, 0x0C, 0x18, 0x17, 0x13, 0x16), /* Negative Gamma Control */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB5,  4, 0xFF, 0xFF, 0x00, 0x04), /* Blanking Porch Control, VFP/VBP/0x00/HBP */ \
    VSF_DISP_MIPI_LCD_WRITE(0xB1,  2, 0x00, 0x10), /* Blanking Porch Control, VFP/VBP/0x00/HBP */ \
    VSF_DISP_MIPI_LCD_WRITE(0xF0,  1, 0x3C), /* Command Set Control, 3Ch disable command 2 part I */ \
    VSF_DISP_MIPI_LCD_WRITE(0xF0,  1, 0x69)  /* Command Set Control, 69h disable command 2 part II */

#define VSF_DISP_MIPI_SPI_LCD_REFRESH_SEQ_LEN                                   \
    (1 + 1 + 4) + (1 + 1 + 4) + (1 + 1 + 4 + 4)

/*============================ TYPES =========================================*/

vsf_class(vk_disp_mipi_spi_lcd_t) {
    public_member(
        implement(vk_disp_t)
        vsf_spi_t       *spi;
        uint32_t         clock_hz;
        const uint8_t   *init_seq;
        const uint16_t   init_seq_len;
#if VSF_DISP_MIPI_SPI_LCD_USING_VSF_GPIO == ENABLED
        struct {
            vsf_gpio_t * gpio;
            uint32_t     pin_mask;
        } reset, dcx;
#endif
    )

    private_member(
        vsf_teda_t           teda;

        vk_disp_area_t      area;
        uint8_t             *cur_buffer;

        uint8_t             refresh_seq[VSF_DISP_MIPI_SPI_LCD_REFRESH_SEQ_LEN];

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

extern const vk_disp_drv_t vk_disp_drv_mipi_spi_lcd;

/*============================ PROTOTYPES ====================================*/

// return true if te is not supported, or return false and call vsf_disp_mipi_te_line_ready when te is ready
extern bool vsf_disp_mipi_spi_lcd_wait_te_line_ready(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd);
extern void vsf_disp_mipi_te_line_ready(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd);

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_MIPI_SPI_LCD_H__
