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

#ifndef __VSF_USBD_DFU_H__
#define __VSF_USBD_DFU_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DFU == ENABLED

#include "../../../common/class/DFU/vsf_usb_DFU.h"
#include "./vsf_usbd_DFU_desc.h"

#if     defined(__VSF_USBD_DFU_CLASS_IMPLEMENT)
#   undef __VSF_USBD_DFU_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_USBD_DFU_CLASS_INHERIT__)
#   undef __VSF_USBD_DFU_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define USB_DFU_IFS_NUM             1

#define __usbd_dfu_desc_iad(__name, __ifs_start, __i_func, __protocol, __dfu_attr, __detach_timeout, __transfer_size)\
            USB_DESC_DFU_IAD((__ifs_start), (__i_func), (__protocol), (__dfu_attr), (__detach_timeout), (__transfer_size))
#define __usbd_dfu_desc(__name, __ifs_start, __i_func, __protocol, __dfu_attr, __detach_timeout, __transfer_size)\
            USB_DESC_DFU((__ifs_start), (__i_func), (__protocol), (__dfu_attr), (__detach_timeout), (__transfer_size))

#define USB_DFU_PARAM(__PROTOCOL, __DFU_ATTR, __TRANSFER_SIZE, __TRANSFER_BUFFER)\
            .protocol = (__PROTOCOL),                                           \
            .attribute = (__DFU_ATTR),                                          \
            .transfer_size = (__TRANSFER_SIZE),                                 \
            .transfer_buffer = (__TRANSFER_BUFFER),

#define __usbd_dfu_func1(__name, __func_id, __protocol, __dfu_attr, __transfer_size, __transfer_buffer)\
            vk_usbd_dfu_t __##__name##_DFU##__func_id = {                       \
                USB_DFU_PARAM((__protocol), (__dfu_attr), (__transfer_size), (__transfer_buffer))\
            };
#define __usbd_dfu_func0(__name, __func_id, __protocol, __dfu_attr, __transfer_size)\
            static uint8_t __##__name##_DFU##__transfer_buffer[__transfer_size];\
            __usbd_dfu_func1(__name, __func_id, (__protocol), (__dfu_attr), (__transfer_size), &__##__name##_DFU##__transfer_buffer[__transfer_size])

#define __usbd_dfu_func(__name, __func_id, __protocol, __dfu_attr, __transfer_size, ...)\
            __PLOOC_EVAL(__usbd_dfu_func, __VA_ARGS__)(__name, __func_id, (__protocol), (__dfu_attr), (__transfer_size), ##__VA_ARGS__)

#define __usbd_dfu_ifs(__name, __func_id)                                       \
            USB_IFS(&vk_usbd_dfu, &__##__name##_DFU##__func_id)

#define usbd_dfu_desc(__name, __ifs_start, __i_func, __protocol, __dfu_attr, __detach_timeout, __transfer_size)\
            __usbd_dfu_desc(__name, (__ifs_start), __i_func, (__protocol), (__dfu_attr), (__detach_timeout), (__transfer_size))
#define usbd_dfu_desc_iad(__name, __ifs_start, __i_func, __protocol, __dfu_attr, __detach_timeout, __transfer_size)\
            __usbd_dfu_desc_iad(__name, (__ifs_start), __i_func, (__protocol), (__dfu_attr), (__transfer_size))
#define usbd_dfu_func(__name, __func_id, __protocol, __dfu_attr, __transfer_size, ...)\
            __usbd_dfu_func(__name, __func_id, (__protocol), (__dfu_attr), (__transfer_size), ##__VA_ARGS__)
#define usbd_dfu_ifs(__name, __func_id)                                         \
            __usbd_dfu_ifs(__name, __func_id)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_usbd_dfu_t) {
    public_member(
        uint8_t protocol;
        uint8_t attribute;
        uint16_t transfer_size;
        uint8_t *transfer_buffer;
    )
    private_member(
        usb_dfu_status_t status;
        uint16_t block_idx;
        uint16_t addr;
        uint16_t cur_size;

        vk_usbd_dev_t *dev;
        vk_usbd_ifs_t *ifs;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbd_class_op_t vk_usbd_dfu;

/*============================ PROTOTYPES ====================================*/

extern void vk_usbd_dfu_downloaded(vk_usbd_dfu_t *dfu, vsf_err_t result);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DFU
#endif      // __VSF_USBD_DFU_H__
