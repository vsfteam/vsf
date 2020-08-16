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

#ifndef __HAL_DRIVER_MT071_USB_H__
#define __HAL_DRIVER_MT071_USB_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../device.h"

#include "hal/interface/vsf_interface_pm.h"
#include "hal/interface/vsf_interface_usb.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __USB_OTG_DEF(__N, __VALUE)                                             \
    extern mt071_usb_t USB_OTG##__N##_IP;                                       \
    extern const i_usb_hc_ip_t VSF_USB_HC##__N##_IP;                            \
    extern const i_usb_dc_ip_t VSF_USB_DC##__N##_IP;
#define _USB_OTG_DEF(__N, __VALUE)          __USB_OTG_DEF(__N, __VALUE)
#define USB_OTG_DEF(__N, __VALUE)           _USB_OTG_DEF(__N, __VALUE)

/*============================ TYPES =========================================*/

struct mt071_usb_const_t {
    IRQn_Type irq;
    pm_sync_clk_no_t sync_clk;
    pm_periph_async_clk_no_t pclk;
    void *reg;
};
typedef struct mt071_usb_const_t mt071_usb_const_t;

struct mt071_usb_t {
    bool is_host;
    struct {
        usb_ip_irq_handler_t irq_handler;
        void *param;
    } callback;
    const mt071_usb_const_t *param;
};
typedef struct mt071_usb_t mt071_usb_t;

/*============================ INCLUDES ======================================*/
#include "./hc/usbh.h"
#include "./dc/usbd.h"

/*============================ GLOBAL VARIABLES ==============================*/

REPEAT_MACRO(USB_OTG_COUNT, USB_OTG_DEF, NULL)

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
