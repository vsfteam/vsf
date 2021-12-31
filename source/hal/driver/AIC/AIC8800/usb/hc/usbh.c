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

//#include "../../common.h"
#include "./usbh.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_DWCOTG == ENABLED

// for vk_dwcotg_hc_ip_info_t and USB_SPEED_XXX
#include "component/vsf_component.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __aic8800_usb_init(aic8800_usb_t *usb, vsf_arch_prio_t priority,
                bool is_fs_phy, usb_ip_irqhandler_t handler, void *param);

static uint_fast32_t __aic8800_usbh_workaround_reset_port(void *param);
static uint_fast32_t __aic8800_usbh_workaround_enable_port(void *param, uint8_t speed);
static bool __aic8800_usbh_workaround_check_dma_addr(void *param, uintptr_t addr);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vk_dwcotg_hcd_workaround_t __aic8800_usbh_workaround = {
    .param          = (void *)AIC_USB_BASE,
    .reset_port     = __aic8800_usbh_workaround_reset_port,
    .enable_port    = __aic8800_usbh_workaround_enable_port,
    .check_dma_addr = __aic8800_usbh_workaround_check_dma_addr,
};

/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __aic8800_usbh_workaround_reset_port(void *param)
{
    PMIC_MEM_WRITE(0X50010118, 0x400);
    PMIC_MEM_WRITE(0X5001011c, 0x400);
    return 10;
}

static uint_fast32_t __aic8800_usbh_workaround_enable_port(void *param, uint8_t speed)
{
    uint32_t *reg = param;
    if (USB_SPEED_FULL == speed) {
        // host.global_regs.hfir = 60000;
        reg[0x101] = 60000;
    }
    return 20;
}

bool __aic8800_usbh_workaround_check_dma_addr(void *param, uintptr_t addr)
{
    return (addr >= 0x001A0000) && (addr < 0x001C8000);
}

vsf_err_t aic8800_usbh_init(aic8800_usb_t *hc, usb_hc_ip_cfg_t *cfg)
{
    bool is_fs_phy = hc->param->speed == USB_SPEED_FULL;
    hc->is_host = true;
    return __aic8800_usb_init(hc, cfg->priority, is_fs_phy, cfg->irqhandler, cfg->param);
}

void aic8800_usbh_get_info(aic8800_usb_t *hc, usb_hc_ip_info_t *info)
{
    const aic8800_usb_const_t *param = hc->param;
    vk_dwcotg_hc_ip_info_t *dwcotg_info = (vk_dwcotg_hc_ip_info_t *)info;

    VSF_HAL_ASSERT(info != NULL);
    dwcotg_info->regbase = hc->param->reg;
    dwcotg_info->ep_num = hc->param->hc_ep_num;
    dwcotg_info->is_dma = true;
    dwcotg_info->use_as__vk_dwcotg_hw_info_t = param->use_as__vk_dwcotg_hw_info_t;
    dwcotg_info->workaround = (vk_dwcotg_hcd_workaround_t *)&__aic8800_usbh_workaround;
}

void aic8800_usbh_irq(aic8800_usb_t *hc)
{
    VSF_HAL_ASSERT(false);
}

#endif
