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

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_USBD_UVC == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_DISP_USBD_UVC_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_disp.h"
#include "./vsf_disp_usbd_uvc.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_EVT_REFRESH = VSF_EVT_USER,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_disp_usbd_uvc_init(vk_disp_t *pthis);
static vsf_err_t __vk_disp_usbd_uvc_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_usbd_uvc = {
    .init           = __vk_disp_usbd_uvc_init,
    .refresh        = __vk_disp_usbd_uvc_refresh,
};

/*============================ IMPLEMENTATION ================================*/

static void __vk_disp_usbd_uvc_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_disp_usbd_uvc_t *disp_uvc = container_of(eda, vk_disp_usbd_uvc_t, eda);

    switch (evt) {
    case VSF_EVT_INIT:
        disp_uvc->header[0] = 2;
        disp_uvc->header[1] = 0;
        disp_uvc->line_cnt = 0;
        disp_uvc->frame_cnt = 0;
        break;
    case VSF_EVT_MESSAGE:
        if (++disp_uvc->line_cnt >= disp_uvc->param.height) {
            disp_uvc->line_cnt = 0;
            disp_uvc->frame_cnt++;
            disp_uvc->header[1] ^= 1;
        }
        vk_disp_on_ready(&disp_uvc->use_as__vk_disp_t);
        break;
    case VSF_EVT_REFRESH:
        vk_usbd_uvc_send_packet(disp_uvc->uvc, disp_uvc->cur_buffer,
                2 + disp_uvc->param.width * VSF_DISP_GET_PIXEL_SIZE(disp_uvc) / 8);
        break;
    }
}

static vsf_err_t __vk_disp_usbd_uvc_init(vk_disp_t *pthis)
{
    vk_disp_usbd_uvc_t *disp_uvc = (vk_disp_usbd_uvc_t *)pthis;
    VSF_UI_ASSERT(disp_uvc != NULL);

    disp_uvc->eda.fn.evthandler = __vk_disp_usbd_uvc_evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    disp_uvc->eda.on_terminate = NULL;
#endif
    return vsf_eda_init(&disp_uvc->eda, vsf_prio_inherit, false);
}

// disp_buff MUST be a line_buffer with 2-byte header
static vsf_err_t __vk_disp_usbd_uvc_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_usbd_uvc_t *disp_uvc = (vk_disp_usbd_uvc_t *)pthis;
    uint8_t *buffer = (uint8_t *)disp_buff - 2;
    VSF_UI_ASSERT(disp_uvc != NULL);

    if (    (0 == area->pos.x)
        &&  (disp_uvc->param.width == area->size.x)
        &&  (1 == area->size.y)) {

        buffer[0] = disp_uvc->header[0];
        buffer[1] = disp_uvc->header[1];
        disp_uvc->cur_buffer = buffer;
        vsf_eda_post_evt(&disp_uvc->eda, VSF_EVT_REFRESH);
    } else {
        vsf_trace_error("non-line disp area [%d,%d], [%d,%d]\r\n",
                    area->pos.x, area->pos.y, area->size.x, area->size.y);
        ASSERT(false);
    }
    return VSF_ERR_NONE;
}

#endif

/* EOF */
