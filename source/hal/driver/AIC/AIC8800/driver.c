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
#include "./device.h"
#include "./driver.h"

#include "time_api.h"
#include "stdio_uart.h"
#include "sysctrl_api.h"

#include "dbg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_HW_RNG_INIT(__COUNT, __DONT_CARE)                                 \
    vsf_hw_rng_init(&VSF_MCONNECT(vsf_hw_rng, __COUNT));

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

// to sustain the data in range [0x00100000, 0x0015FFFF] while enter low power mode,
//  set __data_start__ and _shrbram to a value in the range
int __data_start__ = 0x00100000;
int _shrbram = 0x00100000;

// for SDK-A12-20220225
int __fast_func_start__ = 0;
int __fast_func_end__ = 0;
int __fast_etext = 0;
int __pavol_text_start__ = 0;
int __pavol_text_end__ = 0;
int __pavol_etext = 0;

syscfg_predefined_t const syscfg_predefined = {
    #if PLF_PMIC_LDO_VCORE
    .pmic_vcore_dcdc_off = 1,
    #elif PLF_PMIC_DCDC_VCORE
    .pmic_vcore_ldo_off  = 1,
    #endif
    .pmic_vrtc09_ldo_off = 0,
    #if PLF_LPO_512K
    .pmic_lpo_512k_sel = 1,
    #endif
};

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

// IMPORTANT: to avoid cpu halt if PWR key is pulled up on startup,
//  strong version of user_pwrkey_enabled returning 0 MUST be implemented.
int user_pwrkey_enabled(void)
{
    return 0;
}

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_driver_init(void)
{
    SystemInit();
    sysctrl_clock_cfg(CLK_CFG_D480S240P120F120);
    SystemCoreClockUpdate();

    stdio_uart_init();
    dbg_init();

    aic_time_init(0, 0);

#if VSF_HAL_USE_RNG == ENABLED
    VSF_MREPEAT(VSF_HW_RNG_COUNT, __VSF_HW_RNG_INIT, NULL)
#endif

    return true;
}


/* EOF */
