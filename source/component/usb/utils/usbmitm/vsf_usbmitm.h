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

#ifndef __VSF_USB_MITM_H__
#define __VSF_USB_MITM_H__

/*============================ INCLUDES ======================================*/

#include "component/vsf_component.h"

#if     defined(__VSF_USB_MITM_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#   undef __VSF_USB_MITM_CLASS_IMPLEMENT
#elif   defined(__VSF_USB_MITM_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#   undef __VSF_USB_MITM_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USBD_CFG_RAW_MODE != ENABLED
#   error Please enable VSF_USBD_CFG_RAW_MODE to use usb_mitm
#endif

#ifndef VSF_USB_MITM_USBH_PERIODIC_TICKTOCK
#   define VSF_USB_MITM_USBH_PERIODIC_TICKTOCK          DISABLED
#endif

#ifdef VSF_USB_MITM_USBH_URB_NUM
#   error VSF_USB_MITM_USBH_URB_NUM will be defined according to VSF_USB_MITM_USBH_PERIODIC_TICKTOCK
#endif
#if VSF_USB_MITM_USBH_PERIODIC_TICKTOCK == ENABLED
#   define VSF_USB_MITM_USBH_URB_NUM                    2
#else
#   define VSF_USB_MITM_USBH_URB_NUM                    1
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_vsf_fifo(vsf_usb_mitm_usbh_urb_ready_fifo)
def_vsf_fifo(vsf_usb_mitm_usbh_urb_ready_fifo, vk_usbh_urb_t *, VSF_USB_MITM_USBH_URB_NUM)

typedef struct vsf_usb_mitm_trans_t {
    implement(vk_usbd_trans_t);
    vsf_mem_t mem_save;
} vsf_usb_mitm_trans_t;

typedef enum vsf_usb_mitm_evt_t {
    USB_ON_PREPARE_DATA     = VSF_USBD_MAX_EVT + 0,
} vsf_usb_mitm_evt_t;

typedef struct vsf_usb_mitm_urb_t {
    vk_usbh_urb_t usbh_urb[VSF_USB_MITM_USBH_URB_NUM];
    vsf_fifo(vsf_usb_mitm_usbh_urb_ready_fifo) ready_fifo;
} vsf_usb_mitm_urb_t;

vsf_class(vsf_usb_mitm_t) {
    protected_member(
        vk_usbd_dev_t usb_dev;
        vk_usbh_t usb_host;

        void (*callback)(vsf_usb_mitm_t *mitm, vsf_usb_mitm_evt_t evt, void *param);
    )

    protected_member(
        struct usb_ctrlrequest_t request;
    )

    private_member(
        vsf_teda_t teda;
        uint8_t *config_desc[16], *cur_config_desc;
        uint8_t *ifs_desc_cur[16];
        bool is_setup_pending;

        struct {
            vk_usbh_libusb_dev_t *libusb_dev;
            vk_usbh_class_t libusb;
#if VSF_USBH_USE_HUB == ENABLED
            vk_usbh_class_t hub;
#endif

            vsf_usb_mitm_urb_t urb[2 * 15];

            bool is_resetting;
            bool is_control_requesting;
        } usbh;
        struct {
            vsf_usb_mitm_trans_t control_trans;
            vsf_usb_mitm_trans_t trans[2 * 15];
            uint32_t trans_busy;
            uint32_t ep_mask;
            uint8_t address;
        } usbd;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

void vsf_usb_mitm_start(vsf_usb_mitm_t *mitm);

#ifdef __cplusplus
}
#endif

#endif
