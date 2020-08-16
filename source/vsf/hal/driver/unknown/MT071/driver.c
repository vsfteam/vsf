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

/*============================ MACROS ========================================*/

#define SYSTEM_FREQ                     VSF_SYSTIMER_FREQ

#define PLL_SRC                         CLKREF_12MIRC
#define MAINCLK_SRC                     CLKSRC_PLL

#define PLL_FREQ_HZ                     SYSTEM_FREQ
#define MAINCLK_FREQ_HZ                 SYSTEM_FREQ

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void __low_level_init(void)
{
    
}

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_driver_init(void)
{
    // test only, remove later
    uint_fast32_t timeout = 0;

    // enable clocks
#if (PLL_SRC == CLKREF_12MOSC) || (MAINCLK_SRC == CLKSRC_12MOSC)
//    RCC->PDRUNCFG &= ~RCC_PDRUNCFG_12MOSC;
//    RCC->OSC12_CTRL = RCC_12OSCCTRL_OSC_EN | (1 << 17) | (RCC->OSC12_CTRL & 0xFF); //bit17´ý²âÊÔ
//    while (!(RCC->OSC12_CTRL & RCC_12OSCCTRL_OSC_OK)) {
//        if((timeout++) >= 0x8000) {
//            break;
//        }
//    }
#endif

#if PLL_SRC == CLKREF_12MOSC || PLL_SRC == CLKREF_12MIRC
    uint_fast8_t div = PLL_FREQ_HZ / 12000000;
    if (div < 4) {
        ASSERT(false);
    }
    RCC->PDRUNCFG &= ~RCC_PDRUNCFG_SYSPLL;
    RCC->SYSPLLCTRL = RCC_SYSPLLCTRL_FORCELOCK | (PLL_SRC << 30) | (div - 4);

    while (RCC->SYSPLLSTAT != RCC_SYSPLLSTAT_LOCK) {
        if(timeout++ >= 0x8000) {
            break;
        }
    }
#endif

    if (MAINCLK_FREQ_HZ < 40 * 1000 * 1000) {
        FLASH->ACR = FLASH_ACR_LATENCY_0 | FLASH_ACR_CACHENA;
    } else if (MAINCLK_FREQ_HZ < 80 * 1000 * 1000) {
        FLASH->ACR = FLASH_ACR_LATENCY_1 | FLASH_ACR_CACHENA;
    } else if (MAINCLK_FREQ_HZ < 120 * 1000 * 1000) {
        FLASH->ACR = FLASH_ACR_LATENCY_2 | FLASH_ACR_CACHENA;
    } else {
        FLASH->ACR = FLASH_ACR_LATENCY_3 | FLASH_ACR_CACHENA;
    }

    RCC->MAINCLKSEL = MAINCLK_SRC;
    RCC->MAINCLKUEN = 0;
    RCC->MAINCLKUEN = 1;

    return true;
}


/* EOF */
