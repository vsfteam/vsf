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
#define __VSF_DEMO_CLASS_IMPLEMENT
#include "class_demo.h"

#include "vsf.h"

/*============================ MACROS ========================================*/
#undef  this
#define this    (*ptThis)
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

vsf_err_t class_base_init(class_base_t *ptObj, uint_fast8_t chParam)
{
    class_internal(ptObj, ptThis, class_base_t);
    this.private_param_base = chParam;
    return VSF_ERR_NONE;
}

uint_fast8_t class_base_get_param(class_base_t *ptObj)
{
    class_internal(ptObj, ptThis, class_base_t);
    return this.private_param_base;
}

vsf_err_t class_demo_init(class_demo_t *ptObj, uint_fast8_t chParam, uint_fast8_t chParamBase)
{
    class_internal(ptObj, ptThis, class_demo_t);
    this.private_param_demo = chParam;
    return class_base_init(&this.use_as__class_base_t, chParamBase);
}

uint_fast8_t class_demo_get_param(class_demo_t *ptObj)
{
    class_internal(ptObj, ptThis, class_demo_t);
    return this.private_param_demo;
}

uint_fast8_t class_demo_get_base_param(class_demo_t *ptObj)
{
    class_internal(ptObj, ptThis, class_demo_t);
    uint_fast8_t base_param;
    with_class(class_base_t, &this.use_as__class_base_t,
                    base_param = _->private_param_base;
                );
    return base_param;
}
