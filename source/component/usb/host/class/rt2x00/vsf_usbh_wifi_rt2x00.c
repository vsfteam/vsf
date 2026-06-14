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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_RT2X00 == ENABLED && VSF_USE_WIFI == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbh.h"
#include "./vsf_usbh_wifi_rt2x00_priv.h"

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

#if VSF_WIFI_USE_RT28XX != ENABLED
#   error "USB wifi class needs at least one chip driver: enable VSF_WIFI_USE_RT28XX (or another supported chip)."
#endif

/*============================ FORWARD / EXTERN (chip drivers) ===============*/

#if VSF_WIFI_USE_RT28XX == ENABLED
extern const vsf_wifi_chip_drv_t vsf_wifi_rt28xx_drv;
#endif

/*============================ DEVICE MAP ====================================*/

/*
 * Per-chip device id / devmap segments.  Each segment is keyed on its
 * VSF_WIFI_USE_<CHIP> switch, so disabling a chip driver also strips its
 * USB VID/PID entries from the class-driver match table.
 */
#if VSF_WIFI_USE_RT28XX == ENABLED
#   define __VK_USBH_WIFI_RT28XX_DEV_IDS                                       \
        { VSF_USBH_MATCH_VID_PID(0x148F, 0x5572) },                            \
        { VSF_USBH_MATCH_VID_PID(0x148F, 0x5370) },                            \
        { VSF_USBH_MATCH_VID_PID(0x148F, 0x5372) },                            \
        { VSF_USBH_MATCH_VID_PID(0x148F, 0x2870) },                            \
        { VSF_USBH_MATCH_VID_PID(0x148F, 0x3070) },                            \
        { VSF_USBH_MATCH_VID_PID(0x148F, 0x3071) },                            \
        { VSF_USBH_MATCH_VID_PID(0x148F, 0x3072) },                            \
        { VSF_USBH_MATCH_VID_PID(0x148F, 0x3370) },                            \
        { VSF_USBH_MATCH_VID_PID(0x148F, 0x3572) },                            \
        { VSF_USBH_MATCH_VID_PID(0x148F, 0x8070) },                            \
        { VSF_USBH_MATCH_VID_PID(0x148F, 0x2070) },                            \
        { VSF_USBH_MATCH_VID_PID(0x07D1, 0x3C0D) },                            \
        { VSF_USBH_MATCH_VID_PID(0x07D1, 0x3C0E) },                            \
        { VSF_USBH_MATCH_VID_PID(0x07D1, 0x3C16) },                            \
        { VSF_USBH_MATCH_VID_PID(0x7392, 0x7711) },                            \
        { VSF_USBH_MATCH_VID_PID(0x7392, 0x7722) },
#   define __VK_USBH_WIFI_RT28XX_DEVMAP                                        \
        { 0x148F, 0x5572, &vsf_wifi_rt28xx_drv },                              \
        { 0x148F, 0x5370, &vsf_wifi_rt28xx_drv },                              \
        { 0x148F, 0x5372, &vsf_wifi_rt28xx_drv },                              \
        { 0x148F, 0x2870, &vsf_wifi_rt28xx_drv },                              \
        { 0x148F, 0x3070, &vsf_wifi_rt28xx_drv },                              \
        { 0x148F, 0x3071, &vsf_wifi_rt28xx_drv },                              \
        { 0x148F, 0x3072, &vsf_wifi_rt28xx_drv },                              \
        { 0x148F, 0x3370, &vsf_wifi_rt28xx_drv },                              \
        { 0x148F, 0x3572, &vsf_wifi_rt28xx_drv },                              \
        { 0x148F, 0x8070, &vsf_wifi_rt28xx_drv },                              \
        { 0x148F, 0x2070, &vsf_wifi_rt28xx_drv },                              \
        { 0x07D1, 0x3C0D, &vsf_wifi_rt28xx_drv },                              \
        { 0x07D1, 0x3C0E, &vsf_wifi_rt28xx_drv },                              \
        { 0x07D1, 0x3C16, &vsf_wifi_rt28xx_drv },                              \
        { 0x7392, 0x7711, &vsf_wifi_rt28xx_drv },                              \
        { 0x7392, 0x7722, &vsf_wifi_rt28xx_drv },
#else
#   define __VK_USBH_WIFI_RT28XX_DEV_IDS
#   define __VK_USBH_WIFI_RT28XX_DEVMAP
#endif

static const vk_usbh_dev_id_t __vk_usbh_wifi_dev_id[] = {
    __VK_USBH_WIFI_RT28XX_DEV_IDS
};

typedef struct {
    uint16_t idVendor;
    uint16_t idProduct;
    const vsf_wifi_chip_drv_t *drv;
} __vk_usbh_wifi_devmap_t;

static const __vk_usbh_wifi_devmap_t __vk_usbh_wifi_devmap[] = {
    __VK_USBH_WIFI_RT28XX_DEVMAP
};

/*============================ PROTOTYPES ====================================*/

static void * __vk_usbh_wifi_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_wifi_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        void *param);
static void __vk_usbh_wifi_evthandler(vsf_eda_t *eda, vsf_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_wifi_drv = {
    .name       = "wifi_usb",
    .dev_id_num = dimof(__vk_usbh_wifi_dev_id),
    .dev_ids    = __vk_usbh_wifi_dev_id,
    .probe      = __vk_usbh_wifi_probe,
    .disconnect = __vk_usbh_wifi_disconnect,
};

/*============================ HELPER: DEVMAP LOOKUP =========================*/

static const vsf_wifi_chip_drv_t * __vk_usbh_wifi_find_drv(
        uint16_t vid, uint16_t pid)
{
    for (int i = 0; i < dimof(__vk_usbh_wifi_devmap); i++) {
        if (__vk_usbh_wifi_devmap[i].idVendor  == vid &&
            __vk_usbh_wifi_devmap[i].idProduct == pid)
            return __vk_usbh_wifi_devmap[i].drv;
    }
    return NULL;
}

/*============================ BUS OPS IMPLEMENTATION ========================*
 *
 * USB reg_bus: single reg write/read via ep0 vendor request 0x01, and
 * block write via chunked ep0 transfers.  All async — completion is
 * delivered by VSF_EVT_MESSAGE on the wifi EDA.
 *
 *============================================================================*/

static vsf_err_t __usb_wifi_submit_ep0(vk_usbh_wifi_t *uwifi)
{
    vk_usbh_dev_t *dev = uwifi->dev;
    struct usb_ctrlrequest_t req;

    /* Ralink rt2x00 vendor request layout:
     *   bRequest = USB_MULTI_READ(7) / USB_MULTI_WRITE(6)
     *   wValue   = 0                       (always)
     *   wIndex   = register offset / firmware base + chunk offset
     * The earlier draft used bRequest=1 (which is USB_DEVICE_MODE) and
     * stuffed the register address into wValue, so the device silently
     * dropped every transfer (-121 ERROR_SEM_TIMEOUT).  Verified against
     * drivers/net/wireless/ralink/rt2x00/rt2x00usb.{h,c}. */
#define VSF_USBH_WIFI_RT_REQ_MULTI_WRITE   6
#define VSF_USBH_WIFI_RT_REQ_MULTI_READ    7

    switch (uwifi->bus_state) {
    case VK_USBH_WIFI_BUS_REG_WRITE: {
        /* Script layer already set the ep0 urb buffer to 4 bytes LE32. */
        uint32_t *buf = (uint32_t *)vk_usbh_urb_alloc_buffer(
                &dev->ep0.urb, 4);
        if (NULL == buf) return VSF_ERR_NOT_ENOUGH_RESOURCES;
        /* val was stashed by reg_write in bus_read_out (reuse field). */
        *buf = (uint32_t)(uintptr_t)uwifi->bus_read_out;
        req.bRequestType = USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT;
        req.bRequest     = VSF_USBH_WIFI_RT_REQ_MULTI_WRITE;
        req.wValue       = 0;
        req.wIndex       = uwifi->bus_block_base; /* reg addr */
        req.wLength      = 4;
        break;
    }
    case VK_USBH_WIFI_BUS_REG_READ: {
        if (NULL == vk_usbh_urb_alloc_buffer(&dev->ep0.urb, 4))
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        req.bRequestType = USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_IN;
        req.bRequest     = VSF_USBH_WIFI_RT_REQ_MULTI_READ;
        req.wValue       = 0;
        req.wIndex       = uwifi->bus_block_base; /* reg addr */
        req.wLength      = 4;
        break;
    }
    case VK_USBH_WIFI_BUS_BLOCK_WRITE: {
        uint32_t off  = uwifi->bus_block_offset;
        uint32_t left = uwifi->bus_block_len - off;
        uint16_t chunk = uwifi->bus_block_chunk;
        if (left < chunk) chunk = (uint16_t)left;

        /* The firmware blob lives in a const array (read-only .rdata).
         * WinUsb_ControlTransfer rejects a read-only data pointer with
         * ERROR_NOACCESS (998), so copy each chunk into an HCD-managed
         * writable buffer, mirroring the REG_WRITE path above. */
        uint8_t *buf = (uint8_t *)vk_usbh_urb_alloc_buffer(&dev->ep0.urb, chunk);
        if (NULL == buf) return VSF_ERR_NOT_ENOUGH_RESOURCES;
        memcpy(buf, uwifi->bus_block_data + off, chunk);
        req.bRequestType = USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT;
        req.bRequest     = VSF_USBH_WIFI_RT_REQ_MULTI_WRITE;
        req.wValue       = 0;
        req.wIndex       = (uint16_t)(uwifi->bus_block_base + off);
        req.wLength      = chunk;
        break;
    }
    case VK_USBH_WIFI_BUS_VENDOR: {
        /* No-data-stage vendor request (e.g. USB_DEVICE_MODE / USB_MODE_
         * FIRMWARE to start the 8051 MCU, or USB_MODE_RESET).  wLength=0 so
         * there is no data buffer to allocate. */
        req.bRequestType = USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT;
        req.bRequest     = uwifi->bus_vendor_request;
        req.wValue       = uwifi->bus_vendor_value;
        req.wIndex       = uwifi->bus_vendor_index;
        req.wLength      = 0;
        break;
    }
    default:
        return VSF_ERR_BUG;
    }
    return vk_usbh_control_msg_ex(uwifi->usbh, dev, &req, 0, &uwifi->eda);
}

static void __usb_wifi_bus_finish(vk_usbh_wifi_t *uwifi, vsf_err_t err)
{
    vsf_wifi_done_t done = uwifi->bus_done;
    uwifi->bus_state = VK_USBH_WIFI_BUS_IDLE;
    uwifi->bus_done  = NULL;
    __vsf_eda_crit_npb_leave(&uwifi->dev->ep0.crit);
    if (done != NULL) {
        done(&uwifi->wifi, err);
    }
}

/* reg_bus->reg_write */
static vsf_err_t __usb_wifi_reg_write(vsf_wifi_t *wifi, uint16_t reg,
        uint32_t val, vsf_wifi_done_t done)
{
    vk_usbh_wifi_t *uwifi = vsf_container_of(wifi, vk_usbh_wifi_t, wifi);
    VSF_USB_ASSERT(uwifi->bus_state == VK_USBH_WIFI_BUS_IDLE);

    uwifi->bus_state      = VK_USBH_WIFI_BUS_REG_WRITE;
    uwifi->bus_done       = done;
    uwifi->bus_read_out   = (uint32_t *)(uintptr_t)val; /* stash val */
    uwifi->bus_block_base = reg;

    vsf_err_t err = __vsf_eda_crit_npb_enter(&uwifi->dev->ep0.crit);
    if (VSF_ERR_NONE != err) {
        uwifi->bus_pending_first = true;
        return VSF_ERR_NONE;
    }
    err = __usb_wifi_submit_ep0(uwifi);
    if (VSF_ERR_NONE != err) {
        __usb_wifi_bus_finish(uwifi, err);
        return err;
    }
    return VSF_ERR_NONE;
}

/* reg_bus->reg_read */
static vsf_err_t __usb_wifi_reg_read(vsf_wifi_t *wifi, uint16_t reg,
        uint32_t *out, vsf_wifi_done_t done)
{
    vk_usbh_wifi_t *uwifi = vsf_container_of(wifi, vk_usbh_wifi_t, wifi);
    VSF_USB_ASSERT(uwifi->bus_state == VK_USBH_WIFI_BUS_IDLE);

    uwifi->bus_state      = VK_USBH_WIFI_BUS_REG_READ;
    uwifi->bus_done       = done;
    uwifi->bus_read_out   = out;
    uwifi->bus_block_base = reg;

    vsf_err_t err = __vsf_eda_crit_npb_enter(&uwifi->dev->ep0.crit);
    if (VSF_ERR_NONE != err) {
        uwifi->bus_pending_first = true;
        return VSF_ERR_NONE;
    }
    err = __usb_wifi_submit_ep0(uwifi);
    if (VSF_ERR_NONE != err) {
        __usb_wifi_bus_finish(uwifi, err);
        return err;
    }
    return VSF_ERR_NONE;
}

/* reg_bus->reg_block_write */
static vsf_err_t __usb_wifi_reg_block_write(vsf_wifi_t *wifi, uint16_t base,
        const uint8_t *data, uint32_t len, vsf_wifi_done_t done)
{
    vk_usbh_wifi_t *uwifi = vsf_container_of(wifi, vk_usbh_wifi_t, wifi);
    VSF_USB_ASSERT(uwifi->bus_state == VK_USBH_WIFI_BUS_IDLE);

    uwifi->bus_state        = VK_USBH_WIFI_BUS_BLOCK_WRITE;
    uwifi->bus_done         = done;
    uwifi->bus_block_data   = data;
    uwifi->bus_block_len    = len;
    uwifi->bus_block_offset = 0;
    uwifi->bus_block_base   = base;
    uwifi->bus_block_chunk  = VSF_USBH_WIFI_CFG_FW_CHUNK_SIZE;

    vsf_err_t err = __vsf_eda_crit_npb_enter(&uwifi->dev->ep0.crit);
    if (VSF_ERR_NONE != err) {
        uwifi->bus_pending_first = true;
        return VSF_ERR_NONE;
    }
    err = __usb_wifi_submit_ep0(uwifi);
    if (VSF_ERR_NONE != err) {
        __usb_wifi_bus_finish(uwifi, err);
        return err;
    }
    return VSF_ERR_NONE;
}

/* reg_bus->vendor_request */
static vsf_err_t __usb_wifi_vendor_request(vsf_wifi_t *wifi, uint8_t request,
        uint16_t value, uint16_t index, vsf_wifi_done_t done)
{
    vk_usbh_wifi_t *uwifi = vsf_container_of(wifi, vk_usbh_wifi_t, wifi);
    VSF_USB_ASSERT(uwifi->bus_state == VK_USBH_WIFI_BUS_IDLE);

    uwifi->bus_state          = VK_USBH_WIFI_BUS_VENDOR;
    uwifi->bus_done           = done;
    uwifi->bus_vendor_request = request;
    uwifi->bus_vendor_value   = value;
    uwifi->bus_vendor_index   = index;

    vsf_err_t err = __vsf_eda_crit_npb_enter(&uwifi->dev->ep0.crit);
    if (VSF_ERR_NONE != err) {
        uwifi->bus_pending_first = true;
        return VSF_ERR_NONE;
    }
    err = __usb_wifi_submit_ep0(uwifi);
    if (VSF_ERR_NONE != err) {
        __usb_wifi_bus_finish(uwifi, err);
        return err;
    }
    return VSF_ERR_NONE;
}

/*============================ BUS OPS: on_ready =============================*/

static bool __vk_usbh_wifi_start_rx(vk_usbh_wifi_t *uwifi);

static void __usb_wifi_on_ready(vsf_wifi_t *wifi)
{
    vk_usbh_wifi_t *uwifi = vsf_container_of(wifi, vk_usbh_wifi_t, wifi);
    if (!__vk_usbh_wifi_start_rx(uwifi)) {
        vk_usbh_remove_interface(uwifi->usbh, uwifi->dev, uwifi->ifs);
    }
}

/*============================ BUS OPS: data_tx =============================*/

/* reg_bus->data_tx — ship a chip-encoded TX payload over the bulk OUT
 * endpoint.  Thin wrapper around the existing vk_usbh_wifi_send(); the
 * generic layer hands us a buffer already built by drv->build_tx. */
static vsf_err_t __usb_wifi_data_tx(vsf_wifi_t *wifi, uint8_t *data,
        uint16_t len)
{
    vk_usbh_wifi_t *uwifi = vsf_container_of(wifi, vk_usbh_wifi_t, wifi);
    return vk_usbh_wifi_send(uwifi, data, len);
}

/*============================ BUS OPS VTABLE ================================*/

static const vsf_wifi_reg_bus_t __usb_wifi_reg_bus = {
    .reg_write       = __usb_wifi_reg_write,
    .reg_read        = __usb_wifi_reg_read,
    .reg_block_write = __usb_wifi_reg_block_write,
    .vendor_request  = __usb_wifi_vendor_request,
    .on_ready        = __usb_wifi_on_ready,
    .data_tx         = __usb_wifi_data_tx,
};

/*============================ ATTACH FAIL HOOK ==============================*/

static void __usb_wifi_attach_fail(vsf_wifi_t *wifi, vsf_err_t err)
{
    vk_usbh_wifi_t *uwifi = vsf_container_of(wifi, vk_usbh_wifi_t, wifi);
    (void)err;
    vk_usbh_remove_interface(uwifi->usbh, uwifi->dev, uwifi->ifs);
}

/*============================ EVENT HANDLER =================================*/

static void __vk_usbh_wifi_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_wifi_t *uwifi = __this_wifi(eda);
    vk_usbh_dev_t  *dev   = uwifi->dev;
    vsf_err_t       err;

    if (uwifi->wifi.disconnecting) return;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_wifi_start(&uwifi->wifi);
        return;

    case VSF_EVT_SYNC:
        /* ep0 critical section acquired — submit pending bus op. */
        if (uwifi->bus_pending_first && uwifi->bus_state != VK_USBH_WIFI_BUS_IDLE) {
            uwifi->bus_pending_first = false;
            err = __usb_wifi_submit_ep0(uwifi);
            if (VSF_ERR_NONE != err) {
                __usb_wifi_bus_finish(uwifi, err);
            }
        }
        return;

    case VSF_WIFI_EVT_SCAN_HOP:
        vsf_wifi_on_scan_hop_evt(&uwifi->wifi);
        return;

    case VSF_WIFI_EVT_MLME_RETRY:
        vsf_wifi_on_mlme_retry_evt(&uwifi->wifi);
        return;

    case VSF_EVT_MESSAGE: {
        vk_usbh_hcd_urb_t *urb_hcd =
                (vk_usbh_hcd_urb_t *)vsf_eda_get_cur_msg();

        /* ep0 completion — advance bus op state. */
        if (urb_hcd == dev->ep0.urb.urb_hcd) {
            bool ok = (URB_OK == vk_usbh_urb_get_status(&dev->ep0.urb));

            switch (uwifi->bus_state) {
            case VK_USBH_WIFI_BUS_REG_WRITE:
                vk_usbh_urb_free_buffer(&dev->ep0.urb);
                __usb_wifi_bus_finish(uwifi, ok ? VSF_ERR_NONE : VSF_ERR_FAIL);
                break;
            case VK_USBH_WIFI_BUS_VENDOR:
                /* No data stage -> no buffer was allocated. */
                __usb_wifi_bus_finish(uwifi, ok ? VSF_ERR_NONE : VSF_ERR_FAIL);
                break;
            case VK_USBH_WIFI_BUS_REG_READ:
                if (ok && uwifi->bus_read_out != NULL) {
                    uint32_t *buf = (uint32_t *)vk_usbh_urb_peek_buffer(
                            &dev->ep0.urb);
                    *uwifi->bus_read_out = *buf;
                }
                vk_usbh_urb_free_buffer(&dev->ep0.urb);
                __usb_wifi_bus_finish(uwifi, ok ? VSF_ERR_NONE : VSF_ERR_FAIL);
                break;
            case VK_USBH_WIFI_BUS_BLOCK_WRITE:
                /* Each chunk used an HCD-allocated bounce buffer; release it
                 * before allocating the next one (or finishing). */
                vk_usbh_urb_free_buffer(&dev->ep0.urb);
                if (!ok) {
                    __usb_wifi_bus_finish(uwifi, VSF_ERR_FAIL);
                    break;
                }
                uwifi->bus_block_offset += uwifi->bus_block_chunk;
                if (uwifi->bus_block_offset >= uwifi->bus_block_len) {
                    __usb_wifi_bus_finish(uwifi, VSF_ERR_NONE);
                } else {
                    err = __usb_wifi_submit_ep0(uwifi);
                    if (VSF_ERR_NONE != err) {
                        __usb_wifi_bus_finish(uwifi, err);
                    }
                }
                break;
            default:
                vk_usbh_urb_free_buffer(&dev->ep0.urb);
                break;
            }
            return;
        }

        /* Bulk RX/TX completion — locate iocb by urb_hcd identity. */
        vk_usbh_wifi_iocb_t *iocb = NULL;
        for (int i = 0; i < dimof(uwifi->iocb); i++) {
            if (uwifi->iocb[i].urb.urb_hcd == urb_hcd) {
                iocb = &uwifi->iocb[i];
                break;
            }
        }
        VSF_USB_ASSERT(iocb != NULL);

        if (iocb->is_rx) {
            int  st  = vk_usbh_urb_get_status(&iocb->urb);
            uint32_t len = (URB_OK == st)
                    ? vk_usbh_urb_get_actual_length(&iocb->urb) : 0;
            static unsigned __dbg_rx_cmpl = 0;
            if (__dbg_rx_cmpl < 12) {
                __dbg_rx_cmpl++;
                vsf_wifi_bus_rt2x00_trace_debug(
                    "wifi_usb: rx urb#%u status=%d len=%u"
                    VSF_TRACE_CFG_LINEEND,
                    __dbg_rx_cmpl, st, (unsigned)len);
            }
            if (URB_OK == st && len > 0) {
                uint8_t *frame = vk_usbh_urb_peek_buffer(&iocb->urb);
                vsf_wifi_on_rx_internal(&uwifi->wifi, frame, (uint16_t)len);
            }
            err = vk_usbh_submit_urb(uwifi->usbh, &iocb->urb);
            if (VSF_ERR_NONE != err) {
                vk_usbh_remove_interface(uwifi->usbh, dev, uwifi->ifs);
            }
        } else {
            iocb->is_busy = false;
        }
        }
        return;
    }
}

/*============================ RX/TX URB SETUP ===============================*/

static bool __vk_usbh_wifi_start_rx(vk_usbh_wifi_t *uwifi)
{
    int rx_submitted = 0;
    for (int i = 0; i < VSF_USBH_WIFI_CFG_RX_NUM; i++) {
        vk_usbh_wifi_iocb_t *icb = &uwifi->rx_icb[i];
        if (!icb->is_supported) continue;
        if (VSF_ERR_NONE != vk_usbh_alloc_urb(uwifi->usbh, uwifi->dev, &icb->urb))
            return false;
        if (NULL == vk_usbh_urb_alloc_buffer(&icb->urb,
                VSF_USBH_WIFI_CFG_URB_BUFSIZE))
            return false;
        if (VSF_ERR_NONE != vk_usbh_submit_urb(uwifi->usbh, &icb->urb))
            return false;
        rx_submitted++;
    }
    vsf_wifi_bus_rt2x00_trace_info("wifi_usb: start_rx submitted %d RX URBs (bufsize=%u)"
            VSF_TRACE_CFG_LINEEND, rx_submitted,
            (unsigned)VSF_USBH_WIFI_CFG_URB_BUFSIZE);
    (void)rx_submitted;
    for (int i = 0; i < VSF_USBH_WIFI_CFG_TX_NUM; i++) {
        vk_usbh_wifi_iocb_t *ocb = &uwifi->tx_ocb[i];
        if (!ocb->is_supported) continue;
        if (VSF_ERR_NONE != vk_usbh_alloc_urb(uwifi->usbh, uwifi->dev, &ocb->urb))
            return false;
#if VSF_USBH_WIFI_CFG_TX_USE_LOCAL_BUFFER == ENABLED
        if (NULL == vk_usbh_urb_alloc_buffer(&ocb->urb,
                VSF_USBH_WIFI_CFG_URB_BUFSIZE))
            return false;
#endif
    }
    return true;
}

/*============================ FREE / CLEANUP ================================*/

static void __vk_usbh_wifi_free_all(vk_usbh_wifi_t *uwifi)
{
    for (int i = 0; i < dimof(uwifi->iocb); i++) {
        vk_usbh_wifi_iocb_t *iocb = &uwifi->iocb[i];
        if (vk_usbh_urb_is_alloced(&iocb->urb))
            vk_usbh_free_urb(uwifi->usbh, &iocb->urb);
    }
}

static void __vk_usbh_wifi_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_wifi_t *uwifi = __this_wifi(eda);
    vsf_usbh_free(uwifi);
}

/*============================ PROBE / DISCONNECT ============================*/

static void * __vk_usbh_wifi_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    struct usb_interface_desc_t *desc_ifs =
            parser_ifs->parser_alt[ifs->cur_alt].desc_ifs;
    struct usb_endpoint_desc_t *desc_ep =
            parser_ifs->parser_alt[ifs->cur_alt].desc_ep;
    vk_usbh_wifi_t *uwifi;
    uint_fast8_t epaddr;
    bool is_in, has_rx = false, has_tx = false;

    if (desc_ifs->bInterfaceNumber != 0) return NULL;

    const vsf_wifi_chip_drv_t *drv = __vk_usbh_wifi_find_drv(
            usbh->parser->desc_device->idVendor,
            usbh->parser->desc_device->idProduct);
    if (NULL == drv) return NULL;
    vsf_wifi_bus_rt2x00_trace_info("wifi_usb: probe matched vid=0x%04X pid=0x%04X chip=%s" VSF_TRACE_CFG_LINEEND,
            usbh->parser->desc_device->idVendor,
            usbh->parser->desc_device->idProduct,
            (drv->name != NULL) ? drv->name : "?");

    uwifi = vsf_usbh_malloc(sizeof(vk_usbh_wifi_t));
    if (NULL == uwifi) return NULL;
    memset(uwifi, 0, sizeof(vk_usbh_wifi_t));

    for (int i = 0; i < desc_ifs->bNumEndpoints; i++) {
        if (desc_ep->bLength != USB_DT_ENDPOINT_SIZE) goto free_all;
        epaddr = desc_ep->bEndpointAddress;
        is_in = (epaddr & USB_DIR_MASK) == USB_DIR_IN;

        switch (desc_ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
        case USB_ENDPOINT_XFER_BULK:
            if (is_in) {
                for (int j = 0; j < VSF_USBH_WIFI_CFG_RX_NUM; j++) {
                    vk_usbh_urb_prepare(&uwifi->rx_icb[j].urb, dev, desc_ep);
                    uwifi->rx_icb[j].is_rx        = true;
                    uwifi->rx_icb[j].is_supported = true;
                }
                has_rx = true;
            } else {
                for (int j = 0; j < VSF_USBH_WIFI_CFG_TX_NUM; j++) {
                    vk_usbh_urb_prepare(&uwifi->tx_ocb[j].urb, dev, desc_ep);
                    uwifi->tx_ocb[j].is_rx        = false;
                    uwifi->tx_ocb[j].is_supported = true;
                }
                has_tx = true;
            }
            break;
        case USB_ENDPOINT_XFER_INT:
            if (is_in) {
                for (int j = 0; j < VSF_USBH_WIFI_CFG_RX_NUM; j++) {
                    vk_usbh_urb_prepare(&uwifi->rx_icb[j].urb, dev, desc_ep);
                    uwifi->rx_icb[j].is_rx        = true;
                    uwifi->rx_icb[j].is_supported = true;
                }
                has_rx = true;
            }
            break;
        }
        desc_ep = (struct usb_endpoint_desc_t *)((uintptr_t)desc_ep
                + USB_DT_ENDPOINT_SIZE);
    }
    if (!has_rx || !has_tx) goto free_all;

    uwifi->dev  = dev;
    uwifi->usbh = usbh;
    uwifi->ifs  = ifs;
    uwifi->id.idProduct = usbh->parser->desc_device->idProduct;
    uwifi->id.idVendor  = usbh->parser->desc_device->idVendor;

    /* Initialize the generic wifi layer (embedded). */
    vsf_wifi_init(&uwifi->wifi, drv, &__usb_wifi_reg_bus, &uwifi->eda);
    vsf_wifi_set_attach_fail(&uwifi->wifi, __usb_wifi_attach_fail);

    /* Start EDA — VSF_EVT_INIT will call vsf_wifi_start(). */
    uwifi->eda.fn.evthandler = __vk_usbh_wifi_evthandler;
    uwifi->eda.on_terminate  = __vk_usbh_wifi_on_eda_terminate;
    vsf_eda_init(&uwifi->eda);
#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info(&uwifi->eda, "usbh_wifi", NULL, 0);
#endif
    vsf_wifi_on_new(&uwifi->wifi);
    return uwifi;

free_all:
    __vk_usbh_wifi_free_all(uwifi);
    vsf_usbh_free(uwifi);
    return NULL;
}

static void __vk_usbh_wifi_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        void *param)
{
    vk_usbh_wifi_t *uwifi = (vk_usbh_wifi_t *)param;
    (void)usbh; (void)dev;

    /* Tear down generic wifi state (gates callbacks, cancels timers). */
    vsf_wifi_fini(&uwifi->wifi);

    /* Release every URB. */
    __vk_usbh_wifi_free_all(uwifi);

    /* Tear down the EDA — on_terminate frees the wifi struct. */
    vsf_eda_fini(&uwifi->eda);
}

/*============================ USB-SPECIFIC PUBLIC API ========================*/

vsf_wifi_t * vk_usbh_wifi_get_wifi(void *dev)
{
    vk_usbh_wifi_t *uwifi = (vk_usbh_wifi_t *)dev;
    return &uwifi->wifi;
}

bool vk_usbh_wifi_can_send(void *dev)
{
    vk_usbh_wifi_t *uwifi = (vk_usbh_wifi_t *)dev;
    if (!uwifi->wifi.is_ready) return false;
    for (int i = 0; i < VSF_USBH_WIFI_CFG_TX_NUM; i++) {
        vk_usbh_wifi_iocb_t *ocb = &uwifi->tx_ocb[i];
        if (ocb->is_supported && !ocb->is_busy) return true;
    }
    return false;
}

vsf_err_t vk_usbh_wifi_send(void *dev, uint8_t *frame, uint16_t len)
{
    vk_usbh_wifi_t *uwifi = (vk_usbh_wifi_t *)dev;
    if (!uwifi->wifi.is_ready) return VSF_ERR_NOT_READY;
    if (len > VSF_USBH_WIFI_CFG_URB_BUFSIZE) return VSF_ERR_NOT_SUPPORT;

    vk_usbh_wifi_iocb_t *ocb = NULL;
    for (int i = 0; i < VSF_USBH_WIFI_CFG_TX_NUM; i++) {
        if (uwifi->tx_ocb[i].is_supported && !uwifi->tx_ocb[i].is_busy) {
            ocb = &uwifi->tx_ocb[i];
            break;
        }
    }
    if (NULL == ocb) return VSF_ERR_NOT_AVAILABLE;

#if VSF_USBH_WIFI_CFG_TX_USE_LOCAL_BUFFER == ENABLED
    memcpy(ocb->urb.urb_hcd->buffer, frame, len);
    ocb->urb.urb_hcd->transfer_length = len;
#else
    vk_usbh_urb_set_buffer(&ocb->urb, frame, len);
#endif
    ocb->is_busy = true;
    vsf_err_t err = vk_usbh_submit_urb_ex(uwifi->usbh, &ocb->urb,
            URB_ZERO_PACKET, &uwifi->eda);
    if (VSF_ERR_NONE != err) {
        ocb->is_busy = false;
    }
    return err;
}

#endif      // VSF_USE_USB_HOST && VSF_USBH_USE_RT2X00 && VSF_USE_WIFI
