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

#include "vsf.h"

/*============================ MACROS ========================================*/

//#define VSF_DFU_CFG_DEBUG_EN                ENABLED

#ifndef VSF_DFU_CFG_TRANSFER_SIZE
#   define VSF_DFU_CFG_TRANSFER_SIZE        256
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    DFU_DNLOAD                  = 1,
    DFU_UPLOAD                  = 2,
    DFU_GETSTATUS               = 3,
    DFU_CLRSTATUS               = 4,
    DFU_GETSTATE                = 5,
    DFU_ABORT                   = 6,
};

enum {
    DFU_OK                      = 0,
    DFU_errTARGET               = 1,
    DFU_errFILE                 = 2,
    DFU_errWRITE                = 3,
    DFU_errERASE                = 4,
    DFU_errCHECK_ERASED         = 5,
    DFU_errPROG                 = 6,
    DFU_erVERIFY                = 7,
    DFU_errADDRESS              = 8,
    DFU_errNOTDONE              = 9,
    DFU_errFIRMWARE             = 10,
    DFU_errVENDOR               = 11,
    DFU_errUSBR                 = 12,
    DFU_errPOR                  = 13,
    DFU_errUNKNOWN              = 14,
    DFU_errSTALLEDPKT           = 15,
};

enum {
    DFU_appIDLE                 = 0,
    DFU_appDETACH               = 1,
    DFU_dfuIDLE                 = 2,
    DFU_dfuDNLOAD_SYNC          = 3,
    DFU_dfuDNBUSY               = 4,
    DFU_dfuDNLOAD_IDLE          = 5,
    DFU_dfuMANIFEST_SYNC        = 6,
    DFU_dfuMANIFEST             = 7,
    DFU_dfuMANIFEST_WAIT_RESET  = 8,
    DFU_dfuUPLOAD_IDLE          = 9,
    DFU_dfuERROR                = 10,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const uint8_t __usbd_dev_desc[] = {
    USB_DESC_DEV(0x0210, 0, 0, 0, APP_CFG_USBD_EP0_SIZE, APP_CFG_USBD_VID, APP_CFG_USBD_PID, 0x0000, 1, 2, 3, 1)
};
static const uint8_t __usbd_cfg_desc[] = {
    USB_DESC_CFG(USB_DT_CONFIG_SIZE + USB_DT_INTERFACE_SIZE + 9, 1, 1, 0, USB_CONFIG_ATT_ONE, 100)
    USB_DESC_IFS(0, 0, 0, 0xFE, 0x01, 0x02, 0)

    // dfu functional descriptor
    0x09,       // bLength
    0x21,       // bDescriptorType = DFU_FUNCTINAL
    0x01,       // bmAttributes, bitCanDnload
    0x00, 0x00, // wDetachTimeOut
    USB_DESC_WORD(VSF_DFU_CFG_TRANSFER_SIZE),
                // wTransferSize
    USB_DESC_WORD(0x0110),
                // bcdDFUVersion: 1.10
};
static const uint8_t __usbd_langid_str_desc[] = {
    4,
    USB_DT_STRING,
    0x09, 0x04,
};
struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    USB_UNICODE str[dimof(APP_CFG_USBD_VENDOR_STR) - 1];
} PACKED static const __usbd_vendor_str_desc = {
    .bLength            = sizeof(__usbd_vendor_str_desc),
    .bDescriptorType    = USB_DT_STRING,
    .str                = APP_CFG_USBD_VENDOR_STR,
};
struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    USB_UNICODE str[dimof(APP_CFG_USBD_PRODUCT_STR) - 1];
} PACKED static const __usbd_product_str_desc = {
    .bLength            = sizeof(__usbd_product_str_desc),
    .bDescriptorType    = USB_DT_STRING,
    .str                = APP_CFG_USBD_PRODUCT_STR,
};
struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    USB_UNICODE str[dimof(APP_CFG_USBD_SERIAL_STR) - 1];
} PACKED static const __usbd_serial_str_desc = {
    .bLength            = sizeof(__usbd_serial_str_desc),
    .bDescriptorType    = USB_DT_STRING,
    .str                = APP_CFG_USBD_SERIAL_STR,
};
const uint8_t __usbd_bos_desc[USB_DT_BOS_SIZE + USB_BOS_CAP_WEBUSB_LEN] = {
    USB_DT_BOS_SIZE,
    USB_DT_BOS,
    USB_DESC_WORD(USB_DT_BOS_SIZE + USB_BOS_CAP_WEBUSB_LEN),
    1,

    bos_desc_webusb(1, 1)
};
#define USB_BOS_STR     u"MSFT100!"
struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    USB_UNICODE str[dimof(USB_BOS_STR) - 1];
} PACKED static const __usbd_bos_str_desc = {
    .bLength            = sizeof(__usbd_bos_str_desc),
    .bDescriptorType    = USB_DT_STRING,
    .str                = USB_BOS_STR,
};
static const uint8_t __usbd_winusb_desc[] = {
    USB_DESC_WORD(40),
    0x00, 0x00,
    0x00, 0x01,
    0x04, 0x00,
    0x1,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 

    0x0,
    0x1, 
    'W', 'I', 'N', 'U', 'S', 'B', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
};
struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t prefix;
    uint8_t url[dimof(APP_CFG_USBD_WEBUSB_URL) - 1];
} PACKED static const __usbd_webusb_url_desc = {
    .bLength            = sizeof(__usbd_webusb_url_desc),
    .bDescriptorType    = USB_DT_STRING,
    .prefix             = WEBUSB_URL_PREFIX_HTTPS,
    .url                = APP_CFG_USBD_WEBUSB_URL,
};

static uint8_t __usbd_dfu_buffer[VSF_DFU_CFG_TRANSFER_SIZE];
struct {
    uint8_t bStatus;
    uint8_t bwPollTimeout[3];
    uint8_t bState;
    uint8_t iString;
} PACKED static __usbd_dfu_status;
uint16_t __usbd_dfu_block_idx;
uint16_t __usbd_dfu_buffer_size;
uint32_t __usbd_dfu_addr;

static vk_usbd_dev_t __usbd_dev = {
    .speed          = APP_CFG_USBD_SPEED,
#ifdef VSF_USBD_CFG_DRV_INTERFACE
    .drv            = &VSF_USB_DC0,
#endif
};

#if VSF_DFU_CFG_DEBUG_EN == ENABLED
#define USBD_EVT_STRING(__EVT)      [__EVT] = STR(__EVT)
static const char * __usbd_dev_debug_info[] = {
    USBD_EVT_STRING(USB_ON_ATTACH),
    USBD_EVT_STRING(USB_ON_DETACH),
    USBD_EVT_STRING(USB_ON_RESET),
    USBD_EVT_STRING(USB_ON_SETUP),
    USBD_EVT_STRING(USB_ON_ERROR),
    USBD_EVT_STRING(USB_ON_SUSPEND),
    USBD_EVT_STRING(USB_ON_RESUME),
    USBD_EVT_STRING(USB_ON_SOF),
    USBD_EVT_STRING(USB_ON_IN),
    USBD_EVT_STRING(USB_ON_NAK),
    USBD_EVT_STRING(USB_ON_OUT),
    USBD_EVT_STRING(USB_ON_STATUS),
    USBD_EVT_STRING(USB_ON_UNDERFLOW),
    USBD_EVT_STRING(USB_ON_OVERFLOW),
    USBD_EVT_STRING(USB_ON_INIT),
    USBD_EVT_STRING(USB_ON_FINI),
};
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_usbd_notify_user(vk_usbd_dev_t *dev, usb_evt_t evt, void *param)
{
    VSF_USBD_DRV_PREPARE(dev);
#if VSF_DFU_CFG_DEBUG_EN == ENABLED
    vsf_trace(VSF_TRACE_DEBUG, "usbd: %s" VSF_TRACE_CFG_LINEEND, __usbd_dev_debug_info[evt]);
#endif

    switch (evt) {
    case USB_ON_RESET:
        vk_usbd_drv_ep_add(0 | USB_DIR_OUT, USB_EP_TYPE_CONTROL, APP_CFG_USBD_EP0_SIZE);
        vk_usbd_drv_ep_add(0 | USB_DIR_IN, USB_EP_TYPE_CONTROL, APP_CFG_USBD_EP0_SIZE);
        memset(&__usbd_dfu_status, 0, sizeof(__usbd_dfu_status));
        __usbd_dfu_status.bState = DFU_dfuIDLE;
        __usbd_dfu_block_idx = 0;
        __usbd_dfu_addr = 0;
        break;
    case USB_ON_SETUP: {
            struct usb_ctrlrequest_t *request = param;
            uint8_t *buffer = NULL;
            uint_fast32_t size = 0;

#if VSF_DFU_CFG_DEBUG_EN == ENABLED
            vsf_trace(VSF_TRACE_DEBUG, "usbd_req:" VSF_TRACE_CFG_LINEEND);
            vsf_trace(VSF_TRACE_DEBUG, "\tbmRequestType: %02X" VSF_TRACE_CFG_LINEEND, request->bRequestType);
            vsf_trace(VSF_TRACE_DEBUG, "\tbRequest: %02X" VSF_TRACE_CFG_LINEEND, request->bRequest);
            vsf_trace(VSF_TRACE_DEBUG, "\twValue: %04X" VSF_TRACE_CFG_LINEEND, request->wValue);
            vsf_trace(VSF_TRACE_DEBUG, "\twIndex: %02X" VSF_TRACE_CFG_LINEEND, request->wIndex);
            vsf_trace(VSF_TRACE_DEBUG, "\twLength: %d" VSF_TRACE_CFG_LINEEND, request->wLength);
#endif

            switch (request->bRequestType & USB_TYPE_MASK) {
            case USB_TYPE_STANDARD:
                switch (request->bRequest) {
                case USB_REQ_GET_STATUS:
                    dev->ctrl_handler.reply_buffer[0] = 0;
                    dev->ctrl_handler.reply_buffer[1] = 0;
                    buffer = dev->ctrl_handler.reply_buffer;
                    size = 2;
                    break;
                case USB_REQ_GET_DESCRIPTOR:
                    switch (request->wValue >> 8) {
                    case USB_DT_DEVICE:
                        buffer = (uint8_t *)&__usbd_dev_desc;
                        size = sizeof(__usbd_dev_desc);
                        break;
                    case USB_DT_CONFIG:
                        buffer = (uint8_t *)&__usbd_cfg_desc;
                        size = sizeof(__usbd_cfg_desc);
                        break;
                    case USB_DT_BOS:
                        buffer = (uint8_t *)&__usbd_bos_desc;
                        size = sizeof(__usbd_bos_desc);
                        break;
                    case USB_DT_STRING: {
                            switch (request->wValue & 0xFF) {
                            case 0:
                                buffer = (uint8_t *)&__usbd_langid_str_desc;
                                size = sizeof(__usbd_langid_str_desc);
                                break;
                            case 1:
                                buffer = (uint8_t *)&__usbd_vendor_str_desc;
                                size = sizeof(__usbd_vendor_str_desc);
                                break;
                            case 2:
                                buffer = (uint8_t *)&__usbd_product_str_desc;
                                size = sizeof(__usbd_product_str_desc);
                                break;
                            case 3:
                                buffer = (uint8_t *)&__usbd_serial_str_desc;
                                size = sizeof(__usbd_serial_str_desc);
                                break;
                            case 0xEE:
                                buffer = (uint8_t *)&__usbd_bos_str_desc;
                                size = sizeof(__usbd_bos_str_desc);
                                break;
                            }
                        }
                        break;
                    }
                    break;
                }
                break;
            case USB_TYPE_CLASS:
                switch (request->bRequest) {
                case DFU_DNLOAD:
                    if (request->wValue == __usbd_dfu_block_idx) {
                        buffer = (uint8_t *)&__usbd_dfu_buffer;
                        size = sizeof(__usbd_dfu_buffer);
                    }
                    break;
                case DFU_UPLOAD:
                    break;
                case DFU_GETSTATUS:
                    buffer = (uint8_t *)&__usbd_dfu_status;
                    size = sizeof(__usbd_dfu_status);
                    break;
                case DFU_CLRSTATUS:
                    break;
                case DFU_GETSTATE:
                    buffer = (uint8_t *)&__usbd_dfu_status.bState;
                    size = sizeof(__usbd_dfu_status.bState);
                    break;
                case DFU_ABORT:
                    break;
                }
                break;
            case USB_TYPE_VENDOR:
                switch (request->bRequest) {
                    case 0x21:
                    switch (request->wIndex) {
                    case 0x02:
                        buffer = (uint8_t *)&__usbd_webusb_url_desc;
                        size = sizeof(__usbd_webusb_url_desc);
                        break;
                    case 0x04:
                        buffer = (uint8_t *)&__usbd_winusb_desc;
                        size = sizeof(__usbd_winusb_desc);
                        break;
                    }
                    break;
                }
                break;
            }

            dev->ctrl_handler.trans.buffer = buffer;
            dev->ctrl_handler.trans.size = size;
        }
        break;
    case USB_ON_STATUS: {
            struct usb_ctrlrequest_t *request = param;

            switch (request->bRequestType & USB_TYPE_MASK) {
            case USB_TYPE_STANDARD:
                switch (request->bRequest) {
                case USB_REQ_SET_ADDRESS:
                    vk_usbd_drv_set_address((uint8_t)request->wValue);
                    break;
                }
                break;
            case USB_TYPE_CLASS:
                switch (request->bRequest) {
                case DFU_DNLOAD:
                    __usbd_dfu_buffer_size = request->wLength;
                    if (__usbd_dfu_buffer_size > 0) {
                        __usbd_dfu_status.bState = DFU_dfuDNLOAD_SYNC;
                    } else {
                        __usbd_dfu_status.bState = DFU_dfuMANIFEST_SYNC;
                    }
                    break;
                case DFU_UPLOAD:
                    break;
                case DFU_GETSTATUS:
                    switch (__usbd_dfu_status.bState) {
                    case DFU_dfuDNLOAD_SYNC:
                        // TODO: process data
//                        __usbd_dfu_status.bStatus = usrapp_download(__usbd_dfu_addr, __usbd_dfu_buffer, __usbd_dfu_buffer_size);

                        __usbd_dfu_addr += __usbd_dfu_buffer_size;
                        __usbd_dfu_block_idx++;
                        __usbd_dfu_status.bState = DFU_dfuDNLOAD_IDLE;
                        break;
                    case DFU_dfuMANIFEST_SYNC:
                        __usbd_dfu_status.bState = DFU_dfuMANIFEST;
                        break;
                    }
                    break;
                case DFU_CLRSTATUS:
                    __usbd_dfu_status.bStatus = DFU_OK;
                    __usbd_dfu_status.bState = DFU_dfuIDLE;
                    break;
                case DFU_GETSTATE:
                    break;
                case DFU_ABORT:
                    __usbd_dfu_status.bState = DFU_dfuIDLE;
                    break;
                }
                break;
            }
        }
        break;
    }
    return VSF_ERR_NONE;
}

void vsf_dfu_start(void)
{
    vk_usbd_init(&__usbd_dev);
    vk_usbd_connect(&__usbd_dev);
}
