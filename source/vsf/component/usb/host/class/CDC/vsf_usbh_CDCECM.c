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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED

#define VSF_USBH_IMPLEMENT_CLASS
#define VSF_USBH_CDC_INHERIT
#define VSF_NETDRV_INHERIT_NETLINK
#include "vsf.h"
#include <ctype.h>

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

#ifndef VSF_USBH_CDCECM_CFG_NUM_OF_OCB
#   define VSF_USBH_CDCECM_CFG_NUM_OF_OCB   1
#endif

#ifndef VSF_USBH_CDCECM_CFG_NUM_OF_ICB
#   define VSF_USBH_CDCECM_CFG_NUM_OF_ICB   1
#endif

#ifndef VSF_USBH_CDCECM_SUPPORT_PBUF
#   define VSF_USBH_CDCECM_SUPPORT_PBUF     ENABLED
#endif

#define VSF_USBH_ECM_ETH_HEADER_SIZE    6
#define VSF_USBH_ECM_MAC_STRING_SIZE    (2 + 2 * 2 * VSF_USBH_ECM_ETH_HEADER_SIZE)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vsf_usbh_ecm_iocb_t {
    void *netbuf;
    vsf_usbh_urb_t urb;
};
typedef struct vsf_usbh_ecm_iocb_t vsf_usbh_ecm_iocb_t;

struct vsf_usbh_ecm_ocb_t {
    void *netbuf;
    vsf_usbh_urb_t urb;
#if VSF_USBH_CDCECM_SUPPORT_PBUF == ENABLED
    uint8_t buffer[1514];
#endif
};
typedef struct vsf_usbh_ecm_ocb_t vsf_usbh_ecm_ocb_t;
typedef struct vsf_usbh_ecm_iocb_t vsf_usbh_ecm_icb_t;

struct vsf_usbh_ecm_t {
    implement(vsf_usbh_cdc_t)

    vsf_netdrv_t netdrv;
    vsf_usbh_ecm_ocb_t ocb[VSF_USBH_CDCECM_CFG_NUM_OF_OCB];
    vsf_usbh_ecm_icb_t icb[VSF_USBH_CDCECM_CFG_NUM_OF_ICB];

    uint8_t evt[16];
    uint16_t max_segment_size;
    uint8_t iMAC;
    enum {
        VSF_USBH_ECM_INIT_START,
        VSF_USBH_ECM_INIT_WAIT_CRIT,
        VSF_USBH_ECM_INIT_WAIT_MAC,
        VSF_USBH_ECM_INIT_WAIT_SET_IF0,
        VSF_USBH_ECM_INIT_WAIT_SET_IF1,
    } init_state;
};
typedef struct vsf_usbh_ecm_t vsf_usbh_ecm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION(".text.vsf.kernel.eda")
vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis);

/*============================ IMPLEMENTATION ================================*/

static vsf_usbh_ecm_icb_t * vsf_usbh_ecm_get_icb(vsf_usbh_ecm_t *ecm, vsf_usbh_urb_t *urb)
{
    vsf_usbh_ecm_icb_t *icb = ecm->icb;
    for (int i = 0; i < dimof(ecm->icb); i++, icb++) {
        if (icb->urb.urb_hcd == urb->urb_hcd) {
            return icb;
        }
    }
    return NULL;
}

static vsf_usbh_ecm_ocb_t * vsf_usbh_ecm_get_ocb(vsf_usbh_ecm_t *ecm, vsf_usbh_urb_t *urb)
{
    vsf_usbh_ecm_ocb_t *ocb = ecm->ocb;
    for (int i = 0; i < dimof(ecm->ocb); i++, ocb++) {
        if (ocb->urb.urb_hcd == urb->urb_hcd) {
            return ocb;
        }
    }
    return NULL;
}

static vsf_usbh_ecm_ocb_t * vsf_usbh_ecm_get_idle_ocb(vsf_usbh_ecm_t *ecm)
{
    vsf_usbh_ecm_ocb_t *ocb = ecm->ocb;
    for (int i = 0; i < dimof(ecm->ocb); i++, ocb++) {
        if (NULL == ocb->netbuf) {
            return ocb;
        }
    }
    return NULL;
}

static void vsf_usbh_ecm_recv(vsf_usbh_ecm_t *ecm, vsf_usbh_ecm_icb_t *icb)
{
    if (NULL == icb->netbuf) {
        icb->netbuf = vsf_netdrv_alloc_buf(&ecm->netdrv);
        if (icb->netbuf != NULL) {
            vsf_mem_t mem;
            void *netbuf = vsf_netdrv_read_buf(&ecm->netdrv, icb->netbuf, &mem);
            ASSERT(netbuf == NULL);
            vsf_usbh_urb_set_buffer(&icb->urb, mem.pchBuffer, mem.nSize);
            if (VSF_ERR_NONE != vsf_usbh_cdc_submit_urb(&ecm->use_as__vsf_usbh_cdc_t, &icb->urb)) {
                vsf_netdrv_on_inputted(&ecm->netdrv, icb->netbuf, 0);
            }
        }
    }
}

static vsf_err_t vsf_usbh_ecm_netlink_init(vsf_netdrv_t *netdrv)
{
    vsf_usbh_ecm_t *ecm = container_of(netdrv, vsf_usbh_ecm_t, netdrv);

    netdrv->mtu = ecm->max_segment_size - VSFIP_ETH_HEADSIZE;
    netdrv->mac_header_size = VSFIP_ETH_HEADSIZE;
    netdrv->hwtype = VSFIP_ETH_HWTYPE;
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbh_ecm_netlink_fini(vsf_netdrv_t *netdrv)
{
    if (netdrv->is_to_free) {
        vsf_usbh_ecm_t *ecm = container_of(netdrv, vsf_usbh_ecm_t, netdrv);
        vsf_pnp_on_netdrv_del(&ecm->netdrv);
        VSF_USBH_FREE(ecm);
    }
    return VSF_ERR_NONE;
}

static bool vsf_usbh_ecm_netlink_can_output(vsf_netdrv_t *netdrv)
{
    vsf_usbh_ecm_t *ecm = container_of(netdrv, vsf_usbh_ecm_t, netdrv);
    return NULL != vsf_usbh_ecm_get_idle_ocb(ecm);
}

static vsf_err_t vsf_usbh_ecm_netlink_output(vsf_netdrv_t *netdrv, void *netbuf)
{
    vsf_usbh_ecm_t *ecm = container_of(netdrv, vsf_usbh_ecm_t, netdrv);
    vsf_usbh_ecm_ocb_t *ocb = vsf_usbh_ecm_get_idle_ocb(ecm);
    vsf_err_t err = VSF_ERR_FAIL;
    vsf_mem_t mem;

    ocb->netbuf = netbuf;
#if VSF_USBH_CDCECM_SUPPORT_PBUF == ENABLED
    if ((netbuf = vsf_netdrv_read_buf(netdrv, netbuf, &mem)) != NULL) {
        uint_fast16_t pos = 0;
        do {
            ASSERT((mem.nSize + pos) <= sizeof(ocb->buffer));
            memcpy(&ocb->buffer[pos], mem.pchBuffer, mem.nSize);
            pos += mem.nSize;
        } while ((netbuf = vsf_netdrv_read_buf(netdrv, netbuf, &mem)) != NULL);
        mem.pchBuffer = ocb->buffer;
        mem.nSize = pos;
    }
#else
    if (vsf_netdrv_read_buf(netdrv, netbuf, &mem) != NULL) {
        ASSERT(false);
    }
#endif

#if VSF_USBH_CDCECM_CFG_TRACE_DATA_EN == ENABLED
    vsf_trace(VSF_TRACE_DEBUG, "ecm_output :" VSF_TRACE_CFG_LINEEND);
    vsf_trace_buffer(VSF_TRACE_DEBUG, mem.pchBuffer, mem.nSize, VSF_TRACE_DF_DEFAULT);
#endif
    vsf_usbh_urb_set_buffer(&ocb->urb, mem.pchBuffer, mem.nSize);
    err = vsf_usbh_cdc_submit_urb(&ecm->use_as__vsf_usbh_cdc_t, &ocb->urb);
    if (err != VSF_ERR_NONE) {
        ocb->netbuf = NULL;
    }
    return err;
}

static const struct vsf_netlink_op_t vsf_usbh_ecm_netlink_op =
{
    .init       = vsf_usbh_ecm_netlink_init,
    .fini       = vsf_usbh_ecm_netlink_fini,
    .can_output = vsf_usbh_ecm_netlink_can_output,
    .output     = vsf_usbh_ecm_netlink_output,
};

static vsf_err_t vsf_usbh_ecm_on_cdc_evt(vsf_usbh_cdc_t *cdc, vsf_usbh_cdc_evt_t evt, void *param)
{
    vsf_usbh_ecm_t *ecm = (vsf_usbh_ecm_t *)cdc;

    switch (evt) {
    case VSF_USBH_CDC_ON_INIT:
        for (int i = 0; i < dimof(ecm->icb); i++) {
            vsf_usbh_cdc_prepare_urb(&ecm->use_as__vsf_usbh_cdc_t, false, &ecm->icb[i].urb);
        }
        for (int i = 0; i < dimof(ecm->ocb); i++) {
            vsf_usbh_cdc_prepare_urb(&ecm->use_as__vsf_usbh_cdc_t, true, &ecm->ocb[i].urb);
        }
        break;
    case VSF_USBH_CDC_ON_DESC:
        do {
            usb_cdc_ecm_descriptor_t *ecm_desc = param;

            switch (ecm_desc->bDescriptorSubType) {
            case 0x0F:        // Ethernet Networking Functional Descriptor
                ecm->iMAC = ecm_desc->iMACAddress;
                ecm->max_segment_size = le16_to_cpu(ecm_desc->wMaxSegmentSize);
                break;
            }
        } while (0);
        break;
    case VSF_USBH_CDC_ON_EVENT:
        switch (ecm->evt[1]) {
        case 0x00:            // NETWORK_CONNECTION
            do {
                bool connected = vsf_netdrv_is_connected(&ecm->netdrv);
                if (connected && ecm->evt[2] == 0) {
                    vsf_trace(VSF_TRACE_INFO, "ecm_event: NETWORK_CONNECTION Disconnected" VSF_TRACE_CFG_LINEEND);
                    vsf_netdrv_disconnect(&ecm->netdrv);
                } else if (!connected && (ecm->evt[2] != 0)) {
                    vsf_trace(VSF_TRACE_INFO, "ecm_event: NETWORK_CONNECTION Connected" VSF_TRACE_CFG_LINEEND);
                    vsf_netdrv_connect(&ecm->netdrv);

                    vsf_usbh_ecm_icb_t *icb = ecm->icb;
                    for (int i = 0; i < dimof(ecm->icb); i++, icb++) {
                        vsf_usbh_ecm_recv(ecm, icb);
                    }
                }
            } while (0);
            break;
        case 0x2A:            // CONNECTION_SPEED_CHANGE
//            vsf_trace(VSF_TRACE_INFO, "ecm_event: CONNECTION_SPEED_CHANGE" VSF_TRACE_CFG_LINEEND);
            break;
        default:
            vsf_trace(VSF_TRACE_ERROR, "ecm_event: unknown(%d)" VSF_TRACE_CFG_LINEEND, ecm->evt[0]);
            break;
        }
        break;
    case VSF_USBH_CDC_ON_RX:
        do {
            int_fast32_t size;
            vsf_usbh_ecm_icb_t *icb = vsf_usbh_ecm_get_icb(ecm, (vsf_usbh_urb_t *)param);

            if (URB_OK != vsf_usbh_urb_get_status(&icb->urb)) {
                size = -1;
            } else {
                size = vsf_usbh_urb_get_actual_length(&icb->urb);
            }

            if (size > 0) {
                do {
                    vsf_mem_t mem;
                    vsf_netdrv_t *netdrv = &ecm->netdrv;

                    if (!vsf_netdrv_read_buf(netdrv, icb->netbuf, &mem)) {
#if VSF_USBH_CDCECM_CFG_TRACE_DATA_EN == ENABLED
                        vsf_trace(VSF_TRACE_DEBUG, "ecm_input :" VSF_TRACE_CFG_LINEEND);
                        vsf_trace_buffer(VSF_TRACE_DEBUG, mem.pchBuffer, size, VSF_TRACE_DF_DEFAULT);
#endif
                    }
                } while (0);
            }

            vsf_netdrv_on_inputted(&ecm->netdrv, icb->netbuf, size);
            icb->netbuf = NULL;
            if (vsf_netdrv_is_connected(&ecm->netdrv)) {
                vsf_usbh_ecm_recv(ecm, icb);
            }
        } while (0);
        break;
    case VSF_USBH_CDC_ON_TX:
        do {
            int_fast32_t size;
            vsf_usbh_ecm_ocb_t *ocb = vsf_usbh_ecm_get_ocb(ecm, (vsf_usbh_urb_t *)param);

            if (URB_OK != vsf_usbh_urb_get_status(&ocb->urb)) {
                size = -1;
            } else {
                size = vsf_usbh_urb_get_actual_length(&ocb->urb);
            }

            vsf_netdrv_on_outputted(&ecm->netdrv, ocb->netbuf, size);
            ocb->netbuf = NULL;
        } while (0);
        break;
    }
    return VSF_ERR_NONE;
}

static int hex_to_bin(char ch)
{
    if ((ch >= '0') && (ch <= '9'))
        return ch - '0';
    ch = tolower(ch);
    if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    return -1;
}

static void vsf_usbh_ecm_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_usbh_cdc_t *cdc = container_of(eda, vsf_usbh_cdc_t, eda);
    vsf_usbh_ecm_t *ecm = container_of(cdc, vsf_usbh_ecm_t, use_as__vsf_usbh_cdc_t);
    vsf_usbh_dev_t *dev = cdc->dev;
    vsf_usbh_t *usbh = cdc->usbh;
    vsf_usbh_urb_t *urb = &dev->ep0.urb;
    vsf_err_t err = VSF_ERR_NONE;

    switch (evt) {
    case VSF_EVT_INIT:
        ecm->init_state = VSF_USBH_ECM_INIT_START;
        err = vsf_eda_crit_enter(&dev->ep0.crit, -1);
        if (err != VSF_ERR_NONE) {
            break;
        }
        ecm->init_state++;
        // fall through
    case VSF_EVT_SYNC:
        if (NULL == vsf_usbh_urb_alloc_buffer(urb, VSF_USBH_ECM_MAC_STRING_SIZE)) {
            break;
        }

        err = vsf_usbh_get_descriptor(usbh, dev, USB_DT_STRING, ecm->iMAC, VSF_USBH_ECM_MAC_STRING_SIZE);
        break;
    case VSF_EVT_MESSAGE:
        if (vsf_usbh_urb_get_status(urb) != URB_OK) {
            break;
        }

        switch (ecm->init_state) {
        case VSF_USBH_ECM_INIT_WAIT_MAC:
            if (vsf_usbh_urb_get_actual_length(urb) != VSF_USBH_ECM_MAC_STRING_SIZE) {
                break;
            }

            do {
                char *str = (char *)vsf_usbh_urb_take_buffer(urb) + 2;
                for (uint_fast8_t i = 0; i < VSF_USBH_ECM_ETH_HEADER_SIZE; i++, str += 4) {
                    ecm->netdrv.macaddr.addr_buf[i] = (hex_to_bin(str[0]) << 4) | (hex_to_bin(str[2]) << 0);
                }
                ecm->netdrv.macaddr.size = VSF_USBH_ECM_ETH_HEADER_SIZE;
            } while (0);
            vsf_usbh_urb_free_buffer(urb);

            vsf_trace(VSF_TRACE_INFO, "cdc_cdc: MAC is %02X:%02X:%02X:%02X:%02X:%02X" VSF_TRACE_CFG_LINEEND,
                    ecm->netdrv.macaddr.addr_buf[0], ecm->netdrv.macaddr.addr_buf[1],
                    ecm->netdrv.macaddr.addr_buf[2], ecm->netdrv.macaddr.addr_buf[3],
                    ecm->netdrv.macaddr.addr_buf[4], ecm->netdrv.macaddr.addr_buf[5]);

            err = vsf_usbh_set_interface(usbh, dev, cdc->data_ifs, 0);
            break;
        case VSF_USBH_ECM_INIT_WAIT_SET_IF0:
            err = vsf_usbh_set_interface(usbh, dev, cdc->data_ifs, 1);
            break;
        case VSF_USBH_ECM_INIT_WAIT_SET_IF1:
            vsf_eda_crit_leave(&dev->ep0.crit);

            ecm->netdrv.netlink_op = &vsf_usbh_ecm_netlink_op;
            vsf_pnp_on_netdrv_new(&ecm->netdrv);

            eda->evthandler = vsf_usbh_cdc_evthandler;
            vsf_eda_post_evt(eda, VSF_EVT_INIT);
            return;
        }
    }

    if (err < 0) {
        if (ecm->init_state != VSF_USBH_ECM_INIT_START) {
            vsf_eda_crit_leave(&dev->ep0.crit);
        }
        vsf_usbh_remove_interface(usbh, dev, cdc->ifs);
    } else {
        ecm->init_state++;
    }
}

static void vsf_usbh_ecm_on_eda_terminate(vsf_eda_t *eda)
{
    vsf_usbh_ecm_t *ecm = container_of(eda, vsf_usbh_ecm_t, eda);
    vsf_netdrv_t *netdrv = &ecm->netdrv;

    netdrv->is_to_free = true;
    if (vsf_netdrv_is_connected(netdrv)) {
        vsf_netdrv_disconnect(netdrv);
    } else {
        vsf_usbh_ecm_netlink_fini(netdrv);
    }
}

static void *vsf_usbh_ecm_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
        vsf_usbh_ifs_parser_t *parser_ifs)
{
    vsf_usbh_ecm_t *ecm = VSF_USBH_MALLOC(sizeof(vsf_usbh_ecm_t));
    if (ecm != NULL) {
        vsf_usbh_cdc_t *cdc = &ecm->use_as__vsf_usbh_cdc_t;
        memset(ecm, 0, sizeof(*ecm));
        cdc->evthandler = vsf_usbh_ecm_on_cdc_evt;
        cdc->evt_buffer = ecm->evt;
        cdc->evt_size = sizeof(ecm->evt);
        if (VSF_ERR_NONE != vsf_usbh_cdc_init(cdc, usbh, dev, parser_ifs)) {
            VSF_USBH_FREE(ecm);
            ecm = NULL;
        } else {
            cdc->eda.evthandler = vsf_usbh_ecm_evthandler;
            cdc->eda.on_terminate = vsf_usbh_ecm_on_eda_terminate;
            vsf_eda_init(&cdc->eda, vsf_priority_inherit, false);
        }
    }
    return ecm;
}

static void vsf_usbh_ecm_disconnect(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, void *param)
{
    vsf_usbh_ecm_t *ecm = (vsf_usbh_ecm_t *)param;

    for (int i = 0; i < dimof(ecm->icb); i++) {
        vsf_usbh_cdc_free_urb(&ecm->use_as__vsf_usbh_cdc_t, &ecm->icb[i].urb);
    }
    for (int i = 0; i < dimof(ecm->ocb); i++) {
        vsf_usbh_cdc_free_urb(&ecm->use_as__vsf_usbh_cdc_t, &ecm->ocb[i].urb);
    }

    vsf_usbh_cdc_fini(&ecm->use_as__vsf_usbh_cdc_t);
    __vsf_eda_fini(&ecm->eda);
}

static const vsf_usbh_dev_id_t vsf_usbh_ecm_dev_id[] = {
    {
        .match_int_class    = true,
        .match_int_subclass = true,
        .match_int_protocol = true,

        .bInterfaceClass    = USB_CLASS_COMM,
        .bInterfaceSubClass = 6,        // ECM
        .bInterfaceProtocol = 0x00,
    },
};

const vsf_usbh_class_drv_t vsf_usbh_ecm_drv = {
    .name       = "cdc_ecm",
    .dev_id_num = dimof(vsf_usbh_ecm_dev_id),
    .dev_ids    = vsf_usbh_ecm_dev_id,
    .probe      = vsf_usbh_ecm_probe,
    .disconnect = vsf_usbh_ecm_disconnect,
};

#endif
