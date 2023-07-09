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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DFU == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBD_CLASS_INHERIT__
#define __VSF_USBD_DFU_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbd.h"
#include "./vsf_usbd_DFU.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_usbd_dfu_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_dfu_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_dfu_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vk_usbd_dfu_fini(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbd_class_op_t vk_usbd_dfu = {
    .request_prepare =  __vk_usbd_dfu_request_prepare,
    .request_process =  __vk_usbd_dfu_request_process,
    .init =             __vk_usbd_dfu_init,
    .fini =             __vk_usbd_dfu_fini,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vk_usbd_dfu_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        break;
    }
}

static vsf_err_t __vk_usbd_dfu_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_dfu_t *dfu = ifs->class_param;

    dfu->ifs = ifs;
    dfu->dev = dev;

    dfu->eda.fn.evthandler = __vk_usbd_dfu_evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    dfu->eda.on_terminate = NULL;
#endif
    return vsf_eda_init(&dfu->eda, VSF_USBD_CFG_EDA_PRIORITY);
}

static vsf_err_t __vk_usbd_dfu_fini(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_dfu_t *dfu = ifs->class_param;
    return vsf_eda_fini(&dfu->eda);
}

static vsf_err_t __vk_usbd_dfu_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_dfu_t *dfu = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint8_t *buffer = NULL;
    uint_fast32_t size = 0;

    switch (request->bRequest) {
    case USB_DFUREQ_DETACH:
        break;
    case USB_DFUREQ_DNLOAD:
        break;
    case USB_DFUREQ_UPLOAD:
        break;
    case USB_DFUREQ_GETSTATUS:
        break;
    case USB_DFUREQ_CLRSTATUS:
        break;
    case USB_DFUREQ_GETSTATE:
        break;
    case USB_DFUREQ_ABORT:
        break;
    default:
        return VSF_ERR_FAIL;
    }

    ctrl_handler->trans.buffer = buffer;
    ctrl_handler->trans.size = size;
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_dfu_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_dfu_t *dfu = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;

    switch (request->bRequest) {
    case USB_DFUREQ_DETACH:
        break;
    case USB_DFUREQ_DNLOAD:
        break;
    case USB_DFUREQ_UPLOAD:
        break;
    case USB_DFUREQ_GETSTATUS:
        break;
    case USB_DFUREQ_CLRSTATUS:
        break;
    case USB_DFUREQ_GETSTATE:
        break;
    case USB_DFUREQ_ABORT:
        break;
    default:
        return VSF_ERR_FAIL;
    }

    return VSF_ERR_NONE;
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DFU
