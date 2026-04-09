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

/** \brief SPI CS index used by default weak cs_active/cs_inactive (e.g. 0 for NSS0). Override weak functions for GPIO CS. */
#ifndef VSF_DISP_MIPI_SPI_LCD_CFG_CS_INDEX
#   define VSF_DISP_MIPI_SPI_LCD_CFG_CS_INDEX               0
#endif

/** \brief Panel column/row offset for CASET/RASET (e.g. ST7735 80x160: x=26, y=1). Undef = 0. */
#ifndef VSF_DISP_MIPI_SPI_LCD_CFG_X_OFFSET
#   define VSF_DISP_MIPI_SPI_LCD_CFG_X_OFFSET               0
#endif
#ifndef VSF_DISP_MIPI_SPI_LCD_CFG_Y_OFFSET
#   define VSF_DISP_MIPI_SPI_LCD_CFG_Y_OFFSET               0
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
 @def VSF_DISP_MIPI_SPI_LCD_INITSEQ(__LCD_SEQ, ...)
 @brief 一个 LCD SPI 初始化序列拼接宏
 @param[in] __LCD_SEQ: 包含一堆预定义命令的宏, 增加新的 LCD 支持应该定义类似的命令，部分现在提供的预定义命令：
            VSF_DISP_MIPI_SPI_LCD_S6D05A1_BASE, VSF_DISP_MIPI_SPI_LCD_ILI9488_BASE, VSF_DISP_MIPI_SPI_LCD_ILI9341_BASE, ...

 @param[in] 可选参数: 由调用者自行按需填充初始化命令，顺序不受固定位置参数限制。常见写法包括：
    1. 使用完整命令配置通用寄存器，例如：
         MIPI_SET_PIXEL_BITLEN(16)
         MIPI_SET_ADDRESS_MODE(MIPI_MODE_RGB | MIPI_MODE_X_FLIP)
         MIPI_DCS_CMD_SET_TEAR_SCANLINE(0x10)
    2. 使用简化命令配置通用寄存器，例如 MIPI_SOFT_RESET, MIPI_ENTER_IDLE, MIPI_EXIT_IDLE, MIPI_TEAR_PIN_ON, ...
    3. 使用宏 VSF_DISP_MIPI_LCD_WRITE() 配置部分 LCD 专有寄存器，例如
       VSF_DISP_MIPI_LCD_WRITE(0xB1,  2, 0x00, 0x10),   // for ST7796S, set 0xB1 register with 2 byte parameter

  以下是一些简单的例子：

  // ST7789V with RGB565
  VSF_DISP_MIPI_SPI_LCD_INITSEQ(
    VSF_DISP_MIPI_LCD_ST7789V_BASE,
    MIPI_SET_ADDRESS_MODE(MIPI_MODE_RGB),
    MIPI_SET_PIXEL_BITLEN(16)
  )

  // ST7789V with RGB565, tearing effect line on
  VSF_DISP_MIPI_SPI_LCD_INITSEQ(
    VSF_DISP_MIPI_LCD_ST7789V_BASE,
    MIPI_SET_ADDRESS_MODE(MIPI_MODE_RGB),
    MIPI_SET_PIXEL_BITLEN(16),
    MIPI_TEAR_PIN_ON
  )

  // ST7796S with BGR888, X FLIP
  VSF_DISP_MIPI_SPI_LCD_INITSEQ(
    VSF_DISP_MIPI_LCD_ST7796S_BASE,
    MIPI_SET_ADDRESS_MODE(MIPI_MODE_RGB | MIPI_MODE_X_FLIP),
    MIPI_SET_PIXEL_BITLEN(24),
    // Blanking Porch Control
    VSF_DISP_MIPI_LCD_WRITE(0xB1,  2, 0x00, 0x10),
  )
 */
#define VSF_DISP_MIPI_SPI_LCD_INITSEQ(__LCD_SEQ, ...)                            \
    VSF_DISP_MIPI_LCD_INITSEQ(__LCD_SEQ,                                         \
        ##__VA_ARGS__                                                            \
    )


#define VSF_DISP_MIPI_SPI_LCD_REFRESH_SEQ_LEN                                   \
    (1 + 1 + 4) + (1 + 1 + 4) + (1 + 1 + 4 + 4)


// MACROs for specified LCD

#define VSF_DISP_ST7789V_SPI_INITSEQ(...)                                        \
    VSF_DISP_MIPI_SPI_LCD_INITSEQ(VSF_DISP_MIPI_LCD_ST7789V_BASE,                \
        ##__VA_ARGS__                                                            \
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
        } dcx;
#if VSF_DISP_MIPI_SPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
        struct {
            vsf_gpio_t * gpio;
            uint32_t     pin_mask;
        } reset;
#endif
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

/**
 * \brief Chip-select control (weak, override for GPIO CS or custom index).
 * Default implementation uses vsf_spi_cs_active/inactive with VSF_DISP_MIPI_SPI_LCD_CFG_CS_INDEX.
 */
extern void vsf_disp_mipi_spi_lcd_cs_active(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd);
extern void vsf_disp_mipi_spi_lcd_cs_inactive(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd);

// return true if TE is not supported, or return false and call vsf_disp_mipi_te_line_ready when TE is ready
extern bool vsf_disp_mipi_spi_lcd_wait_te_line_ready(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd);
extern void vsf_disp_mipi_te_line_ready(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd);

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_MIPI_SPI_LCD_H__
