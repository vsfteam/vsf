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

#if VSF_USE_USB_DEVICE == ENABLED || VSF_USE_USB_HOST == ENABLED

#include "hal/vsf_hal.h"

#include "hal/driver/ST/STM32H7RSXX/common/vendor/cmsis_device_h7rs/Include/stm32h7rsxx.h"
#include "./usb_priv.h"

#include "stm32h7rsxx_hal_rcc.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t __vsf_hw_usb_init(vsf_hw_usb_t *usb, vsf_arch_prio_t priority)
{
    const vsf_hw_usb_const_t *param = usb->param;

    vsf_hw_power_enable(param->pwr_regulator_en);
    vsf_hw_power_enable(VSF_HW_PWR_EN_USB33_DET);

    vsf_hw_peripheral_rst_set(param->rst);
    vsf_hw_peripheral_rst_clear(param->rst);
    vsf_hw_peripheral_enable(param->en);

    if (param->phy_clk != NULL) {
        vsf_hw_clk_enable(param->phy_clk);
    }
    if (param->gpio_port_en != 0) {
        vsf_hw_peripheral_enable(param->gpio_port_en);
    }

    if (priority >= 0) {
        NVIC_SetPriority(param->irq, priority);
        NVIC_EnableIRQ(param->irq);
    } else {
        NVIC_DisableIRQ(param->irq);
    }
    return VSF_ERR_NONE;
}

void __vsf_hw_usb_irq(vsf_hw_usb_t *usb)
{
    if (usb->callback.irqhandler != NULL) {
        usb->callback.irqhandler(usb->callback.param);
    }
}

#endif
