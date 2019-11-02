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

struct vsf_usbh_libusb_t {
    struct {
        void *param;
        vsf_usbh_libusb_on_event_t on_event;
    } cb;
};
typedef struct vsf_usbh_libusb_t vsf_usbh_libusb_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vsf_usbh_libusb_t vsf_usbh_libusb;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void *vsf_usbh_libusb_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
        vsf_usbh_ifs_parser_t *parser_ifs)
{
    vsf_usbh_libusb_dev_t *ldev;

    ldev = VSF_USBH_MALLOC(sizeof(vsf_usbh_libusb_dev_t));
    if (ldev == NULL) {
        return NULL;
    }
    memset(ldev, 0, sizeof(vsf_usbh_libusb_dev_t));

    ldev->usbh = usbh;
    ldev->dev = dev;
    ldev->vid = usbh->parser->desc_device->idVendor;
    ldev->pid = usbh->parser->desc_device->idProduct;
    ldev->ifs = parser_ifs->ifs->no;
    ldev->address = dev->devnum;

    if (vsf_usbh_libusb.cb.on_event != NULL) {
        vsf_usbh_libusb.cb.on_event(
                    vsf_usbh_libusb.cb.param,
                    ldev,
                    VSF_USBH_LIBUSB_EVT_ON_ARRIVED);
    }
    return ldev;
}

static void vsf_usbh_libusb_free(vsf_usbh_libusb_dev_t *ldev)
{
    VSF_USBH_FREE(ldev);
}

static void vsf_usbh_libusb_disconnect(vsf_usbh_t *usbh,
        vsf_usbh_dev_t *dev, void *param)
{
    vsf_usbh_libusb_dev_t *ldev = (vsf_usbh_libusb_dev_t *)param;
    if (ldev) {
        if (vsf_usbh_libusb.cb.on_event != NULL) {
            vsf_usbh_libusb.cb.on_event(
                    vsf_usbh_libusb.cb.param,
                    ldev,
                    VSF_USBH_LIBUSB_EVT_ON_LEFT);
        }

        if (ldev->is_opened) {
            ldev->is_to_remove = true;
        } else {
            vsf_usbh_libusb_free(ldev);
        }
    }
}

static const vsf_usbh_dev_id_t vsf_usbh_libusb_id[] = {
    {
        .match_dev_lo = 1,
        .bcdDevice_lo = 0,
        .bDeviceClass = 1,
    },
};

void vsf_usbh_libusb_set_evthandler(void *param, vsf_usbh_libusb_on_event_t on_event)
{
    vsf_usbh_libusb.cb.param = param;
    vsf_usbh_libusb.cb.on_event = on_event;
}

void vsf_usbh_libusb_close(vsf_usbh_libusb_dev_t *ldev)
{
    ldev->is_opened = false;
    if (ldev->is_to_remove) {
        vsf_usbh_libusb_free(ldev);
    }
}

vsf_err_t vsf_usbh_libusb_open(vsf_usbh_libusb_dev_t *ldev)
{
    if (ldev->is_opened) {
        return VSF_ERR_FAIL;
    } else {
        ldev->is_opened = true;
        return VSF_ERR_NONE;
    }
}

const vsf_usbh_class_drv_t vsf_usbh_libusb_drv = {
    .name       = "libusb",
    .dev_id_num = dimof(vsf_usbh_libusb_id),
    .dev_ids    = vsf_usbh_libusb_id,
    .probe      = vsf_usbh_libusb_probe,
    .disconnect = vsf_usbh_libusb_disconnect,
};

#endif
