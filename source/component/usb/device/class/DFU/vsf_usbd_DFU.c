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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DFU == ENABLED && VSF_USBD_CFG_RAW_MODE != ENABLED

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

VSF_CAL_WEAK(vsf_usbd_dfu_download)
vsf_err_t vsf_usbd_dfu_download(vk_usbd_dfu_t *dfu, uint32_t addr, uint8_t *buffer, uint16_t size)
{
    return VSF_ERR_NONE;
}

void vk_usbd_dfu_downloaded(vk_usbd_dfu_t *dfu, vsf_err_t result)
{
    if (VSF_ERR_NONE == result) {
        dfu->addr += dfu->cur_size;
        dfu->block_idx++;
        dfu->status.bState = DFU_dfuDNLOAD_IDLE;
        dfu->status.bStatus = DFU_OK;
    } else {
        dfu->status.bState = DFU_dfuERROR;
        dfu->status.bStatus = DFU_errPROG;
    }
}

static vsf_err_t __vk_usbd_dfu_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_dfu_t *dfu = ifs->class_param;

    dfu->ifs = ifs;
    dfu->dev = dev;
    dfu->block_idx = 0;
    dfu->addr = 0;
    dfu->status.bState = DFU_dfuIDLE;
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_dfu_fini(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbd_dfu_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_dfu_t *dfu = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;
    uint8_t *buffer = NULL;
    uint_fast32_t size = 0;
    vsf_err_t result = VSF_ERR_FAIL;

    switch (request->bRequest) {
    case USB_DFUREQ_DNLOAD:
        if (request->wValue == dfu->block_idx) {
            buffer = dfu->transfer_buffer;
            size = dfu->transfer_size;
            result = VSF_ERR_NONE;
        }
        break;
    case USB_DFUREQ_UPLOAD:
        break;
    case USB_DFUREQ_GETSTATUS:
        buffer = (uint8_t *)&dfu->status;
        size = sizeof(dfu->status);
        result = VSF_ERR_NONE;
        break;
    case USB_DFUREQ_ABORT:
    case USB_DFUREQ_CLRSTATUS:
        result = VSF_ERR_NONE;
        break;
    case USB_DFUREQ_GETSTATE:
        buffer = (uint8_t *)&dfu->status.bState;
        size = sizeof(dfu->status.bState);
        result = VSF_ERR_NONE;
        break;
    }

    ctrl_handler->trans.buffer = buffer;
    ctrl_handler->trans.size = size;
    return result;
}

static vsf_err_t __vk_usbd_dfu_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_usbd_dfu_t *dfu = ifs->class_param;
    vk_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
    struct usb_ctrlrequest_t *request = &ctrl_handler->request;

    switch (request->bRequest) {
    case USB_DFUREQ_DNLOAD:
        dfu->cur_size = request->wLength;
        dfu->status.bState = (dfu->cur_size > 0) ? DFU_dfuDNLOAD_SYNC : DFU_dfuMANIFEST_SYNC;
        break;
    case USB_DFUREQ_UPLOAD:
        break;
    case USB_DFUREQ_GETSTATUS:
        switch (dfu->status.bState) {
        case DFU_dfuDNLOAD_SYNC:
            vsf_usbd_dfu_download(dfu, dfu->addr, dfu->transfer_buffer, dfu->cur_size);
            break;
        case DFU_dfuMANIFEST_SYNC:
            dfu->status.bState = DFU_dfuMANIFEST;
            break;
        }
        break;
    case USB_DFUREQ_CLRSTATUS:
        dfu->status.bStatus = DFU_OK;
        dfu->status.bState = DFU_dfuIDLE;
        break;
    case USB_DFUREQ_ABORT:
        dfu->status.bState = DFU_dfuIDLE;
        break;
    default:
        return VSF_ERR_FAIL;
    }

    return VSF_ERR_NONE;
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DFU
