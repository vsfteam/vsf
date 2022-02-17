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
#include "vsf.h"

#if VSF_USE_UI == ENABLED && APP_USE_DISP_DEMO == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/

#ifndef APP_DISP_DEMO_CFG_WIDTH
#   define APP_DISP_DEMO_CFG_WIDTH          320
#endif

#ifndef APP_DISP_DEMO_CFG_HEIGHT
#   define APP_DISP_DEMO_CFG_HEIGHT         100
#endif

#ifndef APP_DISP_DEMO_FPS_OUTPUT
#   define APP_DISP_DEMO_FPS_OUTPUT         ENABLED
#endif

#ifndef APP_DISP_DEMO_PRIO
#   define APP_DISP_DEMO_PRIO               vsf_prio_0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsf_teda_t __disp_task;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


static void __disp_demo_fps_dump(void)
{
#if APP_DISP_DEMO_FPS_OUTPUT == ENABLED
    static uint32_t __refresh_cnt;
    static vsf_systimer_tick_t __start_tick;

    uint32_t elapse;
    uint32_t current_tick;

    __refresh_cnt++;
    current_tick = vsf_systimer_get_tick();
    elapse = vsf_systimer_tick_to_ms(current_tick - __start_tick);

    if (elapse >= 1000) {
        vsf_trace_info("disp demo, fps: %d" VSF_TRACE_CFG_LINEEND, __refresh_cnt * 1000 / elapse);
        __refresh_cnt = 0;
        __start_tick = current_tick;
    }
#endif
}

static void __disp_demo_update_buffer(uint16_t *buf, uint32_t size)
{
    // TODO: add color support
    static const uint16_t __colors[] = {0x1F << 11, 0x3F << 6, 0x1F};
    static int __color_index = 0;

    for (int i = 0; i < size; i++) {
        buf[i] = __colors[__color_index];
    }
    __color_index = (__color_index + 1) % dimof(__colors);
}

static void __vk_disp_on_ready(vk_disp_t* disp)
{
    VSF_ASSERT(NULL != disp);
    vsf_eda_t* eda = (vsf_eda_t*)disp->ui_data;
    VSF_ASSERT(NULL != eda);

    vsf_eda_post_evt(eda, VSF_EVT_MESSAGE);
}

static void __disp_demo_evthandler(vsf_eda_t* eda, vsf_evt_t evt)
{
    static uint16_t __color_buf[APP_DISP_DEMO_CFG_WIDTH * APP_DISP_DEMO_CFG_HEIGHT];
    static vk_disp_area_t __disp_area = {
        .pos = {
            .x = 0,
            .y = 0,
        },
        .size = {
            .x = APP_DISP_DEMO_CFG_WIDTH,
            .y = APP_DISP_DEMO_CFG_HEIGHT,
        },
    };

    vk_disp_t * disp = usrapp_ui_common.disp;
    VSF_ASSERT(NULL != eda);

    switch (evt) {
    case VSF_EVT_INIT:
        disp->ui_data = eda;
        disp->ui_on_ready = __vk_disp_on_ready;
        vk_disp_init(disp);
        break;

    case VSF_EVT_MESSAGE:
        __disp_demo_update_buffer(__color_buf, dimof(__color_buf));
        vk_disp_refresh(disp, &__disp_area, __color_buf);
        __disp_demo_fps_dump();
        break;
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
int disp_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

    const vsf_eda_cfg_t cfg = {
        .fn.evthandler = __disp_demo_evthandler,
        .priority = APP_DISP_DEMO_PRIO,
    };
    vsf_teda_start(&__disp_task, (vsf_eda_cfg_t*)&cfg);

    return 0;
}

#endif
