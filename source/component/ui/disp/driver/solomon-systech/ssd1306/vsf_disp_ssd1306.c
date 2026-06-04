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

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_SSD1306 == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_DISP_SSD1306_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../../vsf_disp.h"
#include "./vsf_disp_ssd1306.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_EVT_REFRESH = VSF_EVT_USER,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_disp_ssd1306_init(vk_disp_t *pthis);
static vsf_err_t __vk_disp_ssd1306_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_ssd1306 = {
    .init           = __vk_disp_ssd1306_init,
    .refresh        = __vk_disp_ssd1306_refresh,
};

/*============================ IMPLEMENTATION ================================*/

static void __ssd1306_i2c_isr(void *target, vsf_i2c_t *i2c, vsf_i2c_irq_mask_t mask)
{
    vsf_eda_post_evt((vsf_eda_t *)target, VSF_EVT_RETURN);
}

static void __vk_disp_ssd1306_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_disp_ssd1306_t *disp_ssd1306 = vsf_container_of(eda, vk_disp_ssd1306_t, eda);

    switch (evt) {
    case VSF_EVT_INIT:
        disp_ssd1306->is_inited = false;
        disp_ssd1306->is_area_set = false;
        disp_ssd1306->is_ctrl_sent = false;

#if VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_IIC
        vsf_i2c_master_request(disp_ssd1306->hw.port, 0x3C,
            VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE,
            disp_ssd1306->init_seq_len, (uint8_t *)disp_ssd1306->init_seq);
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_SPI
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_EBI
#endif
        break;
    case VSF_EVT_RETURN:
        if (!disp_ssd1306->is_inited) {
            disp_ssd1306->is_inited = true;
            vk_disp_on_ready(&disp_ssd1306->use_as__vk_disp_t);
            break;
        }
        // fall through
    case VSF_EVT_REFRESH:
        if (disp_ssd1306->is_inited) {
            if (disp_ssd1306->ctx.refresh.buffer && !disp_ssd1306->is_area_set) {
                disp_ssd1306->is_area_set = true;
                disp_ssd1306->is_ctrl_sent = false;

#if VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_IIC
                uint8_t iic_seq_buffer[] = {
                    VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_COLUMN_ADDRESS),
                    VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(disp_ssd1306->ctx.refresh.area.pos.x),
                    VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(disp_ssd1306->ctx.refresh.area.pos.x + disp_ssd1306->ctx.refresh.area.size.x - 1),
                    VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(SSD1306_SET_PAGE_ADDRESS),
                    VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(disp_ssd1306->ctx.refresh.area.pos.y),
                    VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(disp_ssd1306->ctx.refresh.area.pos.y + disp_ssd1306->ctx.refresh.area.size.y - 1),
                };
                VSF_UI_ASSERT(sizeof(disp_ssd1306->set_area_cmd_buffer) >= sizeof(iic_seq_buffer));
                memcpy(disp_ssd1306->set_area_cmd_buffer, iic_seq_buffer, sizeof(iic_seq_buffer));
                vsf_i2c_master_request(disp_ssd1306->hw.port, 0x3C,
                    VSF_I2C_CMD_START | VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE,
                    sizeof(iic_seq_buffer), disp_ssd1306->set_area_cmd_buffer);
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_SPI
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_EBI
#endif
            } else if (disp_ssd1306->is_area_set) {
                if (disp_ssd1306->ctx.refresh.buffer != NULL) {
#if VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_IIC
                    if (!disp_ssd1306->is_ctrl_sent) {
                        disp_ssd1306->is_ctrl_sent = true;
                        disp_ssd1306->ctrl_byte = 0x40;
                        vsf_i2c_master_request(disp_ssd1306->hw.port, 0x3C,
                            VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE,
                            1, &disp_ssd1306->ctrl_byte);
                    } else {
                        uint16_t total = disp_ssd1306->ctx.refresh.area.size.x
                                       * disp_ssd1306->ctx.refresh.area.size.y;
                        vsf_i2c_master_request(disp_ssd1306->hw.port, 0x3C,
                            VSF_I2C_CMD_STOP | VSF_I2C_CMD_WRITE,
                            total, disp_ssd1306->ctx.refresh.buffer);
                        disp_ssd1306->is_area_set = false;
                        disp_ssd1306->is_ctrl_sent = false;
                        disp_ssd1306->ctx.refresh.buffer = NULL;
                        vk_disp_on_ready(&disp_ssd1306->use_as__vk_disp_t);
                    }
#endif
                } else {
                    disp_ssd1306->is_area_set = false;
                    disp_ssd1306->is_ctrl_sent = false;
                    vk_disp_on_ready(&disp_ssd1306->use_as__vk_disp_t);
                }
            }
        }
        break;
    }
}

static vsf_err_t __vk_disp_ssd1306_init(vk_disp_t *pthis)
{
    vk_disp_ssd1306_t *disp_ssd1306 = (vk_disp_ssd1306_t *)pthis;
    VSF_UI_ASSERT(disp_ssd1306 != NULL);

    disp_ssd1306->ctx.refresh.buffer = NULL;
    disp_ssd1306->eda.fn.evthandler = __vk_disp_ssd1306_evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    disp_ssd1306->eda.on_terminate = NULL;
#endif

#if VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_IIC
    vsf_i2c_cfg_t cfg = {
        .mode = VSF_I2C_MODE_MASTER | VSF_I2C_ADDR_7_BITS,
        .clock_hz = disp_ssd1306->clock_hz,
        .isr = {
            .handler_fn = __ssd1306_i2c_isr,
            .target_ptr = &disp_ssd1306->eda,
            .prio = vsf_arch_prio_0,
        },
    };
    vsf_err_t err = vsf_i2c_init(disp_ssd1306->hw.port, &cfg);
    if (err != VSF_ERR_NONE) {
        return err;
    }
    vsf_i2c_irq_enable(disp_ssd1306->hw.port, VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);
#endif

    return vsf_eda_init(&disp_ssd1306->eda);
}

static vsf_err_t __vk_disp_ssd1306_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_ssd1306_t *disp_ssd1306 = (vk_disp_ssd1306_t *)pthis;
    VSF_UI_ASSERT((disp_ssd1306 != NULL) && (area != NULL) && (disp_buff != NULL));
    VSF_UI_ASSERT(!(area->size.y & 0x07) && !(area->pos.y & 0x07));

    disp_ssd1306->ctx.refresh.area = *area;
    disp_ssd1306->ctx.refresh.area.pos.y >>= 3;
    disp_ssd1306->ctx.refresh.area.size.y >>= 3;
    disp_ssd1306->ctx.refresh.buffer = disp_buff;
    return vsf_eda_post_evt(&disp_ssd1306->eda, VSF_EVT_REFRESH);
}

#endif

/* EOF */
