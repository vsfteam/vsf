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

/**
 \~chinese
 @def VSF_DISP_MIPI_SPI_LCD_INITSEQ(__LCD_SEQ, __PIXEL_FORMAT, __MODE, ...)
 @brief 一个简化 LCD 初始化序列的宏
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
    VSF_DISP_MIPI_LCD_INITSEQ(__LCD_SEQ,                                        \
        MIPI_DCS_CMD_SET_ADDRESS_MODE(__MODE),                                  \
        MIPI_DCS_CMD_SET_PIXEL_FORMAT(__PIXEL_FORMAT),                          \
        ##__VA_ARGS__                                                           \
    )

#define VSF_DISP_MIPI_SPI_LCD_REFRESH_SEQ_LEN                                   \
    (1 + 1 + 4) + (1 + 1 + 4) + (1 + 1 + 4 + 4)


// MACROs for specified LCD

#define VSF_DISP_ST7789V_SPI_INITSEQ(__PIXEL_FORMAT, __MODE, ...)               \
    VSF_DISP_MIPI_SPI_LCD_INITSEQ(VSF_DISP_MIPI_LCD_ST7789V_BASE,               \
        __PIXEL_FORMAT, __MODE,                                                 \
        ##__VA_ARGS__                                                           \
    )

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
