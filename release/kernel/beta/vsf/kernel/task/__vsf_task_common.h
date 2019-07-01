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

#ifndef __VSF_TASK_COMMON_H__
#define __VSF_TASK_COMMON_H__

/*============================ INCLUDES ======================================*/
#include "./kernel/vsf_kernel_cfg.h"

#include "../vsf_eda.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_pt_begin_common(__state)                                          \
            enum {                                                              \
                count_offset = __COUNTER__ + 1,                                 \
            };                                                                  \
            switch ((__state)) {                                                \
                case __COUNTER__ - count_offset:

#define __vsf_pt_entry_common(__state, ...)                                     \
            (__state) = (__COUNTER__ - count_offset + 1) >> 1;                  \
            __VA_ARGS__;                                                        \
            case (__COUNTER__ - count_offset) >> 1:

#define __vsf_pt_end_common()       } vsf_eda_return();
#define __vsf_pt_end_closure_common()                                           \
                }   /* for switch */                                            \
            __vsf_pt_end_common()

#define __vsf_pt_wait_cond_common(__state, ...)                                 \
    do {                                                                        \
        evt = VSF_EVT_INVALID;                                                  \
        __vsf_pt_entry_common(__state);                                         \
        if (__VA_ARGS__){                                                       \
            return ;                                                            \
        }                                                                       \
    } while (0)

#define __vsf_pt_wfe_common(__state, __evt)                                     \
    __vsf_pt_wait_cond_common(__state, (evt != __evt))

#define __vsf_pt_func_common(__NAME)        vsf_pt_func_##__NAME
#define __vsf_pt_common(__NAME)             pt_cb_##__NAME

#define __def_vsf_pt_common(__NAME, ...)                                        \
        struct __vsf_pt_common(__NAME) {                                        \
            __VA_ARGS__                                                         \
        };                                                                      \
        struct __NAME {                                                         \
            implement(vsf_pt_t);                                                \
            implement_ex(__vsf_pt_common(__NAME), param);                       \
        };                                                                      
        

#define __declare_vsf_pt_common(__NAME)                                         \
            typedef struct __NAME __NAME;                                       \
            typedef struct __vsf_pt_common(__NAME) __vsf_pt_common(__NAME);

#define __implement_vsf_pt_common(__NAME, __ARG0)                               \
        void __vsf_pt_func_common(__NAME)(__ARG0, vsf_evt_t evt)

#define __extern_vsf_pt_common(__NAME, __ARG0)                                  \
        extern void __vsf_pt_func_common(__NAME)(__ARG0, vsf_evt_t evt)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif
