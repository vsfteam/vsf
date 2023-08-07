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

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_HID == ENABLED && VSF_USBD_USE_MSC == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBD_CLASS_INHERIT__
#define __VSF_USBD_HID_CLASS_IMPLEMENT
#define __VSF_USBD_HIDMSC_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbd.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static vk_usbd_desc_t * __vk_usbd_hidmsc_get_desc(vk_usbd_dev_t *dev,
            uint_fast8_t type, uint_fast8_t index, uint_fast16_t lanid);
static vsf_err_t __vk_usbd_hidmsc_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_hidmsc_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_hidmsc_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_hidmsc_fini(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbd_class_op_t vk_usbd_hidmsc = {
    .get_desc =         __vk_usbd_hidmsc_get_desc,
    .request_prepare =  __vk_usbd_hidmsc_request_prepare,
    .request_process =  __vk_usbd_hidmsc_request_process,
    .init =             __vk_usbd_hidmsc_init,
    .fini =             __vk_usbd_hidmsc_fini,
};

/*============================ LOCAL VARIABLES ===============================*/

static const uint8_t __vk_usbd_mschid_report_desc[__USB_HIDMSC_REPORT_DESC_SIZE] = {
    0x06, 0x00, 0xFF,   // Usage Page (Vendor)
    0x09, 0x01,         // Usage (0x01)
    0xA1, 0x01,         // Collection (Application)
    0x15, 0x00,         //   Logical Minimum (0)
    0x26, 0xFF, 0x00,   //   Logical Maximum (255)
    0x75, 0x08,         //   Report Size (8)
    0x96, (512 >> 0) & 0xFF, (512 >> 8) & 0xFF,
                        //   Report Count (512)
    0x09, 0x01,         //   Usage (0x01)
    0x81, 0x02,         //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x96, (512 >> 0) & 0xFF, (512 >> 8) & 0xFF,
                        //   Report Count (512)
    0x09, 0x01,         //   Usage (0x01)
    0x91, 0x02,         //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,               // End Collection
};

static const vk_usbd_desc_t __vk_usbd_mschid_desc[1] = {
    VSF_USBD_DESC_HID_REPORT((__vk_usbd_mschid_report_desc), sizeof(__vk_usbd_mschid_report_desc)),
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vk_usbd_desc_t * __vk_usbd_hidmsc_get_desc(vk_usbd_dev_t *dev,
            uint_fast8_t type, uint_fast8_t index, uint_fast16_t lanid)
{
    return vk_usbd_hid.get_desc(dev, type, index, lanid);
}

static vsf_err_t __vk_usbd_hidmsc_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    return vk_usbd_hid.request_prepare(dev, ifs);
}

static vsf_err_t __vk_usbd_hidmsc_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    return vk_usbd_hid.request_process(dev, ifs);
}

static vsf_err_t __vk_usbd_hidmsc_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_hidmsc_t *hidmsc = ifs->class_param;

    hidmsc->desc_num = dimof(__vk_usbd_mschid_desc);
    hidmsc->desc = (vk_usbd_desc_t *)__vk_usbd_mschid_desc;
    vsf_err_t err = vk_usbd_hid.init(dev, ifs);
    if (err != VSF_ERR_NONE) {
        return err;
    }
    vsf_eda_fini(&hidmsc->teda.use_as__vsf_eda_t);

    hidmsc->msc_ifs = *ifs;
    hidmsc->msc_ifs.class_op = &vk_usbd_msc;
    hidmsc->msc_ifs.class_param = &hidmsc->msc;
    return vk_usbd_msc.init(dev, &hidmsc->msc_ifs);
}

static vsf_err_t __vk_usbd_hidmsc_fini(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    return vk_usbd_hid.fini(dev, ifs);
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_HID && VSF_USBD_USE_MSC
