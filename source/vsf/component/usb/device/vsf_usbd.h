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

#ifndef __VSF_USBD_H__
#define __VSF_USBD_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#include "../common/usb_common.h"
#include "../common/usb_desc.h"
#include "kernel/vsf_kernel.h"
#include "hal/vsf_hal.h"

#include "./extension/bos/vsf_usbd_bos.h"
#include "./extension/winusb/vsf_usbd_winusb.h"
#include "./extension/webusb/vsf_usbd_webusb.h"

#if VSF_USBD_CFG_RAW_MODE == ENABLED
// user want to do every thing, expose driver interface
#   include "./vsf_usbd_drv_ifs.h"
#endif

#if     defined(__VSF_USBD_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_USBD_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_USBD_CFG_AUTOSETUP
#   define VSF_USBD_CFG_AUTOSETUP           ENABLED
#endif

#ifndef VSF_USBD_CFG_USE_EDA
#   define VSF_USBD_CFG_USE_EDA             VSF_USE_KERNEL
#endif

#if (VSF_USBD_CFG_USE_EDA == ENABLED) && (VSF_USE_KERNEL != ENABLED)
#   error "VSF_USBD_CFG_USE_EDA depend on VSF_USE_KERNEL"
#endif

#if (VSF_USBD_CFG_USE_EDA == ENABLED) && !defined(VSF_USBD_CFG_EDA_PRIORITY)
#   define VSF_USBD_CFG_EDA_PRIORITY        vsf_prio_0
#endif

#if !defined(VSF_USBD_CFG_HW_PRIORITY)
#   define VSF_USBD_CFG_HW_PRIORITY         vsf_arch_prio_0
#endif

#define VSF_USBD_DESC_DEVICE(__DESC, __SIZE)                                    \
    {USB_DT_DEVICE, 0, 0, (__SIZE), (uint8_t*)(__DESC)}
#define VSF_USBD_DESC_CONFIG(__INDEX, __DESC, __SIZE)                           \
    {USB_DT_CONFIG, (__INDEX), 0, (__SIZE), (uint8_t*)(__DESC)}
#define VSF_USBD_DESC_STRING(__LANGID, __INDEX, __DESC, __SIZE)                 \
    {USB_DT_STRING, (__INDEX), (__LANGID), (__SIZE), (uint8_t*)(__DESC)}
#define VSF_USBD_DESC_QUALIFIER(__DESC)                                         \
    {USB_DT_DEVICE_QUALIFIER, 0, 0, USB_DT_DEVICE_QUALIFIER_SIZE, (uint8_t*)(__DESC)}



#define USB_CONFIG(__CONFIG, __INDEX, __IFS)                                    \
        __CONFIG[(__INDEX)]     = {                                             \
            .num_of_ifs         = dimof(__IFS),                                 \
            .ifs                = &(__IFS),                                     \
        },

#define USB_DEVICE(__DEVICE, __CONFIG, __DESC, __SPEED, __DRV)                  \
        __DEVICE                = {                                             \
            .num_of_config      = dimof(__CONFIG),                              \
            .config             = &(__CONFIG),                                  \
            .num_of_desc        = dimof(__DESC),                                \
            .desc               = &(__DESC),                                    \
            .speed              = (__SPEED),                                    \
            .drv                = (__DRV),                                      \
        },

// __USB_IFS is used for class drivers
#define __USB_IFS(__DRV, __PARAM)                                               \
            {                                                                   \
                .class_op       = __DRV,                                        \
                .class_param    = __PARAM,                                      \
            },
#define USB_IFS(__DRV, __PARAM)                                                 \
            __USB_IFS((__DRV), (__PARAM))



#if __STDC_VERSION__ >= 201112L
#   define usb_unicode_t        char16_t
#else
#   define usb_unicode_t        wchar_t
#endif

#define __describe_usbd_version_langid_bcd_ver(                                 \
                                __name,     /* name of the usbd, eg: user_usbd */\
                                __vid,      /* vendor id, 0x0000 - 0xFFFF */    \
                                __pid,      /* product id, 0x0000 - 0xFFFF */   \
                                __speed,    /* usb speed, USB_DC_SPEED_[LOW/FULL/HIGH/SUPER]*/\
                                __version,  /* usb version, eg: 0x210/0x200 */  \
                                __lang_id,  /* language id, eg: 0x0409 */       \
                                __bcd_ver   /* bcdVersion */                    \
                                )                                               \
        enum {                                                                  \
            __##__name##_version        = (__version),                          \
            __##__name##_pid            = (__pid),                              \
            __##__name##_vid            = (__vid),                              \
            __##__name##_langid         = (__lang_id),                          \
            __##__name##_speed          = (__speed),                            \
            __##__name##_bcd_version    = (__bcd_ver),                          \
        };
#define __describe_usbd_version_langid(                                         \
                                __name,     /* name of the usbd, eg: user_usbd */\
                                __vid,      /* vendor id, 0x0000 - 0xFFFF */    \
                                __pid,      /* product id, 0x0000 - 0xFFFF */   \
                                __speed,    /* usb speed, USB_DC_SPEED_[LOW/FULL/HIGH/SUPER]*/\
                                __version,  /* usb version, eg: 0x210/0x200 */  \
                                __lang_id   /* language id, eg: 0x0409 */       \
                                )                                               \
        __describe_usbd_version_langid_bcd_ver(__name, __vid, __pid, __speed, __version, __lang_id, 0x0100)
#define __describe_usbd_version(__name,     /* name of the usbd, eg: user_usbd */\
                                __vid,      /* vendor id, 0x0000 - 0xFFFF */    \
                                __pid,      /* product id, 0x0000 - 0xFFFF */   \
                                __speed,    /* usb speed, USB_DC_SPEED_[LOW/FULL/HIGH/SUPER]*/\
                                __version   /* usb version, eg: 0x210/0x200 */  \
                                )                                               \
        __describe_usbd_version_langid(__name, __vid, __pid, __speed, __version, 0x0409)
#define __describe_usbd(        __name,     /* name of the usbd, eg: user_usbd */\
                                __vid,      /* vendor id, 0x0000 - 0xFFFF */    \
                                __pid,      /* product id, 0x0000 - 0xFFFF */   \
                                __speed     /* usb speed, USB_DC_SPEED_[LOW/FULL/HIGH/SUPER]*/\
                                )                                               \
        __describe_usbd_version(__name, __vid, __pid, __speed, 0x0200)
#define __describe_usbd4                __describe_usbd
#define __describe_usbd5                __describe_usbd_version
#define __describe_usbd6                __describe_usbd_version_langid
#define __describe_usbd7                __describe_usbd_version_langid_bcd_ver

// __str in UTF16 format, eg u"string"
#define __usbd_str_desc(__name, __type, __str)                                  \
        struct {                                                                \
            uint8_t bLength;                                                    \
            uint8_t bDescriptorType;                                            \
            usb_unicode_t str[dimof(__str) - 1];                                \
        } PACKED const __##__name##_str_##__type = {                            \
            .bLength            = sizeof(__##__name##_str_##__type),            \
            .bDescriptorType    = USB_DT_STRING,                                \
            .str                = __str,                                        \
        };

#define __usbd_str_product_desc(__name, __str_product)                          \
        __usbd_str_desc(__name, product, __str_product)
#define usbd_str_product_desc(__name, __str_product)                            \
        __usbd_str_product_desc(__name, __str_product)

#define __usbd_str_vendor_desc(__name, __str_vendor)                            \
        __usbd_str_desc(__name, vendor, __str_vendor)
#define usbd_str_vendor_desc(__name, __str_vendor)                              \
        __usbd_str_vendor_desc(__name, __str_vendor)

#define __usbd_str_serial_desc(__name, __str_serial)                            \
        __usbd_str_desc(__name, serial, __str_serial)
#define usbd_str_serial_desc(__name, __str_serial)                              \
        __usbd_str_serial_desc(__name, __str_serial)

#define __usbd_str_langid_desc(__name)                                          \
        struct {                                                                \
            uint8_t bLength;                                                    \
            uint8_t bDescriptorType;                                            \
            usb_unicode_t str[1];                                               \
        } PACKED const __##__name##_str_langid = {                              \
            .bLength            = 4,                                            \
            .bDescriptorType    = USB_DT_STRING,                                \
            .str                = (__##__name##_langid),                        \
        };
#define usbd_str_langid_desc(__name)                                            \
        __usbd_str_langid_desc(__name)

#define __usbd_device_iad_desc(__name)                                          \
        const uint8_t __##__name##_device_desc[USB_DT_DEVICE_SIZE] = {          \
            USB_DESC_DEV_IAD((__##__name##_version), (__##__name##_ep0size),    \
                (__##__name##_vid), (__##__name##_pid),                         \
                (__##__name##_bcd_version), 1, 2, 3, (__##__name##_config_num)) \
        };
#define usbd_device_iad_desc(__name)                                            \
        __usbd_device_iad_desc(__name)

#define __usbd_device_desc(__name, __class, __subclass, __protocol)             \
        const uint8_t __##__name##_device_desc[USB_DT_DEVICE_SIZE] = {          \
            USB_DESC_DEV((__##__name##_version), (__class), (__subclass),       \
                (__protocol), (__##__name##_ep0size), (__##__name##_vid),       \
                (__##__name##_pid), (__##__name##_bcd_version), 1, 2, 3,        \
                (__##__name##_config_num))                                      \
        };
#define usbd_device_desc(__name, __class, __subclass, __protocol)               \
        __usbd_device_desc(__name, (__class), (__subclass), (__protocol))

#define __usbd_config_desc(__name, __func_desc_size, __func_ifs_num, __attribute, __max_power_ma)\
        const uint8_t __##__name##_config_desc[USB_DT_CONFIG_SIZE + (__func_desc_size)] = {\
            USB_DESC_CFG(USB_DT_CONFIG_SIZE + (__func_desc_size), (__func_ifs_num),\
                        1, 0, USB_CONFIG_ATT_ONE | (__attribute), (__max_power_ma) >> 1)
#define usbd_config_desc(__name, __func_desc_size, __func_ifs_num, __attribute, __max_power_ma)\
        __usbd_config_desc(__name, (__func_desc_size), (__func_ifs_num), (__attribute), (__max_power_ma))

#define __usbd_basic_desc(__name, __func_ifs_num, __ep0_size)                   \
        enum {                                                                  \
            __##__name##_ifsnum = (__func_ifs_num),                             \
            __##__name##_ep0size = (__ep0_size),                                \
            __##__name##_config_num = 1,                                        \
        };
#define usbd_basic_desc(__name, __func_ifs_num, __ep0_size)                     \
        __usbd_basic_desc(__name, (__func_ifs_num), (__ep0_size))

// known limitations:
//  __str_product/__str_vendor/__str_serial MUST be strings in UTF16
//  multiple configuration is not supported
//  does not support big endian
//  to add
#define __usbd_common_desc(             __name,             /* name of the usbd, eg: user_usbd */\
                                        __str_product,      /* product string in UTF16, eg: u"VSF_Board" */\
                                        __str_vendor,       /* vendor string in UTF16, eg: u"VSF" */\
                                        __str_serial,       /* serial string in UTF16, eg: u"1.0.0" */\
                                        __ep0_size,         /* size of endpoint 0, 0 - 64 */\
                                        __func_desc_size,   /* size of all func descriptors, eg: USB_DESC_CDC_ACM_IAD_LEN + USB_DESC_MSCBOT_IAD_LEN */\
                                        __func_ifs_num,     /* number of all func interfaces, eg: USB_CDC_ACM_IFS_NUM + USB_MSC_IFS_NUM */\
                                        __attribute,        /* mask attributes, eg: USB_CONFIG_ATT_[SELFPOWER/WAKEUP/BATTERY] */\
                                        __max_power_ma      /* power consumption from USB host in mA, eg: 500 */\
                                        )                                       \
        usbd_basic_desc(__name, __func_ifs_num, __ep0_size)                     \
        usbd_str_langid_desc(__name)                                            \
        usbd_str_product_desc(__name, __str_product)                            \
        usbd_str_vendor_desc(__name, __str_vendor)                              \
        usbd_str_serial_desc(__name, __str_serial)                              \
        usbd_device_iad_desc(__name)                                            \
        usbd_config_desc(__name, (__func_desc_size), (__func_ifs_num), (__attribute), (__max_power_ma))

#define __usbd_func_desc(__name)                                                \
        };

#define __usbd_func_str_desc(__name, __func_id, __str_func)                     \
        __usbd_str_desc(__name, func##__func_id, __str_func)

#define __usbd_qualifier_desc(__name)                                           \
        const uint8_t __##__name##_qualifier_desc[USB_DT_DEVICE_QUALIFIER_SIZE] = {\
            USB_DESC_QUALIFIER((__##__name##_version), 0, 0, 0, (__##__name##_ep0size), (__##__name##_config_num))\
        };

#define __usbd_std_desc_table(__name)                                           \
        const vk_usbd_desc_t __##__name##_std_descs[] = {                       \
            VSF_USBD_DESC_DEVICE(__##__name##_device_desc, sizeof(__##__name##_device_desc)),\
            VSF_USBD_DESC_CONFIG(0, __##__name##_config_desc, sizeof(__##__name##_config_desc)),\
            VSF_USBD_DESC_STRING(0, 0, &__##__name##_str_langid, sizeof(__##__name##_str_langid)),\
            VSF_USBD_DESC_STRING(__##__name##_langid, 1, &__##__name##_str_vendor, sizeof(__##__name##_str_vendor)),\
            VSF_USBD_DESC_STRING(__##__name##_langid, 2, &__##__name##_str_product, sizeof(__##__name##_str_product)),\
            VSF_USBD_DESC_STRING(__##__name##_langid, 3, &__##__name##_str_serial, sizeof(__##__name##_str_serial)),

#define __usbd_func_str_desc_table_langid(__name, __func_id, __lang_id)          \
            VSF_USBD_DESC_STRING((__lang_id), 4 + __func_id, &__##__name##_str_func##__func_id, sizeof(__##__name##_str_func##__func_id)),
#define __usbd_func_str_desc_table(__name, __func_id)                           \
        __usbd_func_str_desc_table_langid(__name, __func_id, __##__name##_langid)
#define __usbd_func_str_desc_table2         __usbd_func_str_desc_table
#define __usbd_func_str_desc_table3         __usbd_func_str_desc_table_langid

#define __usbd_qualifier_desc_table(__name)                                     \
            VSF_USBD_DESC_QUALIFIER(&__##__name##_qualifier_desc),

#define __usbd_func(__name)                                                     \
        };

#define __usbd_ifs(__name)                                                      \
        vk_usbd_ifs_t __##__name##_ifs[__##__name##_ifsnum] = {

#define __end_describe_usbd(__name, __drv)                                      \
        };                                                                      \
        vk_usbd_cfg_t __##__name##_cfg[1] = {                                   \
            {                                                                   \
                .num_of_ifs         = dimof(__##__name##_ifs),                  \
                .ifs                = __##__name##_ifs,                         \
            },                                                                  \
        };                                                                      \
        vk_usbd_dev_t __name = {                                                \
            .num_of_config      = dimof(__##__name##_cfg),                      \
            .config             = __##__name##_cfg,                             \
            .num_of_desc        = dimof(__##__name##_std_descs),                \
            .desc               = (vk_usbd_desc_t *)__##__name##_std_descs,     \
            .speed              = (usb_dc_speed_t)__##__name##_speed,           \
            .drv                = &(__drv),                                     \
        };


// prototype:
//  describe_usbd(__name, __vid, __pid, __speed, __version = 0x0200, __lang_id = 0x0409, __bcd_ver = 0x0100)
#define describe_usbd(__name, __vid, __pid, __speed, ...)                       \
        extern vk_usbd_dev_t __name;                                            \
        __PLOOC_EVAL(__describe_usbd, __name, __vid, __pid, __speed, ##__VA_ARGS__)(__name, __vid, __pid, __speed, ##__VA_ARGS__)
#define usbd_common_desc(__name, __str_product, __str_vendor, __str_serial, __ep0_size, __func_desc_size, __func_ifs_num, __attribute, __max_power_ma)\
        __usbd_common_desc(__name, __str_product, __str_vendor, __str_serial, (__ep0_size), (__func_desc_size), (__func_ifs_num), (__attribute), (__max_power_ma))
#define usbd_func_desc(__name)                                                  \
        __usbd_func_desc(__name)
#define usbd_func_str_desc(__name, __func_id, __str_func)                       \
        __usbd_func_str_desc(__name, __func_id, __str_func)
#define usbd_qualifier_desc(__name)                                             \
        __usbd_qualifier_desc(__name)
#define usbd_std_desc_table(__name)                                             \
        __usbd_std_desc_table(__name)
// prototype:
//  usbd_func_str_desc_table(__name, __func_id, __lang_id = __LANG_ID_FROM_DESCRIBE_USBD)
#define usbd_func_str_desc_table(__name, __func_id, ...)                        \
        __PLOOC_EVAL(__usbd_func_str_desc_table, __name, __func_id, ##__VA_ARGS__)(__name, __func_id, ##__VA_ARGS__)
#define usbd_qualifier_desc_table(__name)                                       \
        __usbd_qualifier_desc_table(__name)
#define usbd_func(__name)                                                       \
        __usbd_func(__name)
#define usbd_ifs(__name)                                                        \
        __usbd_ifs(__name)
#define end_describe_usbd(__name, __drv)                                        \
        __end_describe_usbd(__name, (__drv))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_usbd_dev_t)
dcl_simple_class(vk_usbd_cfg_t)
dcl_simple_class(vk_usbd_ifs_t)
dcl_simple_class(vk_usbd_trans_t)
dcl_simple_class(vk_usbd_class_op_t)

typedef enum vk_usbd_evt_t {
    USB_ON_INIT =   USB_USR_EVT + 0,
    USB_ON_FINI =   USB_USR_EVT + 1,
} vk_usbd_evt_t;

typedef struct vk_usbd_desc_t {
    uint8_t type;
    uint8_t index;
    uint16_t langid;
    uint16_t size;
    uint8_t *buffer;
} vk_usbd_desc_t;

#if VSF_USBD_CFG_RAW_MODE != ENABLED
def_simple_class(vk_usbd_class_op_t) {
    protected_member(
        vk_usbd_desc_t * (*get_desc)(vk_usbd_dev_t *dev, uint_fast8_t type,
                uint_fast8_t index, uint_fast16_t langid);

        vsf_err_t (*request_prepare)(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
        vsf_err_t (*request_process)(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

        vsf_err_t (*init)(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
        vsf_err_t (*fini)(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
    )
};
#endif

def_simple_class(vk_usbd_trans_t) {
    public_member(
        uint8_t ep;
        uint8_t feature;
        bool zlp;
        bool notify_eda;
        implement(vsf_mem_t)
        union {
            struct {
                void (*on_finish)(void *param);
                void *param;
            };
#if VSF_USE_KERNEL == ENABLED
            struct {
                vsf_eda_t *eda;
            };
#endif
        };
    )

    private_member(
        vsf_slist_node_t node;
        uint8_t *cur;
    )
};

typedef struct vk_usbd_ctrl_handler_t {
    struct usb_ctrlrequest_t request;
    vk_usbd_trans_t trans;
    uint8_t reply_buffer[4];
} vk_usbd_ctrl_handler_t;

#if VSF_USBD_CFG_RAW_MODE != ENABLED
def_simple_class(vk_usbd_ifs_t) {

    public_member(
        const vk_usbd_class_op_t *class_op;
        void *class_param;
    )

    private_member(
        uint8_t alternate_setting;
        bool is_inited;
    )
};

def_simple_class(vk_usbd_cfg_t) {

    public_member(
        vsf_err_t (*init)(vk_usbd_dev_t *dev);
        vsf_err_t (*fini)(vk_usbd_dev_t *dev);

        uint8_t num_of_ifs;
        vk_usbd_ifs_t *ifs;
    )

    private_member(
        uint8_t configuration_value;
        int8_t ep_ifs_map[32];
    )
};
#endif

def_simple_class(vk_usbd_dev_t) {
    public_member(
        usb_dc_speed_t speed;

#if VSF_USBD_CFG_RAW_MODE != ENABLED
        uint8_t num_of_config;
        uint8_t num_of_desc;
        vk_usbd_desc_t *desc;
#endif

#if VSF_USBD_CFG_RAW_MODE != ENABLED
        vk_usbd_cfg_t *config;
#endif

#ifdef __VSF_USBD_CFG_DRV_INTERFACE
        const i_usb_dc_t *drv;
#endif
    )

#if VSF_USBD_CFG_RAW_MODE == ENABLED
    public_member(
#else
    protected_member(
#endif
        vk_usbd_ctrl_handler_t ctrl_handler;
    )

    private_member(
#if VSF_USBD_CFG_RAW_MODE != ENABLED
        uint8_t address;
        uint8_t configuration;
        uint8_t feature;
        bool configured;
#endif
        vsf_slist_t trans_list;
#if VSF_USBD_CFG_USE_EDA == ENABLED
        vsf_eda_t eda;
#endif
    )
};

#if VSF_USBD_CFG_STREAM_EN == ENABLED
#if VSF_USE_SIMPLE_STREAM == ENABLED
dcl_simple_class(vk_usbd_ep_stream_t)
def_simple_class(vk_usbd_ep_stream_t) {

    public_member(
        vsf_stream_t *stream;
        struct {
           void (*on_finish)(void *param);
           void *param;
       } callback;
    )

    protected_member(
        implement(vk_usbd_trans_t)
        vk_usbd_dev_t *dev;
        uint32_t total_size;
        uint32_t transfered_size;
        uint32_t cur_size       : 31;
        uint32_t zlp_save       : 1;
    )
};

#elif VSF_USE_STREAM == ENABLED

declare_class(vk_usbd_ep_stream_t)


typedef struct vk_usbd_ep_stream_cfg_t {
    uint8_t rx_ep;
    uint8_t tx_ep;
    //vk_usbd_dev_t *dev;
} vk_usbd_ep_stream_cfg_t;

def_class(vk_usbd_ep_stream_t,
    which(
        implement(vsf_stream_src_t)
        implement(vsf_stream_usr_t)
    )
    private_member(
        vk_usbd_trans_t tx_trans;
        vk_usbd_trans_t rx_trans;
        vsf_pbuf_t     *tx_current;
        vsf_pbuf_t     *rx_current;
        vk_usbd_dev_t *dev;
    )
)
end_def_class(vk_usbd_ep_stream_src_t)

#endif
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vk_usbd_init(vk_usbd_dev_t *dev);
extern void vk_usbd_fini(vk_usbd_dev_t *dev);
extern void vk_usbd_connect(vk_usbd_dev_t *dev);
extern void vk_usbd_disconnect(vk_usbd_dev_t *dev);
extern void vk_usbd_wakeup(vk_usbd_dev_t *dev);

#if defined(__VSF_USBD_CLASS_IMPLEMENT) || defined(__VSF_USBD_CLASS_INHERIT__)

#if VSF_USBD_CFG_RAW_MODE != ENABLED
extern vk_usbd_desc_t * vk_usbd_get_descriptor(vk_usbd_desc_t *desc,
        uint_fast8_t desc_num, uint_fast8_t type,
        uint_fast8_t index, uint_fast16_t langid);

extern vk_usbd_cfg_t * vk_usbd_get_cur_cfg(vk_usbd_dev_t *dev);
extern vk_usbd_ifs_t * vk_usbd_get_ifs(vk_usbd_dev_t *dev, uint_fast8_t ifs_no);
#endif

extern vsf_err_t vk_usbd_ep_stall(vk_usbd_dev_t *dev, uint_fast8_t ep);
extern vsf_err_t vk_usbd_ep_recv(vk_usbd_dev_t *dev, vk_usbd_trans_t *trans);
extern vsf_err_t vk_usbd_ep_send(vk_usbd_dev_t *dev, vk_usbd_trans_t *trans);


#   if VSF_USBD_CFG_STREAM_EN == ENABLED
extern vsf_err_t vk_usbd_ep_recv_stream(vk_usbd_ep_stream_t *stream, uint_fast32_t size);
extern vsf_err_t vk_usbd_ep_send_stream(vk_usbd_ep_stream_t *stream, uint_fast32_t size);

#       if VSF_USE_STREAM == ENABLED

extern
void vk_usbd_ep_stream_init(   vk_usbd_ep_stream_t *obj,
                                vk_usbd_ep_stream_cfg_t *cfg);

extern
void vk_usbd_ep_stream_connect_dev(vk_usbd_ep_stream_t *obj,
                                    vk_usbd_dev_t *dev);
#       endif   // VSF_USE_STREAM
#   endif       // VSF_USBD_CFG_STREAM_EN

#endif          // __VSF_USBD_CLASS_IMPLEMENT || __VSF_USBD_CLASS_INHERIT__

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#include "./class/CDC/vsf_usbd_CDC.h"
#include "./class/CDC/vsf_usbd_CDCACM.h"
#include "./class/HID/vsf_usbd_HID.h"
#include "./class/UVC/vsf_usbd_UVC.h"
#include "./class/UAC/vsf_usbd_UAC.h"
#include "./class/MSC/vsf_usbd_MSC.h"


#include "../driver/otg/musb/fdrc/vsf_musb_fdrc_dcd.h"
#include "../driver/otg/dwcotg/vsf_dwcotg_dcd.h"
#include "../driver/dcd/usbip_dcd/vsf_usbip_dcd.h"

#undef __VSF_USBD_CLASS_IMPLEMENT
#undef __VSF_USBD_CLASS_INHERIT__

#endif    // VSF_USE_USB_DEVICE
#endif    // __VSF_USBD_H__
