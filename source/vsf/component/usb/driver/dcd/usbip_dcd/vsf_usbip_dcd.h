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

#ifndef __VSF_USBIP_DCD_H__
#define __VSF_USBIP_DCD_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_USBIP == ENABLED

#include "hal/vsf_hal.h"
#include "component/usb/common/usb_common.h"

#if     defined(__VSF_USBIP_DCD_CLASS_IMPLEMENT)
#   undef __VSF_USBIP_DCD_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_USBIP_DCD_CFG_BACKEND_WIN   1
#define VSF_USBIP_DCD_CFG_BACKEND_VSF   2
#define VSF_USBIP_DCD_CFG_BACKEND_LIB   3

#ifndef VSF_USBIP_DCD_CFG_BACKEND
#   define VSF_USBIP_DCD_CFG_BACKEND    VSF_USBIP_DCD_CFG_BACKEND_WIN
#endif

#ifndef VSF_USBIP_DCD_CFG_PATH
#   define VSF_USBIP_DCD_CFG_PATH       "/vsf/usbd/0"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_usb_dc_from_usbip_ip(__n, __obj, __drv_name)                        \
        __USB_DC_FROM_IP(__n, (__obj), __drv_name, vk_usbip_usbd)

/*============================ TYPES =========================================*/

dcl_simple_class(vk_usbip_dcd_t)

typedef struct vk_usbip_dcd_param_t {
    uint16_t port;
} vk_usbip_dcd_param_t;

typedef struct vk_usbip_dcd_ep_t {
    uint16_t size;
    usb_ep_type_t type;
    bool is_stalled;
    uint8_t ep;

    vsf_dlist_t urb_list;
    struct {
        vsf_mem_t mem;
        uint32_t size;
        bool zlp;
        bool pending;
    } transfer;
} vk_usbip_dcd_ep_t;

def_simple_class(vk_usbip_dcd_t) {
    public_member(
        vk_usbip_dcd_param_t const * const param;
    )

    private_member(
        struct {
            usb_dc_evt_handler_t evt_handler;
            void *param;
        } callback;

        union {
            struct {
                vk_usbip_dcd_ep_t ep_out[16];
                vk_usbip_dcd_ep_t ep_in[16];
            };
            vk_usbip_dcd_ep_t ep[32];
        };
        struct usb_ctrlrequest_t setup;
        uint16_t frame_number;
        uint16_t mframe_number;
        uint8_t address;
        bool is_connected;
    )
};

#ifdef __VSF_USBIP_DCD_CLASS_IMPLEMENT_BACKEND__
#   undef __VSF_USBIP_DCD_CLASS_IMPLEMENT_BACKEND__

typedef struct vk_usbip_rep_ifs_t {
    uint8_t     bInterfaceClass;
    uint8_t     bInterfaceSubClass;
    uint8_t     bInterfaceProtocol;
    uint8_t     padding;
} PACKED vk_usbip_rep_ifs_t;

typedef struct vk_usbip_rep_dev_t {
    uint32_t    busnum;
    uint32_t    devnum;
    uint32_t    speed;
    uint16_t    idVendor;
    uint16_t    idProduct;
    uint16_t    bcdDevice;
    uint8_t     bDeviceClass;
    uint8_t     bDeviceSubClass;
    uint8_t     bDeviceProtocol;
    uint8_t     bConfigurationValue;
    uint8_t     bNumConfigurations;
    uint8_t     bNumInterfaces;
    vk_usbip_rep_ifs_t ifs[16];
} PACKED vk_usbip_rep_dev_t;

typedef enum vk_usbip_cmd_t {
    USBIP_CMD_INVALID   = 0x00000000,
    USBIP_CMD_SUBMIT    = 0x00000001,
    USBIP_CMD_UNLINK    = 0x00000002,
    USBIP_RET_SUBMIT    = 0x00000003,
    USBIP_RET_UNLINK    = 0x00000004,

    USBIP_REQ_DEVLIST   = 0x8005,
    USBIP_REP_DEVLIST   = 0x8005,
    USBIP_REQ_IMPORT    = 0x8003,
    USBIP_REP_IMPORT    = 0x8003,
} vk_usbip_cmd_t;

typedef struct vk_usbip_op_common_t {
    uint16_t    version_bcd;
    uint16_t    code;
    uint32_t    status;
} PACKED vk_usbip_op_common_t;

// OP_REQ_DEVLIST
typedef struct vk_usbip_req_devlist_t {
    implement(vk_usbip_op_common_t)
} PACKED vk_usbip_req_devlist_t;

// OP_REP_DEVLIST
//  vk_usbip_rep_devlist_t
//  vk_usbip_rep_dev_t[devnum]
//  vk_usbip_rep_devlist_ifs_t[bNumInterfaces]
typedef struct vk_usbip_rep_devlist_t {
    implement(vk_usbip_op_common_t)
    uint32_t    devnum;     // number of devices
} PACKED vk_usbip_rep_devlist_t;

// OP_REQ_IMPORT
typedef struct vk_usbip_req_import_t {
    implement(vk_usbip_op_common_t)
    char        busid[32];
} PACKED vk_usbip_req_import_t;

// OP_REP_IMPORT
//  vk_usbip_rep_import_t
//  vk_usbip_rep_dev_t
typedef struct vk_usbip_rep_import_t {
    implement(vk_usbip_op_common_t)
} PACKED vk_usbip_rep_import_t;

// USBIP_CMD_SUBMIT
//  vk_usbip_req_submit_t
//  data[transfer_length]
typedef struct vk_usbip_req_submit_t {
    uint32_t    command;
    uint32_t    seqnum;
    uint32_t    devid;
    uint32_t    direction;          // 0: OUT, 1: IN
    uint32_t    ep;
    uint32_t    transfer_flags;
    uint32_t    transfer_length;
    uint32_t    start_frame;        // for ISO
    uint32_t    number_of_packets;  // for ISO
    uint32_t    interval;
    struct usb_ctrlrequest_t setup;
} PACKED vk_usbip_req_submit_t;

// USBIP_RET_SUBMIT
//  vk_usbip_rep_submit_t
//  data[actual_length]
typedef struct vk_usbip_rep_submit_t {
    uint32_t    command;
    uint32_t    seqnum;
    uint32_t    devid;
    uint32_t    direction;
    uint32_t    ep;
    uint32_t    status;
    uint32_t    actual_length;
    uint32_t    start_frame;        // for ISO
    uint32_t    number_of_packets;  // for ISO
    uint32_t    error_count;
} PACKED vk_usbip_rep_submit_t;

// USBIP_CMD_UNLINK
//  vk_usbip_req_unlink_t
//  data[unknown]
typedef struct vk_usbip_req_unlink_t {
    uint32_t    command;
    uint32_t    seqnum;
    uint32_t    devid;
    uint32_t    direction;          // 0: OUT, 1: IN
    uint32_t    ep;
    uint32_t    seqnum_to_unlink;
} PACKED vk_usbip_req_unlink_t;

// USBIP_RET_UNLINK
//  vk_usbip_rep_unlink_t
//  data[unknown]
typedef struct vk_usbip_rep_unlink_t {
    uint32_t    command;
    uint32_t    seqnum;
    uint32_t    devid;
    uint32_t    direction;          // 0: OUT, 1: IN
    uint32_t    ep;
    uint32_t    status;
} PACKED vk_usbip_rep_unlink_t;

typedef struct vk_usbip_urb_t {
    vsf_dlist_node_t urb_node;
    vsf_dlist_node_t urb_node_ep;
    vk_usbip_req_submit_t req;
    union {
        vk_usbip_rep_submit_t rep;
        vk_usbip_rep_unlink_t unlink;
    };
    vsf_mem_t mem;
    vsf_mem_t dynmem;

    bool is_unlinked;
    enum {
        VSF_USBIP_URB_IDLE = 0,
        VSF_USBIP_URB_QUEUED,
        VSF_USBIP_URB_COMITTED,
        VSF_USBIP_URB_DONE,
    } state;
} vk_usbip_urb_t;

dcl_vsf_pool(vk_usbip_urb_poll)
def_vsf_pool(vk_usbip_urb_poll, vk_usbip_urb_t)

typedef struct vk_usbip_server_t {
    vk_usbip_dcd_t *usbd;
    vsf_teda_t teda;
    uint16_t port;
    vsf_err_t err;

    enum {
        VSF_USBIP_SERVER_REQ_COMMAND,
        VSF_USBIP_SERVER_REQ_PARAM,
        VSF_USBIP_SERVER_REQ_DATA,
    } req_state;
    vk_usbip_cmd_t command;

    enum {
        VSF_USBIP_SERVER_REP_DONE           = 0,
        VSF_USBIP_SERVER_REP_REPLY          = 1,
        VSF_USBIP_SERVER_REP_PARAM          = 2,
        VSF_USBIP_SERVER_REP_DATA           = 3,

        // for devlist
        VSF_USBIP_SERVER_REP_DEVLIST_PATH   = 2,
        VSF_USBIP_SERVER_REP_DEVLIST_BUSID  = 3,
        VSF_USBIP_SERVER_REP_DEVLIST_DEV    = 4,
        VSF_USBIP_SERVER_REP_DEVLIST_CFG    = 5,

        // for import
        VSF_USBIP_SERVER_REP_IMPORT_PATH    = VSF_USBIP_SERVER_REP_DEVLIST_PATH,
        VSF_USBIP_SERVER_REP_IMPORT_BUSID   = VSF_USBIP_SERVER_REP_DEVLIST_BUSID,
        VSF_USBIP_SERVER_REP_IMPORT_DEV     = VSF_USBIP_SERVER_REP_DEVLIST_DEV,
    } rep_state;
    vk_usbip_cmd_t reply;

    bool is_connected;
    union {
        uint32_t cmd32;
        struct {
            uint16_t version_bcd;
            uint16_t cmd16;
        } PACKED;
        vk_usbip_req_devlist_t devlist;
        vk_usbip_req_import_t import;
        vk_usbip_req_submit_t submit;
        vk_usbip_req_unlink_t unlink;
    } req;

    union {
        vk_usbip_op_common_t common;
        vk_usbip_rep_devlist_t devlist;
        vk_usbip_rep_import_t import;
        vk_usbip_rep_unlink_t unlink;
        uint8_t buffer[256];        // for path
    } rep;

    vk_usbip_rep_dev_t dev;
    vsf_pool(vk_usbip_urb_poll) urb_pool;
    vsf_dlist_t urb_list;
    vsf_dlist_t urb_done_list;
    vsf_dlist_t urb_free_list;
    vk_usbip_urb_t *cur_urb;
} vk_usbip_server_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_usbip_usbd_init(vk_usbip_dcd_t *usbd, usb_dc_cfg_t *cfg);
extern void vk_usbip_usbd_fini(vk_usbip_dcd_t *usbd);
extern void vk_usbip_usbd_reset(vk_usbip_dcd_t *usbd, usb_dc_cfg_t *cfg);

extern void vk_usbip_usbd_connect(vk_usbip_dcd_t *usbd);
extern void vk_usbip_usbd_disconnect(vk_usbip_dcd_t *usbd);
extern void vk_usbip_usbd_wakeup(vk_usbip_dcd_t *usbd);

extern void vk_usbip_usbd_set_address(vk_usbip_dcd_t *usbd, uint_fast8_t addr);
extern uint_fast8_t vk_usbip_usbd_get_address(vk_usbip_dcd_t *usbd);

extern uint_fast16_t vk_usbip_usbd_get_frame_number(vk_usbip_dcd_t *usbd);
extern uint_fast8_t vk_usbip_usbd_get_mframe_number(vk_usbip_dcd_t *usbd);

extern void vk_usbip_usbd_get_setup(vk_usbip_dcd_t *usbd, uint8_t *buffer);
extern void vk_usbip_usbd_status_stage(vk_usbip_dcd_t *usbd, bool is_in);

extern uint_fast8_t vk_usbip_usbd_ep_get_feature(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint_fast8_t feature);
extern vsf_err_t vk_usbip_usbd_ep_add(vk_usbip_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vk_usbip_usbd_ep_get_size(vk_usbip_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vk_usbip_usbd_ep_set_stall(vk_usbip_dcd_t *usbd, uint_fast8_t ep);
extern bool vk_usbip_usbd_ep_is_stalled(vk_usbip_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vk_usbip_usbd_ep_clear_stall(vk_usbip_dcd_t *usbd, uint_fast8_t ep);

extern uint_fast32_t vk_usbip_usbd_ep_get_data_size(vk_usbip_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vk_usbip_usbd_ep_transaction_read_buffer(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vk_usbip_usbd_ep_transaction_enable_out(vk_usbip_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vk_usbip_usbd_ep_transaction_set_data_size(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vk_usbip_usbd_ep_transaction_write_buffer(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t vk_usbip_usbd_ep_transfer_recv(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t vk_usbip_usbd_ep_transfer_send(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);

extern void vk_usbip_usbd_irq(vk_usbip_dcd_t *usbd);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
