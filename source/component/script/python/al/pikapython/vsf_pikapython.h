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

#ifndef __VSF_PIKAPYTHON_H__
#define __VSF_PIKAPYTHON_H__

/*============================ INCLUDES ======================================*/

#include "PikaObj.h"
#include "PikaStdData_List.h"
#include "PikaStdData_Dict.h"
#include "PikaStdData_Tuple.h"

/*============================ MACROS ========================================*/

// features

#ifndef VSF_PYAL_FEATURE_FUNC_ARG_NUM_CHECK
#   define VSF_PYAL_FEATURE_FUNC_ARG_NUM_CHECK      1
#endif
#define VSF_PYAL_FEATURE_MODULE_IS_DYN              1

/*============================ MACROFIED FUNCTIONS ===========================*/

// arg, used as element of list/dict etc

typedef Arg *                                       vsf_pyal_arg_t;

#define vsf_pyal_newarg_str(__str)                  arg_setStr(NULL, "", __str)
#define vsf_pyal_strarg_get_str(__str_arg)          arg_getStr(__str_arg)
#define vsf_pyal_newarg_bytes(__data, __len)        arg_setBytes((uint8_t *)(__data), __len)
#define vsf_pyal_newarg_obj(__obj)                  arg_newObj(__obj)

// object

typedef PikaObj *                                   vsf_pyal_obj_t;
#define VSF_PYAL_OBJ_NULL                           NULL

// list

#define vsf_pyal_newlist(__num, __args)                                         \
    ({                                                                          \
        PikaObj *list = New_PikaList();                                         \
        for (int i = 0; i < (__num); i++) {                                     \
            PikaStdData_List_append(list, ((vsf_pyal_arg_t *)(__args))[i]);     \
        }                                                                       \
        list;                                                                   \
    })
#define vsf_pyal_listobj_append(__listobj, __arg)   pikaList_append((__listobj), (__arg))

// str

#define vsf_pyal_strobj_get_str(__strojb)           arg_getStr(__strojb)

// typle

#define vsf_pyal_newtuple(__size, __args)                                       \
    ({                                                                          \
        PikaObj *tuple = New_PikaTuple();                                       \
        for (int i = 0; i < (__num); i++) {                                     \
            PikaStdData_List_append(tuple, (__args)[i]);                        \
        }                                                                       \
        tuple;                                                                  \
    })

// dict

typedef PikaObjEvt                                  vsf_pyal_dict_evt_t;
#define VSF_PYAL_DICT_EVT_ON_UPDATE                 PIKAOBJ_EVT_UPDATE
#define VSF_PYAL_DICT_EVT_ON_DELETE                 PIKAOBJ_EVT_DELETE

typedef char *                                      vsf_pyal_dict_key_t;
#define vsf_pyal_dictkey_get_str(__dict_key)        (__dict_key)

#define vsf_pyal_newdict()                          New_PikaDict()

#define vsf_pyal_dict_set_on_update(__dict_obj, __on_update)                    \
    ({                                                                          \
        __dict_obj->on_update = (__on_update);                                  \
    })
#define vsf_pyal_dict_set(__dict_obj, __key_str, __value_arg)                   \
    pikaDict_set((__dict_obj), __key_str, (__value_arg))

// module

#define vsf_pyal_module_add_obj(__mod, __name, __obj)                           \
    ({                                                                          \
        vsf_pyal_arg_t arg = vsf_pyal_newarg_obj(__obj);                        \
        obj_setArg(self, (__name), arg);                                        \
        arg_deinit(arg);                                                        \
    })

#define vsf_pyal_func_arg_strobj(__name)            vsf_pyal_arg_t __name
#define vsf_pyal_func_arg_var(__name)               PikaTuple *__name
#define vsf_pyal_func_arg_var_num(__name)           pikaTuple_getSize(__name)
#define vsf_pyal_func_arg_var_get(__name, __idx)    pikaTuple_getArg(__name, __idx)

#define vsf_pyal_module_func_var_imp(__name, __func, __ret_type, min_arg, max_arg, ...)\
    __ret_type __name ## _ ## __func(PikaObj *self, ##__VA_ARGS__)
#define VSF_PYAL_MODULE_FUNC_ARG_OBJ_0              0
#define VSF_PYAL_MODULE_FUNC_ARG_OBJ_1              1
#define vsf_pyal_module_func_fix_imp(__name, __func, __func_type, __ret_type, ...)\
    __ret_type __name ## _ ## __func(PikaObj *self, ##__VA_ARGS__)
#define vsf_pyal_module_func_init_imp(__name)                                   \
    vsf_pyal_module_func_fix_imp(__name, __init__, VSF_PYAL_MODULE_FUNC_ARG_OBJ_0, void)
#define vsf_pyal_module_func_init_return()

#define vsf_pyal_module_int(__mod, __name, __value)
#define vsf_pyal_module_str(__mod, __name, __str)
#define vsf_pyal_module_func(__mod, __name)
#define vsf_pyal_module_dict(__mod, __name, __dict)
#define vsf_pyal_module(__name, ...)

// APIs

#define vsf_pyal_raise(__fmt, ...)                                              \
    ({                                                                          \
        pika_platform_printf((__fmt), ##__VA_ARGS__);                           \
        obj_setErrorCode(self, -1);                                             \
    })

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
