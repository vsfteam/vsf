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
#include "../../pm/pm.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t m480_ohci_init(m480_ohci_t *hc, usb_hc_ip_cfg_t *cfg)
{
    const m480_ohci_const_t *hc_cfg = hc->param;
    bool state = m480_reg_unlock();
        vsf_pm_sync_clk_enable(hc_cfg->ahbclk);
        do {
//            uint_fast32_t clk = vsf_pm_pll_get_clk_out(PLL0_idx);
            // TODO: remove later
            uint_fast32_t clk = (384 * 1000 * 1000);
            const pm_periph_asyn_clk_cfg_t cfg = {
                .clk_src    = 0,
                .div        = clk / (48 * 1000 * 1000) - 1,
            };
            vsf_pm_peripheral_config(hc_cfg->periph_async_clk, &cfg);
        } while (0);

        CLK->APBCLK0 |= CLK_APBCLK0_USBDCKEN_Msk | CLK_APBCLK0_OTGCKEN_Msk;
//        vsf_pm_sync_clk_enable(SyncCLK_USBD_idx);
//        vsf_pm_sync_clk_enable(SyncCLK_OTG_idx);

        // TODO: use vsf_gpio_config instead
#define SYS_GPA_MFPH_PA13MFP_USB_D_N		(0x0EUL<<SYS_GPA_MFPH_PA13MFP_Pos) /*!< USB Full speed differential signal D-. \hideinitializer */
#define SYS_GPA_MFPH_PA14MFP_USB_D_P		(0x0EUL<<SYS_GPA_MFPH_PA14MFP_Pos) /*!< USB Full speed differential signal D+. \hideinitializer */
        SYS->GPA_MFPH &= ~(SYS_GPA_MFPH_PA13MFP_Msk | SYS_GPA_MFPH_PA14MFP_Msk);
        SYS->GPA_MFPH |= SYS_GPA_MFPH_PA13MFP_USB_D_N | SYS_GPA_MFPH_PA14MFP_USB_D_P;
        // vsf_gpio_config(&hc_cfg->dp, 2);

        m480_enable_usbphy(hc_cfg->phy, M480_USBPHY_HOST);
    m480_reg_lock(state);

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

void m480_ohci_get_info(m480_ohci_t *hc, usb_hc_ip_info_t *info)
{
    if (info != NULL) {
        info->regbase = hc->param->reg;
    }
}

void m480_ohci_irq(m480_ohci_t *hc)
{
    if (hc->callback.irq_handler != NULL) {
        hc->callback.irq_handler(hc->callback.param);
    }
}