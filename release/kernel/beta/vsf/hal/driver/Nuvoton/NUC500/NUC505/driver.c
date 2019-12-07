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
#include "../../NuConsole/NuConsole.h"

/*============================ MACROS ========================================*/

#define NUC500_HXT_FREQ_HZ                  (12UL * 1000 * 1000)
#define NUC500_PLL_FREQ_HZ                  (480UL * 1000 * 1000)
#define NUC500_HCLK_FREQ_HZ                 (96UL * 1000 * 1000)
#define NUC500_PCLK_FREQ_HZ                 (48UL * 1000 * 1000)

#define NUC500_HCLKSRC_HXT                  0
#define NUC500_HCLKSRC_PLLFOUT              1

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

// TODO: if these initialization is implemented in startup file, remove here
char __low_level_init(void)
{
    return 1;
}

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_driver_init(void)
{
    // TODO: test code here
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;
    CLK->CLKDIV0 &= ~CLK_CLKDIV0_HCLKSEL_Msk;

    // configure pll
    // TODO: if pll enabled
    if (1) {
        // pll configure
        uint_fast32_t n, m, p;

        for (p = 1; p <= 9; p++) {
            if (    (NUC500_PLL_FREQ_HZ * p > (300 * 1000 * 1000))
                &&  (NUC500_PLL_FREQ_HZ * p < (1000 * 1000 * 1000))) {
                    break;
            }
        }
        if (    (8 == p)
            &&  (   (NUC500_PLL_FREQ_HZ * 8 < (300 * 1000 * 1000))
                ||  (NUC500_PLL_FREQ_HZ * 8 > (1000 * 1000 * 1000)))) {
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }

        if (NUC500_PLL_FREQ_HZ * p > (768 * 1000 * 1000)) {
            m = 1;
        } else {
            m = 2;
        }

        n = NUC500_PLL_FREQ_HZ * p * m / NUC500_HXT_FREQ_HZ;
        if ((n < 1) || (n > 128)) {
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }
        CLK->PLLCTL = (n - 1) | ((m - 1) << 7) | ((p - 1) << 13);
    } else {
        CLK->PLLCTL |= CLK_PLLCTL_PD_Msk;
    }

    // confiure hclk and pclk
    {
        // TODO: get usr setting of HCLKSRC
        uint_fast8_t hclksrc = NUC500_HCLKSRC_PLLFOUT;
        uint_fast32_t div;

        // use lowest speed for PCLK
        CLK->CLKDIV0 = (CLK->CLKDIV0 & ~CLK_CLKDIV0_PCLKDIV_Msk) | ((16 - 1) << CLK_CLKDIV0_PCLKDIV_Pos);

        switch (hclksrc) {
        case NUC500_HCLKSRC_HXT:
            div = NUC500_HXT_FREQ_HZ;
            break;
        case NUC500_HCLKSRC_PLLFOUT:
            div = NUC500_PLL_FREQ_HZ;
            break;
        }
        div /= NUC500_HCLK_FREQ_HZ;
        if ((div < 1) || (div > 16)) {
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }

        CLK->CLKDIV0 =  (CLK->CLKDIV0 & ~(CLK_CLKDIV0_HCLKDIV_Msk | CLK_CLKDIV0_HCLKSEL_Msk))
                    |   ((div - 1) << CLK_CLKDIV0_HCLKDIV_Pos)
                    |   (hclksrc << CLK_CLKDIV0_HCLKSEL_Pos);

        div = NUC500_HCLK_FREQ_HZ / NUC500_PCLK_FREQ_HZ;
        if ((div < 1) || (div > 16)) {
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }
        CLK->CLKDIV0 = (CLK->CLKDIV0 & ~CLK_CLKDIV0_PCLKDIV_Msk) | ((div - 1) << CLK_CLKDIV0_PCLKDIV_Pos);
    }

#if VSF_HAL_CFG_SUPPORT_DEVICE_DEBUGGER_SERIAL_PORT == ENABLED
    NuConsole_Init();
#endif

    return true;
}


/* EOF */
