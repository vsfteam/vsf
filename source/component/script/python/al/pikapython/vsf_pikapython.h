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
#define vsf_pyal_arg_is_float(__arg)                (arg_getType(__arg) == ARG_TYPE_FLOAT)
#define vsf_pyal_arg_is_str(__arg)                  (arg_getType(__arg) == ARG_TYPE_STRING)
#define vsf_pyal_arg_is_bool(__arg)                 (arg_getType(__arg) == ARG_TYPE_BOOL)
#define vsf_pyal_arg_is_list(__arg)                 arg_isList(__arg)
#define vsf_pyal_arg_is_tuple(__arg)                arg_isTuple(__arg)
#define vsf_pyal_arg_is_callable(__arg)             arg_isCallable(__arg)
#define vsf_pyal_arg_is_iterable(__arg)             arg_isIterable(__arg)
#define vsf_pyal_arg_is_class(__arg, __mod, __class)                            \
    (vsf_pyal_arg_getobj(__arg)->constructor == New_ ## __mod ## _ ## __class)
#define vsf_pyal_arg_getobj(__arg)                  arg_getObj(__arg)
#define vsf_pyal_arg_free(__arg)                    arg_deinit(__arg)

// TODO:
#define vsf_pyal_arg_iter(__arg, ...)

// int

#define vsf_pyal_intvalue_t                         int64_t
#define vsf_pyal_newarg_int(__value)                arg_newInt(__value)
#define vsf_pyal_arg_getint(__intarg)               arg_getInt(__intarg)

// float

#define vsf_pyal_newarg_float(__value)              arg_newFloat(__value)
#define vsf_pyal_arg_getfloat(__floatarg)           arg_getFloat(__floatarg)

// str

#define vsf_pyal_newarg_str(__str)                  arg_newStr(__str)
#define vsf_pyal_newarg_str_ret(__str)                                          \
    ({                                                                          \
        obj_setStr(selfobj, __FUNCTION__, (__str));                             \
        obj_getStr(selfobj, __FUNCTION__);                                      \
    })
#define vsf_pyal_newarg_str_and_free(__str)                                     \
    ({                                                                          \
        obj_setStr(selfobj, __FUNCTION__, (__str));                             \
        free(__str);                                                            \
        obj_getStr(selfobj, __FUNCTION__);                                      \
    })
#define vsf_pyal_arg_getstr(__str_arg)              arg_getStr(__str_arg)

// bytes

#define vsf_pyal_newarg_bytes(__data, __len)        arg_newBytes((uint8_t *)(__data), (__len))
#define vsf_pyal_newarg_bytes_ret(__data, __len)                                \
    ({                                                                          \
        obj_setBytes(selfobj, __FUNCTION__, (uint8_t *)(__data), (__len));      \
        obj_getBytes(selfobj, __FUNCTION__);                                    \
    })
#define vsf_pyal_arg_getbytes(__bytesarg, __len_ptr)                            \
    ({                                                                          \
        if ((__len_ptr) != NULL) {                                              \
            *((uint32_t *)(__len_ptr)) = arg_getSize(__bytesarg) - sizeof(size_t) - 1;\
        }                                                                       \
        arg_getBytes(__bytesarg);                                               \
    })

#define vsf_pyal_newarg_bytes_ret_and_free(__data, __len)                       \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(arg) = vsf_pyal_newarg_bytes((__data), (__len));\
        free(__data);                                                           \
        VSF_MACRO_SAFE_NAME(arg);                                               \
    })
#define vsf_pyal_newfuncarg_bytes_ret_and_free(__data, __len)                   \
    ({                                                                          \
        obj_setBytes(selfobj, __FUNCTION__, (uint8_t *)(__data), (__len));      \
        free(__data);                                                           \
        obj_getBytes(selfobj, __FUNCTION__);                                    \
    })
#define vsf_pyal_newobj_bytes_ret_and_free(__data, __len)                       \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(arg) = vsf_pyal_newarg_bytes((__data), (__len));\
        free(__data);                                                           \
        vsf_pyal_arg_getobj(VSF_MACRO_SAFE_NAME(arg));                          \
    })

// bool

#define vsf_pyal_newarg_bool(__bool)                arg_newBool(__bool)
#define vsf_pyal_arg_getbool(__boolarg)             arg_getBool(__boolarg)

// object

typedef PikaObj *                                   vsf_pyal_obj_t;
#define VSF_PYAL_OBJ_NULL                           NULL

#define vsf_pyal_obj_lookup(__obj, __name)          obj_getArg((__obj), VSF_STR(__name))

// list

#define vsf_pyal_newobj_list(__num, __args)                                     \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(listobj) = New_PikaList();           \
        for (int i = 0; i < (__num); i++) {                                     \
            PikaStdData_List_append(VSF_MACRO_SAFE_NAME(listobj), ((vsf_pyal_arg_t *)(__args))[i]);\
        }                                                                       \
        VSF_MACRO_SAFE_NAME(listobj);                                           \
    })
#define vsf_pyal_newarg_list(__num, __args_ptr)                                 \
    ({                                                                          \
        vsf_pyal_obj_t listobj = vsf_pyal_newobj_list((__num), (__args_ptr));   \
        vsf_pyal_newarg_obj(listobj);                                           \
    })
#define vsf_pyal_listobj_append(__listobj, __arg)   pikaList_append((__listobj), (__arg))

#define vsf_pyal_listobj_getnum(__listobj)          pikaList_getSize(__listobj)
#define vsf_pyal_listobj_getarg(__listobj, __idx)   pikaList_getArg((__listobj), (__idx))

#define vsf_pyal_listarg_getnum(__listarg)          vsf_pyal_listobj_getnum(arg_getObj(__listarg))
#define vsf_pyal_listarg_getarg(__listarg, __idx)   vsf_pyal_listobj_getarg(arg_getObj(__listarg), (__idx))

// instance

#define vsf_pyal_inst_base_def()
// do not use __vsf_pyal_newobj_inst directly, because users MUST find all parents class
//  and call init for all these parent classes
#define __vsf_pyal_newobj_inst(__mod, __class)      newNormalObj(New_ ## __mod ## _ ## __class)
#define vsf_pyal_newobj_inst(__mod, __class)        not supported
#define vsf_pyal_instobj_get(__instobj)             arg_getBytes(obj_getArg((__instobj), "_self"))
#define vsf_pyal_instarg_get(__instarg)             vsf_pyal_instobj_get(vsf_pyal_arg_getobj(__instarg))
#define vsf_pyal_instobj_strkey_addobj(__instobj, __key_str, __obj)             \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(arg) = vsf_pyal_newarg_obj(__obj);   \
        obj_setArg_noCopy((__instobj), (__key_str), VSF_MACRO_SAFE_NAME(arg));  \
    })

// file

#define vsf_pyal_newobj_file(__file, __is_text)                                 \
    ({                                                                          \
        (void)(__is_text);                                                      \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(fileobj) = newNormalObj(New_PikaStdData_FILEIO);\
        obj_setPtr(VSF_MACRO_SAFE_NAME(fileobj), "_f", (__file));               \
        obj_setStr(VSF_MACRO_SAFE_NAME(fileobj), "_mode", (__is_text) ? "t" : "b");\
        VSF_MACRO_SAFE_NAME(fileobj);                                           \
    })

#define vsf_pyal_newarg_file(__file, __is_text)                                 \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(fileobj) = vsf_pyal_newobj_file((__file), (__is_text));\
        arg_newObj(VSF_MACRO_SAFE_NAME(fileobj));                               \
    })
#define vsf_pyal_filearg_getfile(__filearg)                                     \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(fileobj) = arg_getObj(__filearg);    \
        (FILE *)obj_getPtr(VSF_MACRO_SAFE_NAME(fileobj), "_f");                 \
    })
#define vsf_pyal_filearg_clear(__filearg)                                       \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(fileobj) = arg_getObj(__filearg);    \
        obj_setPtr(VSF_MACRO_SAFE_NAME(fileobj), "_f", NULL);                   \
    })

#define vsf_pyal_arg_is_file(__arg)                                             \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(fileobj) = arg_getObj(__arg);        \
        VSF_MACRO_SAFE_NAME(fileobj)->constructor == New_PikaStdData_FILEIO;    \
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
#define vsf_pyal_tupleobj_getint(__tupleobj, __idx) pikaTuple_getInt((__tupleobj), (__idx))
#define vsf_pyal_tupleobj_getstr(__tupleobj, __idx) pikaTuple_getStr((__tupleobj), (__idx))
#define vsf_pyal_tupleobj_getarg(__tupleobj, __idx) pikaTuple_getArg((__tupleobj), (__idx))
#define vsf_pyal_tupleobj_getnum(__tupleobj)        pikaTuple_getSize(__tupleobj)

#define vsf_pyal_newarg_tuple(__num, __args)                                    \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(tupleobj) = vsf_pyal_newobj_tuple((__num), (vsf_pyal_arg_t *)(__args));\
        arg_newObj(VSF_MACRO_SAFE_NAME(tupleobj));                              \
    })
#define vsf_pyal_tuplearg_getint(__tuplearg, __idx)     pikaTuple_getInt(arg_getObj(__tuplearg), (__idx))
#define vsf_pyal_tuplearg_getstr(__tuplearg, __idx)     pikaTuple_getStr(arg_getObj(__tuplearg), (__idx))
#define vsf_pyal_tuplearg_getfloat(__tuplearg, __idx)   pikaTuple_getFloat(arg_getObj(__tuplearg), (__idx))
#define vsf_pyal_tuplearg_getarg(__tuplearg, __idx)     pikaTuple_getArg(arg_getObj(__tuplearg), (__idx))
#define vsf_pyal_tuplearg_getnum(__tuplearg)            pikaTuple_getSize(arg_getObj(__tuplearg))

// dict

typedef char *                                      vsf_pyal_dict_key_t;
#define vsf_pyal_dictkey_getstr(__dict_key)         (__dict_key)

#define vsf_pyal_newobj_dict()                      New_PikaDict()

#define vsf_pyal_dictobj_strkey_set(__dict_obj, __key_str, __value_arg)         \
                                                    pikaDict_set((__dict_obj), __key_str, (__value_arg))

// module

#define vsf_pyal_module_addobj(__mod, __key_str, __obj)                         \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(arg) = vsf_pyal_newarg_obj(__obj);   \
        obj_setArg_noCopy(selfobj, (__key_str), VSF_MACRO_SAFE_NAME(arg));      \
    })
#define vsf_pyal_module_addstr(__mod, __name, __str)                            \
                                                    obj_setStr(selfobj, (__name), (__str))
#define vsf_pyal_module_addint(__mod, __name, __value)                          \
                                                    obj_setInt(selfobj, (__name), (__value))

#define vsf_pyal_funcarg_str_t                      char *
#define vsf_pyal_funcarg_getstr(__arg)              (__arg)
#define vsf_pyal_newfuncarg_str(__str)              obj_cacheStr(selfobj, (__str))

#define vsf_pyal_funcarg_int_t                      int
#define vsf_pyal_funcarg_getint(__arg)              (__arg)
#define vsf_pyal_newfuncarg_int(__value)            (__value)
#define vsf_pyal_funcarg_bool_t                     bool
#define vsf_pyal_funcarg_getbool(__arg)             (__arg)
#define vsf_pyal_newfuncarg_bool(__bool)            (__bool)
#define vsf_pyal_funcarg_float_t                    float
#define vsf_pyal_funcarg_getfloat(__arg)            (__arg)
#define vsf_pyal_newfuncarg_float(__value)          (__value)
#define vsf_pyal_funcarg_bytes_t                    uint8_t *
#define vsf_pyal_funcarg_callable_t                 vsf_pyal_arg_t
#define vsf_pyal_funcarg_tuple_t                    vsf_pyal_obj_t
#define vsf_pyal_funcarg_tuple_getint(__tupleobj, __idx)    vsf_pyal_tupleobj_getint((__tupleobj), (__idx))
#define vsf_pyal_funcarg_tuple_getstr(__tupleobj, __idx)    vsf_pyal_tupleobj_getstr((__tupleobj), (__idx))
#define vsf_pyal_funcarg_tuple_getarg(__tupleobj, __idx)    vsf_pyal_tupleobj_getarg((__tupleobj), (__idx))
#define vsf_pyal_funcarg_var(__name)                vsf_pyal_obj_t __name
#define vsf_pyal_funcarg_var_for_call(__name)       __name
#define vsf_pyal_funcarg_var_num(__name)            pikaTuple_getSize(__name)
#define vsf_pyal_funcarg_var_is_str(__name, __idx)  (pikaTuple_getType((__name), (__idx)) == ARG_TYPE_STRING)
#define vsf_pyal_funcarg_var_getstr(__name, __idx)  pikaTuple_getStr((__name), (__idx))
#define vsf_pyal_funcarg_var_is_int(__name, __idx)  (pikaTuple_getType((__name), (__idx)) == ARG_TYPE_INT)
#define vsf_pyal_funcarg_var_getint(__name, __idx)  pikaTuple_getInt((__name), (__idx))
// use arg_getBool if added in pikascript
//#define vsf_pyal_funcarg_var_getbool(__name, __idx)pikaTuple_getBool((__name), (__idx))
#define vsf_pyal_funcarg_var_getbool(__name, __idx) arg_getBool(pikaTuple_getArg((__name), (__idx)))
#define vsf_pyal_funcarg_var_is_tuple(__name, __idx)(pikaTuple_getType((__name), (__idx)) == ARG_TYPE_TUPLE)
#define vsf_pyal_funcarg_var_getarg(__name, __idx)  pikaTuple_getArg((__name), (__idx))
#define vsf_pyal_funcarg_keyword(__name)            vsf_pyal_obj_t __name ## _args
#define vsf_pyal_funcarg_void
#define vsf_pyal_class_func_var_arg_skip_self(__arg_name)
#define vsf_pyal_class_func_var_arg_restore_self(__arg_name)

#define vsf_pyal_func_void_return_t                 void
#define vsf_pyal_func_void_return()                 return

#define vsf_pyal_module_func_var_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, vsf_pyal_funcarg_var(__arg_name))

#define VSF_PYAL_MODULE_FUNCARG_OBJ_0               0
#define VSF_PYAL_MODULE_FUNCARG_OBJ_1               1
#define VSF_PYAL_MODULE_FUNCARG_OBJ_2               2
#define vsf_pyal_module_func_fix_imp(__mod, __func, __func_type, __ret_type, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, ##__VA_ARGS__)

#define vsf_pyal_keyword_is_str
#define vsf_pyal_keyword(__key_name)                VSF_STR(__key_name)
// TODO: implement vsf_pyal_keyword_getxxx
#define __vsf_pyal_keyword_getint(__arg_name, __key, __idx)
#define __vsf_pyal_keyword_getbool(__arg_name, __key, __idx)
#define __vsf_pyal_keyword_getarg(__arg_name, __key, __idx)
#define __vsf_pyal_keyword_parser_arg(__arg_name)   vsf_pyal_obj_t __name ## _args
extern int __vsf_pyal_parse_tuple_keyword(__vsf_pyal_keyword_parser_arg(arg), const char *format, ...);
#define vsf_pyal_parse_tuple_keyword(__arg_name, ...)   __vsf_pyal_parse_tuple_keyword(__arg_name ## _args, ##__VA_ARGS__)

#define vsf_pyal_keyword_getint_forced(__arg_name, __key_name, __key_idx)       \
    ({                                                                          \
        int result = 0;                                                         \
        if (__arg_name ## _args_num <= (__key_idx)) {                           \
            vsf_pyal_raise("invalie argument");                                 \
        } else {                                                                \
            result = vsf_pyal_arg_getint(pikaDict_getArgByidex(__arg_name ## _args, (__key_idx)));\
        }                                                                       \
        result;                                                                 \
    })
#define vsf_pyal_keyword_getbool_forced(__arg_name, __key_name, __key_idx)      \
    ({                                                                          \
        bool result = false;                                                    \
        if (__arg_name ## _args_num <= (__key_idx)) {                           \
            vsf_pyal_raise("invalie argument");                                 \
        } else {                                                                \
            result = vsf_pyal_arg_getbool(pikaDict_getArgByidex(__arg_name ## _args, (__key_idx)));\
        }                                                                       \
        result;                                                                 \
    })
#define vsf_pyal_keyword_getarg_forced(__arg_name, __key_name, __key_idx)       \
    ({                                                                          \
        vsf_pyal_arg_t result = VSF_PYAL_ARG_NULL;                              \
        if (__arg_name ## _args_num <= (__key_idx)) {                           \
            vsf_pyal_raise("invalie argument");                                 \
        } else {                                                                \
            result = pikaDict_getArgByidex(__arg_name ## _args, (__key_idx));   \
        }                                                                       \
        result;                                                                 \
    })

#define vsf_pyal_keyword_getint(__arg_name, __key_name, __key_idx, __default)   \
    (__arg_name ## _args_num <= (__key_idx)) ? (__default) :                    \
            vsf_pyal_arg_getint(pikaDict_getArgByidex(__arg_name ## _args, (__key_idx)))
#define vsf_pyal_keyword_getbool(__arg_name, __key_name, __key_idx, __default)  \
    (__arg_name ## _args_num <= (__key_idx)) ? (__default) :                    \
            vsf_pyal_arg_getbool(pikaDict_getArgByidex(__arg_name ## _args, (__key_idx)))
#define vsf_pyal_keyword_getarg(__arg_name, __key_name, __key_idx, __default)   \
    (__arg_name ## _args_num <= (__key_idx)) ? (__default) :                    \
            pikaDict_getArgByidex(__arg_name ## _args, (__key_idx))

#define vsf_pyal_module_func_keyword_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, vsf_pyal_funcarg_keyword(__arg_name)) {\
        int __arg_name ## _args_num = pikaDict_getSize(__arg_name ## _args);
#define vsf_pyal_module_func_keyword_imp_end()      }

#define vsf_pyal_module_func_init_prototype(__mod)                              \
    extern void __mod ## _ ## __init__(vsf_pyal_obj_t selfobj)
#define vsf_pyal_module_func_init_declare(__mod)                                \
    extern void __mod ## _ ## __init__(vsf_pyal_obj_t selfobj)
#define vsf_pyal_module_func_init_imp(__mod)                                    \
    vsf_pyal_module_func_fix_imp(__mod, __init__, VSF_PYAL_MODULE_FUNCARG_OBJ_0, void)
#define vsf_pyal_module_func_init_return()

#define vsf_pyal_module_func_type(__mod, __func, __ret_type, ...)               \
    typedef __ret_type (*__mod ## _ ## __func ## _t)(vsf_pyal_obj_t selfobj, ##__VA_ARGS__)
#define vsf_pyal_module_func_prototype(__mod, __func, __ret_type, ...)          \
    extern __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, ##__VA_ARGS__)
#define vsf_pyal_module_func_name(__mod, __func)                                \
    __mod ## _ ## __func
#define vsf_pyal_module_func_call(__func_full_name, ...)                        \
    __func_full_name(selfobj, ##__VA_ARGS__)
#define vsf_pyal_module_func_declare(__mod, __func, __ret_type, ...)            \
    extern __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, ##__VA_ARGS__)

#define vsf_pyal_module_func_type_noarg(__mod, __func, __ret_type)              \
    typedef __ret_type (*__mod ## _ ## __func ## _t)(vsf_pyal_obj_t selfobj)
#define vsf_pyal_module_func_prototype_noarg(__mod, __func, __ret_type)         \
    extern __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj)
#define vsf_pyal_module_func_declare_noarg(__mod, __func, __ret_type)           \
    extern __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj)

// class

#define vsf_pyal_class_getself_from_obj(__mod, __class, __name, __instobj)      \
        __mod ## _ ## __class ## _t *__name = (__mod ## _ ## __class ## _t *)vsf_pyal_instobj_get(__instobj)
#define vsf_pyal_class_getself_from_arg(__mod, __class, __name, __instarg)      \
        __mod ## _ ## __class ## _t *__name = (__mod ## _ ## __class ## _t *)vsf_pyal_instarg_get(__instarg)
#define vsf_pyal_class_arg_getself(__mod, __class, __name)                      \
        __mod ## _ ## __class ## _t *__name = (__mod ## _ ## __class ## _t *)vsf_pyal_instobj_get(selfobj)

// vsf_pyal_class_new will create class as in python
#define vsf_pyal_class_new(__mod, __class, __args_num, __args)                  \
    ({                                                                          \
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(tupleobj) = vsf_pyal_newobj_tuple(__args_num, __args);\
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(selfobj) = __vsf_pyal_newobj_inst(__mod, __class);\
        __mod ## _ ## __class ## ___init__(VSF_MACRO_SAFE_NAME(selfobj), VSF_MACRO_SAFE_NAME(tupleobj));\
        VSF_MACRO_SAFE_NAME(selfobj);                                           \
    })

// vsf_pyal_class_newc will create class in C, user can set a exsize and get a C pointer
#define vsf_pyal_class_newc(__mod, __class, __exsize, __obj_ptr)                \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(instarg) = arg_newBytes(NULL, sizeof(__mod ## _ ## __class ## _t) + (__exsize));\
        vsf_pyal_obj_t VSF_MACRO_SAFE_NAME(selfobj) = __vsf_pyal_newobj_inst(__mod, __class);\
        obj_setArg_noCopy(VSF_MACRO_SAFE_NAME(selfobj), "_self", VSF_MACRO_SAFE_NAME(instarg));\
        *(__obj_ptr) = VSF_MACRO_SAFE_NAME(selfobj);                            \
        (__mod ## _ ## __class ## _t *)arg_getBytes(VSF_MACRO_SAFE_NAME(instarg));\
    })

#define vsf_pyal_class_new_keyword_func(__mod, __class, __arg_name, ...)        \
    void __mod ## _ ## __class ## ___init__(vsf_pyal_obj_t selfobj, vsf_pyal_funcarg_keyword(__arg_name)) {\
        int __arg_name ## _args_num = pikaDict_getSize(__arg_name ## _args);    \
        __mod ## _ ## __class ## _t *self = NULL;

#define vsf_pyal_class_new_func(__mod, __class, __arg_name)                     \
    void __mod ## _ ## __class ## ___init__(vsf_pyal_obj_t selfobj, vsf_pyal_funcarg_var(__arg_name ## _args)) {\
        __mod ## _ ## __class ## _t *self = NULL;

#define vsf_pyal_class_new_create(__mod, __class, __exsize)                     \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(instarg) = arg_newBytes(NULL, sizeof(__mod ## _ ## __class ## _t) + (__exsize));\
        obj_setArg_noCopy(selfobj, "_self", VSF_MACRO_SAFE_NAME(instarg));      \
        (__mod ## _ ## __class ## _t *)arg_getBytes(VSF_MACRO_SAFE_NAME(instarg));\
    })
#define vsf_pyal_class_new_del(__mod, __class, __exsize, __ptr)                 \
        obj_removeArg(selfobj, "_self")
#define vsf_pyal_class_new_fail(__mod, __class, __fmt, ...)                     \
        vsf_pyal_raise((__fmt), ##__VA_ARGS__);                                 \
        return;
#define vsf_pyal_class_new_arg_num(__name)          vsf_pyal_funcarg_var_num(__name ## _args)
#define vsf_pyal_class_new_is_int(__name, __idx)    vsf_pyal_funcarg_var_is_int(__name ## _args, __idx)
#define vsf_pyal_class_new_getint(__name, __idx)    vsf_pyal_funcarg_var_getint(__name ## _args, __idx)
#define vsf_pyal_class_new_getstr(__name, __idx)    vsf_pyal_funcarg_var_getstr(__name ## _args, __idx)
#define vsf_pyal_class_new_getarg(__name, __idx)    vsf_pyal_funcarg_var_getarg(__name ## _args, __idx)
#define vsf_pyal_class_new_func_end()                                           \
        return;                                                                 \
    }

#define vsf_pyal_class_del_func(__mod, __class)                                 \
    void __mod ## _ ## __class ## ___del__(vsf_pyal_obj_t selfobj) {            \
        __mod ## _ ## __class ## _t *self = (__mod ## _ ## __class ## _t *)vsf_pyal_instobj_get(selfobj);
#define vsf_pyal_class_del_func_end()                                           \
        vsf_pyal_func_void_return();                                            \
    }

#define vsf_pyal_class_print_func(__mod, __class)                               \
    vsf_pyal_funcarg_str_t __mod ## _ ## __class ## ___str__(vsf_pyal_obj_t selfobj)
#define vsf_pyal_class_print_func_fmt(__mod, __class, __fmt, ...)               \
    vsf_pyal_funcarg_str_t __mod ## _ ## __class ## ___str__(vsf_pyal_obj_t selfobj) {\
        vsf_pyal_class_arg_getself(__mod, __class, self);                       \
        int len = snprintf(NULL, 0, (__fmt), ##__VA_ARGS__);                    \
        extern vsf_pyal_arg_t arg_set(vsf_pyal_arg_t selfarg, char *name,       \
             ArgType type, uint8_t *content, uint32_t size);                    \
        vsf_pyal_arg_t strarg = arg_set(NULL, "", ARG_TYPE_STRING, NULL, len + 1);\
        obj_setStr(selfobj, "_buf", arg_getStr(strarg));                        \
        arg_deinit(strarg);                                                     \
        char *result = obj_getStr(selfobj, "_buf");                             \
        snprintf(result, len + 1, (__fmt), ##__VA_ARGS__);                      \
        return result;                                                          \
    }

#define vsf_pyal_class_attr_func(__mod, __class, __arg_name)                    \
    extern vsf_pyal_arg_t __mod ## _ ## __class ## _attr(vsf_pyal_obj_t selfobj,\
        vsf_pyal_funcarg_str_t __arg_name ## keyobj, vsf_pyal_arg_t __arg_name ## valuearg,\
        bool __arg_name ## is_del);                                             \
    vsf_pyal_class_func_fix_imp(__mod ## _ ## __class, __getattr__, VSF_PYAL_MODULE_FUNCARG_OBJ_2,\
        vsf_pyal_arg_t, vsf_pyal_funcarg_str_t keyobj)                          \
    {                                                                           \
        return __mod ## _ ## __class ## _attr(selfobj, keyobj, VSF_PYAL_ARG_NULL, false);\
    }                                                                           \
    vsf_pyal_class_func_fix_imp(__mod ## _ ## __class, __delattr__, VSF_PYAL_MODULE_FUNCARG_OBJ_2,\
        vsf_pyal_arg_t, vsf_pyal_funcarg_str_t keyobj)                          \
    {                                                                           \
        return __mod ## _ ## __class ## _attr(selfobj, keyobj, VSF_PYAL_ARG_NULL, true);\
    }                                                                           \
    vsf_pyal_class_func_fix_imp(__mod ## _ ## __class, __setattr__, VSF_PYAL_MODULE_FUNCARG_OBJ_3,\
        vsf_pyal_func_void_return_t, vsf_pyal_funcarg_str_t keyobj, vsf_pyal_arg_t arg)\
    {                                                                           \
        __mod ## _ ## __class ## _attr(selfobj, keyobj, arg, false);            \
        vsf_pyal_func_void_return();                                            \
    }                                                                           \
    vsf_pyal_arg_t __mod ## _ ## __class ## _attr(vsf_pyal_obj_t selfobj,       \
        vsf_pyal_funcarg_str_t __arg_name ## keyobj, vsf_pyal_arg_t __arg_name ## valuearg,\
        bool __arg_name ## is_del)

//#define vsf_pyal_class_attr_get_attr(__arg_name)    vsf_pyal_hash_string(__arg_name ## keyobj)
//#define vsf_pyal_class_attr_const_attr(__attr)      constexpr vsf_pyal_hash_string(VSF_STR(__attr))
#define vsf_pyal_class_attr_is_load(__arg_name)     ((VSF_PYAL_ARG_NULL == __arg_name ## valuearg) && !__arg_name ## is_del)
#define vsf_pyal_class_attr_is_store(__arg_name)    (__arg_name ## valuearg != VSF_PYAL_ARG_NULL)
#define vsf_pyal_class_attr_is_delete(__arg_name)   (__arg_name ## is_del)
#define vsf_pyal_class_attr_get_valuearg(__arg_name)(__arg_name ## valuearg)
#define vsf_pyal_class_attr_ret_load_fail(__arg_name)                           \
    return VSF_PYAL_OBJ_NULL
#define vsf_pyal_class_attr_ret_load_obj(__arg_name, __obj)                     \
    return (__obj)
#define vsf_pyal_class_attr_ret_load_method(__arg_name, __method)               \
    return (__method)
#define vsf_pyal_class_attr_ret_fail(__arg_name)                                \
    return VSF_PYAL_OBJ_NULL
#define vsf_pyal_class_attr_ret_success(__arg_name)                             \
    return VSF_PYAL_OBJ_NULL

#define vsf_pyal_class_func_var_private_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, vsf_pyal_funcarg_var(__arg_name))
#define vsf_pyal_class_func_var_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, vsf_pyal_funcarg_var(__arg_name))
#define vsf_pyal_class_func_fix_imp(__mod, __func, __func_type, __ret_type, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, ##__VA_ARGS__)
#define vsf_pyal_class_func_keyword_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, vsf_pyal_funcarg_keyword(__arg_name)) {\
        int __arg_name ## _args_num = pikaDict_getSize(__arg_name ## _args);
#define vsf_pyal_class_func_keyword_imp_end()       }

#define vsf_pyal_class_func_call_var(__mod, __func, __instobj, arg)             \
    __mod ## _ ## __func(__instobj, vsf_pyal_funcarg_var_for_call(arg))
#define vsf_pyal_class_func_call_fix(__mod, __func, __instobj, ...)             \
    __mod ## _ ## __func(__instobj, ##__VA_ARGS__)

#define vsf_pyal_class_declare(__mod, __class)                                  \
    extern void __mod ## _ ## __class ## ___init__(vsf_pyal_obj_t unused, vsf_pyal_funcarg_var(__arg_name ## _args));\
    extern vsf_pyal_obj_t New_ ## __mod ## _ ## __class(Args *args);            \
    extern const NativeProperty __mod ## _ ## __class ## NativeProp

// APIs

#define vsf_pyal_hash_string(__str)                 hash_time33(__str)

#define vsf_pyal_raise(__fmt, ...)                                              \
    ({                                                                          \
        pika_platform_printf((__fmt), ##__VA_ARGS__);                           \
        obj_setErrorCode(selfobj, -1);                                          \
    })

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
