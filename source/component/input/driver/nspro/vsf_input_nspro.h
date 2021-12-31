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

#ifndef __VSF_INPUT_NSPRO_H__
#define __VSF_INPUT_NSPRO_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_input_cfg.h"

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_NSPRO == ENABLED

#include "../../vsf_input_get_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_INPUT_TYPE_NSPRO = VSF_INPUT_USER_TYPE,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_nspro_on_new_dev(void *dev);
extern void vsf_nspro_on_free_dev(void *dev);
extern void vsf_nspro_on_gamepad(vk_gamepad_evt_t *gamepad_evt);
extern void vsf_nspro_on_sensor(vk_sensor_evt_t *sensor_evt);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_INPUT && VSF_INPUT_USE_NSPRO
#endif      // __VSF_INPUT_NSPRO_H__
