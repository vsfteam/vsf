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

#define __VSF_DISP_CLASS_INHERIT__
#include <unistd.h>

#if VSF_USE_UI == ENABLED && VSF_USE_LLGUI == ENABLED && VSF_USE_LLGUI == ENABLED

#include "../common/usrapp_common.h"
#include "LL_Handler.h"
#include "LL_Timer.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vsf_eda_t * __llgui_disp_on_inited_notifier_eda = NULL;
static vsf_callback_timer_t __llgui_timer;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __llgui_disp_on_refreshed(vk_disp_t *disp)
{
    if (__llgui_disp_on_inited_notifier_eda != NULL) {
        vsf_eda_post_evt(__llgui_disp_on_inited_notifier_eda, VSF_EVT_USER);
    }
}

static void __llgui_disp_on_inited(vk_disp_t *disp)
{
    disp->ui_on_ready = __llgui_disp_on_refreshed;
    __llgui_disp_on_refreshed(disp);
}

static void __llgui_on_timer(vsf_callback_timer_t *timer)
{
    llTimer_ticks(1);
}

void llCfgSetPoint(int16_t x,int16_t y,llColor color)
{
    usrapp_ui_common.llgui.color[y * CONFIG_MONITOR_WIDTH + x] = color;
}

llColor llCfgGetPoint(int16_t x,int16_t y)
{
    return usrapp_ui_common.llgui.color[y * CONFIG_MONITOR_WIDTH + x];
}

bool llCfgClickGetPoint(int16_t *x,int16_t *y)
{
    return false;
}

void llCfgFillSingleColor(int16_t x0,int16_t y0,int16_t x1,int16_t y1,llColor color)
{
    int16_t x_left = min(x0, x1);
    int16_t x_right = max(x0, x1);
    int16_t y_left = min(y0, y1);
    int16_t y_right = max(y0, y1);
    int32_t offset;

    // TODO: optimize gram copy
    for (int32_t y = y_left; y <= y_right; y++) {
        offset = y * CONFIG_MONITOR_WIDTH;
        for (int32_t x = x_left; x <= x_right; x++) {
            usrapp_ui_common.llgui.color[offset + x] = color;
        }
    }
}

void llGetRtc(uint8_t *readBuf)
{
}

void llSetRtc(uint8_t *writeBuf)
{
}

void llExFlashInit(void)
{
}

void llReadExFlash(uint32_t addr,uint8_t* pBuffer,uint16_t length)
{
}

void llBuzzerBeep(void)
{
}


int llgui_main(int argc, char *argv[])
{
    vk_disp_t *vsf_disp = usrapp_ui_common.disp;
    if (NULL == vsf_disp) {
        return -1;
    }

    if (vsf_disp->param.color != VSF_DISP_COLOR_RGB565) {
        // insecure operation
        ((vk_disp_param_t *)&vsf_disp->param)->color = VSF_DISP_COLOR_RGB565;
    }

    __llgui_disp_on_inited_notifier_eda = vsf_eda_get_cur();
    vsf_disp->ui_on_ready = __llgui_disp_on_inited;
    vk_disp_init(vsf_disp);

    vsf_thread_wfe(VSF_EVT_USER);

    __llgui_timer.on_timer = __llgui_on_timer;
    vsf_callback_timer_init(&__llgui_timer);
    vsf_callback_timer_add_ms(&__llgui_timer, 1);

    // avoid div0 error
    llTimer_ticks(1);

    VSF_ASSERT( (vsf_disp->param.height >= CONFIG_MONITOR_HEIGHT)
            &&  (vsf_disp->param.width >= CONFIG_MONITOR_WIDTH));
    vk_disp_area_t llgui_area = {
        .pos.x      = 0,
        .pos.y      = 0,
        .size.x     = CONFIG_MONITOR_WIDTH,
        .size.y     = CONFIG_MONITOR_HEIGHT,
    };

    llInit();
    while (true) {
        llHandler();

        vk_disp_refresh(vsf_disp, &llgui_area, usrapp_ui_common.llgui.color);
        vsf_thread_wfe(VSF_EVT_USER);
    }
    return 0;
}

#endif
