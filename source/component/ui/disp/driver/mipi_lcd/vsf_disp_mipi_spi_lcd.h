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

#include "./vsf_disp_mipi_lcd_dcs.h"

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

#ifndef VSF_DISP_MIPI_SPI_LCD_CFG_WIDTH
#   define VSF_DISP_MIPI_SPI_LCD_CFG_WIDTH                  240
#endif

#ifndef VSF_DISP_MIPI_SPI_LCD_CFG_HEIGTH
#   define VSF_DISP_MIPI_SPI_LCD_CFG_HEIGTH                 320
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

// pixel bitlen in [3, 8, 12, 16, 18, 24]
#define MIPI_PIXEL_FORMAT_BITLEN(__BITLEN)              MIPI_DCS_PIXEL_FORMAT_DBI_BITS(__BITLEN)

#define MIPI_MODE_X_FLIP                                MIPI_DCS_COLUME_ADDRESS_RIGHT_TO_LEFT
#define MIPI_MODE_RGB                                   MIPI_DCS_DEVICE_REFRESH_RGB
#define MIPI_MODE_BGR                                   MIPI_DCS_DEVICE_REFRESH_BRT

#define VSF_DISP_MIPI_SPI_LCD_INIT_MODE_AND_FORMAT(__MODE, __PIXEL_FORMAT)      \
    MIPI_DCS_CMD_SET_ADDRESS_MODE(__MODE),                                      \
    MIPI_DCS_CMD_SET_PIXEL_FORMAT(__PIXEL_FORMAT),                              \
    MIPI_DCS_CMD_SET_DISPLAY_ON                                                 \
    //MIPI_DCS_CMD_SET_TEAR_ON /* Tearing Effect Line ON, for V-Blanking information only */

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
    VSF_DISP_MIPI_LCD_WRITE(0xB0, 1,  0x00),                      /* Interface Mode Contro */ \
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

#define VSF_DISP_ST7789V_SPI_INITSEQ(__MODE, __PIXEL_FORMAT)                    \
    VSF_DISP_MIPI_SPI_LCD_ST7789V_BASE,                                         \
    VSF_DISP_MIPI_SPI_LCD_INIT_MODE_AND_FORMAT(__MODE, __PIXEL_FORMAT)

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
        vsf_eda_evthandler_t evthandler;

        vk_disp_area_t      area;
        uint8_t             *cur_buffer;

        uint8_t             refresh_seq[VSF_DISP_MIPI_SPI_LCD_REFRESH_SEQ_LEN];
#if VSF_DISP_MIPI_SPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
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

extern const vk_disp_drv_t vk_disp_drv_mipi_spi_lcd;

/*============================ PROTOTYPES ====================================*/

extern void vsf_disp_mipi_spi_lcd_wait_te_line_ready(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd);

#if VSF_DISP_MIPI_SPI_LCD_SPI_MODE == VSF_DISP_MIPI_SPI_LCD_SPI_8BITS_MODE
extern void vsf_disp_mipi_spi_lcd_dcx_io_write(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd, bool state);
#endif

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_MIPI_SPI_LCD_H__
