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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_CDCACM == ENABLED

#include "../../../common/class/CDC/vsf_usb_CDCACM.h"
#include "./vsf_usbd_CDCACM_desc.h"

#if     defined(VSF_USBD_CDCACM_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSF_USBD_CDCACM_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

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

#define USB_CDC_ACM_IFS_NUM     USB_CDC_IFS_NUM
#define USB_CDC_ACM_IFS_CONTROL(__CDC_ACM_PARAM)                                \
            {                                                                   \
                .class_op       = &vk_usbd_cdcacm_control,                      \
                .class_param    = &__CDC_ACM_PARAM,                             \
            },
#define USB_CDC_ACM_IFS_DATA(__CDC_ACM_PARAM)                                   \
            {                                                                   \
                .class_op       = &vk_usbd_cdcacm_data,                         \
                .class_param    = &__CDC_ACM_PARAM,                             \
            },



#define __cdc_acm_desc(__NAME, __IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL)\
            USB_DESC_CDC_ACM_IAD((__IFS_START), (__I_FUNC), (__INT_IN_EP), (__BULK_IN_EP), (__BULK_OUT_EP), (__BULK_EP_SIZE), (__INT_EP_INTERVAL))

#define __cdc_acm_func(__NAME, __FUNC_ID, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __STREAM_RX, __STREAM_TX, ...)\
            vk_usbd_cdcacm_t __##__NAME##_CDCACM##__FUNC_ID = {                 \
                USB_CDC_ACM_PARAM((__INT_IN_EP), (__BULK_IN_EP), (__BULK_OUT_EP), (__STREAM_RX), (__STREAM_TX), __VA_ARGS__)\
            };

#define __cdc_acm_ifs(__NAME, __FUNC_ID)                                        \
                {                                                               \
                    USB_CDC_ACM_IFS_CONTROL(__##__NAME##_CDCACM##__FUNC_ID)     \
                },                                                              \
                {                                                               \
                    USB_CDC_ACM_IFS_DATA(__##__NAME##_CDCACM##__FUNC_ID)        \
                },

#define cdc_acm_desc(__NAME, __IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL)\
            __cdc_acm_desc(__NAME, (__IFS_START), 4 + (__I_FUNC), (__INT_IN_EP), (__BULK_IN_EP), (__BULK_OUT_EP), (__BULK_EP_SIZE), (__INT_EP_INTERVAL))
#define cdc_acm_func(__NAME, __FUNC_ID, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __STREAM_RX, __STREAM_TX, ...)\
            __cdc_acm_func(__NAME, __FUNC_ID, (__INT_IN_EP), (__BULK_IN_EP), (__BULK_OUT_EP), (__STREAM_RX), (__STREAM_TX), __VA_ARGS__)
#define cdc_acm_ifs(__NAME, __FUNC_ID)                                          \
            __cdc_acm_ifs(__NAME, __FUNC_ID)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_usbd_cdcacm_t)

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

struct vk_usbd_cdcacm_cfg_t {
    implement_ex(vk_usbd_ep_cfg_t, ep)
#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
    vsf_stream_t *tx_stream;
    vsf_stream_t *rx_stream;
#elif   VSF_USE_SERVICE_STREAM == ENABLED
    implement_ex(vsf_stream_usr_cfg_t, stream_usr);
    implement_ex(vsf_stream_src_cfg_t, stream_src);
#endif
};
typedef struct vk_usbd_cdcacm_cfg_t vk_usbd_cdcacm_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbd_class_op_t vk_usbd_cdcacm_control;
extern const vk_usbd_class_op_t vk_usbd_cdcacm_data;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_usbd_cdcacm_init(vk_usbd_cdcacm_t *obj, const vk_usbd_cdcacm_cfg_t *cfg);

#endif  // VSF_USE_USB_DEVICE && VSF_USE_USB_DEVICE_CDCACM
#endif	// __VSF_USBD_CDCACM_H__
