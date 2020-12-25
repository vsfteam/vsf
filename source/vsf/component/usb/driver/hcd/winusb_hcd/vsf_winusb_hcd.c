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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_WINUSB == ENABLED

#define __VSF_USBH_CLASS_IMPLEMENT_HCD__
#define __VSF_USBH_CLASS_IMPLEMENT_HUB__
#define __VSF_EDA_CLASS_INHERIT__

#include "kernel/vsf_kernel.h"
#include "component/usb/host/vsf_usbh.h"
#include "./vsf_winusb_hcd.h"

#include <Windows.h>
#include <SetupAPI.h>
#include <winusb.h>

#if VSF_WINUSB_CFG_INSTALL_DRIVER == ENABLED
#   include "libwdi.h"
#endif

#pragma comment(lib, "SetupAPI.lib")
#pragma comment(lib, "winusb.lib")

/*============================ MACROS ========================================*/

#ifndef VSF_WINUSB_CFG_WIN7
#   define VSF_WINUSB_CFG_WIN7              DISABLED
#endif

#define VSF_EVT_WINUSB_HCD_BASE             ((VSF_EVT_USER + 0x100) & ~0xFF)

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_WINUSB_HCD_DEF_DEV(__N, __BIT)                                      \
            {                                                                   \
                .vid = VSF_WINUSB_HCD_DEV##__N##_VID,                           \
                .pid = VSF_WINUSB_HCD_DEV##__N##_PID,                           \
                .addr = -1,                                                     \
            },

/*============================ TYPES =========================================*/

typedef struct vk_winusb_hcd_dev_ep_t {
    int8_t ep2ifs;
#if VSF_WINUSB_CFG_WIN7 == ENABLED
    WINUSB_PIPE_INFORMATION pipe_info;
#else
    WINUSB_PIPE_INFORMATION_EX pipe_info;
#endif
} vk_winusb_hcd_dev_ep_t;

typedef struct vk_winusb_hcd_dev_t {
    uint16_t vid, pid;
    HANDLE hDev;
    WINUSB_INTERFACE_HANDLE hUsbIfs[8];
    vk_usbh_dev_t *dev;

    enum usb_device_speed_t speed;
    enum {
        VSF_WINUSB_HCD_DEV_STATE_DETACHED,
        VSF_WINUSB_HCD_DEV_STATE_DETACHING,
        VSF_WINUSB_HCD_DEV_STATE_ATTACHED,
    } state;

    int8_t addr;
    uint8_t ifs_num;
    union {
        uint8_t value;
        struct {
            uint8_t is_resetting    : 1;
            uint8_t is_attaching    : 1;
            uint8_t is_detaching    : 1;
            uint8_t is_detached     : 1;
        };
    } evt_mask;

    vsf_arch_irq_thread_t irq_thread;
    vsf_arch_irq_request_t irq_request;
    vsf_dlist_t urb_pending_list;

    vk_winusb_hcd_dev_ep_t ep[2 * 16];
} vk_winusb_hcd_dev_t;

typedef struct vk_winusb_hcd_t {
    vk_winusb_hcd_dev_t devs[VSF_WINUSB_HCD_CFG_DEV_NUM];
    vk_usbh_hcd_t *hcd;
    uint32_t new_mask;
    uint8_t cur_dev_idx;

    vsf_eda_t *init_eda;
    vsf_arch_irq_thread_t init_thread;
    vsf_teda_t teda;
    vsf_sem_t sem;
    vsf_dlist_t urb_list;
} vk_winusb_hcd_t;

typedef struct vk_winusb_hcd_urb_t {
    vsf_dlist_node_t urb_node;
    vsf_dlist_node_t urb_pending_node;

    enum {
        VSF_WINUSB_HCD_URB_STATE_IDLE,
        VSF_WINUSB_HCD_URB_STATE_QUEUED,
        VSF_WINUSB_HCD_URB_STATE_SUBMITTING,
        VSF_WINUSB_HCD_URB_STATE_WAIT_TO_FREE,
        VSF_WINUSB_HCD_URB_STATE_TO_FREE,
    } state;

    bool is_irq_enabled;
    bool is_msg_processed;

    vsf_arch_irq_thread_t irq_thread;
    vsf_arch_irq_request_t irq_request;
} vk_winusb_hcd_urb_t;

enum {
    VSF_EVT_WINUSB_HCD_ATTACH           = VSF_EVT_WINUSB_HCD_BASE + 0x100,
    VSF_EVT_WINUSB_HCD_DETACH           = VSF_EVT_WINUSB_HCD_BASE + 0x200,
    VSF_EVT_WINUSB_HCD_READY            = VSF_EVT_WINUSB_HCD_BASE + 0x300,
};

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_winusb_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_winusb_hcd_fini(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_winusb_hcd_suspend(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_winusb_hcd_resume(vk_usbh_hcd_t *hcd);
static uint_fast16_t __vk_winusb_hcd_get_frame_number(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_winusb_hcd_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static void __vk_winusb_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static vk_usbh_hcd_urb_t * __vk_winusb_hcd_alloc_urb(vk_usbh_hcd_t *hcd);
static void __vk_winusb_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_winusb_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_winusb_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_winusb_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static bool __vk_winusb_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);

static void __vk_winusb_hcd_dev_thread(void *arg);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_hcd_drv_t vk_winusb_hcd_drv = {
    .init_evthandler    = __vk_winusb_hcd_init_evthandler,
    .fini               = __vk_winusb_hcd_fini,
    .suspend            = __vk_winusb_hcd_suspend,
    .resume             = __vk_winusb_hcd_resume,
    .get_frame_number   = __vk_winusb_hcd_get_frame_number,
    .alloc_device       = __vk_winusb_hcd_alloc_device,
    .free_device        = __vk_winusb_hcd_free_device,
    .alloc_urb          = __vk_winusb_hcd_alloc_urb,
    .free_urb           = __vk_winusb_hcd_free_urb,
    .submit_urb         = __vk_winusb_hcd_submit_urb,
    .relink_urb         = __vk_winusb_hcd_relink_urb,
    .reset_dev          = __vk_winusb_hcd_reset_dev,
    .is_dev_reset       = __vk_winusb_hcd_is_dev_reset,
};

/*============================ LOCAL VARIABLES ===============================*/

static vk_winusb_hcd_t __vk_winusb_hcd = {
    .devs = {
        REPEAT_MACRO(VSF_WINUSB_HCD_CFG_DEV_NUM, VSF_WINUSB_HCD_DEF_DEV, NULL)
    },
};

/*============================ IMPLEMENTATION ================================*/

#if VSF_WINUSB_CFG_INSTALL_DRIVER == ENABLED
static bool __vk_winusb_ensure_driver(uint_fast16_t vid, uint_fast16_t pid, bool force)
{
    struct wdi_options_create_list cl_options = {
        .list_all           = TRUE,
        .list_hubs          = TRUE,
        .trim_whitespaces   = TRUE,
    };
    struct wdi_options_prepare_driver pd_options = {
        .driver_type        = WDI_WINUSB,
    };
    struct wdi_device_info *device, *list;
    int r = WDI_ERROR_OTHER;

    wdi_set_log_level(3);
    r = wdi_create_list(&list, &cl_options);
    if (r != WDI_SUCCESS) {
        return false;
    }

    for (device = list; device != NULL; device = device->next) {
        if (    (device->vid == vid) && (device->pid == pid) && !device->is_composite
            &&  (force || strcmp(device->driver, "WinUSB"))) {
            if (wdi_prepare_driver(device, "usb_driver", "winusb_device.inf", &pd_options) == WDI_SUCCESS) {
                wdi_install_driver(device, "usb_driver", "winusb_device.inf", NULL);
            }
            break;
        }
    }
    wdi_destroy_list(list);
    return true;
}
#endif

static HANDLE __vk_winusb_open_device(uint_fast16_t vid, uint_fast16_t pid)
{
    HANDLE hDev = INVALID_HANDLE_VALUE;
    HDEVINFO hDeviceInfo = SetupDiGetClassDevsA(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (INVALID_HANDLE_VALUE == hDeviceInfo) {
        return INVALID_HANDLE_VALUE;
    }

    SP_DEVINFO_DATA DeviceInfoData = {
        .cbSize = sizeof(SP_DEVINFO_DATA),
    };
    PSP_DEVICE_INTERFACE_DETAIL_DATA_A pInterfaceDetailData = NULL;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    ULONG requiredLength = 0;
    for (int i = 0; SetupDiEnumDeviceInfo(hDeviceInfo, i, &DeviceInfoData); i++) {
        if (pInterfaceDetailData) {
            LocalFree(pInterfaceDetailData);
            pInterfaceDetailData = NULL;
        }

        deviceInterfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);
        const GUID GUID_DEVINTERFACE_USB_DEVICE = {0xA5DCBF10, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};
        if (!SetupDiEnumDeviceInterfaces(hDeviceInfo, &DeviceInfoData, &GUID_DEVINTERFACE_USB_DEVICE, 0, &deviceInterfaceData)) {
            continue;
        }

        if (!SetupDiGetDeviceInterfaceDetailA(hDeviceInfo, &deviceInterfaceData, NULL, 0, &requiredLength, NULL)) {
            if ((ERROR_INSUFFICIENT_BUFFER == GetLastError()) && (requiredLength > 0)) {
                //we got the size, allocate buffer
                pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)LocalAlloc(LPTR, requiredLength);
                if (!pInterfaceDetailData) {
                    goto done;
                }
            } else {
                goto done;
            }
        }

        pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
        if (!SetupDiGetDeviceInterfaceDetailA(hDeviceInfo, &deviceInterfaceData, pInterfaceDetailData, requiredLength, NULL, NULL)) {
            goto done;
        }

        char str_tmp[9];
        sprintf(str_tmp, "vid_%04x", vid);
        if (NULL != strstr(pInterfaceDetailData->DevicePath, str_tmp)) {
            sprintf(str_tmp, "pid_%04x", pid);
            if (NULL != strstr(pInterfaceDetailData->DevicePath, str_tmp)) {
                vsf_trace_debug("Device path: %s" VSF_TRACE_CFG_LINEEND, pInterfaceDetailData->DevicePath);
                hDev = CreateFileA(pInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
                if (hDev != INVALID_HANDLE_VALUE) {
                    goto done;
                }
            }
        }
    }

done:
    if (pInterfaceDetailData) {
        LocalFree(pInterfaceDetailData);
    }
    SetupDiDestroyDeviceInfoList(hDeviceInfo);
    return hDev;
}

static void __vk_winusb_hcd_on_left(vk_winusb_hcd_dev_t *winusb_dev)
{
    winusb_dev->evt_mask.is_detaching = true;
    __vsf_arch_irq_request_send(&winusb_dev->irq_request);
}

static void __vk_winusb_hcd_on_arrived(vk_winusb_hcd_dev_t *winusb_dev)
{
    UCHAR speed;
    ULONG length = sizeof(speed);
    WinUsb_QueryDeviceInformation(winusb_dev->hUsbIfs[0], DEVICE_SPEED, &length, &speed);
    switch (speed) {
    case LowSpeed:  winusb_dev->speed = USB_SPEED_LOW;      break;
    case FullSpeed: winusb_dev->speed = USB_SPEED_FULL;     break;
    case HighSpeed: winusb_dev->speed = USB_SPEED_HIGH;     break;
    default:        winusb_dev->speed = USB_SPEED_UNKNOWN;  break;
    }
    winusb_dev->evt_mask.is_attaching = true;
    __vsf_arch_irq_request_send(&winusb_dev->irq_request);
}

static void __vk_winusb_hcd_assign_endpoints(vk_winusb_hcd_dev_t *winusb_dev, uint_fast8_t ifs, uint_fast8_t alt)
{
    USB_INTERFACE_DESCRIPTOR ifs_desc;
#if VSF_WINUSB_CFG_WIN7 == ENABLED
    WINUSB_PIPE_INFORMATION pipe;
#else
    WINUSB_PIPE_INFORMATION_EX pipe;
#endif
    uint_fast8_t idx;

    VSF_USB_ASSERT(ifs < winusb_dev->ifs_num);
    do {
        if (!WinUsb_QueryInterfaceSettings(winusb_dev->hUsbIfs[ifs], alt, &ifs_desc)) {
            break;
        }
        for (uint_fast8_t i = 0; i < ifs_desc.bNumEndpoints; i++) {
#if VSF_WINUSB_CFG_WIN7 == ENABLED
            if (!WinUsb_QueryPipe(winusb_dev->hUsbIfs[ifs], alt, i, &pipe)) {
#else
            if (!WinUsb_QueryPipeEx(winusb_dev->hUsbIfs[ifs], alt, i, &pipe)) {
#endif
                break;
            }
            idx = USB_ENDPOINT_DIRECTION_IN(pipe.PipeId) ? 0x10 : 0x00;
            idx += pipe.PipeId & USB_ENDPOINT_ADDRESS_MASK;
            winusb_dev->ep[idx].ep2ifs = ifs;
            winusb_dev->ep[idx].pipe_info = pipe;
        }
    } while (0);
}

// return 0 on success, non-0 otherwise
static int __vk_winusb_hcd_init(void)
{
    vk_winusb_hcd_param_t *param = __vk_winusb_hcd.hcd->param;
    vk_winusb_hcd_dev_t *winusb_dev;

    for (int i = 0; i < dimof(__vk_winusb_hcd.devs); i++) {
        winusb_dev = &__vk_winusb_hcd.devs[i];
        winusb_dev->state = VSF_WINUSB_HCD_DEV_STATE_DETACHED;
        winusb_dev->evt_mask.value = 0;
        winusb_dev->hDev = INVALID_HANDLE_VALUE;
        winusb_dev->addr = -1;

        __vsf_arch_irq_request_init(&winusb_dev->irq_request);
        __vsf_arch_irq_init(&winusb_dev->irq_thread, "winusb_hcd_dev", __vk_winusb_hcd_dev_thread, param->priority);
    }
    return 0;
}

static bool __vk_winusb_hcd_sumbit_urb_epnz(vk_usbh_hcd_urb_t *urb, WINUSB_INTERFACE_HANDLE handle, PULONG real_size)
{
    vk_usbh_pipe_t pipe = urb->pipe;
    bool result;

    if (pipe.type == USB_ENDPOINT_XFER_ISOC) {
#if VSF_WINUSB_CFG_WIN7 == ENABLED
        // TODO: add iso support for win7
        return false;
#else
        VSF_USB_ASSERT(urb->iso_packet.number_of_packets > 0);
        for (int i = 0; i < urb->iso_packet.number_of_packets; i++) {
            urb->iso_packet.frame_desc[i].actual_length = 0;
        }

        WINUSB_ISOCH_BUFFER_HANDLE buffer_handle;
        result = WinUsb_RegisterIsochBuffer(handle, pipe.endpoint | (pipe.dir_in1out0 ? 0x80 : 0x00), urb->buffer, urb->transfer_length, &buffer_handle);
        if (!result) {
            return result;
        }

        if (pipe.dir_in1out0) {
            USBD_ISO_PACKET_DESCRIPTOR desc[urb->iso_packet.number_of_packets];
            result = WinUsb_ReadIsochPipeAsap(buffer_handle, 0, urb->transfer_length, FALSE,
                        urb->iso_packet.number_of_packets, (PUSBD_ISO_PACKET_DESCRIPTOR)&desc, NULL);

            if (result) {
                *real_size = 0;
                for (int i = 0; i < urb->iso_packet.number_of_packets; i++) {
                    urb->iso_packet.frame_desc[i].actual_length = desc[i].Length;
                    *real_size += desc[i].Length;
                    urb->iso_packet.frame_desc[i].status = desc[i].Status;
                }
            }
        } else {
            result = WinUsb_WriteIsochPipeAsap(buffer_handle, 0, urb->transfer_length, FALSE, NULL);
            for (int i = 0; i < urb->iso_packet.number_of_packets; i++) {
                urb->iso_packet.frame_desc[i].status = result ? URB_OK : URB_FAIL;
            }
        }
        WinUsb_UnregisterIsochBuffer(buffer_handle);
#endif
    } else {
        if (pipe.dir_in1out0) {
            result = WinUsb_ReadPipe(handle, 0x80 | pipe.endpoint, urb->buffer, urb->transfer_length, real_size, NULL);
        } else {
            result = WinUsb_WritePipe(handle, pipe.endpoint, urb->buffer, urb->transfer_length, real_size, NULL);
        }
    }
    return result;
}

static int __vk_winusb_hcd_submit_urb_do(vk_usbh_hcd_urb_t *urb)
{
    vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
    vk_winusb_hcd_dev_t *winusb_dev = dev->dev_priv;
    vk_usbh_pipe_t pipe = urb->pipe;
    ULONG real_size = 0;

    if (NULL == winusb_dev->hDev) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    switch (pipe.type) {
    case USB_ENDPOINT_XFER_CONTROL:
        if (pipe.endpoint != 0) {
            return VSF_ERR_INVALID_PARAMETER;
        } else {
            struct usb_ctrlrequest_t *setup = &urb->setup_packet;

            if (USB_REQ_SET_INTERFACE == setup->bRequest) {
                VSF_USB_ASSERT(setup->wIndex < winusb_dev->ifs_num);
                if (!WinUsb_SetCurrentAlternateSetting(winusb_dev->hUsbIfs[setup->wIndex], setup->wValue)) {
                    return -GetLastError();
                }
                __vk_winusb_hcd_assign_endpoints(winusb_dev, setup->wIndex, setup->wValue);
                return 0;
            } else {
                WINUSB_SETUP_PACKET SetupPacket = {
                    .RequestType    = setup->bRequestType,
                    .Request        = setup->bRequest,
                    .Value          = setup->wValue,
                    .Index          = setup->wIndex,
                    .Length         = setup->wLength,
                };

                if (USB_REQ_SET_CONFIGURATION == setup->bRequest) {
                    // TODO: update winusbh_dev->ifs_num
                    // workaround: set ifs_num large enough
                    winusb_dev->ifs_num = 0xFF;
                }
                if (!WinUsb_ControlTransfer(winusb_dev->hUsbIfs[0], SetupPacket, urb->buffer,
                            setup->wLength, &real_size, NULL)) {
                    return -GetLastError();
                }
                return real_size;
            }
        }
    case USB_ENDPOINT_XFER_ISOC:
    case USB_ENDPOINT_XFER_BULK:
    case USB_ENDPOINT_XFER_INT: {
            uint8_t ep_idx = pipe.endpoint | (pipe.dir_in1out0 ? 0x10 : 0);
            int8_t ifs_idx = winusb_dev->ep[ep_idx].ep2ifs;
            VSF_USB_ASSERT(ifs_idx >= 0);
            WINUSB_INTERFACE_HANDLE handle = winusb_dev->hUsbIfs[ifs_idx];

            if (!__vk_winusb_hcd_sumbit_urb_epnz(urb, handle, &real_size)) {
                return -GetLastError();
            }
            return real_size;
        }
    }
    return VSF_ERR_INVALID_PARAMETER;
}

static void __vk_winusb_hcd_dev_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_winusb_hcd_dev_t *winusb_dev = container_of(irq_thread, vk_winusb_hcd_dev_t, irq_thread);
    vsf_arch_irq_request_t *irq_request = &winusb_dev->irq_request;
    int idx = winusb_dev - &__vk_winusb_hcd.devs[0];

    __vsf_arch_irq_set_background(irq_thread);
    while (1) {
        __vsf_arch_irq_request_pend(irq_request);

        if (winusb_dev->evt_mask.is_attaching) {
            winusb_dev->evt_mask.is_attaching = false;
            __vsf_arch_irq_start(irq_thread);
                vsf_eda_post_evt(&__vk_winusb_hcd.teda.use_as__vsf_eda_t, VSF_EVT_WINUSB_HCD_ATTACH | idx);
            __vsf_arch_irq_end(irq_thread, false);
        }
        if (winusb_dev->evt_mask.is_detaching) {
            __vsf_arch_irq_start(irq_thread);
                vsf_eda_post_evt(&__vk_winusb_hcd.teda.use_as__vsf_eda_t, VSF_EVT_WINUSB_HCD_DETACH | idx);
            __vsf_arch_irq_end(irq_thread, false);
        }
        if (winusb_dev->evt_mask.is_detached) {
            for (int i = 0; (i < dimof(winusb_dev->hUsbIfs)) && (winusb_dev->hUsbIfs[i] != NULL); i++) {
                WinUsb_Free(winusb_dev->hUsbIfs[i]);
                winusb_dev->hUsbIfs[i] = NULL;
            }
            CloseHandle(winusb_dev->hDev);
            winusb_dev->hDev = INVALID_HANDLE_VALUE;
            winusb_dev->evt_mask.is_detached = false;
        }
        if (winusb_dev->evt_mask.is_resetting) {
            // TODO: reset hUsbIfs[0]
//            WinUsb_Reset(winusb_dev->hUsbIfs[0]);
            winusb_dev->evt_mask.is_resetting = false;
        }
    }
}

static void __vk_winusb_hcd_urb_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_winusb_hcd_urb_t *winusb_urb = container_of(irq_thread, vk_winusb_hcd_urb_t, irq_thread);
    vk_usbh_hcd_urb_t *urb = container_of(winusb_urb, vk_usbh_hcd_urb_t, priv);
    vsf_arch_irq_request_t *irq_request = &winusb_urb->irq_request;
    bool is_to_free;
    int actual_length;

    __vsf_arch_irq_set_background(irq_thread);
    while (1) {
        __vsf_arch_irq_request_pend(irq_request);

        while (!winusb_urb->is_msg_processed) {
            Sleep(1);
        }

        is_to_free = VSF_WINUSB_HCD_URB_STATE_TO_FREE == winusb_urb->state;
        if (!is_to_free) {
            actual_length = __vk_winusb_hcd_submit_urb_do(urb);
            if (actual_length < 0) {
                urb->status = actual_length;
            } else {
                urb->status = URB_OK;
                urb->actual_length = actual_length;
            }
        }

        __vsf_arch_irq_start(irq_thread);
            if (is_to_free) {
                winusb_urb->is_irq_enabled = false;
            }
            winusb_urb->is_msg_processed = false;
            vsf_eda_post_msg(&__vk_winusb_hcd.teda.use_as__vsf_eda_t, urb);
        __vsf_arch_irq_end(irq_thread, false);

        if (is_to_free) {
            __vsf_arch_irq_fini(irq_thread);
            __vsf_arch_irq_request_fini(irq_request);
            return;
        }
    }
}

static void __vk_winusb_hcd_init_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;

    __vsf_arch_irq_set_background(irq_thread);
        __vk_winusb_hcd_init();
    __vsf_arch_irq_start(irq_thread);
        vsf_eda_post_evt(__vk_winusb_hcd.init_eda, VSF_EVT_WINUSB_HCD_READY);
    __vsf_arch_irq_end(irq_thread, false);

    while (1) {
        vk_winusb_hcd_dev_t *winusb_dev = &__vk_winusb_hcd.devs[0];
        for (int i = 0; i < dimof(__vk_winusb_hcd.devs); i++, winusb_dev++) {
            if ((INVALID_HANDLE_VALUE == winusb_dev->hDev) && (0 == winusb_dev->evt_mask.value)) {
                __vk_winusb_ensure_driver(winusb_dev->vid, winusb_dev->pid, false);
                winusb_dev->hDev = __vk_winusb_open_device(winusb_dev->vid, winusb_dev->pid);
                if (winusb_dev->hDev != INVALID_HANDLE_VALUE) {
                    if (WinUsb_Initialize(winusb_dev->hDev, &winusb_dev->hUsbIfs[0])) {
#if VSF_WINUSB_CFG_INSTALL_DRIVER == ENABLED
                    succeed:
#endif
                        for (uint_fast8_t i = 0; i < dimof(winusb_dev->ep); i++) {
                            winusb_dev->ep[i].ep2ifs = -1;
                        }
                        winusb_dev->ifs_num = 1;
                        __vk_winusb_hcd_assign_endpoints(winusb_dev, 0, 0);
                        for (uint_fast8_t i = 0; i < dimof(winusb_dev->hUsbIfs) - 1; i++)  {
                            if (!WinUsb_GetAssociatedInterface(winusb_dev->hUsbIfs[0], i, &winusb_dev->hUsbIfs[i + 1])) {
                                break;
                            }
                            winusb_dev->ifs_num++;
                            __vk_winusb_hcd_assign_endpoints(winusb_dev, i + 1, 0);
                        }
                        __vk_winusb_hcd_on_arrived(winusb_dev);
                    } else {
                        CloseHandle(winusb_dev->hDev);
                        winusb_dev->hDev = INVALID_HANDLE_VALUE;

#if VSF_WINUSB_CFG_INSTALL_DRIVER == ENABLED
                        // fail to open device, force to change driver using libwdi
                        __vk_winusb_ensure_driver(winusb_dev->vid, winusb_dev->pid, true);

                        winusb_dev->hDev = __vk_winusb_open_device(winusb_dev->vid, winusb_dev->pid);
                        if (winusb_dev->hDev != INVALID_HANDLE_VALUE) {
                            if (WinUsb_Initialize(winusb_dev->hDev, &winusb_dev->hUsbIfs[0])) {
                                goto succeed;
                            } else {
                                CloseHandle(winusb_dev->hDev);
                                winusb_dev->hDev = INVALID_HANDLE_VALUE;
                            }
                        }
#endif
                    }
                }
            }
        }
        Sleep(100);
    }
    __vsf_arch_irq_fini(irq_thread);
}






static bool __vk_winusb_hcd_free_urb_do(vk_usbh_hcd_urb_t *urb)
{
    vk_winusb_hcd_urb_t *winusb_urb = (vk_winusb_hcd_urb_t *)urb->priv;
    if (winusb_urb->is_irq_enabled) {
        VSF_USB_ASSERT(VSF_WINUSB_HCD_URB_STATE_TO_FREE == winusb_urb->state);
        __vsf_arch_irq_request_send(&winusb_urb->irq_request);
        return false;
    } else {
        vk_usbh_hcd_urb_free_buffer(urb);
        vsf_usbh_free(urb);
        return true;
    }
}

static void __vk_winusb_hcd_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_winusb_hcd_t *winusb = container_of(eda, vk_winusb_hcd_t, teda);

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_dlist_init(&__vk_winusb_hcd.urb_list);
        vsf_eda_sem_init(&__vk_winusb_hcd.sem, 0);
        vsf_teda_set_timer_ms(100);

    wait_next_urb:
        if (vsf_eda_sem_pend(&__vk_winusb_hcd.sem, -1)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC: {
            vk_winusb_hcd_urb_t *winusb_urb;
            vsf_protect_t orig = vsf_protect_sched();
                vsf_dlist_remove_head(vk_winusb_hcd_urb_t, urb_node,
                        &__vk_winusb_hcd.urb_list, winusb_urb);
            vsf_unprotect_sched(orig);

            if (winusb_urb != NULL) {
                vk_usbh_hcd_urb_t *urb = container_of(winusb_urb, vk_usbh_hcd_urb_t, priv);

                if (VSF_WINUSB_HCD_URB_STATE_TO_FREE == winusb_urb->state) {
                    __vk_winusb_hcd_free_urb_do(urb);
                } else {
                    vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
                    vk_winusb_hcd_dev_t *winusb_dev = dev->dev_priv;

                    if (USB_ENDPOINT_XFER_CONTROL == urb->pipe.type) {
                        struct usb_ctrlrequest_t *setup = &urb->setup_packet;

                        // set address is handled here
                        if ((USB_RECIP_DEVICE | USB_DIR_OUT) == setup->bRequestType) {
                            if (USB_REQ_SET_ADDRESS == setup->bRequest) {
                                VSF_USB_ASSERT(0 == winusb_dev->addr);
                                winusb_dev->addr = setup->wValue;
                                urb->status = URB_OK;
                                urb->actual_length = 0;

                                VSF_USB_ASSERT(vsf_dlist_is_empty(&winusb_dev->urb_pending_list));
                                vsf_dlist_add_to_tail(vk_winusb_hcd_urb_t, urb_pending_node, &winusb_dev->urb_pending_list, winusb_urb);
                                vsf_eda_post_msg(&__vk_winusb_hcd.teda.use_as__vsf_eda_t, urb);
                                goto wait_next_urb;
                            }
                        }
                    }

                    if (!urb->pipe.dir_in1out0) {
                        vk_winusb_hcd_urb_t *winusb_urb_head;

                        // add to urb_pending_list for out transfer
                        vsf_dlist_add_to_tail(vk_winusb_hcd_urb_t, urb_pending_node, &winusb_dev->urb_pending_list, winusb_urb);

                        vsf_dlist_peek_head(vk_winusb_hcd_urb_t, urb_pending_node, &winusb_dev->urb_pending_list, winusb_urb_head);
                        if (winusb_urb_head != winusb_urb) {
                            goto wait_next_urb;
                        }
                    }

                    // irq_thread will process the urb
                    winusb_urb->state = VSF_WINUSB_HCD_URB_STATE_SUBMITTING;
                    __vsf_arch_irq_request_send(&winusb_urb->irq_request);
                }
            }
            goto wait_next_urb;
        }
        break;
    case VSF_EVT_TIMER:
        if (__vk_winusb_hcd.new_mask != 0) {
            vk_usbh_t *usbh = (vk_usbh_t *)winusb->hcd;
            if (NULL == usbh->dev_new) {
                int idx = ffz(~__vk_winusb_hcd.new_mask);
                VSF_USB_ASSERT(idx < dimof(__vk_winusb_hcd.devs));
                vk_winusb_hcd_dev_t *winusb_dev = &__vk_winusb_hcd.devs[idx];
                VSF_USB_ASSERT(vsf_dlist_is_empty(&winusb_dev->urb_pending_list));
                __vk_winusb_hcd.cur_dev_idx = idx;
                __vk_winusb_hcd.new_mask &= ~(1 << idx);
                winusb_dev->addr = 0;
                winusb_dev->dev = vk_usbh_new_device((vk_usbh_t *)winusb->hcd, winusb_dev->speed, NULL, 0);
                winusb_dev->state = VSF_WINUSB_HCD_DEV_STATE_ATTACHED;
            }
        }
        vsf_teda_set_timer_ms(100);
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_hcd_urb_t *urb = vsf_eda_get_cur_msg();
            VSF_USB_ASSERT((urb != NULL) && urb->pipe.is_pipe);
            vk_winusb_hcd_urb_t *winusb_urb = (vk_winusb_hcd_urb_t *)urb->priv;

            do {
                if (!urb->pipe.dir_in1out0) {
                    vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
                    if (NULL == dev) { break; }
                    vk_winusb_hcd_dev_t *winusb_dev = dev->dev_priv;
                    if (NULL == winusb_dev) { break; }
                    vk_winusb_hcd_urb_t *winusb_urb_head;

                    if (VSF_WINUSB_HCD_DEV_STATE_ATTACHED == winusb_dev->state) {
                        vsf_dlist_remove_head(vk_winusb_hcd_urb_t, urb_pending_node, &winusb_dev->urb_pending_list, winusb_urb_head);
                        VSF_USB_ASSERT(winusb_urb_head == winusb_urb);
                        vsf_dlist_peek_head(vk_winusb_hcd_urb_t, urb_pending_node, &winusb_dev->urb_pending_list, winusb_urb_head);
                        if (winusb_urb_head != NULL) {
                            winusb_urb_head->state = VSF_WINUSB_HCD_URB_STATE_SUBMITTING;
                            __vsf_arch_irq_request_send(&winusb_urb_head->irq_request);
                        }
                    }
                }
            } while (0);

            if (VSF_WINUSB_HCD_URB_STATE_TO_FREE == winusb_urb->state) {
                if (!__vk_winusb_hcd_free_urb_do(urb)) {
                    winusb_urb->is_msg_processed = true;
                }
            } else {
                vsf_eda_post_msg(urb->eda_caller, urb);
                winusb_urb->state = VSF_WINUSB_HCD_URB_STATE_IDLE;
                winusb_urb->is_msg_processed = true;
            }
        }
        break;
    default: {
            int idx = evt & 0xFF;
            VSF_USB_ASSERT(idx < dimof(__vk_winusb_hcd.devs));
            vk_winusb_hcd_dev_t *winusb_dev = &__vk_winusb_hcd.devs[idx];

            switch (evt & ~0xFF) {
            case VSF_EVT_WINUSB_HCD_ATTACH:
                if (winusb_dev->state != VSF_WINUSB_HCD_DEV_STATE_ATTACHED) {
                    __vk_winusb_hcd.new_mask |= 1 << idx;
                }
                break;
            case VSF_EVT_WINUSB_HCD_DETACH:
                if (winusb_dev->state != VSF_WINUSB_HCD_DEV_STATE_DETACHED) {
                    winusb_dev->state = VSF_WINUSB_HCD_DEV_STATE_DETACHED;
                    vk_usbh_disconnect_device((vk_usbh_t *)winusb->hcd, winusb_dev->dev);
                    vsf_dlist_init(&winusb_dev->urb_pending_list);
                    winusb_dev->evt_mask.is_detached = true;
                    winusb_dev->evt_mask.is_detaching = false;
                    __vsf_arch_irq_request_send(&winusb_dev->irq_request);
                } else {
                    winusb_dev->evt_mask.is_detaching = false;
                }
                break;
            }
        }
    }
}

static vsf_err_t __vk_winusb_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd)
{
    vk_winusb_hcd_param_t *param = hcd->param;

    switch (evt) {
    case VSF_EVT_INIT:
        __vk_winusb_hcd.hcd = hcd;
        __vk_winusb_hcd.new_mask = 0;
        __vk_winusb_hcd.cur_dev_idx = 0;

        __vk_winusb_hcd.teda.fn.evthandler = __vk_winusb_hcd_evthandler;
        vsf_teda_init(&__vk_winusb_hcd.teda, vsf_prio_inherit, false);

        __vk_winusb_hcd.init_eda = eda;
        __vsf_arch_irq_init(&__vk_winusb_hcd.init_thread, "winusb_hcd_init", __vk_winusb_hcd_init_thread, param->priority);
        break;
    case VSF_EVT_WINUSB_HCD_READY:
        return VSF_ERR_NONE;
    }
    return VSF_ERR_NOT_READY;
}

static vsf_err_t __vk_winusb_hcd_fini(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_winusb_hcd_suspend(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_winusb_hcd_resume(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static uint_fast16_t __vk_winusb_hcd_get_frame_number(vk_usbh_hcd_t *hcd)
{
    return 0;
}

static vsf_err_t __vk_winusb_hcd_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    VSF_USB_ASSERT(__vk_winusb_hcd.cur_dev_idx < dimof(__vk_winusb_hcd.devs));
    dev->dev_priv = &__vk_winusb_hcd.devs[__vk_winusb_hcd.cur_dev_idx];
    return VSF_ERR_NONE;
}

static void __vk_winusb_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    dev->dev_priv = NULL;
}

static vk_usbh_hcd_urb_t * __vk_winusb_hcd_alloc_urb(vk_usbh_hcd_t *hcd)
{
    uint_fast32_t size = sizeof(vk_usbh_hcd_urb_t) + sizeof(vk_winusb_hcd_urb_t);
    vk_usbh_hcd_urb_t *urb = vsf_usbh_malloc(size);

    if (urb != NULL) {
        memset(urb, 0, size);

        vk_winusb_hcd_urb_t *winusb_urb = (vk_winusb_hcd_urb_t *)urb->priv;
        vk_winusb_hcd_param_t *param = __vk_winusb_hcd.hcd->param;
        __vsf_arch_irq_request_init(&winusb_urb->irq_request);
        winusb_urb->is_msg_processed = true;
        winusb_urb->is_irq_enabled = true;
        __vsf_arch_irq_init(&winusb_urb->irq_thread, "winusb_hcd_urb", __vk_winusb_hcd_urb_thread, param->priority);
    }
    return urb;
}

static void __vk_winusb_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_winusb_hcd_urb_t *winusb_urb = (vk_winusb_hcd_urb_t *)urb->priv;
    if (VSF_WINUSB_HCD_URB_STATE_TO_FREE != winusb_urb->state) {
        vsf_protect_t orig = vsf_protect_int();
            winusb_urb->state = VSF_WINUSB_HCD_URB_STATE_TO_FREE;
        if (winusb_urb->is_irq_enabled) {
            vsf_unprotect_int(orig);
            __vsf_arch_irq_request_send(&winusb_urb->irq_request);
            return;
        }
        vsf_unprotect_int(orig);
    }
}

static vsf_err_t __vk_winusb_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_winusb_hcd_urb_t *winusb_urb = (vk_winusb_hcd_urb_t *)urb->priv;
    vsf_dlist_init_node(vk_winusb_hcd_urb_t, urb_node, winusb_urb);
    vsf_dlist_init_node(vk_winusb_hcd_urb_t, urb_pending_node, winusb_urb);
    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_add_to_tail(vk_winusb_hcd_urb_t, urb_node, &__vk_winusb_hcd.urb_list, winusb_urb);
        winusb_urb->state = VSF_WINUSB_HCD_URB_STATE_QUEUED;
    vsf_unprotect_sched(orig);
    vsf_eda_sem_post(&__vk_winusb_hcd.sem);
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_winusb_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return __vk_winusb_hcd_submit_urb(hcd, urb);
}

static vsf_err_t __vk_winusb_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    vk_winusb_hcd_dev_t *winusb_dev = (vk_winusb_hcd_dev_t *)dev->dev_priv;
    winusb_dev->evt_mask.is_resetting = true;
    __vsf_arch_irq_request_send(&winusb_dev->irq_request);
    return VSF_ERR_NONE;
}

static bool __vk_winusb_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    vk_winusb_hcd_dev_t *winusb_dev = (vk_winusb_hcd_dev_t *)dev->dev_priv;
    return winusb_dev->evt_mask.is_resetting;
}

#endif
