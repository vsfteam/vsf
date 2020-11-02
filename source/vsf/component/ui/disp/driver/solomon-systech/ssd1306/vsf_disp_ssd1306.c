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

static void __vk_disp_ssd1306_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_disp_ssd1306_t *disp_ssd1306 = container_of(eda, vk_disp_ssd1306_t, eda);

    switch (evt) {
    case VSF_EVT_INIT:
        disp_ssd1306->is_inited = false;
        disp_ssd1306->is_area_set = false;

#if VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_IIC
        vsf_multiplex_iic_write(disp_ssd1306->hw.port, 0x3C, (uint8_t *)disp_ssd1306->init_seq, disp_ssd1306->init_seq_len, 0);
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_SPI
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_EBI
#endif
        break;
    case VSF_EVT_RETURN:
        if (!disp_ssd1306->is_inited) {
            disp_ssd1306->is_inited = true;
        }
        // fall through
    case VSF_EVT_REFRESH:
        if (disp_ssd1306->is_inited) {
            if (disp_ssd1306->ctx.refresh.buffer && !disp_ssd1306->is_area_set) {
                disp_ssd1306->is_area_set = true;

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
                vsf_multiplex_iic_write(disp_ssd1306->hw.port, 0x3C, disp_ssd1306->set_area_cmd_buffer, sizeof(iic_seq_buffer), 0);
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_SPI
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_EBI
#endif
            } else if (disp_ssd1306->is_area_set) {
                if (disp_ssd1306->ctx.refresh.buffer != NULL) {
#if VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_IIC
                    vsf_multiplex_iic_write(disp_ssd1306->hw.port, 0x3C, disp_ssd1306->ctx.refresh.buffer,
                        disp_ssd1306->ctx.refresh.area.size.x * disp_ssd1306->ctx.refresh.area.size.y * 2, 0);
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_SPI
#elif VSF_SSD1306_CFG_PORT == VSF_SSD1306_PORT_EBI
#endif
                    disp_ssd1306->ctx.refresh.buffer = NULL;
                } else {
                    disp_ssd1306->is_area_set = false;
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
    return vsf_eda_init(&disp_ssd1306->eda, vsf_prio_inherit, false);
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
