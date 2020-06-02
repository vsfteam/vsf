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

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_LIBUSB == ENABLED

#define VSF_USBH_IMPLEMENT_CLASS
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vk_usbh_libusb_t {
    struct {
        void *param;
        vk_usbh_libusb_on_event_t on_event;
    } cb;
};
typedef struct vk_usbh_libusb_t vk_usbh_libusb_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vk_usbh_libusb_t vk_usbh_libusb;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void *vk_usbh_libusb_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        vk_usbh_ifs_parser_t *parser_ifs)
{
    struct usb_device_desc_t *desc_device;
    vk_usbh_libusb_dev_t *ldev;

    if (parser_ifs->ifs->no != 0) {
        return NULL;
    }

    ldev = VSF_USBH_MALLOC(sizeof(vk_usbh_libusb_dev_t));
    if (ldev == NULL) {
        return NULL;
    }
    memset(ldev, 0, sizeof(vk_usbh_libusb_dev_t));

    ldev->usbh = usbh;
    ldev->dev = dev;
    ldev->urb.pipe.is_pipe = true;
    desc_device = usbh->parser->desc_device;
    ldev->vid = desc_device->idVendor;
    ldev->pid = desc_device->idProduct;
    ldev->c = desc_device->bDeviceClass;
    ldev->subc = desc_device->bDeviceSubClass;
    ldev->protocol = desc_device->bDeviceProtocol;
    ldev->ep0size = desc_device->bMaxPacketSize0;
    ldev->address = dev->devnum;

    if (vk_usbh_libusb.cb.on_event != NULL) {
        vk_usbh_libusb.cb.on_event(
                    vk_usbh_libusb.cb.param,
                    ldev,
                    VSF_USBH_LIBUSB_EVT_ON_ARRIVED);
    }
    return ldev;
}

static void vk_usbh_libusb_free(vk_usbh_libusb_dev_t *ldev)
{
    VSF_USBH_FREE(ldev);
}

static void vk_usbh_libusb_disconnect(vk_usbh_t *usbh,
        vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_libusb_dev_t *ldev = (vk_usbh_libusb_dev_t *)param;
    if (ldev) {
        if (vk_usbh_libusb.cb.on_event != NULL) {
            vk_usbh_libusb.cb.on_event(
                    vk_usbh_libusb.cb.param,
                    ldev,
                    VSF_USBH_LIBUSB_EVT_ON_LEFT);
        }

        if (ldev->is_opened) {
            ldev->is_to_remove = true;
        } else {
            vk_usbh_libusb_free(ldev);
        }
    }
}

static const vk_usbh_dev_id_t vk_usbh_libusb_id[] = {
    {
        .match_dev_lo = 1,
        .bcdDevice_lo = 0,
        .bDeviceClass = 1,
    },
};

void vk_usbh_libusb_set_evthandler(void *param, vk_usbh_libusb_on_event_t on_event)
{
    vk_usbh_libusb.cb.param = param;
    vk_usbh_libusb.cb.on_event = on_event;
}

void vk_usbh_libusb_close(vk_usbh_libusb_dev_t *ldev)
{
    ldev->is_opened = false;
    if (ldev->is_to_remove) {
        vk_usbh_libusb_free(ldev);
    }
}

vsf_err_t vk_usbh_libusb_open(vk_usbh_libusb_dev_t *ldev)
{
    if (ldev->is_opened) {
        return VSF_ERR_FAIL;
    } else {
        ldev->is_opened = true;
        return VSF_ERR_NONE;
    }
}

const vk_usbh_class_drv_t vk_usbh_libusb_drv = {
    .name       = "libusb",
    .dev_id_num = dimof(vk_usbh_libusb_id),
    .dev_ids    = vk_usbh_libusb_id,
    .probe      = vk_usbh_libusb_probe,
    .disconnect = vk_usbh_libusb_disconnect,
};

#endif
