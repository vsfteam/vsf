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

#ifndef __VSF_USBH_WIFI_RT2X00_PRIV_H__
#define __VSF_USBH_WIFI_RT2X00_PRIV_H__

/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"
#include "../../vsf_usbh.h"
#include "./vsf_usbh_wifi_rt2x00.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_RT2X00 == ENABLED && VSF_USE_WIFI == ENABLED

#include "component/wifi/vsf_wifi_priv.h"

/*============================ MACROS ========================================*/

#ifndef VSF_USBH_WIFI_CFG_RX_NUM
#   define VSF_USBH_WIFI_CFG_RX_NUM           4
#endif
#ifndef VSF_USBH_WIFI_CFG_TX_NUM
#   define VSF_USBH_WIFI_CFG_TX_NUM           4
#endif
#ifndef VSF_USBH_WIFI_CFG_URB_BUFSIZE
#   define VSF_USBH_WIFI_CFG_URB_BUFSIZE      4096
#endif
#ifndef VSF_USBH_WIFI_CFG_TX_USE_LOCAL_BUFFER
#   define VSF_USBH_WIFI_CFG_TX_USE_LOCAL_BUFFER    ENABLED
#endif

/*
 * Per-chunk size for firmware blob upload via reg_block_write.
 * Each chunk becomes one ep0 vendor 0x01 OUT transfer.
 */
#ifndef VSF_USBH_WIFI_CFG_FW_CHUNK_SIZE
#   define VSF_USBH_WIFI_CFG_FW_CHUNK_SIZE    64
#endif

/*============================ TYPES =========================================*/

typedef struct vk_usbh_wifi_iocb_t {
    vk_usbh_urb_t urb;
    uint8_t is_rx        : 1;
    uint8_t is_supported : 1;
    uint8_t is_busy      : 1;
} vk_usbh_wifi_iocb_t;

/*
 * Bus operation in-flight state.  At most one reg_bus call is active at a
 * time (the generic layer's script_busy flag guarantees serialisation).
 */
typedef enum {
    VK_USBH_WIFI_BUS_IDLE = 0,
    VK_USBH_WIFI_BUS_REG_WRITE,
    VK_USBH_WIFI_BUS_REG_READ,
    VK_USBH_WIFI_BUS_BLOCK_WRITE,
    VK_USBH_WIFI_BUS_VENDOR,        /* ep0 vendor request, no data stage */
} vk_usbh_wifi_bus_state_t;

struct vk_usbh_wifi_t {
    /* Generic wifi state (embedded; use vk_usbh_wifi_get_wifi to expose). */
    vsf_wifi_t           wifi;

    /* USB host bindings */
    vk_usbh_t           *usbh;
    vk_usbh_dev_t       *dev;
    vk_usbh_ifs_t       *ifs;
    vk_usbh_dev_id_t     id;
    vsf_eda_t            eda;

    /* Bus-op in-flight state */
    vk_usbh_wifi_bus_state_t bus_state;
    vsf_wifi_done_t      bus_done;      /* completion callback */
    uint32_t            *bus_read_out;   /* output pointer for reg_read */
    bool                 bus_pending_first; /* waiting for ep0 crit */

    /* Block-write multi-chunk state */
    const uint8_t       *bus_block_data;
    uint32_t             bus_block_len;
    uint32_t             bus_block_offset;
    uint16_t             bus_block_base;
    uint16_t             bus_block_chunk;

    /* Vendor-request (USB_DEVICE_MODE etc.) state: bRequest/wValue/wIndex,
     * no data stage (wLength=0). */
    uint8_t              bus_vendor_request;
    uint16_t             bus_vendor_value;
    uint16_t             bus_vendor_index;

    /* Bulk RX/TX iocb pool */
    union {
        struct {
            vk_usbh_wifi_iocb_t rx_icb[VSF_USBH_WIFI_CFG_RX_NUM];
            vk_usbh_wifi_iocb_t tx_ocb[VSF_USBH_WIFI_CFG_TX_NUM];
        };
        vk_usbh_wifi_iocb_t iocb[VSF_USBH_WIFI_CFG_RX_NUM
                               + VSF_USBH_WIFI_CFG_TX_NUM];
    };
};

/*============================ HELPER MACRO ==================================*/

#define __this_wifi(__eda)   vsf_container_of(__eda, vk_usbh_wifi_t, eda)

#endif // VSF_USE_USB_HOST && VSF_USBH_USE_RT2X00 && VSF_USE_WIFI
#endif // __VSF_USBH_WIFI_RT2X00_PRIV_H__
