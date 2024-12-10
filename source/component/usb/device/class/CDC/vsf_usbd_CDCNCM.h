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

#ifndef __VSF_USBD_CDCNCM_H__
#define __VSF_USBD_CDCNCM_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_CDCNCM == ENABLED

#include "component/tcpip/vsf_tcpip.h"
#include "component/usb/common/class/CDC/vsf_usb_CDCECM.h"
#include "component/usb/common/class/CDC/vsf_usb_CDCNCM.h"
#include "./vsf_usbd_CDCNCM_desc.h"

#if     defined(__VSF_USBD_CDCNCM_CLASS_IMPLEMENT)
#   undef __VSF_USBD_CDCNCM_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_TCPIP != ENABLED
#   error Please enable VSF_USE_TCPIP to use CDCNCM
#endif

#ifndef VSF_USBD_CDCNCM_SUPPORT_NTB32
#   define VSF_USBD_CDCNCM_SUPPORT_NTB32                ENABLED
#endif
#ifndef VSF_USBD_CDCNCM_CFG_MAX_NTB_IN_SIZE
#   define VSF_USBD_CDCNCM_CFG_MAX_NTB_IN_SIZE          (4 * 1024)
#endif
#ifndef VSF_USBD_CDCNCM_CFG_MAX_NTB_OUT_SIZE
#   define VSF_USBD_CDCNCM_CFG_MAX_NTB_OUT_SIZE         (4 * 1024)
#endif

#define USB_CDC_NCM_PARAM(__INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP)             \
            .ep = {                                                             \
                .notify         = (__INT_IN_EP),                                \
                .out            = (__BULK_OUT_EP),                              \
                .in             = (__BULK_IN_EP),                               \
            },

#define USB_CDC_NCM_IFS_NUM     USB_CDC_IFS_NUM
#define USB_CDC_NCM_IFS_CONTROL(__CDC_NCM_PARAM)                                \
            USB_IFS(&vk_usbd_cdcncm_control, &__CDC_NCM_PARAM)
#define USB_CDC_NCM_IFS_DATA(__CDC_NCM_PARAM)                                   \
            USB_IFS(&vk_usbd_cdcncm_data, &__CDC_NCM_PARAM)



#define __usbd_cdc_ncm_desc_iad(__name, __ifs_start, __i_func,                  \
        __int_in_ep, __bulk_in_ep, __bulk_out_ep,                               \
        __bulk_ep_size, __int_ep_interval, __i_mac, __max_datagram_size, ...)   \
            USB_DESC_CDC_NCM_IAD((__ifs_start), (__i_func),                     \
                (__int_in_ep), (__bulk_in_ep), (__bulk_out_ep),                 \
                (__bulk_ep_size), (__int_ep_interval), (__i_mac),               \
                (__max_datagram_size), ##__VA_ARGS__)

#define __usbd_cdc_ncm_func1(__name, __func_id, __str_func, __i_func, __ifs_start,\
        __int_in_ep, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __int_ep_interval,\
        __i_mac, __str_mac, __max_datagram_size)                                \
            enum {                                                              \
                __##__name##_CDCNCM##__func_id##_IFS_START  = (__ifs_start),    \
                __##__name##_CDCNCM##__func_id##_I_FUNC     = (__i_func),       \
                __##__name##_CDCNCM##__func_id##_INTIN_EP   = (__int_in_ep),    \
                __##__name##_CDCNCM##__func_id##_BULKIN_EP  = (__bulk_in_ep),   \
                __##__name##_CDCNCM##__func_id##_BULKOUT_EP = (__bulk_out_ep),  \
                __##__name##_CDCNCM##__func_id##_BULK_EP_SIZE = (__bulk_ep_size),\
                __##__name##_CDCNCM##__func_id##_BULK_EP_INTERVAL = (__int_ep_interval),\
                __##__name##_CDCNCM##__func_id##_I_MAC      = (__i_mac),        \
                __##__name##_CDCNCM##__func_id##_MAX_DATAGRAM_SIZE = (__max_datagram_size),\
            };                                                                  \
            usbd_func_str_desc(__name, __func_id, __str_func)                   \
            usbd_func_str_desc(__name, __func_id ## _mac, __str_mac)            \
            vk_usbd_cdcncm_t __##__name##_CDCNCM##__func_id = {                 \
                USB_CDC_NCM_PARAM((__int_in_ep), (__bulk_in_ep), (__bulk_out_ep))\
                .max_datagram_size = (__max_datagram_size),                     \
                    .str_mac = (__##__name##_str_##func##__func_id##_mac.str),  \
            };
#define __usbd_cdc_ncm_func0(__name, __func_id, __str_func, __i_func, __ifs_start,\
        __int_in_ep, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __int_ep_interval,\
        __i_mac, __str_mac)                                                     \
            __usbd_cdc_ncm_func1(__name, __func_id, __str_func, __i_func, __ifs_start,\
                __int_in_ep, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __int_ep_interval,\
                __i_mac, __str_mac, (14 + 1500))
#define __usbd_cdc_ncm_func(__name, __func_id, __str_func, __i_func, __ifs_start,\
        __int_in_ep, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __int_ep_interval,\
        __i_mac, __str_mac, ...)                                                \
            __PLOOC_EVAL(__usbd_cdc_ncm_func, __VA_ARGS__)(__name, __func_id, __str_func, __i_func, __ifs_start,\
                __int_in_ep, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __int_ep_interval,\
                __i_mac, __str_mac, ##__VA_ARGS__)

#define __usbd_cdc_ncm_ifs(__name, __func_id)                                   \
            USB_CDC_NCM_IFS_CONTROL(__##__name##_CDCNCM##__func_id)             \
            USB_CDC_NCM_IFS_DATA(__##__name##_CDCNCM##__func_id)

// prototype: usbd_cdc_ncm_desc_iad(__name, __func_id,                          \
            __network_cap = 0, __multicast_filter_num = 0, __power_filter_num = 0)
#define usbd_cdc_ncm_desc_iad(__name, __func_id, ...)                           \
            __usbd_cdc_ncm_desc_iad(__name,                                     \
                __##__name##_CDCNCM##__func_id##_IFS_START,                     \
                4 + __##__name##_CDCNCM##__func_id##_I_FUNC,                    \
                __##__name##_CDCNCM##__func_id##_INTIN_EP,                      \
                __##__name##_CDCNCM##__func_id##_BULKIN_EP,                     \
                __##__name##_CDCNCM##__func_id##_BULKOUT_EP,                    \
                __##__name##_CDCNCM##__func_id##_BULK_EP_SIZE,                  \
                __##__name##_CDCNCM##__func_id##_BULK_EP_INTERVAL,              \
                __##__name##_CDCNCM##__func_id##_I_MAC,                         \
                __##__name##_CDCNCM##__func_id##_MAX_DATAGRAM_SIZE, ##__VA_ARGS__)
#define usbd_cdc_ncm_desc_table(__name, __func_id)                              \
            usbd_func_str_desc_table(__name, __func_id)                         \
            VSF_USBD_DESC_STRING(0x0409, __##__name##_CDCNCM##__func_id##_I_MAC,\
                &__##__name##_str_func##__func_id##_mac, sizeof(__##__name##_str_func##__func_id##_mac)),
#define usbd_cdc_ncm_func(__name, __func_id, __str_func, __i_func, __ifs_start, \
        __int_in_ep, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size, __int_ep_interval,\
        __i_mac, __str_mac, ...)                                                \
            __usbd_cdc_ncm_func(__name, __func_id, (__str_func), (__i_func), (__ifs_start),\
                (__int_in_ep), (__bulk_in_ep), (__bulk_out_ep), (__bulk_ep_size), (__int_ep_interval),\
                    (__i_mac), (__str_mac), ##__VA_ARGS__)
#define usbd_cdc_ncm_ifs(__name, __func_id)                                     \
            __usbd_cdc_ncm_ifs(__name, __func_id)

#define usbd_cdcncm_desc_iad(...)       usbd_cdc_ncm_desc_iad(__VA_ARGS__)
#define usbd_cdcncm_desc_table(...)     usbd_cdc_ncm_desc_table(__VA_ARGS__)
#define usbd_cdcncm_func(...)           usbd_cdc_ncm_func(__VA_ARGS__)
#define usbd_cdcncm_ifs(...)            usbd_cdc_ncm_ifs(__VA_ARGS__)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_usbd_cdcncm_t) {
    public_member(
        implement(vk_usbd_cdc_t)
        uint16_t max_datagram_size;
        const usb_unicode_t *str_mac;
    )
    private_member(
        vsf_eda_t *thread;
        vsf_sem_t sem;

        vk_usbd_dev_t *dev;
        vk_usbd_trans_t transact_in;
        vk_usbd_trans_t transact_out;

        vk_netdrv_t netdrv;

        uint16_t seq;
        uint16_t ntb_format;
        uint16_t crc_mode;
        uint16_t packet_filter;
        bool is_tx_busy;
        uint8_t connect_state;
        usb_cdcncm_ntb_input_size_t ntb_input_size;
        uint32_t cur_rx_size;
        uint32_t ntb_in_buffer[VSF_USBD_CDCNCM_CFG_MAX_NTB_IN_SIZE / 4];
        uint32_t ntb_out_buffer[VSF_USBD_CDCNCM_CFG_MAX_NTB_OUT_SIZE / 4];
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbd_class_op_t vk_usbd_cdcncm_control;
extern const vk_usbd_class_op_t vk_usbd_cdcncm_data;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_USB_DEVICE && VSF_USBD_USE_CDCNCM
#endif  // __VSF_USBD_CDCNCM_H__
