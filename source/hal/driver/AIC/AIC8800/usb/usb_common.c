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

#include "./usb.h"

#if     (VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DWCOTG == ENABLED)   \
    ||  (VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_DWCOTG == ENABLED)

#include "sysctrl_api.h"
#if PLF_PMIC_VER_LITE
#   include "aic1000Lite_analog_reg.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t __aic8800_usb_init(aic8800_usb_t *usb, vsf_arch_prio_t priority,
            bool is_fs_phy, usb_ip_irqhandler_t handler, void *param)
{
    usb->callback.irqhandler = handler;
    usb->callback.param = param;

#if PLF_PMIC && PLF_PMIC_VER_LITE
    if (pwrctrl_pwrmd_cpusys_sw_record_getf() == CPU_SYS_POWER_ON_RESET) {
        // power up usb & usb_pll
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteAnalogReg->por_ctrl),
            (AIC1000LITE_ANALOG_REG_CFG_RTC_USB_PLL_PU | AIC1000LITE_ANALOG_REG_CFG_RTC_USB_PU |
            AIC1000LITE_ANALOG_REG_CFG_RTC_USBPLL_CLK_EN),
            (AIC1000LITE_ANALOG_REG_CFG_RTC_USB_PLL_PU | AIC1000LITE_ANALOG_REG_CFG_RTC_USB_PU |
            AIC1000LITE_ANALOG_REG_CFG_RTC_USBPLL_CLK_EN));

        if (is_fs_phy) {
            PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteAnalogReg->cfg_ana_usb_ctrl1),
                AIC1000LITE_ANALOG_REG_CFG_ANA_USB_FSLS_DRV_BIT(3),
                AIC1000LITE_ANALOG_REG_CFG_ANA_USB_FSLS_DRV_BIT(3));
        } else {
            // need to clear this bit for high speed phy?
//            PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteAnalogReg->cfg_ana_usb_ctrl1),
//                0,
//                AIC1000LITE_ANALOG_REG_CFG_ANA_USB_FSLS_DRV_BIT(3));
            PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteAnalogReg->cfg_ana_usb_ctrl0),
                (AIC1000LITE_ANALOG_REG_CFG_ANA_USB_ISET_HS_DISCONNECT(7)),
                (AIC1000LITE_ANALOG_REG_CFG_ANA_USB_ISET_HS_DISCONNECT(7)));
        }
    }
#endif
    // power up mmsys
    pwrctrl_mmsys_set(PWRCTRL_POWERUP);
    // clk en
    cpusysctrl_hclkme_set(CSC_HCLKME_USBC_EN_BIT);
    cpusysctrl_oclkme_set(CSC_OCLKME_ULPI_EN_BIT);

    if (cpusysctrl_ulpics_get()) {
        // ULPI clock is stable, release ULPI & USBC RESETn
        cpusysctrl_oclkrs_ulpiset_setb(); // RESETn set
        cpusysctrl_hclkrs_usbcset_setb();
        cpusysctrl_oclkrc_ulpiclr_setb(); // RESETn clr
        cpusysctrl_hclkrc_usbcclr_setb();
    } else {
        return VSF_ERR_FAIL;
    }

    NVIC_SetPriority(USBDMA_IRQn, priority);
    NVIC_ClearPendingIRQ(USBDMA_IRQn);
    NVIC_EnableIRQ(USBDMA_IRQn);
    return VSF_ERR_NONE;
}

#endif
