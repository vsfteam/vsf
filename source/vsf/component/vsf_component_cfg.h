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


//! \note do not move this pre-processor statement to other places
#include "vsf_cfg.h"

#ifndef __VSF_COMPONENT_CFG_H__
#define __VSF_COMPONENT_CFG_H__

//! all service modules use this configuation file

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#ifndef VSF_COMPONENT_ASSERT
#   define VSF_COMPONENT_ASSERT                     ASSERT
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

// internal use only
#define vsf_peda_evthandler_t                       vsf_param_eda_evthandler_t

#define __vsf_component_peda_ifs0(__ifs_name)                                   \
            dcl_vsf_peda_ctx(__ifs_name)                                        \
            def_vsf_peda_ctx(__ifs_name)                                        \
            end_def_vsf_peda_ctx(__ifs_name)

#define __vsf_component_peda_ifs1(__ifs_name, __args)                           \
            dcl_vsf_peda_ctx(__ifs_name)                                        \
            def_vsf_peda_ctx(__ifs_name, def_args(__args))                      \
            end_def_vsf_peda_ctx(__ifs_name)

#define __vsf_component_peda_ifs2(__ifs_name, __params, __args)                 \
            dcl_vsf_peda_ctx(__ifs_name)                                        \
            def_vsf_peda_ctx(__ifs_name, __params, __args)                      \
            end_def_vsf_peda_ctx(__ifs_name)

#define __vsf_component_peda_ifs3(__ifs_name, __params, __args, __locals)       \
            dcl_vsf_peda_ctx(__ifs_name)                                        \
            def_vsf_peda_ctx(__ifs_name, __params, __args, __locals)            \
            end_def_vsf_peda_ctx(__ifs_name)

//prototype:
//  __vsf_component_peda_ifs(__peda_name)
//  __vsf_component_peda_ifs(__peda_name, __args)
#define __vsf_component_peda_ifs(__peda_name, ...)                              \
            __PLOOC_EVAL(__vsf_component_peda_ifs, __VA_ARGS__)                 \
                (__peda_name, ##__VA_ARGS__)

//prototype:
//  __vsf_component_peda_ifs_entry(__peda_name, __ifs_name)
//  __vsf_component_peda_ifs_entry(__peda_name, __ifs_name, __locals)
#define __vsf_component_peda_ifs_entry(__peda_name, __ifs_name,  ...)           \
            dcl_vsf_peda_ctx(__peda_name)                                       \
            def_vsf_peda_ctx(__peda_name,                                       \
                def_args(                                                       \
                    implement(vsf_peda_arg(__ifs_name))                         \
                )                                                               \
                def_locals(                                                     \
                    __VA_ARGS__                                                 \
                )                                                               \
            )                                                                   \
            end_def_vsf_peda_ctx(__peda_name)                                   \
            static imp_vsf_peda(__peda_name)

#define __vsf_component_peda_entry0(__peda_name, __decoration)                  \
            __vsf_component_peda_ifs(__peda_name)                               \
            __decoration imp_vsf_peda(__peda_name)

#define __vsf_component_peda_entry1(__peda_name, __decoration, __args)          \
            __vsf_component_peda_ifs(__peda_name, __args)                       \
            __decoration imp_vsf_peda(__peda_name)

#define __vsf_component_peda_entry2(__peda_name, __decoration, __params, __args)\
            __vsf_component_peda_ifs(__peda_name, __params, __args)             \
            __decoration imp_vsf_peda(__peda_name)

#define __vsf_component_peda_entry3(__peda_name, __decoration, __params, __args, __locals)\
            __vsf_component_peda_ifs(__peda_name, __params, __args, __locals)   \
            __decoration imp_vsf_peda(__peda_name)

//prototype:
//  __vsf_component_peda_xxxx_entry(__peda_name)
//  __vsf_component_peda_xxxx_entry(__peda_name, __args)
//  __vsf_component_peda_xxxx_entry(__peda_name, __params, __args)
//  __vsf_component_peda_xxxx_entry(__peda_name, __params, __args, __locals)
#define __vsf_component_peda_public_entry(__peda_name, ...)                     \
            __PLOOC_EVAL(__vsf_component_peda_entry, __VA_ARGS__)               \
                (__peda_name, , ##__VA_ARGS__)

#define __vsf_component_peda_private_entry(__peda_name, ...)                    \
            __PLOOC_EVAL(__vsf_component_peda_entry, __VA_ARGS__)               \
                (__peda_name, static, ##__VA_ARGS__)

#define __vsf_component_call_peda_ifs0( __peda_name,                            \
                                        __err,                                  \
                                        __entry,                                \
                                        __extra_local_size,                     \
                                        __param)                                \
            {                                                                   \
                *(&(__err)) = vsf_call_peda(__peda_name,                        \
                                            (uintptr_t)(__entry),               \
                                            (__extra_local_size),               \
                                            (uintptr_t)(__param));              \
            }

#define __vsf_component_call_peda_ifsn( __peda_name,                            \
                                        __err,                                  \
                                        __entry,                                \
                                        __extra_local_size,                     \
                                        __param,                                \
                                        ...)                                    \
            {                                                                   \
                vsf_peda_arg(__peda_name) _ = { __VA_ARGS__ };                  \
                *(&(__err)) = vsf_call_peda(__peda_name,                        \
                                            (uintptr_t)(__entry),               \
                                            (__extra_local_size),               \
                                            (uintptr_t)(__param),               \
                                            (uintptr_t)&_);                     \
            }

#define __vsf_component_call_peda_ifs1           __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs2           __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs3           __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs4           __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs5           __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs6           __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs7           __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs8           __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs9           __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs10          __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs11          __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs12          __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs13          __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs14          __vsf_component_call_peda_ifsn
#define __vsf_component_call_peda_ifs15          __vsf_component_call_peda_ifsn

//prototype:
//  __vsf_component_call_peda_ifs(__peda_name, __ret, __entry, __local_size, __param)
//  __vsf_component_call_peda_ifs(__peda_name, __ret, __entry, __local_size, __param, __local_initializer)
#define __vsf_component_call_peda_ifs(__peda_name, __ret, __entry, __local_size, __param, ...)\
            __PLOOC_EVAL(__vsf_component_call_peda_ifs, __VA_ARGS__)            \
                (__peda_name, (__ret), __entry, (__local_size), (__param), ##__VA_ARGS__)

// prototype:
//  __vsf_component_call_peda(__peda_name, __ret, __param)
//  __vsf_component_call_peda(__peda_name, __ret, __param, __args)
#define __vsf_component_call_peda(__peda_name, __ret, __param, ...)             \
            __vsf_component_call_peda_ifs(__peda_name, __ret, vsf_peda_func(__peda_name), 0, (__param), ##__VA_ARGS__)

// public use
#define vsf_component_peda_ifs_entry            __vsf_component_peda_ifs_entry

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */