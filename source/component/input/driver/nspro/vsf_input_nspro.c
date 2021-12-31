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

/*============================ INCLUDES ======================================*/

#include "../../vsf_input_cfg.h"

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_NSPRO == ENABLED

#include "../../vsf_input.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_input_on_new_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_free_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_sensor(vk_sensor_evt_t *sensor_evt);
extern void vsf_input_on_gamepad(vk_gamepad_evt_t *gamepad_evt);

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_NSPRO_ON_NEW_DEV
WEAK(vsf_nspro_on_new_dev)
void vsf_nspro_on_new_dev(void *dev)
{
    vsf_input_on_new_dev(VSF_INPUT_TYPE_NSPRO, dev);
}
#endif

#ifndef WEAK_VSF_NSPRO_ON_FREE_DEV
WEAK(vsf_nspro_on_free_dev)
void vsf_nspro_on_free_dev(void *dev)
{
    vsf_input_on_free_dev(VSF_INPUT_TYPE_NSPRO, dev);
}
#endif

#ifndef WEAK_VSF_NSPRO_ON_REPORT_PARSED
WEAK(vsf_nspro_on_report_parsed)
void vsf_nspro_on_report_parsed(vk_input_evt_t *evt)
{
    vsf_input_on_evt(VSF_INPUT_TYPE_SYNC, evt);
}
#endif

#ifndef WEAK_VSF_NSPRO_ON_GAMEPAD
WEAK(vsf_nspro_on_gamepad)
void vsf_nspro_on_gamepad(vk_gamepad_evt_t *gamepad_evt)
{
    vsf_input_on_gamepad(gamepad_evt);
}
#endif

#ifndef WEAK_VSF_NSPRO_ON_SENSOR
WEAK(vsf_nspro_on_sensor)
void vsf_nspro_on_sensor(vk_sensor_evt_t *sensor_evt)
{
    vsf_input_on_sensor(sensor_evt);
}
#endif

#endif      // VSF_USE_INPUT && VSF_INPUT_USE_NSPRO
