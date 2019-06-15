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

/*============================ MACROS ========================================*/

#define GENERATE_HEX(value)                TPASTE2(0x, value)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_const_t {
#if VSF_USE_USB_HOST == ENABLED
    vsf_ohci_param_t ohci_param;
#endif

#if VSF_USE_USB_DEVICE == ENABLED
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
#endif
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
#if VSF_USE_USB_HOST == ENABLED
    vsf_usbh_t usbh;
    vsf_usbh_class_t usbh_hub;
    vsf_usbh_class_t usbh_libusb;
    vsf_usbh_class_t usbh_ecm;
    uint8_t dev_count;
    //uint8_t heap[0x4000];

    struct {
        bool inited;
        vsfip_netif_t netif;
        union {
            vsfip_dhcpc_t dhcpc;
            vsfip_dhcpd_t dhcpd;
        };
    } tcpip;
#endif

#if VSF_USE_USB_DEVICE == ENABLED
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
            struct { 
                vsf_stream_fifo_t tx;
                vsf_stream_fifo_t rx;
            }stream[2];
#endif
        } cdc;
/*
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        struct {
            vsf_stream_t stream;      
        }debug;
#endif
*/
        vsf_usbd_ifs_t ifs[4]; 
        vsf_usbd_cfg_t config[1];
        vsf_usbd_dev_t dev;
        vsf_callback_timer_t timer;
    } usbd;
#endif
};
typedef struct usrapp_t usrapp_t;

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
declare_vsf_thread(user_task_t)

def_vsf_thread(user_task_t, 1024,

    features_used(
        mem_sharable( using_grouped_evt; )
        mem_nonsharable( )
    )
    
    def_params(
    ));
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t usrapp_const = {
#if VSF_USE_USB_HOST == ENABLED
    .ohci_param                 = {
        .hc                     = (vsf_usb_hc_t *)&USB_HC0,
        .priority               = 0xFF,
    },
#endif

#if VSF_USE_USB_DEVICE == ENABLED
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
#endif
};

static usrapp_t usrapp = {
#if VSF_USE_USB_HOST == ENABLED
    .usbh.drv                   = &vsf_ohci_drv,
    .usbh.param                 = (void *)&usrapp_const.ohci_param,

    .usbh_hub.drv               = &vsf_usbh_hub_drv,
    .usbh_libusb.drv            = &vsf_usbh_libusb_drv,
    .usbh_ecm.drv               = &vsf_usbh_ecm_drv,
#endif

#if VSF_USE_USB_DEVICE == ENABLED
    .usbd                       = {
/*
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
*/
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        //.cdc.param[0].stream.tx.stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[0].tx,
        //.cdc.param[0].stream.rx.stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[0].rx,
        .cdc.stream[0]          = {
            .tx.op              = &vsf_fifo_stream_op,
            .tx.buffer          = (uint8_t *)&usrapp.usbd.cdc.stream[0].tx_buffer,
            .tx.size            = sizeof(usrapp.usbd.cdc.stream[0].tx_buffer),
            .rx.op              = &vsf_fifo_stream_op,
            .rx.buffer          = (uint8_t *)&usrapp.usbd.cdc.stream[0].rx_buffer,
            .rx.size            = sizeof(usrapp.usbd.cdc.stream[0].rx_buffer),
        },
        //.cdc.param[1].stream.tx.stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[1].tx,
        //.cdc.param[1].stream.rx.stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[1].rx,
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
#endif
};

/*============================ PROTOTYPES ====================================*/
extern void usrapp_log_info(const char *fmt, ...);

/*============================ IMPLEMENTATION ================================*/

void app_led_set(uint8_t count)
{
    count ^= 0x07;
    if (count & 1) {
        PA->DOUT |= 1 << 9;
    } else {
        PA->DOUT &= ~(1 << 9);
    }

    if (count & 2) {
        PA->DOUT |= 1 << 10;
    } else {
        PA->DOUT &= ~(1 << 10);
    }

    if (count & 4) {
        PA->DOUT |= 1 << 11;
    } else {
        PA->DOUT &= ~(1 << 11);
    }
}

void app_led_init(void)
{
    PA->MODE &= ~(0x3F << 18);
    PA->MODE |= 0x15 << 18;
    app_led_set(0);
}

#if VSF_USE_USB_HOST == ENABLED
static void app_on_libusb_event(void *param,
                                vsf_usbh_libusb_dev_t *dev,
                                vsf_usbh_libusb_evt_t evt)
{
    usrapp_t *app = (usrapp_t *)param;
    switch (evt) {
    case VSF_USBH_LIBUSB_EVT_ON_ARRIVED:
        app->dev_count++;
        break;
    case VSF_USBH_LIBUSB_EVT_ON_LEFT:
        app->dev_count--;
        break;
    }
    app_led_set(app->dev_count);
}
#endif


#if VSF_USE_USB_HOST == ENABLED

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
    if (usrapp.tcpip.inited && (usrapp.tcpip.netif.netdrv == netdrv)) {
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
    if (usrapp.tcpip.inited && (usrapp.tcpip.netif.netdrv == netdrv)) {
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

#endif

#if VSF_USE_USB_DEVICE == ENABLED
void usrapp_usbd_connect(vsf_callback_timer_t *timer)
{
    vsf_usbd_connect(&usrapp.usbd.dev);
}
#endif

static uint_fast32_t count = 0;
#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
implement_vsf_thread(user_task_t) 
{
    while (1) {
        
#if VSF_USE_TRACE == ENABLED
        vsf_delay(500);
        vsf_trace(VSF_TRACE_INFO, "test info [%d]" VSF_TRACE_CFG_LINEEND, count++);
#endif
    }
}
#else
static void usrapp_heartbeat_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_TIMER:
        vsf_trace(VSF_TRACE_INFO, "test info [%d]" VSF_TRACE_CFG_LINEEND, count++);
        // fall through
    case VSF_EVT_INIT:
        vsf_teda_set_timer_ms(500);
        break;
    }
}
#endif

int main(void)
{
#if VSF_USE_USB_HOST == ENABLED
    //vsf_heap_init();
    //vsf_heap_add(usrapp.heap, sizeof(usrapp.heap));

    app_led_init();

    vsf_ohci_init();
    vsf_usbh_init(&usrapp.usbh);
    vsf_usbh_libusb_set_evthandler(&usrapp, app_on_libusb_event);
//    vsf_usbh_register_class(&usrapp.usbh, &usrapp.usbh_libusb);
    vsf_usbh_register_class(&usrapp.usbh, &usrapp.usbh_hub);
    vsf_usbh_register_class(&usrapp.usbh, &usrapp.usbh_ecm);
#endif

#if VSF_USE_USB_DEVICE == ENABLED

#   if VSF_USE_SERVICE_STREAM == ENABLED
    vsf_stream_fifo_init(&usrapp.usbd.cdc.stream[0].tx, NULL);
    vsf_stream_fifo_init(&usrapp.usbd.cdc.stream[0].rx, NULL);
    vsf_stream_fifo_init(&usrapp.usbd.cdc.stream[1].tx, NULL);
    vsf_stream_fifo_init(&usrapp.usbd.cdc.stream[1].rx, NULL);
#endif

    //! initialise CDC0
    do {
        const vsf_usbd_CDCACM_cfg_t cfg = {
            .ep = {
                .notify         = 1,
                .out            = 2,
                .in             = 2,
            },
        #if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
            .rx_stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[0].rx,
            .tx_stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[0].tx,
        #elif   VSF_USE_SERVICE_STREAM == ENABLED
            .ptRX = &usrapp.usbd.cdc.stream[0].tx.RX,   //! echo
            .ptTX = &usrapp.usbd.cdc.stream[0].tx.TX,
        #   if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
            .hwpbufPoolReserve = 0xFF,          //! mark as privileged user
        #   endif
        #endif
        };

        vsf_usbd_CMDACM_init(&usrapp.usbd.cdc.param[0], &cfg);
    } while(0);

    //! initialise CDC1
    do {
        const vsf_usbd_CDCACM_cfg_t cfg = {
            .ep = {
                .notify         = 3,
                .out            = 4,
                .in             = 4,
            },
        #if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
            .rx_stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[1].rx,
            .tx_stream = (vsf_stream_t *)&usrapp.usbd.cdc.stream[1].tx,
        #elif   VSF_USE_SERVICE_STREAM == ENABLED
            .ptRX = &usrapp.usbd.cdc.stream[1].tx.RX,
            .ptTX = &usrapp.usbd.cdc.stream[1].tx.TX,
        #   if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
            .hwpbufPoolReserve = 0xFF,          //! mark as privileged user
        #   endif
        #endif
        };
        vsf_usbd_CMDACM_init(&usrapp.usbd.cdc.param[1], &cfg);
    } while(0);

    vsf_usbd_init(&usrapp.usbd.dev);
    vsf_usbd_disconnect(&usrapp.usbd.dev);

    usrapp.usbd.timer.on_timer = usrapp_usbd_connect;
    vsf_callback_timer_add_ms(&usrapp.usbd.timer, 200);
#endif

#if VSF_USE_TRACE == ENABLED
#   if 0
#       if      VSF_USE_SERVICE_VSFSTREAM == ENABLED
    vsf_trace_init(&usrapp.usbd.cdc.stream[0].tx);
#       elif    VSF_USE_SERVICE_STREAM == ENABLED
    vsf_trace_init(&(usrapp.usbd.cdc.stream[0].tx.TX));
#       else
    vsf_trace_init(NULL);
#       endif
#   else
    vsf_trace_init(NULL);
#   endif
#endif
    

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    do {
        static NO_INIT user_task_t __user_task;
        init_vsf_thread(user_task_t, &__user_task, vsf_priority_0);
    } while(0);
#else
    do {
        static vsf_teda_t teda = { .evthandler = usrapp_heartbeat_evthandler };
        vsf_teda_init(&teda, vsf_priority_0, false);
    } while (0);
#endif

    return 0;
}

/* EOF */
