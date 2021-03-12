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

bool vsf_driver_init(void)
{
	// TODO:
    RCU_CTL |= RCU_CTL_IRC8MEN;
    while (!(RCU_CTL & RCU_CTL_IRC8MSTB));
    RCU_CFG0 &= ~RCU_CFG0_SCS;

    RCU_ADDCTL |= RCU_ADDCTL_IRC48MEN;
    while (!(RCU_ADDCTL & RCU_ADDCTL_IRC48MSTB));

    RCU_CTL &= ~RCU_CTL_PLLEN;

    RCU_CFG1 &= ~(RCU_CFG1_PLLPRESEL | RCU_CFG1_PREDV0 | RCU_CFG1_PREDV0SEL);
    RCU_CFG1 |= RCU_CFG1_PLLPRESEL | (48000000 / 8000000 - 1);
    RCU_CFG0 |= RCU_CFG0_PLLSEL;

    RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4);
    RCU_CFG0 |= ((13 & 0xf) << 18) | ((13 & 0x10) << 25); // 8M * 15 = 120M

    RCU_CTL |= RCU_CTL_PLLEN;
    while (!(RCU_CTL & RCU_CTL_PLLSTB));

    FMC_WS &= ~FMC_WS_WSCNT;
    FMC_WS |= WS_WSCNT(3);

    // config ahb apb1 apb2
    RCU_CFG0 &= ~(RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC);
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1 | RCU_APB1_CKAHB_DIV2 | RCU_APB2_CKAHB_DIV2;

    RCU_CFG0 |= RCU_CKSYSSRC_PLL;
    while ((RCU_CFG0 & RCU_SCSS_PLL) != RCU_SCSS_PLL);

    RCU_CTL &= ~RCU_CTL_IRC8MEN;
    RCU_AHBEN |= RCU_AHBEN_DMA0EN | RCU_AHBEN_DMA1EN;
	RCU_APB2EN |= RCU_APB2EN_AFEN;
    return true;
}

/* EOF */
