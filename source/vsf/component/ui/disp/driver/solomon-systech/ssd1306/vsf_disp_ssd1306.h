/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
#ifndef __VSF_DISP_SSD1306_H__
#define __VSF_DISP_SSD1306_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_SSD1306 == ENABLED

#include "../common/vsf_disp_solomon_systech_common.h"

#if     defined(__VSF_DISP_SSD1306_CLASS_IMPLEMENT)
#   undef __VSF_DISP_SSD1306_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_SSD1306_PORT_IIC                            0
#define VSF_SSD1306_PORT_SPI                            1
#define VSF_SSD1306_PORT_EBI                            2

#ifndef VSF_SSD1306_CFG_PORT
#   define VSF_SSD1306_CFG_PORT                         VSF_SSD1306_PORT_IIC
#endif

#define SSD1306_SET_DISPLAY_ON(__ON)                    (0xAE | ((__ON) ? 1 : 0))
#define SSD1306_SET_DISPLAY_START_LINE(__LINE)          (0x40 | ((__LINE) & 0x3F))
#define SSD1306_SET_POAGE_START_ADDRESS(__PAGE)         (0xB0 | ((__PAGE) & 0x07))
#define SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION(__DIR)    (0xC0 | ((__DIR) ? 0x08 : 0x00))
#define SSD1306_SET_CONTRAST_CONTROL                    (0x81)
#define SSD1306_SET_SEGMENT_REMAP(__REMAP)              (0xA0 | ((__REMAP) ? 1 : 0))
#define SSD1306_SET_INVERSE_DISPLAY(__INVERSE)          (0xA6 | ((__INVERSE) ? 1 : 0))
#define SSD1306_SET_MULTIPLEX_RATIO                     (0xA8)
#define SSD1306_SET_DISPLAY_OFFSET                      (0xD3)
#define SSD1306_SET_OSC_DIV                             (0xD5)
#define SSD1306_SET_PRECHARGE_PERIOD                    (0xD9)
#define SSD1306_SET_COM_PIN                             (0xDA)
#define SSD1306_SET_VCOMH                               (0xDB)
#define SSD1306_CHARGE_PUMP                             (0x8D)

#define SSD1306_SET_ADDRESS_MODE                        (0x20)
#   define SSD1306_ADDRESS_MODE_HORIZONTAL              (0x00)
#   define SSD1306_ADDRESS_MODE_VERTICAL                (0x01)
#   define SSD1306_ADDRESS_MODE_PAGE                    (0x02)
#define SSD1306_SET_COLUMN_ADDRESS                      (0x21)
#define SSD1306_SET_PAGE_ADDRESS                        (0x22)

/*============================ MACROFIED FUNCTIONS ===========================*/

// TODO: add more patameters
#if VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_IIC
#   define VSF_DISP_SSD1306_IIC_INIT_SEQ(__LINE, __CONTRAST, __INVERSE, __DIRECTION)\
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_DISPLAY_ON(0)),    \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_DISPLAY_START_LINE(0)),\
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION(__DIRECTION)),\
                                                                                \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_CONTRAST_CONTROL), \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(__CONTRAST),                   \
                                                                                \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_SEGMENT_REMAP(1)), \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_INVERSE_DISPLAY(__INVERSE)),\
                                                                                \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_MULTIPLEX_RATIO),  \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I((__LINE) - 1),                 \
                                                                                \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_DISPLAY_OFFSET),   \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(0),                            \
                                                                                \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_OSC_DIV),          \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(0xF0),                         \
                                                                                \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_PRECHARGE_PERIOD), \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(0x22),                         \
                                                                                \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_COM_PIN),          \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(0x02),                         \
                                                                                \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_VCOMH),            \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(0x20),                         \
                                                                                \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_CHARGE_PUMP),          \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(0x15),                         \
                                                                                \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_DISPLAY_ON(1)),    \
                                                                                \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_ADDRESS_MODE),     \
            VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_ADDRESS_MODE_HORIZONTAL)
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_SPI
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_EBI
#endif

/*============================ TYPES =========================================*/

dcl_simple_class(vk_disp_ssd1306_t)

def_simple_class(vk_disp_ssd1306_t) {
    public_member(
        implement(vk_disp_t)

#if VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_IIC
        implement_ex(vsf_disp_solomon_systech_hw_iic_t, hw)
        const uint8_t *init_seq;
        const uint16_t init_seq_len;
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_SPI
        implement_ex(vsf_disp_solomon_systech_hw_spi_t, hw)
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_EBI
        implement_ex(vsf_disp_solomon_systech_hw_ebi_t, hw)
#endif
    )

    private_member(
        vsf_eda_t eda;
        struct {
            struct {
                vk_disp_area_t area;
                uint8_t *buffer;
            } refresh;
        } ctx;

        bool is_inited;
        bool is_area_set;
#if VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_IIC
        uint8_t set_area_cmd_buffer[6 * 2];
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_SPI
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_EBI
#endif
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_disp_drv_t vk_disp_drv_ssd1306;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_SSD1306_H__
