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

#ifndef __VSF_USBD_CDCACM_H__
#define __VSF_USBD_CDCACM_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_CDCACM == ENABLED

#include "component/usb/common/class/CDC/vsf_usb_CDCACM.h"
#include "./vsf_usbd_CDCACM_desc.h"

#if     defined(__VSF_USBD_CDCACM_CLASS_IMPLEMENT)
#   undef __VSF_USBD_CDCACM_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define USB_CDC_ACM_PARAM(__INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __STREAM_RX, __STREAM_TX, ...)\
            .ep = {                                                             \
                .notify         = (__INT_IN_EP),                                \
                .out            = (__BULK_OUT_EP),                              \
                .in             = (__BULK_IN_EP),                               \
            },                                                                  \
            .line_coding        = __VA_ARGS__,                                  \
            .stream.tx.stream   = (vsf_stream_t *)(__STREAM_TX),                \
            .stream.rx.stream   = (vsf_stream_t *)(__STREAM_RX),

#define USB_CDC_ACM_PARITY_NONE 0
#define USB_CDC_ACM_PARITY_ODD  1
#define USB_CDC_ACM_PARITY_EVEN 2

#define USB_CDC_ACM_STOPBIT_1   0
#define USB_CDC_ACM_STOPBIT_1P5 1
#define USB_CDC_ACM_STOPBIT_2   2

#define USB_CDC_ACM_LINECODE(__BAUDRATE, __BITLEN, __PARITY, __STOP)            \
            {                                                                   \
                .bitrate        = (__BAUDRATE),                                 \
                .stop           = (__STOP),                                     \
                .parity         = (__PARITY),                                   \
                .datalen        = (__BITLEN),                                   \
            }
// commonly used line coding
#define USB_CDC_ACM_LINECODE_115200_8N1                                         \
            USB_CDC_ACM_LINECODE(115200, 8, USB_CDC_ACM_PARITY_NONE, USB_CDC_ACM_STOPBIT_1)

#define USB_CDC_ACM_IFS_NUM     USB_CDC_IFS_NUM
#define USB_CDC_ACM_IFS_CONTROL(__CDC_ACM_PARAM)                                \
            USB_IFS(&vk_usbd_cdcacm_control, &__CDC_ACM_PARAM)
#define USB_CDC_ACM_IFS_DATA(__CDC_ACM_PARAM)                                   \
            USB_IFS(&vk_usbd_cdcacm_data, &__CDC_ACM_PARAM)



#define __usbd_cdc_acm_desc_iad(__name, __ifs_start, __i_func,                  \
        __int_in_ep, __bulk_in_ep, __bulk_out_ep,                               \
        __bulk_ep_size, __int_ep_interval)                                      \
            USB_DESC_CDC_ACM_IAD((__ifs_start), (__i_func),                     \
                (__int_in_ep), (__bulk_in_ep), (__bulk_out_ep),                 \
                (__bulk_ep_size), (__int_ep_interval))

#define __usbd_cdc_acm_func(__name, __func_id, __str_func, __i_func, __ifs_start,\
        __int_in_ep, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __int_ep_interval,\
        __stream_rx, __stream_tx, ...)                                          \
            enum {                                                              \
                __##__name##_CDCACM##__func_id##_IFS_START  = (__ifs_start),    \
                __##__name##_CDCACM##__func_id##_I_FUNC     = (__i_func),       \
                __##__name##_CDCACM##__func_id##_INTIN_EP   = (__int_in_ep),    \
                __##__name##_CDCACM##__func_id##_BULKIN_EP  = (__bulk_in_ep),   \
                __##__name##_CDCACM##__func_id##_BULKOUT_EP = (__bulk_out_ep),  \
                __##__name##_CDCACM##__func_id##_BULK_EP_SIZE = (__bulk_ep_size),\
                __##__name##_CDCACM##__func_id##_BULK_EP_INTERVAL = (__int_ep_interval),\
            };                                                                  \
            usbd_func_str_desc(__name, __func_id, __str_func)                   \
            vk_usbd_cdcacm_t __##__name##_CDCACM##__func_id = {                 \
                USB_CDC_ACM_PARAM((__int_in_ep), (__bulk_in_ep), (__bulk_out_ep),\
                (__stream_rx), (__stream_tx), __VA_ARGS__)                      \
            };

#define __usbd_cdc_acm_ifs(__name, __func_id)                                   \
            USB_CDC_ACM_IFS_CONTROL(__##__name##_CDCACM##__func_id)             \
            USB_CDC_ACM_IFS_DATA(__##__name##_CDCACM##__func_id)

#define usbd_cdc_acm_desc_iad(__name, __func_id)                                \
            __usbd_cdc_acm_desc_iad(__name,                                     \
                __##__name##_CDCACM##__func_id##_IFS_START,                     \
                4 + __##__name##_CDCACM##__func_id##_I_FUNC,                    \
                __##__name##_CDCACM##__func_id##_INTIN_EP,                      \
                __##__name##_CDCACM##__func_id##_BULKIN_EP,                     \
                __##__name##_CDCACM##__func_id##_BULKOUT_EP,                    \
                __##__name##_CDCACM##__func_id##_BULK_EP_SIZE,                  \
                __##__name##_CDCACM##__func_id##_BULK_EP_INTERVAL)
#define usbd_cdc_acm_desc_table(__name, __func_id)                              \
            usbd_func_str_desc_table(__name, __func_id)
#define usbd_cdc_acm_func(__name, __func_id, __str_func, __i_func, __ifs_start, \
        __int_in_ep, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __int_ep_interval,\
        __stream_rx, __stream_tx, ...)                                          \
            __usbd_cdc_acm_func(__name, __func_id, (__str_func), (__i_func), (__ifs_start),\
                (__int_in_ep), (__bulk_in_ep), (__bulk_out_ep), (__bulk_ep_size), (__int_ep_interval),\
                (__stream_rx), (__stream_tx), __VA_ARGS__)
#define usbd_cdc_acm_ifs(__name, __func_id)                                     \
            __usbd_cdc_acm_ifs(__name, __func_id)

#define usbd_cdcacm_desc_iad(...)           usbd_cdc_acm_desc_iad(__VA_ARGS__)
#define usbd_cdcacm_desc_table(...)         usbd_cdc_acm_desc_table(__VA_ARGS__)
#define usbd_cdcacm_func(...)               usbd_cdc_acm_func(__VA_ARGS__)
#define usbd_cdcacm_ifs(...)                usbd_cdc_acm_ifs(__VA_ARGS__)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_usbd_cdcacm_t) {

    public_member(
        implement(vk_usbd_cdc_t)

        usb_cdcacm_line_coding_t line_coding;
        uint8_t control_line;

        struct {
            vsf_err_t (*set_line_coding)(usb_cdcacm_line_coding_t *line_coding);
            vsf_err_t (*set_control_line)(uint8_t control_line);
            vsf_err_t (*get_control_line)(uint8_t *control_line);
            vsf_err_t (*send_break)(void);
        } callback;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbd_class_op_t vk_usbd_cdcacm_control;
extern const vk_usbd_class_op_t vk_usbd_cdcacm_data;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_USB_DEVICE && VSF_USBD_USE_CDCACM
#endif  // __VSF_USBD_CDCACM_H__
