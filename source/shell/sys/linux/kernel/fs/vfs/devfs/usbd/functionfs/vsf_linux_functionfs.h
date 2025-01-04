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

#ifndef __VSF_LINUX_FUNCTIONFS_INTERNAL_H__
#define __VSF_LINUX_FUNCTIONFS_INTERNAL_H__

/*============================ INCLUDES ======================================*/

#include "../../../../vsf_linux_cfg.h"

#if     VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_DEVFS == ENABLED              \
    &&  VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_CFG_RAW_MODE != ENABLED

#include "component/usb/device/vsf_usbd.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_usbd_ifs_t vsf_linux_usbd_ifs_t;
typedef struct vsf_linux_usbd_ep_t {
    // ep_no/zlp/mts and mts MUST be initialized by user
    uint8_t ep_no;
    bool zlp;
    uint32_t mts;

    // private member, don't touch
    uint32_t cur_pos;
    vsf_linux_fd_op_t op;
    vsf_linux_usbd_ifs_t *ifs;
} vsf_linux_usbd_ep_t;

typedef struct vsf_linux_usbd_ifs_t {
    vk_usbd_dev_t *dev;
    vk_usbd_ifs_t *ifs;

    vk_usbd_desc_t * (*get_desc)(vk_usbd_dev_t *dev, vsf_linux_usbd_ifs_t *ifs,
            uint_fast8_t type, uint_fast8_t index, uint_fast16_t langid);
    vsf_err_t (*request_prepare)(vk_usbd_dev_t *dev, vsf_linux_usbd_ifs_t *ifs, struct usb_ctrlrequest_t *request);
    vsf_err_t (*request_process)(vk_usbd_dev_t *dev, vsf_linux_usbd_ifs_t *ifs, struct usb_ctrlrequest_t *request);
} vsf_linux_usbd_ifs_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

// ep0 will be binded under ifs_path if success
extern int vsf_linux_fs_bind_usbd_ifs(char *ifs_path, vsf_linux_usbd_ifs_t *ifs);

// for IN ep, msb of ep_no is 1
// for OUT ep, msb of ep_no is 0
// ep 0 is bi-directional
// no need to bind ep0, it will be binded in vsf_linux_fs_bind_usbd_ifs
extern int vsf_linux_fs_bind_usbd_ep(char *ifs_path, vsf_linux_usbd_ep_t *ep);

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_DEVFS && VSF_USE_USB_DEVICE && !VSF_USBD_CFG_RAW_MODE
#endif      // __VSF_LINUX_FUNCTIONFS_INTERNAL_H__
/* EOF */