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

#ifndef USRAPP_CFG_FAKEFAT32
#   define USRAPP_CFG_FAKEFAT32                 ENABLED
#endif

#if USRAPP_CFG_FAKEFAT32 == ENABLED
#   undef USRAPP_CFG_MSC_SIZE
#   define USRAPP_CFG_MSC_SIZE                  (512 * 0x1040)
#endif

#ifndef USRAPP_CFG_MSC_SIZE
#   define USRAPP_CFG_MSC_SIZE                  (16 * 1024)
#endif

/*============================ INCLUDES ======================================*/

#if USRAPP_CFG_FAKEFAT32 == ENABLED
#   include "fakefat32.c"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_const_t {
    struct {
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        vk_dwcotg_dcd_param_t dwcotg_param;
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        vk_musb_fdrc_dcd_param_t musb_fdrc_param;
#endif
        uint8_t dev_desc[18];
        uint8_t config_desc[9 + USB_DESC_MSCBOT_IAD_LEN];
        uint8_t str_lanid[4];
        uint8_t str_vendor[20];
        uint8_t str_product[16];
        vk_usbd_desc_t std_desc[6];
    } usbd;

    struct {
        vk_virtual_scsi_param_t param;
    } scsi;
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
    struct {
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        vk_dwcotg_dcd_t dwcotg_dcd;
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        vk_musb_fdrc_dcd_t musb_fdrc_dcd;
#endif
        struct {
            vk_usbd_msc_t param;
        } msc;

        vk_usbd_ifs_t ifs[1];
        vk_usbd_cfg_t config[1];
        vk_usbd_dev_t dev;

        vsf_callback_timer_t connect_timer;
    } usbd;

    struct {
#if USRAPP_CFG_FAKEFAT32 == ENABLED
        vk_fakefat32_mal_t fakefat32_mal;
#else
        uint8_t mem[USRAPP_CFG_MSC_SIZE];
        vk_mem_mal_t mem_mal;
#endif
        vk_mal_scsi_t mal_scsi;
    } scsi;

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
    struct {
        uint8_t buffer[1024];
        vsf_mem_stream_t mem_stream;
    } stream;
#endif
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t __usrapp_const = {
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
            USB_DESC_CFG(sizeof(__usrapp_const.usbd.config_desc), 1, 1, 0, 0x80, 100)
#if USRAPP_CFG_USBD_SPEED == USB_SPEED_HIGH
            USB_DESC_MSCBOT_IAD(0, 4, 1, 1, 512)
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
            VSF_USBD_DESC_DEVICE(0, __usrapp_const.usbd.dev_desc, sizeof(__usrapp_const.usbd.dev_desc)),
            VSF_USBD_DESC_CONFIG(0, 0, __usrapp_const.usbd.config_desc, sizeof(__usrapp_const.usbd.config_desc)),
            VSF_USBD_DESC_STRING(0, 0, __usrapp_const.usbd.str_lanid, sizeof(__usrapp_const.usbd.str_lanid)),
            VSF_USBD_DESC_STRING(0x0409, 1, __usrapp_const.usbd.str_vendor, sizeof(__usrapp_const.usbd.str_vendor)),
            VSF_USBD_DESC_STRING(0x0409, 2, __usrapp_const.usbd.str_product, sizeof(__usrapp_const.usbd.str_product)),
            VSF_USBD_DESC_STRING(0x0409, 4, __usrapp_const.usbd.str_product, sizeof(__usrapp_const.usbd.str_product)),
        },
    },

    .scsi.param                 = {
        .block_size             = 512,
        .block_num              = USRAPP_CFG_MSC_SIZE / 512,
        .vendor                 = "Simon   ",
        .product                = "VSFDriver       ",
        .revision               = "1.00",
        .type                   = SCSI_PDT_DIRECT_ACCESS_BLOCK,
    },
};

static usrapp_t __usrapp;
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
VSF_USB_DC_FROM_DWCOTG_IP(0, __usrapp.usbd.dwcotg_dcd, VSF_USB_DC0)
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
VSF_USB_DC_FROM_MUSB_FDRC_IP(0, __usrapp.usbd.musb_fdrc_dcd, VSF_USB_DC0)
#endif

static usrapp_t __usrapp = {
    .usbd                       = {
#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        .dwcotg_dcd.param       = &__usrapp_const.usbd.dwcotg_param,
#elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        .musb_fdrc_dcd.param    = &__usrapp_const.usbd.musb_fdrc_param,
#endif

        .msc.param              = {
            .ep_out             = 1,
            .ep_in              = 1,
            .max_lun            = 1,
            .scsi               = &__usrapp.scsi.mal_scsi.use_as__vk_scsi_t,
            .stream             = &__usrapp.stream.mem_stream.use_as__vsf_stream_t,
        },

        .ifs[0].class_op        = &vk_usbd_msc_class,
        .ifs[0].class_param     = &__usrapp.usbd.msc.param,

        .config[0].num_of_ifs   = dimof(__usrapp.usbd.ifs),
        .config[0].ifs          = __usrapp.usbd.ifs,

        .dev.num_of_config      = dimof(__usrapp.usbd.config),
        .dev.config             = __usrapp.usbd.config,
        .dev.num_of_desc        = dimof(__usrapp_const.usbd.std_desc),
        .dev.desc               = (vk_usbd_desc_t *)__usrapp_const.usbd.std_desc,

#if USRAPP_CFG_USBD_SPEED == USB_SPEED_HIGH
        .dev.speed              = USB_DC_SPEED_HIGH,
#elif USRAPP_CFG_USBD_SPEED == USB_SPEED_FULL
        .dev.speed              = USB_DC_SPEED_FULL,
#endif
        .dev.drv                = &VSF_USB_DC0,//&VSF_USB.DC[0],
    },

    .scsi                       = {
#if USRAPP_CFG_FAKEFAT32 == ENABLED
        .fakefat32_mal          = {
            .drv                = &VK_FAKEFAT32_MAL_DRV,
            .sector_size        = 512,
            .sector_number      = USRAPP_CFG_MSC_SIZE / 512,
            .sectors_per_cluster= 8,
            .volume_id          = 0x12345678,
            .disk_id            = 0x9ABCEF01,
            .root               = {
                .name           = "ROOT",
                .d.child        = (vk_memfs_file_t *)__fakefat32_root,
                .d.child_num    = dimof(__fakefat32_root),
            },
        },
#else
        .mem_mal                = {
            .drv                = &VK_MEM_MAL_DRV,
            .mem                = {
                .pchBuffer      = __usrapp.scsi.mem,
                .nSize          = sizeof(__usrapp.scsi.mem),
            },
            .blksz              = 512,
        },
#endif
        .mal_scsi               = {
            .drv                = &VK_VIRTUAL_SCSI_DRV,
            .param              = (void *)&__usrapp_const.scsi.param,
            .virtual_scsi_drv   = &VK_MAL_VIRTUAL_SCSI_DRV,
#if USRAPP_CFG_FAKEFAT32 == ENABLED
            .mal                = &__usrapp.scsi.fakefat32_mal.use_as__vk_mal_t,
#else
            .mal                = &__usrapp.scsi.mem_mal.use_as__vk_mal_t,
#endif
        },
    },

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
    .stream                     = {
        .mem_stream             = {
            .op                 = &vsf_mem_stream_op,
            .pchBuffer          = __usrapp.stream.buffer,
            .nSize              = sizeof(__usrapp.stream.buffer),
        },
    },
#endif
};

/*============================ IMPLEMENTATION ================================*/

static void __usrapp_on_timer(vsf_callback_timer_t *timer)
{
    vk_usbd_connect(&__usrapp.usbd.dev);
}

int main(void)
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_init(NULL);
    vsf_stdio_init();
#endif

    vk_usbd_init(&__usrapp.usbd.dev);
    vk_usbd_disconnect(&__usrapp.usbd.dev);
    __usrapp.usbd.connect_timer.on_timer = __usrapp_on_timer;
    vsf_callback_timer_add_ms(&__usrapp.usbd.connect_timer, 200);
    return 0;
}

/* EOF */
