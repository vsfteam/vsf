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
#include "utilities/vsf_utilities.h"

#if     defined(CLASS_SIMPLE_DEMO_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef CLASS_SIMPLE_DEMO_IMPLEMENT
#elif   defined(CLASS_SIMPLE_DEMO_INHERIT)
#   define __PLOOC_CLASS_INHERIT__
#   undef CLASS_SIMPLE_DEMO_INHERIT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_class(class_simple_base_t)
declare_class(class_simple_demo_t)

/* You can use the syntax compatible with ooc_class_strict */
def_class(class_simple_base_t,
    public_member(
        uint8_t public_param_base;
    )

    private_member(
        uint8_t private_param_base;
    )
)

/* You can also use the STRUCTURE-OBVIOUSE syntax of ooc_class_simple */
def_simple_class(class_simple_demo_t) {
    which(
        implement(class_simple_base_t)
    )
    public_member(
        uint8_t public_param_demo;
    )
    private_member(
        uint8_t private_param_demo;
    )
};

declare_class(xxxxx_t)

typedef struct xxxxx_cfg_t xxxxx_cfg_t;
struct xxxxx_cfg_t {
    uint8_t *buffer_ptr;
    uint32_t wNumber;
};

def_simple_class(xxxxx_t) {
    
    private_member(
        implement(xxxxx_cfg_t)
        uint32_t other_private_members;
    )

};
/*
vsf_err_t xxxxx_cfg(xxxxx_t *obj_ptr, xxxxx_cfg_t *cfg_ptr)
{
    ...
    obj_ptr->use_as__xxxxx_cfg_t = (*cfg_ptr);      //! copy to initialise
    ...
}
*/
#if defined(__OOC_RELEASE__)
#define STATIC_XXXXX(__VAR, ...)            \
        do {                                \
            xxxxx_t __VAR = {               \
                .use_as__xxxxx_cfg_t = {    \
                    __VA_ARGS__             \
                },                          \
            };                              \
        } while(0);
#else
#define STATIC_XXXXX(__VAR, ...)            \
        do {                                \
            xxxxx_t __VAR;                  \
            const xxxxx_cfg_t cfg = {      \
                __VA_ARGS__                 \
            };                              \
            xxxxx_cfg(&(__VAR), &cfg);     \
        } while(0);
#endif


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t class_simple_base_init(class_simple_base_t *pthis, 
                                        uint_fast8_t param);
extern uint_fast8_t class_simple_base_get_param(class_simple_base_t *pthis);

extern vsf_err_t class_simple_demo_init(class_simple_demo_t *pthis,
                                        uint_fast8_t param, 
                                        uint_fast8_t param_base);
extern uint_fast8_t class_simple_demo_get_param(class_simple_demo_t *pthis);
extern uint_fast8_t class_simple_demo_get_base_param(class_simple_demo_t *pthis);

#endif      // __CLASS_SIMPLE_DEMO_H__
