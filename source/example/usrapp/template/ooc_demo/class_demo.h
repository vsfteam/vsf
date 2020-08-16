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
#include "utilities/vsf_utilities.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__VSF_DEMO_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_DEMO_CLASS_IMPLEMENT
#elif   defined(__VSF_DEMO_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_DEMO_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_class(class_base_t)
declare_class(class_demo_t)

def_class(class_base_t,
    public_member(
        uint8_t chPublicParamBase;
    )

    private_member(
        uint8_t chPrivateParamBase;
    )
)

def_class(class_demo_t,
    which(
        implement(class_base_t)
    )
    public_member(
        uint8_t chPublicParamDemo;
    )

    private_member(
        uint8_t chPrivateParamDemo;
    )
)


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t class_base_init(class_base_t *this_ptr, uint_fast8_t chParam);
extern uint_fast8_t class_base_get_param(class_base_t *this_ptr);

extern vsf_err_t class_demo_init(   class_demo_t *this_ptr, 
                                    uint_fast8_t chParam, 
                                    uint_fast8_t chParamBase);
extern uint_fast8_t class_demo_get_param(class_demo_t *this_ptr);
extern uint_fast8_t class_demo_get_base_param(class_demo_t *this_ptr);

#endif      // __CLASS_DEMO_H__
