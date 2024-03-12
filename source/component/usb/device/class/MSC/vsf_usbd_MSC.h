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

#ifndef __VSF_USBD_MSC_H__
#define __VSF_USBD_MSC_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_MSC == ENABLED

#include "component/usb/common/class/MSC/vsf_usb_MSC.h"
#include "./vsf_usbd_MSC_desc.h"
#include "component/scsi/vsf_scsi.h"

#if     defined(__VSF_USBD_MSC_CLASS_IMPLEMENT)
#   undef __VSF_USBD_MSC_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_SCSI != ENABLED
#   error msc uses scsi!!!
#endif
#if VSF_USBD_CFG_STREAM_EN != ENABLED
#   error msc need stream
#endif



#define USB_MSC_PARAM(__BULK_IN_EP, __BULK_OUT_EP, __STREAM, __MAX_LUN, __SCSI_PDEVS)\
            .ep_out             = (__BULK_OUT_EP),                              \
            .ep_in              = (__BULK_IN_EP),                               \
            .stream             = (__STREAM),                                   \
            .max_lun            = (__MAX_LUN),                                  \
            .scsi_devs          = (__SCSI_PDEVS),

#define USB_MSC_IFS_NUM             1
#define USB_MSCBOT_IFS_NUM          USB_MSC_IFS_NUM
#define USB_MSC_IFS(__MSC_PARAM)    USB_IFS(&vk_usbd_msc, &(__MSC_PARAM))


#define __usbd_mscbot_desc(__name, __ifs, __i_func, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size)\
            USB_DESC_MSCBOT((__ifs), 4 + (__i_func), (__bulk_in_ep), (__bulk_out_ep), (__bulk_ep_size))
#define __usbd_mscbot_desc_iad(__name, __ifs, __i_func, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size)\
            USB_DESC_MSCBOT_IAD((__ifs), 4 + (__i_func), (__bulk_in_ep), (__bulk_out_ep), (__bulk_ep_size))

#define __usbd_mscbot_func(__name, __func_id, __str_func, __i_func, __ifs,      \
        __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __stream, ...)             \
            enum {                                                              \
                __##__name##_MSCBOT##__func_id##_IFS        = (__ifs),          \
                __##__name##_MSCBOT##__func_id##_I_FUNC     = (__i_func),       \
                __##__name##_MSCBOT##__func_id##_BULKIN_EP  = (__bulk_in_ep),   \
                __##__name##_MSCBOT##__func_id##_BULKOUT_EP = (__bulk_out_ep),  \
                __##__name##_MSCBOT##__func_id##_BULK_EP_SIZE = (__bulk_ep_size),\
            };                                                                  \
            usbd_func_str_desc(__name, __func_id, __str_func)                   \
            vk_scsi_t * __##__name##_MSC##__scsi_devs[] = {                     \
                __VA_ARGS__                                                     \
            };                                                                  \
            vk_usbd_msc_t __##__name##_MSC##__func_id = {                       \
                USB_MSC_PARAM((__bulk_in_ep), (__bulk_out_ep), (__stream),      \
                    dimof(__##__name##_MSC##__scsi_devs) - 1,                   \
                    __##__name##_MSC##__scsi_devs)                              \
            };

#define __usbd_msc_ifs(__name, __func_id)                                       \
            USB_MSC_IFS(__##__name##_MSC##__func_id)

#define usbd_mscbot_desc(__name, __func_id)                                     \
            __usbd_mscbot_desc(__name,                                          \
                __##__name##_MSCBOT##__func_id##_IFS,                           \
                __##__name##_MSCBOT##__func_id##_I_FUNC,                        \
                __##__name##_MSCBOT##__func_id##_BULKIN_EP,                     \
                __##__name##_MSCBOT##__func_id##_BULKOUT_EP,                    \
                __##__name##_MSCBOT##__func_id##_BULK_EP_SIZE)
#define usbd_mscbot_desc_iad(__name, __func_id)                                 \
            __usbd_mscbot_desc_iad(__name,                                      \
                __##__name##_MSCBOT##__func_id##_IFS,                           \
                __##__name##_MSCBOT##__func_id##_I_FUNC,                        \
                __##__name##_MSCBOT##__func_id##_BULKIN_EP,                     \
                __##__name##_MSCBOT##__func_id##_BULKOUT_EP,                    \
                __##__name##_MSCBOT##__func_id##_BULK_EP_SIZE)
#define usbd_mscbot_func(__name, __func_id, __str_func, __i_func, __ifs,        \
        __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __stream, ...)             \
            __usbd_mscbot_func(__name, __func_id, (__str_func), (__i_func), (__ifs),\
                (__bulk_in_ep), (__bulk_out_ep), (__bulk_ep_size),              \
                (__stream), __VA_ARGS__)
#define usbd_mscbot_ifs(__name, __func_id)                                      \
            __usbd_msc_ifs(__name, __func_id)

// usbd_mscbot_scsi_config MUST be called for each scsi device before usbd startup.
#define usbd_mscbot_scsi_config(__name, __func_id, __scsi_idx, __is_inited)\
            extern vk_usbd_msc_t __##__name##_MSC##__func_id;                   \
            vk_usbd_mscbot_scsi_config(&__##__name##_MSC##__func_id, (__scsi_idx), (__is_inited))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef union vk_usbd_msc_scsi_ctx_t {
    usb_msc_cbw_t cbw;
    struct {
        usb_msc_csw_t csw;
        struct {
            uint32_t cur_size;
            uint32_t reply_size;
        };
    };
} vk_usbd_msc_scsi_ctx_t;

vsf_class(vk_usbd_msc_t) {

    private_member(
        vsf_eda_t eda;
        vk_usbd_msc_scsi_ctx_t ctx;
        int32_t reply_len;
        vk_usbd_dev_t *dev;
        vk_usbd_ep_stream_t ep_stream;
        uint8_t scsi_inited_mask;
        uint8_t is_scsi_init    : 1;
        uint8_t is_stream       : 1;
        uint8_t is_scsi_done    : 1;
        uint8_t is_data_done    : 1;
    )

    public_member(
        const uint8_t ep_out;
        const uint8_t ep_in;
        const uint8_t max_lun;
        vsf_stream_t *stream;
        vk_scsi_t **scsi_devs;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbd_class_op_t vk_usbd_msc;

/*============================ PROTOTYPES ====================================*/

// vk_usbd_mscbot_scsi_config MUST be called for each scsi device before usbd startup.
void vk_usbd_mscbot_scsi_config(vk_usbd_msc_t *msc, uint8_t idx, bool is_inited);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_MSC
#endif      // __VSF_USBD_MSC_H__
