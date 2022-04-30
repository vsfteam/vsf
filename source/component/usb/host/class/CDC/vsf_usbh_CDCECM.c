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

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE != ENABLED
#   error "VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE is required"
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

// only for usb hcd supporting hardware transfer queue for one ep,
//  VSF_USBH_CDCECM_CFG_NUM_OF_OCB/VSF_USBH_CDCECM_CFG_NUM_OF_ICB can be larger than 1
#ifndef VSF_USBH_CDCECM_CFG_NUM_OF_OCB
#   define VSF_USBH_CDCECM_CFG_NUM_OF_OCB   1
#endif

#ifndef VSF_USBH_CDCECM_CFG_NUM_OF_ICB
#   define VSF_USBH_CDCECM_CFG_NUM_OF_ICB   1
#endif

#ifndef VSF_USBH_CDCECM_SUPPORT_PBUF
#   define VSF_USBH_CDCECM_SUPPORT_PBUF     ENABLED
#endif

#ifndef VSF_USBH_CDCECM_SUPPORT_THREAD
#   define VSF_USBH_CDCECM_SUPPORT_THREAD   ENABLED
#endif


#define VSF_USBH_ECM_ETH_HEADER_SIZE        6
#define VSF_USBH_ECM_MAC_STRING_SIZE        (2 + 2 * 2 * VSF_USBH_ECM_ETH_HEADER_SIZE)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vk_usbh_ecm_evt_t {
    VSF_USBH_CDCECM_ON_CONNECT,
    VSF_USBH_CDCECM_ON_DISCONNECT,
    VSF_USBH_CDCECM_ON_RX,
} vk_usbh_ecm_evt_t;

typedef struct vk_usbh_ecm_iocb_t {
#if VSF_USBH_CDCECM_SUPPORT_THREAD == ENABLED
    vsf_slist_node_t iocb_node;
#endif
    void *netbuf;
    vk_usbh_urb_t urb;
    int_fast32_t size;
#if VSF_USBH_CDCECM_SUPPORT_PBUF == ENABLED
    uint8_t buffer[1500 + TCPIP_ETH_HEADSIZE];
#endif
} vk_usbh_ecm_iocb_t;

typedef struct vk_usbh_ecm_iocb_t vk_usbh_ecm_ocb_t;
typedef struct vk_usbh_ecm_iocb_t vk_usbh_ecm_icb_t;

typedef struct vk_usbh_ecm_t {
    implement(vk_usbh_cdc_t)

#if VSF_USBH_CDCECM_SUPPORT_THREAD == ENABLED
    vsf_eda_t *thread;
    vsf_slist_queue_t iocb_queue;
    vsf_sem_t sem;
#endif

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
        VSF_USBH_ECM_INIT_WAIT_SET_IF1,
        VSF_USBH_ECM_INIT_WAIT_SET_FILTER,
    } init_state;
} vk_usbh_ecm_t;

/*============================ PROTOTYPES ====================================*/

static void *__vk_usbh_ecm_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_ecm_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

static vsf_err_t __vk_usbh_ecm_netlink_init(vk_netdrv_t *netdrv);
static vsf_err_t __vk_usbh_ecm_netlink_fini(vk_netdrv_t *netdrv);
static void * __vk_usbh_ecm_netlink_can_output(vk_netdrv_t *netdrv);
static vsf_err_t __vk_usbh_ecm_netlink_output(vk_netdrv_t *netdrv, void *slot, void *netbuf);

#if VSF_USBH_USE_LIBUSB == ENABLED
static void *__vk_usbh_ecm_block_libusb_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
extern void __vk_usbh_libusb_block_dev(vk_usbh_dev_t *dev);
#endif

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

#if VSF_USBH_USE_LIBUSB == ENABLED
static const vk_usbh_dev_id_t __vk_usbh_ecm_block_libusb_dev_id[] = {
    { VSF_USBH_MATCH_VID_PID(0x0BDA, 0x8152) },     // RTL8152 series
    { VSF_USBH_MATCH_VID_PID(0x0BDA, 0x8153) },     // RTL8153 series
    { VSF_USBH_MATCH_VID_PID(0x0FE6, 0x9900) },     // SR9900 series
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_ecm_drv = {
    .name       = "cdc_ecm",
    .dev_id_num = dimof(__vk_usbh_ecm_dev_id),
    .dev_ids    = __vk_usbh_ecm_dev_id,
    .probe      = __vk_usbh_ecm_probe,
    .disconnect = __vk_usbh_ecm_disconnect,
};

#if VSF_USBH_USE_LIBUSB == ENABLED
const vk_usbh_class_drv_t vk_usbh_ecm_block_libusb_drv = {
    .name       = "cdc_ecm_block_libusb",
    .dev_id_num = dimof(__vk_usbh_ecm_block_libusb_dev_id),
    .dev_ids    = __vk_usbh_ecm_block_libusb_dev_id,
    .probe      = __vk_usbh_ecm_block_libusb_probe,
};
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USBH_USE_LIBUSB == ENABLED
static void *__vk_usbh_ecm_block_libusb_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    __vk_usbh_libusb_block_dev(dev);
    return NULL;
}
#endif

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
        if (NULL == icb->netbuf) {
            vsf_trace_error("ecm: fail to allocate netbuf" VSF_TRACE_CFG_LINEEND);
            VSF_USB_ASSERT(false);
            return;
        }

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

static void * __vk_usbh_ecm_netlink_can_output(vk_netdrv_t *netdrv)
{
    vk_usbh_ecm_t *ecm = container_of(netdrv, vk_usbh_ecm_t, netdrv);
    vk_usbh_ecm_ocb_t *ocb = __vk_usbh_ecm_get_idle_ocb(ecm);
    if (ocb != NULL) {
        ocb->netbuf = (void *)1;
    }
    return ocb;
}

static vsf_err_t __vk_usbh_ecm_netlink_output(vk_netdrv_t *netdrv, void *slot, void *netbuf)
{
    vk_usbh_ecm_t *ecm = container_of(netdrv, vk_usbh_ecm_t, netdrv);
    vk_usbh_ecm_ocb_t *ocb = slot;
    vsf_err_t err = VSF_ERR_FAIL;
#if VSF_USBH_CDCECM_SUPPORT_PBUF == ENABLED
    uint_fast16_t pos = 0;
#endif
    vsf_mem_t mem;

    VSF_USB_ASSERT(ocb != NULL);
    ocb->netbuf = netbuf;
#if VSF_USBH_CDCECM_SUPPORT_PBUF == ENABLED
    void *netbuf_tmp = vk_netdrv_read_buf(netdrv, netbuf, &mem);
    while (true) {
        VSF_USB_ASSERT((mem.size + pos) <= sizeof(ocb->buffer));
        memcpy(&ocb->buffer[pos], mem.buffer, mem.size);
        pos += mem.size;

        if (NULL == netbuf_tmp) {
            break;
        }
        netbuf_tmp = vk_netdrv_read_buf(netdrv, netbuf_tmp, &mem);
    }
    mem.buffer = ocb->buffer;
    mem.size = pos;
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
    } else {
        // IMPORTANT: DO NOT use ocb->netbuf here, becase there is racing condition
        //  in __vk_usbh_ecm_on_cdc_evt, maybe ocb->netbuf is cleared here
//        vk_netdrv_on_netbuf_outputted(netdrv, ocb->netbuf);
        vk_netdrv_on_netbuf_outputted(netdrv, netbuf);
    }
    return err;
}

static void __vk_usbh_ecm_netdrv_evthandler(vk_usbh_ecm_t *ecm, vk_usbh_ecm_evt_t evt, void *param)
{
    vk_netdrv_t *netdrv = &ecm->netdrv;
    int_fast32_t size;

    switch (evt) {
    case VSF_USBH_CDCECM_ON_CONNECT: {
            vk_netdrv_connect(netdrv);

            vk_usbh_ecm_icb_t *icb = ecm->icb;
            for (int i = 0; i < dimof(ecm->icb); i++, icb++) {
                __vk_usbh_ecm_recv(ecm, icb);
            }
            break;
        }
    case VSF_USBH_CDCECM_ON_DISCONNECT:
        vk_netdrv_disconnect(netdrv);
        break;
    case VSF_USBH_CDCECM_ON_RX: {
            vk_usbh_ecm_icb_t *icb = param;
            size = icb->size;

            vk_netdrv_on_inputted(netdrv, icb->netbuf, size);
            icb->netbuf = NULL;
            if (vk_netdrv_is_connected(netdrv)) {
                __vk_usbh_ecm_recv(ecm, icb);
            }
            break;
        }
    }
}

#if VSF_USBH_CDCECM_SUPPORT_THREAD == ENABLED
static void __vk_usbh_ecm_netdrv_thread(void *param)
{
    vk_usbh_ecm_t *ecm = param;
    vsf_sync_reason_t reason;
    vk_usbh_ecm_iocb_t *iocb;
    vsf_protect_t orig;

    vsf_eda_sem_init(&ecm->sem);
    __vk_usbh_ecm_netdrv_evthandler(ecm, VSF_USBH_CDCECM_ON_CONNECT, NULL);

    while (true) {
        reason = vsf_thread_sem_pend(&ecm->sem, -1);
        VSF_USB_ASSERT(VSF_SYNC_GET == reason);

        orig = vsf_protect_sched();
        if (vsf_slist_queue_is_empty(&ecm->iocb_queue)) {
            vsf_unprotect_sched(orig);

            __vk_usbh_ecm_netdrv_evthandler(ecm, VSF_USBH_CDCECM_ON_DISCONNECT, NULL);
            ecm->thread = NULL;
            break;
        } else {
            vsf_slist_queue_dequeue(vk_usbh_ecm_iocb_t, iocb_node, &ecm->iocb_queue, iocb);
            vsf_unprotect_sched(orig);

            __vk_usbh_ecm_netdrv_evthandler(ecm, VSF_USBH_CDCECM_ON_RX, iocb);
        }
    }
}
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

static vsf_err_t __vk_usbh_ecm_on_cdc_evt(vk_usbh_cdc_t *cdc, vk_usbh_cdc_evt_t evt, void *param)
{
    // cast increases requiredalignment of target type in GCC
    vk_usbh_ecm_t *ecm = (vk_usbh_ecm_t *)cdc;
    vk_netdrv_t *netdrv = &ecm->netdrv;

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
                bool connected = vk_netdrv_is_connected(netdrv);
                if (connected && ecm->evt[2] == 0) {
                    vsf_trace_info("ecm_event: NETWORK_CONNECTION Disconnected" VSF_TRACE_CFG_LINEEND);

#if VSF_USBH_CDCECM_SUPPORT_THREAD == ENABLED
                    if (vk_netdrv_feature(netdrv) & VSF_NETDRV_FEATURE_THREAD) {
                        VSF_USB_ASSERT(ecm->thread != NULL);
                        vsf_eda_sem_post(&ecm->sem);
                    } else
#endif
                        __vk_usbh_ecm_netdrv_evthandler(ecm, VSF_USBH_CDCECM_ON_CONNECT, NULL);
                } else if (!connected && (ecm->evt[2] != 0)) {
                    vsf_trace_info("ecm_event: NETWORK_CONNECTION Connected" VSF_TRACE_CFG_LINEEND);
                    vk_netdrv_prepare(netdrv);

#if VSF_USBH_CDCECM_SUPPORT_THREAD == ENABLED
                    if (vk_netdrv_feature(netdrv) & VSF_NETDRV_FEATURE_THREAD) {
                        VSF_USB_ASSERT(NULL == ecm->thread);
                        ecm->thread = vk_netdrv_thread(netdrv, __vk_usbh_ecm_netdrv_thread, ecm);
                        VSF_USB_ASSERT(ecm->thread != NULL);
                    } else
#endif
                        __vk_usbh_ecm_netdrv_evthandler(ecm, VSF_USBH_CDCECM_ON_CONNECT, NULL);
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
            vk_usbh_ecm_icb_t *icb = __vk_usbh_ecm_get_icb(ecm, (vk_usbh_urb_t *)param);
            int_fast32_t size;

            if (URB_OK != vk_usbh_urb_get_status(&icb->urb)) {
                size = -1;
            } else {
                size = vk_usbh_urb_get_actual_length(&icb->urb);
            }

            if (size > 0) {
                vsf_mem_t mem;

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
                    cur_size = vsf_min(mem.size, remain);
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

            icb->size = size;
#if VSF_USBH_CDCECM_SUPPORT_THREAD == ENABLED
            if (vk_netdrv_feature(netdrv) & VSF_NETDRV_FEATURE_THREAD) {
                VSF_USB_ASSERT(ecm->thread != NULL);
                vsf_protect_t orig = vsf_protect_sched();
                    vsf_slist_queue_enqueue(vk_usbh_ecm_iocb_t, iocb_node, &ecm->iocb_queue, (vk_usbh_ecm_iocb_t *)icb);
                vsf_unprotect_sched(orig);
                vsf_eda_sem_post(&ecm->sem);
            } else
#endif
                __vk_usbh_ecm_netdrv_evthandler(ecm, VSF_USBH_CDCECM_ON_RX, icb);
        }
        break;
    case VSF_USBH_CDC_ON_TX: {
            vk_usbh_ecm_ocb_t *ocb = __vk_usbh_ecm_get_ocb(ecm, (vk_usbh_urb_t *)param);
            vsf_err_t err = (URB_OK != vk_usbh_urb_get_status(&ocb->urb)) ? VSF_ERR_FAIL : VSF_ERR_NONE;

            ocb->netbuf = NULL;
            vk_netdrv_on_netlink_outputted(netdrv, err);
        }
        break;
    }
    return VSF_ERR_NONE;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

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

static vsf_err_t __vk_usbh_ecm_set_filter(vk_usbh_ecm_t *ecm, uint_fast16_t filter)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
        .bRequest        =  0x43,       // SET_ETHERNET_PACKET_FILTER
        .wValue          =  filter,
        .wIndex          =  ecm->ifs->no,
        .wLength         =  0,
    };
    return vk_usbh_control_msg(ecm->usbh, ecm->dev, &req);
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
            err = VSF_ERR_FAIL;
            break;
        }

        err = vk_usbh_get_descriptor(usbh, dev, USB_DT_STRING, ecm->iMAC, VSF_USBH_ECM_MAC_STRING_SIZE);
        break;
    case VSF_EVT_MESSAGE:
        if (vk_usbh_urb_get_status(urb) != URB_OK) {
            err = VSF_ERR_FAIL;
            break;
        }

        switch (ecm->init_state) {
        case VSF_USBH_ECM_INIT_WAIT_MAC:
            if (vk_usbh_urb_get_actual_length(urb) != VSF_USBH_ECM_MAC_STRING_SIZE) {
                err = VSF_ERR_FAIL;
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

            err = vk_usbh_set_interface(usbh, dev, cdc->data_ifs, 1);
            break;
        case VSF_USBH_ECM_INIT_WAIT_SET_IF1:
            err = __vk_usbh_ecm_set_filter(ecm, 0x0E);
            break;
        case VSF_USBH_ECM_INIT_WAIT_SET_FILTER:
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
#if VSF_USBH_CDCECM_SUPPORT_THREAD == ENABLED
        if (vk_netdrv_feature(netdrv) & VSF_NETDRV_FEATURE_THREAD) {
            VSF_USB_ASSERT(ecm->thread != NULL);
            vsf_eda_sem_post(&ecm->sem);
        } else
#endif
            __vk_usbh_ecm_netdrv_evthandler(ecm, VSF_USBH_CDCECM_ON_DISCONNECT, NULL);
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
#ifdef VSF_USBH_CDCECM_CFG_PRIORITY
            vsf_eda_init(&cdc->eda, VSF_USBH_CDCECM_CFG_PRIORITY);
#else
            vsf_eda_init(&cdc->eda);
#endif
#if VSF_KERNEL_CFG_TRACE == ENABLED
            vsf_kernel_trace_eda_info(&cdc->eda, "usbh_ecm_task", NULL, 0);
#endif
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
