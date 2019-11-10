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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_const_t {
    struct {
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        vsf_dwcotg_dcd_param_t dwcotg_param;
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        vsf_musb_fdrc_dcd_param_t musb_fdrc_param;
#endif
        uint8_t dev_desc[18];
        uint8_t config_desc[9 + USB_DESC_MSCBOT_IAD_LEN];
        uint8_t str_lanid[4];
        uint8_t str_vendor[20];
        uint8_t str_product[16];
        vsf_usbd_desc_t std_desc[6];
    } usbd;
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
    struct {
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        vsf_dwcotg_dcd_t dwcotg_dcd;
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        vsf_musb_fdrc_dcd_t musb_fdrc_dcd;
#endif
        struct {
            vsf_usbd_msc_t param;
        } msc;

        vsf_usbd_ifs_t ifs[1];
        vsf_usbd_cfg_t config[1];
        vsf_usbd_dev_t dev;

        vsf_callback_timer_t connect_timer;
    } usbd;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/

/*============================ PROTOTYPES ====================================*/

extern void usrapp_msc_scsi_init(vsf_usbd_msc_t *msc);
extern vsf_usbd_msc_op_t usrapp_msc_op;

/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t usrapp_const = {
    .usbd                       = {
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        .dwcotg_param           = {
            .op                 = &VSF_USB_DC0_IP,
            .speed              = USRAPP_CFG_USBD_SPEED,
        },
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        musb_fdrc_param         = {
            .op                 = &VSF_USB_DC0_IP,
        },
#endif

        .dev_desc               = {
            USB_DESC_DEV_IAD(64, APP_CFG_USBD_VID, APP_CFG_USBD_PID, 1, 2, 0, 1)
        },
        .config_desc            = {
            USB_DESC_CFG(sizeof(usrapp_const.usbd.config_desc), 1, 1, 0, 0x80, 100)
#if USRAPP_CFG_USBD_SPEED == USB_SPEED_HIGH
            USB_DESC_MSCBOT_IAD(0, 4, 1, 1, 256)
#elif USRAPP_CFG_USBD_SPEED == USB_SPEED_FULL
            USB_DESC_MSCBOT_IAD(0, 4, 1, 1, 64)
#endif
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
            USB_DESC_STRING(14,
                'V', 0, 'S', 0, 'F', 0, '-', 0, 'M', 0, 'S', 0, 'C', 0
            )
        },
        .std_desc               = {
            VSF_USBD_DESC_DEVICE(0, usrapp_const.usbd.dev_desc, sizeof(usrapp_const.usbd.dev_desc)),
            VSF_USBD_DESC_CONFIG(0, 0, usrapp_const.usbd.config_desc, sizeof(usrapp_const.usbd.config_desc)),
            VSF_USBD_DESC_STRING(0, 0, usrapp_const.usbd.str_lanid, sizeof(usrapp_const.usbd.str_lanid)),
            VSF_USBD_DESC_STRING(0x0409, 1, usrapp_const.usbd.str_vendor, sizeof(usrapp_const.usbd.str_vendor)),
            VSF_USBD_DESC_STRING(0x0409, 2, usrapp_const.usbd.str_product, sizeof(usrapp_const.usbd.str_product)),
            VSF_USBD_DESC_STRING(0x0409, 4, usrapp_const.usbd.str_product, sizeof(usrapp_const.usbd.str_product)),
        },
    },
};

static usrapp_t usrapp;
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
VSF_USB_DC_FROM_DWCOTG_IP(0, usrapp.usbd.dwcotg_dcd, VSF_USB_DC0)
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
VSF_USB_DC_FROM_MUSB_FDRC_IP(0, usrapp.usbd.musb_fdrc_dcd, VSF_USB_DC0)
#endif

static usrapp_t usrapp = {
    .usbd                       = {
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        .dwcotg_dcd.param       = &usrapp_const.usbd.dwcotg_param,
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        .musb_fdrc_dcd.param    = &usrapp_const.usbd.musb_fdrc_param,
#endif

        .msc.param              = {
            .ep_out             = 1,
            .ep_in              = 1,
            .max_lun            = 1,
            .op                 = &usrapp_msc_op,
        },

        .ifs[0].class_op        = &vsf_usbd_msc_class,
        .ifs[0].class_param     = &usrapp.usbd.msc.param,

        .config[0].num_of_ifs   = dimof(usrapp.usbd.ifs),
        .config[0].ifs          = usrapp.usbd.ifs,

        .dev.num_of_config      = dimof(usrapp.usbd.config),
        .dev.config             = usrapp.usbd.config,
        .dev.num_of_desc        = dimof(usrapp_const.usbd.std_desc),
        .dev.desc               = (vsf_usbd_desc_t *)usrapp_const.usbd.std_desc,

#if USRAPP_CFG_USBD_SPEED == USB_SPEED_HIGH
        .dev.speed              = USB_DC_SPEED_HIGH,
#elif USRAPP_CFG_USBD_SPEED == USB_SPEED_FULL
        .dev.speed              = USB_DC_SPEED_FULL,
#endif
        .dev.drv                = &VSF_USB_DC0,//&VSF_USB.DC[0],
    },
};

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

    usrapp_msc_scsi_init(&usrapp.usbd.msc.param);
    vsf_usbd_init(&usrapp.usbd.dev);
    vsf_usbd_disconnect(&usrapp.usbd.dev);
    usrapp.usbd.connect_timer.on_timer = usrapp_on_timer;
    vsf_callback_timer_add_ms(&usrapp.usbd.connect_timer, 200);
}

/* EOF */
