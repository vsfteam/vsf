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

#ifndef __VSF_INPUT_NSPRO_H__
#define __VSF_INPUT_NSPRO_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_input_cfg.h"

#if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_NSPRO == ENABLED

#include "component/usb/common/class/HID/vsf_usb_nspro.h"
#include "../../vsf_input_get_type.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_INPUT_TYPE_NSPRO = VSF_INPUT_USER_TYPE,
};

struct vk_input_nspro_t {
    vsf_usb_nspro_gamepad_in_report_t data;
    vk_input_timestamp_t timestamp;
};
typedef struct vk_input_nspro_t vk_input_nspro_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_input_item_info_t vk_nspro_gamepad_item_info[GAMEPAD_ID_NUM];
extern const vk_sensor_item_info_t vk_nspro_sensor_item_info[6];

/*============================ PROTOTYPES ====================================*/

extern void vk_nspro_process_input(vk_input_nspro_t *dev, vsf_usb_nspro_gamepad_in_report_t *data);
extern void vk_nspro_new_dev(vk_input_nspro_t *dev);
extern void vk_nspro_free_dev(vk_input_nspro_t *dev);

#endif      // VSF_USE_INPUT && VSF_USE_INPUT_NSPRO
#endif      // __VSF_INPUT_NSPRO_H__
