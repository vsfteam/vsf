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

#ifndef __VSF_USBH_H__
#define __VSF_USBH_H__

/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED

#include "../common/usb_common.h"
#include "../common/usb_ch11.h"

#if     defined(VSF_USBH_IMPLEMENT)
#   define VSF_USBH_IMPLEMENT_vsf_usbh_hcd_t
#   define VSF_USBH_IMPLEMENT_vsf_usbh_hcd_urb_t
#   define VSF_USBH_IMPLEMENT_vsf_usbh_urb_t
#   define VSF_USBH_IMPLEMENT_vsf_usbh_dev_t
#   define VSF_USBH_IMPLEMENT_vsf_usbh_t
#elif   defined(VSF_USBH_IMPLEMENT_HCD)
#   define VSF_USBH_IMPLEMENT_vsf_usbh_hcd_t
#   define VSF_USBH_IMPLEMENT_vsf_usbh_hcd_urb_t
#   define VSF_USBH_IMPLEMENT_vsf_usbh_dev_t
#elif   defined(VSF_USBH_IMPLEMENT_CLASS)
#   define VSF_USBH_IMPLEMENT_vsf_usbh_dev_t
#   define VSF_USBH_INHERIT_vsf_usbh_t
#   define VSF_USBH_INHERIT_vsf_usbh_urb_t
#elif   defined(VSF_USBH_IMPLEMENT_HUB)
#   define VSF_USBH_IMPLEMENT_CLASS
#   define VSF_USBH_IMPLEMENT_vsf_usbh_dev_t
#   define VSF_USBH_INHERIT_vsf_usbh_t
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#ifdef VSF_USB_HC_ISO_EN
#ifndef VSF_USB_HC_ISO_PACKET_LIMIT
#define VSF_USB_HC_ISO_PACKET_LIMIT     2
#endif
#endif

#ifndef VSF_USBH_CFG_ENABLE_ROOT_HUB
#   define VSF_USBH_CFG_ENABLE_ROOT_HUB ENABLED
#endif

#ifndef VSF_USBH_CFG_MAX_DEVICE
#   define VSF_USBH_CFG_MAX_DEVICE      127
#endif

#ifdef VSF_USBH_CFG_HEAP
#   undef VSF_USBH_MALLOC
#   undef VSF_USBH_MALLOC_ALIGNED
#   undef VSF_USBH_FREE
#   define VSF_USBH_MALLOC              VSF_USBH_CFG_HEAP.Malloc
#   define VSF_USBH_MALLOC_ALIGNED      VSF_USBH_CFG_HEAP.MallocAligned
#   define VSF_USBH_FREE                VSF_USBH_CFG_HEAP.Free

#else
#   ifndef VSF_USBH_MALLOC
#       define VSF_USBH_MALLOC         vsf_heap_malloc
#   endif
#   ifndef VSF_USBH_MALLOC_ALIGNED
#       define VSF_USBH_MALLOC_ALIGNED vsf_heap_malloc_aligned
#   endif
#   ifndef VSF_USBH_FREE
#       define VSF_USBH_FREE           vsf_heap_free
#   endif

#endif



#define URB_OK                  VSF_ERR_NONE
#define URB_FAIL                VSF_ERR_FAIL
#define URB_PENDING             VSF_ERR_NOT_READY

// struct vsf_hcd_urb_t.transfer_flags
#define URB_SHORT_NOT_OK        0x01
#define URB_ISO_ASAP            0x02
#define URB_ZERO_PACKET         0x40

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_usbh_hcd_t)
declare_simple_class(vsf_usbh_hcd_drv_t)
declare_simple_class(vsf_usbh_hcd_urb_t)
declare_simple_class(vsf_usbh_hcd_dev_t)

declare_simple_class(vsf_usbh_urb_t)
declare_simple_class(vsf_usbh_dev_t)
declare_simple_class(vsf_usbh_t)

typedef struct vsf_usbh_class_drv_t vsf_usbh_class_drv_t;

struct vsf_usbh_ifs_t {
    const vsf_usbh_class_drv_t *drv;
    void *param;

    uint8_t no;
    uint8_t num_of_alt;
    uint8_t cur_alt;
};
typedef struct vsf_usbh_ifs_t vsf_usbh_ifs_t;

struct vsf_usbh_ifs_alt_parser_t {
    struct usb_interface_desc_t *desc_ifs;
    struct usb_endpoint_desc_t *desc_ep;
    uint16_t desc_size;
    uint8_t num_of_ep;
};
typedef struct vsf_usbh_ifs_alt_parser_t vsf_usbh_ifs_alt_parser_t;

struct vsf_usbh_ifs_parser_t {
    vsf_usbh_ifs_alt_parser_t *parser_alt;
    vsf_usbh_ifs_t *ifs;
};
typedef struct vsf_usbh_ifs_parser_t vsf_usbh_ifs_parser_t;

struct vsf_usbh_dev_parser_t {
    struct usb_device_descriptor_t *desc_device;
    struct usb_config_descriptor_t *desc_config;
    vsf_usbh_ifs_parser_t *parser_ifs;
    uint8_t num_of_ifs;
    uint8_t devnum_temp;
    enum {
        VSF_USBH_PROBE_START,
        VSF_USBH_PROBE_WAIT_DEVICE_DESC,
        VSF_USBH_PROBE_WAIT_DEVICE_RESET = VSF_USBH_PROBE_WAIT_DEVICE_DESC,
        VSF_USBH_PROBE_WAIT_SET_ADDRESS,
        VSF_USBH_PROBE_WAIT_ADDRESS_STABLE,
        VSF_USBH_PROBE_WAIT_FULL_DEVICE_DESC,
        VSF_USBH_PROBE_WAIT_CONFIG_DESC_SIZE,
        VSF_USBH_PROBE_WAIT_FULL_CONFIG_DESC,
        VSF_USBH_PROBE_WAIT_SET_CONFIG,
    } probe_state;
};
typedef struct vsf_usbh_dev_parser_t vsf_usbh_dev_parser_t;

struct vsf_usbh_dev_id_t {
    uint16_t match_vendor       : 1;
    uint16_t match_product      : 1;
    uint16_t match_dev_lo       : 1;
    uint16_t match_dev_hi       : 1;
    uint16_t match_dev_class    : 1;
    uint16_t match_dev_subclass : 1;
    uint16_t match_dev_protocol : 1;
    uint16_t match_int_class    : 1;
    uint16_t match_int_subclass : 1;
    uint16_t match_int_protocol : 1;
    uint16_t                    : 6;

    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice_lo, bcdDevice_hi;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    //uint32_t driver_info;
};
typedef struct vsf_usbh_dev_id_t vsf_usbh_dev_id_t;

struct vsf_usbh_class_drv_t {
    const char *name;
    uint8_t dev_id_num;
    const vsf_usbh_dev_id_t *dev_ids;
    void * (*probe)(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
                    vsf_usbh_ifs_parser_t *parser_ifs);
    void (*disconnect)(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, void *param);
};

struct vsf_usbh_class_t {
    const vsf_usbh_class_drv_t *drv;
    vsf_slist_node_t node;
};
typedef struct vsf_usbh_class_t vsf_usbh_class_t;

struct vsf_usbh_eppipe_t {
    union {
        struct {
            uint32_t is_pipe        : 1;
            uint32_t is_submitted   : 1;
            uint32_t size           : 10;
            uint32_t endpoint       : 4;
            uint32_t type           : 2;
            uint32_t speed          : 2;
            uint32_t address        : 7;
            uint32_t dir_in1out0    : 1;
        };
        uint32_t value;
    };
};
typedef struct vsf_usbh_eppipe_t vsf_usbh_eppipe_t;

#if     defined(VSF_USBH_IMPLEMENT_vsf_usbh_hcd_t)
#   undef VSF_USBH_IMPLEMENT_vsf_usbh_hcd_t
#   define __PLOOC_CLASS_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

def_simple_class(vsf_usbh_hcd_drv_t) {
    private_member(
        vsf_err_t (*init_evthandler)(vsf_eda_t *eda, vsf_evt_t evt, vsf_usbh_hcd_t *hcd);
        vsf_err_t (*fini)(vsf_usbh_hcd_t *hcd);
        vsf_err_t (*suspend)(vsf_usbh_hcd_t *hcd);
        vsf_err_t (*resume)(vsf_usbh_hcd_t *hcd);
        vsf_err_t (*alloc_device)(vsf_usbh_hcd_t *hcd, vsf_usbh_dev_t *dev);
        void (*free_device)(vsf_usbh_hcd_t *hcd, vsf_usbh_dev_t *dev);
        vsf_usbh_hcd_urb_t * (*alloc_urb)(vsf_usbh_hcd_t *hcd);
        void (*free_urb)(vsf_usbh_hcd_t *hcd, vsf_usbh_hcd_urb_t *urb);
        vsf_err_t (*submit_urb)(vsf_usbh_hcd_t *hcd, vsf_usbh_hcd_urb_t *urb);
        vsf_err_t (*relink_urb)(vsf_usbh_hcd_t *hcd, vsf_usbh_hcd_urb_t *urb);
        int (*rh_control)(vsf_usbh_hcd_t *hcd, vsf_usbh_hcd_urb_t *urb);
    )
};

def_simple_class(vsf_usbh_hcd_t) {
    public_member(
        const vsf_usbh_hcd_drv_t *drv;
        void *param;
        uint8_t rh_speed;
    )

    private_member(
        void *priv;
    )
};

#ifdef VSF_USB_HC_ISO_EN
struct iso_packet_descriptor_t {
    uint32_t offset;                /*!< Start offset in transfer buffer*/
    uint32_t length;                /*!< Length in transfer buffer      */
    uint32_t actual_length;         /*!< Actual transfer length         */
    int32_t status;                 /*!< Transfer status                */
};
typedef struct iso_packet_descriptor_t iso_packet_descriptor_t;

struct iso_packet_t {
    uint32_t start_frame;           /*!< start frame (iso/irq only)     */
    uint32_t number_of_packets;     /*!< number of packets (iso)        */
    //uint32_t error_count;         /*!< number of errors (iso only)    */
    iso_packet_descriptor_t frame_desc[VSF_USB_HC_ISO_PACKET_LIMIT];
};
typedef struct iso_packet_t iso_packet_t;
#endif

#if     defined(VSF_USBH_IMPLEMENT_vsf_usbh_hcd_urb_t)
#   undef VSF_USBH_IMPLEMENT_vsf_usbh_hcd_urb_t
#   define __PLOOC_CLASS_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

def_simple_class(vsf_usbh_hcd_dev_t) {
    private_member(
        union {
            void *dev_priv;
            uint32_t value;
            uint16_t toggle[2];
        };
    )
};

def_simple_class(vsf_usbh_hcd_urb_t) {

    private_member(
        vsf_usbh_hcd_dev_t *dev_hcd;
        vsf_usbh_eppipe_t pipe;

        uint32_t transfer_length;
        uint32_t actual_length;
        uint16_t timeout;
        uint16_t transfer_flags;    /*!< USB_DISABLE_SPD | USB_ISO_ASAP | etc. */
        uint16_t interval;          /*!< polling interval (iso/irq only)    */
        int16_t status;             /*!< returned status                    */

        void *buffer;
        void (*free_buffer)(void *param);
        void *free_buffer_param;

        union {
            struct usb_ctrlrequest_t setup_packet;
#ifdef VSF_USB_HC_ISO_EN
            iso_packet_t iso_packet;
#endif
        };
    )

    private_member(
        vsf_eda_t *eda_caller;
        uint32_t priv[0];
    )
};

#if     defined(VSF_USBH_IMPLEMENT_vsf_usbh_urb_t)
#   undef VSF_USBH_IMPLEMENT_vsf_usbh_urb_t
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_USBH_INHERIT_vsf_usbh_urb_t)
#   undef VSF_USBH_INHERIT_vsf_usbh_urb_t
#   define __PLOOC_CLASS_INHERIT
#endif
#include "utilities/ooc_class.h"
def_simple_class(vsf_usbh_urb_t) {
    protected_member(
        union {
            vsf_usbh_eppipe_t pipe;
            vsf_usbh_hcd_urb_t *urb_hcd;
        };
    )
};

#if     defined(VSF_USBH_IMPLEMENT_vsf_usbh_dev_t)
#   undef VSF_USBH_IMPLEMENT_vsf_usbh_dev_t
#   define __PLOOC_CLASS_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

struct vsf_usbh_ep0_t {
    vsf_crit_t crit;
    vsf_usbh_urb_t urb;
};
typedef struct vsf_usbh_ep0_t vsf_usbh_ep0_t;

def_simple_class(vsf_usbh_dev_t) {

    public_member(
        implement(vsf_usbh_hcd_dev_t)
    )

    protected_member(
        vsf_usbh_ep0_t ep0;
        vsf_usbh_ifs_t *ifs;

        vsf_usbh_dev_t *dev_parent;
        vsf_slist_node_t child_node;
        vsf_slist_t children_list;

        uint8_t devnum;
        uint8_t num_of_ifs;
        uint8_t speed       : 2;
        uint8_t cur_config  : 5;
        uint8_t is_ep0_rdy  : 1;
        uint8_t maxchild    : 4;
        uint8_t index       : 4;
    )
};

#if     defined(VSF_USBH_IMPLEMENT_vsf_usbh_t)
#   undef VSF_USBH_IMPLEMENT_vsf_usbh_t
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_USBH_INHERIT_vsf_usbh_t)
#   undef VSF_USBH_INHERIT_vsf_usbh_t
#   define __PLOOC_CLASS_INHERIT
#endif
#include "utilities/ooc_class.h"

vsf_declare_bitmap(vsf_usbh_devnum_bitmap, VSF_USBH_CFG_MAX_DEVICE + 1);

def_simple_class(vsf_usbh_t) {

    public_member(
        implement(vsf_usbh_hcd_t)
    )

    private_member(
        vsf_bitmap(vsf_usbh_devnum_bitmap) device_bitmap;
#if VSF_USBH_CFG_ENABLE_ROOT_HUB == ENABLED
        vsf_usbh_dev_t *dev_rh;
#endif
        vsf_slist_t class_list;
    )

    protected_member(
        vsf_usbh_dev_t *dev_new;
        vsf_usbh_dev_parser_t *parser;

        vsf_teda_t teda;
    )
};


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if !defined(VSF_USBH_IMPLEMENT_HCD) && !defined(VSF_USBH_IMPLEMENT_CLASS)
// APIs to be called by user
extern vsf_err_t vsf_usbh_init(vsf_usbh_t *usbh);
extern vsf_err_t vsf_usbh_fini(vsf_usbh_t *usbh);
extern vsf_err_t vsf_usbh_register_class_driver(vsf_usbh_t *usbh,
        const vsf_usbh_class_drv_t *drv);
extern void vsf_usbh_register_class(vsf_usbh_t *usbh, vsf_usbh_class_t *class);
#endif

#if defined(VSF_USBH_IMPLEMENT) || defined(VSF_USBH_IMPLEMENT_HCD)
// APIs to be called by hcd drivers
void vsf_usbh_hcd_urb_free_buffer(vsf_usbh_hcd_urb_t *urb_hcd);
#endif

#if defined(VSF_USBH_IMPLEMENT) || defined(VSF_USBH_IMPLEMENT_CLASS)
// APIs to be called by class drivers
extern vsf_usbh_eppipe_t vsf_usbh_get_pipe_from_ep_desc(vsf_usbh_dev_t *dev,
            struct usb_endpoint_desc_t *desc_ep);
extern void vsf_usbh_urb_prepare_by_pipe(vsf_usbh_urb_t *urb, vsf_usbh_dev_t *dev,
            vsf_usbh_eppipe_t pipe);
extern void vsf_usbh_urb_prepare(vsf_usbh_urb_t *urb, vsf_usbh_dev_t *dev,
            struct usb_endpoint_desc_t *desc_ep);
extern bool vsf_usbh_urb_is_valid(vsf_usbh_urb_t *urb);
extern bool vsf_usbh_urb_is_alloced(vsf_usbh_urb_t *urb);
extern vsf_usbh_eppipe_t vsf_usbh_urb_get_pipe(vsf_usbh_urb_t *urb);
extern vsf_err_t vsf_usbh_alloc_urb(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, vsf_usbh_urb_t *urb);
extern void vsf_usbh_free_urb(vsf_usbh_t *usbh, vsf_usbh_urb_t *urb);
extern void * vsf_usbh_urb_alloc_buffer(vsf_usbh_urb_t *urb, uint_fast16_t size);
extern void vsf_usbh_urb_free_buffer(vsf_usbh_urb_t *urb);
extern void * vsf_usbh_urb_take_buffer(vsf_usbh_urb_t *urb);
extern void * vsf_usbh_urb_peek_buffer(vsf_usbh_urb_t *urb);
extern void vsf_usbh_urb_set_buffer(vsf_usbh_urb_t *urb, void *buffer,
            uint_fast32_t size);
extern int_fast16_t vsf_usbh_urb_get_status(vsf_usbh_urb_t *urb);
extern uint_fast32_t vsf_usbh_urb_get_actual_length(vsf_usbh_urb_t *urb);

extern vsf_err_t vsf_usbh_submit_urb(vsf_usbh_t *usbh, vsf_usbh_urb_t *urb);
extern vsf_err_t vsf_usbh_submit_urb_flags(vsf_usbh_t *usbh, vsf_usbh_urb_t *urb, uint_fast16_t flags);
extern vsf_err_t vsf_usbh_submit_urb_ex(vsf_usbh_t *usbh, vsf_usbh_urb_t *urb, uint_fast16_t flags, vsf_eda_t *eda);

#if defined(VSF_USBH_IMPLEMENT_HUB)
extern vsf_usbh_dev_t * vsf_usbh_alloc_device(vsf_usbh_t *usbh);
#endif

extern void vsf_usbh_disconnect_device(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev);
extern void vsf_usbh_remove_interface(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
        vsf_usbh_ifs_t *ifs);

extern vsf_err_t vsf_usbh_control_msg(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
        struct usb_ctrlrequest_t *req);
extern vsf_err_t vsf_usbh_control_msg_ex(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
        struct usb_ctrlrequest_t *req, vsf_eda_t *eda);

extern vsf_err_t vsf_usbh_get_descriptor(vsf_usbh_t *usbh,
        vsf_usbh_dev_t *dev, uint_fast8_t type, uint_fast8_t index,
        uint_fast16_t size);
extern vsf_err_t vsf_usbh_get_class_descriptor(vsf_usbh_t *usbh,
        vsf_usbh_dev_t *dev, uint_fast16_t ifnum, uint_fast8_t type,
        uint_fast8_t id, uint_fast16_t size);
extern vsf_err_t vsf_usbh_set_Zuration(vsf_usbh_t *usbh,
        vsf_usbh_dev_t *dev, uint_fast8_t configuration);
extern vsf_err_t vsf_usbh_set_interface(vsf_usbh_t *usbh,
        vsf_usbh_dev_t *dev, uint_fast8_t ifnum, uint_fast8_t alternate);

extern vsf_err_t vsf_usbh_get_extra_descriptor(uint8_t *buf, uint_fast16_t size,
        uint_fast8_t type, void **ptr);
#endif

#undef VSF_USBH_IMPLEMENT
#undef VSF_USBH_IMPLEMENT_CLASS
#undef VSF_USBH_IMPLEMENT_HCD
#undef VSF_USBH_IMPLEMENT_HUB

#endif

#endif    // __VSF_USBH_H__
