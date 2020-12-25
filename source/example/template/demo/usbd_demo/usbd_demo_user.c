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

#define __VSF_USBD_CLASS_INHERIT__
#include "vsf.h"

#if VSF_USE_USB_DEVICE == ENABLED && APP_USE_USBD_DEMO == ENABLED && APP_USE_USBD_USER_DEMO == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/

#ifndef USRAPP_CFG_USBD_SPEED
#   define USRAPP_CFG_USBD_SPEED            USB_DC_SPEED_HIGH
#endif

// __APP_CFG_CDC_BULK_SIZE is for internal usage
#if USRAPP_CFG_USBD_SPEED == USB_DC_SPEED_HIGH
#   define __APP_CFG_BULK_SIZE              512
#else
#   define __APP_CFG_BULK_SIZE              64
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_usbd_user_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbd_class_op_t __vk_usbd_user = {
    .init = __vk_usbd_user_init,
};

describe_usbd(__user_usbd, APP_CFG_USBD_VID, APP_CFG_USBD_PID, USRAPP_CFG_USBD_SPEED)
    usbd_common_desc(__user_usbd, u"VSF-USBD-User", u"SimonQian", u"1.0.0", 64, USB_DESC_IAD_LEN + USB_DESC_IFS_LEN + 2 * USB_DESC_EP_LEN, 1, USB_CONFIG_ATT_WAKEUP, 100)
        USB_DESC_IAD(0, 1, 0xFF, 0xFF, 0, 0)
        USB_DESC_IFS(0, 0, 2, 0xFF, 0xFF, 0, 0)
        USB_DESC_EP(USB_DIR_IN | 1, USB_ENDPOINT_XFER_BULK, __APP_CFG_BULK_SIZE, 0x00)
        USB_DESC_EP(USB_DIR_OUT | 1, USB_ENDPOINT_XFER_BULK, __APP_CFG_BULK_SIZE, 0x00)
    usbd_func_desc(__user_usbd)
        usbd_func_str_desc(__user_usbd, 0, u"VSF-User0")
    usbd_std_desc_table(__user_usbd)
        usbd_func_str_desc_table(__user_usbd, 0)
    usbd_func(__user_usbd)
        typedef struct vk_user_usbd_param_t {
            vk_usbd_dev_t *dev;
            vk_usbd_trans_t trans_tx;
            vk_usbd_trans_t trans_rx;
            uint8_t buffer[4096];
        } vk_user_usbd_param_t;
        static vk_user_usbd_param_t __user_usbd_param;
    usbd_ifs(__user_usbd)
        USB_IFS(&__vk_usbd_user, &__user_usbd_param)
end_describe_usbd(__user_usbd, VSF_USB_DC0)

/*============================ IMPLEMENTATION ================================*/

void __vk_usbd_user_on_tx(void *param)
{
    vk_user_usbd_param_t *user = param;

    user->trans_rx.buffer       = user->buffer;
    user->trans_rx.size         = sizeof(user->buffer);
    vk_usbd_ep_recv(user->dev, &user->trans_rx);
}

void __vk_usbd_user_on_rx(void *param)
{
    vk_user_usbd_param_t *user = param;

    user->trans_tx.buffer       = user->buffer;
    user->trans_tx.size         = sizeof(user->buffer) - user->trans_rx.size;
    vk_usbd_ep_send(user->dev, &user->trans_tx);
}

static vsf_err_t __vk_usbd_user_init(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs)
{
    vk_user_usbd_param_t *user = ifs->class_param;

    user->dev                   = dev;

    user->trans_tx.ep           = 0x81;
    user->trans_tx.on_finish    = __vk_usbd_user_on_tx;
    user->trans_tx.param        = user;

    user->trans_rx.ep           = 0x01;
    user->trans_rx.on_finish    = __vk_usbd_user_on_rx;
    user->trans_rx.param        = user;

    __vk_usbd_user_on_tx(user);
    return VSF_ERR_NONE;
}

#if APP_USE_LINUX_DEMO == ENABLED
int usbd_user_main(int argc, char *argv[])
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

    vk_usbd_init(&__user_usbd);
    vk_usbd_connect(&__user_usbd);
    return 0;
}

#endif
