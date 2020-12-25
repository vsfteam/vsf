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

#if VSF_USE_USB_DEVICE == ENABLED && APP_USE_USBD_DEMO == ENABLED && APP_USE_USBD_CDC_DEMO == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/

#if !((   (APP_USE_LINUX_DEMO != ENABLED)                                   \
        ||  (   (APP_USE_LINUX_DEMO == ENABLED)                             \
            &&  (USRAPP_CFG_LINUX_TTY != USRAPP_CFG_LINUX_TTY_CDC))))
#   error configuration not supported
#endif

#ifndef USRAPP_CFG_USBD_SPEED
#   define USRAPP_CFG_USBD_SPEED            USB_DC_SPEED_HIGH
#endif

// __APP_CFG_CDC_BULK_SIZE is for internal usage
#if USRAPP_CFG_USBD_SPEED == USB_DC_SPEED_HIGH
#   define __APP_CFG_CDC_BULK_SIZE          512
#else
#   define __APP_CFG_CDC_BULK_SIZE          64
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

describe_block_stream(__user_usbd_cdc_stream0, 2, __APP_CFG_CDC_BULK_SIZE)

describe_usbd(__user_usbd_cdc, APP_CFG_USBD_VID, APP_CFG_USBD_PID, USRAPP_CFG_USBD_SPEED)
    usbd_common_desc(__user_usbd_cdc, u"VSF-USBD-Simplest", u"SimonQian", u"1.0.0", 64, USB_DESC_CDC_ACM_IAD_LEN, USB_CDC_ACM_IFS_NUM, USB_CONFIG_ATT_WAKEUP, 100)
        cdc_acm_desc(__user_usbd_cdc, 0 * USB_CDC_ACM_IFS_NUM, 0, 1, 2, 2, __APP_CFG_CDC_BULK_SIZE, 16)
    usbd_func_desc(__user_usbd_cdc)
        usbd_func_str_desc(__user_usbd_cdc, 0, u"VSF-CDC0")
    usbd_std_desc_table(__user_usbd_cdc)
        usbd_func_str_desc_table(__user_usbd_cdc, 0)
    usbd_func(__user_usbd_cdc)
        cdc_acm_func(__user_usbd_cdc, 0, 1, 2, 2, &__user_usbd_cdc_stream0, &__user_usbd_cdc_stream0, USB_CDC_ACM_LINECODE(115200, 8, USB_CDC_ACM_PARITY_NONE, USB_CDC_ACM_STOPBIT_1))
    usbd_ifs(__user_usbd_cdc)
        cdc_acm_ifs(__user_usbd_cdc, 0)
end_describe_usbd(__user_usbd_cdc, VSF_USB_DC0)

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if APP_USE_LINUX_DEMO == ENABLED
int usbd_cdc_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

    VSF_STREAM_INIT(&__user_usbd_cdc_stream0);
    vk_usbd_init(&__user_usbd_cdc);
    vk_usbd_connect(&__user_usbd_cdc);
    return 0;
}

#endif
