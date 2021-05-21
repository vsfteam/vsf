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

#include "hal/vsf_hal_cfg.h"
#include "./device.h"
#include "./driver.h"

#include "time_api.h"
#include "stdio_uart.h"
#include "sysctrl_api.h"

#include "rtos_al.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#include "hal/driver/common/swi/arm/vsf_swi_template.inc"

int __low_level_init(void)
{
    // avoid systick interrupt before os started while running in ram
    SysTick->CTRL = 0;
    return 1;
}

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_driver_init(void)
{
    SystemInit();
    sysctrl_clock_cfg(CLK_CFG_D480S240P120F80);
    SystemCoreClockUpdate();

    stdio_uart_init();
    dbg_init();
    dbg("\r\n    dsp_clock: %dM, sys_clock: %dM, pclk: %dM, flash_clock: %dM\r\n",
            DSPSysCoreClock / 1000000, SystemCoreClock / 1000000,
            PeripheralClock / 1000000, sysctrl_clock_get(PER_FLASH) / 1000000);

    aic_time_init(0, 0);
    return true;
}


/* EOF */
