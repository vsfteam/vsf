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
#include "./ohci.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t nuc500_ohci_init(nuc500_ohci_t *hc, usb_hc_ip_cfg_t *cfg)
{
    const nuc500_ohci_const_t *hc_cfg = hc->param;

    // TODO: use pm to configure usbh clock
//    vsf_pm_sync_clk_enable(hc_cfg->ahbclk);
    CLK->SyncCLK |= CLK_SyncCLK_USBHCKEN_Msk;
    // TODO: get pll clock from pm
    uint_fast32_t pll_clk = 480000000;
    CLK->CLKDIV0 = (CLK->CLKDIV0 & ~(CLK_CLKDIV0_USBHDIV_Msk | CLK_CLKDIV0_USBHSEL_Msk))
                |   CLK_CLKDIV0_USBHSEL_Msk | ((pll_clk / 48000000 - 1) << CLK_CLKDIV0_USBHDIV_Pos);

    // TODO: GPIO for USBH should be configured by user, use use PB12/PB13 for test
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk))
                |   (0x2 << SYS_GPB_MFPH_PB12MFP_Pos) | (0x2 << SYS_GPB_MFPH_PB13MFP_Pos);

    hc->callback.irq_handler = cfg->irq_handler;
    hc->callback.param = cfg->param;

    if (cfg->priority >= 0) {
        NVIC_SetPriority(hc_cfg->irq, cfg->priority);
        NVIC_EnableIRQ(hc_cfg->irq);
    } else {
        NVIC_DisableIRQ(hc_cfg->irq);
    }
    return VSF_ERR_NONE;
}

void nuc500_ohci_get_info(nuc500_ohci_t *hc, usb_hc_ip_info_t *info)
{
    if (info != NULL) {
        info->regbase = hc->param->reg;
    }
}

void nuc500_ohci_irq(nuc500_ohci_t *hc)
{
    if (hc->callback.irq_handler != NULL) {
        hc->callback.irq_handler(hc->callback.param);
    }
}