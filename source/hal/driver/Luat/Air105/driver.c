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

#include "hal/vsf_hal_cfg.h"

#ifdef FALSE
#   undef FALSE
#endif
#ifdef TRUE
#   undef TRUE
#endif
#include "air105.h"
#undef CRC

#include "global_config.h"

#include "./device.h"
#include "./driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

uint32_t SystemCoreClock;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#include "hal/driver/common/swi/arm/vsf_swi_template.inc"

int vsf_hal_pre_startup_init(void)
{
    // seems systick is not reset on hw reset of aic8800
    // avoid systick interrupt before os started
    SysTick->CTRL = 0;
    return 1;
}

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
VSF_CAL_WEAK(vsf_driver_init)
bool vsf_driver_init(void)
{
    //SystemInit();
    SYSCTRL->CG_CTRL1 = SYSCTRL_APBPeriph_ALL;
    // GPIO reset will halt debug
    SYSCTRL->SOFT_RST1 = SYSCTRL_APBPeriph_ALL & ~SYSCTRL_APBPeriph_GPIO;
    SYSCTRL->SOFT_RST2 &= ~SYSCTRL_USB_RESET;
    SYSCTRL->LOCK_R |= SYSCTRL_USB_RESET;
    SYSCTRL->LDO25_CR &= ~(VSF_BIT(4) | VSF_BIT(5));
    BPU->SEN_ANA0 |= (7 << 25) | (2 << 29);

    //SystemCoreClockUpdate();
    SystemCoreClock = HSE_VALUE * (((SYSCTRL->FREQ_SEL & SYSCTRL_FREQ_SEL_XTAL_Mask) >> SYSCTRL_FREQ_SEL_XTAL_Pos) + 1);

    return true;
}


/* EOF */
