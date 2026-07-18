/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *                                                                           *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software       *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  either express or implied. See the License for the specific language     *
 *  governing permissions and limitations under the License.                 *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_AIC8800D == ENABLED && VSF_USE_WIFI == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__

#include "kernel/vsf_kernel.h"
#include "component/wifi/vsf_wifi_priv.h"
#include "component/wifi/chip/aic8800d/vsf_wifi_aic8800d.h"
#include "../../vsf_usbh.h"
#include "./vsf_usbh_wifi_aic8800d.h"

#if VSF_WIFI_USE_AIC8800D != ENABLED
#   error "USB AIC8800D class needs VSF_WIFI_USE_AIC8800D enabled."
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_USBH_WIFI_AIC8800D_CFG_RX_NUM
#   define VSF_USBH_WIFI_AIC8800D_CFG_RX_NUM      8
#endif
#ifndef VSF_USBH_WIFI_AIC8800D_CFG_TX_NUM
#   define VSF_USBH_WIFI_AIC8800D_CFG_TX_NUM      8
#endif
#ifndef VSF_USBH_WIFI_AIC8800D_CFG_RX_BUFSIZE
#   define VSF_USBH_WIFI_AIC8800D_CFG_RX_BUFSIZE  2048
#endif
#ifndef VSF_USBH_WIFI_AIC8800D_CFG_TX_BUFSIZE
#   define VSF_USBH_WIFI_AIC8800D_CFG_TX_BUFSIZE  2048
#endif

#define AIC8800D_USB_VID    0xA69C
#define AIC8800D_V2_VID     0x368B
#define AIC8800D_TP_VID     0x2357
#define AIC8800D_TENDA_VID  0x2604

/* Boot-loader PIDs: firmware must be uploaded. */
#define AIC8800D80_PID      0x8D80
#define AIC8800D81_PID      0x8D81

/* Runtime PIDs after firmware upload (Linux aic8800_fdrv id table). */
#define AIC8800D_RUNTIME_AIC8800_PID        0x8800
#define AIC8800D_RUNTIME_AIC8801_PID        0x8801
#define AIC8800D_RUNTIME_AIC8800DC_PID      0x88DC
#define AIC8800D_RUNTIME_AIC8800DW_PID      0x88DD
#define AIC8800D_RUNTIME_AIC8800D81_PID     0x8D81
#define AIC8800D_RUNTIME_TP_PID             0x014E
#define AIC8800D_RUNTIME_MERCURY_PID        0x014B
#define AIC8800D_RUNTIME_FAST_PID           0x014F
#define AIC8800D_RUNTIME_TENDA_PID          0x001F

/*============================ TYPES =========================================*/

typedef struct vk_usbh_wifi_aic8800d_iocb_t {
    vk_usbh_urb_t       urb;
    struct vk_usbh_wifi_aic8800d_t *aic;
    vsf_callback_timer_t rx_retry_timer;
    uint8_t             is_rx        : 1;
    uint8_t             is_supported : 1;
    uint8_t             is_busy      : 1;
} vk_usbh_wifi_aic8800d_iocb_t;

typedef struct vk_usbh_wifi_aic8800d_t {
    vk_usbh_t           *usbh;
    vk_usbh_dev_t       *dev;
    vk_usbh_ifs_t       *ifs;
    vsf_eda_t            eda;
    vsf_wifi_t           wifi;

    /* Per-instance bus ops so the chip driver can tell boot-ROM from runtime. */
    vsf_wifi_aic8800d_bus_ops_t bus_ops;

    uint8_t              ep_in;
    uint8_t              ep_out;
    uint16_t             ep_in_mps;
    uint16_t             ep_out_mps;

    /* Bulk RX/TX iocb pool */
    union {
        struct {
            vk_usbh_wifi_aic8800d_iocb_t rx_icb[VSF_USBH_WIFI_AIC8800D_CFG_RX_NUM];
            vk_usbh_wifi_aic8800d_iocb_t tx_ocb[VSF_USBH_WIFI_AIC8800D_CFG_TX_NUM];
        };
        vk_usbh_wifi_aic8800d_iocb_t iocb[VSF_USBH_WIFI_AIC8800D_CFG_RX_NUM
                                       + VSF_USBH_WIFI_AIC8800D_CFG_TX_NUM];
    };
} vk_usbh_wifi_aic8800d_t;

/*============================ PROTOTYPES ====================================*/

static void * __vk_usbh_wifi_aic8800d_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_wifi_aic8800d_disconnect(vk_usbh_t *usbh,
        vk_usbh_dev_t *dev, void *param);
static void __vk_usbh_wifi_aic8800d_evthandler(vsf_eda_t *eda, vsf_evt_t evt);
static void __vk_usbh_wifi_aic8800d_rx_retry_cb(vsf_callback_timer_t *timer);
static void __vk_usbh_wifi_aic8800d_on_eda_terminate(vsf_eda_t *eda);
static void __vk_usbh_wifi_aic8800d_attach_fail(vsf_wifi_t *wifi, vsf_err_t err);
static void __vk_usbh_wifi_aic8800d_on_ready(vsf_wifi_t *wifi);
static vsf_err_t __vk_usbh_wifi_aic8800d_send(vsf_wifi_t *wifi,
        const uint8_t *data, uint16_t len, vsf_wifi_done_t done);
static bool __vk_usbh_wifi_aic8800d_can_send(vsf_wifi_t *wifi);

/*============================ DEVICE MAP ====================================*/

static const vk_usbh_dev_id_t __vk_usbh_wifi_aic8800d_dev_id[] = {
    /* Boot-loader mode: upload firmware first. */
    { VSF_USBH_MATCH_VID_PID(AIC8800D_USB_VID, AIC8800D80_PID) },
    { VSF_USBH_MATCH_VID_PID(AIC8800D_USB_VID, AIC8800D81_PID) },
    /* Runtime mode: firmware already running, start LMAC init directly. */
    { VSF_USBH_MATCH_VID_PID(AIC8800D_USB_VID, AIC8800D_RUNTIME_AIC8800_PID) },
    { VSF_USBH_MATCH_VID_PID(AIC8800D_USB_VID, AIC8800D_RUNTIME_AIC8801_PID) },
    { VSF_USBH_MATCH_VID_PID(AIC8800D_USB_VID, AIC8800D_RUNTIME_AIC8800DC_PID) },
    { VSF_USBH_MATCH_VID_PID(AIC8800D_USB_VID, AIC8800D_RUNTIME_AIC8800DW_PID) },
    { VSF_USBH_MATCH_VID_PID(AIC8800D_USB_VID, AIC8800D_RUNTIME_AIC8800D81_PID) },
    { VSF_USBH_MATCH_VID_PID(AIC8800D_TP_VID,  AIC8800D_RUNTIME_TP_PID) },
    { VSF_USBH_MATCH_VID_PID(AIC8800D_TP_VID,  AIC8800D_RUNTIME_MERCURY_PID) },
    { VSF_USBH_MATCH_VID_PID(AIC8800D_TP_VID,  AIC8800D_RUNTIME_FAST_PID) },
    { VSF_USBH_MATCH_VID_PID(AIC8800D_TENDA_VID, AIC8800D_RUNTIME_TENDA_PID) },
};

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_wifi_aic8800d_drv = {
    .name       = "aic8800d_wifi_usb",
    .dev_id_num = dimof(__vk_usbh_wifi_aic8800d_dev_id),
    .dev_ids    = __vk_usbh_wifi_aic8800d_dev_id,
    .probe      = __vk_usbh_wifi_aic8800d_probe,
    .disconnect = __vk_usbh_wifi_aic8800d_disconnect,
};

/*============================ LOCAL FUNCTIONS ===============================*/

static vk_usbh_wifi_aic8800d_t * __this_aic(vsf_eda_t *eda)
{
    return vsf_container_of(eda, vk_usbh_wifi_aic8800d_t, eda);
}

static void __vk_usbh_wifi_aic8800d_free_all_urb(vk_usbh_wifi_aic8800d_t *aic)
{
    for (int i = 0; i < dimof(aic->iocb); i++) {
        vk_usbh_wifi_aic8800d_iocb_t *iocb = &aic->iocb[i];
        if (vk_usbh_urb_is_alloced(&iocb->urb)) {
            vk_usbh_free_urb(aic->usbh, &iocb->urb);
        }
    }
}

static bool __vk_usbh_wifi_aic8800d_start_rx(vk_usbh_wifi_aic8800d_t *aic)
{
    int rx_submitted = 0;
    for (int i = 0; i < VSF_USBH_WIFI_AIC8800D_CFG_RX_NUM; i++) {
        vk_usbh_wifi_aic8800d_iocb_t *icb = &aic->rx_icb[i];
        if (!icb->is_supported) continue;
        if (VSF_ERR_NONE != vk_usbh_submit_urb(aic->usbh, &icb->urb))
            return false;
        rx_submitted++;
    }
    vsf_wifi_aic8800d_trace_info("aic8800d_usb: start_rx submitted %d RX URBs"
            VSF_TRACE_CFG_LINEEND, rx_submitted);
    (void)rx_submitted;
    return true;
}

static void __vk_usbh_wifi_aic8800d_on_ready(vsf_wifi_t *wifi)
{
    /* RX URBs are already armed in VSF_EVT_INIT so that firmware-load CFMs
     * can be received. Nothing to do here for now. */
    (void)wifi;
}

static vsf_err_t __vk_usbh_wifi_aic8800d_send(vsf_wifi_t *wifi,
        const uint8_t *data, uint16_t len, vsf_wifi_done_t done)
{
    vk_usbh_wifi_aic8800d_t *aic = vsf_container_of(wifi, vk_usbh_wifi_aic8800d_t, wifi);
    (void)done;

    if (len > VSF_USBH_WIFI_AIC8800D_CFG_TX_BUFSIZE) return VSF_ERR_NOT_SUPPORT;

    vk_usbh_wifi_aic8800d_iocb_t *ocb = NULL;
    for (int i = 0; i < VSF_USBH_WIFI_AIC8800D_CFG_TX_NUM; i++) {
        if (aic->tx_ocb[i].is_supported && !aic->tx_ocb[i].is_busy) {
            ocb = &aic->tx_ocb[i];
            break;
        }
    }
    if (NULL == ocb) return VSF_ERR_NOT_AVAILABLE;

    uint8_t *buf = vk_usbh_urb_peek_buffer(&ocb->urb);
    if (NULL == buf) {
        buf = vk_usbh_urb_alloc_buffer(&ocb->urb, VSF_USBH_WIFI_AIC8800D_CFG_TX_BUFSIZE);
        if (NULL == buf) return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    memcpy(buf, data, len);
    ocb->urb.urb_hcd->transfer_length = len;

    ocb->is_busy = true;
    vsf_err_t err = vk_usbh_submit_urb_ex(aic->usbh, &ocb->urb, URB_ZERO_PACKET, &aic->eda);
    if (VSF_ERR_NONE != err) {
        ocb->is_busy = false;
    }
    return err;
}

static bool __vk_usbh_wifi_aic8800d_can_send(vsf_wifi_t *wifi)
{
    vk_usbh_wifi_aic8800d_t *aic = vsf_container_of(wifi, vk_usbh_wifi_aic8800d_t, wifi);
    if (!wifi->is_ready) return false;
    for (int i = 0; i < VSF_USBH_WIFI_AIC8800D_CFG_TX_NUM; i++) {
        vk_usbh_wifi_aic8800d_iocb_t *ocb = &aic->tx_ocb[i];
        if (ocb->is_supported && !ocb->is_busy) return true;
    }
    return false;
}

/* Chip-level bus operations exposed to vsf_wifi_aic8800d.c */
static const vsf_wifi_aic8800d_bus_ops_t __vk_usbh_wifi_aic8800d_bus_ops_template = {
    .base = {
        .on_ready   = __vk_usbh_wifi_aic8800d_on_ready,
    },
    .send       = __vk_usbh_wifi_aic8800d_send,
    .can_send   = __vk_usbh_wifi_aic8800d_can_send,
};

static void __vk_usbh_wifi_aic8800d_rx_retry_cb(vsf_callback_timer_t *timer)
{
    vk_usbh_wifi_aic8800d_iocb_t *iocb = vsf_container_of(timer,
            vk_usbh_wifi_aic8800d_iocb_t, rx_retry_timer);
    vk_usbh_wifi_aic8800d_t *aic = iocb->aic;
    if (aic != NULL && !aic->wifi.disconnecting) {
        vsf_err_t err = vk_usbh_submit_urb(aic->usbh, &iocb->urb);
        if (VSF_ERR_NONE != err) {
            vk_usbh_remove_interface(aic->usbh, aic->dev, aic->ifs);
        }
    }
}

static void __vk_usbh_wifi_aic8800d_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_wifi_aic8800d_t *aic = __this_aic(eda);

    vsf_trace_info("aic8800d_usb: evthandler evt=%d disconnecting=%d INIT=%d"
            VSF_TRACE_CFG_LINEEND, (int)evt, (int)aic->wifi.disconnecting, (int)VSF_EVT_INIT);

    if (aic->wifi.disconnecting) return;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_trace_info("aic8800d_usb: VSF_EVT_INIT matched, calling wifi_start"
                VSF_TRACE_CFG_LINEEND);
        if (!__vk_usbh_wifi_aic8800d_start_rx(aic)) {
            vk_usbh_remove_interface(aic->usbh, aic->dev, aic->ifs);
            return;
        }
        vsf_wifi_start(&aic->wifi);
        vsf_trace_info("aic8800d_usb: wifi_start returned" VSF_TRACE_CFG_LINEEND);
        return;

    case VSF_WIFI_EVT_SCAN_HOP:
        vsf_wifi_on_scan_hop_evt(&aic->wifi);
        return;

    case VSF_WIFI_EVT_MLME_RETRY:
        vsf_wifi_on_mlme_retry_evt(&aic->wifi);
        return;

    case VSF_EVT_MESSAGE: {
        vk_usbh_hcd_urb_t *urb_hcd = (vk_usbh_hcd_urb_t *)vsf_eda_get_cur_msg();
        vk_usbh_wifi_aic8800d_iocb_t *iocb = NULL;
        for (int i = 0; i < dimof(aic->iocb); i++) {
            if (aic->iocb[i].urb.urb_hcd == urb_hcd) {
                iocb = &aic->iocb[i];
                break;
            }
        }
        if (NULL == iocb) return;

        if (iocb->is_rx) {
            int st = vk_usbh_urb_get_status(&iocb->urb);
            uint32_t len = (URB_OK == st)
                    ? vk_usbh_urb_get_actual_length(&iocb->urb) : 0;
            vsf_trace_info("aic8800d_usb: rx done st=%d len=%u" VSF_TRACE_CFG_LINEEND,
                    st, (unsigned)len);
            if (URB_OK == st && len > 0) {
                uint8_t *frame = vk_usbh_urb_peek_buffer(&iocb->urb);
                vsf_wifi_aic8800d_on_rx(&aic->wifi, frame, (uint16_t)len);
            }
            vsf_err_t err = vk_usbh_submit_urb(aic->usbh, &iocb->urb);
            if (VSF_ERR_NONE != err) {
                vk_usbh_remove_interface(aic->usbh, aic->dev, aic->ifs);
            }
        } else {
            int st = vk_usbh_urb_get_status(&iocb->urb);
            vsf_trace_info("aic8800d_usb: tx done st=%d" VSF_TRACE_CFG_LINEEND, st);
            iocb->is_busy = false;
        }
        return;
    }

    default:
        break;
    }
}

static void __vk_usbh_wifi_aic8800d_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_wifi_aic8800d_t *aic = __this_aic(eda);
    __vk_usbh_wifi_aic8800d_free_all_urb(aic);
    vsf_usbh_free(aic);
}

static void __vk_usbh_wifi_aic8800d_attach_fail(vsf_wifi_t *wifi, vsf_err_t err)
{
    vk_usbh_wifi_aic8800d_t *aic = vsf_container_of(wifi, vk_usbh_wifi_aic8800d_t, wifi);
    (void)err;
    vk_usbh_remove_interface(aic->usbh, aic->dev, aic->ifs);
}

static void * __vk_usbh_wifi_aic8800d_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_wifi_aic8800d_t *aic = NULL;
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    vk_usbh_ifs_alt_parser_t *alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = alt->desc_ifs;
    struct usb_endpoint_desc_t  *desc_ep  = alt->desc_ep;
    uint_fast8_t ep_num = alt->num_of_ep;
    uint8_t epaddr;
    bool is_in, has_rx = false, has_tx = false;

    if (desc_ifs->bInterfaceNumber != 0) return NULL;

    aic = vsf_usbh_malloc(sizeof(vk_usbh_wifi_aic8800d_t));
    if (NULL == aic) return NULL;
    memset(aic, 0, sizeof(*aic));

    for (uint_fast8_t i = 0; i < ep_num; i++) {
        if (desc_ep->bLength != USB_DT_ENDPOINT_SIZE) goto free_all;
        epaddr = desc_ep->bEndpointAddress;
        is_in = (epaddr & USB_DIR_MASK) == USB_DIR_IN;

        switch (desc_ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
        case USB_ENDPOINT_XFER_BULK:
            if (is_in) {
                aic->ep_in     = epaddr;
                aic->ep_in_mps = desc_ep->wMaxPacketSize;
                has_rx = true;
                for (int j = 0; j < VSF_USBH_WIFI_AIC8800D_CFG_RX_NUM; j++) {
                    vk_usbh_urb_prepare(&aic->rx_icb[j].urb, dev, desc_ep);
                    aic->rx_icb[j].is_rx        = true;
                    aic->rx_icb[j].is_supported = true;
                    if (VSF_ERR_NONE != vk_usbh_alloc_urb(usbh, dev, &aic->rx_icb[j].urb))
                        goto free_all;
                    if (NULL == vk_usbh_urb_alloc_buffer(&aic->rx_icb[j].urb,
                            VSF_USBH_WIFI_AIC8800D_CFG_RX_BUFSIZE))
                        goto free_all;
                    aic->rx_icb[j].urb.urb_hcd->timeout = 5000;
                }
            } else {
                aic->ep_out     = epaddr;
                aic->ep_out_mps = desc_ep->wMaxPacketSize;
                has_tx = true;
                for (int j = 0; j < VSF_USBH_WIFI_AIC8800D_CFG_TX_NUM; j++) {
                    vk_usbh_urb_prepare(&aic->tx_ocb[j].urb, dev, desc_ep);
                    aic->tx_ocb[j].is_rx        = false;
                    aic->tx_ocb[j].is_supported = true;
                    if (VSF_ERR_NONE != vk_usbh_alloc_urb(usbh, dev, &aic->tx_ocb[j].urb))
                        goto free_all;
                    if (NULL == vk_usbh_urb_alloc_buffer(&aic->tx_ocb[j].urb,
                            VSF_USBH_WIFI_AIC8800D_CFG_TX_BUFSIZE))
                        goto free_all;
                    aic->tx_ocb[j].urb.urb_hcd->timeout = 5000;
                }
            }
            break;
        }
        desc_ep = (struct usb_endpoint_desc_t *)((uintptr_t)desc_ep
                + USB_DT_ENDPOINT_SIZE);
    }

    if (!has_rx || !has_tx) goto free_all;

    aic->usbh = usbh;
    aic->dev  = dev;
    aic->ifs  = ifs;

    for (int j = 0; j < VSF_USBH_WIFI_AIC8800D_CFG_RX_NUM; j++) {
        aic->rx_icb[j].aic = aic;
        aic->rx_icb[j].rx_retry_timer.on_timer = __vk_usbh_wifi_aic8800d_rx_retry_cb;
    }
    for (int j = 0; j < VSF_USBH_WIFI_AIC8800D_CFG_TX_NUM; j++) {
        aic->tx_ocb[j].aic = aic;
    }

    /* Initialise per-instance bus ops from the template and decide whether
     * this is a boot-loader device (needs firmware upload) or a runtime
     * device (firmware already running after boot-ROM load). */
    {
        uint16_t vid = usbh->parser->desc_device->idVendor;
        uint16_t pid = usbh->parser->desc_device->idProduct;
        bool is_runtime = false;

        if ((vid == AIC8800D_USB_VID) &&
            ((pid == AIC8800D_RUNTIME_AIC8800_PID) ||
             (pid == AIC8800D_RUNTIME_AIC8801_PID) ||
             (pid == AIC8800D_RUNTIME_AIC8800DC_PID) ||
             (pid == AIC8800D_RUNTIME_AIC8800DW_PID) ||
             (pid == AIC8800D_RUNTIME_AIC8800D81_PID))) {
            is_runtime = true;
        } else if ((vid == AIC8800D_TP_VID) &&
            ((pid == AIC8800D_RUNTIME_TP_PID) ||
             (pid == AIC8800D_RUNTIME_MERCURY_PID) ||
             (pid == AIC8800D_RUNTIME_FAST_PID))) {
            is_runtime = true;
        } else if ((vid == AIC8800D_TENDA_VID) &&
                   (pid == AIC8800D_RUNTIME_TENDA_PID)) {
            is_runtime = true;
        }

        aic->bus_ops = __vk_usbh_wifi_aic8800d_bus_ops_template;
        aic->bus_ops.skip_firmware_load = is_runtime;
        vsf_wifi_aic8800d_trace_info("aic8800d_usb: probe vid=0x%04X pid=0x%04X runtime=%d"
                VSF_TRACE_CFG_LINEEND, vid, pid, (int)is_runtime);
    }

    vsf_wifi_init(&aic->wifi, &vsf_wifi_aic8800d_drv,
            &aic->bus_ops.base, &aic->eda);
    aic->wifi.bus_ops = &aic->bus_ops.base;
    vsf_wifi_set_attach_fail(&aic->wifi, __vk_usbh_wifi_aic8800d_attach_fail);

    aic->eda.fn.evthandler = __vk_usbh_wifi_aic8800d_evthandler;
    aic->eda.on_terminate  = __vk_usbh_wifi_aic8800d_on_eda_terminate;
    vsf_err_t eda_err = vsf_eda_init(&aic->eda);
    vsf_trace_info("aic8800d_usb: eda_init err=%d prio=%d" VSF_TRACE_CFG_LINEEND, (int)eda_err, (int)aic->eda.priority);
    vsf_trace_info("aic8800d_usb: about to return aic=%p" VSF_TRACE_CFG_LINEEND, (void *)aic);
#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info(&aic->eda, "usbh_wifi_aic8800d", NULL, 0);
#endif
    vsf_wifi_on_new(&aic->wifi);
    return aic;

free_all:
    __vk_usbh_wifi_aic8800d_free_all_urb(aic);
    vsf_usbh_free(aic);
    return NULL;
}

static void __vk_usbh_wifi_aic8800d_disconnect(vk_usbh_t *usbh,
        vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_wifi_aic8800d_t *aic = (vk_usbh_wifi_aic8800d_t *)param;
    (void)usbh; (void)dev;
    if (NULL == aic) return;
    vsf_wifi_fini(&aic->wifi);
    vsf_eda_fini(&aic->eda);
}

#endif      /* VSF_USE_USB_HOST && VSF_USBH_USE_AIC8800D && VSF_USE_WIFI */
