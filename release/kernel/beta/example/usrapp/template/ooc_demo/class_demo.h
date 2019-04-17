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
#ifndef __CLASS_DEMO_H__
#define __CLASS_DEMO_H__

/*============================ INCLUDES ======================================*/

#include "vsf_cfg.h"
#include "./__class_demo.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t class_base_init(class_base_t *pthis, uint_fast8_t param);
extern uint_fast8_t class_base_get_param(class_base_t *pthis);

extern vsf_err_t class_demo_init(class_demo_t *pthis, uint_fast8_t param, uint_fast8_t param_base);
extern uint_fast8_t class_demo_get_param(class_demo_t *pthis);
extern uint_fast8_t class_demo_get_base_param(class_demo_t *pthis);

#endif      // __CLASS_DEMO_H__
