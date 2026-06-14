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
static vsf_err_t __vk_usbh_wifi_mt76_mcu_cmd(vsf_wifi_t *wifi,
    uint8_t req, uint8_t req_type, uint16_t value, uint16_t index,
    void *buf, uint16_t len, vsf_wifi_done_t done);
static vsf_err_t __vk_usbh_wifi_mt76_tx_frame(vsf_wifi_t *wifi,
    const uint8_t *data, uint16_t len, uint8_t queue_idx);
static vsf_err_t __vk_usbh_wifi_mt76_rx_submit(vsf_wifi_t *wifi,
    uint8_t *buf, uint16_t len, uint8_t queue_idx);

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
    uint8_t in_ep[MT76_EP_IN_MAX];
    uint8_t out_ep[MT76_EP_OUT_MAX];
    int in_cnt = 0, out_cnt = 0;

    if (desc_ifs->bInterfaceNumber != 0) return NULL;

    for (int i = 0; i < desc_ifs->bNumEndpoints; i++) {
        uint_fast8_t epaddr = desc_ep->bEndpointAddress;
        bool is_in = (epaddr & USB_DIR_MASK) == USB_DIR_IN;
        uint_fast8_t type = desc_ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;

        if (type == USB_ENDPOINT_XFER_BULK) {
            if (is_in && in_cnt < MT76_EP_IN_MAX) {
                in_ep[in_cnt++] = epaddr;
            } else if (!is_in && out_cnt < MT76_EP_OUT_MAX) {
                out_ep[out_cnt++] = epaddr;
            }
        } else if (type == USB_ENDPOINT_XFER_INT && is_in && in_cnt < MT76_EP_IN_MAX) {
            in_ep[in_cnt++] = epaddr;
        }
        desc_ep = (struct usb_endpoint_desc_t *)((uintptr_t)desc_ep
                + USB_DT_ENDPOINT_SIZE);
    }

    if (in_cnt == 0 || out_cnt == 0) return NULL;

    uwifi = vsf_usbh_malloc(sizeof(vk_usbh_wifi_mt76_t));
    if (NULL == uwifi) return NULL;
    memset(uwifi, 0, sizeof(vk_usbh_wifi_mt76_t));

    memcpy(uwifi->in_ep, in_ep, sizeof(in_ep));
    memcpy(uwifi->out_ep, out_ep, sizeof(out_ep));
    uwifi->usbh = usbh;
    uwifi->dev  = dev;
    uwifi->ifs  = ifs;

    uwifi->mt76_priv.bus_priv = uwifi;

    /* MT76 is a command/event chip; it does not use the register-bus helper.
     * It uses a chip-specific bus_ops vtable instead. */
    static const vsf_wifi_mt76_bus_ops_t __vk_usbh_wifi_mt76_bus_ops = {
        .mcu_cmd    = __vk_usbh_wifi_mt76_mcu_cmd,
        .tx_frame   = __vk_usbh_wifi_mt76_tx_frame,
        .rx_submit  = __vk_usbh_wifi_mt76_rx_submit,
    };
    vsf_wifi_init(&uwifi->wifi, &vsf_wifi_mt76_drv, NULL, &uwifi->eda);
    uwifi->wifi.chip_priv = &uwifi->mt76_priv;
    uwifi->wifi.bus_ops   = &__vk_usbh_wifi_mt76_bus_ops;
    vsf_wifi_set_attach_fail(&uwifi->wifi, __vk_usbh_wifi_mt76_attach_fail);

    uwifi->eda.fn.evthandler = __vk_usbh_wifi_mt76_evthandler;
    uwifi->eda.on_terminate  = __vk_usbh_wifi_mt76_on_eda_terminate;
    vsf_eda_init(&uwifi->eda);
#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info(&uwifi->eda, "usbh_wifi_mt76", NULL, 0);
#endif
    vsf_wifi_on_new(&uwifi->wifi);
    return uwifi;
}

static void __vk_usbh_wifi_mt76_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
                                           void *param)
{
    vk_usbh_wifi_mt76_t *uwifi = (vk_usbh_wifi_mt76_t *)param;
    (void)usbh; (void)dev;

    vsf_wifi_fini(&uwifi->wifi);
    /* URB cleanup happens in on_terminate after the EDA finishes. */
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
    vsf_usbh_free(uwifi);
}

/*============================ EVENT HANDLER =================================*/

static void __vk_usbh_wifi_mt76_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_wifi_mt76_t *uwifi = __this_uwifi(eda);

    if (uwifi->wifi.disconnecting) return;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_wifi_start(&uwifi->wifi);
        return;

    case VSF_WIFI_EVT_SCAN_HOP:
        vsf_wifi_on_scan_hop_evt(&uwifi->wifi);
        return;

    case VSF_WIFI_EVT_MLME_RETRY:
        vsf_wifi_on_mlme_retry_evt(&uwifi->wifi);
        return;

    case VSF_EVT_MESSAGE:
        /* TODO: dispatch ep0 / bulk IN / bulk OUT completions */
        return;

    default:
        return;
    }
}

/*============================ MT76 BUS OPS ===================================
 *
 * The bus_ops interface is intentionally named after chip-level semantics
 * (MCU command, TX frame, RX submit).  The USB class driver below maps those
 * to ep0 vendor requests and bulk URBs; other bus drivers would map them to
 * SDIO/SPI primitives without touching the chip driver.
 *============================================================================*/

static vsf_err_t __vk_usbh_wifi_mt76_mcu_cmd(vsf_wifi_t *wifi,
    uint8_t req, uint8_t req_type, uint16_t value, uint16_t index,
    void *buf, uint16_t len, vsf_wifi_done_t done)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);
    (void)uwifi; (void)req; (void)req_type; (void)value; (void)index;
    (void)buf; (void)len; (void)done;
    /* TODO: build usb_ctrlrequest_t and submit via vk_usbh_control_msg_ex */
    return VSF_ERR_NOT_SUPPORT;
}

static vsf_err_t __vk_usbh_wifi_mt76_tx_frame(vsf_wifi_t *wifi,
    const uint8_t *data, uint16_t len, uint8_t queue_idx)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);
    (void)uwifi; (void)data; (void)len; (void)queue_idx;
    /* TODO: submit USB bulk OUT URB */
    return VSF_ERR_NOT_SUPPORT;
}

static vsf_err_t __vk_usbh_wifi_mt76_rx_submit(vsf_wifi_t *wifi,
    uint8_t *buf, uint16_t len, uint8_t queue_idx)
{
    vk_usbh_wifi_mt76_t *uwifi = __vk_usbh_wifi_mt76_from_wifi(wifi);
    (void)uwifi; (void)buf; (void)len; (void)queue_idx;
    /* TODO: submit USB bulk IN URB */
    return VSF_ERR_NOT_SUPPORT;
}

#endif      /* VSF_USE_USB_HOST && VSF_USBH_USE_MT76 && VSF_USE_WIFI */
