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

/*============================ MACROS ========================================*/

#ifndef USRAPP_CFG_CDC_NUM
#   define USRAPP_CFG_CDC_NUM               6
#endif

#if USRAPP_CFG_CDC_NUM >= 10
#   error not support
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

// __INT_EP starts from USRAPP_CFG_CDC_NUM + 1
// __I_FUNC starts from 4(first 3 are manufactorer/device/serial strings)
#define USRAPP_DESC_CDC_CFG(__N, __UNUSED)                                      \
            USB_DESC_CDC_UART_HS((__N) * 2, 4 + (__N), USRAPP_CFG_CDC_NUM + 1 + (__N), 1 + (__N), 1 + (__N))
#define USRAPP_DESC_CDC_STRING(__N, __UNUSED)                                   \
            .str_cdc[(__N)]             = {                                     \
                USB_DESC_STRING(14,                                             \
                    'V', 0, 'S', 0, 'F', 0, 'C', 0, 'D', 0, 'C', 0, '0' + (__N), 0\
                )                                                               \
            },
#define USRAPP_DESC_CDC_STRING_DECLARE(__N, __USBD_CONST)                       \
            VSF_USBD_DESC_STRING(0x0409, 4 + (__N), __USBD_CONST.str_cdc[(__N)], sizeof(__USBD_CONST.str_cdc[(__N)])),

#define USRAPP_CDC_INSTANCE(__N, __USBD)                                        \
    .cdc[(__N)]                 = {                                             \
        .param                  = {                                             \
            .ep = {                                                             \
                .notify         = USRAPP_CFG_CDC_NUM + 1 + (__N),               \
                .out            = 1 + (__N),                                    \
                .in             = 1 + (__N),                                    \
            },                                                                  \
            .line_coding        = {                                             \
                .bitrate        = 115200,                                       \
                .stop           = 0,                                            \
                .parity         = 0,                                            \
                .datalen        = 8,                                            \
            },                                                                  \
            .stream.tx.stream   = (vsf_stream_t *)&__USBD.cdc[(__N)].stream.tx, \
            .stream.rx.stream   = (vsf_stream_t *)&__USBD.cdc[(__N)].stream.rx, \
        },                                                                      \
        .stream             = {                                                 \
            .tx.op              = &vsf_fifo_stream_op,                          \
            .tx.buffer          = (uint8_t *)&__USBD.cdc[(__N)].stream.tx_buffer,\
            .tx.size            = sizeof(__USBD.cdc[(__N)].stream.tx_buffer),   \
            .rx.op              = &vsf_fifo_stream_op,                          \
            .rx.buffer          = (uint8_t *)&__USBD.cdc[(__N)].stream.rx_buffer,\
            .rx.size            = sizeof(__USBD.cdc[(__N)].stream.rx_buffer),   \
        },                                                                      \
    },                                                                          \
    .ifs[2 * (__N)].class_op        = &vsf_usbd_CDCACM_control,                 \
    .ifs[2 * (__N)].class_param     = &__USBD.cdc[(__N)].param,                 \
    .ifs[2 * (__N) + 1].class_op    = &vsf_usbd_CDCACM_data,                    \
    .ifs[2 * (__N) + 1].class_param = &__USBD.cdc[(__N)].param,

/*============================ TYPES =========================================*/

struct usrapp_const_t {
    struct {
#ifdef __GD32VF103__
        vsf_dwcotg_dcd_param_t dwcotg_param;
#endif
        uint8_t dev_desc[18];
        uint8_t config_desc[9 + USRAPP_CFG_CDC_NUM * USB_DESC_CDC_ACM_LEN];
        uint8_t str_lanid[4];
        uint8_t str_vendor[20];
        uint8_t str_product[26];
        uint8_t str_cdc[USRAPP_CFG_CDC_NUM][16];
        vsf_usbd_desc_t std_desc[5 + USRAPP_CFG_CDC_NUM];
    } usbd;
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
    struct {
#ifdef __GD32VF103__
        vsf_dwcotg_dcd_t dwcotg_dcd;
#endif
        struct {
            vsf_usbd_CDCACM_t param;
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
            struct {
                vsf_fifo_stream_t tx;
                vsf_fifo_stream_t rx;
                uint8_t tx_buffer[4 * 1024];
                uint8_t rx_buffer[4 * 1024];
            } stream;
#elif VSF_USE_SERVICE_STREAM == ENABLED
#endif
        } cdc[USRAPP_CFG_CDC_NUM];

        vsf_usbd_ifs_t ifs[2 * USRAPP_CFG_CDC_NUM];
        vsf_usbd_cfg_t config[1];
        vsf_usbd_dev_t dev;

        vsf_callback_timer_t connect_timer;
    } usbd;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t usrapp_const = {
    .usbd                       = {
#ifdef __GD32VF103__
        .dwcotg_param           = {
            .op                 = &VSF_USB_DC0_IP,
            .speed              = USB_SPEED_FULL,
        },
#endif
        .dev_desc               = {
            USB_DESC_DEV(64, APP_CFG_USBD_VID, APP_CFG_USBD_PID, 1, 2, 0, 1)
        },
        .config_desc            = {
            USB_DESC_CFG(sizeof(usrapp_const.usbd.config_desc), 2 * USRAPP_CFG_CDC_NUM, 1, 0, 0x80, 100)
            MREPEAT(USRAPP_CFG_CDC_NUM, USRAPP_DESC_CDC_CFG, NULL)
        },
        .str_lanid              = {
            USB_DESC_STRING(2, 0x09, 0x04)
        },
        .str_vendor             = {
            USB_DESC_STRING(18,
                'S', 0, 'i', 0, 'm', 0, 'o', 0, 'n', 0, 'Q', 0, 'i', 0, 'a', 0,
                'n', 0
            )
        },
        .str_product            = {
            USB_DESC_STRING(24,
                'V', 0, 'S', 0, 'F', 0, '-', 0, 'M', 0, 'u', 0, 'l', 0, 't', 0,
                'i', 0, 'C', 0, 'D', 0, 'C', 0
            )
        },
        MREPEAT(USRAPP_CFG_CDC_NUM, USRAPP_DESC_CDC_STRING, NULL)
        .std_desc               = {
            VSF_USBD_DESC_DEVICE(0, usrapp_const.usbd.dev_desc, sizeof(usrapp_const.usbd.dev_desc)),
            VSF_USBD_DESC_CONFIG(0, 0, usrapp_const.usbd.config_desc, sizeof(usrapp_const.usbd.config_desc)),
            VSF_USBD_DESC_STRING(0, 0, usrapp_const.usbd.str_lanid, sizeof(usrapp_const.usbd.str_lanid)),
            VSF_USBD_DESC_STRING(0x0409, 1, usrapp_const.usbd.str_vendor, sizeof(usrapp_const.usbd.str_vendor)),
            VSF_USBD_DESC_STRING(0x0409, 2, usrapp_const.usbd.str_product, sizeof(usrapp_const.usbd.str_product)),
            MREPEAT(USRAPP_CFG_CDC_NUM, USRAPP_DESC_CDC_STRING_DECLARE, usrapp_const.usbd)
        },
    },
};

#ifdef __GD32VF103__
static usrapp_t usrapp;
VSF_USB_DC_FROM_DWCOTG_IP(0, usrapp.usbd.dwcotg_dcd, VSF_USB_DC0)
#endif

static usrapp_t usrapp = {
    .usbd                       = {
#ifdef __GD32VF103__
        .dwcotg_dcd.param       = &usrapp_const.usbd.dwcotg_param,
#endif
        MREPEAT(USRAPP_CFG_CDC_NUM, USRAPP_CDC_INSTANCE, usrapp.usbd)

        .config[0].num_of_ifs   = dimof(usrapp.usbd.ifs),
        .config[0].ifs          = usrapp.usbd.ifs,

        .dev.num_of_config      = dimof(usrapp.usbd.config),
        .dev.config             = usrapp.usbd.config,
        .dev.num_of_desc        = dimof(usrapp_const.usbd.std_desc),
        .dev.desc               = (vsf_usbd_desc_t *)usrapp_const.usbd.std_desc,

        .dev.speed              = USB_DC_SPEED_HIGH,
        .dev.drv                = &VSF_USB_DC0,//&VSF_USB.DC[0],
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void usrapp_on_timer(vsf_callback_timer_t *timer)
{
    vsf_usbd_connect(&usrapp.usbd.dev);
}

void main(void)
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_init(NULL);
    vsf_stdio_init();
#endif

    vsf_usbd_init(&usrapp.usbd.dev);
    vsf_usbd_disconnect(&usrapp.usbd.dev);
    usrapp.usbd.connect_timer.on_timer = usrapp_on_timer;
    vsf_callback_timer_add_ms(&usrapp.usbd.connect_timer, 200);
}

/* EOF */
