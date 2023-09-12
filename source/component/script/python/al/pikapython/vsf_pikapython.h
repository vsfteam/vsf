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
#define vsf_pyal_arg_is_int(__arg)                  (arg_getType(__arg) == ARG_TYPE_INT)
#define vsf_pyal_arg_is_str(__arg)                  (arg_getType(__arg) == ARG_TYPE_STRING)
#define vsf_pyal_arg_is_list(__arg)                 arg_isList(__arg)
#define vsf_pyal_arg_is_tuple(__arg)                arg_isTuple(__arg)
#define vsf_pyal_arg_is_callable(__arg)             arg_isCallable(__arg)
#define vsf_pyal_arg_free(__arg)                    arg_deinit(__arg)

// int

#define vsf_pyal_newarg_int(__value)                arg_newInt(__value)
#define vsf_pyal_intarg_get_int(__intarg)           arg_getInt(__intarg)

// float

#define vsf_pyal_newarg_float(__value)              arg_newFloat(__value)
#define vsf_pyal_floatarg_get_float(__floatarg)     arg_getFloat(__floatarg)

// str

#define vsf_pyal_newarg_str(__str)                  arg_newStr(__str)
#define vsf_pyal_newarg_str_ret(__str)                                          \
    ({                                                                          \
        obj_setStr(self, __FUNCTION__, (__str));                                \
        obj_getStr(self, __FUNCTION__);                                         \
    })
#define vsf_pyal_newarg_str_and_free(__str)                                     \
    ({                                                                          \
        obj_setStr(self, __FUNCTION__, (__str));                                \
        free(__str);                                                            \
        obj_getStr(self, __FUNCTION__);                                         \
    })
#define vsf_pyal_strarg_get_str(__str_arg)          arg_getStr(__str_arg)

// bytes

#define vsf_pyal_newarg_bytes(__data, __len)        arg_setBytes((uint8_t *)(__data), __len)
#define vsf_pyal_newarg_bytes_ret(__data, __len)                                \
    ({                                                                          \
        obj_setBytes(self, __FUNCTION__, (uint8_t *)(__data), (__len));         \
        obj_getBytes(self, __FUNCTION__);                                       \
    })
#define vsf_pyal_newarg_bytes_ret_and_free(__data, __len)                       \
    ({                                                                          \
        obj_setBytes(self, __FUNCTION__, (uint8_t *)(__data), (__len));         \
        free(__data);                                                           \
        obj_getBytes(self, __FUNCTION__);                                       \
    })
#define vsf_pyal_bytesarg_get_data(__bytesarg, __len_ptr)                       \
    ({                                                                          \
        if ((__len_ptr) != NULL) {                                              \
            *(__len_ptr) = arg_getSize(__bytesarg) - sizeof(size_t) - 1;        \
        }                                                                       \
        arg_getBytes(__bytesarg);                                               \
    })

// bool

#define vsf_pyal_newarg_bool(__bool)                arg_newBool(__bool)

// object

typedef PikaObj *                                   vsf_pyal_obj_t;
#define VSF_PYAL_OBJ_NULL                           NULL

// list

#define vsf_pyal_newobj_list(__num, __args)                                     \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(listobj) = New_PikaList();           \
        for (int i = 0; i < (__num); i++) {                                     \
            PikaStdData_List_append(VSF_MACRO_SAFE_NAME(listobj), ((vsf_pyal_arg_t *)(__args))[i]);\
        }                                                                       \
        VSF_MACRO_SAFE_NAME(listobj);                                           \
    })
#define vsf_pyal_listobj_append(__listobj, __arg, __free_arg)                   \
                                                    pikaList_append((__listobj), (__arg))

#define vsf_pyal_listobj_get_num(__listobj)         pikaList_getSize(__listobj)
#define vsf_pyal_listobj_get_arg(__listobj, __idx)  pikaList_getArg((__listobj), (__idx))

#define vsf_pyal_listarg_get_num(__listarg)         vsf_pyal_listobj_get_num(arg_getObj(__listarg))
#define vsf_pyal_listarg_get_arg(__listarg, __idx)  vsf_pyal_listobj_get_arg(arg_getObj(__listarg), (__idx))

// instance

#define vsf_pyal_inst_base_def()
#define vsf_pyal_instobj_get(__instobj)             arg_getBytes(obj_getArg((__instobj), "_self"))

// file

#define vsf_pyal_newobj_file(__file, __is_text)                                 \
    ({                                                                          \
        (void)(__is_text);                                                      \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(fileobj) = newNormalObj(New_PikaStdData_FILEIO);\
        obj_setPtr(VSF_MACRO_SAFE_NAME(fileobj), "_f", (__file));               \
        obj_setStr(VSF_MACRO_SAFE_NAME(fileobj), "_mode", (__is_text) ? "t" : "b");\
        VSF_MACRO_SAFE_NAME(fileobj);                                           \
    })
#define vsf_pyal_fileobj_get_file(__fileobj)        
#define vsf_pyal_fileobj_clear(__fileobj)           

#define vsf_pyal_newarg_file(__file, __is_text)                                 \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(fileobj) = vsf_pyal_newobj_file((__file), (__is_text));\
        arg_newObj(VSF_MACRO_SAFE_NAME(fileobj));                               \
    })   
#define vsf_pyal_filearg_get_file(__filearg)                                    \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(fileobj) = arg_getObj(__filearg);    \
        (FILE *)obj_getPtr(VSF_MACRO_SAFE_NAME(fileobj), "_f");                 \
    })
#define vsf_pyal_filearg_clear(__filearg)                                       \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(fileobj) = arg_getObj(__filearg);    \
        obj_setPtr(VSF_MACRO_SAFE_NAME(fileobj), "_f", NULL);                   \
    })

// tuple

#define vsf_pyal_newobj_tuple(__num, __args)                                    \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(tupleobj) = New_PikaTuple();         \
        for (int i = 0; i < (__num); i++) {                                     \
            PikaStdData_List_append(VSF_MACRO_SAFE_NAME(tupleobj), (__args)[i]);\
        }                                                                       \
        VSF_MACRO_SAFE_NAME(tupleobj);                                          \
    })
#define vsf_pyal_tupleobj_get_int(__tupleobj, __idx)pikaTuple_getInt((__tupleobj), (__idx))
#define vsf_pyal_tupleobj_get_str(__tupleobj, __idx)pikaTuple_getStr((__tupleobj), (__idx))
#define vsf_pyal_tupleobj_get_arg(__tupleobj, __idx)pikaTuple_getArg((__tupleobj), (__idx))

#define vsf_pyal_newarg_tuple(__num, __args)                                    \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(tupleobj) = vsf_pyal_newobj_tuple((__num), (__args));\
        arg_newObj(VSF_MACRO_SAFE_NAME(tupleobj));                              \
    })
#define vsf_pyal_tuplearg_get_int(__tuplearg, __idx)pikaTuple_getInt(arg_getObj(__tuplearg), (__idx))
#define vsf_pyal_tuplearg_get_str(__tuplearg, __idx)pikaTuple_getStr(arg_getObj(__tuplearg), (__idx))
#define vsf_pyal_tuplearg_get_arg(__tuplearg, __idx)pikaTuple_getArg(arg_getObj(__tuplearg), (__idx))

// dict

typedef char *                                      vsf_pyal_dict_key_t;
#define vsf_pyal_dictkey_get_str(__dict_key)        (__dict_key)

#define vsf_pyal_newdict()                          New_PikaDict()

#define vsf_pyal_dictobj_set(__dict_obj, __key_str, __value_arg, __free_arg)    \
                                                    pikaDict_set((__dict_obj), __key_str, (__value_arg))

// module

#define vsf_pyal_module_add_obj(__mod, __name, __obj)                           \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(arg) = vsf_pyal_newarg_obj(__obj);   \
        obj_setArg_noCopy(self, (__name), VSF_MACRO_SAFE_NAME(arg));            \
    })
#define vsf_pyal_module_add_str(__mod, __name, __str)                           \
                                                    obj_setStr(self, (__name), (__str))
#define vsf_pyal_module_add_int(__mod, __name, __value)                         \
                                                    obj_setInt(self, (__name), (__value))

#define vsf_pyal_funcarg_strobj                     char *
#define vsf_pyal_funcarg_strobj_get_str(__arg)      (__arg)
#define vsf_pyal_funcarg_newstr(__str)              obj_cacheStr(self, (__str))

#define vsf_pyal_funcarg_intobj                     int
#define vsf_pyal_funcarg_intobj_get_int(__arg)      (__arg)
#define vsf_pyal_funcarg_newint(__value)            (__value)
#define vsf_pyal_funcarg_boolobj                    bool
#define vsf_pyal_funcarg_newbool(__bool)            (__bool)
#define vsf_pyal_funcarg_bytesobj                   uint8_t *
#define vsf_pyal_funcarg_callable                   vsf_pyal_arg_t
#define vsf_pyal_funcarg_tuple                      vsf_pyal_obj_t
#define vsf_pyal_funcarg_tuple_get_int(__tupleobj, __idx)   vsf_pyal_tupleobj_get_int((__tupleobj), (__idx))
#define vsf_pyal_funcarg_tuple_get_str(__tupleobj, __idx)   vsf_pyal_tupleobj_get_str((__tupleobj), (__idx))
#define vsf_pyal_funcarg_tuple_get_arg(__tupleobj, __idx)   vsf_pyal_tupleobj_get_arg((__tupleobj), (__idx))
#define vsf_pyal_funcarg_var(__name)                vsf_pyal_obj_t __name
#define vsf_pyal_funcarg_var_num(__name)            pikaTuple_getSize(__name)
#define vsf_pyal_funcarg_var_is_str(__name, __idx)  (pikaTuple_getType((__name), (__idx)) == ARG_TYPE_STRING)
#define vsf_pyal_funcarg_var_get_str(__name, __idx) pikaTuple_getStr((__name), (__idx))
#define vsf_pyal_funcarg_var_is_int(__name, __idx)  (pikaTuple_getType((__name), (__idx)) == ARG_TYPE_INT)
#define vsf_pyal_funcarg_var_get_int(__name, __idx) pikaTuple_getInt((__name), (__idx))
#define vsf_pyal_funcarg_var_get_arg(__name, __idx) pikaTuple_getArg((__name), (__idx))
#define vsf_pyal_funcarg_keyword(__name)            vsf_pyal_obj_t __name
#define vsf_pyal_funcarg_void

#define vsf_pyal_func_void_return_t                 void
#define vsf_pyal_func_void_return()                 return

#define vsf_pyal_module_func_var_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t self, vsf_pyal_funcarg_var(__arg_name))

#define VSF_PYAL_MODULE_FUNCARG_OBJ_0               0
#define VSF_PYAL_MODULE_FUNCARG_OBJ_1               1
#define VSF_PYAL_MODULE_FUNCARG_OBJ_2               2
#define vsf_pyal_module_func_fix_imp(__mod, __func, __func_type, __ret_type, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t self, ##__VA_ARGS__)

#define vsf_pyal_module_func_keyword_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t self, vsf_pyal_funcarg_keyword(__arg_name)) {
#define vsf_pyal_module_func_keyword_get_int(__arg_name, __key_name)            \
    pikaDict_getInt(__arg_name, #__key_name)
#define vsf_pyal_module_func_keyword_get_bool(__arg_name, __key_name)           \
    pikaDict_getInt(__arg_name, #__key_name)
#define vsf_pyal_module_func_keyword_get_arg(__arg_name, __key_name)            \
    pikaDict_getArg(__arg_name, #__key_name)
#define vsf_pyal_module_func_keyword_imp_end()      }

#define vsf_pyal_module_func_init_prototype(__mod)                              \
    extern void __mod ## _ ## __init__(vsf_pyal_obj_t self)
#define vsf_pyal_module_func_init_declare(__mod)                                \
    extern void __mod ## _ ## __init__(vsf_pyal_obj_t self)
#define vsf_pyal_module_func_init_imp(__mod)                                    \
    vsf_pyal_module_func_fix_imp(__mod, __init__, VSF_PYAL_MODULE_FUNCARG_OBJ_0, void)
#define vsf_pyal_module_func_init_return()

#define vsf_pyal_module_func_type(__mod, __func, __ret_type, ...)               \
    typedef __ret_type (*__mod ## _ ## __func ## _t)(vsf_pyal_obj_t self, ##__VA_ARGS__)
#define vsf_pyal_module_func_prototype(__mod, __func, __ret_type, ...)          \
    extern __ret_type __mod ## _ ## __func(vsf_pyal_obj_t self, ##__VA_ARGS__)
#define vsf_pyal_module_func_name(__mod, __func)                                \
    __mod ## _ ## __func
#define vsf_pyal_module_func_call(__func_full_name, ...)                        \
    __func_full_name(self, ##__VA_ARGS__)
#define vsf_pyal_module_func_declare(__mod, __func, __ret_type, ...)            \
    extern __ret_type __mod ## _ ## __func(vsf_pyal_obj_t self, ##__VA_ARGS__)

#define vsf_pyal_module_func_type_noarg(__mod, __func, __ret_type)              \
    typedef __ret_type (*__mod ## _ ## __func ## _t)(vsf_pyal_obj_t self)
#define vsf_pyal_module_func_prototype_noarg(__mod, __func, __ret_type)         \
    extern __ret_type __mod ## _ ## __func(vsf_pyal_obj_t self)
#define vsf_pyal_module_func_declare_noarg(__mod, __func, __ret_type)           \
    extern __ret_type __mod ## _ ## __func(vsf_pyal_obj_t self)

// class

#define vsf_pyal_class_arg_get_self(__mod, __class, __name)                     \
        __mod ## _ ## __class ## _t *__name = vsf_pyal_instobj_get(selfobj)
#define vsf_pyal_class_arg_get_self_from(__mod, __class, __name, __instobj)     \
        __mod ## _ ## __class ## _t *__name = vsf_pyal_instobj_get(__instobj)

#define vsf_pyal_class_create(__mod, __class, __exsize, __obj_ptr)              \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(instarg) = arg_newBytes(NULL, sizeof(__mod ## _ ## __class ## _t) + (__exsize));\
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(selfobj) = newNormalObj(New_ ## __mod ## _ ## __class);\
        obj_setArg_noCopy(VSF_MACRO_SAFE_NAME(selfobj), "_self", VSF_MACRO_SAFE_NAME(instarg));\
        *(__obj_ptr) = VSF_MACRO_SAFE_NAME(selfobj);                            \
        (__mod ## _ ## __class ## _t *)arg_getBytes(VSF_MACRO_SAFE_NAME(instarg));\
    })

#define vsf_pyal_class_new_keyword_func(__mod, __class, __arg_name, ...)        \
    void __mod ## _ ## __class ## ___init__(vsf_pyal_obj_t selfobj, vsf_pyal_funcarg_keyword(__arg_name ## _args)) {\
        __mod ## _ ## __class ## _t *self;

#define vsf_pyal_class_new_func(__mod, __class, __arg_name)                     \
    void __mod ## _ ## __class ## ___init__(vsf_pyal_obj_t selfobj, vsf_pyal_funcarg_var(__arg_name ## _args)) {\
        __mod ## _ ## __class ## _t *self;

#define vsf_pyal_class_new_create(__mod, __class, __exsize)                     \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(instarg) = arg_newBytes(NULL, sizeof(__mod ## _ ## __class ## _t) + (__exsize));\
        obj_setArg_noCopy(selfobj, "_self", VSF_MACRO_SAFE_NAME(instarg));      \
        (__mod ## _ ## __class ## _t *)arg_getBytes(VSF_MACRO_SAFE_NAME(instarg));\
    })
#define vsf_pyal_class_new_fail(__mod, __class, __fmt, ...)                     \
        vsf_pyal_raise((__fmt), ##__VA_ARGS__);                                 \
        return;
#define vsf_pyal_class_new_arg_num(__name)          vsf_pyal_funcarg_var_num(__name ## _args)
#define vsf_pyal_class_new_is_int(__name, __idx)    vsf_pyal_funcarg_var_is_int(__name ## _args, __idx)
#define vsf_pyal_class_new_get_int(__name, __idx)   vsf_pyal_funcarg_var_get_int(__name ## _args, __idx)
#define vsf_pyal_class_new_get_arg(__name, __idx)   vsf_pyal_funcarg_var_get_arg(__name ## _args, __idx)
#define vsf_pyal_class_new_func_end()                                           \
        return;                                                                 \
    }

#define vsf_pyal_class_func_var_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, vsf_pyal_funcarg_var(__arg_name))
#define vsf_pyal_class_func_fix_imp(__mod, __func, __func_type, __ret_type, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, ##__VA_ARGS__)

#define vsf_pyal_class_declare(__mod, __class)                                  \
    extern void __mod ## _ ## __class ## ___init__(vsf_pyal_obj_t unused, vsf_pyal_funcarg_var(__arg_name ## _args));\
    extern vsf_pyal_obj_t New_ ## __mod ## _ ## __class(Args *args);            \
    extern const NativeProperty __mod ## _ ## __class ## NativeProp

#define vsf_pyal_class_new_call(__mod, __class, __args_num, __args)             \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(tupleobj) = vsf_pyal_newobj_tuple(__args_num, __args);\
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(selfobj) = New_pygame_Color(NULL);   \
        __mod ## _ ## __class ## ___init__(VSF_MACRO_SAFE_NAME(selfobj), VSF_MACRO_SAFE_NAME(tupleobj));\
        vsf_pyal_arg_t selfarg = obj_getArg(VSF_MACRO_SAFE_NAME(selfobj), "_self");\
        (__mod ## _ ## __class ## _t *)arg_getBytes(selfarg);                   \
    })

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
