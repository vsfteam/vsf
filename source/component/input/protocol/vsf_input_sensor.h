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



#ifndef __VSF_INPUT_SENSOR_H__
#define __VSF_INPUT_SENSOR_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_input_cfg.h"

#include "../vsf_input_get_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define vsf_input_sensor_set(__event, __type, __subtype, __min, __max, __res, __value)\
            do {                                                                \
                (__event)->id = ((uint64_t)(__type) << 0) | ((uint64_t)(__subtype) << 32);\
                (__event)->info.minimum = (__min);                              \
                (__event)->info.maximum = (__max);                              \
                (__event)->info.resolution = (__res);                           \
                (__event)->cur.val64 = (__value);                               \
            } while (0)

#define vsf_input_sensor_get_type(__event)                                      \
            ((vk_sensor_type_t)(((__event)->id >> 0) & 0xFFFFFFFF))
#define vsf_input_sensor_get_subtype(__event)                                   \
            ((vk_sensor_subtype_t)(((__event)->id >> 32) & 0xFFFFFFFF))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_INPUT_TYPE_SENSOR = VSF_INPUT_USER_TYPE,
};

typedef enum vk_sensor_type_t {
    SENSOR_TYPE_ACC,
    SENSOR_TYPE_GYRO,
    SENSOR_TYPE_MAG,

    SENSOR_TYPE_USER,
} vk_sensor_type_t;

typedef enum vk_sensor_subtype_t {
    SENSOR_SUBTYPE_X,
    SENSOR_SUBTYPE_Y,
    SENSOR_SUBTYPE_Z,

    SENSOR_SUBTYPE_PITCH = SENSOR_SUBTYPE_X,
    SENSOR_SUBTYPE_YAW = SENSOR_SUBTYPE_Y,
    SENSOR_SUBTYPE_ROLL = SENSOR_SUBTYPE_Z,

    SENSOR_SUBTYPE_USER,
} vk_sensor_subtype_t;

typedef struct vk_sensor_info_t {
    int32_t minimum;
    int32_t maximum;
    uint32_t resolution;
} vk_sensor_info_t;

typedef struct vk_sensor_evt_t {
    implement(vk_input_evt_t)
    vk_sensor_info_t info;
} vk_sensor_evt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */