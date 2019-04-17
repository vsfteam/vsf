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
#ifndef __CLASS_SIMPLE_DEMO_H__
#define __CLASS_SIMPLE_DEMO_H__

/*============================ INCLUDES ======================================*/

#include "vsf_cfg.h"

#if     defined(CLASS_SIMPLE_DEMO_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT
#   undef CLASS_SIMPLE_DEMO_IMPLEMENT
#elif   defined(CLASS_SIMPLE_DEMO_INHERIT)
#   define __VSF_CLASS_INHERIT
#   undef CLASS_SIMPLE_DEMO_INHERIT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(class_simple_base_t)
declare_simple_class(class_simple_demo_t)

def_simple_class(class_simple_base_t) {
    public_member(
        uint8_t public_param_base;
    )

    private_member(
        uint8_t private_param_base;
    )
};

def_simple_class(class_simple_demo_t) {
    implement(class_simple_base_t)

    public_member(
        uint8_t public_param_demo;
    )

    private_member(
        uint8_t private_param_demo;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t class_simple_base_init(class_simple_base_t *pthis, uint_fast8_t param);
extern uint_fast8_t class_simple_base_get_param(class_simple_base_t *pthis);

extern vsf_err_t class_simple_demo_init(class_simple_demo_t *pthis, uint_fast8_t param, uint_fast8_t param_base);
extern uint_fast8_t class_simple_demo_get_param(class_simple_demo_t *pthis);
extern uint_fast8_t class_simple_demo_get_base_param(class_simple_demo_t *pthis);

#endif      // __CLASS_SIMPLE_DEMO_H__
