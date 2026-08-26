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
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_CH1115 == ENABLED

#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_DISP_CH1115_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../../vsf_disp.h"
#include "./vsf_disp_ch1115.h"

/*============================ MACROS ========================================*/

#ifndef VSF_CH1115_CFG_ISR_PRIO
#   define VSF_CH1115_CFG_ISR_PRIO          vsf_arch_prio_0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_EVT_REFRESH = VSF_EVT_USER,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_disp_ch1115_init(vk_disp_t *pthis);
static vsf_err_t __vk_disp_ch1115_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_ch1115 = {
    .init           = __vk_disp_ch1115_init,
    .refresh        = __vk_disp_ch1115_refresh,
};

/*============================ IMPLEMENTATION ================================*/

// Advance the page-addressed refresh: send the per-page command header, then
// the page data block; on the last page, report completion. Runs in the I2C
// ISR (via VSF_EVT_RETURN) and in the refresh() caller context.
static void __vk_disp_ch1115_refresh_next(vk_disp_ch1115_t *disp_ch1115)
{
    if (!disp_ch1115->is_refreshing) {
        return;
    }

    if (!disp_ch1115->is_header_sent) {
        if (disp_ch1115->ctx.refresh.cur_page >= disp_ch1115->ctx.refresh.end_page) {
            disp_ch1115->is_refreshing = false;
            disp_ch1115->ctx.refresh.buffer = NULL;
            vk_disp_on_ready(&disp_ch1115->use_as__vk_disp_t);
            return;
        }
        uint8_t col = (uint8_t)disp_ch1115->ctx.refresh.area.pos.x;
        disp_ch1115->page_cmd_buffer[0] = 0x80;
        disp_ch1115->page_cmd_buffer[1] = CH1115_SET_PAGE_ADDRESS(disp_ch1115->ctx.refresh.cur_page);
        disp_ch1115->page_cmd_buffer[2] = 0x80;
        disp_ch1115->page_cmd_buffer[3] = CH1115_SET_COLUMN_LOW(col);
        disp_ch1115->page_cmd_buffer[4] = 0x80;
        disp_ch1115->page_cmd_buffer[5] = CH1115_SET_COLUMN_HIGH(col);
        disp_ch1115->page_cmd_buffer[6] = 0x40;
        disp_ch1115->is_header_sent = true;
        vsf_i2c_master_request(disp_ch1115->hw.port, CH1115_I2C_ADDR,
            VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE,
            sizeof(disp_ch1115->page_cmd_buffer), disp_ch1115->page_cmd_buffer);
    } else {
        uint8_t page_idx = disp_ch1115->ctx.refresh.cur_page
                         - (uint8_t)disp_ch1115->ctx.refresh.area.pos.y;
        uint8_t *data = disp_ch1115->ctx.refresh.buffer
                      + page_idx * disp_ch1115->ctx.refresh.area.size.x;
        disp_ch1115->is_header_sent = false;
        disp_ch1115->ctx.refresh.cur_page++;
        vsf_i2c_master_request(disp_ch1115->hw.port, CH1115_I2C_ADDR,
            VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE,
            disp_ch1115->ctx.refresh.area.size.x, data);
    }
}

static void __vk_disp_ch1115_state_machine(vk_disp_ch1115_t *disp_ch1115, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        disp_ch1115->is_inited = false;
        disp_ch1115->is_refreshing = false;
        disp_ch1115->is_header_sent = false;

        vsf_i2c_master_request(disp_ch1115->hw.port, CH1115_I2C_ADDR,
            VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE,
            disp_ch1115->init_seq_len, (uint8_t *)disp_ch1115->init_seq);
        break;
    case VSF_EVT_RETURN:
        if (!disp_ch1115->is_inited) {
            disp_ch1115->is_inited = true;
            vk_disp_on_ready(&disp_ch1115->use_as__vk_disp_t);
            break;
        }
        __vk_disp_ch1115_refresh_next(disp_ch1115);
        break;
    case VSF_EVT_REFRESH:
        if (disp_ch1115->is_inited && disp_ch1115->ctx.refresh.buffer != NULL) {
            disp_ch1115->is_refreshing = true;
            disp_ch1115->is_header_sent = false;
            disp_ch1115->ctx.refresh.cur_page = (uint8_t)disp_ch1115->ctx.refresh.area.pos.y;
            disp_ch1115->ctx.refresh.end_page = (uint8_t)(disp_ch1115->ctx.refresh.area.pos.y
                                              + disp_ch1115->ctx.refresh.area.size.y);
            __vk_disp_ch1115_refresh_next(disp_ch1115);
        }
        break;
    }
}

static void __ch1115_i2c_isr(void *target, vsf_i2c_t *i2c, vsf_i2c_irq_mask_t mask)
{
    vk_disp_ch1115_t *disp_ch1115 = (vk_disp_ch1115_t *)target;
    __vk_disp_ch1115_state_machine(disp_ch1115, VSF_EVT_RETURN);
}

static vsf_err_t __vk_disp_ch1115_init(vk_disp_t *pthis)
{
    vk_disp_ch1115_t *disp_ch1115 = (vk_disp_ch1115_t *)pthis;
    VSF_UI_ASSERT(disp_ch1115 != NULL);

    disp_ch1115->ctx.refresh.buffer = NULL;

    // NOTE: the CH1115 module has no RES pin (module-internal power-on
    // reset). vk_disp_init() must run some milliseconds after the module is
    // powered so the RC reset has finished; on this platform display init
    // happens in the application start path, long after power-on.
    vsf_i2c_cfg_t cfg = {
        .mode = VSF_I2C_MODE_MASTER | VSF_I2C_ADDR_7_BITS,
        .clock_hz = disp_ch1115->clock_hz,
        .isr = {
            .handler_fn = __ch1115_i2c_isr,
            .target_ptr = disp_ch1115,
            .prio = VSF_CH1115_CFG_ISR_PRIO,
        },
    };
    vsf_err_t err = vsf_i2c_init(disp_ch1115->hw.port, &cfg);
    if (err != VSF_ERR_NONE) {
        return err;
    }
    vsf_i2c_irq_enable(disp_ch1115->hw.port, VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);

    __vk_disp_ch1115_state_machine(disp_ch1115, VSF_EVT_INIT);
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_disp_ch1115_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_ch1115_t *disp_ch1115 = (vk_disp_ch1115_t *)pthis;
    VSF_UI_ASSERT((disp_ch1115 != NULL) && (area != NULL) && (disp_buff != NULL));
    VSF_UI_ASSERT(!(area->size.y & 0x07) && !(area->pos.y & 0x07));

    disp_ch1115->ctx.refresh.area = *area;
    disp_ch1115->ctx.refresh.area.pos.y >>= 3;
    disp_ch1115->ctx.refresh.area.size.y >>= 3;
    disp_ch1115->ctx.refresh.buffer = disp_buff;
    __vk_disp_ch1115_state_machine(disp_ch1115, VSF_EVT_REFRESH);
    return VSF_ERR_NONE;
}

#endif

/* EOF */
