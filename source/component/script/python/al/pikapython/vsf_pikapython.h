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
#include "TinyObj.h"
#include "PikaStdData_List.h"
#include "PikaStdData_Dict.h"
#include "PikaStdData_Tuple.h"
#include "PikaStdData_FILEIO.h"

/*============================ MACROS ========================================*/

// features

#ifndef VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
#   define VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK      1
#endif
#define VSF_PYAL_FEATURE_MODULE_IS_DYN              1

/*============================ MACROFIED FUNCTIONS ===========================*/

// arg, used as element of list/dict etc

typedef Arg *                                       vsf_pyal_arg_t;
#define VSF_PYAL_ARG_NULL                           NULL


#define vsf_pyal_newarg_obj(__obj)                  arg_newObj(__obj)
#define vsf_pyal_arg_free(__arg)                    arg_deinit(__arg)

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
#define vsf_pyal_listobj_append(__listobj, __arg, __free_arg)                   \
                                                    pikaList_append((__listobj), (__arg))

// int

#define vsf_pyal_newarg_int(__value)                arg_newInt(__value)

// str

#define vsf_pyal_newarg_str(__str)                  arg_newStr(__str)
#define vsf_pyal_newarg_str_ret(__str, __need_free)                             \
    ({                                                                          \
        obj_setStr(self, __FUNCTION__, (__str));                                \
        if (__need_free) { free(__str); }                                       \
        obj_getStr(self, __FUNCTION__);                                         \
    })
#define vsf_pyal_strarg_get_str(__str_arg)          arg_getStr(__str_arg)

// bytes

#define vsf_pyal_newobj_bytes(__data, __len)
#define vsf_pyal_bytesobj_get_data(__bytesobj, __len_ptr)

#define vsf_pyal_newarg_bytes(__data, __len)        arg_setBytes((uint8_t *)(__data), __len)
#define vsf_pyal_newarg_bytes_ret(__data, __len, __need_free)                   \
    ({                                                                          \
        obj_setBytes(self, __FUNCTION__, (uint8_t *)(__data), (__len));         \
        if (__need_free) { free(__data); }                                      \
        obj_getBytes(self, __FUNCTION__);                                       \
    })
#define vsf_pyal_bytesarg_get_data(__bytesarg, __len_ptr)                       \
    ({                                                                          \
        if ((__len_ptr) != NULL) {                                              \
            *(__len_ptr) = arg_getSize(__bytesarg) - sizeof(size_t) - 1;        \
        }                                                                       \
        arg_getBytes(__bytesarg);                                               \
    })

// file

#define vsf_pyal_newobj_file(__file, __is_text)                                 \
    ({                                                                          \
        (void)(__is_text);                                                      \
        vsf_pyal_obj_t fileobj = newNormalObj(New_PikaStdData_FILEIO);          \
        obj_setPtr(fileobj, "_f", (__file));                                    \
        obj_setStr(fileobj, "_mode", (__is_text) ? "t" : "b");                  \
        fileobj;                                                                \
    })
#define vsf_pyal_fileobj_get_file(__fileobj)        
#define vsf_pyal_fileobj_clear(__fileobj)           

#define vsf_pyal_newarg_file(__file, __is_text)                                 \
    ({                                                                          \
        vsf_pyal_obj_t fileobj = vsf_pyal_newobj_file((__file), (__is_text));   \
        arg_newObj(fileobj);                                                    \
    })   
#define vsf_pyal_filearg_get_file(__filearg)                                    \
    ({                                                                          \
        vsf_pyal_obj_t fileobj = arg_getObj(__filearg);                         \
        (FILE *)obj_getPtr(fileobj, "_f");                                      \
    })
#define vsf_pyal_filearg_clear(__filearg)                                       \
    ({                                                                          \
        vsf_pyal_obj_t fileobj = arg_getObj(__filearg);                         \
        obj_setPtr(fileobj, "_f", NULL);                                        \
    })

// tuple

#define vsf_pyal_newobj_tuple(__num, __args)                                    \
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

#define vsf_pyal_dictobj_set_on_update(__dict_obj, __on_update)                 \
    ({                                                                          \
        __dict_obj->on_update = (__on_update);                                  \
    })
#define vsf_pyal_dictobj_set(__dict_obj, __key_str, __value_arg, __free_arg)    \
                                                    pikaDict_set((__dict_obj), __key_str, (__value_arg))

// module

#define vsf_pyal_module_add_obj(__mod, __name, __obj)                           \
    ({                                                                          \
        vsf_pyal_arg_t arg = vsf_pyal_newarg_obj(__obj);                        \
        obj_setArg(self, (__name), arg);                                        \
        arg_deinit(arg);                                                        \
    })
#define vsf_pyal_module_add_str(__mod, __name, __str)                           \
                                                    obj_setStr(self, (__name), (__str))
#define vsf_pyal_module_add_int(__mod, __name, __value)                         \
                                                    obj_setInt(self, (__name), (__value))

#define vsf_pyal_func_void_return_t                 void
#define vsf_pyal_func_void_return()                 return

#define vsf_pyal_funcarg_strobj                     char *
#define vsf_pyal_funcarg_strobj_get_str(__arg)      (__arg)
#define vsf_pyal_funcarg_newstr(__str)                                          \
    ({                                                                          \
        char *result = obj_cacheStr(self, (__str));                             \
        result;                                                                 \
    })
#define vsf_pyal_funcarg_intobj                     int
#define vsf_pyal_funcarg_intobj_get_int(__arg)      (__arg)
#define vsf_pyal_funcarg_newint(__value)            (__value)
#define vsf_pyal_funcarg_boolobj                    bool
#define vsf_pyal_funcarg_newbool(__bool)            (__bool)
#define vsf_pyal_funcarg_bytesobj                   uint8_t *
#define vsf_pyal_funcarg_var(__name)                PikaTuple *__name
#define vsf_pyal_funcarg_var_num(__name)            pikaTuple_getSize(__name)
#define vsf_pyal_funcarg_var_is_str(__name, __idx)  (pikaTuple_getType((__name), (__idx)) == ARG_TYPE_STRING)
#define vsf_pyal_funcarg_var_get_str(__name, __idx) pikaTuple_getStr((__name), (__idx))
#define vsf_pyal_funcarg_var_is_int(__name, __idx)  (pikaTuple_getType((__name), (__idx)) == ARG_TYPE_INT)
#define vsf_pyal_funcarg_var_get_int(__name, __idx) pikaTuple_getInt((__name), (__idx))
#define vsf_pyal_funcarg_var_get_arg(__name, __idx) pikaTuple_getArg((__name), (__idx))

#define vsf_pyal_module_func_var_imp(__mod, __func, __ret_type, min_arg, max_arg, ...)\
    __ret_type __mod ## _ ## __func(PikaObj *self, ##__VA_ARGS__)
#define VSF_PYAL_MODULE_FUNCARG_OBJ_0               0
#define VSF_PYAL_MODULE_FUNCARG_OBJ_1               1
#define VSF_PYAL_MODULE_FUNCARG_OBJ_2               2
#define vsf_pyal_module_func_fix_imp(__mod, __func, __func_type, __ret_type, ...)\
    __ret_type __mod ## _ ## __func(PikaObj *self, ##__VA_ARGS__)
#define vsf_pyal_module_func_init_imp(__mod)                                    \
    vsf_pyal_module_func_fix_imp(__mod, __init__, VSF_PYAL_MODULE_FUNCARG_OBJ_0, void)
#define vsf_pyal_module_func_init_return()
#define vsf_pyal_module_func_call(__mod, __func, ...)                           \
    __mod ## _ ## __func(self, ##__VA_ARGS__)

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
