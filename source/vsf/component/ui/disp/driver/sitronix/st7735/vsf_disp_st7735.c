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

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_ST7735 == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_DISP_SITRONIX_CLASS_INHERIT__
#define __VSF_DISP_ST7735_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_disp.h"
#include "./vsf_disp_st7735.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_EVT_REFRESH = VSF_EVT_USER,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_disp_st7735_init(vk_disp_t *pthis);
static vsf_err_t __vk_disp_st7735_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_st7735 = {
    .init           = __vk_disp_st7735_init,
    .refresh        = __vk_disp_st7735_refresh,
};

/*============================ IMPLEMENTATION ================================*/

static void __vk_disp_st3375_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_disp_st7735_t *disp_st7735 = container_of(eda, vk_disp_st7735_t, teda);

    switch (evt) {
    case VSF_EVT_INIT:
        disp_st7735->state = 0;
        disp_st7735->is_inited = false;
        disp_st7735->is_area_set = false;

        vk_disp_sitronix_init(&disp_st7735->use_as__vk_disp_sitronix_t);
        vk_disp_sitronix_reset(&disp_st7735->use_as__vk_disp_sitronix_t, true);
        vsf_teda_set_timer_ms(10);
        break;
    case VSF_EVT_TIMER:
        if (!disp_st7735->is_inited) {
            switch (disp_st7735->state) {
            case 1:
                vk_disp_sitronix_reset(&disp_st7735->use_as__vk_disp_sitronix_t, false);
                vsf_teda_set_timer_ms(120);
                break;
            case 2:
                vk_disp_sitronix_write(&disp_st7735->use_as__vk_disp_sitronix_t, ST7735_SLPOUT, NULL, 0);
                break;
            case 4:
                // start init seq
                vk_disp_sitronix_seq(&disp_st7735->use_as__vk_disp_sitronix_t, (uint8_t *)disp_st7735->init_seq, disp_st7735->init_seq_len);
                break;
            }
        } else {
            VSF_UI_ASSERT(false);
        }
        break;
    case VSF_EVT_RETURN:
        if (!disp_st7735->is_inited) {
            switch (disp_st7735->state) {
            case 3:
                vsf_teda_set_timer_ms(120);
                break;
            case 5:
                disp_st7735->is_inited = true;
                goto refresh;
            }
        } else {
            goto refresh_evt_return;
        }
        break;
    case VSF_EVT_REFRESH:
    refresh:
        if (disp_st7735->is_inited) {
        refresh_evt_return:
            vk_disp_area_t *area = &disp_st7735->ctx.refresh.area;
            if (disp_st7735->ctx.refresh.buffer && !disp_st7735->is_area_set) {
                uint8_t cmd[] = {
                    VSF_DISP_SITRONIX_WRITE(ST7735_CASET, 4,
                        0x00, disp_st7735->colume_offset + area->pos.x,
                        0x00, disp_st7735->colume_offset + area->pos.x + area->size.x - 1),
                    VSF_DISP_SITRONIX_WRITE(ST7735_RASET, 4,
                        0x00, disp_st7735->page_offset + area->pos.y,
                        0x00, disp_st7735->page_offset + area->pos.y + area->size.y - 1),
                };

                VSF_UI_ASSERT(sizeof(disp_st7735->set_area_cmd_buffer) >= sizeof(cmd));
                memcpy(disp_st7735->set_area_cmd_buffer, cmd, sizeof(cmd));
                vk_disp_sitronix_seq(&disp_st7735->use_as__vk_disp_sitronix_t,
                        (uint8_t *)disp_st7735->set_area_cmd_buffer, sizeof(cmd));
                disp_st7735->is_area_set = true;
            } else if (disp_st7735->is_area_set) {
                if (disp_st7735->ctx.refresh.buffer != NULL) {
                    vk_disp_sitronix_write(&disp_st7735->use_as__vk_disp_sitronix_t, ST7735_RAMWR,
                            disp_st7735->ctx.refresh.buffer, area->size.x * area->size.y * 2);
                    disp_st7735->ctx.refresh.buffer = NULL;
                } else {
                    disp_st7735->is_area_set = false;
                    vk_disp_on_ready(&disp_st7735->use_as__vk_disp_t);
                }
            }
        } else {
            return;
        }
        break;
    }
    disp_st7735->state++;
}

static vsf_err_t __vk_disp_st7735_init(vk_disp_t *pthis)
{
    vk_disp_st7735_t *disp_st7735 = (vk_disp_st7735_t *)pthis;
    VSF_UI_ASSERT(disp_st7735 != NULL);

    disp_st7735->ctx.refresh.buffer = NULL;
    disp_st7735->teda.fn.evthandler = __vk_disp_st3375_evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    disp_st7735->teda.on_terminate = NULL;
#endif
    return vsf_teda_init(&disp_st7735->teda, vsf_prio_inherit, false);
}

static vsf_err_t __vk_disp_st7735_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_st7735_t *disp_st7735 = (vk_disp_st7735_t *)pthis;
    VSF_UI_ASSERT(disp_st7735 != NULL);

    disp_st7735->ctx.refresh.area = *area;
    disp_st7735->ctx.refresh.buffer = disp_buff;
    return vsf_eda_post_evt(&disp_st7735->teda.use_as__vsf_eda_t, VSF_EVT_REFRESH);
}

#endif

/* EOF */
