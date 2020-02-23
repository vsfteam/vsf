/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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
#include "../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED


declare_class(vsf_tgui_t)

#define __VK_TGUI_CONTROLS_SLIDER_CLASS_IMPLEMENT
#include "./__vk_tgui_slider.h"
#include <math.h>


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void vk_tgui_slider_init(   __vk_tgui_slider_t *ptSlider, 
                            const vsf_tgui_control_t *ptHost,
                            uint_fast8_t chFPS)
{
    VSF_TGUI_ASSERT(NULL != ptSlider);

    if (0 == ptSlider->hwSpeed) {
        ptSlider->hwSpeed = 300;
    }
    if (0 == chFPS) {
        /* use default frame rate per sec (1000 ms)*/
        chFPS = 30;
    }

    ptSlider->tSlideTimer.u29Interval = 1024 / chFPS;

    vsf_tgui_timer_init(&ptSlider->tSlideTimer, ptHost);
}

static void __vk_tgui_slider_update(__vk_tgui_slider_t *ptSlider)
{
    if (ptSlider->tPosition.iCurrent != ptSlider->tPosition.iTarget) {
        if (!vsf_tgui_timer_is_working(&ptSlider->tSlideTimer)) {
            ptSlider->tOldTimeTick = vsf_systimer_tick_to_ms( vsf_systimer_get());
            vsf_tgui_timer_enable(&ptSlider->tSlideTimer);
        }
    } 
}

int_fast16_t vk_tgui_slider_on_timer_event_handler(__vk_tgui_slider_t *ptSlider)
{
    VSF_TGUI_ASSERT(NULL != ptSlider);
    vsf_timer_tick_t tCurrentTime = vsf_systimer_tick_to_ms( vsf_systimer_get());
    vsf_timer_tick_t tElapsed = tCurrentTime - ptSlider->tOldTimeTick + ptSlider->tPosition.iResidual;
    ptSlider->tOldTimeTick = tCurrentTime;

    uint_fast32_t wStep = tElapsed * ptSlider->hwSpeed;
    ptSlider->tPosition.iResidual = (wStep & (_BV(10) - 1)) / ptSlider->hwSpeed;
    wStep >>= 10;
    
    int_fast16_t nDelta = ptSlider->tPosition.iTarget - ptSlider->tPosition.iCurrent;
    
    wStep = min(ABS(nDelta), wStep);

    ptSlider->tPosition.iCurrent += ((int16_t)wStep * sign(nDelta));
    
    __vk_tgui_slider_update(ptSlider);

    return ptSlider->tPosition.iCurrent;
}

int_fast16_t vk_tgui_slider_location_target_set(  __vk_tgui_slider_t *ptSlider, 
                                                    int_fast16_t iLocation)
{
    VSF_TGUI_ASSERT(NULL != ptSlider);

    ptSlider->tPosition.iTarget = iLocation;

    __vk_tgui_slider_update(ptSlider);
    return ptSlider->tPosition.iCurrent;
}

int_fast16_t vk_tgui_slider_location_current_get( __vk_tgui_slider_t *ptSlider)
{
    VSF_TGUI_ASSERT(NULL != ptSlider);
    return ptSlider->tPosition.iCurrent;
}

int_fast16_t vk_tgui_slider_location_current_set( __vk_tgui_slider_t *ptSlider,
                                                    int_fast16_t iLocation)
{
    VSF_TGUI_ASSERT(NULL != ptSlider);
    ptSlider->tPosition.iCurrent = iLocation;

    __vk_tgui_slider_update(ptSlider);
    return iLocation;
}


#endif


/* EOF */
