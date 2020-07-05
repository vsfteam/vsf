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

#define USB_DC0                 USB_OTG0
#define USB_HC0                 USB_OTG0

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct f1cx00s_usb_dcd_trans_t {
    uint8_t *buffer;
    uint32_t remain;
    uint32_t size;
    uint8_t zlp;
} f1cx00s_usb_dcd_trans_t;

typedef struct f1cx00s_usb_otg_t {
    uint8_t ep_num;
    bool is_host;
    struct {
        void (*irqhandler)(void *param);
    } irq;

    union {
        // TODO: add f1cx00s_usb_dc_t
        struct {
            struct {
#if VSF_USE_USB_DEVICE == ENABLED
                usb_dc_evt_handler_t evthandler;
#endif
                void *param;
            } callback;
            uint16_t fifo_pos;
            uint16_t out_enable;
            bool has_data_stage;
            enum {
                MUSB_USBD_EP0_IDLE,
                MUSB_USBD_EP0_SETUP,
                MUSB_USBD_EP0_DATA_IN,
                MUSB_USBD_EP0_DATA_OUT,
                MUSB_USBD_EP0_STATUS,
            } ep0_state;
            // 4 eps include ep0, both IN and OUT
            f1cx00s_usb_dcd_trans_t trans[2 * 4];
        } dc;
    };
} f1cx00s_usb_otg_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern f1cx00s_usb_otg_t USB_OTG0;
extern const i_usb_dc_t VSF_USB_DC0;
//extern const i_usb_hc_t VSF_USB_HC0;

/*============================ INCLUDES ======================================*/
#include "./dc/usbd.h"
//#include "./hc/usbh.h"
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
