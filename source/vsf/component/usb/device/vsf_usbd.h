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

#include "./extension/bos/vsf_usbd_bos.h"
#include "./extension/winusb/vsf_usbd_winusb.h"
#include "./extension/webusb/vsf_usbd_webusb.h"

#if __STDC_VERSION__ >= 201112L
#   include <uchar.h>
#endif

#if     defined(VSF_USBD_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_USBD_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#ifndef VSF_USBD_CFG_AUTOSETUP
#   define VSF_USBD_CFG_AUTOSETUP           ENABLED
#endif

#ifndef VSF_USBD_CFG_USE_EDA
#   define VSF_USBD_CFG_USE_EDA             ENABLED
#endif

#if VSF_USBD_CFG_USE_EDA != ENABLED
#   error "support VSF_USBD_CFG_USE_EDA only, please enable it!!!"
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
#define VSF_USBD_DESC_STRING(__LANID, __INDEX, __DESC, __SIZE)                  \
    {USB_DT_STRING, (__INDEX), (__LANID), (__SIZE), (uint8_t*)(__DESC)}
#define VSF_USBD_DESC_QUALIFIER(__DESC)                                         \
    {USB_DT_DEVICE_QUALIFIER, 0, 0, USB_DT_DEVICE_QUALIFIER_SIZE, (uint8_t*)(__DESC)}

#if defined(VSF_USBD_CFG_DRV_LV0) || defined(VSF_USBD_CFG_DRV_LV1)
#   undef VSF_USBD_CFG_DRV_INTERFACE
#else
#   define VSF_USBD_CFG_DRV_INTERFACE
#endif



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



#if __STDC_VERSION__ >= 201112L
#   define USB_UNICODE          char16_t
#else
#   define USB_UNICODE          wchar_t
#endif

#define __describe_usbd_version_langid_bcd_ver(                                 \
                                __NAME,     /* name of the usbd, eg: user_usbd */\
                                __VID,      /* vendor id, 0x0000 - 0xFFFF */    \
                                __PID,      /* product id, 0x0000 - 0xFFFF */   \
                                __SPEED,    /* usb speed, USB_DC_SPEED_[LOW/FULL/HIGH/SUPER]*/\
                                __VERSION,  /* usb version, eg: 0x210/0x200 */  \
                                __LANG_ID,  /* language id, eg: 0x0409 */       \
                                __BCD_VER   /* bcdVersion */                    \
                                )                                               \
        enum {                                                                  \
            __##__NAME##_VERSION        = (__VERSION),                          \
            __##__NAME##_PID            = (__PID),                              \
            __##__NAME##_VID            = (__VID),                              \
            __##__NAME##_LANGID         = (__LANG_ID),                          \
            __##__NAME##_SPEED          = (__SPEED),                            \
            __##__NAME##_BCD_VERSION    = (__BCD_VER),                          \
        };
#define __describe_usbd_version_langid(                                         \
                                __NAME,     /* name of the usbd, eg: user_usbd */\
                                __VID,      /* vendor id, 0x0000 - 0xFFFF */    \
                                __PID,      /* product id, 0x0000 - 0xFFFF */   \
                                __SPEED,    /* usb speed, USB_DC_SPEED_[LOW/FULL/HIGH/SUPER]*/\
                                __VERSION,  /* usb version, eg: 0x210/0x200 */  \
                                __LANG_ID   /* language id, eg: 0x0409 */       \
                                )                                               \
        __describe_usbd_version_langid_bcd_ver(__NAME, __VID, __PID, __SPEED, __VERSION, __LANG_ID, 0x0100)
#define __describe_usbd_version(__NAME,     /* name of the usbd, eg: user_usbd */\
                                __VID,      /* vendor id, 0x0000 - 0xFFFF */    \
                                __PID,      /* product id, 0x0000 - 0xFFFF */   \
                                __SPEED,    /* usb speed, USB_DC_SPEED_[LOW/FULL/HIGH/SUPER]*/\
                                __VERSION   /* usb version, eg: 0x210/0x200 */  \
                                )                                               \
        __describe_usbd_version_langid(__NAME, __VID, __PID, __SPEED, __VERSION, 0x0409)
#define __describe_usbd(        __NAME,     /* name of the usbd, eg: user_usbd */\
                                __VID,      /* vendor id, 0x0000 - 0xFFFF */    \
                                __PID,      /* product id, 0x0000 - 0xFFFF */   \
                                __SPEED     /* usb speed, USB_DC_SPEED_[LOW/FULL/HIGH/SUPER]*/\
                                )                                               \
        __describe_usbd_version(__NAME, __VID, __PID, __SPEED, 0x0200)
#define __describe_usbd4                __describe_usbd
#define __describe_usbd5                __describe_usbd_version
#define __describe_usbd6                __describe_usbd_version_langid
#define __describe_usbd7                __describe_usbd_version_langid_bcd_ver

// known limitations:
//  __STR_PRODUCT/__STR_VENDOR/__STR_SERIAL MUST be ASCII strings
//  multiple configuration is not supported
//  does not support big endian
//  to add
#define __usbd_common_desc(             __NAME,             /* name of the usbd, eg: user_usbd */\
                                        __STR_PRODUCT,      /* product string in UTF16, eg: u"VSF_Board" */\
                                        __STR_VENDOR,       /* vendor string in UTF16, eg: u"VSF" */\
                                        __STR_SERIAL,       /* serial string in UTF16, eg: u"1.0.0" */\
                                        __EP0_SIZE,         /* size of endpoint 0, 0 - 64 */\
                                        __FUNC_DESC_SIZE,   /* size of all func descriptors, eg: USB_DESC_CDC_ACM_IAD_LEN + USB_DESC_MSCBOT_IAD_LEN */\
                                        __FUNC_IFS_NUM,     /* number of all func interfaces, eg: USB_CDC_ACM_IFS_NUM + USB_MSC_IFS_NUM */\
                                        __ATTRIBUTE,        /* mask attributes, eg: USB_CONFIG_ATT_[SELFPOWER/WAKEUP/BATTERY] */\
                                        __MAX_POWER_MA      /* power consumption from USB host in mA, eg: 500 */\
                                        )                                       \
        enum {                                                                  \
            __##__NAME##_IFSNUM = (__FUNC_IFS_NUM),                             \
            __##__NAME##_EP0SIZE = (__EP0_SIZE),                                \
            __##__NAME##_CONFIG_NUM = 1,                                        \
        };                                                                      \
        struct {                                                                \
            uint8_t bLength;                                                    \
            uint8_t bDescriptorType;                                            \
            USB_UNICODE str[1];                                                 \
        } PACKED const __##__NAME##_str_lanid = {                               \
            .bLength            = 4,                                            \
            .bDescriptorType    = USB_DT_STRING,                                \
            .str                = (__##__NAME##_LANGID),                        \
        };                                                                      \
        struct {                                                                \
            uint8_t bLength;                                                    \
            uint8_t bDescriptorType;                                            \
            USB_UNICODE str[dimof(__STR_PRODUCT) - 1];                          \
        } PACKED const __##__NAME##_str_product = {                             \
            .bLength            = sizeof(__##__NAME##_str_product),             \
            .bDescriptorType    = USB_DT_STRING,                                \
            .str                = __STR_PRODUCT,                                \
        };                                                                      \
        struct {                                                                \
            uint8_t bLength;                                                    \
            uint8_t bDescriptorType;                                            \
            USB_UNICODE str[dimof(__STR_VENDOR) - 1];                           \
        } PACKED const __##__NAME##_str_vendor = {                              \
            .bLength            = sizeof(__##__NAME##_str_vendor),              \
            .bDescriptorType    = USB_DT_STRING,                                \
            .str                = __STR_VENDOR,                                 \
        };                                                                      \
        struct {                                                                \
            uint8_t bLength;                                                    \
            uint8_t bDescriptorType;                                            \
            USB_UNICODE str[dimof(__STR_SERIAL) - 1];                           \
        } PACKED const __##__NAME##_str_serial = {                              \
            .bLength            = sizeof(__##__NAME##_str_serial),              \
            .bDescriptorType    = USB_DT_STRING,                                \
            .str                = __STR_SERIAL,                                 \
        };                                                                      \
        const uint8_t __##__NAME##_device_desc[USB_DT_DEVICE_SIZE] = {          \
            USB_DESC_DEV_IAD((__##__NAME##_VERSION), (__##__NAME##_EP0SIZE), (__##__NAME##_VID), (__##__NAME##_PID), (__##__NAME##_BCD_VERSION), 1, 2, 3, (__##__NAME##_CONFIG_NUM))\
        };                                                                      \
        const uint8_t __##__NAME_config_desc[USB_DT_CONFIG_SIZE + (__FUNC_DESC_SIZE)] = {\
            USB_DESC_CFG(USB_DT_CONFIG_SIZE + (__FUNC_DESC_SIZE), (__FUNC_IFS_NUM),\
                        1, 0, USB_CONFIG_ATT_ONE | (__ATTRIBUTE), (__MAX_POWER_MA) >> 1)

#define __usbd_func_desc(__NAME)                                                \
        };

#define __usbd_func_str_desc(__NAME, __FUNC_ID, __STR_FUNC)                     \
        struct {                                                                \
            uint8_t bLength;                                                    \
            uint8_t bDescriptorType;                                            \
            USB_UNICODE str[dimof(__STR_FUNC) - 1];                             \
        } PACKED const __##__NAME##_str_func##__FUNC_ID = {                     \
            .bLength            = sizeof(__##__NAME##_str_func##__FUNC_ID),     \
            .bDescriptorType    = USB_DT_STRING,                                \
            .str                = __STR_FUNC,                                   \
        };

#define __usbd_qualifier_desc(__NAME)                                           \
        const uint8_t __##__NAME_qualifier_desc[USB_DT_DEVICE_QUALIFIER_SIZE] = {\
            USB_DESC_QUALIFIER((__##__NAME##_VERSION), 0, 0, 0, (__##__NAME##_EP0SIZE), (__##__NAME##_CONFIG_NUM))\
        };

#define __usbd_std_desc_table(__NAME)                                           \
        const vk_usbd_desc_t __##__NAME##_std_descs[] = {                       \
            VSF_USBD_DESC_DEVICE(__##__NAME##_device_desc, sizeof(__##__NAME##_device_desc)),\
            VSF_USBD_DESC_CONFIG(0, __##__NAME_config_desc, sizeof(__##__NAME_config_desc)),\
            VSF_USBD_DESC_STRING(0, 0, &__##__NAME##_str_lanid, sizeof(__##__NAME##_str_lanid)),\
            VSF_USBD_DESC_STRING(__##__NAME##_LANGID, 1, &__##__NAME##_str_vendor, sizeof(__##__NAME##_str_vendor)),\
            VSF_USBD_DESC_STRING(__##__NAME##_LANGID, 2, &__##__NAME##_str_product, sizeof(__##__NAME##_str_product)),\
            VSF_USBD_DESC_STRING(__##__NAME##_LANGID, 3, &__##__NAME##_str_serial, sizeof(__##__NAME##_str_serial)),

#define __usbd_func_str_desc_table_langid(__NAME, __FUNC_ID, __LANG_ID)          \
            VSF_USBD_DESC_STRING((__LANG_ID), 4 + __FUNC_ID, &__##__NAME##_str_func##__FUNC_ID, sizeof(__##__NAME##_str_func##__FUNC_ID)),
#define __usbd_func_str_desc_table(__NAME, __FUNC_ID)                           \
        __usbd_func_str_desc_table_langid(__NAME, __FUNC_ID, __##__NAME##_LANGID)
#define __usbd_func_str_desc_table2         __usbd_func_str_desc_table
#define __usbd_func_str_desc_table3         __usbd_func_str_desc_table_langid

#define __usbd_qualifier_desc_table(__NAME)                                     \
            VSF_USBD_DESC_QUALIFIER(&__##__NAME_qualifier_desc),

#define __usbd_func(__NAME)                                                     \
        };

#define __usbd_ifs(__NAME)                                                      \
        vk_usbd_ifs_t __##__NAME##_ifs[__##__NAME##_IFSNUM] = {

#define __end_describe_usbd(__NAME, __DEV)                                      \
        };                                                                      \
        vk_usbd_cfg_t __##__NAME##_cfg[1] = {                                   \
            {                                                                   \
                .num_of_ifs         = dimof(__##__NAME##_ifs),                  \
                .ifs                = __##__NAME##_ifs,                         \
            },                                                                  \
        };                                                                      \
        vk_usbd_dev_t __NAME = {                                                \
            .num_of_config      = dimof(__##__NAME##_cfg),                      \
            .config             = __##__NAME##_cfg,                             \
            .num_of_desc        = dimof(__##__NAME##_std_descs),                \
            .desc               = (vk_usbd_desc_t *)__##__NAME##_std_descs,     \
            .speed              = (usb_dc_speed_t)__##__NAME##_SPEED,           \
            .drv                = &(__DEV),                                     \
        };


// prototype:
//  describe_usbd(__NAME, __VID, __PID, __SPEED)
//  describe_usbd(__NAME, __VID, __PID, __SPEED, __VERSION)
//  describe_usbd(__NAME, __VID, __PID, __SPEED, __VERSION, __LANG_ID)
//  describe_usbd(__NAME, __VID, __PID, __SPEED, __VERSION, __LANG_ID, __BCD_VER)
#define describe_usbd(__NAME, __VID, __PID, __SPEED, ...)            \
        __PLOOC_EVAL(__describe_usbd, __NAME, __VID, __PID, __SPEED, ##__VA_ARGS__)(__NAME, __VID, __PID, __SPEED, ##__VA_ARGS__)
#define usbd_common_desc(__NAME, __STR_PRODUCT, __STR_VENDOR, __STR_SERIAL, __EP0_SIZE, __FUNC_DESC_SIZE, __FUNC_IFS_NUM, __ATTRIBUTE, __MAX_POWER_MA)\
        __usbd_common_desc(__NAME, __STR_PRODUCT, __STR_VENDOR, __STR_SERIAL, (__EP0_SIZE), (__FUNC_DESC_SIZE), (__FUNC_IFS_NUM), (__ATTRIBUTE), (__MAX_POWER_MA))
#define usbd_func_desc(__NAME)                                                  \
        __usbd_func_desc(__NAME)
#define usbd_func_str_desc(__NAME, __FUNC_ID, __STR_FUNC)                       \
        __usbd_func_str_desc(__NAME, __FUNC_ID, __STR_FUNC)
#define usbd_qualifier_desc(__NAME)                                             \
        __usbd_qualifier_desc(__NAME)
#define usbd_std_desc_table(__NAME)                                             \
        __usbd_std_desc_table(__NAME)
// prototype:
//  usbd_func_str_desc_table(__NAME, __FUNC_ID)             // using the __LANG_ID in describe_usbd
//  usbd_func_str_desc_table(__NAME, __FUNC_ID, __LANG_ID)  // using the specified __LANG_ID
#define usbd_func_str_desc_table(__NAME, __FUNC_ID, ...)                        \
        __PLOOC_EVAL(__usbd_func_str_desc_table, __NAME, __FUNC_ID, ##__VA_ARGS__)(__NAME, __FUNC_ID, ##__VA_ARGS__)
#define usbd_qualifier_desc_table(__NAME)                                       \
        __usbd_qualifier_desc_table(__NAME)
#define usbd_func(__NAME)                                                       \
        __usbd_func(__NAME)
#define usbd_ifs(__NAME)                                                        \
        __usbd_ifs(__NAME)
#define end_describe_usbd(__NAME, __DEV)                                        \
        __end_describe_usbd(__NAME, (__DEV))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef union vk_usbd_ep_cfg_t vk_usbd_ep_cfg_t;
union vk_usbd_ep_cfg_t {
    struct {
        uint8_t notify;
        uint8_t out;
        uint8_t in;
    };
    uint32_t ep_cfg;
};

declare_simple_class(vk_usbd_dev_t)
declare_simple_class(vk_usbd_cfg_t)
declare_simple_class(vk_usbd_ifs_t)
declare_simple_class(vk_usbd_trans_t)

enum vk_usbd_evt_t {
    USB_ON_INIT =   USB_USR_EVT + 0,
    USB_ON_FINI =   USB_USR_EVT + 1
};
typedef enum vk_usbd_evt_t vk_usbd_evt_t;

struct vk_usbd_desc_t {
    uint8_t type;
    uint8_t index;
    uint16_t lanid;
    uint16_t size;
    uint8_t *buffer;
};
typedef struct vk_usbd_desc_t vk_usbd_desc_t;

struct vk_usbd_class_op_t {
    vk_usbd_desc_t * (*get_desc)(vk_usbd_dev_t *dev, uint_fast8_t type,
                uint_fast8_t index, uint_fast16_t lanid);

    vsf_err_t (*request_prepare)(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
    vsf_err_t (*request_process)(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

    vsf_err_t (*init)(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
    vsf_err_t (*fini)(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
};
typedef struct vk_usbd_class_op_t vk_usbd_class_op_t;

def_simple_class(vk_usbd_trans_t) {
    public_member(
        uint8_t ep;
        bool zlp;
        bool notify_eda;
        implement(vsf_mem_t)
        union {
            struct {
                void (*on_finish)(void *param);
                void *param;
            };
            struct {
                vsf_eda_t *eda;
            };
        };
    )

	private_member(
        vsf_slist_node_t node;
        uint8_t *cur;
    )
};

struct vk_usbd_ctrl_handler_t {
    struct usb_ctrlrequest_t request;
    vk_usbd_trans_t trans;
    uint8_t reply_buffer[4];
};
typedef struct vk_usbd_ctrl_handler_t vk_usbd_ctrl_handler_t;

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

def_simple_class(vk_usbd_dev_t) {
    public_member(
        uint8_t num_of_config;
        uint8_t num_of_desc;

        usb_dc_speed_t speed;
        vk_usbd_cfg_t *config;
        vk_usbd_desc_t *desc;

#ifdef VSF_USBD_CFG_DRV_INTERFACE
        const i_usb_dc_t *drv;
#endif
    )

    protected_member(
        vk_usbd_ctrl_handler_t ctrl_handler;
    )

    private_member(
        uint8_t address;
        uint8_t configuration;
        uint8_t feature;
        bool configured;
        vsf_slist_t trans_list;
#if VSF_USBD_CFG_USE_EDA == ENABLED
        vsf_eda_t eda;
#endif
    )
};

#if VSF_USBD_CFG_STREAM_EN == ENABLED
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
declare_simple_class(vk_usbd_ep_stream_t)
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
        uint32_t cur_size;
    )
};

#elif VSF_USE_SERVICE_STREAM == ENABLED

declare_class(vk_usbd_ep_stream_t)


typedef struct {
    uint8_t rx_ep;
    uint8_t tx_ep;
    //vk_usbd_dev_t *dev;
}vk_usbd_ep_stream_cfg_t;

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

#if defined(VSF_USBD_IMPLEMENT) || defined(VSF_USBD_INHERIT)
extern vk_usbd_desc_t * vk_usbd_get_descriptor(vk_usbd_desc_t *desc,
        uint_fast8_t desc_num, uint_fast8_t type,
        uint_fast8_t index, uint_fast16_t lanid);
extern vk_usbd_cfg_t * vk_usbd_get_cur_cfg(vk_usbd_dev_t *dev);
extern vk_usbd_ifs_t * vk_usbd_get_ifs(vk_usbd_dev_t *dev, uint_fast8_t ifs_no);

extern vsf_err_t vk_usbd_ep_stall(vk_usbd_dev_t *dev, uint_fast8_t ep);
extern vsf_err_t vk_usbd_ep_recv(vk_usbd_dev_t *dev, vk_usbd_trans_t *trans);
extern vsf_err_t vk_usbd_ep_send(vk_usbd_dev_t *dev, vk_usbd_trans_t *trans);


#if VSF_USBD_CFG_STREAM_EN == ENABLED
extern vsf_err_t vk_usbd_ep_recv_stream(vk_usbd_ep_stream_t *stream, uint_fast32_t size);
extern vsf_err_t vk_usbd_ep_send_stream(vk_usbd_ep_stream_t *stream, uint_fast32_t size);

#if VSF_USE_SERVICE_STREAM == ENABLED

extern 
void vk_usbd_ep_stream_init(   vk_usbd_ep_stream_t *obj, 
                                vk_usbd_ep_stream_cfg_t *cfg);

extern 
void vk_usbd_ep_stream_connect_dev(vk_usbd_ep_stream_t *obj, 
                                    vk_usbd_dev_t *dev);
#endif      // VSF_USE_SERVICE_STREAM
#endif      // VSF_USBD_CFG_STREAM_EN

#endif      // VSF_USBD_IMPLEMENT || VSF_USBD_INHERIT

#undef VSF_USBD_IMPLEMENT
#undef VSF_USBD_INHERIT

#endif    // VSF_USE_USB_DEVICE
#endif    // __VSF_USBD_H__
