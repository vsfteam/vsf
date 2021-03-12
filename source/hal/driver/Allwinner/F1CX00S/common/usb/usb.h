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

#ifndef __HAL_DRIVER_ALLWINNER_F1CX00S_USB_H__
#define __HAL_DRIVER_ALLWINNER_F1CX00S_USB_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"

// for usb_dc and usb_hc structures
#include "hal/interface/vsf_interface_usb.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct f1cx00s_usb_otg_t f1cx00s_usb_otg_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern f1cx00s_usb_otg_t USB_OTG0;
#if VSF_USE_USB_DEVICE == ENABLED
extern const i_usb_dc_t VSF_USB_DC0;
#endif

/*============================ INCLUDES ======================================*/
#include "./dc/usbd.h"
#include "./hc/usbh.h"

/*============================ PROTOTYPES ====================================*/

/*! \note can be used for polling
 *  \param none
 *  \retval true it is safe to enter sleep mode
 *  \retval false polling work is on going, please keep calling the function
 */
extern bool f1cx00s_usb_irq(f1cx00s_usb_otg_t *usb);

#endif
/* EOF */
