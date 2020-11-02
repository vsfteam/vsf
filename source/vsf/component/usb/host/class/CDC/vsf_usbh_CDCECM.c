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

#if     VSF_USE_USB_HOST == ENABLED                                             \
    &&  VSF_USBH_USE_ECM == ENABLED                                             \
    &&  VSF_USE_TCPIP == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_NETDRV_CLASS_INHERIT_NETLINK__
#define __VSF_USBH_CDC_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__

// for ctype.h
#include "utilities/vsf_utilities.h"
#include "kernel/vsf_kernel.h"
#include "../../vsf_usbh.h"
#include "./vsf_usbh_CDCECM.h"
#include "component/tcpip/vsf_tcpip.h"

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

#define VSF_USBH_ECM_ETH_HEADER_SIZE        6
#define VSF_USBH_ECM_MAC_STRING_SIZE        (2 + 2 * 2 * VSF_USBH_ECM_ETH_HEADER_SIZE)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_usbh_ecm_iocb_t {
    void *netbuf;
    vk_usbh_urb_t urb;
#if VSF_USBH_CDCECM_SUPPORT_PBUF == ENABLED
    uint8_t buffer[1500 + TCPIP_ETH_HEADSIZE];
#endif
} vk_usbh_ecm_iocb_t;

typedef struct vk_usbh_ecm_iocb_t vk_usbh_ecm_ocb_t;
typedef struct vk_usbh_ecm_iocb_t vk_usbh_ecm_icb_t;

typedef struct vk_usbh_ecm_t {
    implement(vk_usbh_cdc_t)

    vk_netdrv_t netdrv;
    vk_usbh_ecm_ocb_t ocb[VSF_USBH_CDCECM_CFG_NUM_OF_OCB];
    vk_usbh_ecm_icb_t icb[VSF_USBH_CDCECM_CFG_NUM_OF_ICB];

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
} vk_usbh_ecm_t;

/*============================ PROTOTYPES ====================================*/

static void *__vk_usbh_ecm_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_ecm_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

static vsf_err_t __vk_usbh_ecm_netlink_init(vk_netdrv_t *netdrv);
static vsf_err_t __vk_usbh_ecm_netlink_fini(vk_netdrv_t *netdrv);
static bool __vk_usbh_ecm_netlink_can_output(vk_netdrv_t *netdrv);
static vsf_err_t __vk_usbh_ecm_netlink_output(vk_netdrv_t *netdrv, void *netbuf);

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_ecm_dev_id[] = {
    { VSF_USBH_MATCH_IFS_CLASS(USB_CLASS_COMM, 6, 0) },
};

static const struct vk_netlink_op_t __vk_usbh_ecm_netlink_op =
{
    .init       = __vk_usbh_ecm_netlink_init,
    .fini       = __vk_usbh_ecm_netlink_fini,
    .can_output = __vk_usbh_ecm_netlink_can_output,
    .output     = __vk_usbh_ecm_netlink_output,
};

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_ecm_drv = {
    .name       = "cdc_ecm",
    .dev_id_num = dimof(__vk_usbh_ecm_dev_id),
    .dev_ids    = __vk_usbh_ecm_dev_id,
    .probe      = __vk_usbh_ecm_probe,
    .disconnect = __vk_usbh_ecm_disconnect,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vk_usbh_ecm_icb_t * __vk_usbh_ecm_get_icb(vk_usbh_ecm_t *ecm, vk_usbh_urb_t *urb)
{
    vk_usbh_ecm_icb_t *icb = ecm->icb;
    for (int i = 0; i < dimof(ecm->icb); i++, icb++) {
        if (icb->urb.urb_hcd == urb->urb_hcd) {
            return icb;
        }
    }
    return NULL;
}

static vk_usbh_ecm_ocb_t * __vk_usbh_ecm_get_ocb(vk_usbh_ecm_t *ecm, vk_usbh_urb_t *urb)
{
    vk_usbh_ecm_ocb_t *ocb = ecm->ocb;
    for (int i = 0; i < dimof(ecm->ocb); i++, ocb++) {
        if (ocb->urb.urb_hcd == urb->urb_hcd) {
            return ocb;
        }
    }
    return NULL;
}

static vk_usbh_ecm_ocb_t * __vk_usbh_ecm_get_idle_ocb(vk_usbh_ecm_t *ecm)
{
    vk_usbh_ecm_ocb_t *ocb = ecm->ocb;
    for (int i = 0; i < dimof(ecm->ocb); i++, ocb++) {
        if (NULL == ocb->netbuf) {
            return ocb;
        }
    }
    return NULL;
}

static void __vk_usbh_ecm_recv(vk_usbh_ecm_t *ecm, vk_usbh_ecm_icb_t *icb)
{
    if (NULL == icb->netbuf) {
        icb->netbuf = vk_netdrv_alloc_buf(&ecm->netdrv);
        if (icb->netbuf != NULL) {
#if VSF_USBH_CDCECM_SUPPORT_PBUF == ENABLED
            vk_usbh_urb_set_buffer(&icb->urb, icb->buffer, sizeof(icb->buffer));
#else
            vsf_mem_t mem;
            void *netbuf = vk_netdrv_read_buf(&ecm->netdrv, icb->netbuf, &mem);
            VSF_USB_ASSERT(netbuf == NULL);
            vk_usbh_urb_set_buffer(&icb->urb, mem.buffer, mem.size);
#endif
            if (VSF_ERR_NONE != vk_usbh_cdc_submit_urb(&ecm->use_as__vk_usbh_cdc_t, &icb->urb)) {
                vk_netdrv_on_inputted(&ecm->netdrv, icb->netbuf, 0);
            }
        }
    }
}

static vsf_err_t __vk_usbh_ecm_netlink_init(vk_netdrv_t *netdrv)
{
    vk_usbh_ecm_t *ecm = container_of(netdrv, vk_usbh_ecm_t, netdrv);

    netdrv->mtu = ecm->max_segment_size - TCPIP_ETH_HEADSIZE;
    netdrv->mac_header_size = TCPIP_ETH_HEADSIZE;
    netdrv->hwtype = TCPIP_ETH_HWTYPE;
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_usbh_ecm_netlink_fini(vk_netdrv_t *netdrv)
{
    if (netdrv->is_to_free) {
        vk_usbh_ecm_t *ecm = container_of(netdrv, vk_usbh_ecm_t, netdrv);
        vsf_pnp_on_netdrv_del(&ecm->netdrv);
        vsf_usbh_free(ecm);
    }
    return VSF_ERR_NONE;
}

static bool __vk_usbh_ecm_netlink_can_output(vk_netdrv_t *netdrv)
{
    vk_usbh_ecm_t *ecm = container_of(netdrv, vk_usbh_ecm_t, netdrv);
    return NULL != __vk_usbh_ecm_get_idle_ocb(ecm);
}

static vsf_err_t __vk_usbh_ecm_netlink_output(vk_netdrv_t *netdrv, void *netbuf)
{
    vk_usbh_ecm_t *ecm = container_of(netdrv, vk_usbh_ecm_t, netdrv);
    vk_usbh_ecm_ocb_t *ocb = __vk_usbh_ecm_get_idle_ocb(ecm);
    vsf_err_t err = VSF_ERR_FAIL;
    vsf_mem_t mem;

    ocb->netbuf = netbuf;
#if VSF_USBH_CDCECM_SUPPORT_PBUF == ENABLED
    if ((netbuf = vk_netdrv_read_buf(netdrv, netbuf, &mem)) != NULL) {
        uint_fast16_t pos = 0;
        do {
            VSF_USB_ASSERT((mem.size + pos) <= sizeof(ocb->buffer));
            memcpy(&ocb->buffer[pos], mem.buffer, mem.size);
            pos += mem.size;
        } while ((netbuf = vk_netdrv_read_buf(netdrv, netbuf, &mem)) != NULL);
        mem.buffer = ocb->buffer;
        mem.size = pos;
    }
#else
    if (vk_netdrv_read_buf(netdrv, netbuf, &mem) != NULL) {
        VSF_USB_ASSERT(false);
    }
#endif

#if VSF_USBH_CDCECM_CFG_TRACE_DATA_EN == ENABLED
    vsf_trace_debug("ecm_output :" VSF_TRACE_CFG_LINEEND);
    vsf_trace_buffer(VSF_TRACE_DEBUG, mem.buffer, mem.size, VSF_TRACE_DF_DEFAULT);
#endif
    vk_usbh_urb_set_buffer(&ocb->urb, mem.buffer, mem.size);
    err = vk_usbh_cdc_submit_urb(&ecm->use_as__vk_usbh_cdc_t, &ocb->urb);
    if (err != VSF_ERR_NONE) {
        ocb->netbuf = NULL;
    }
    return err;
}

static vsf_err_t __vk_usbh_ecm_on_cdc_evt(vk_usbh_cdc_t *cdc, vk_usbh_cdc_evt_t evt, void *param)
{
    vk_usbh_ecm_t *ecm = (vk_usbh_ecm_t *)cdc;

    switch (evt) {
    case VSF_USBH_CDC_ON_INIT:
        for (int i = 0; i < dimof(ecm->icb); i++) {
            vk_usbh_cdc_prepare_urb(&ecm->use_as__vk_usbh_cdc_t, false, &ecm->icb[i].urb);
        }
        for (int i = 0; i < dimof(ecm->ocb); i++) {
            vk_usbh_cdc_prepare_urb(&ecm->use_as__vk_usbh_cdc_t, true, &ecm->ocb[i].urb);
        }
        break;
    case VSF_USBH_CDC_ON_DESC: {
            usb_cdc_ecm_descriptor_t *ecm_desc = param;

            switch (ecm_desc->bDescriptorSubType) {
            case 0x0F:        // Ethernet Networking Functional Descriptor
                ecm->iMAC = ecm_desc->iMACAddress;
                ecm->max_segment_size = le16_to_cpu(ecm_desc->wMaxSegmentSize);
                break;
            }
        }
        break;
    case VSF_USBH_CDC_ON_EVENT:
        switch (ecm->evt[1]) {
        case 0x00: {          // NETWORK_CONNECTION
                bool connected = vk_netdrv_is_connected(&ecm->netdrv);
                if (connected && ecm->evt[2] == 0) {
                    vsf_trace_info("ecm_event: NETWORK_CONNECTION Disconnected" VSF_TRACE_CFG_LINEEND);
                    vk_netdrv_disconnect(&ecm->netdrv);
                } else if (!connected && (ecm->evt[2] != 0)) {
                    vsf_trace_info("ecm_event: NETWORK_CONNECTION Connected" VSF_TRACE_CFG_LINEEND);
                    vk_netdrv_connect(&ecm->netdrv);

                    vk_usbh_ecm_icb_t *icb = ecm->icb;
                    for (int i = 0; i < dimof(ecm->icb); i++, icb++) {
                        __vk_usbh_ecm_recv(ecm, icb);
                    }
                }
            }
            break;
        case 0x2A:            // CONNECTION_SPEED_CHANGE
//            vsf_trace_info("ecm_event: CONNECTION_SPEED_CHANGE" VSF_TRACE_CFG_LINEEND);
            break;
        default:
            vsf_trace_error("ecm_event: unknown(%d)" VSF_TRACE_CFG_LINEEND, ecm->evt[0]);
            break;
        }
        break;
    case VSF_USBH_CDC_ON_RX: {
            int_fast32_t size;
            vk_usbh_ecm_icb_t *icb = __vk_usbh_ecm_get_icb(ecm, (vk_usbh_urb_t *)param);

            if (URB_OK != vk_usbh_urb_get_status(&icb->urb)) {
                size = -1;
            } else {
                size = vk_usbh_urb_get_actual_length(&icb->urb);
            }

            if (size > 0) {
                vsf_mem_t mem;
                vk_netdrv_t *netdrv = &ecm->netdrv;

#if VSF_USBH_CDCECM_SUPPORT_PBUF == ENABLED
                void *netbuf = icb->netbuf;
                uint_fast32_t remain = size;
                size_t cur_size;
                uint8_t *buffer = icb->buffer;

#   if VSF_USBH_CDCECM_CFG_TRACE_DATA_EN == ENABLED
                vsf_trace_debug("ecm_input :" VSF_TRACE_CFG_LINEEND);
                vsf_trace_buffer(VSF_TRACE_DEBUG, buffer, remain, VSF_TRACE_DF_DEFAULT);
#   endif

                do {
                    netbuf = vk_netdrv_read_buf(netdrv, netbuf, &mem);
                    cur_size = min(mem.size, remain);
                    memcpy(mem.buffer, buffer, cur_size);
                    remain -= cur_size;
                    buffer += cur_size;
                } while ((netbuf != NULL) && (remain > 0));
#else
                if (!vk_netdrv_read_buf(netdrv, icb->netbuf, &mem)) {
#   if VSF_USBH_CDCECM_CFG_TRACE_DATA_EN == ENABLED
                    vsf_trace_debug("ecm_input :" VSF_TRACE_CFG_LINEEND);
                    vsf_trace_buffer(VSF_TRACE_DEBUG, mem.buffer, size, VSF_TRACE_DF_DEFAULT);
#   endif
                }
#endif
            }

            vk_netdrv_on_inputted(&ecm->netdrv, icb->netbuf, size);
            icb->netbuf = NULL;
            if (vk_netdrv_is_connected(&ecm->netdrv)) {
                __vk_usbh_ecm_recv(ecm, icb);
            }
        }
        break;
    case VSF_USBH_CDC_ON_TX: {
            int_fast32_t size;
            vk_usbh_ecm_ocb_t *ocb = __vk_usbh_ecm_get_ocb(ecm, (vk_usbh_urb_t *)param);

            if (URB_OK != vk_usbh_urb_get_status(&ocb->urb)) {
                size = -1;
            } else {
                size = vk_usbh_urb_get_actual_length(&ocb->urb);
            }

            vk_netdrv_on_outputted(&ecm->netdrv, ocb->netbuf, size);
            ocb->netbuf = NULL;
        }
        break;
    }
    return VSF_ERR_NONE;
}

// TODO: move hex_to_bin to misc
int hex_to_bin(char ch)
{
    if ((ch >= '0') && (ch <= '9')) {
        return ch - '0';
    }
    ch = tolower(ch);
    if ((ch >= 'a') && (ch <= 'f')) {
        return ch - 'a' + 10;
    }
    return -1;
}

static void __vk_usbh_ecm_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_cdc_t *cdc = container_of(eda, vk_usbh_cdc_t, eda);
    vk_usbh_ecm_t *ecm = container_of(cdc, vk_usbh_ecm_t, use_as__vk_usbh_cdc_t);
    vk_usbh_dev_t *dev = cdc->dev;
    vk_usbh_t *usbh = cdc->usbh;
    vk_usbh_urb_t *urb = &dev->ep0.urb;
    vsf_err_t err = VSF_ERR_NONE;

    switch (evt) {
    case VSF_EVT_INIT:
        ecm->init_state = VSF_USBH_ECM_INIT_START;
        err = __vsf_eda_crit_npb_enter(&dev->ep0.crit);
        if (err != VSF_ERR_NONE) {
            break;
        }
        ecm->init_state++;
        // fall through
    case VSF_EVT_SYNC:
        if (NULL == vk_usbh_urb_alloc_buffer(urb, VSF_USBH_ECM_MAC_STRING_SIZE)) {
            break;
        }

        err = vk_usbh_get_descriptor(usbh, dev, USB_DT_STRING, ecm->iMAC, VSF_USBH_ECM_MAC_STRING_SIZE);
        break;
    case VSF_EVT_MESSAGE:
        if (vk_usbh_urb_get_status(urb) != URB_OK) {
            break;
        }

        switch (ecm->init_state) {
        case VSF_USBH_ECM_INIT_WAIT_MAC:
            if (vk_usbh_urb_get_actual_length(urb) != VSF_USBH_ECM_MAC_STRING_SIZE) {
                break;
            }

            {
                char *str = (char *)vk_usbh_urb_take_buffer(urb) + 2;
                for (uint_fast8_t i = 0; i < VSF_USBH_ECM_ETH_HEADER_SIZE; i++, str += 4) {
                    ecm->netdrv.macaddr.addr_buf[i] = (hex_to_bin(str[0]) << 4) | (hex_to_bin(str[2]) << 0);
                }
                ecm->netdrv.macaddr.size = VSF_USBH_ECM_ETH_HEADER_SIZE;
            }
            vk_usbh_urb_free_buffer(urb);

            vsf_trace_info("cdc_cdc: MAC is %02X:%02X:%02X:%02X:%02X:%02X" VSF_TRACE_CFG_LINEEND,
                    ecm->netdrv.macaddr.addr_buf[0], ecm->netdrv.macaddr.addr_buf[1],
                    ecm->netdrv.macaddr.addr_buf[2], ecm->netdrv.macaddr.addr_buf[3],
                    ecm->netdrv.macaddr.addr_buf[4], ecm->netdrv.macaddr.addr_buf[5]);

            err = vk_usbh_set_interface(usbh, dev, cdc->data_ifs, 0);
            break;
        case VSF_USBH_ECM_INIT_WAIT_SET_IF0:
            err = vk_usbh_set_interface(usbh, dev, cdc->data_ifs, 1);
            break;
        case VSF_USBH_ECM_INIT_WAIT_SET_IF1:
            __vsf_eda_crit_npb_leave(&dev->ep0.crit);

            ecm->netdrv.netlink.op = &__vk_usbh_ecm_netlink_op;
            vsf_pnp_on_netdrv_new(&ecm->netdrv);

            eda->fn.evthandler = vk_usbh_cdc_evthandler;
            vsf_eda_post_evt(eda, VSF_EVT_INIT);
            return;
        }
    }

    if (err < 0) {
        if (ecm->init_state != VSF_USBH_ECM_INIT_START) {
            __vsf_eda_crit_npb_leave(&dev->ep0.crit);
        }
        vk_usbh_remove_interface(usbh, dev, cdc->ifs);
    } else {
        ecm->init_state++;
    }
}

static void __vk_usbh_ecm_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_ecm_t *ecm = container_of(eda, vk_usbh_ecm_t, eda);
    vk_netdrv_t *netdrv = &ecm->netdrv;

    netdrv->is_to_free = true;
    if (vk_netdrv_is_connected(netdrv)) {
        vk_netdrv_disconnect(netdrv);
    } else {
        __vk_usbh_ecm_netlink_fini(netdrv);
    }
}

static void *__vk_usbh_ecm_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ecm_t *ecm = vsf_usbh_malloc(sizeof(vk_usbh_ecm_t));
    if (ecm != NULL) {
        vk_usbh_cdc_t *cdc = &ecm->use_as__vk_usbh_cdc_t;
        memset(ecm, 0, sizeof(*ecm));
        cdc->evthandler = __vk_usbh_ecm_on_cdc_evt;
        cdc->evt_buffer = ecm->evt;
        cdc->evt_size = sizeof(ecm->evt);
        if (VSF_ERR_NONE != vk_usbh_cdc_init(cdc, usbh, dev, parser_ifs)) {
            vsf_usbh_free(ecm);
            ecm = NULL;
        } else {
            cdc->eda.fn.evthandler = __vk_usbh_ecm_evthandler;
            cdc->eda.on_terminate = __vk_usbh_ecm_on_eda_terminate;
            vsf_eda_init(&cdc->eda, vsf_prio_inherit, false);
        }
    }
    return ecm;
}

static void __vk_usbh_ecm_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_ecm_t *ecm = (vk_usbh_ecm_t *)param;

    for (int i = 0; i < dimof(ecm->icb); i++) {
        vk_usbh_cdc_free_urb(&ecm->use_as__vk_usbh_cdc_t, &ecm->icb[i].urb);
    }
    for (int i = 0; i < dimof(ecm->ocb); i++) {
        vk_usbh_cdc_free_urb(&ecm->use_as__vk_usbh_cdc_t, &ecm->ocb[i].urb);
    }

    vk_usbh_cdc_fini(&ecm->use_as__vk_usbh_cdc_t);
    vsf_eda_fini(&ecm->eda);
}

#endif
