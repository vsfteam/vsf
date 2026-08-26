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
 ****************************************************************************/
#ifndef __VSF_DISP_CH1115_H__
#define __VSF_DISP_CH1115_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_CH1115 == ENABLED

#include "hal/vsf_hal.h"

#if     defined(__VSF_DISP_CH1115_CLASS_IMPLEMENT)
#   undef __VSF_DISP_CH1115_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// CH1115 (Chip Wealth) supports PAGE addressing ONLY — there is no 0x20/0x21/
// 0x22 memory-addressing-mode/window command set (CH1115 datasheet V0.3,
// command table). The refresh path below therefore walks the area page by
// page: [B0|page][column low][column high] then a 0x40 data block, relying on
// the column address auto-increment within a page.
#define CH1115_I2C_ADDR                             0x3C

#define CH1115_SET_COLUMN_LOW(__COL)                (0x00 | ((__COL) & 0x0F))
#define CH1115_SET_COLUMN_HIGH(__COL)               (0x10 | (((__COL) >> 4) & 0x0F))
#define CH1115_SET_PAGE_ADDRESS(__PAGE)             (0xB0 | ((__PAGE) & 0x07))

// init_seq byte-stream control bytes (Co/D#C), same protocol as SSD1306:
// 0x80 = command, more control bytes follow; 0x40 = data bytes follow.
#define VSF_DISP_CH1115_IIC_WRITE_I(__INSTR)                            \
            0x80, (__INSTR)
#define VSF_DISP_CH1115_IIC_WRITE_D(__DATA)                             \
            0x40, (__DATA)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_disp_ch1115_hw_iic_t {
    vsf_i2c_t *port;
} vsf_disp_ch1115_hw_iic_t;

vsf_class(vk_disp_ch1115_t) {
    public_member(
        implement(vk_disp_t)

        implement_ex(vsf_disp_ch1115_hw_iic_t, hw)
        const uint8_t *init_seq;
        const uint16_t init_seq_len;
        uint32_t clock_hz;
    )

    private_member(
        struct {
            struct {
                // pos.y / size.y are in PAGES (refresh() shifts them down)
                vk_disp_area_t area;
                uint8_t *buffer;
                uint8_t cur_page;
                uint8_t end_page;
            } refresh;
        } ctx;

        bool is_inited;
        bool is_refreshing;
        bool is_header_sent;
        // [0x80, B0|page, 0x80, col low, 0x80, col high, 0x40]
        uint8_t page_cmd_buffer[7];
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_disp_drv_t vk_disp_drv_ch1115;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI == ENABLED && VSF_DISP_USE_CH1115 == ENABLED
#endif  // __VSF_DISP_CH1115_H__
