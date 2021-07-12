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

#include "vsf.h"

#if APP_USE_GUILITE_DEMO == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct EXTERNAL_GFX_OP {
    void (*draw_pixel)(int x, int y, unsigned int rgb);
    void (*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void startHelloStar(void *phy_fb, int width, int height, int color_bytes, struct EXTERNAL_GFX_OP *gfx_op);
static void __guilite_gfx_draw_pixel(int x, int y, unsigned int rgb);
static void __guilite_gfx_fill_rect(int x0, int y0, int x1, int y1, unsigned int rgb);

/*============================ LOCAL VARIABLES ===============================*/

static const struct EXTERNAL_GFX_OP __guilite_gfx_op = {
    .draw_pixel		= __guilite_gfx_draw_pixel,
//    .fill_rect		= __guilite_gfx_fill_rect,
};

static vsf_eda_t *__eda;

/*============================ IMPLEMENTATION ================================*/

static void __vsf_guilite_disp_on_inited(vk_disp_t *disp)
{
    VSF_ASSERT(__eda != NULL);
    vsf_eda_post_evt(__eda, VSF_EVT_USER);
    __eda = NULL;
}

static void __vsf_guilite_disp_on_flushed(vk_disp_t *disp)
{
    VSF_ASSERT(__eda != NULL);
    vsf_eda_post_evt(__eda, VSF_EVT_USER);
    __eda = NULL;
}

static void __guilite_gfx_draw_pixel(int x, int y, unsigned int rgb)
{
    vk_disp_t *vsf_disp = usrapp_ui_common.disp;
    VSF_ASSERT(vsf_disp != NULL);

    vk_disp_area_t disp_area = {
        .pos.x = x,
        .pos.y = y,
        .size.x = 1,
        .size.y = 1,
    };

    switch (vsf_disp->param.color) {
    case VSF_DISP_COLOR_RGB565:
        rgb = ((rgb & 0x0000FF) >> 3) | ((rgb & 0x00FC00) >> 5) | ((rgb & 0xF80000) >> 8);
        break;
    default:
        VSF_ASSERT(false);
    }

    __eda = vsf_eda_get_cur();
    vk_disp_refresh(vsf_disp, &disp_area, &rgb);
    vsf_thread_wfe(VSF_EVT_USER);
}

static void __guilite_gfx_fill_rect(int x0, int y0, int x1, int y1, unsigned int rgb)
{
    vk_disp_t *vsf_disp = usrapp_ui_common.disp;
    VSF_ASSERT(vsf_disp != NULL);
}

void delay_ms(unsigned short nms)
{
    vsf_thread_delay_ms(nms);
}

int guilite_main(int argc, char *argv[])
{
    vk_disp_t *vsf_disp = usrapp_ui_common.disp;
    VSF_ASSERT(vsf_disp != NULL);

    __eda = vsf_eda_get_cur();
    vsf_disp->ui_on_ready = __vsf_guilite_disp_on_inited;
    vk_disp_init(vsf_disp);
    vsf_thread_wfe(VSF_EVT_USER);

    startHelloStar(NULL, 240, 320, 2, (struct EXTERNAL_GFX_OP *)&__guilite_gfx_op);
    return 0;
}

#endif
