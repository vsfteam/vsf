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



#ifndef __VSF_INPUT_SENSOR_H__
#define __VSF_INPUT_SENSOR_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_input_cfg.h"

#include "../vsf_input_get_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_SENSOR_DEF_ITEM_INFO(__ID, __SUBID, __BITLEN)                       \
            {                                                                   \
                .id     = (__ID),                                               \
                .subid  = (__SUBID),                                            \
                .bitlen = (__BITLEN),                                           \
            }

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_INPUT_TYPE_SENSOR = VSF_INPUT_USER_TYPE,
};

typedef enum vk_sensor_id_t {
    SENSOR_ID_ACC,
    SENSOR_ID_GYRO,
    SENSOR_ID_MAG,

    SENSOR_ID_USER,
} vk_sensor_id_t;

typedef enum vk_sensor_subid_t {
    SENSOR_SUBID_X,
    SENSOR_SUBID_Y,
    SENSOR_SUBID_Z,

    SENSOR_SUBID_PITCH,
    SENSOR_SUBID_YAW,
    SENSOR_SUBID_ROLL,

    SENSOR_SUBID_USER,
} vk_sensor_subid_t;

typedef struct vk_sensor_item_info_t {
    vk_sensor_id_t id;
    vk_sensor_subid_t subid;
    uint8_t bitlen;
} vk_sensor_item_info_t;

typedef struct vk_sensor_desc_t {
    uint8_t item_num;
    vk_sensor_item_info_t *item_info;
} vk_sensor_desc_t;

typedef struct vk_sensor_evt_t {
    implement(vk_input_evt_t)
    vk_sensor_desc_t desc;
    void *data;
} vk_sensor_evt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */