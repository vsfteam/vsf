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

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "usrapp_common.h"

#ifdef MSCBOOT_CFG_INCLUDE
#   include MSCBOOT_CFG_INCLUDE
#endif

/*============================ MACROS ========================================*/

#ifndef USRAPP_CFG_USBD_SPEED
#   define USRAPP_CFG_USBD_SPEED            USB_DC_SPEED_HIGH
#endif

// __APP_CFG_MSC_BULK_SIZE is for internal usage
#if USRAPP_CFG_USBD_SPEED == USB_DC_SPEED_HIGH
#   define __APP_CFG_MSC_BULK_SIZE          512
#else
#   define __APP_CFG_MSC_BULK_SIZE          64
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

describe_mem_stream(__user_usbd_msc_stream, 1024)
static const vk_virtual_scsi_param_t __usrapp_scsi_param = {
    .block_size             = USRAPP_CFG_FAKEFAT32_SECTOR_SIZE,
    .block_num              = USRAPP_CFG_FAKEFAT32_SIZE / USRAPP_CFG_FAKEFAT32_SECTOR_SIZE,
    .vendor                 = "Simon   ",
    .product                = "VSFDriver       ",
    .revision               = "1.00",
    .type                   = SCSI_PDT_DIRECT_ACCESS_BLOCK,
};
static vk_mal_scsi_t __usrapp_mal_scsi = {
    .drv                = &vk_virtual_scsi_drv,
    .param              = (void *)&__usrapp_scsi_param,
    .virtual_scsi_drv   = &vk_mal_virtual_scsi_drv,
    .mal                = &usrapp_common.mal.fakefat32.use_as__vk_mal_t,
};

describe_usbd(__user_usbd_msc, APP_CFG_USBD_VID, APP_CFG_USBD_PID, USRAPP_CFG_USBD_SPEED)
    usbd_common_desc_iad(__user_usbd_msc,
                        // str_product, str_vendor, str_serial
                        u"VSF-USBD-Simplest", u"SimonQian", u"1.0.0",
                        // ep0_size
                        64,
                        // total function descriptor size
                        USB_DESC_MSCBOT_IAD_LEN,
                        // total function interface number
                        USB_MSCBOT_IFS_NUM,
                        // attribute, max_power
                        USB_CONFIG_ATT_WAKEUP, 100
    )
        usbd_mscbot_desc_iad(__user_usbd_msc,
                        // interface
                        0,
                        // function string index(start from 0)
                        0,
                        // bulk in ep, bulk out ep
                        1, 1,
                        // bulk ep size
                        __APP_CFG_MSC_BULK_SIZE
        )
    usbd_func_desc(__user_usbd_msc)
        usbd_func_str_desc(__user_usbd_msc, 0, u"VSF-MSC0")
    usbd_std_desc_table(__user_usbd_msc)
        usbd_func_str_desc_table(__user_usbd_msc, 0)
    usbd_func(__user_usbd_msc)
        usbd_mscbot_func(__user_usbd_msc,
                        // function index
                        0,
                        // bulk in ep, bulk out ep
                        1, 1,
                        // max lun(logic unit number)
                        0,
                        // scsi_dev
                        &__usrapp_mal_scsi.use_as__vk_scsi_t,
                        // stream
                        &__user_usbd_msc_stream.use_as__vsf_stream_t
        )
    usbd_ifs(__user_usbd_msc)
        usbd_mscbot_ifs(__user_usbd_msc, 0)
end_describe_usbd(__user_usbd_msc, VSF_USB_DC0)

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#ifdef mscboot_implement
mscboot_implement()
#endif

int VSF_USER_ENTRY(void)
{
#if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#endif

    mscboot_init();
    if (mscboot_check()) {
        vk_usbd_init(&__user_usbd_msc);
        vk_usbd_connect(&__user_usbd_msc);
    } else {
        mscboot_fini();
        mscboot_boot();
    }
    return 0;
}

int vsf_stdout_putchar(char ch)
{
    return 0;
}
