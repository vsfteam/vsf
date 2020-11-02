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

#if USRAPP_CFG_USBD_SPEED == USB_SPEED_HIGH
#   define USRAPP_CFG_USBD_DC_EPSIZE            512
#   define USRAPP_CFG_USBD_DC_SPEED             USB_DC_SPEED_HIGH
#elif USRAPP_CFG_USBD_SPEED == USB_SPEED_FULL
#   define USRAPP_CFG_USBD_DC_EPSIZE            64
#   define USRAPP_CFG_USBD_DC_SPEED             USB_DC_SPEED_FULL
#endif

/*============================ INCLUDES ======================================*/

#if USRAPP_CFG_FAKEFAT32 == ENABLED
#   include "fakefat32.c"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_const_t {
#if     USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG                       \
    ||  USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
    struct {
#   if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        vk_dwcotg_dcd_param_t dwcotg_param;
#   elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        vk_musb_fdrc_dcd_param_t musb_fdrc_param;
#   endif
    } usbd;
#endif

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

#if VSF_USE_SIMPLE_STREAM == ENABLED
    struct {
        uint8_t buffer[1024];
        vsf_mem_stream_t mem_stream;
    } stream;
#endif
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static void __usrapp_on_timer(vsf_callback_timer_t *timer);

/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t __usrapp_const = {
#if     USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG                       \
    ||  USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
    .usbd                       = {
#   if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
        .dwcotg_param           = {
            .op                 = &VSF_USB_DC0_IP,
            .speed              = USRAPP_CFG_USBD_SPEED,
        },
#   elif USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_MUSB_FDRC
        musb_fdrc_param         = {
            .op                 = &VSF_USB_DC0_IP,
        },
#   endif
    },
#endif

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
        .connect_timer.on_timer = __usrapp_on_timer,
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
                .buffer         = __usrapp.scsi.mem,
                .size           = sizeof(__usrapp.scsi.mem),
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

#if VSF_USE_SIMPLE_STREAM == ENABLED
    .stream                     = {
        .mem_stream             = {
            .op                 = &vsf_mem_stream_op,
            .buffer             = __usrapp.stream.buffer,
            .size               = sizeof(__usrapp.stream.buffer),
        },
    },
#endif
};

describe_usbd(user_usbd, APP_CFG_USBD_PID, APP_CFG_USBD_VID, USRAPP_CFG_USBD_DC_SPEED)
    usbd_common_desc(user_usbd, u"VSF-MSC", u"SimonQian", u"1.0.0", 64, USB_DESC_MSCBOT_IAD_LEN, USB_MSCBOT_IFS_NUM, USB_CONFIG_ATT_WAKEUP, 100)
        mscbot_desc(user_usbd, 0, 0, 1, 1, USRAPP_CFG_USBD_DC_EPSIZE)
    usbd_func_desc(user_usbd)
        usbd_func_str_desc(user_usbd, 0, u"VSF-MSC")
    usbd_std_desc_table(user_usbd)
        usbd_func_str_desc_table(user_usbd, 0)
    usbd_func(user_usbd)
        mscbot_func(user_usbd, 0, 1, 1, 1,
            &__usrapp.scsi.mal_scsi.use_as__vk_scsi_t,
            &__usrapp.stream.mem_stream.use_as__vsf_stream_t)
    usbd_ifs(user_usbd)
        mscbot_ifs(user_usbd, 0)
end_describe_usbd(user_usbd, VSF_USB_DC0)

/*============================ IMPLEMENTATION ================================*/

static void __usrapp_on_timer(vsf_callback_timer_t *timer)
{
    vk_usbd_connect(&user_usbd);
}

int main(void)
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_init((vsf_stream_t *)&VSF_DEBUG_STREAM_TX);
    vsf_stdio_init();
#endif

    vk_usbd_init(&user_usbd);
    vk_usbd_disconnect(&user_usbd);
    vsf_callback_timer_add_ms(&__usrapp.usbd.connect_timer, 200);
    return 0;
}

/* EOF */
