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

#ifndef __HAL_DRIVER_NUVOTON_NUC500_OHCI_H__
#define __HAL_DRIVER_NUVOTON_NUC500_OHCI_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../../__device.h"

//! include the infrastructure
//#include "../../io/io.h"
//#include "../../pm/pm.h"

#include "hal/interface/vsf_interface_usb.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct nuc500_ohci_const_t {
    IRQn_Type irq;
    USBH_T *reg;
};
typedef struct nuc500_ohci_const_t nuc500_ohci_const_t;

struct nuc500_ohci_t {
    struct {
        void (*irq_handler)(void *param);
        void *param;
    } callback;

    const nuc500_ohci_const_t *param;
};
typedef struct nuc500_ohci_t nuc500_ohci_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t nuc500_ohci_init(nuc500_ohci_t *hc, usb_hc_ip_cfg_t *cfg);
extern void nuc500_ohci_get_info(nuc500_ohci_t *hc, usb_hc_ip_info_t *info);
extern void nuc500_ohci_irq(nuc500_ohci_t *hc);

#endif
/* EOF */
