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

#ifndef __HAL_DRIVER_NUVOTON_M480_OHCI_H__
#define __HAL_DRIVER_NUVOTON_M480_OHCI_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../../__device.h"

//! include the infrastructure
#include "../../io/io.h"
#include "../../pm/pm.h"

#include "hal/interface/vsf_interface_usb.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct m480_ohci_const_t {
    m480_usbphy_t phy;
    IRQn_Type irq;
    pm_ahb_clk_no_t ahbclk;
    pm_periph_async_clk_no_t periph_async_clk;

    USBH_T *reg;
    io_cfg_t dp;
    io_cfg_t dm;
};
typedef struct m480_ohci_const_t m480_ohci_const_t;

struct m480_ohci_t {
    struct {
        void (*irq_handler)(void *param);
        void *param;
    } callback;

    const m480_ohci_const_t *param;
};
typedef struct m480_ohci_t m480_ohci_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t m480_ohci_init(m480_ohci_t *hc, usb_hc_ip_cfg_t *cfg);
extern void m480_ohci_get_info(m480_ohci_t *hc, usb_hc_ip_info_t *info);
extern void m480_ohci_irq(m480_ohci_t *hc);

#endif
/* EOF */
