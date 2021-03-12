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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_driver_init(void)
{
    // TODO: test code here
    uint_fast32_t tmp32;

    // RCC reset
    RCC->CTLR |= RCC_HSION;
    RCC->CFGR0 &= ~(RCC_CFGR0_MCO | RCC_ADCPRE | RCC_PPRE2 | RCC_PPRE1 | RCC_HPRE | RCC_SW);
    RCC->CTLR &= ~(RCC_PLLON | RCC_CSSON | RCC_HSEON);
    RCC->CTLR &= ~(RCC_HSEBYP);
    RCC->CFGR0 &= ~(RCC_USBPRE | RCC_PLLMULL | RCC_PLLXTPRE | RCC_PLLSRC);
    RCC->INTR = RCC_PLLRDYC | RCC_HSERDYC | RCC_HSIRDYC | RCC_LSERDYC | RCC_LSIRDYC;

    RCC->CTLR |= RCC_HSEON;
    while (!(RCC->CTLR & RCC_HSERDY));

    if (CH32F10X_SYS_FREQ_HZ > (48 * 1000 * 1000)) {
        FLASH->ACTLR = FLASH_ACTLR_PRFTBE | FLASH_ACTLR_LATENCY_2;
    } else if (CH32F10X_SYS_FREQ_HZ > (24 * 1000 * 1000)) {
        FLASH->ACTLR = FLASH_ACTLR_PRFTBE | FLASH_ACTLR_LATENCY_1;
    } else {
        FLASH->ACTLR = FLASH_ACTLR_PRFTBE | FLASH_ACTLR_LATENCY_0;
    }

    // configure AHB/APB1/APB2
    tmp32 = ffz(~(CH32F10X_SYS_FREQ_HZ / CH32F10X_AHB_FREQ_HZ));
    if (tmp32) {
        // set HPRE, TODO: use offset MACRO when available
        RCC->CFGR0 |= (0x08 | (tmp32 - 1)) << 4;
    }
    tmp32 = ffz(~(CH32F10X_AHB_FREQ_HZ / CH32F10X_APB1_FREQ_HZ));
    if (tmp32) {
        // set PPRE1, TODO: use offset MACRO when available
        RCC->CFGR0 |= (0x04 | (tmp32 - 1)) << 8;
    }
    tmp32 = ffz(~(CH32F10X_AHB_FREQ_HZ / CH32F10X_APB2_FREQ_HZ));
    if (tmp32) {
        // set PPRE2, TODO: use offset MACRO when available
        RCC->CFGR0 |= (0x04 | (tmp32 - 1)) << 11;
    }

    // setup PLL
    RCC->CFGR0 |= RCC_PLLSRC_HSE;
    tmp32 = CH32F10X_PLL_FREQ_HZ / CH32F10X_HSE_FREQ_HZ;
    VSF_HAL_ASSERT((tmp32 >= 2) && (tmp32 <= 16));
    // set PLLMUL, use offset MACRO when available
    RCC->CFGR0 |= (tmp32 - 2) << 18;
    RCC->CTLR |= RCC_PLLON;
    while (!(RCC->CTLR & RCC_PLLRDY));

    RCC->CFGR0 |= RCC_SW_PLL;
    while ((RCC->CFGR0 & RCC_SW) != RCC_SW_PLL);

    // SWD
    RCC->APB2PCENR |= RCC_AFIOEN;
    AFIO->PCFR1 &= ~AFIO_PCFR1_SWJ_CFG;
    AFIO->PCFR1 |= AFIO_PCFR1_SWJ_CFG_JTAGDISABLE;

    // VTOR
#ifdef CH32F10X_VECTOR_TABLE
    SCB->VTOR = CH32F10X_VECTOR_TABLE;
#endif

    return true;
}


/* EOF */
