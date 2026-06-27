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

#ifndef __VSF_USBH_WIFI_MT76_PRIV_H__
#define __VSF_USBH_WIFI_MT76_PRIV_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_usbh_wifi_mt76.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_MT76 == ENABLED && VSF_USE_WIFI == ENABLED

#include "component/wifi/vsf_wifi_priv.h"
#include "component/wifi/chip/mt76/vsf_wifi_mt76.h"

/*============================ MACROS ========================================*/

#define MT76_EP_IN_PKT_RX       0
#define MT76_EP_IN_CMD_RESP     1
#define MT76_EP_IN_MAX          2

#define MT76_EP_OUT_INBAND_CMD  0
#define MT76_EP_OUT_AC_BE       1
#define MT76_EP_OUT_AC_BK       2
#define MT76_EP_OUT_AC_VI       3
#define MT76_EP_OUT_AC_VO       4
#define MT76_EP_OUT_HCCA        5
#define MT76_EP_OUT_MAX         6

#ifndef VSF_USBH_WIFI_MT76_CFG_RX_NUM
#   define VSF_USBH_WIFI_MT76_CFG_RX_NUM        4
#endif
#ifndef VSF_USBH_WIFI_MT76_CFG_TX_NUM
#   define VSF_USBH_WIFI_MT76_CFG_TX_NUM        4
#endif
#ifndef VSF_USBH_WIFI_MT76_CFG_RX_BUFSIZE
#   define VSF_USBH_WIFI_MT76_CFG_RX_BUFSIZE    4096
#endif
#ifndef VSF_USBH_WIFI_MT76_CFG_TX_BUFSIZE
#   define VSF_USBH_WIFI_MT76_CFG_TX_BUFSIZE    4096
#endif

/*============================ TYPES =========================================*/

typedef enum {
    MT76_EP0_IDLE = 0,
    MT76_EP0_MCU_CMD,
    MT76_EP0_FCE_LO,
    MT76_EP0_FCE_HI,
} vk_usbh_wifi_mt76_ep0_state_t;

typedef enum {
    MT76_EP0_REQ_READ,
    MT76_EP0_REQ_WRITE,
    MT76_EP0_REQ_FCE_WRITE,
    MT76_EP0_REQ_DEV_CMD,
    MT76_EP0_REQ_DEV_CLASS_CMD,
} vk_usbh_wifi_mt76_ep0_req_type_t;

#define MT76_EP0_CLASS_DATA_SIZE    64
#define MT76_EP0_QUEUE_SIZE         8

typedef struct vk_usbh_wifi_mt76_ep0_req_t {
    vk_usbh_wifi_mt76_ep0_req_type_t type;
    struct usb_ctrlrequest_t    req;
    uint32_t                    fce_addr;
    uint32_t                    fce_val;
    void                        *buf;       /* caller buffer for IN/OUT data */
    uint8_t                     class_data[MT76_EP0_CLASS_DATA_SIZE];
    uint16_t                    class_len;
    vsf_wifi_done_t             done;
} vk_usbh_wifi_mt76_ep0_req_t;

typedef struct vk_usbh_wifi_mt76_ep0_queue_t {
    vk_usbh_wifi_mt76_ep0_req_t reqs[MT76_EP0_QUEUE_SIZE];
    uint8_t                     head;
    uint8_t                     tail;
    uint8_t                     count;
} vk_usbh_wifi_mt76_ep0_queue_t;

typedef struct vk_usbh_wifi_mt76_ep_t {
    uint8_t                     addr;
    struct usb_endpoint_desc_t *desc;
} vk_usbh_wifi_mt76_ep_t;

typedef struct vk_usbh_wifi_mt76_iocb_t {
    vk_usbh_urb_t               urb;
    uint8_t                     is_rx        : 1;
    uint8_t                     is_supported : 1;
    uint8_t                     is_busy      : 1;
    uint8_t                     ep_idx       : 4;
    vsf_wifi_done_t             done;       /* completion callback */
} vk_usbh_wifi_mt76_iocb_t;

typedef struct vk_usbh_wifi_mt76_t {
    vk_usbh_t                   *usbh;
    vk_usbh_dev_t               *dev;
    vk_usbh_ifs_t               *ifs;

    vk_usbh_wifi_mt76_ep_t      in_ep[MT76_EP_IN_MAX];
    vk_usbh_wifi_mt76_ep_t      out_ep[MT76_EP_OUT_MAX];

    vsf_eda_t                   eda;
    vsf_wifi_t                  wifi;

    /* chip driver private data follows wifi */
    mt76_wifi_priv_t            mt76_priv;

    /* ep0 control-transfer state.  All EP0 requests go through ep0_queue;
     * only one request is in flight at a time, and the dispatcher starts the
     * next one after the current completes.  This removes the old requirement
     * that callers wait for ep0_state == IDLE before submitting. */
    vk_usbh_wifi_mt76_ep0_state_t ep0_state;
    vsf_wifi_done_t             ep0_done;
    bool                        ep0_busy;
    bool                        ep0_crit_pend;  /* waiting for ep0.crit */
    vk_usbh_wifi_mt76_ep0_queue_t ep0_queue;
    struct usb_ctrlrequest_t    ep0_req;
    void                        *ep0_buf;   /* caller buffer for IN/OUT data */
    uint32_t                    ep0_fce_addr;
    uint32_t                    ep0_fce_val;

    /* Bulk RX/TX iocb pool */
    vk_usbh_wifi_mt76_iocb_t    rx_pkt_iocb[VSF_USBH_WIFI_MT76_CFG_RX_NUM];
    vk_usbh_wifi_mt76_iocb_t    rx_cmd_iocb;
    vk_usbh_wifi_mt76_iocb_t    tx_iocb[VSF_USBH_WIFI_MT76_CFG_TX_NUM];
    vk_usbh_wifi_mt76_iocb_t    mcu_cmd_iocb;
} vk_usbh_wifi_mt76_t;

/*============================ HELPER MACRO ==================================*/

#define __this_uwifi(__eda)   vsf_container_of(__eda, vk_usbh_wifi_mt76_t, eda)

#endif      /* VSF_USE_USB_HOST && VSF_USBH_USE_MT76 && VSF_USE_WIFI */
#endif      /* __VSF_USBH_WIFI_MT76_PRIV_H__ */
