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

#if __M484__

#define GPIO_PIN_DATA(port, pin) (*((volatile uint32_t *)((GPIO_PIN_DATA_BASE+(0x40*(port))) + ((pin)<<2)))) /*!< Pin Data Input/Output \hideinitializer */
#define PB13                     GPIO_PIN_DATA(1, 13) /*!< Specify PB.13 Pin Data Input/Output \hideinitializer */
#define PB10                     GPIO_PIN_DATA(1, 10) /*!< Specify PB.13 Pin Data Input/Output \hideinitializer */
#define PB11                     GPIO_PIN_DATA(1, 11) /*!< Specify PB.13 Pin Data Input/Output \hideinitializer */

#ifdef WEAK_VK_DISP_MIPI_LCD_IO_INIT
void vk_disp_mipi_lcd_io_init(vk_disp_mipi_lcd_t *disp_mipi_lcd)
{
    SYS->GPB_MFPH = SYS->GPB_MFPH & ~(  SYS_GPB_MFPH_PB10MFP_Msk
                                      | SYS_GPB_MFPH_PB12MFP_Msk
                                      | SYS_GPB_MFPH_PB13MFP_Msk
                                      | SYS_GPB_MFPH_PB14MFP_Msk
                                      | SYS_GPB_MFPH_PB15MFP_Msk )

                                      | (0x04UL << SYS_GPB_MFPH_PB12MFP_Pos)
                                      | (0x00UL << SYS_GPB_MFPH_PB13MFP_Pos) //| (0x04UL << SYS_GPB_MFPH_PB13MFP_Pos)
                                      | (0x00UL << SYS_GPB_MFPH_PB10MFP_Pos)
                                      | (0x04UL << SYS_GPB_MFPH_PB14MFP_Pos)
                                      | (0x04UL << SYS_GPB_MFPH_PB15MFP_Pos);
    PB->SMTEN  |= GPIO_SMTEN_SMTEN15_Msk;
    PB->SLEWCTL = (PB->SLEWCTL & 0x00FFFFFF) | 0x55100000;
    PB->MODE = PB->MODE & ~(0x03 << 26) | (0x01 << 26);                // force MOSI as GPIO
    PB->MODE = PB->MODE & ~(0x03 << 20) | (0x01 << 20);                // PB10


    SYS->GPB_MFPH = SYS->GPB_MFPH & ~(  SYS_GPB_MFPH_PB10MFP_Msk
                                      | SYS_GPB_MFPH_PB12MFP_Msk
                                      | SYS_GPB_MFPH_PB13MFP_Msk
                                      | SYS_GPB_MFPH_PB14MFP_Msk
                                      | SYS_GPB_MFPH_PB15MFP_Msk )

                                      | (0x04UL << SYS_GPB_MFPH_PB12MFP_Pos)
                                      | (0x00UL << SYS_GPB_MFPH_PB13MFP_Pos) //| (0x04UL << SYS_GPB_MFPH_PB13MFP_Pos)
                                      | (0x00UL << SYS_GPB_MFPH_PB10MFP_Pos)
                                      | (0x04UL << SYS_GPB_MFPH_PB14MFP_Pos)
                                      | (0x04UL << SYS_GPB_MFPH_PB15MFP_Pos);
    PB->SMTEN  |= GPIO_SMTEN_SMTEN15_Msk;
    PB->SLEWCTL = (PB->SLEWCTL & 0x00FFFFFF) | 0x55100000;
    PB->MODE = PB->MODE & ~(0x03 << 26) | (0x01 << 26);                // force MOSI as GPIO
    PB->MODE = PB->MODE & ~(0x03 << 20) | (0x01 << 20);                // PB10

    // PB10 PB11 as gpio
    SYS->GPB_MFPH = SYS->GPB_MFPH & ~(  SYS_GPB_MFPH_PB9MFP_Msk
                                      | SYS_GPB_MFPH_PB10MFP_Msk
                                      | SYS_GPB_MFPH_PB11MFP_Msk)
                                      | (0x00UL << SYS_GPB_MFPH_PB9MFP_Pos)
                                      | (0x00UL << SYS_GPB_MFPH_PB10MFP_Pos)
                                      | (0x00UL << SYS_GPB_MFPH_PB11MFP_Pos);
    // high speed
    PB->SLEWCTL = (PB->SLEWCTL & ~(GPIO_SLEWCTL_HSREN9_Msk | GPIO_SLEWCTL_HSREN10_Msk | GPIO_SLEWCTL_HSREN11_Msk))
                    | (0x1UL << GPIO_SLEWCTL_HSREN9_Pos)
                    | (0x1UL << GPIO_SLEWCTL_HSREN10_Pos)
                    | (0x1UL << GPIO_SLEWCTL_HSREN11_Pos) ;

    // input or ouput
    PB->MODE = PB->MODE & ~(GPIO_MODE_MODE10_Msk | GPIO_MODE_MODE11_Msk)
                | (0x01 << GPIO_MODE_MODE9_Pos)  // output
                | (0x01 << GPIO_MODE_MODE10_Pos)  // output
                | (0x00 << GPIO_MODE_MODE11_Pos); // input
}
#endif

#if VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
void vk_disp_mipi_lcd_hw_reset(vk_disp_mipi_lcd_t *disp_mipi_lcd, bool level)
{
    if (level) {
        PB10 = 1;
    } else {
        PB10 = 0;
    }
}
#endif

void vk_disp_mipi_lcd_dcx_set(vk_disp_mipi_lcd_t *disp_mipi_lcd, bool state)
{
    PB13 = state;
}

void vk_disp_mipi_te_line_interrupt_enable_once(vk_disp_mipi_lcd_t *disp_mipi_lcd)
{
    while (PB11 != 0);
    while (PB11 == 0);

    vk_disp_mipi_tearing_effect_line_ready(disp_mipi_lcd);
}
#endif

static void __disp_demo_fps_dump(void)
{
#if APP_DISP_DEMO_FPS_OUTPUT == ENABLED
    static uint16_t __fps;
    static uint32_t __refresh_cnt;
    static vsf_systimer_tick_t __start_tick;

    uint32_t elapse;
    uint32_t current_tick;

    __refresh_cnt++;
    current_tick = vsf_systimer_get_tick();
    elapse = vsf_systimer_tick_to_ms(current_tick - __start_tick);

    if (elapse >= 1000) {
        __start_tick = current_tick;
        __fps = __refresh_cnt * 1000 / elapse;
        __refresh_cnt = 0;
        vsf_trace_info("disp demo, fps: %d\n", __fps);
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
