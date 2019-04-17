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

/*============================ INCLUDES ======================================*/

#define CLASS_SIMPLE_DEMO_IMPLEMENT

#include "class_simple_demo.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

vsf_err_t class_simple_base_init(class_simple_base_t *pthis, uint_fast8_t param)
{
    this.private_param_base = param;
    return VSF_ERR_NONE;
}

uint_fast8_t class_simple_base_get_param(class_simple_base_t *pthis)
{
    return this.private_param_base;
}

vsf_err_t class_simple_demo_init(class_simple_demo_t *pthis, uint_fast8_t param, uint_fast8_t param_base)
{
    this.private_param_demo = param;
    return class_simple_base_init(&this.use_as__class_simple_base_t, param_base);
}

uint_fast8_t class_simple_demo_get_param(class_simple_demo_t *pthis)
{
    return this.private_param_demo;
}

uint_fast8_t class_simple_demo_get_base_param(class_simple_demo_t *pthis)
{
    return this.private_param_base;
}
