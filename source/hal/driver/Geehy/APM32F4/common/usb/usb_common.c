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

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_hw_usb_phyreg_t {
    uint32_t USB_SWITCH;
    uint32_t POWERON_CORE;
    uint32_t USB_PLL_EN;
    uint32_t SHORT_5V_ENABLE;
    uint32_t OTG_SUSPENDM;
    uint32_t TXBITSTUFFENABLE;
    uint32_t PLLICP_SEL_I2C;
    uint32_t HSZR_CNTL_I2C;
    uint32_t SQVTH_CNTL_I2C;
    uint32_t SW_RREF_I2C;
    uint32_t SW_BUF_I2C;
    uint32_t TX2RX_T_CFG_I2C;
    uint32_t TEST_ANA_FAST_I2C;
    uint32_t CLK_MODE_I2C;
    uint32_t USB_DBNCE_FLTR_BYPASS;
    uint32_t USB_SS_SCALEDOWN_MODE;
} vk_hw_usb_phyreg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t __vsf_hw_usb_init(vsf_hw_usb_t *usb, vsf_arch_prio_t priority,
            bool is_fs_phy, usb_ip_irqhandler_t handler, void *param)
{
    const vsf_hw_usb_const_t *usb_hw_param = usb->param;
    vk_hw_usb_phyreg_t *phy_reg = (vk_hw_usb_phyreg_t *)usb_hw_param->phy_reg;

    if (phy_reg != NULL) {
        phy_reg->USB_SWITCH = 1;
        phy_reg->POWERON_CORE = 1;
        phy_reg->OTG_SUSPENDM = 1;

        __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop");
        __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop");
        phy_reg->PLLICP_SEL_I2C = 5;
    }

    usb->callback.irqhandler = handler;
    usb->callback.param = param;

    NVIC_SetPriority(usb_hw_param->irq, priority);
    NVIC_ClearPendingIRQ(usb_hw_param->irq);
    return VSF_ERR_NONE;
}

void __vsf_hw_usb_irq_enable(vsf_hw_usb_t *usb)
{
    NVIC_EnableIRQ(usb->param->irq);
}

void __vsf_hw_usb_irq_disable(vsf_hw_usb_t *usb)
{
    NVIC_DisableIRQ(usb->param->irq);
}

#endif
