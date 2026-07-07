/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software       *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_MT76 == ENABLED && VSF_USE_WIFI == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbh.h"
#include "./vsf_usbh_wifi_mt76_priv.h"

/*============================ MACROS ========================================*/

/* USB vendor request numbers used by MT76x2U control transfers. */
#define MT76_VEND_DEV_MODE          0x01
#define MT76_VEND_MULTI_WRITE       0x06
#define MT76_VEND_MULTI_READ        0x07
#define MT76_VEND_READ_EEPROM       0x09
#define MT76_VEND_WRITE_FCE         0x42
#define MT76_VEND_WRITE_CFG         0x46
#define MT76_VEND_READ_CFG          0x47

#define MT76_VEND_TYPE_CFG          ((uint32_t)1 << 30)
#define MT76_VEND_TYPE_EEPROM       ((uint32_t)1 << 31)
#define MT76_VEND_TYPE_MASK         (MT76_VEND_TYPE_CFG | MT76_VEND_TYPE_EEPROM)

/*============================ DEVICE MAP ====================================*/

static const vk_usbh_dev_id_t __vk_usbh_wifi_mt76_dev_id[] = {
    { VSF_USBH_MATCH_VID_PID(0x0e8d, 0x7612) }, /* MT7612U reference */
    { VSF_USBH_MATCH_VID_PID(0x0b05, 0x1833) }, /* Asus USB-AC54 */
    { VSF_USBH_MATCH_VID_PID(0x0b05, 0x17eb) }, /* Asus USB-AC55 */
    { VSF_USBH_MATCH_VID_PID(0x0b05, 0x180b) }, /* Asus USB-N53 B1 */
    { VSF_USBH_MATCH_VID_PID(0x057c, 0x8503) }, /* AVM FRITZ!WLAN AC860 */
    { VSF_USBH_MATCH_VID_PID(0x7392, 0xb711) }, /* Edimax EW 7722 UAC */
    { VSF_USBH_MATCH_VID_PID(0x0846, 0x9014) }, /* Netgear WNDA3100v3 */
    { VSF_USBH_MATCH_VID_PID(0x0846, 0x9053) }, /* Netgear A6210 */
    { VSF_USBH_MATCH_VID_PID(0x2357, 0x0137) }, /* TP-Link TL-WDN6200 */
};

/*============================ PROTOTYPES ====================================*/

static void *__vk_usbh_wifi_mt76_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
                                       vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_wifi_mt76_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
                                           void *param);
static void __vk_usbh_wifi_mt76_evthandler(vsf_eda_t *eda, vsf_evt_t evt);
static void __vk_usbh_wifi_mt76_attach_fail(vsf_wifi_t *wifi, vsf_err_t err);
static void __vk_usbh_wifi_mt76_on_eda_terminate(vsf_eda_t *eda);
static void __vk_usbh_wifi_mt76_on_ready(vsf_wifi_t *wifi);
static vsf_err_t __vk_usbh_wifi_mt76_reg_read(vsf_wifi_t *wifi,
    uint32_t reg, uint32_t *out, vsf_wifi_done_t done);
static vsf_err_t __vk_usbh_wifi_mt76_reg_write(vsf_wifi_t *wifi,
    uint32_t reg, uint32_t val, vsf_wifi_done_t done);
static vsf_err_t __vk_usbh_wifi_mt76_fce_write(vsf_wifi_t *wifi,
    uint32_t addr, uint32_t val, vsf_wifi_done_t done);
static vsf_err_t __vk_usbh_wifi_mt76_dev_cmd(vsf_wifi_t *wifi,
    uint8_t req, uint16_t value, uint16_t index, vsf_wifi_done_t done);
static vsf_err_t __vk_usbh_wifi_mt76_dev_class_cmd(vsf_wifi_t *wifi,
    uint8_t req, uint16_t value, uint16_t index,
    const uint8_t *data, uint16_t len, vsf_wifi_done_t done);
static vsf_err_t __vk_usbh_wifi_mt76_mcu_cmd(vsf_wifi_t *wifi,
    const uint8_t *data, uint16_t len, vsf_wifi_done_t done);
static vsf_err_t __vk_usbh_wifi_mt76_tx_frame(vsf_wifi_t *wifi,
    const uint8_t *data, uint16_t len, uint8_t queue_idx, vsf_wifi_done_t done);
static vsf_err_t __vk_usbh_wifi_mt76_rx_submit(vsf_wifi_t *wifi,
    uint8_t *buf, uint16_t len, uint8_t queue_idx);
static bool __vk_usbh_wifi_mt76_ep0_queue_init(
    vk_usbh_wifi_mt76_ep0_queue_t *q);
static void __vk_usbh_wifi_mt76_ep0_submit(vk_usbh_wifi_mt76_t *uwifi);
static void __vk_usbh_wifi_mt76_ep0_dispatch(vk_usbh_wifi_mt76_t *uwifi);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_wifi_mt76_drv = {
    .name       = "wifi_mt76",
    .dev_id_num = dimof(__vk_usbh_wifi_mt76_dev_id),
    .dev_ids    = __vk_usbh_wifi_mt76_dev_id,
    .probe      = __vk_usbh_wifi_mt76_probe,
    .disconnect = __vk_usbh_wifi_mt76_disconnect,
};

/*============================ HELPERS =======================================*/

static vk_usbh_wifi_mt76_t *__vk_usbh_wifi_mt76_from_wifi(vsf_wifi_t *wifi)
{
    mt76_wifi_priv_t *priv = (mt76_wifi_priv_t *)wifi->chip_priv;
    VSF_USB_ASSERT(priv != NULL);
    return (vk_usbh_wifi_mt76_t *)priv->bus_priv;
}

static vk_usbh_wifi_mt76_iocb_t *__vk_usbh_wifi_mt76_find_iocb(
        vk_usbh_wifi_mt76_t *uwifi, vk_usbh_hcd_urb_t *urb_hcd)
{
    for (int i = 0; i < dimof(uwifi->rx_pkt_iocb); i++) {
        if (uwifi->rx_pkt_iocb[i].urb.urb_hcd == urb_hcd)
            return &uwifi->rx_pkt_iocb[i];
    }
    if (uwifi->rx_cmd_iocb.urb.urb_hcd == urb_hcd)
        return &uwifi->rx_cmd_iocb;
    if (uwifi->mcu_cmd_iocb.urb.urb_hcd == urb_hcd)
        return &uwifi->mcu_cmd_iocb;
    for (int i = 0; i < dimof(uwifi->tx_iocb); i++) {
        if (uwifi->tx_iocb[i].urb.urb_hcd == urb_hcd)
            return &uwifi->tx_iocb[i];
    }
    return NULL;
}

/*============================ RX/TX URB SETUP ===============================*/

static bool __vk_usbh_wifi_mt76_start_rx(vk_usbh_wifi_mt76_t *uwifi)
{
    int submitted = 0;

    if (uwifi->in_ep[MT76_EP_IN_PKT_RX].desc != NULL) {
        for (int i = 0; i < dimof(uwifi->rx_pkt_iocb); i++) {
            vk_usbh_wifi_mt76_iocb_t *iocb = &uwifi->rx_pkt_iocb[i];
            iocb->urb.urb_hcd->timeout = 5000;
            iocb->is_rx        = true;
            iocb->is_supported = true;
            iocb->ep_idx       = MT76_EP_IN_PKT_RX;
            iocb->is_busy      = true;
            if (VSF_ERR_NONE != vk_usbh_submit_urb(uwifi->usbh, &iocb->urb))
                return false;
            submitted++;
        }
    }

    if (uwifi->in_ep[MT76_EP_IN_CMD_RESP].desc != NULL) {
        vk_usbh_wifi_mt76_iocb_t *iocb = &uwifi->rx_cmd_iocb;
        iocb->urb.urb_hcd->timeout = 5000;
        iocb->is_rx        = true;
        iocb->is_supported = true;
        iocb->ep_idx       = MT76_EP_IN_CMD_RESP;
        iocb->is_busy      = true;
        if (VSF_ERR_NONE != vk_usbh_submit_urb(uwifi->usbh, &iocb->urb))
            return false;
        submitted++;
    }

    (void)submitted;
    return true;
}

static void __vk_usbh_wifi_mt76_free_all_urbs(vk_usbh_wifi_mt76_t *uwifi)
{
    for (int i = 0; i < dimof(uwifi->rx_pkt_iocb); i++) {
        vk_usbh_wifi_mt76_iocb_t *iocb = &uwifi->rx_pkt_iocb[i];
        if (vk_usbh_urb_is_alloced(&iocb->urb))
            vk_usbh_free_urb(uwifi->usbh, &iocb->urb);
    }
    if (vk_usbh_urb_is_alloced(&uwifi->rx_cmd_iocb.urb))
        vk_usbh_free_urb(uwifi->usbh, &uwifi->rx_cmd_iocb.urb);
    if (vk_usbh_urb_is_alloced(&uwifi->mcu_cmd_iocb.urb))
        vk_usbh_free_urb(uwifi->usbh, &uwifi->mcu_cmd_iocb.urb);
    for (int i = 0; i < dimof(uwifi->tx_iocb); i++) {
        vk_usbh_wifi_mt76_iocb_t *iocb = &uwifi->tx_iocb[i];
        if (vk_usbh_urb_is_alloced(&iocb->urb))
            vk_usbh_free_urb(uwifi->usbh, &iocb->urb);
    }
}

/*============================ PROBE / DISCONNECT ============================*/

static void *__vk_usbh_wifi_mt76_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
                                       vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    struct usb_interface_desc_t *desc_ifs =
            parser_ifs->parser_alt[ifs->cur_alt].desc_ifs;
    struct usb_endpoint_desc_t *desc_ep =
            parser_ifs->parser_alt[ifs->cur_alt].desc_ep;
    vk_usbh_wifi_mt76_t *uwifi;
    int in_cnt = 0, out_cnt = 0;

    if (desc_ifs->bInterfaceNumber != 0) return NULL;

    vsf_wifi_chip_mt76_trace_debug(
        "mt76usb: probe if=%d" VSF_TRACE_CFG_LINEEND,
        desc_ifs->bInterfaceNumber);

    uwifi = vsf_usbh_malloc(sizeof(vk_usbh_wifi_mt76_t));
    if (NULL == uwifi) return NULL;
    memset(uwifi, 0, sizeof(vk_usbh_wifi_mt76_t));

    for (int i = 0; i < desc_ifs->bNumEndpoints; i++) {
        uint_fast8_t epaddr = desc_ep->bEndpointAddress;
        bool is_in = (epaddr & USB_DIR_MASK) == USB_DIR_IN;
        uint_fast8_t type = desc_ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;

        if (type == USB_ENDPOINT_XFER_BULK) {
            if (is_in && in_cnt < MT76_EP_IN_MAX) {
                uwifi->in_ep[in_cnt].addr = epaddr;
                uwifi->in_ep[in_cnt].desc = desc_ep;
                in_cnt++;
            } else if (!is_in && out_cnt < MT76_EP_OUT_MAX) {
                uwifi->out_ep[out_cnt].addr = epaddr;
                uwifi->out_ep[out_cnt].desc = desc_ep;
                out_cnt++;
            }
        }
        /* Linux mt76 ignores interrupt endpoints; only bulk IN/OUT matter. */
        desc_ep = (struct usb_endpoint_desc_t *)((uintptr_t)desc_ep
                + USB_DT_ENDPOINT_SIZE);
    }

    if (uwifi->in_ep[MT76_EP_IN_PKT_RX].desc == NULL ||
        uwifi->out_ep[MT76_EP_OUT_INBAND_CMD].desc == NULL) {
        goto free_all;
    }

    uwifi->usbh = usbh;
    uwifi->dev  = dev;
    uwifi->ifs  = ifs;

    /* Pre-allocate TX URBs before firmware loading starts.
     * The chip driver calls tx_frame during firmware upload, at which point
     * the IOCB must already hold a valid URB and buffer. */
    for (int i = 0; i < dimof(uwifi->tx_iocb); i++) {
        vk_usbh_wifi_mt76_iocb_t *iocb = &uwifi->tx_iocb[i];
        vk_usbh_urb_prepare(&iocb->urb, dev, uwifi->out_ep[MT76_EP_OUT_INBAND_CMD].desc);
        if (VSF_ERR_NONE != vk_usbh_alloc_urb(usbh, dev, &iocb->urb))
            goto free_all;
        if (NULL == vk_usbh_urb_alloc_buffer(&iocb->urb, VSF_USBH_WIFI_MT76_CFG_TX_BUFSIZE))
            goto free_all;
        iocb->urb.urb_hcd->timeout = 5000;
        iocb->is_rx        = false;
        iocb->is_supported = true;
    }

    /* Pre-allocate MCU command URB/buffer as well; mcu_cmd uses a dedicated
     * IOCB and expects the buffer to be available immediately. */
    {
        vk_usbh_wifi_mt76_iocb_t *iocb = &uwifi->mcu_cmd_iocb;
        vk_usbh_urb_prepare(&iocb->urb, dev, uwifi->out_ep[MT76_EP_OUT_INBAND_CMD].desc);
        if (VSF_ERR_NONE != vk_usbh_alloc_urb(usbh, dev, &iocb->urb))
            goto free_all;
        if (NULL == vk_usbh_urb_alloc_buffer(&iocb->urb, VSF_USBH_WIFI_MT76_CFG_TX_BUFSIZE))
            goto free_all;
        iocb->urb.urb_hcd->timeout = 5000;
        iocb->is_rx        = false;
        iocb->is_supported = true;
    }

    /* Pre-allocate RX URBs/buffers so that response and packet URBs can be
     * armed on demand without runtime allocation overhead. */
    for (int i = 0; i < dimof(uwifi->rx_pkt_iocb); i++) {
        vk_usbh_wifi_mt76_iocb_t *iocb = &uwifi->rx_pkt_iocb[i];
        vk_usbh_urb_prepare(&iocb->urb, dev, uwifi->in_ep[MT76_EP_IN_PKT_RX].desc);
        if (VSF_ERR_NONE != vk_usbh_alloc_urb(usbh, dev, &iocb->urb))
            goto free_all;
        if (NULL == vk_usbh_urb_alloc_buffer(&iocb->urb,
                VSF_USBH_WIFI_MT76_CFG_RX_BUFSIZE))
            goto free_all;
    }
    if (uwifi->in_ep[MT76_EP_IN_CMD_RESP].desc != NULL) {
        vk_usbh_wifi_mt76_iocb_t *iocb = &uwifi->rx_cmd_iocb;
        vk_usbh_urb_prepare(&iocb->urb, dev, uwifi->in_ep[MT76_EP_IN_CMD_RESP].desc);
        if (VSF_ERR_NONE != vk_usbh_alloc_urb(usbh, dev, &iocb->urb))
            goto free_all;
        if (NULL == vk_usbh_urb_alloc_buffer(&iocb->urb,
                VSF_USBH_WIFI_MT76_CFG_RX_BUFSIZE))
            goto free_all;
    }

    uwifi->mt76_priv.bus_priv = uwifi;
    uwifi->mt76_priv.wifi     = &uwifi->wifi;
    __vk_usbh_wifi_mt76_ep0_queue_init(&uwifi->ep0_queue);

    /* MT76 is a register-based chip with chip-private extensions.  The shared
     * register-bus part goes to wifi->reg_bus so the generic helper can be
     * used; the full vtable goes to wifi->bus_ops for the chip driver. */
    static const vsf_wifi_mt76_bus_ops_t __vk_usbh_wifi_mt76_bus_ops = {
        .base = {
            .reg_write  = __vk_usbh_wifi_mt76_reg_write,
            .reg_read   = __vk_usbh_wifi_mt76_reg_read,
            .on_ready   = __vk_usbh_wifi_mt76_on_ready,
        },
        .fce_write      = __vk_usbh_wifi_mt76_fce_write,
        .dev_cmd        = __vk_usbh_wifi_mt76_dev_cmd,
        .dev_class_cmd  = __vk_usbh_wifi_mt76_dev_class_cmd,
        .mcu_cmd        = __vk_usbh_wifi_mt76_mcu_cmd,
        .tx_frame       = __vk_usbh_wifi_mt76_tx_frame,
        .rx_submit      = __vk_usbh_wifi_mt76_rx_submit,
    };
    vsf_wifi_init(&uwifi->wifi, &vsf_wifi_mt76_drv,
            &__vk_usbh_wifi_mt76_bus_ops.base, &uwifi->eda);
    uwifi->wifi.chip_priv = &uwifi->mt76_priv;
    uwifi->wifi.bus_ops   = &__vk_usbh_wifi_mt76_bus_ops;
    vsf_wifi_set_attach_fail(&uwifi->wifi, __vk_usbh_wifi_mt76_attach_fail);

    uwifi->eda.fn.evthandler = __vk_usbh_wifi_mt76_evthandler;
    uwifi->eda.on_terminate  = __vk_usbh_wifi_mt76_on_eda_terminate;
    vsf_eda_init(&uwifi->eda);
#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info(&uwifi->eda, "usbh_wifi_mt76", NULL, 0);
#endif
    vsf_wifi_chip_mt76_trace_debug(
        "mt76usb: probe ok, starting wifi" VSF_TRACE_CFG_LINEEND);
    vsf_wifi_on_new(&uwifi->wifi);
    return uwifi;

free_all:
    vsf_wifi_chip_mt76_trace_debug(
        "mt76usb: probe failed" VSF_TRACE_CFG_LINEEND);
    if (uwifi->usbh != NULL) {
        __vk_usbh_wifi_mt76_free_all_urbs(uwifi);
    }
    vsf_usbh_free(uwifi);
    return NULL;
}

static void __vk_usbh_wifi_mt76_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
                                           void *param)
{
    vk_usbh_wifi_mt76_t *uwifi = (vk_usbh_wifi_mt76_t *)param;
    (void)usbh; (void)dev;

    vsf_wifi_fini(&uwifi->wifi);
    vsf_eda_fini(&uwifi->eda);
}

static void __vk_usbh_wifi_mt76_attach_fail(vsf_wifi_t *wifi, vsf_err_t err)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);
    (void)err;
    vk_usbh_remove_interface(uwifi->usbh, uwifi->dev, uwifi->ifs);
}

static void __vk_usbh_wifi_mt76_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_wifi_mt76_t *uwifi = __this_uwifi(eda);
    __vk_usbh_wifi_mt76_free_all_urbs(uwifi);
    vsf_usbh_free(uwifi);
}

/*============================ EVENT HANDLER =================================*/

static void __vk_usbh_wifi_mt76_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_wifi_mt76_t *uwifi = __this_uwifi(eda);
    vk_usbh_dev_t *dev = uwifi->dev;

    if (uwifi->wifi.disconnecting) return;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_wifi_start(&uwifi->wifi);
        return;

    case VSF_EVT_SYNC:
        /* The EP0 crit was just released.  If we were waiting for it,
         * submit the already-prepared request; otherwise dispatch queued
         * requests. */
        if (uwifi->ep0_crit_pend) {
            uwifi->ep0_crit_pend = false;
            __vk_usbh_wifi_mt76_ep0_submit(uwifi);
        } else {
            __vk_usbh_wifi_mt76_ep0_dispatch(uwifi);
        }
        return;

    case VSF_WIFI_EVT_SCAN_HOP:
        vsf_wifi_on_scan_hop_evt(&uwifi->wifi);
        return;

    case VSF_WIFI_EVT_MLME_RETRY:
        vsf_wifi_on_mlme_retry_evt(&uwifi->wifi);
        return;

    case VSF_EVT_MESSAGE: {
        vk_usbh_hcd_urb_t *urb_hcd = (vk_usbh_hcd_urb_t *)vsf_eda_get_cur_msg();

        /* ep0 control transfer completion */
        if (urb_hcd == dev->ep0.urb.urb_hcd) {
            int status = vk_usbh_urb_get_status(&dev->ep0.urb);
            bool ok = (URB_OK == status);
            void *caller_buf = uwifi->ep0_buf;
            uint16_t len = uwifi->ep0_req.wLength;
            vsf_wifi_done_t done = uwifi->ep0_done;

            vsf_wifi_chip_mt76_trace_debug(
                "mt76usb: ep0 done state=%d ok=%d status=%d req=0x%02X" VSF_TRACE_CFG_LINEEND,
                uwifi->ep0_state, ok, status, uwifi->ep0_req.bRequest);

            /* FCE single-word writes are split into two 16-bit transfers. */
            if (ok && uwifi->ep0_state == MT76_EP0_FCE_LO) {
                struct usb_ctrlrequest_t *req_usb = &uwifi->ep0_req;

                uwifi->ep0_state = MT76_EP0_FCE_HI;
                req_usb->wValue  = (uint16_t)(uwifi->ep0_fce_val >> 16);
                req_usb->wIndex  = (uint16_t)((uwifi->ep0_fce_addr + 2) & 0xFFFF);

                vsf_err_t err = vk_usbh_control_msg_ex(uwifi->usbh, dev, req_usb,
                                                         0, &uwifi->eda);
                if (VSF_ERR_NONE == err) {
                    return;
                }
                /* Second phase failed; fall through to completion. */
                ok = false;
                status = -1;
            }

            vsf_err_t done_err = ok ? VSF_ERR_NONE : VSF_ERR_FAIL;
            uwifi->ep0_state = MT76_EP0_IDLE;
            uwifi->ep0_done  = NULL;
            uwifi->ep0_buf   = NULL;
            uwifi->ep0_busy  = false;

            if (ok && caller_buf != NULL && len == sizeof(uint32_t) &&
                (uwifi->ep0_req.bRequestType & USB_DIR_IN) != 0) {
                /* IN transfer: copy received data back to caller buffer. */
                uint8_t *hcd_buf = vk_usbh_urb_peek_buffer(&dev->ep0.urb);
                if (hcd_buf != NULL) {
                    memcpy(caller_buf, hcd_buf, len);
                }
            }
            vk_usbh_urb_free_buffer(&dev->ep0.urb);
            __vsf_eda_crit_npb_leave(&dev->ep0.crit);
            if (done != NULL) done(&uwifi->wifi, done_err);
            /* Kick the dispatcher in case more EP0 requests are queued. */
            __vk_usbh_wifi_mt76_ep0_dispatch(uwifi);
            return;
        }

        /* Bulk RX/TX completion */
        vk_usbh_wifi_mt76_iocb_t *iocb = __vk_usbh_wifi_mt76_find_iocb(uwifi, urb_hcd);
        if (iocb == NULL) {
            vsf_wifi_chip_mt76_trace_error(
                "mt76usb: iocb not found for urb_hcd=%p ep=0x%02X status=%d actual=%u" VSF_TRACE_CFG_LINEEND,
                (void *)urb_hcd, urb_hcd->pipe.endpoint,
                vk_usbh_urb_get_status((vk_usbh_urb_t *)urb_hcd),
                (unsigned)vk_usbh_urb_get_actual_length((vk_usbh_urb_t *)urb_hcd));
            for (int __i = 0; __i < dimof(uwifi->rx_pkt_iocb); __i++) {
                vsf_wifi_chip_mt76_trace_error(
                    "  rx_pkt_iocb[%d] urb_hcd=%p ep=0x%02X is_rx=%u" VSF_TRACE_CFG_LINEEND,
                    __i, (void *)uwifi->rx_pkt_iocb[__i].urb.urb_hcd,
                    uwifi->rx_pkt_iocb[__i].urb.urb_hcd ? uwifi->rx_pkt_iocb[__i].urb.urb_hcd->pipe.endpoint : 0xFF,
                    uwifi->rx_pkt_iocb[__i].is_rx);
            }
            vsf_wifi_chip_mt76_trace_error(
                "  rx_cmd_iocb urb_hcd=%p ep=0x%02X" VSF_TRACE_CFG_LINEEND,
                (void *)uwifi->rx_cmd_iocb.urb.urb_hcd,
                uwifi->rx_cmd_iocb.urb.urb_hcd ? uwifi->rx_cmd_iocb.urb.urb_hcd->pipe.endpoint : 0xFF);
            vsf_wifi_chip_mt76_trace_error(
                "  mcu_cmd_iocb urb_hcd=%p ep=0x%02X" VSF_TRACE_CFG_LINEEND,
                (void *)uwifi->mcu_cmd_iocb.urb.urb_hcd,
                uwifi->mcu_cmd_iocb.urb.urb_hcd ? uwifi->mcu_cmd_iocb.urb.urb_hcd->pipe.endpoint : 0xFF);
            for (int __i = 0; __i < dimof(uwifi->tx_iocb); __i++) {
                vsf_wifi_chip_mt76_trace_error(
                    "  tx_iocb[%d] urb_hcd=%p ep=0x%02X is_rx=%u" VSF_TRACE_CFG_LINEEND,
                    __i, (void *)uwifi->tx_iocb[__i].urb.urb_hcd,
                    uwifi->tx_iocb[__i].urb.urb_hcd ? uwifi->tx_iocb[__i].urb.urb_hcd->pipe.endpoint : 0xFF,
                    uwifi->tx_iocb[__i].is_rx);
            }
            VSF_USB_ASSERT(iocb != NULL);
        }

        if (iocb->is_rx) {
            int status = vk_usbh_urb_get_status(&iocb->urb);
            uint32_t len = (URB_OK == status)
                    ? vk_usbh_urb_get_actual_length(&iocb->urb) : 0;
            if (len > 0) {
                vsf_wifi_chip_mt76_trace_debug(
                    "mt76usb: rx done ep=0x%02X status=%d len=%u"
                    VSF_TRACE_CFG_LINEEND,
                    iocb->urb.urb_hcd->pipe.endpoint, status, (unsigned)len);
            } else {
                vsf_wifi_chip_mt76_trace_debug(
                    "mt76usb: rx done ep=0x%02X status=%d len=%u"
                    VSF_TRACE_CFG_LINEEND,
                    iocb->urb.urb_hcd->pipe.endpoint, status, (unsigned)len);
            }
            if (URB_OK == status && len > 0) {
                uint8_t *buf = vk_usbh_urb_peek_buffer(&iocb->urb);
                mt76_wifi_priv_t *priv = (mt76_wifi_priv_t *)uwifi->wifi.chip_priv;
                if (iocb->ep_idx == MT76_EP_IN_PKT_RX &&
                        priv->on_rx_pkt != NULL) {
                    priv->on_rx_pkt(&uwifi->wifi, buf, (uint16_t)len);
                } else if (priv->on_rx != NULL) {
                    priv->on_rx(&uwifi->wifi, buf, (uint16_t)len);
                }
            }
            if (iocb->ep_idx == MT76_EP_IN_PKT_RX) {
                vsf_err_t err = vk_usbh_submit_urb(uwifi->usbh, &iocb->urb);
                if (VSF_ERR_NONE != err) {
                    vk_usbh_remove_interface(uwifi->usbh, uwifi->dev, uwifi->ifs);
                }
            } else {
                iocb->is_busy = false;
            }
        } else {
            int status = vk_usbh_urb_get_status(&iocb->urb);
            uint32_t actual = vk_usbh_urb_get_actual_length(&iocb->urb);
            (void)actual;
            vsf_err_t done_err = (URB_OK == status) ? VSF_ERR_NONE : VSF_ERR_FAIL;
            vsf_wifi_chip_mt76_trace_debug(
                "mt76usb: tx done queue=%u status=%d actual=%u"
                VSF_TRACE_CFG_LINEEND,
                iocb->ep_idx, status, (unsigned)actual);
            iocb->is_busy = false;
            if (iocb->done != NULL) {
                vsf_wifi_done_t done = iocb->done;
                iocb->done = NULL;
                done(&uwifi->wifi, done_err);
            }
        }
        return;
    }

    default:
        return;
    }
}

/*============================ EP0 REQUEST QUEUE =============================
 *
 * MT76's EP0 state machine only allows one control transfer at a time.  The
 * chip driver issues many overlapping reg_read/reg_write/fce_write/dev_cmd
 * requests (firmware load, channel switch, key install, TX status polling).
 * Instead of requiring every caller to wait for ep0_state == IDLE, we queue
 * all EP0 requests here and dispatch them one by one.
 *============================================================================*/

static bool __vk_usbh_wifi_mt76_ep0_queue_init(
        vk_usbh_wifi_mt76_ep0_queue_t *q)
{
    memset(q, 0, sizeof(*q));
    return true;
}

static bool __vk_usbh_wifi_mt76_ep0_queue_enqueue(
        vk_usbh_wifi_mt76_ep0_queue_t *q,
        const vk_usbh_wifi_mt76_ep0_req_t *req)
{
    if (q->count >= MT76_EP0_QUEUE_SIZE) {
        return false;
    }
    q->reqs[q->tail] = *req;
    q->tail = (q->tail + 1) % MT76_EP0_QUEUE_SIZE;
    q->count++;
    return true;
}

static bool __vk_usbh_wifi_mt76_ep0_queue_dequeue(
        vk_usbh_wifi_mt76_ep0_queue_t *q,
        vk_usbh_wifi_mt76_ep0_req_t *req)
{
    if (q->count == 0) {
        return false;
    }
    *req = q->reqs[q->head];
    q->head = (q->head + 1) % MT76_EP0_QUEUE_SIZE;
    q->count--;
    return true;
}

static void __vk_usbh_wifi_mt76_ep0_dispatch(vk_usbh_wifi_mt76_t *uwifi);

static void __vk_usbh_wifi_mt76_ep0_done(vk_usbh_wifi_mt76_t *uwifi,
        vsf_err_t err)
{
    vsf_wifi_done_t done = uwifi->ep0_done;
    uwifi->ep0_done = NULL;
    if (done != NULL) {
        done(&uwifi->wifi, err);
    }
}

static void __vk_usbh_wifi_mt76_ep0_fail(vk_usbh_wifi_mt76_t *uwifi)
{
    vk_usbh_dev_t *dev = uwifi->dev;

    uwifi->ep0_state     = MT76_EP0_IDLE;
    uwifi->ep0_busy      = false;
    uwifi->ep0_crit_pend = false;
    vk_usbh_urb_free_buffer(&dev->ep0.urb);
    __vk_usbh_wifi_mt76_ep0_done(uwifi, VSF_ERR_FAIL);
    __vk_usbh_wifi_mt76_ep0_dispatch(uwifi);
}

/* Submit the already-prepared EP0 control request.  The caller must already
 * own (or have been transferred) dev->ep0.crit. */
static void __vk_usbh_wifi_mt76_ep0_submit(vk_usbh_wifi_mt76_t *uwifi)
{
    vk_usbh_dev_t *dev = uwifi->dev;

    vsf_err_t err = vk_usbh_control_msg_ex(uwifi->usbh, dev, &uwifi->ep0_req,
                                             0, &uwifi->eda);
    if (VSF_ERR_NONE != err) {
        __vsf_eda_crit_npb_leave(&dev->ep0.crit);
        __vk_usbh_wifi_mt76_ep0_fail(uwifi);
    }
}

static void __vk_usbh_wifi_mt76_ep0_start(
        vk_usbh_wifi_mt76_t *uwifi,
        const vk_usbh_wifi_mt76_ep0_req_t *req)
{
    vk_usbh_dev_t *dev = uwifi->dev;
    struct usb_ctrlrequest_t *req_usb = &uwifi->ep0_req;

    VSF_USB_ASSERT(!uwifi->ep0_busy);
    VSF_USB_ASSERT(uwifi->ep0_state == MT76_EP0_IDLE);

    uwifi->ep0_busy = true;
    *req_usb = req->req;
    uwifi->ep0_done  = req->done;
    uwifi->ep0_buf   = req->buf;
    uwifi->ep0_fce_addr = req->fce_addr;
    uwifi->ep0_fce_val  = req->fce_val;

    switch (req->type) {
    case MT76_EP0_REQ_READ:
        uwifi->ep0_state = MT76_EP0_MCU_CMD;
        if (NULL == vk_usbh_urb_alloc_buffer(&dev->ep0.urb, sizeof(uint32_t))) {
            goto fail;
        }
        break;
    case MT76_EP0_REQ_WRITE:
        uwifi->ep0_state = MT76_EP0_MCU_CMD;
        {
            uint8_t *out_buf = vk_usbh_urb_alloc_buffer(&dev->ep0.urb, sizeof(uint32_t));
            if (NULL == out_buf) goto fail;
            *(uint32_t *)out_buf = req->fce_val;
        }
        break;
    case MT76_EP0_REQ_FCE_WRITE:
        uwifi->ep0_state = MT76_EP0_FCE_LO;
        break;
    case MT76_EP0_REQ_DEV_CMD:
        uwifi->ep0_state = MT76_EP0_MCU_CMD;
        break;
    case MT76_EP0_REQ_DEV_CLASS_CMD:
        uwifi->ep0_state = MT76_EP0_MCU_CMD;
        if (req->class_len > 0) {
            uint8_t *out_buf = vk_usbh_urb_alloc_buffer(&dev->ep0.urb, req->class_len);
            if (NULL == out_buf) goto fail;
            memcpy(out_buf, req->class_data, req->class_len);
        }
        break;
    default:
        VSF_USB_ASSERT(false);
        goto fail;
    }

    vsf_err_t err = __vsf_eda_crit_npb_enter(&dev->ep0.crit);
    if (VSF_ERR_NONE != err) {
        /* The USBH core holds ep0.crit while probe() runs; it is released
         * after parse_ok and we will get VSF_EVT_SYNC. */
        uwifi->ep0_crit_pend = true;
        return;
    }
    __vk_usbh_wifi_mt76_ep0_submit(uwifi);
    return;

fail:
    uwifi->ep0_state = MT76_EP0_IDLE;
    uwifi->ep0_busy  = false;
    vk_usbh_urb_free_buffer(&dev->ep0.urb);
    __vk_usbh_wifi_mt76_ep0_done(uwifi, VSF_ERR_FAIL);
    __vk_usbh_wifi_mt76_ep0_dispatch(uwifi);
}

static void __vk_usbh_wifi_mt76_ep0_dispatch(vk_usbh_wifi_mt76_t *uwifi)
{
    if (uwifi->ep0_busy || uwifi->ep0_state != MT76_EP0_IDLE) {
        return;
    }

    vk_usbh_wifi_mt76_ep0_req_t req;
    if (!__vk_usbh_wifi_mt76_ep0_queue_dequeue(&uwifi->ep0_queue, &req)) {
        return;
    }

    __vk_usbh_wifi_mt76_ep0_start(uwifi, &req);
}

/*============================ MT76 BUS OPS ===================================
 *
 * The bus_ops interface is intentionally named after chip-level semantics
 * (on_ready, MCU command, TX frame, RX submit).  The USB class driver below
 * maps those to ep0 vendor requests and bulk URBs; other bus drivers would
 * map them to SDIO/SPI primitives without touching the chip driver.
 *============================================================================*/

static void __vk_usbh_wifi_mt76_on_ready(vsf_wifi_t *wifi)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);
    vsf_wifi_chip_mt76_trace_debug(
        "mt76usb: on_ready called" VSF_TRACE_CFG_LINEEND);
    if (!__vk_usbh_wifi_mt76_start_rx(uwifi)) {
        vk_usbh_remove_interface(uwifi->usbh, uwifi->dev, uwifi->ifs);
    }
}

static vsf_err_t __vk_usbh_wifi_mt76_reg_read(vsf_wifi_t *wifi,
    uint32_t reg, uint32_t *out, vsf_wifi_done_t done)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);
    vk_usbh_wifi_mt76_ep0_req_t req;
    uint8_t bRequest;

    if ((reg & MT76_VEND_TYPE_MASK) == MT76_VEND_TYPE_CFG) {
        bRequest = MT76_VEND_READ_CFG;
    } else if ((reg & MT76_VEND_TYPE_MASK) == MT76_VEND_TYPE_EEPROM) {
        bRequest = MT76_VEND_READ_EEPROM;
    } else {
        bRequest = MT76_VEND_MULTI_READ;
    }
    reg &= ~MT76_VEND_TYPE_MASK;

    memset(&req, 0, sizeof(req));
    req.type = MT76_EP0_REQ_READ;
    req.req.bRequestType = USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE;
    req.req.bRequest     = bRequest;
    req.req.wValue       = (uint16_t)(reg >> 16);
    req.req.wIndex       = (uint16_t)(reg & 0xFFFF);
    req.req.wLength      = sizeof(uint32_t);
    req.buf              = out;
    req.done             = done;

    if (!__vk_usbh_wifi_mt76_ep0_queue_enqueue(&uwifi->ep0_queue, &req)) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    __vk_usbh_wifi_mt76_ep0_dispatch(uwifi);
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbh_wifi_mt76_reg_write(vsf_wifi_t *wifi,
    uint32_t reg, uint32_t val, vsf_wifi_done_t done)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);
    vk_usbh_wifi_mt76_ep0_req_t req;
    uint8_t bRequest;

    if ((reg & MT76_VEND_TYPE_MASK) == MT76_VEND_TYPE_CFG) {
        bRequest = MT76_VEND_WRITE_CFG;
    } else {
        bRequest = MT76_VEND_MULTI_WRITE;
    }
    reg &= ~MT76_VEND_TYPE_MASK;

    memset(&req, 0, sizeof(req));
    req.type = MT76_EP0_REQ_WRITE;
    req.req.bRequestType = USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE;
    req.req.bRequest     = bRequest;
    req.req.wValue       = (uint16_t)(reg >> 16);
    req.req.wIndex       = (uint16_t)(reg & 0xFFFF);
    req.req.wLength      = sizeof(uint32_t);
    req.fce_val          = val;     /* reused as write value */
    req.done             = done;

    if (!__vk_usbh_wifi_mt76_ep0_queue_enqueue(&uwifi->ep0_queue, &req)) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    __vk_usbh_wifi_mt76_ep0_dispatch(uwifi);
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbh_wifi_mt76_fce_write(vsf_wifi_t *wifi,
    uint32_t addr, uint32_t val, vsf_wifi_done_t done)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);
    vk_usbh_wifi_mt76_ep0_req_t req;

    memset(&req, 0, sizeof(req));
    req.type = MT76_EP0_REQ_FCE_WRITE;
    req.req.bRequestType = USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE;
    req.req.bRequest     = MT76_VEND_WRITE_FCE;
    req.req.wValue       = (uint16_t)(val & 0xFFFF);
    req.req.wIndex       = (uint16_t)(addr & 0xFFFF);
    req.req.wLength      = 0;
    req.fce_addr         = addr;
    req.fce_val          = val;
    req.done             = done;

    if (!__vk_usbh_wifi_mt76_ep0_queue_enqueue(&uwifi->ep0_queue, &req)) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    __vk_usbh_wifi_mt76_ep0_dispatch(uwifi);
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbh_wifi_mt76_dev_cmd(vsf_wifi_t *wifi,
    uint8_t req, uint16_t value, uint16_t index, vsf_wifi_done_t done)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);
    vk_usbh_wifi_mt76_ep0_req_t ep0_req;

    memset(&ep0_req, 0, sizeof(ep0_req));
    ep0_req.type = MT76_EP0_REQ_DEV_CMD;
    ep0_req.req.bRequestType = USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE;
    ep0_req.req.bRequest     = req;
    ep0_req.req.wValue       = value;
    ep0_req.req.wIndex       = index;
    ep0_req.req.wLength      = 0;
    ep0_req.done             = done;

    if (!__vk_usbh_wifi_mt76_ep0_queue_enqueue(&uwifi->ep0_queue, &ep0_req)) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    __vk_usbh_wifi_mt76_ep0_dispatch(uwifi);
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbh_wifi_mt76_dev_class_cmd(vsf_wifi_t *wifi,
    uint8_t req, uint16_t value, uint16_t index,
    const uint8_t *data, uint16_t len, vsf_wifi_done_t done)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);
    vk_usbh_wifi_mt76_ep0_req_t ep0_req;

    if (len > MT76_EP0_CLASS_DATA_SIZE) {
        return VSF_ERR_NOT_SUPPORT;
    }

    memset(&ep0_req, 0, sizeof(ep0_req));
    ep0_req.type = MT76_EP0_REQ_DEV_CLASS_CMD;
    ep0_req.req.bRequestType = USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_DEVICE;
    ep0_req.req.bRequest     = req;
    ep0_req.req.wValue       = value;
    ep0_req.req.wIndex       = index;
    ep0_req.req.wLength      = len;
    ep0_req.class_len        = len;
    if (len > 0) {
        memcpy(ep0_req.class_data, data, len);
    }
    ep0_req.done             = done;

    if (!__vk_usbh_wifi_mt76_ep0_queue_enqueue(&uwifi->ep0_queue, &ep0_req)) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    __vk_usbh_wifi_mt76_ep0_dispatch(uwifi);
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbh_wifi_mt76_mcu_cmd(vsf_wifi_t *wifi,
    const uint8_t *data, uint16_t len, vsf_wifi_done_t done)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);

    if (len > VSF_USBH_WIFI_MT76_CFG_TX_BUFSIZE) return VSF_ERR_NOT_SUPPORT;
    if (uwifi->out_ep[MT76_EP_OUT_INBAND_CMD].desc == NULL) return VSF_ERR_NOT_SUPPORT;

    vk_usbh_wifi_mt76_iocb_t *iocb = &uwifi->mcu_cmd_iocb;
    if (iocb->is_busy) return VSF_ERR_NOT_AVAILABLE;

    uint8_t *cmd_buf = vk_usbh_urb_peek_buffer(&iocb->urb);
    if (NULL == cmd_buf) {
        /* The URB was pre-allocated in probe. Only the buffer may have been
         * freed by the HCD; re-allocate just the buffer, never the URB itself,
         * or the old HCD URB will leak and produce stray completion events. */
        cmd_buf = vk_usbh_urb_alloc_buffer(&iocb->urb, VSF_USBH_WIFI_MT76_CFG_TX_BUFSIZE);
        if (NULL == cmd_buf) return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }

    memcpy(cmd_buf, data, len);
    /* Do NOT use vk_usbh_urb_set_buffer with the URB-owned buffer:
     * set_buffer frees the current buffer and leaves free_buffer_param stale,
     * causing use-after-free / double-free. Just update transfer length. */
    iocb->urb.urb_hcd->transfer_length = len;
    iocb->is_busy = true;
    iocb->done    = done;

    vsf_err_t err = vk_usbh_submit_urb_ex(uwifi->usbh, &iocb->urb,
            0, &uwifi->eda);
    vsf_wifi_chip_mt76_trace_debug("mt76usb: submit_urb ep_hcd=0x%02X err=%d" VSF_TRACE_CFG_LINEEND,
            iocb->urb.urb_hcd->pipe.endpoint, (int)err);
    if (VSF_ERR_NONE != err) {
        iocb->is_busy = false;
        iocb->done    = NULL;
    }
    return err;
}

static vsf_err_t __vk_usbh_wifi_mt76_tx_frame(vsf_wifi_t *wifi,
    const uint8_t *data, uint16_t len, uint8_t queue_idx, vsf_wifi_done_t done)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);

    vsf_wifi_chip_mt76_trace_debug("mt76usb: tx_frame queue=%u len=%u ep=0x%02X" VSF_TRACE_CFG_LINEEND,
            queue_idx, len,
            uwifi->out_ep[queue_idx].desc ? uwifi->out_ep[queue_idx].desc->bEndpointAddress : 0xFF);

    if (queue_idx >= MT76_EP_OUT_MAX) return VSF_ERR_NOT_SUPPORT;
    if (len > VSF_USBH_WIFI_MT76_CFG_TX_BUFSIZE) return VSF_ERR_NOT_SUPPORT;
    if (uwifi->out_ep[queue_idx].desc == NULL) return VSF_ERR_NOT_SUPPORT;

    vk_usbh_wifi_mt76_iocb_t *iocb = NULL;
    for (int i = 0; i < dimof(uwifi->tx_iocb); i++) {
        if (!uwifi->tx_iocb[i].is_busy) {
            iocb = &uwifi->tx_iocb[i];
            break;
        }
    }
    if (NULL == iocb) return VSF_ERR_NOT_AVAILABLE;
    iocb->is_supported = true;

    vsf_wifi_chip_mt76_trace_debug("mt76usb: selected iocb=%d alloced=%d ep_hcd=0x%02X" VSF_TRACE_CFG_LINEEND,
            (int)(iocb - uwifi->tx_iocb), (int)vk_usbh_urb_is_alloced(&iocb->urb),
            vk_usbh_urb_is_alloced(&iocb->urb) ? iocb->urb.urb_hcd->pipe.endpoint : 0xFF);

    uint8_t *tx_buf = vk_usbh_urb_peek_buffer(&iocb->urb);
    if (NULL == tx_buf) {
        /* The URB was pre-allocated in probe. Only the buffer may have been
         * freed by the HCD; re-allocate just the buffer, never the URB itself,
         * or the old HCD URB will leak and produce stray completion events. */
        tx_buf = vk_usbh_urb_alloc_buffer(&iocb->urb, VSF_USBH_WIFI_MT76_CFG_TX_BUFSIZE);
        if (NULL == tx_buf) return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }

    memcpy(tx_buf, data, len);
    /* Do NOT use vk_usbh_urb_set_buffer with the URB-owned buffer:
     * set_buffer frees the current buffer and leaves free_buffer_param stale,
     * causing use-after-free / double-free. Just update transfer length. */
    iocb->urb.urb_hcd->transfer_length = len;

    vk_usbh_pipe_t target_pipe = vk_usbh_get_pipe_from_ep_desc(uwifi->dev,
            uwifi->out_ep[queue_idx].desc);
    vsf_wifi_chip_mt76_trace_debug("mt76usb: tx before switch iocb_ep=%d target_ep=%d" VSF_TRACE_CFG_LINEEND,
            iocb->urb.urb_hcd->pipe.endpoint, target_pipe.endpoint);
    if (iocb->urb.urb_hcd->pipe.endpoint != target_pipe.endpoint) {
        /* A pre-allocated IOCB may be bound to the firmware endpoint; switch
         * the underlying HCD pipe to the target TX queue endpoint. */
        iocb->urb.urb_hcd->pipe = target_pipe;
    }
    vsf_wifi_chip_mt76_trace_debug("mt76usb: tx after switch iocb_ep=%d" VSF_TRACE_CFG_LINEEND,
            iocb->urb.urb_hcd->pipe.endpoint);
    iocb->is_busy = true;
    iocb->ep_idx  = queue_idx;
    iocb->done    = done;

    vsf_err_t err = vk_usbh_submit_urb_ex(uwifi->usbh, &iocb->urb,
            0, &uwifi->eda);
    vsf_wifi_chip_mt76_trace_debug("mt76usb: submit_urb err=%d" VSF_TRACE_CFG_LINEEND, (int)err);
    if (VSF_ERR_NONE != err) {
        iocb->is_busy = false;
        iocb->done    = NULL;
    }
    return err;
}

static vsf_err_t __vk_usbh_wifi_mt76_rx_submit(vsf_wifi_t *wifi,
    uint8_t *buf, uint16_t len, uint8_t queue_idx)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);

    if (queue_idx >= MT76_EP_IN_MAX) return VSF_ERR_NOT_SUPPORT;
    if (uwifi->in_ep[queue_idx].desc == NULL) return VSF_ERR_NOT_SUPPORT;

    vk_usbh_wifi_mt76_iocb_t *iocb;
    if (queue_idx == MT76_EP_IN_CMD_RESP) {
        iocb = &uwifi->rx_cmd_iocb;
    } else {
        iocb = NULL;
        for (int i = 0; i < dimof(uwifi->rx_pkt_iocb); i++) {
            if (!uwifi->rx_pkt_iocb[i].is_busy) {
                iocb = &uwifi->rx_pkt_iocb[i];
                break;
            }
        }
    }
    if (NULL == iocb) return VSF_ERR_NOT_AVAILABLE;

    if (buf != NULL) {
        vk_usbh_urb_set_buffer(&iocb->urb, buf, len);
    } else {
        iocb->urb.urb_hcd->transfer_length = VSF_USBH_WIFI_MT76_CFG_RX_BUFSIZE;
    }
    vsf_wifi_chip_mt76_trace_debug(
        "mt76usb: rx_submit ep=0x%02X len=%u" VSF_TRACE_CFG_LINEEND,
        iocb->urb.urb_hcd->pipe.endpoint,
        (unsigned)iocb->urb.urb_hcd->transfer_length);
    iocb->urb.urb_hcd->timeout = 5000;
    iocb->is_rx        = true;
    iocb->is_supported = true;
    iocb->ep_idx       = queue_idx;
    iocb->is_busy      = true;

    vsf_err_t err = vk_usbh_submit_urb_ex(uwifi->usbh, &iocb->urb, 0, &uwifi->eda);
    if (VSF_ERR_NONE != err) {
        iocb->is_busy = false;
    }
    return err;
}

#endif      /* VSF_USE_USB_HOST && VSF_USBH_USE_MT76 && VSF_USE_WIFI */
