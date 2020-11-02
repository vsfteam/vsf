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

#ifndef __VSF_USBD_CDCACM_H__
#define __VSF_USBD_CDCACM_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_CDCACM == ENABLED

#include "component/usb/common/class/CDC/vsf_usb_CDCACM.h"
#include "./vsf_usbd_CDCACM_desc.h"

#if     defined(__VSF_USBD_CDCACM_CLASS_IMPLEMENT)
#   undef __VSF_USBD_CDCACM_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
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



#define __cdc_acm_desc(__name, __ifs_start, __i_func, __int_in_ep, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __int_ep_interval)\
            USB_DESC_CDC_ACM_IAD((__ifs_start), (__i_func), (__int_in_ep), (__bulk_in_ep), (__bulk_out_ep), (__bulk_ep_size), (__int_ep_interval))

#define __cdc_acm_func(__name, __func_id, __int_in_ep, __bulk_in_ep, __bulk_out_ep, __stream_rx, __stream_tx, ...)\
            vk_usbd_cdcacm_t __##__name##_CDCACM##__func_id = {                 \
                USB_CDC_ACM_PARAM((__int_in_ep), (__bulk_in_ep), (__bulk_out_ep), (__stream_rx), (__stream_tx), __VA_ARGS__)\
            };

#define __cdc_acm_ifs(__name, __func_id)                                        \
            USB_CDC_ACM_IFS_CONTROL(__##__name##_CDCACM##__func_id)             \
            USB_CDC_ACM_IFS_DATA(__##__name##_CDCACM##__func_id)

#define cdc_acm_desc(__name, __ifs_start, __i_func, __int_in_ep, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __int_ep_interval)\
            __cdc_acm_desc(__name, (__ifs_start), 4 + (__i_func), (__int_in_ep), (__bulk_in_ep), (__bulk_out_ep), (__bulk_ep_size), (__int_ep_interval))
#define cdc_acm_func(__name, __func_id, __int_in_ep, __bulk_in_ep, __bulk_out_ep, __stream_rx, __stream_tx, ...)\
            __cdc_acm_func(__name, __func_id, (__int_in_ep), (__bulk_in_ep), (__bulk_out_ep), (__stream_rx), (__stream_tx), __VA_ARGS__)
#define cdc_acm_ifs(__name, __func_id)                                          \
            __cdc_acm_ifs(__name, __func_id)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_usbd_cdcacm_t)

def_simple_class(vk_usbd_cdcacm_t) {

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

typedef struct vk_usbd_cdcacm_cfg_t {
    implement_ex(vk_usbd_cdc_ep_t, ep);
#if     VSF_USE_SIMPLE_STREAM == ENABLED
    vsf_stream_t *tx_stream;
    vsf_stream_t *rx_stream;
#elif   VSF_USE_STREAM == ENABLED
    implement_ex(vsf_stream_usr_cfg_t, stream_usr);
    implement_ex(vsf_stream_src_cfg_t, stream_src);
#endif
} vk_usbd_cdcacm_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbd_class_op_t vk_usbd_cdcacm_control;
extern const vk_usbd_class_op_t vk_usbd_cdcacm_data;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_usbd_cdcacm_init(vk_usbd_cdcacm_t *obj, const vk_usbd_cdcacm_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_USB_DEVICE && VSF_USBD_USE_CDCACM
#endif	// __VSF_USBD_CDCACM_H__
