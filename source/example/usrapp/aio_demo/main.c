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

#include "vsf.h"
#include <stdio.h>
#include <stdarg.h>

#include "component/3rd-party/lwip/1.4.1/port/lwip_netdrv_adapter.h"

/*============================ MACROS ========================================*/

#define VSF_CFG_VSFIP_EN                DISABLED

#define GENERATE_HEX(value)             TPASTE2(0x, value)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_const_t {
    struct {
        vsf_ohci_param_t ohci_param;
    } usbh;

    struct {
        uint8_t dev_desc[18];
        uint8_t config_desc[75 + 66];
        uint8_t str_lanid[4];
        uint8_t str_vendor[20];
        uint8_t str_product[14];
        uint8_t str_cdc[14];
        uint8_t str_cdc2[16];
        vsf_usbd_desc_t std_desc[7];
    } usbd;
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
    struct {
        vsf_usbh_t host;
        vsf_usbh_class_t hub;
        vsf_usbh_class_t ecm;
        vsf_usbh_class_t bthci;
        vsf_usbh_class_t hid;
    } usbh;

    struct {
        bool inited;
#if VSF_CFG_VSFIP_EN == ENABLED
        vsfip_netif_t netif;
        union {
            vsfip_dhcpc_t dhcpc;
            vsfip_dhcpd_t dhcpd;
        };
#else
        struct netif netif;
#endif
    } tcpip;
/*
    struct {
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        vsf_stream_t stream;
#endif
    } debug;
*/
    struct {
        struct {
            vsf_usbd_CDCACM_t param[2];
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
            struct {
                vsf_fifo_stream_t tx;
                vsf_fifo_stream_t rx;
                uint8_t tx_buffer[4 * 1024];
                uint8_t rx_buffer[4 * 1024];
            } stream[2];
#elif VSF_USE_SERVICE_STREAM == ENABLED
#endif
        } cdc;
        vsf_usbd_ifs_t ifs[4];
        vsf_usbd_cfg_t config[1];
        vsf_usbd_dev_t dev;
        bool is_connected;
    } usbd;

    vsf_callback_timer_t poll_timer;
    //uint8_t heap[0x8000];
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t usrapp_const = {
    .usbh.ohci_param            = {
        .hc                     = (vsf_usb_hc_t *)&USB_HC0,
        .priority               = 0xFF,
    },

    .usbd                       = {
        .dev_desc               = {
            USB_DT_DEVICE_SIZE,
            USB_DT_DEVICE,
            0x00, 0x02,     // bcdUSB
            0xEF,           // device class: IAD
            0x02,           // device sub class
            0x01,           // device protocol
            64,             // max packet size
            (GENERATE_HEX(APP_CFG_USBD_VID) >> 0) & 0xFF,
            (GENERATE_HEX(APP_CFG_USBD_VID) >> 8) & 0xFF,
                            // vendor
            (GENERATE_HEX(APP_CFG_USBD_PID) >> 0) & 0xFF,
            (GENERATE_HEX(APP_CFG_USBD_PID) >> 8) & 0xFF,
                            // product
            0x00, 0x02,     // bcdDevice
            1,              // manu facturer
            2,              // product
            0,              // serial number
            1,              // number of configuration
        },
        .config_desc            = {
            USB_DT_CONFIG_SIZE,
            USB_DT_CONFIG,
            (sizeof(usrapp_const.usbd.config_desc) >> 0) & 0xFF,
            (sizeof(usrapp_const.usbd.config_desc) >> 8) & 0xFF,
                            // wTotalLength
            0x04,           // bNumInterfaces: 4 interfaces
            0x01,           // bConfigurationValue: Configuration value
            0x00,           // iConfiguration: Index of string descriptor describing the configuration
            0x80,           // bmAttributes: bus powered
            0x64,           // MaxPower

            // IDA for CDC
            USB_DT_INTERFACE_ASSOCIATION_SIZE,
            USB_DT_INTERFACE_ASSOCIATION,
            0,              // bFirstInterface
            2,              // bInterfaceCount
            0x02,           // bFunctionClass
            0x02,           // bFunctionSubClass
            0x01,           // bFunctionProtocol
            0x04,           // iFunction

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x00,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x01,           // bNumEndpoints
            0x02,           // bInterfaceClass:
            0x02,           // bInterfaceSubClass:
            0x01,           // nInterfaceProtocol:
            0x04,           // iInterface:

            // Header Functional Descriptor
            0x05,           // bLength: Endpoint Descriptor size
            0x24,           // bDescriptorType: CS_INTERFACE
            0x00,           // bDescriptorSubtype: Header Func Desc
            0x10,           // bcdCDC: spec release number
            0x01,

            // Call Managment Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x01,           // bDescriptorSubtype: Call Management Func Desc
            0x00,           // bmCapabilities: D0+D1
            0x01,           // bDataInterface: 1

            // ACM Functional Descriptor
            0x04,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x02,           // bDescriptorSubtype: Abstract Control Management desc
            0x02,           // bmCapabilities

            // Union Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x06,           // bDescriptorSubtype: Union func desc
            0,              // bMasterInterface: Communication class interface
            1,              // bSlaveInterface0: Data Class Interface

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x81,           // bEndpointAddress: (IN1)
            0x03,           // bmAttributes: Interrupt
            8, 0x00,        // wMaxPacketSize:
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x10,           // bInterval:
#else
            0xFF,           // bInterval:
#endif

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x01,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x02,           // bNumEndpoints
            0x0A,           // bInterfaceClass
            0x00,           // bInterfaceSubClass
            0x00,           // nInterfaceProtocol
            0x04,           // iInterface:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x82,           // bEndpointAddress: (IN2)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x00, 0x02,     // wMaxPacketSize:
#else
            0x40, 0x00,     // wMaxPacketSize:
#endif
            0x00,           // bInterval:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x02,           // bEndpointAddress: (OUT2)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x00, 0x02,     // wMaxPacketSize:
#else
            0x40, 0x00,     // wMaxPacketSize:
#endif
            0x00,           // bInterval:

            // IDA for another CDC
            USB_DT_INTERFACE_ASSOCIATION_SIZE,
            USB_DT_INTERFACE_ASSOCIATION,
            2,              // bFirstInterface
            2,              // bInterfaceCount
            0x02,           // bFunctionClass
            0x02,           // bFunctionSubClass
            0x01,           // bFunctionProtocol
            0x05,           // iFunction

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x02,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x01,           // bNumEndpoints
            0x02,           // bInterfaceClass:
            0x02,           // bInterfaceSubClass:
            0x01,           // nInterfaceProtocol:
            0x05,           // iInterface:

            // Header Functional Descriptor
            0x05,           // bLength: Endpoint Descriptor size
            0x24,           // bDescriptorType: CS_INTERFACE
            0x00,           // bDescriptorSubtype: Header Func Desc
            0x10,           // bcdCDC: spec release number
            0x01,

            // Call Managment Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x01,           // bDescriptorSubtype: Call Management Func Desc
            0x00,           // bmCapabilities: D0+D1
            0x01,           // bDataInterface: 1

            // ACM Functional Descriptor
            0x04,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x02,           // bDescriptorSubtype: Abstract Control Management desc
            0x02,           // bmCapabilities

            // Union Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x06,           // bDescriptorSubtype: Union func desc
            2,              // bMasterInterface: Communication class interface
            3,              // bSlaveInterface0: Data Class Interface

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x83,           // bEndpointAddress: (IN3)
            0x03,           // bmAttributes: Interrupt
            8, 0x00,        // wMaxPacketSize:
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x10,           // bInterval:
#else
            0xFF,           // bInterval:
#endif

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x03,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x02,           // bNumEndpoints
            0x0A,           // bInterfaceClass
            0x00,           // bInterfaceSubClass
            0x00,           // nInterfaceProtocol
            0x05,           // iInterface:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x84,           // bEndpointAddress: (IN4)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x00, 0x02,     // wMaxPacketSize:
#else
            0x40, 0x00,     // wMaxPacketSize:
#endif
            0x00,           // bInterval:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x04,           // bEndpointAddress: (OUT4)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x00, 0x02,     // wMaxPacketSize:
#else
            0x40, 0x00,     // wMaxPacketSize:
#endif
            0x00,           // bInterval:
        },
        .str_lanid              = {
            4,
            USB_DT_STRING,
            0x09,
            0x04,
        },
        .str_vendor             = {
            20,
            USB_DT_STRING,
            'S', 0, 'i', 0, 'm', 0, 'o', 0, 'n', 0, 'Q', 0, 'i', 0, 'a', 0,
            'n', 0,
        },
        .str_product            = {
            14,
            USB_DT_STRING,
            'V', 0, 'S', 0, 'F', 0, 'A', 0, 'I', 0, 'O', 0,
        },
        .str_cdc                = {
            14,
            USB_DT_STRING,
            'V', 0, 'S', 0, 'F', 0, 'C', 0, 'D', 0, 'C', 0,
        },
        .str_cdc2               = {
            16,
            USB_DT_STRING,
            'V', 0, 'S', 0, 'F', 0, 'C', 0, 'D', 0, 'C', 0, '2', 0,
        },
        .std_desc               = {
            VSF_USBD_DESC_DEVICE(0, usrapp_const.usbd.dev_desc, sizeof(usrapp_const.usbd.dev_desc)),
            VSF_USBD_DESC_CONFIG(0, 0, usrapp_const.usbd.config_desc, sizeof(usrapp_const.usbd.config_desc)),
            VSF_USBD_DESC_STRING(0, 0, usrapp_const.usbd.str_lanid, sizeof(usrapp_const.usbd.str_lanid)),
            VSF_USBD_DESC_STRING(0x0409, 1, usrapp_const.usbd.str_vendor, sizeof(usrapp_const.usbd.str_vendor)),
            VSF_USBD_DESC_STRING(0x0409, 2, usrapp_const.usbd.str_product, sizeof(usrapp_const.usbd.str_product)),
            VSF_USBD_DESC_STRING(0x0409, 4, usrapp_const.usbd.str_cdc, sizeof(usrapp_const.usbd.str_cdc)),
            VSF_USBD_DESC_STRING(0x0409, 5, usrapp_const.usbd.str_cdc2, sizeof(usrapp_const.usbd.str_cdc2)),
        },
    },
};

static usrapp_t usrapp = {
    .usbh                       = {
        .host.drv               = &vsf_ohci_drv,
        .host.param             = (void *)&usrapp_const.usbh.ohci_param,

        .hub.drv                = &vsf_usbh_hub_drv,
        .ecm.drv                = &vsf_usbh_ecm_drv,
        .bthci.drv              = &vsf_usbh_bthci_drv,
        .hid.drv                = &vsf_usbh_hid_drv,
    },
/*
    .debug.stream               = {
        .op                     = &vsf_nu_console_stream_op,
    },
*/
    .usbd                       = {
        .cdc.param[0]           = {
            .ep = {
                .notify         = 1,
                .out            = 2,
                .in             = 2,
            },
            .line_coding        = {
                .bitrate        = 115200,
                .stop           = 0,
                .parity         = 0,
                .datalen        = 8,
            },
        },
        .cdc.param[1]           = {
            .ep = {
                .notify         = 3,
                .out            = 4,
                .in             = 4,
            },
            .line_coding        = {
                .bitrate        = 115200,
                .stop           = 0,
                .parity         = 0,
                .datalen        = 8,
            },
        },
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        .cdc.param[0].stream.tx.stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[0].tx,
        .cdc.param[0].stream.rx.stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[0].rx,
        .cdc.stream[0]          = {
            .tx.op              = &vsf_fifo_stream_op,
            .tx.buffer          = (uint8_t *)&usrapp.usbd.cdc.stream[0].tx_buffer,
            .tx.size            = sizeof(usrapp.usbd.cdc.stream[0].tx_buffer),
            .rx.op              = &vsf_fifo_stream_op,
            .rx.buffer          = (uint8_t *)&usrapp.usbd.cdc.stream[0].rx_buffer,
            .rx.size            = sizeof(usrapp.usbd.cdc.stream[0].rx_buffer),
        },
        .cdc.param[1].stream.tx.stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[1].tx,
        .cdc.param[1].stream.rx.stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[1].rx,
        .cdc.stream[1]          = {
            .tx.op              = &vsf_fifo_stream_op,
            .tx.buffer          = (uint8_t *)&usrapp.usbd.cdc.stream[1].tx_buffer,
            .tx.size            = sizeof(usrapp.usbd.cdc.stream[1].tx_buffer),
            .rx.op              = &vsf_fifo_stream_op,
            .rx.buffer          = (uint8_t *)&usrapp.usbd.cdc.stream[1].rx_buffer,
            .rx.size            = sizeof(usrapp.usbd.cdc.stream[1].rx_buffer),
        },
#elif VSF_USE_SERVICE_STREAM == ENABLED
#endif

        .ifs[0].class_op        = &vsf_usbd_CDCACM_control,
        .ifs[0].class_param     = &usrapp.usbd.cdc.param[0],
        .ifs[1].class_op        = &vsf_usbd_CDCACM_data,
        .ifs[1].class_param     = &usrapp.usbd.cdc.param[0],

        .ifs[2].class_op        = &vsf_usbd_CDCACM_control,
        .ifs[2].class_param     = &usrapp.usbd.cdc.param[1],
        .ifs[3].class_op        = &vsf_usbd_CDCACM_data,
        .ifs[3].class_param     = &usrapp.usbd.cdc.param[1],

        .config[0].num_of_ifs   = dimof(usrapp.usbd.ifs),
        .config[0].ifs          = usrapp.usbd.ifs,

        .dev.num_of_config      = dimof(usrapp.usbd.config),
        .dev.config             = usrapp.usbd.config,
        .dev.num_of_desc        = dimof(usrapp_const.usbd.std_desc),
        .dev.desc               = (vsf_usbd_desc_t *)usrapp_const.usbd.std_desc,

        .dev.speed              = USB_DC_SPEED_HIGH,
        .dev.priority_int       = 0,
#if VSF_USBD_CFG_USE_EDA == ENABLED
        .dev.priority_eda       = vsf_priority_0,
#endif
        .dev.drv                = &VSF_USB_DC0,//&VSF_USB.DC[0],
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_CFG_VSFIP_EN == ENABLED
vsfip_socket_t * vsfip_mem_socket_get(void)
{
    return vsf_heap_malloc(sizeof(vsfip_socket_t));
}

void vsfip_mem_socket_free(vsfip_socket_t *socket)
{
    vsf_heap_free(socket);
}

vsfip_netbuf_t * vsfip_mem_netbuf_get(uint_fast32_t size)
{
    vsfip_netbuf_t *netbuf = vsf_heap_malloc(sizeof(*netbuf) + size);
    if (netbuf != NULL) {
        netbuf->buffer = (uint8_t *)&netbuf[1];
    }
    return netbuf;
}

void vsfip_mem_netbuf_free(vsfip_netbuf_t *netbuf)
{
    vsf_heap_free(netbuf);
}

void vsf_pnp_on_netdrv_disconnect(vsf_netdrv_t *netdrv)
{
    if (usrapp.tcpip.inited && (netdrv == vsfip_netif_get_netdrv(&usrapp.tcpip.netif))) {
        vsfip_dhcpc_stop(&usrapp.tcpip.dhcpc);
    }
}

void vsf_pnp_on_netdrv_connect(vsf_netdrv_t *netdrv)
{
    usrapp.tcpip.netif.op = &vsfip_eth_op;
    vsfip_netif_set_netdrv(&usrapp.tcpip.netif, netdrv);
}

void vsf_pnp_on_netdrv_connected(vsf_netdrv_t *netdrv)
{
    if (usrapp.tcpip.inited && (netdrv == vsfip_netif_get_netdrv(&usrapp.tcpip.netif))) {
        vsfip_dhcpc_start(&usrapp.tcpip.netif, &usrapp.tcpip.dhcpc);
    }
}

void vsf_pnp_on_netdrv_new(vsf_netdrv_t *netdrv)
{
    vsf_protect_t origlevel = vsf_protect_scheduler();
        if (usrapp.tcpip.inited) {
            vsf_unprotect_scheduler(origlevel);
            return;
        }
        usrapp.tcpip.inited = true;
    vsf_unprotect_scheduler(origlevel);
}

void vsf_pnp_on_netdrv_del(vsf_netdrv_t *netdrv)
{
    vsf_protect_t origlevel = vsf_protect_scheduler();
        usrapp.tcpip.inited = false;
    vsf_unprotect_scheduler(origlevel);
}
#else
void vsf_pnp_on_netdrv_disconnect(vsf_netdrv_t *netdrv)
{
    if (usrapp.tcpip.inited && (netdrv == lwip_netif_get_netdrv(&usrapp.tcpip.netif))) {
        dhcp_stop(&usrapp.tcpip.netif);
    }
}

void vsf_pnp_on_netdrv_connect(vsf_netdrv_t *netdrv)
{
    lwip_netif_set_netdrv(&usrapp.tcpip.netif, netdrv);
}

void vsf_pnp_on_netdrv_connected(vsf_netdrv_t *netdrv)
{
    if (usrapp.tcpip.inited && (netdrv == lwip_netif_get_netdrv(&usrapp.tcpip.netif))) {
        dhcp_start(&usrapp.tcpip.netif);
    }
}

void vsf_pnp_on_netdrv_new(vsf_netdrv_t *netdrv)
{
    vsf_protect_t origlevel = vsf_protect_scheduler();
        if (usrapp.tcpip.inited) {
            vsf_unprotect_scheduler(origlevel);
            return;
        }
        usrapp.tcpip.inited = true;
    vsf_unprotect_scheduler(origlevel);
}

void vsf_pnp_on_netdrv_del(vsf_netdrv_t *netdrv)
{
    vsf_protect_t origlevel = vsf_protect_scheduler();
        usrapp.tcpip.inited = false;
    vsf_unprotect_scheduler(origlevel);
}
#endif

void usrapp_on_timer(vsf_callback_timer_t *timer)
{
    if (!usrapp.usbd.is_connected) {
        usrapp.usbd.is_connected = true;
        vsf_usbd_connect(&usrapp.usbd.dev);
    } else {
        vsf_trace(VSF_TRACE_INFO, "heartbeat: [%lld]" VSF_TRACE_CFG_LINEEND, vsf_timer_get_tick());
    }
    vsf_callback_timer_add_ms(timer, 1000);
}

#include "btstack_event.h"
#include "btstack_run_loop.h"
#include "btstack_memory.h"
#include "hci.h"
#include "btstack_chipset_csr.h"
#include "component/3rd-party/btstack/port/btstack_run_loop_vsf.h"

int btstack_main(int argc, const char * argv[]);
vsf_err_t vsf_bluetooth_h2_on_new(void *dev, vsf_usbh_dev_id_t *id)
{
	if ((id->idVendor == 0x0A12) && (id->idProduct == 0x0001)) {
		btstack_memory_init();
		btstack_run_loop_init(btstack_run_loop_vsf_get_instance());
		hci_init(hci_transport_usb_instance(), dev);
		hci_set_chipset(btstack_chipset_csr_instance());
		btstack_main(0, NULL);
        return VSF_ERR_NONE;
	}
    return VSF_ERR_FAIL;
}

static void usrapp_trace_hid(vsf_hid_event_t *hid_evt)
{
    if (hid_evt->id != 0) {
        uint_fast16_t generic_usage, usage_page, usage_id;

        generic_usage = HID_GET_GENERIC_USAGE(hid_evt->id);
        usage_page = HID_GET_USAGE_PAGE(hid_evt->id);
        usage_id = HID_GET_USAGE_ID(hid_evt->id);

        vsf_trace(VSF_TRACE_DEBUG, "hid(%d): page=%d, id=%d, pre=%d, cur=%d" VSF_TRACE_CFG_LINEEND,
            generic_usage, usage_page, usage_id, hid_evt->pre, hid_evt->cur);
    }
}

#if 1
void vsf_input_on_evt(vsf_input_type_t type, vsf_input_evt_t *event)
{
    switch (type) {
    case VSF_INPUT_TYPE_HID:
        usrapp_trace_hid((vsf_hid_event_t *)event);
        break;
    case VSF_INPUT_TYPE_SENSOR:
        break;
    default:
        break;
    }
}
#else
void vsf_hid_on_report_input(vsf_hid_event_t *hid_evt)
{
    usrapp_trace_hid(hid_evt);
}
#endif

#include "lwip/init.h"

int main(void)
{
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
//    vsf_trace_init(&usrapp.usbd.cdc.stream[0].tx.use_as__vsf_stream_t);

    //vsf_stream_init(&usrapp.debug.stream);
    vsf_trace_init(NULL);
#elif VSF_USE_SERVICE_STREAM == ENABLED
#endif

    //vsf_heap_init();
    //vsf_heap_add(usrapp.heap, sizeof(usrapp.heap));

    vsf_ohci_init();
    vsf_usbh_init(&usrapp.usbh.host);
    vsf_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.hub);
    vsf_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.ecm);
    vsf_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.bthci);
    vsf_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.hid);


    vsf_usbd_init(&usrapp.usbd.dev);
    vsf_usbd_disconnect(&usrapp.usbd.dev);

#if VSF_CFG_VSFIP_EN != ENABLED
    tcpip_init(NULL, NULL);
#endif

    usrapp.poll_timer.on_timer = usrapp_on_timer;
    vsf_callback_timer_add_ms(&usrapp.poll_timer, 200);

    return 0;
}

/* EOF */
