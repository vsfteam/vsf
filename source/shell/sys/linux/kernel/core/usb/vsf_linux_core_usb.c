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

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED

#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#include <unistd.h>

#include <linux/types.h>
#include <linux/usb.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

int usb_control_msg(struct usb_device *udev, unsigned int pipe, __u8 request, __u8 requesttype, __u16 value, __u16 index, void *data, __u16 size, int timeout)
{
    vk_usbh_urb_t *urb = &udev->__dev->ep0.urb;
    vk_usbh_urb_set_buffer(urb, data, size);
    vsf_err_t err = vk_usbh_control_msg(udev->__host, udev->__dev, &(struct usb_ctrlrequest_t){
        .bRequest       = requesttype,
        .bRequest       = request,
        .wValue         = value,
        .wIndex         = index,
        .wLength        = size,
    });
    if (VSF_ERR_NONE != err) {
        return -EIO;
    }

    vsf_thread_wfm();

    if (URB_OK != vk_usbh_urb_get_status(&udev->__dev->ep0.urb)) {
        return -EIO;
    }
    return vk_usbh_urb_get_actual_length(urb);
}

int usb_interrupt_msg(struct usb_device *udev, unsigned int pipe, void *data, int len, int *actual_length, int timeout)
{
    struct urb *urb = vsf_usbh_malloc(sizeof(*urb));
    if (NULL == urb) {
        return -ENOMEM;
    }
    vk_usbh_urb_prepare(&urb->__urb, udev->__dev, (struct usb_endpoint_desc_t *)&(usb_pipe_endpoint(udev, pipe)->desc));
    vk_usbh_alloc_urb(udev->__host, udev->__dev, &urb->__urb);
    vk_usbh_urb_set_buffer(&urb->__urb, data, len);
    if (VSF_ERR_NONE != vk_usbh_submit_urb(udev->__host, &urb->__urb)) {
        vk_usbh_free_urb(udev->__host, &urb->__urb);
        return -EIO;
    }

    vsf_thread_wfm();

    if (URB_OK != vk_usbh_urb_get_status(&urb->__urb)) {
        return -EIO;
    }
    return vk_usbh_urb_get_actual_length(&urb->__urb);
}

int usb_bulk_msg(struct usb_device *udev, unsigned int pipe, void *data, int len, int *actual_length, int timeout)
{
    return usb_interrupt_msg(udev, pipe, data, len, actual_length, timeout);
}

#endif
