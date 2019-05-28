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

#define M480_PLLSRC                         M480_PLLSRC_HXT
#define M480_HCLKSRC                        M480_HCLKSRC_PLLFOUT
#define M480_PLL_FREQ_HZ                    (384 * 1000 * 1000)
#define M480_CPU_FREQ_HZ                    (192 * 1000 * 1000)
#define M480_HCLK_FREQ_HZ                   (192 * 1000 * 1000)
#define M480_HXT_FREQ_HZ                    (12 * 1000 * 1000)
#define M480_LXT_FREQ_HZ                    0
#define M480_HIRC_FREQ_HZ                   (22 * 1000 * 1000)
#define M480_LIRC_FREQ_HZ                   (10 * 1000 * 1000)






#define M480_CLK_PLLCTL_NO_1				(0x0UL << CLK_PLLCTL_OUTDIV_Pos)
#define M480_CLK_PLLCTL_NO_2				(0x1UL << CLK_PLLCTL_OUTDIV_Pos)
#define M480_CLK_PLLCTL_NO_4				(0x3UL << CLK_PLLCTL_OUTDIV_Pos)

#define M480_CLK_CLKSEL0_HCLKSEL_HXT		(0x00UL << CLK_CLKSEL0_HCLKSEL_Pos)
#define M480_CLK_CLKSEL0_HCLKSEL_LXT		(0x01UL << CLK_CLKSEL0_HCLKSEL_Pos)
#define M480_CLK_CLKSEL0_HCLKSEL_PLL		(0x02UL << CLK_CLKSEL0_HCLKSEL_Pos)
#define M480_CLK_CLKSEL0_HCLKSEL_LIRC		(0x03UL << CLK_CLKSEL0_HCLKSEL_Pos)
#define M480_CLK_CLKSEL0_HCLKSEL_HIRC		(0x07UL << CLK_CLKSEL0_HCLKSEL_Pos)

#define M480_CLK_CLKSEL1_TIM0SEL_HXT		(0x00UL << CLK_CLKSEL1_TMR0SEL_Pos)
#define M480_CLK_CLKSEL1_TIM0SEL_LXT		(0x01UL << CLK_CLKSEL1_TMR0SEL_Pos)
#define M480_CLK_CLKSEL1_TIM0SEL_PCLK		(0x02UL << CLK_CLKSEL1_TMR0SEL_Pos)
#define M480_CLK_CLKSEL1_TIM0SEL_EXTTRG		(0x03UL << CLK_CLKSEL1_TMR0SEL_Pos)
#define NM480_CLK_CLKSEL1_TIM0SEL_LIRC		(0x05UL << CLK_CLKSEL1_TMR0SEL_Pos)
#define M480_CLK_CLKSEL1_TIM0SEL_HIRC		(0x07UL << CLK_CLKSEL1_TMR0SEL_Pos)

#define M480_TCSR_MODE_ONESHOT				(0x00UL << TIMER_CTL_OPMODE_Pos)
#define M480_TCSR_MODE_PERIODIC				(0x01UL << TIMER_CTL_OPMODE_Pos)
#define M480_TCSR_MODE_TOGGLE				(0x02UL << TIMER_CTL_OPMODE_Pos)
#define M480_TCSR_MODE_COUNTINUOUS			(0x03UL << TIMER_CTL_OPMODE_Pos)

/*============================ MACROFIED FUNCTIONS ===========================*/

#define M480_CLK_PLLCTL_NR(x)				(((x) - 1) << 9)
#define M480_CLK_PLLCTL_NF(x)				(((x) - 2) << 0)

/*============================ TYPES =========================================*/

// test only, remove later
enum m480_hclksrc_t
{
    M480_HCLKSRC_HIRC = 7,
    M480_HCLKSRC_PLL2FOUT = 4,
    M480_HCLKSRC_LIRC = 3,
    M480_HCLKSRC_PLLFOUT = 2,
    M480_HCLKSRC_LXT = 1,
    M480_HCLKSRC_HXT = 0,
};
typedef enum m480_hclksrc_t m480_hclksrc_t;

enum m480_pclksrc_t
{
    M480_PCLKSRC_HCLK = 0,
    M480_PCLKSRC_HCLKd2 = 1,
};
typedef enum m480_pclksrc_t m480_pclksrc_t;

enum m480_pllsrc_t
{
    M480_PLLSRC_HXT = 0,
    M480_PLLSRC_HIRC = 1,
    M480_PLLSRC_NONE = -1,
};
typedef enum m480_pllsrc_t m480_pllsrc_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void __low_level_init(void)
{
    // to use SRAM BANK2
    CLK->AHBCLK |= CLK_AHBCLK_SPIMCKEN_Msk;
    SPIM->CTL1 |= 1 << 2;
}

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_driver_init(void)
{
    // test only, remove later
    uint32_t temp32;
    uint32_t freq_in;
    bool state = m480_reg_unlock();

    // enable clocks
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;
    while ((CLK->STATUS & CLK_STATUS_HXTSTB_Msk) != CLK_STATUS_HXTSTB_Msk);

    CLK->PWRCTL |= CLK_PWRCTL_HIRCEN_Msk;
    while ((CLK->STATUS & CLK_STATUS_HXTSTB_Msk) != CLK_STATUS_HXTSTB_Msk);
    CLK->CLKSEL0 |= CLK_CLKSEL0_HCLKSEL_Msk;
    CLK->CLKDIV0 &= (~CLK_CLKDIV0_HCLKDIV_Msk);

    // enable PLLs
    if (1) {
        uint8_t no;
        uint32_t no_mask;

        switch (M480_PLLSRC) {
        case M480_PLLSRC_HXT:
            temp32 = 0;
            freq_in = M480_HXT_FREQ_HZ;
            break;
        case M480_PLLSRC_HIRC:
            temp32 = CLK_PLLCTL_PLLSRC_Msk;
            freq_in = M480_HIRC_FREQ_HZ;
            break;
        default:
            return VSF_ERR_INVALID_PARAMETER;
        }
        // Fin/NR: 2MHz
        if ((M480_PLL_FREQ_HZ * 1 > ( 200* 1000 * 1000)) &&
                (M480_PLL_FREQ_HZ * 1 < (500 * 1000 * 1000))) {
            no = 1;
            no_mask = M480_CLK_PLLCTL_NO_1;            
        } else if ((M480_PLL_FREQ_HZ * 2 > (200 * 1000 * 1000)) &&
                (M480_PLL_FREQ_HZ * 2 < (500 * 1000 * 1000)))
        {
            no = 2;
            no_mask = M480_CLK_PLLCTL_NO_2;
        } else if ((M480_PLL_FREQ_HZ * 4 > (200 * 1000 * 1000)) &&
                (M480_PLL_FREQ_HZ * 4 < (500 * 1000 * 1000)))
        {
            no = 4;
            no_mask = M480_CLK_PLLCTL_NO_4;
        } else {
            return VSF_ERR_INVALID_PARAMETER;
        }
        CLK->PLLCTL |= CLK_PLLCTL_PD_Msk;
        CLK->PLLCTL = temp32 | no_mask |
        M480_CLK_PLLCTL_NR(freq_in / 6000000) |
        M480_CLK_PLLCTL_NF(M480_PLL_FREQ_HZ * no / 12000000);
        while ((CLK->STATUS & CLK_STATUS_PLLSTB_Msk) != CLK_STATUS_PLLSTB_Msk);
    }

    if (M480_CPU_FREQ_HZ < 27 * 1000 * 1000) {
        FMC->CYCCTL = 1;
    } else if (M480_CPU_FREQ_HZ < 54 * 1000 * 1000) {
        FMC->CYCCTL = 2;
    } else if (M480_CPU_FREQ_HZ < 81 * 1000 * 1000) {
        FMC->CYCCTL = 3;
    } else if (M480_CPU_FREQ_HZ < 108 * 1000 * 1000) {
        FMC->CYCCTL = 4;
    } else if (M480_CPU_FREQ_HZ < 135 * 1000 * 1000) {
        FMC->CYCCTL = 5;
    } else if (M480_CPU_FREQ_HZ < 162 * 1000 * 1000) {
        FMC->CYCCTL = 6;
    } else if (M480_CPU_FREQ_HZ < 192 * 1000 * 1000) {
        FMC->CYCCTL = 7;
    } else {
        FMC->CYCCTL = 8;
    }

    // set hclk
    switch (M480_HCLKSRC) {
    case M480_HCLKSRC_HIRC:
        freq_in = M480_HIRC_FREQ_HZ;
        temp32 = M480_CLK_CLKSEL0_HCLKSEL_HIRC;
        break;
    case M480_HCLKSRC_LIRC:
        freq_in = M480_LIRC_FREQ_HZ;
        temp32 = M480_CLK_CLKSEL0_HCLKSEL_LIRC;
        break;
    case M480_HCLKSRC_PLLFOUT:
        freq_in = M480_PLL_FREQ_HZ;
        temp32 = M480_CLK_CLKSEL0_HCLKSEL_PLL;
        break;
    case M480_HCLKSRC_LXT:
        freq_in = M480_LXT_FREQ_HZ;
        temp32 = M480_CLK_CLKSEL0_HCLKSEL_LXT;
        break;
    case M480_HCLKSRC_HXT:
        freq_in = M480_HXT_FREQ_HZ;
        temp32 = M480_CLK_CLKSEL0_HCLKSEL_HXT;
        break;
    default:
        return VSF_ERR_INVALID_PARAMETER;
    }

    CLK->PWRCTL |= CLK_PWRCTL_HIRCEN_Msk;
    while ((CLK->STATUS & CLK_STATUS_HXTSTB_Msk) != CLK_STATUS_HXTSTB_Msk);
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk)) | M480_CLK_CLKSEL0_HCLKSEL_HIRC;

    CLK->CLKDIV0 = (CLK->CLKDIV0 & ~CLK_CLKDIV0_HCLKDIV_Msk) |
            ((freq_in / M480_HCLK_FREQ_HZ) - 1);
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | temp32;

    m480_reg_lock(state);

    return true;
}


/* EOF */
