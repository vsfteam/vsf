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

#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/

/*\note first define basic info for arch. */
#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)

//! arch info
#   define VSF_ARCH_PRI_NUM         16
#   define VSF_ARCH_PRI_BIT         4

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM             1

#else

#ifndef __HAL_DEVICE_NUVOTON_NUC505_H__
#define __HAL_DEVICE_NUVOTON_NUC505_H__

#define VSF_DEV_SWI_LIST            31

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
#include "./vendor/Include/NUC505Series.h"
#include "../common/__common.h"

/*============================ MACROS ========================================*/

#define GPIO_COUNT                  4

#define PIO_PORTA
#define PIO_PORTA_PIN_NUM           16

#define PIO_PORTB
#define PIO_PORTB_PIN_NUM           16

#define PIO_PORTC
#define PIO_PORTC_PIN_NUM           15

#define PIO_PORTD
#define PIO_PORTD_PIN_NUM           5


#define USB_HC_COUNT                1
#define USB_HC_OHCI_COUNT           1

#define USB_HC0_TYPE                ohci
#define USB_HC0_IRQHandler          USBH_IRQHandler
#define USB_HC0_CONFIG                                                          \
    .reg                = USBH,                                                 \
    .irq                = USBH_IRQn,

#define USB_DC_COUNT                0
#define USB_DC_HS_COUNT             0
#define USB_DC_FS_COUNT             0


#define USB_DC0_TYPE                usbd_hs
#define USB_DC0_IRQHandler          USBD_IRQHandler
#define USB_DC0_EP_NUM              14
#define USB_DC0_CONFIG                                                          \
    .reg                = HSUSBD,                                               \
    .irq                = USBD_IRQn,


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_NUVOTON_NUC505_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
