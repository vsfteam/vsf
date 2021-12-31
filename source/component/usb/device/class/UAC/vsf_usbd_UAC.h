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

#ifndef __VSF_USBD_UAC_H__
#define __VSF_USBD_UAC_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_UAC == ENABLED

#include "../../../common/class/UAC/vsf_usb_UAC.h"
#include "component/av/vsf_av.h"

#if     defined(__VSF_USBD_UAC_CLASS_IMPLEMENT)
#   undef __VSF_USBD_UAC_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_AUDIO != ENABLED
#   error "VSF_USE_AUDIO MUST be enabled to use uac"
#endif

#ifndef VSF_USBD_UAC_WORKAROUND_CONTROL_OVERFLOW
#   define VSF_USBD_UAC_WORKAROUND_CONTROL_OVERFLOW     ENABLED
#endif

#ifndef VSF_USBD_UAC_CFG_UAC1_EN
#   define VSF_USBD_UAC_CFG_UAC1_EN                     ENABLED
#endif

#ifndef VSF_USBD_UAC_CFG_UAC2_EN
#   define VSF_USBD_UAC_CFG_UAC2_EN                     ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_usbd_uac_control_t vk_usbd_uac_control_t;

typedef struct vk_usbd_uac_control_info_t {
    uint8_t selector;
    uint8_t channel;
    uint16_t size;

    void (*on_set)(vk_usbd_uac_control_t *control);
    
    vk_av_control_value_t min;
    vk_av_control_value_t max;
    vk_av_control_value_t res;

} vk_usbd_uac_control_info_t;

typedef struct vk_usbd_uac2_lay1_t {
    int8_t bMIN;
    int8_t bMAX;
    int8_t bRES;
} vk_usbd_uac2_lay1_t;

typedef struct vk_usbd_uac2_lay2_t {
    int16_t wMIN;
    int16_t wMAX;
    int16_t wRES;
} vk_usbd_uac2_lay2_t;

typedef struct vk_usbd_uac2_lay3_t {
    int32_t dMIN;
    int32_t dMAX;
    int32_t dRES;
} vk_usbd_uac2_lay3_t;

typedef struct vk_usbd_uac2_range_t {
    uint16_t number;
    union {
        vk_usbd_uac2_lay1_t lay1;
        vk_usbd_uac2_lay2_t lay2;
        vk_usbd_uac2_lay3_t lay3;
    } attributes[0];
} PACKED vk_usbd_uac2_range_t;

typedef struct vk_usbd_uac2_control_info_t {
    uint8_t selector;
    uint8_t channel;
    uint16_t size;

    void (*on_set)(vk_usbd_uac_control_t *control);

    const vk_usbd_uac2_range_t *range;

} vk_usbd_uac2_control_info_t;


typedef struct vk_usbd_uac_control_t {
    union {
        const vk_usbd_uac_control_info_t *info;
        const vk_usbd_uac2_control_info_t *info2;
    };    
    vk_av_control_value_t cur;
} vk_usbd_uac_control_t;

typedef struct vk_usbd_uac_entity_t {
    uint8_t id;
    uint8_t control_num;
    vk_usbd_uac_control_t *control;
} vk_usbd_uac_entity_t;

typedef enum vk_usb_uac_version_t {
    VK_USB_UAC1,
    VK_USB_UAC2,
} vk_usb_uac_version_t;

// audio control
vsf_class(vk_usbd_uac_ac_t) {

    public_member(
        uint8_t entity_num;
        uint8_t stream_num;
        vk_usbd_uac_entity_t *entity;
    )

    private_member(
        vk_usbd_dev_t *dev;
        vk_usbd_ifs_t *ifs;
#if VSF_USBD_UAC_CFG_TRACE_EN == ENABLED
        uint32_t cur_size;
#endif
#if VSF_USBD_UAC_WORKAROUND_CONTROL_OVERFLOW == ENABLED
        uint32_t control_value;
#endif
#if (VSF_USBD_UAC_CFG_UAC1_EN == ENABLED) && (VSF_USBD_UAC_CFG_UAC2_EN == ENABLED)
        vk_usb_uac_version_t version; 
#endif
    )
};

// audio stream
vsf_class(vk_usbd_uac_as_t) {
    public_member(
        uint8_t ep;
        uint16_t packet_size;
        vsf_stream_t *stream;
        vk_usbd_uac_ac_t *uac_ac;
    )
    private_member(
        bool is_submitted;
#if VSF_USBD_UAC_CFG_TRACE_EN == ENABLED
        uint32_t cur_size;
#endif
        vk_usbd_trans_t trans;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbd_class_op_t vk_usbd_uac_control_class;
extern const vk_usbd_class_op_t vk_usbd_uac_stream_class;

/*============================ PROTOTYPES ====================================*/

extern void vk_usbd_uac_connect_stream(vk_usbd_uac_as_t *uac_as, vsf_stream_t *stream);
extern void vk_usbd_uac_disconnect_stream(vk_usbd_uac_as_t *uac_as);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_UAC
#endif      // __VSF_USBD_UAC_H__
