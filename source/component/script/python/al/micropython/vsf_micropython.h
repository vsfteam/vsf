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

#ifndef __VSF_MICROPYTHON_H__
#define __VSF_MICROPYTHON_H__

/*============================ INCLUDES ======================================*/

#include "py/runtime.h"
#include "py/objstr.h"
#include "py/mperrno.h"
#include "py/runtime.h"

// for VSF_MFOREACH and VSF_VA_NUM_ARGS
#include "utilities/vsf_utilities.h"

/*============================ MACROS ========================================*/

// features

#undef VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
#define VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK          0
#define VSF_PYAL_FEATURE_MODULE_IS_DYN              0

/*============================ MACROFIED FUNCTIONS ===========================*/

// buffer

#define vsf_pyal_buffer_t                           vstr_t
#define vsf_pyal_new_buffer(__size)                                             \
    ({                                                                          \
        vstr_t VSF_MACRO_SAFE_NAME(vstr);                                       \
        vstr_init_len(&VSF_MACRO_SAFE_NAME(vstr), (__size));                    \
        VSF_MACRO_SAFE_NAME(vstr);                                              \
    })
#define vsf_pyal_buffer_get_buffer(__buffer)        (__buffer).buf
#define vsf_pyal_buffer_get_len(__buffer)           (__buffer).len
#define vsf_pyal_buffer_set_len(__buffer, __len)    ((__buffer).len = (__len))
#define vsf_pyal_buffer_free(__buffer)              vstr_clear(&(__buffer))

// arg, used as element of list/dict etc

typedef mp_obj_t                                    vsf_pyal_arg_t;
#define VSF_PYAL_ARG_NULL                           MP_OBJ_NULL

#define vsf_pyal_arg_is_int(__arg)                  mp_obj_is_int(__arg)
#define vsf_pyal_arg_is_str(__arg)                  mp_obj_is_str(__arg)
#define vsf_pyal_arg_is_list(__arg)                 (mp_obj_get_type(__arg) == &mp_type_list)
#define vsf_pyal_arg_is_tuple(__arg)                (mp_obj_get_type(__arg) == &mp_type_tuple)
#define vsf_pyal_arg_is_callable(__arg)             mp_obj_is_callable(__arg)
#define vsf_pyal_arg_free(__arg)

// callable

#define vsf_pyal_call(__func, __arg_num, __kw_num, __args)                      \
    mp_call_function_n_kw((__func), (__arg_num), (__kw_num), (__args))

// int

#define vsf_pyal_newarg_int(__value)                mp_obj_new_int(__value)
#define vsf_pyal_intarg_get_int(__intarg)           mp_obj_get_int(__intarg)

// str

#define vsf_pyal_newarg_str(__str)                  mp_obj_new_str((const char *)(__str), strlen(__str))
#define vsf_pyal_newarg_str_ret(__str)              vsf_pyal_newarg_str(__str)
#define vsf_pyal_newarg_str_ret_and_free(__str)                                 \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(arg) = vsf_pyal_newarg_str(__str);   \
        free(__str);                                                            \
        VSF_MACRO_SAFE_NAME(arg);                                               \
    })
#define vsf_pyal_strarg_get_str(__str_arg)          mp_obj_str_get_str(__str_arg)

// bytes

#define vsf_pyal_newarg_bytes(__data, __len)        mp_obj_new_bytes((const byte *)(__data), __len)
#define vsf_pyal_newarg_bytes_ret(__data, __len)    vsf_pyal_newarg_bytes((__data), (__len))
#define vsf_pyal_newarg_bytes_ret_and_free(__data, __len)                       \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(arg) = vsf_pyal_newarg_bytes((__data), (__len));\
        free(__data);                                                           \
        VSF_MACRO_SAFE_NAME(arg);                                               \
    })
#define vsf_pyal_newarg_bytes_ret_from_buffer(__buffer, __length)               \
    ({                                                                          \
        vsf_pyal_buffer_set_len((__buffer), (__length));                        \
        mp_obj_new_bytes_from_vstr(&(__buffer));                                \
    })    
#define vsf_pyal_bytesarg_get_data(__bytesarg, __len_ptr)                       \
                                                    (uint8_t *)mp_obj_str_get_data((__bytesarg), (__len_ptr))

// bool

#define vsf_pyal_newarg_bool(__bool)                ({ (__bool) ? mp_const_true : mp_const_false; })

// object

typedef mp_obj_t                                    vsf_pyal_obj_t;
#define VSF_PYAL_OBJ_NULL                           MP_OBJ_NULL

// list

#define vsf_pyal_newobj_list(__num, __args_ptr)     mp_obj_new_list((__num), (__args_ptr))
#define vsf_pyal_listobj_append(__listobj, __arg, __free_arg)                   \
                                                    mp_obj_list_append((__listobj), (__arg))
#define vsf_pyal_listobj_append(__listobj, __arg, __free_arg)                   \
                                                    mp_obj_list_append((__listobj), (__arg))
extern size_t vsf_pyal_listobj_get_num(vsf_pyal_obj_t self_in);
extern vsf_pyal_arg_t vsf_pyal_listobj_get_arg(vsf_pyal_obj_t self_in, int idx);

#define vsf_pyal_listarg_get_num                    vsf_pyal_listobj_get_num
#define vsf_pyal_listarg_get_arg                    vsf_pyal_listobj_get_arg

// instance

#define vsf_pyal_inst_base()                        mp_obj_base_t base;
#define vsf_pyal_instobj_get(__instobj)             MP_OBJ_TO_PTR(__instobj)
#define vsf_pyal_instarg_get(__instarg)             MP_OBJ_TO_PTR(__instarg)

// file

typedef struct _mp_obj_file_t {
    vsf_pyal_inst_base()
    union {
        FILE *f;
        int fd;
    };
} mp_obj_file_t;
extern const mp_obj_type_t mp_type_fileio;
extern const mp_obj_type_t mp_type_textio;
#define vsf_pyal_newobj_file(__file, __is_text)                                 \
    ({                                                                          \
        mp_obj_file_t *VSF_MACRO_SAFE_NAME(fileobj) = m_new_obj_with_finaliser(mp_obj_file_t);\
        VSF_MACRO_SAFE_NAME(fileobj)->base.type = (__is_text) ? &mp_type_textio : & mp_type_fileio;\
        VSF_MACRO_SAFE_NAME(fileobj)->f = (__file);                             \
        MP_OBJ_FROM_PTR(VSF_MACRO_SAFE_NAME(fileobj));                          \
    })
#define vsf_pyal_fileobj_get_file(__fileobj)        ((mp_obj_file_t *)(__fileobj))->f
#define vsf_pyal_fileobj_clear(__fileobj)           ((mp_obj_file_t *)(__fileobj))->f = NULL

#define vsf_pyal_newarg_file(__file, __is_text)     vsf_pyal_newobj_file((__file), (__is_text))
#define vsf_pyal_filearg_get_file(__filearg)        vsf_pyal_fileobj_get_file(__filearg)
#define vsf_pyal_filearg_clear(__filearg)           vsf_pyal_fileobj_clear(__filearg)

// tuple

#define vsf_pyal_newobj_tuple(__num, __args)        mp_obj_new_tuple((__num), (__args))
#define vsf_pyal_tupleobj_get_int(__tupleobj, __idx)vsf_pyal_intarg_get_int(vsf_pyal_tupleobj_get_arg((__tupleobj), (__idx)))
#define vsf_pyal_tupleobj_get_str(__tupleobj, __idx)vsf_pyal_strarg_get_str(vsf_pyal_tupleobj_get_arg((__tupleobj), (__idx)))
#define vsf_pyal_tupleobj_get_arg                   vsf_pyal_tuplearg_get_arg

#define vsf_pyal_newarg_tuple(__num, __args)        mp_obj_new_tuple((__num), (__args))
#define vsf_pyal_tuplearg_get_int(__tuplearg, __idx)vsf_pyal_intarg_get_int(vsf_pyal_tuplearg_get_arg((__tuplearg), (__idx)))
#define vsf_pyal_tuplearg_get_str(__tuplearg, __idx)vsf_pyal_strarg_get_str(vsf_pyal_tuplearg_get_arg((__tuplearg), (__idx)))
extern vsf_pyal_arg_t vsf_pyal_tuplearg_get_arg(vsf_pyal_arg_t self_in, int idx);

// dict

typedef mp_obj_t                                    vsf_pyal_dict_key_t;
#define vsf_pyal_dictkey_get_str(__dict_key)        vsf_pyal_strarg_get_str(__dict_key)

#define vsf_pyal_newdict()                          mp_obj_new_dict(0)

#define vsf_pyal_static_dict(__name)                                            \
    mp_obj_dict_t __name MICROPY_OBJ_BASE_ALIGNMENT = {                         \
        .base = {                                                               \
            .type = &mp_type_dict,                                              \
        },                                                                      \
        .map = {                                                                \
            .all_keys_are_qstrs = 1,                                            \
        },                                                                      \
    }
#define vsf_pyal_dictobj_set(__dict_obj, __key_str, __value_arg, __free_arg)    \
    mp_obj_dict_store((__dict_obj), vsf_pyal_newarg_str(__key_str), (__value_arg))

// module

#if VSF_PYAL_FEATURE_MODULE_IS_DYN
#   define vsf_pyal_module_add_obj(__mod, __name, __obj)
#endif

#define vsf_pyal_funcarg_strobj                     vsf_pyal_arg_t
#define vsf_pyal_funcarg_strobj_get_str(__arg)      vsf_pyal_strarg_get_str(__arg)
#define vsf_pyal_funcarg_newstr(__str)              vsf_pyal_newarg_str(__str)
#define vsf_pyal_funcarg_intobj                     vsf_pyal_arg_t
#define vsf_pyal_funcarg_intobj_get_int(__arg)      vsf_pyal_intarg_get_int(__arg)
#define vsf_pyal_funcarg_newint(__value)            vsf_pyal_newarg_int(__value)
#define vsf_pyal_funcarg_boolobj                    vsf_pyal_arg_t
#define vsf_pyal_funcarg_newbool(__bool)            vsf_pyal_newarg_bool(__bool)
#define vsf_pyal_funcarg_bytesobj                   vsf_pyal_arg_t
#define vsf_pyal_funcarg_callable                   vsf_pyal_arg_t
#define vsf_pyal_funcarg_tuple                      vsf_pyal_arg_t
#define vsf_pyal_funcarg_tuple_get_int(__tuplearg, __idx)   vsf_pyal_tuplearg_get_int((__tuplearg), (__idx))
#define vsf_pyal_funcarg_tuple_get_str(__tuplearg, __idx)   vsf_pyal_tuplearg_get_str((__tuplearg), (__idx))
#define vsf_pyal_funcarg_tuple_get_arg(__tuplearg, __idx)   vsf_pyal_tuplearg_get_arg((__tuplearg), (__idx))
#define vsf_pyal_funcarg_var(__name)                size_t __name ## _num, const mp_obj_t *__name ## _arr
#define vsf_pyal_funcarg_var_num(__name)            __name ## _num
#define vsf_pyal_funcarg_var_is_str(__name, __idx)  vsf_pyal_arg_is_str((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_get_str(__name, __idx) vsf_pyal_strarg_get_str((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_is_int(__name, __idx)  vsf_pyal_arg_is_int((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_get_int(__name, __idx) vsf_pyal_intarg_get_int((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_get_arg(__name, __idx) ((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_keyword(__name)            size_t __name ## _num, const mp_obj_t *__name ## _arr, mp_map_t *__name ## _map
#define vsf_pyal_funcarg_void                       void

#define vsf_pyal_func_void_return_t                 mp_obj_t
#define vsf_pyal_func_void_return()                 return mp_const_none

#define vsf_pyal_module_func_var_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name)\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_var(__arg_name));          \
    MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_ ## __mod ## _ ## __func ## _obj, __min_arg, __max_arg, __mod ## _ ## __func);\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_var(__arg_name))

#define __vsf_pyal_module_func_keyword_enum(__arg_name, __key_name)             \
    ARG_ ## __key_name,
#define vsf_pyal_module_func_keyword_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_keyword(__arg_name));      \
    MP_DEFINE_CONST_FUN_OBJ_KW(mp_ ## __mod ## _ ## __func ## _obj, __min_arg, __mod ## _ ## __func);\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_keyword(__arg_name)) {     \
        enum { VSF_MFOREACH_ARG1(__vsf_pyal_module_func_keyword_enum, __arg_name, __VA_ARGS__) };\
        mp_arg_val_t __arg_name ## _val[VSF_VA_NUM_ARGS(__VA_ARGS__)];
#define VSF_PYAL_MODULE_FUNC_KEYWORD_ONLY           MP_ARG_KW_ONLY
#define VSF_PYAL_MODULE_FUNC_KEYWORD_REQUIRED       MP_ARG_REQUIRED
#define vsf_pyal_module_func_keyword_int(__key_name, __key_feature, __default)  \
    [ARG_ ## __key_name] = { MP_QSTR_ ## __key_name, (__key_feature) | MP_ARG_INT, {.u_int = (__default)} }
#define vsf_pyal_module_func_keyword_bool(__key_name, __key_feature, __default) \
    [ARG_ ## __key_name] = { MP_QSTR_ ## __key_name, (__key_feature) | MP_ARG_BOOL, {.u_bool = (__default)} }
#define vsf_pyal_module_func_keyword_arg(__key_name, __key_feature, __default)  \
    [ARG_ ## __key_name] = { MP_QSTR_ ## __key_name, (__key_feature) | MP_ARG_OBJ, {.u_rom_obj = (__default)} }
#define vsf_pyal_module_func_keyword_prepare(__arg_name, ...)                   \
    static const mp_arg_t __arg_name ## _args[] = { __VA_ARGS__ };              \
    mp_arg_parse_all(__arg_name ## _num, __arg_name ## _arr, __arg_name ## _map,\
        MP_ARRAY_SIZE(__arg_name ## _args), __arg_name ## _args, __arg_name ## _val)
#define vsf_pyal_module_func_keyword_get_int(__arg_name, __key_name)            \
    __arg_name ## _val[ARG_ ## __key_name].u_int
#define vsf_pyal_module_func_keyword_get_bool(__arg_name, __key_name)           \
    __arg_name ## _val[ARG_ ## __key_name].u_bool
#define vsf_pyal_module_func_keyword_get_arg(__arg_name, __key_name)            \
    __arg_name ## _val[ARG_ ## __key_name].u_obj
#define vsf_pyal_module_func_keyword_imp_end()      }

#define VSF_PYAL_MODULE_FUNCARG_OBJ_0               MP_DEFINE_CONST_FUN_OBJ_0
#define VSF_PYAL_MODULE_FUNCARG_OBJ_1               MP_DEFINE_CONST_FUN_OBJ_1
#define VSF_PYAL_MODULE_FUNCARG_OBJ_2               MP_DEFINE_CONST_FUN_OBJ_2
#define VSF_PYAL_MODULE_FUNCARG_OBJ_3               MP_DEFINE_CONST_FUN_OBJ_3
#define vsf_pyal_module_func_fix_imp(__mod, __func, __func_type, __ret_type, ...)\
    __ret_type __mod ## _ ## __func(__VA_ARGS__);                               \
    __func_type(mp_ ## __mod ## _ ## __func ## _obj, __mod ## _ ## __func);     \
    __ret_type __mod ## _ ## __func(__VA_ARGS__)

#define vsf_pyal_module_func_init_prototype(__mod)                              \
    extern vsf_pyal_obj_t __mod ## _ ## __init__(void)
#define vsf_pyal_module_func_init_declare(__mod)                                \
    extern mp_rom_obj_t mp_ ## __mod ## _ ## __init__ ## _obj;                  \
    extern vsf_pyal_obj_t __mod ## _ ## __init__(void)
#define vsf_pyal_module_func_init_imp(__mod)                                    \
    vsf_pyal_module_func_fix_imp(__mod, __init__, VSF_PYAL_MODULE_FUNCARG_OBJ_0, vsf_pyal_obj_t, void)
#define vsf_pyal_module_func_init_return()          vsf_pyal_func_void_return()

#define vsf_pyal_module_func_type(__mod, __func, __ret_type, ...)               \
    typedef __ret_type (*__mod ## _ ## __func ## _t)(__VA_ARGS__)
#define vsf_pyal_module_func_prototype(__mod, __func, __ret_type, ...)          \
    extern __ret_type __mod ## _ ## __func(__VA_ARGS__)
#define vsf_pyal_module_func_name(__mod, __func)                                \
    __mod ## _ ## __func
#define vsf_pyal_module_func_call(__func_full_name, ...)                        \
    __func_full_name(__VA_ARGS__)
#define vsf_pyal_module_func_declare(__mod, __func, __ret_type, ...)            \
    extern mp_rom_obj_t mp_ ## __mod ## _ ## __func ## _obj;                    \
    extern __ret_type __mod ## _ ## __func(__VA_ARGS__)

#define vsf_pyal_module_func_type_noarg(__mod, __func, __ret_type)              \
    typedef __ret_type (*__mod ## _ ## __func ## _t)(void)
#define vsf_pyal_module_func_prototype_noarg(__mod, __func, __ret_type)         \
    extern __ret_type __mod ## _ ## __func(void)
#define vsf_pyal_module_func_declare_noarg(__mod, __func, __ret_type)           \
    extern mp_rom_obj_t mp_ ## __mod ## _ ## __func ## _obj;                    \
    extern __ret_type __mod ## _ ## __func(void)

#define vsf_pyal_module_int(__mod, __name, __value) { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_INT(__value) }
#define vsf_pyal_module_str(__mod, __name, __str)   { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_QSTR(MP_QSTR_ ## __str) }
#define vsf_pyal_module_func(__mod, __name)         { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&mp_ ## __mod ## _ ## __name ## _obj) }
#define vsf_pyal_module_dict(__mod, __name, __dict) { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&__dict) }
#define vsf_pyal_module_class(__mod, __name)        { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&mp_type_ ## __mod ## _ ## __name) }
#define vsf_pyal_module_submod(__mod, __name)       { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&(mp_module_ ## __name)) }
#define vsf_pyal_module(__name, ...)                                            \
    STATIC const mp_rom_map_elem_t __name ## _module_globals_table[] = {        \
        { MP_ROM_QSTR(MP_QSTR_ ## __name__), MP_ROM_QSTR(MP_QSTR_ ## __name) }, \
        __VA_ARGS__                                                             \
    };                                                                          \
    STATIC MP_DEFINE_CONST_DICT(__name ## _module_globals, __name ## _module_globals_table);\
    const mp_obj_module_t mp_module_ ## __name = {                              \
        .base = { &mp_type_module },                                            \
        .globals = (mp_obj_dict_t *)&(__name ## _module_globals),               \
    };

// class

#define vsf_pyal_class_arg_get_self(__mod, __class, __name)                     \
    __mod ## _ ## __class ## _t *__name = vsf_pyal_instobj_get(selfobj)
#define vsf_pyal_class_arg_get_self_from(__mod, __class, __name, __instobj)     \
    __mod ## _ ## __class ## _t *__name = vsf_pyal_instobj_get(__instobj)

#define vsf_pyal_class_create(__mod, __class, __exsize, __obj_ptr)              \
    ({                                                                          \
        __mod ## _ ## __class ## _t *VSF_MACRO_SAFE_NAME(inst) = (__mod ## _ ## __class ## _t *)m_malloc(sizeof(__mod ## _ ## __class ## _t) + (__exsize));\
        VSF_MACRO_SAFE_NAME(inst)->base.type = &mp_type_ ## __mod ## _ ## __class;\
        *(__obj_ptr) = MP_OBJ_FROM_PTR(VSF_MACRO_SAFE_NAME(inst));              \
        VSF_MACRO_SAFE_NAME(inst);                                              \
    })

#define vsf_pyal_class_new_keyword_func(__mod, __class, __arg_name, ...)        \
    vsf_pyal_obj_t __mod ## _ ## __class ## _make_new(const mp_obj_type_t *type, size_t __arg_name ## _arg_num, size_t n_kw, const mp_obj_t *__arg_name ## _args) {\
        enum { VSF_MFOREACH_ARG1(__vsf_pyal_module_func_keyword_enum, __arg_name, __VA_ARGS__) };\
        mp_arg_val_t __arg_name ## _val[VSF_VA_NUM_ARGS(__VA_ARGS__)];          \
        __mod ## _ ## __class ## _t *self;

#define vsf_pyal_class_new_func(__mod, __class, __arg_name)                     \
    vsf_pyal_obj_t __mod ## _ ## __class ## _make_new(const mp_obj_type_t *type, size_t __arg_name ## _arg_num, size_t n_kw, const mp_obj_t *__arg_name ## _args) {\
        __mod ## _ ## __class ## _t *self;

#define vsf_pyal_class_new_create(__mod, __class, __exsize)                     \
    ({                                                                          \
        __mod ## _ ## __class ## _t *VSF_MACRO_SAFE_NAME(inst) = (__mod ## _ ## __class ## _t *)m_malloc(sizeof(__mod ## _ ## __class ## _t) + (__exsize));\
        VSF_MACRO_SAFE_NAME(inst)->base.type = &mp_type_ ## __mod ## _ ## __class;\
        VSF_MACRO_SAFE_NAME(inst);                                              \
    })
#define vsf_pyal_class_new_fail(__mod, __class, __fmt, ...)                     \
        vsf_pyal_raise((__fmt), ##__VA_ARGS__);                                 \
        return VSF_PYAL_OBJ_NULL;
#define vsf_pyal_class_new_arg_num(__name)          __name ## _arg_num
#define vsf_pyal_class_new_is_int(__name, __idx)    vsf_pyal_arg_is_int((__name ## _args)[__idx])
#define vsf_pyal_class_new_get_int(__name, __idx)   vsf_pyal_intarg_get_int((__name ## _args)[__idx])
#define vsf_pyal_class_new_get_arg(__name, __idx)   ((__name ## _args)[__idx])
#define vsf_pyal_class_new_func_end()                                           \
        return MP_OBJ_FROM_PTR(self);                                           \
    }

#define vsf_pyal_class_func_var_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name)\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_var(__arg_name));          \
    MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_ ## __mod ## _ ## __func ## _obj, __min_arg, __max_arg, __mod ## _ ## __func);\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_var(__arg_name))
#define vsf_pyal_class_func_fix_imp(__mod, __func, __func_type, __ret_type, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, ##__VA_ARGS__);     \
    __func_type(mp_ ## __mod ## _ ## __func ## _obj, __mod ## _ ## __func);     \
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, ##__VA_ARGS__)

#define vsf_pyal_class_int(__class, __name, __value){ MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_INT(__value) }
#define vsf_pyal_class_str(__class, __name, __str)  { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_QSTR(MP_QSTR_ ## __str) }
#define vsf_pyal_class_func(__class, __name)        { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&mp_ ## __class ## _ ## __name ## _obj) }
#define vsf_pyal_class(__mod, __class, ...)                                     \
    STATIC const mp_rom_map_elem_t __ ## __class ## _locals_dict_table[] = {    \
        __VA_ARGS__                                                             \
    };                                                                          \
    STATIC MP_DEFINE_CONST_DICT(__ ## __class ## _locals_dict, __ ## __class ## _locals_dict_table);\
    MP_DEFINE_CONST_OBJ_TYPE(                                                   \
        mp_type_ ## __mod ## _ ## __class, MP_QSTR_ ## __class, MP_TYPE_FLAG_NONE,\
        make_new, __mod ## _ ## __class ## _make_new,                           \
        locals_dict, &__ ## __class ## _locals_dict                             \
    );
#define vsf_pyal_class_declare(__mod, __class)                                  \
    extern const mp_obj_type_t mp_type_ ## __mod ## _ ## __class;               \
    extern vsf_pyal_obj_t __mod ## _ ## __class ## _make_new(const mp_obj_type_t *type, size_t arg_num, size_t n_kw, const mp_obj_t *args)

#define vsf_pyal_class_new_call(__mod, __class, __args_num, __args)             \
    __mod ## _ ## __class ## _make_new(NULL, (__args_num), 0, (__args))

// APIs

#define vsf_pyal_raise(__fmt, ...)                  mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT(__fmt), ##__VA_ARGS__)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
