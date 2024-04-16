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
#include "py/objtype.h"
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
#define vsf_pyal_buffer_getptr(__buffer)            (__buffer).buf
#define vsf_pyal_buffer_getlen(__buffer)            (__buffer).len
#define vsf_pyal_buffer_setlen(__buffer, __len)     ((__buffer).len = (__len))
#define vsf_pyal_buffer_free(__buffer)              vstr_clear(&(__buffer))

// arg, used as element of list/dict etc

typedef mp_obj_t                                    vsf_pyal_arg_t;
#define VSF_PYAL_ARG_NULL                           MP_OBJ_NULL

#define vsf_pyal_arg_is_int(__arg)                  mp_obj_is_int(__arg)
#define vsf_pyal_arg_is_float(__arg)                mp_obj_is_float(__arg)
#define vsf_pyal_arg_is_str(__arg)                  mp_obj_is_str(__arg)
#define vsf_pyal_arg_is_bool(__arg)                 mp_obj_is_bool(__arg)
#define vsf_pyal_arg_is_list(__arg)                 (mp_obj_get_type(__arg) == &mp_type_list)
#define vsf_pyal_arg_is_tuple(__arg)                (mp_obj_get_type(__arg) == &mp_type_tuple)
#define vsf_pyal_arg_is_callable(__arg)             mp_obj_is_callable(__arg)
#define vsf_pyal_arg_is_iterable(__arg)             mp_obj_is_iterable(__arg)
#define vsf_pyal_arg_is_class(__arg, __mod, __class)mp_obj_is_exact_type((__arg), &mp_type_ ## __mod ## _ ## __class)
#define vsf_pyal_arg_getobj(__arg)                  (__arg)
#define vsf_pyal_arg_free(__arg)

#define vsf_pyal_arg_is_child_of(__arg, __mod, __class)                         \
    mp_obj_is_subclass_fast(mp_obj_get_type(__arg), &mp_type_ ## __mod ## _ ## __class)
#define vsf_pyal_arg_getobj_parent(__arg, __parent_mod, __parent_class)         \
    mp_obj_cast_to_native_base((__arg), (mp_const_obj_t)&mp_type_ ## __parent_mod ## _ ## __parent_class)

#define vsf_pyal_arg_iter(__arg, ...)                                           \
    {                                                                           \
        mp_obj_iter_buf_t iter_buf;                                             \
        vsf_pyal_arg_t _, iterable = mp_getiter((__arg), &iter_buf);            \
        while ((_ = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {          \
            __VA_ARGS__                                                         \
        }                                                                       \
    }

// callable

#define vsf_pyal_call(__func, __arg_num, __kw_num, __args)                      \
    mp_call_function_n_kw((__func), (__arg_num), (__kw_num), (__args))

// int

#define vsf_pyal_intvalue_t                         mp_int_t
#define vsf_pyal_newobj_int(__value)                mp_obj_new_int(__value)
#define vsf_pyal_newarg_int(__value)                mp_obj_new_int(__value)
#define vsf_pyal_arg_getint(__intarg)               mp_obj_get_int(__intarg)

// float

#define vsf_pyal_newobj_float(__value)              mp_obj_new_float(__value)
#define vsf_pyal_newarg_float(__value)              mp_obj_new_float(__value)
#define vsf_pyal_arg_getfloat(__floatarg)           mp_obj_get_float(__floatarg)

// str

#define vsf_pyal_newarg_str(__str)                  mp_obj_new_str((const char *)(__str), strlen(__str))
#define vsf_pyal_newarg_str_ret(__str)              vsf_pyal_newarg_str(__str)
#define vsf_pyal_newarg_str_ret_and_free(__str)                                 \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(arg) = vsf_pyal_newarg_str(__str);   \
        free(__str);                                                            \
        VSF_MACRO_SAFE_NAME(arg);                                               \
    })
#define vsf_pyal_arg_getstr(__str_arg)              mp_obj_str_get_str(__str_arg)

// bytes

#define vsf_pyal_newarg_bytes(__data, __len)        mp_obj_new_bytes((const byte *)(__data), __len)
#define vsf_pyal_newarg_bytes_ret(__data, __len)    vsf_pyal_newarg_bytes((__data), (__len))
#define vsf_pyal_newarg_bytes_ret_from_buffer(__buffer, __length)               \
    ({                                                                          \
        vsf_pyal_buffer_setlen((__buffer), (__length));                         \
        mp_obj_new_bytes_from_vstr(&(__buffer));                                \
    })
#define vsf_pyal_arg_getbytes(__bytesarg, __len_ptr)                            \
                                                    (uint8_t *)mp_obj_str_get_data((__bytesarg), (size_t *)(__len_ptr))

#define vsf_pyal_newarg_bytes_ret_and_free(__data, __len)                       \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(arg) = vsf_pyal_newarg_bytes((__data), (__len));\
        free(__data);                                                           \
        VSF_MACRO_SAFE_NAME(arg);                                               \
    })
#define vsf_pyal_newobj_bytes_ret_and_free(__data, __len)                       \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(arg) = vsf_pyal_newarg_bytes_ret_and_free((__data), (__len));\
        vsf_pyal_arg_getobj(VSF_MACRO_SAFE_NAME(arg));                          \
    })
#define vsf_pyal_newfuncarg_bytes_ret_and_free(__data, __len)                   \
    ({                                                                          \
        vsf_pyal_arg_t VSF_MACRO_SAFE_NAME(arg) = vsf_pyal_newarg_bytes((__data), (__len));\
        free(__data);                                                           \
        VSF_MACRO_SAFE_NAME(arg);                                               \
    })
#define vsf_pyal_newobj_bytes_ret_from_buffer(__buffer, __length)               \
    ({                                                                          \
        vsf_pyal_buffer_setlen((__buffer), (__length));                         \
        mp_obj_new_bytes_from_vstr(&(__buffer));                                \
    })

// bool

#define vsf_pyal_newarg_bool(__bool)                ({ (__bool) ? mp_const_true : mp_const_false; })
#define vsf_pyal_arg_getbool(__boolarg)             mp_obj_is_true(__boolarg)

// object

typedef mp_obj_t                                    vsf_pyal_obj_t;
#define VSF_PYAL_OBJ_NULL                           MP_OBJ_NULL
#define vsf_pyal_obj_lookup(__obj, __name)                                      \
    ({                                                                          \
        mp_map_elem_t *elem = NULL;                                             \
        size_t qstr = MP_QSTR_ ## __name;                                       \
        if (mp_obj_is_instance_type(mp_obj_get_type(__obj))) {                  \
            mp_obj_instance_t *self = vsf_pyal_instobj_get(__obj);              \
            elem = mp_map_lookup(&self->members, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP);\
        }                                                                       \
        elem != NULL ? elem->value : mp_load_attr(__obj, qstr);                 \
    })

// list

#define vsf_pyal_newarg_list(__num, __args_ptr)     mp_obj_new_list((__num), (__args_ptr))
#define vsf_pyal_newobj_list(__num, __args_ptr)     mp_obj_new_list((__num), (__args_ptr))
#define vsf_pyal_listobj_contain(__listobj, __arg)                              \
    ({                                                                          \
        bool result = false;                                                    \
        vsf_pyal_arg_iter((__listobj), if (_ == (__arg)) { result = true; break; })\
        result;                                                                 \
    })
#define vsf_pyal_listobj_append(__listobj, __arg)   mp_obj_list_append((__listobj), (__arg))
#define vsf_pyal_listobj_remove(__listobj, __arg)   mp_obj_list_remove((__listobj), (__arg))
#define vsf_pyal_listobj_contain_obj(__listobj, __obj)  vsf_pyal_listobj_contain((__listobj), (__obj))
#define vsf_pyal_listobj_append_obj(__listobj, __obj)   mp_obj_list_append((__listobj), (__obj))
#define vsf_pyal_listobj_remove_obj(__listobj, __obj)   mp_obj_list_remove((__listobj), (__obj))

extern size_t vsf_pyal_listobj_getnum(vsf_pyal_obj_t self_in);
extern vsf_pyal_arg_t vsf_pyal_listobj_getarg(vsf_pyal_obj_t self_in, int idx);

#define vsf_pyal_listarg_getnum                     vsf_pyal_listobj_getnum
#define vsf_pyal_listarg_getarg                     vsf_pyal_listobj_getarg

// instance

#define vsf_pyal_inst_base_init(__mod, __class)                                 \
        .base = &mp_type_ ## __mod ## _ ## __class,
#define vsf_pyal_inst_base_def()                    mp_obj_base_t base;
#define vsf_pyal_instobj_get(__instobj)             MP_OBJ_TO_PTR(__instobj)
#define vsf_pyal_instarg_get(__instarg)             MP_OBJ_TO_PTR(__instarg)
#define vsf_pyal_inst_to_obj(__inst)                MP_OBJ_FROM_PTR(__inst)
#define vsf_pyal_inst_to_arg(__inst)                MP_OBJ_FROM_PTR(__inst)

// file

typedef struct _mp_obj_file_t {
    vsf_pyal_inst_base_def()
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
        vsf_pyal_inst_to_obj(VSF_MACRO_SAFE_NAME(fileobj));                     \
    })

#define vsf_pyal_newarg_file(__file, __is_text)     vsf_pyal_newobj_file((__file), (__is_text))
#define vsf_pyal_filearg_getfile(__filearg)         ((mp_obj_file_t *)(__filearg))->f
#define vsf_pyal_filearg_clear(__filearg)           ((mp_obj_file_t *)(__filearg))->f = NULL

#define vsf_pyal_arg_is_file(__arg)                 ((mp_obj_get_type(__arg) == &mp_type_fileio) || (mp_obj_get_type(__arg) == &mp_type_textio))

// tuple

#define vsf_pyal_newobj_tuple(__num, __args)        mp_obj_new_tuple((__num), (__args))
#define vsf_pyal_tupleobj_getint(__tupleobj, __idx) vsf_pyal_arg_getint(vsf_pyal_tupleobj_getarg((__tupleobj), (__idx)))
#define vsf_pyal_tupleobj_getstr(__tupleobj, __idx) vsf_pyal_arg_getstr(vsf_pyal_tupleobj_getarg((__tupleobj), (__idx)))
extern vsf_pyal_arg_t vsf_pyal_tupleobj_getarg(vsf_pyal_obj_t self_in, int idx);
extern int vsf_pyal_tupleobj_getnum(vsf_pyal_obj_t self_in);

#define vsf_pyal_newarg_tuple(__num, __args)        mp_obj_new_tuple((__num), (__args))
#define vsf_pyal_tuplearg_getnum                    vsf_pyal_tupleobj_getnum
#define vsf_pyal_tuplearg_getint(__tuplearg, __idx)     vsf_pyal_arg_getint(vsf_pyal_tuplearg_getarg((__tuplearg), (__idx)))
#define vsf_pyal_tuplearg_getstr(__tuplearg, __idx)     vsf_pyal_arg_getstr(vsf_pyal_tuplearg_getarg((__tuplearg), (__idx)))
#define vsf_pyal_tuplearg_getfloat(__tuplearg, __idx)   vsf_pyal_arg_getfloat(vsf_pyal_tuplearg_getarg((__tuplearg), (__idx)))
#define vsf_pyal_tuplearg_getarg                    vsf_pyal_tupleobj_getarg

// dict

#define vsf_pyal_dict_type                          mp_type_dict
#define vsf_pyal_static_dict_t                      mp_obj_dict_t
#define vsf_pyal_static_dict_init(__type)                                       \
    {                                                                           \
        .base = {                                                               \
            .type = &(__type),                                                  \
        },                                                                      \
        .map = {                                                                \
            .all_keys_are_qstrs = 1,                                            \
        },                                                                      \
    }
#define vsf_pyal_static_dict(__name)                                            \
     vsf_pyal_static_dict_t __name MICROPY_OBJ_BASE_ALIGNMENT = vsf_pyal_static_dict_init(vsf_pyal_dict_type)
#define vsf_pyal_static_dict_type(__name, __type)                               \
     vsf_pyal_static_dict_t __name MICROPY_OBJ_BASE_ALIGNMENT = vsf_pyal_static_dict_init(__type)

typedef mp_obj_t                                    vsf_pyal_dict_key_t;
#define vsf_pyal_dictkey_getstr(__dictkey)          vsf_pyal_arg_getstr(__dictkey)

#define vsf_pyal_newobj_dict()                      mp_obj_new_dict(0)
#define vsf_pyal_dictobj_strkey_set(__dictobj, __key_str, __value_arg)          \
    mp_obj_dict_store((__dictobj), vsf_pyal_newarg_str(__key_str), (__value_arg))
#define vsf_pyal_dictobj_strkey_get(__dictobj, __key_str)   mp_obj_dict_get((__dictobj), vsf_pyal_newarg_str(__key_str))
#define vsf_pyal_dictobj_strkey_del(__dictobj, __key_str)   mp_obj_dict_delete((__dictobj), vsf_pyal_newarg_str(__key_str))
#define vsf_pyal_dictobj_objkey_set(__dictobj, __keyobj, __value_arg)  mp_obj_dict_store((__dictobj), (__keyobj), (__value_arg))
#define vsf_pyal_dictobj_objkey_get(__dictobj, __keyobj)    mp_obj_dict_get((__dictobj), (__keyobj))
#define vsf_pyal_dictobj_objkey_del(__dictobj, __keyobj)    mp_obj_dict_delete((__dictobj), (__keyobj))

#define vsf_pyal_newarg_dict()                      mp_obj_new_dict(0)
#define vsf_pyal_dictarg_set(__dictarg, __key_str, __value_arg)                 \
    mp_obj_dict_store((__dictarg), vsf_pyal_newarg_str(__key_str), (__value_arg))
#define vsf_pyal_dictarg_get(__dictarg, __key_str)  mp_obj_dict_get((__dictarg), vsf_pyal_newarg_str(__key_str))
#define vsf_pyal_dictarg_del(__dictarg, __key_str)  mp_obj_dict_delete((__dictarg), vsf_pyal_newarg_str(__key_str))

// module

#define vsf_pyal_funcarg_str_t                      vsf_pyal_arg_t
#define vsf_pyal_funcarg_getstr(__arg)              vsf_pyal_arg_getstr(__arg)
#define vsf_pyal_newfuncarg_str(__str)              vsf_pyal_newarg_str(__str)
#define vsf_pyal_funcarg_int_t                      vsf_pyal_arg_t
#define vsf_pyal_funcarg_getint(__arg)              vsf_pyal_arg_getint(__arg)
#define vsf_pyal_newfuncarg_int(__value)            vsf_pyal_newarg_int(__value)
#define vsf_pyal_funcarg_bool_t                     vsf_pyal_arg_t
#define vsf_pyal_funcarg_getbool(__arg)             vsf_pyal_arg_getbool(__arg)
#define vsf_pyal_newfuncarg_bool(__bool)            vsf_pyal_newarg_bool(__bool)
#define vsf_pyal_funcarg_float_t                    vsf_pyal_arg_t
#define vsf_pyal_funcarg_getfloat(__arg)            vsf_pyal_arg_getfloat(__arg)
#define vsf_pyal_newfuncarg_float(__value)          vsf_pyal_newarg_float(__value)
#define vsf_pyal_funcarg_bytes_t                    vsf_pyal_arg_t
#define vsf_pyal_funcarg_callable_t                 vsf_pyal_arg_t
#define vsf_pyal_funcarg_tuple_t                    vsf_pyal_arg_t
#define vsf_pyal_funcarg_tuple_getint(__tuplearg, __idx)    vsf_pyal_tuplearg_getint((__tuplearg), (__idx))
#define vsf_pyal_funcarg_tuple_getstr(__tuplearg, __idx)    vsf_pyal_tuplearg_getstr((__tuplearg), (__idx))
#define vsf_pyal_funcarg_tuple_getarg(__tuplearg, __idx)    vsf_pyal_tuplearg_getarg((__tuplearg), (__idx))
#define vsf_pyal_funcarg_var(__name)                size_t __name ## _num, const vsf_pyal_arg_t *__name ## _arr
#define vsf_pyal_funcarg_var_for_call(__name)       __name ## _num, __name ## _arr
#define vsf_pyal_funcarg_var_num(__name)            __name ## _num
#define vsf_pyal_funcarg_var_is_str(__name, __idx)  vsf_pyal_arg_is_str((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_getstr(__name, __idx)  vsf_pyal_arg_getstr((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_is_int(__name, __idx)  vsf_pyal_arg_is_int((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_getint(__name, __idx)  vsf_pyal_arg_getint((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_getbool(__name, __idx) vsf_pyal_arg_getbool((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_is_tuple(__name, __idx)vsf_pyal_arg_is_tuple((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_is_iterable(__name, __idx)                         \
                                                    vsf_pyal_arg_is_iterable((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_getarg(__name, __idx)  ((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_keyword(__name)            size_t __name ## _num, const vsf_pyal_arg_t *__name ## _arr, mp_map_t *__name ## _map
#define vsf_pyal_funcarg_void                       void
#define vsf_pyal_class_func_var_arg_skip_self(__arg_name)                       \
    vsf_pyal_obj_t selfobj = ((__arg_name ## _arr)[0]);                         \
    VSF_UNUSED_PARAM(selfobj);                                                  \
    __arg_name ## _arr++;                                                       \
    __arg_name ## _num--
#define vsf_pyal_class_func_var_arg_restore_self(__arg_name)                    \
    __arg_name ## _arr--;                                                       \
    __arg_name ## _num++

#define vsf_pyal_func_void_return_t                 vsf_pyal_arg_t
#define vsf_pyal_func_void_return()                 return mp_const_none

#define vsf_pyal_module_func_var_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name)\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_var(__arg_name));          \
    MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_ ## __mod ## _ ## __func ## _obj, __min_arg, __max_arg, __mod ## _ ## __func);\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_var(__arg_name))

#define vsf_pyal_keyword_is_int
#define vsf_pyal_keyword(__key_name)                ARG_ ## __key_name
// called in __vsf_pyal_parse_tuple_keyword only
#define __vsf_pyal_keyword_getint(__arg_name, __key, __idx)     __arg_name ## _val[__key].u_int
#define __vsf_pyal_keyword_getbool(__arg_name, __key, __idx)    __arg_name ## _val[__key].u_bool
#define __vsf_pyal_keyword_getarg(__arg_name, __key, __idx)     __arg_name ## _val[__key].u_obj
#define __vsf_pyal_keyword_parser_arg(__arg_name)   mp_arg_val_t *__arg_name ## _val
extern int __vsf_pyal_parse_tuple_keyword(__vsf_pyal_keyword_parser_arg(arg), const char *format, ...);
#define vsf_pyal_parse_tuple_keyword(__arg_name, ...)   __vsf_pyal_parse_tuple_keyword(__arg_name ## _val, ##__VA_ARGS__)

#define __vsf_pyal_keyword_enum(__arg_name, __key_name)                         \
    ARG_ ## __key_name,
#define VSF_PYAL_KEYWORD_ONLY                       MP_ARG_KW_ONLY
#define VSF_PYAL_KEYWORD_REQUIRED                   MP_ARG_REQUIRED
#define vsf_pyal_keyword_prepare_int(__key_name, __key_feature, __default)      \
    [ARG_ ## __key_name] = { MP_QSTR_ ## __key_name, (__key_feature) | MP_ARG_INT, {.u_int = (__default)} }
#define vsf_pyal_keyword_prepare_bool(__key_name, __key_feature, __default)     \
    [ARG_ ## __key_name] = { MP_QSTR_ ## __key_name, (__key_feature) | MP_ARG_BOOL, {.u_bool = (__default)} }
#define vsf_pyal_keyword_prepare_arg(__key_name, __key_feature, __default)      \
    [ARG_ ## __key_name] = { MP_QSTR_ ## __key_name, (__key_feature) | MP_ARG_OBJ, {.u_rom_obj = (__default)} }
#define vsf_pyal_keyword_prepare(__arg_name, ...)                               \
    STATIC const mp_arg_t __arg_name ## _args[] = { __VA_ARGS__ };              \
    mp_arg_parse_all(__arg_name ## _num, __arg_name ## _arr, __arg_name ## _map,\
        MP_ARRAY_SIZE(__arg_name ## _args), __arg_name ## _args, __arg_name ## _val)
#define vsf_pyal_keyword_getint_forced(__arg_name, __key_name, __key_idx)       \
    __arg_name ## _val[ARG_ ## __key_name].u_int
#define vsf_pyal_keyword_getbool_forced(__arg_name, __key_name, __key_idx)      \
    __arg_name ## _val[ARG_ ## __key_name].u_bool
#define vsf_pyal_keyword_getarg_forced(__arg_name, __key_name, __key_idx)       \
    __arg_name ## _val[ARG_ ## __key_name].u_obj
#define vsf_pyal_keyword_getint(__arg_name, __key_name, __key_idx, __default)   \
    __arg_name ## _val[ARG_ ## __key_name].u_int
#define vsf_pyal_keyword_getbool(__arg_name, __key_name, __key_idx, __default)  \
    __arg_name ## _val[ARG_ ## __key_name].u_bool
#define vsf_pyal_keyword_getarg(__arg_name, __key_name, __key_idx, __default)   \
    __arg_name ## _val[ARG_ ## __key_name].u_obj

#define vsf_pyal_module_func_keyword_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_keyword(__arg_name));      \
    MP_DEFINE_CONST_FUN_OBJ_KW(mp_ ## __mod ## _ ## __func ## _obj, 0, __mod ## _ ## __func);\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_keyword(__arg_name)) {     \
        enum { VSF_MFOREACH_ARG1(__vsf_pyal_keyword_enum, __arg_name, __VA_ARGS__) };\
        mp_arg_val_t __arg_name ## _val[VSF_VA_NUM_ARGS(__VA_ARGS__)];
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

#define vsf_pyal_module_selfobj(__mod)              &mp_module_ ## __mod
#define vsf_pyal_module_declare(__mod)              extern const mp_obj_module_t mp_module_ ## __mod

#define vsf_pyal_module_int(__mod, __name, __value) { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_INT(__value) }
#define vsf_pyal_module_str(__mod, __name, __str)   { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_QSTR(MP_QSTR_ ## __str) }
#define vsf_pyal_module_func(__mod, __name)         { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&mp_ ## __mod ## _ ## __name ## _obj) }
#define vsf_pyal_module_inst(__mod, __name, __inst) { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&__inst) }
#define vsf_pyal_module_dict(__mod, __name, __dict) { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&__dict) }
#define vsf_pyal_module_class(__mod, __name)        { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&mp_type_ ## __mod ## _ ## __name) }
#define vsf_pyal_module_submod(__mod, __name)       { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&(mp_module_ ## __mod ## _ ## __name)) }
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
#define vsf_pyal_submodule(__mod, __name, ...)                                  \
    STATIC const mp_rom_map_elem_t __mod ## _ ## __name ## _module_globals_table[] = {\
        { MP_ROM_QSTR(MP_QSTR_ ## __name__), MP_ROM_QSTR(MP_QSTR_ ## __name) }, \
        __VA_ARGS__                                                             \
    };                                                                          \
    STATIC MP_DEFINE_CONST_DICT(__mod ## _ ## __name ## _module_globals, __mod ## _ ## __name ## _module_globals_table);\
    const mp_obj_module_t mp_module_ ## __mod ## _ ## __name = {                \
        .base = { &mp_type_module },                                            \
        .globals = (mp_obj_dict_t *)&(__mod ## _ ## __name ## _module_globals), \
    };

// class

#define vsf_pyal_builtinclass_declare(__class)                                  \
    extern const mp_obj_type_t mp_type_ ## __class;

#define vsf_pyal_class_getself_from_obj(__mod, __class, __name, __instobj)      \
    __mod ## _ ## __class ## _t *__name = vsf_pyal_instobj_get(__instobj)
#define vsf_pyal_class_getself_from_arg(__mod, __class, __name, __instarg)      \
    __mod ## _ ## __class ## _t *__name = vsf_pyal_instobj_get(__instarg)
#define vsf_pyal_class_arg_getself(__mod, __class, __name)                      \
    __mod ## _ ## __class ## _t *__name = vsf_pyal_instobj_get(selfobj)

// vsf_pyal_class_new will create class as normal python class
#define vsf_pyal_class_new(__mod, __class, __args_num, __args)                  \
    __mod ## _ ## __class ## _make_new(NULL, (__args_num), 0, (__args))

// vsf_pyal_class_newc will create class in C, user can set a exsize and get a C pointer
#define vsf_pyal_class_newc(__mod, __class, __exsize, __obj_ptr)                \
    ({                                                                          \
        __mod ## _ ## __class ## _t *VSF_MACRO_SAFE_NAME(inst) = (__mod ## _ ## __class ## _t *)m_malloc_with_finaliser(sizeof(__mod ## _ ## __class ## _t) + (__exsize));\
        VSF_MACRO_SAFE_NAME(inst)->base.type = &mp_type_ ## __mod ## _ ## __class;\
        *(__obj_ptr) = vsf_pyal_inst_to_obj(VSF_MACRO_SAFE_NAME(inst));         \
        VSF_MACRO_SAFE_NAME(inst);                                              \
    })

#define vsf_pyal_class_new_keyword_func(__mod, __class, __arg_name, ...)        \
    vsf_pyal_obj_t __mod ## _ ## __class ## _make_new(const mp_obj_type_t *type, size_t __arg_name ## _arg_num, size_t __arg_name ## _kw_num, const vsf_pyal_arg_t *__arg_name ## _arr) {\
        enum { VSF_MFOREACH_ARG1(__vsf_pyal_keyword_enum, __arg_name, __VA_ARGS__) };\
        mp_arg_val_t __arg_name ## _val[VSF_VA_NUM_ARGS(__VA_ARGS__)];          \
        __mod ## _ ## __class ## _t *self = NULL;                               \
        VSF_UNUSED_PARAM(__arg_name ## _val);                                   \
        VSF_UNUSED_PARAM(self);
#define vsf_pyal_class_new_func_keyword_prepare(__arg_name, ...)                \
    STATIC const mp_arg_t __arg_name ## _args[] = { __VA_ARGS__ };              \
    mp_arg_parse_all_kw_array(__arg_name ## _arg_num, __arg_name ## _kw_num, __arg_name ## _arr,\
        MP_ARRAY_SIZE(__arg_name ## _args), __arg_name ## _args, __arg_name ## _val)

#define vsf_pyal_class_new_func(__mod, __class, __arg_name)                     \
    vsf_pyal_obj_t __mod ## _ ## __class ## _make_new(const mp_obj_type_t *type, size_t __arg_name ## _arg_num, size_t n_kw, const vsf_pyal_arg_t *__arg_name ## _args) {\
        __mod ## _ ## __class ## _t *self = NULL;                               \
        VSF_UNUSED_PARAM(self);

#define vsf_pyal_class_new_create(__mod, __class, __exsize)                     \
    ({                                                                          \
        __mod ## _ ## __class ## _t *VSF_MACRO_SAFE_NAME(inst) = (__mod ## _ ## __class ## _t *)m_malloc_with_finaliser(sizeof(__mod ## _ ## __class ## _t) + (__exsize));\
        VSF_MACRO_SAFE_NAME(inst)->base.type = &mp_type_ ## __mod ## _ ## __class;\
        VSF_MACRO_SAFE_NAME(inst);                                              \
    })
#define vsf_pyal_class_new_del(__mod, __class, __exsize, __ptr)                 \
        m_free((__ptr), sizeof(__mod ## _ ## __class ## _t) + (__exsize))
#define vsf_pyal_class_new_fail(__mod, __class, __fmt, ...)                     \
        vsf_pyal_raise((__fmt), ##__VA_ARGS__);                                 \
        return VSF_PYAL_OBJ_NULL;
#define vsf_pyal_class_new_arg_num(__name)          __name ## _arg_num
#define vsf_pyal_class_new_is_int(__name, __idx)    vsf_pyal_arg_is_int((__name ## _args)[__idx])
#define vsf_pyal_class_new_getint(__name, __idx)    vsf_pyal_arg_getint((__name ## _args)[__idx])
#define vsf_pyal_class_new_getstr(__name, __idx)    vsf_pyal_arg_getstr((__name ## _args)[__idx])
#define vsf_pyal_class_new_getarg(__name, __idx)    ((__name ## _args)[__idx])
#define vsf_pyal_class_new_func_end()                                           \
        return vsf_pyal_inst_to_obj(self);                                      \
    }

#define vsf_pyal_class_del_func_declare(__mod, __class)                         \
    extern vsf_pyal_obj_t __mod ## _ ## __class ## ___del__(vsf_pyal_obj_t selfobj)
#define vsf_pyal_class_del_func_call(__mod, __class, __selfobj)                 \
    __mod ## _ ## __class ## ___del__(selfobj)
#define vsf_pyal_class_del_func(__mod, __class)                                 \
    vsf_pyal_obj_t __mod ## _ ## __class ## ___del__(vsf_pyal_obj_t selfobj);   \
    VSF_PYAL_MODULE_FUNCARG_OBJ_1(mp_ ## __mod ## _ ## __class ## ___del___obj, __mod ## _ ## __class ## ___del__);\
    vsf_pyal_obj_t __mod ## _ ## __class ## ___del__(vsf_pyal_obj_t selfobj) {  \
        __mod ## _ ## __class ## _t *self = (__mod ## _ ## __class ## _t *)vsf_pyal_instobj_get(selfobj);\
        VSF_UNUSED_PARAM(self);
#define vsf_pyal_class_del_func_end()                                           \
        vsf_pyal_func_void_return();                                            \
    }

#define vsf_pyal_class_print_func(__mod, __class)                               \
    void __mod ## _ ## __class ## _print(const mp_print_t *print, vsf_pyal_obj_t selfobj, mp_print_kind_t kind)
#define vsf_pyal_class_print_func_fmt(__mod, __class, __fmt, ...)               \
    void __mod ## _ ## __class ## _print(const mp_print_t *VSF_MACRO_SAFE_NAME(print), vsf_pyal_obj_t selfobj, mp_print_kind_t VSF_MACRO_SAFE_NAME(kind)) {\
        VSF_UNUSED_PARAM(VSF_MACRO_SAFE_NAME(kind));                            \
        vsf_pyal_class_arg_getself(__mod, __class, self);                       \
        mp_printf(VSF_MACRO_SAFE_NAME(print), (__fmt), ##__VA_ARGS__);          \
    }
#define vsf_pyal_class_print_declare(__mod, __class)                            \
    void __mod ## _ ## __class ## _print(const mp_print_t *VSF_MACRO_SAFE_NAME(print), vsf_pyal_obj_t selfobj, mp_print_kind_t VSF_MACRO_SAFE_NAME(kind))
// vsf_pyal_class_print_call can only be used in vsf_pyal_class_print_func
#define vsf_pyal_class_print_call(__mod, __class)                               \
    __mod ## _ ## __class ## _print(print, selfobj, kind)

#define vsf_pyal_class_iterator_func(__mod, __class)                            \
    vsf_pyal_obj_t __mod ## _ ## __class ## _iter(vsf_pyal_obj_t selfobj, mp_obj_iter_buf_t *iter_buf)

#define vsf_pyal_class_unary_func(__mod, __class)                               \
    vsf_pyal_obj_t __mod ## _ ## __class ## _unary_op(mp_unary_op_t op, vsf_pyal_obj_t selfobj)
#define vsf_pyal_class_binary_func(__mod, __class)                              \
    vsf_pyal_obj_t __mod ## _ ## __class ## _binary_op(mp_binary_op_t op, vsf_pyal_obj_t selfobj, vsf_pyal_obj_t valueobj)

#define vsf_pyal_class_attr_func(__mod, __class, __arg_name)                    \
    void __mod ## _ ## __class ## _attr(vsf_pyal_obj_t selfobj, qstr __arg_name ## _attr, vsf_pyal_arg_t *__arg_name ## _args)
#define vsf_pyal_class_attr_get_attr(__arg_name)    (__arg_name ## _attr)
#define vsf_pyal_class_attr_const_attr(__attr)      (MP_QSTR_ ## __attr)
#define vsf_pyal_class_attr_is_load(__arg_name)     (MP_OBJ_NULL == __arg_name ## _args[0])
#define vsf_pyal_class_attr_is_store(__arg_name)    ((MP_OBJ_SENTINEL == __arg_name ## _args[0]) && (__arg_name ## _args[1] != MP_OBJ_NULL))
#define vsf_pyal_class_attr_is_delete(__arg_name)   ((MP_OBJ_SENTINEL == __arg_name ## _args[0]) && (__arg_name ## _args[1] == MP_OBJ_NULL))
#define vsf_pyal_class_attr_get_valuearg(__arg_name)(__arg_name ## _args[1])
#define vsf_pyal_class_attr_ret_load_fail(__arg_name)                           \
    __arg_name ## _args[1] = MP_OBJ_SENTINEL; return
#define vsf_pyal_class_attr_ret_load_obj(__arg_name, __arg)                     \
    __arg_name ## _args[0] = (__arg); return
#define vsf_pyal_class_attr_ret_load_method(__arg_name, __method)               \
    __arg_name ## _args[0] = (__method); __arg_name ## _args[1] = (selfobj); return
#define vsf_pyal_class_attr_ret_fail(__arg_name)                                \
    return
#define vsf_pyal_class_attr_ret_success(__arg_name)                             \
    __arg_name ## _args[0] = MP_OBJ_NULL; return

#define vsf_pyal_class_subscript_is_load(__arg_name)     (MP_OBJ_SENTINEL == vsf_pyal_class_subscript_valuearg(__arg_name))
#define vsf_pyal_class_subscript_is_store(__arg_name)    ((vsf_pyal_class_subscript_valuearg(__arg_name) != MP_OBJ_SENTINEL) && (vsf_pyal_class_subscript_valuearg(__arg_name) != MP_OBJ_NULL))
#define vsf_pyal_class_subscript_is_delete(__arg_name)   (MP_OBJ_NULL == vsf_pyal_class_subscript_valuearg(__arg_name))
#define vsf_pyal_class_subscript_idxarg(__arg_name)     __arg_name ## _idxarg
#define vsf_pyal_class_subscript_valuearg(__arg_name)   __arg_name ## _valuearg
#define vsf_pyal_class_subscript_func(__mod, __class, __arg_name)               \
    vsf_pyal_obj_t __mod ## _ ## __class ## _subscr(vsf_pyal_obj_t selfobj,     \
        vsf_pyal_arg_t vsf_pyal_class_subscript_idxarg(__arg_name),             \
        vsf_pyal_arg_t vsf_pyal_class_subscript_valuearg(__arg_name))

#define vsf_pyal_class_func_var_private_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name)\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_var(__arg_name))
#define vsf_pyal_class_func_var_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name)\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_var(__arg_name));          \
    MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_ ## __mod ## _ ## __func ## _obj, __min_arg, __max_arg, __mod ## _ ## __func);\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_var(__arg_name))
#define vsf_pyal_class_func_fix_imp(__mod, __func, __func_type, __ret_type, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, ##__VA_ARGS__);     \
    __func_type(mp_ ## __mod ## _ ## __func ## _obj, __mod ## _ ## __func);     \
    __ret_type __mod ## _ ## __func(vsf_pyal_obj_t selfobj, ##__VA_ARGS__)
#define vsf_pyal_class_func_keyword_imp_noarg(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name)\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_keyword(__arg_name));      \
    MP_DEFINE_CONST_FUN_OBJ_KW(mp_ ## __mod ## _ ## __func ## _obj, 0, __mod ## _ ## __func);\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_keyword(__arg_name))
#define vsf_pyal_class_func_keyword_imp(__mod, __func, __ret_type, __min_arg, __max_arg, __arg_name, ...)\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_keyword(__arg_name));      \
    MP_DEFINE_CONST_FUN_OBJ_KW(mp_ ## __mod ## _ ## __func ## _obj, 0, __mod ## _ ## __func);\
    __ret_type __mod ## _ ## __func(vsf_pyal_funcarg_keyword(__arg_name)) {     \
        enum { VSF_MFOREACH_ARG1(__vsf_pyal_keyword_enum, __arg_name, __VA_ARGS__) };\
        mp_arg_val_t __arg_name ## _val[VSF_VA_NUM_ARGS(__VA_ARGS__)];          \
        VSF_UNUSED_PARAM(__arg_name ## _val);
#define vsf_pyal_class_func_keyword_imp_end()       }

#define vsf_pyal_class_func_call_var(__mod, __func, __instobj, arg)             \
    __mod ## _ ## __func(vsf_pyal_funcarg_var_for_call(arg))
#define vsf_pyal_class_func_call_fix(__mod, __func, __instobj, ...)             \
    __mod ## _ ## __func(__instobj, ##__VA_ARGS__)

#define vsf_pyal_class_int(__class, __name, __value){ MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_INT(__value) }
#define vsf_pyal_class_str(__class, __name, __str)  { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_QSTR(MP_QSTR_ ## __str) }
#define vsf_pyal_class_func(__class, __name)        { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&mp_ ## __class ## _ ## __name ## _obj) }

#define __vsf_pyal_class_feature(__mod, __class, __feature)                     \
        __feature, &__mod ## _ ## __class ## _ ## __feature
#define vsf_pyal_class_feature_new(__mod, __class)                              \
        __vsf_pyal_class_feature(__mod, __class, make_new)
#define vsf_pyal_class_feature_attr(__mod, __class)                             \
        __vsf_pyal_class_feature(__mod, __class, attr)
#define vsf_pyal_class_feature_entry(__mod, __class)                            \
        __vsf_pyal_class_feature(__mod, __class, locals_dict)
#define vsf_pyal_class_feature_subscript(__mod, __class)                        \
        __vsf_pyal_class_feature(__mod, __class, subscr)
#define vsf_pyal_class_feature_print(__mod, __class)                            \
        __vsf_pyal_class_feature(__mod, __class, print)
#define vsf_pyal_class_feature_unary(__mod, __class)                            \
        __vsf_pyal_class_feature(__mod, __class, unary_op)
#define vsf_pyal_class_feature_binary(__mod, __class)                           \
        __vsf_pyal_class_feature(__mod, __class, binary_op)
#define vsf_pyal_class_feature_iterator(__mod, __class)                         \
        __vsf_pyal_class_feature(__mod, __class, iter)
#define vsf_pyal_class_feature_parent(__mod, __class, __parent_mod, __parent_class)\
        parent, &mp_type_ ## __parent_mod ## _ ## __parent_class

#define vsf_pyal_class_inherit_func_call(__mod, __class, __builtinclass)        \
    __mod ## _ ## __class ## _inherit_from_ ## __builtinclass()

#define __vsf_pyal_class_call_builtin_feature(__builtinclass, __feature)        \
    mp_type_ ## __builtinclass.slots[mp_type_ ## __builtinclass.slot_index_ ## __feature - 1]
#define vsf_pyal_class_call_builtin_feathre_new(__builtinclass, __arg_name)     \
        ((mp_make_new_fun_t)__vsf_pyal_class_call_builtin_feature(__builtinclass, make_new))(type, __arg_name ## _arg_num, n_kw, __arg_name ## _args)
#define vsf_pyal_class_call_builtin_feature_attr(__builtinclass, __arg_name)    \
        ((mp_attr_fun_t)__vsf_pyal_class_call_builtin_feature(__builtinclass, attr))(selfobj, __target_attr, __arg_name ## _args)
#define vsf_pyal_class_call_builtin_feature_subscript(__builtinclass, __arg_name)\
        ((mp_subscr_fun_t)__vsf_pyal_class_call_builtin_feature(__builtinclass, subscr))(selfobj, __arg_name ## _idxarg, __arg_name ## _valuearg)
#define vsf_pyal_class_call_builtin_feature_unary(__builtinclass, __arg_name)   \
        ((mp_unary_op_fun_t)__vsf_pyal_class_call_builtin_feature(__builtinclass, unary_op))(op, selfobj)
#define vsf_pyal_class_call_builtin_feature_binary(__builtinclass, __arg_name)  \
        ((mp_binary_op_fun_t)__vsf_pyal_class_call_builtin_feature(__builtinclass, binary_op))(op, selfobj, valueobj)

#define vsf_pyal_class_inherit_builtin_feature(__mod, __class, __builtinclass, __feature)\
    vsf_pyal_class_inherit_builtin_feature_ ## __feature(__mod, __class, __builtinclass);
#define vsf_pyal_class_inherit_func(__mod, __class, __builtinclass, ...)        \
    void __mod ## _ ## __class ## _inherit_from_ ## __builtinclass(void) {      \
        vsf_pyal_class_inherit_builtin(__mod, __class, __builtinclass);         \
        VSF_MFOREACH_ARG3(vsf_pyal_class_inherit_builtin_feature, __mod, __class, __builtinclass, __VA_ARGS__)\
    }
#define vsf_pyal_class_inherit_builtin(__mod, __class, __builtinclass)          \
        vsf_pyal_class_type(__mod, __class).flags = mp_type_ ## __builtinclass.flags
#define __vsf_pyal_class_inherit_builtin_feature(__mod, __class, __builtinclass, __feature)\
        vsf_pyal_class_type(__mod, __class).slots[vsf_pyal_class_type(__mod, __class).slot_index_ ## __feature - 1] =\
            mp_type_ ## __builtinclass.slots[mp_type_ ## __builtinclass.slot_index_ ## __feature - 1]
#define vsf_pyal_class_inherit_builtin_feathre_new(__mod, __class, __builtinclass)\
        __vsf_pyal_class_inherit_builtin_feature(__mod, __class, __builtinclass, make_new)
#define vsf_pyal_class_inherit_builtin_feature_attr(__mod, __class, __builtinclass)\
        __vsf_pyal_class_inherit_builtin_feature(__mod, __class, __builtinclass, attr)
#define vsf_pyal_class_inherit_builtin_feature_entry(__mod, __class, __builtinclass)\
        __vsf_pyal_class_inherit_builtin_feature(__mod, __class, __builtinclass, locals_dict)
#define vsf_pyal_class_inherit_builtin_feature_subscript(__mod, __class, __builtinclass)\
        __vsf_pyal_class_inherit_builtin_feature(__mod, __class, __builtinclass, subscr)
#define vsf_pyal_class_inherit_builtin_feature_print(__mod, __class, __builtinclass)\
        __vsf_pyal_class_inherit_builtin_feature(__mod, __class, __builtinclass, print)
#define vsf_pyal_class_inherit_builtin_feature_unary(__mod, __class, __builtinclass)\
        __vsf_pyal_class_inherit_builtin_feature(__mod, __class, __builtinclass, unary_op)
#define vsf_pyal_class_inherit_builtin_feature_binary(__mod, __class, __builtinclass)\
        __vsf_pyal_class_inherit_builtin_feature(__mod, __class, __builtinclass, binary_op)
#define vsf_pyal_class_inherit_builtin_feature_iterator(__mod, __class, __builtinclass)\
        __vsf_pyal_class_inherit_builtin_feature(__mod, __class, __builtinclass, iter)
#define vsf_pyal_class_begin_for_inherit(__mod, __class, __builtinclass)
#define vsf_pyal_class_declare_for_inherit(__mod, __class, __builtinclass)      \
    extern mp_obj_type_t mp_type_ ## __mod ## _ ## __class;                     \
    extern vsf_pyal_obj_t __mod ## _ ## __class ## _make_new(const mp_obj_type_t *type, size_t arg_num, size_t n_kw, const vsf_pyal_arg_t *args);\
    extern void __mod ## _ ## __class ## _inherit_from_ ## __builtinclass(void)
#define vsf_pyal_class_end_for_inherit(__mod, __class, __builtinclass, ...)     \
    MP_DEFINE_OBJ_TYPE(                                                         \
        mp_type_ ## __mod ## _ ## __class, MP_QSTR_ ## __class, MP_TYPE_FLAG_NONE,\
        __VA_ARGS__                                                             \
    );

#define vsf_pyal_class_begin(__mod, __class)
#define vsf_pyal_class_entry(__mod, __class, ...)                               \
    STATIC const mp_rom_map_elem_t __ ## __class ## _locals_dict_table[] = {    \
        __VA_ARGS__                                                             \
    };                                                                          \
    STATIC MP_DEFINE_CONST_DICT(__mod ## _ ## __class ## _locals_dict, __ ## __class ## _locals_dict_table);
#define vsf_pyal_class_end(__mod, __class, ...)                                 \
    MP_DEFINE_CONST_OBJ_TYPE(                                                   \
        mp_type_ ## __mod ## _ ## __class, MP_QSTR_ ## __class, MP_TYPE_FLAG_NONE,\
        __VA_ARGS__                                                             \
    );

#define vsf_pyal_class_declare(__mod, __class)                                  \
    typedef struct __mod ## _ ## __class ## _t __mod ## _ ## __class ## _t;     \
    extern const mp_obj_type_t mp_type_ ## __mod ## _ ## __class;               \
    extern vsf_pyal_obj_t __mod ## _ ## __class ## _make_new(const mp_obj_type_t *type, size_t arg_num, size_t n_kw, const vsf_pyal_arg_t *args)
#define vsf_pyal_class_type(__mod, __class)                                     \
    mp_type_ ## __mod ## _ ## __class

// APIs

#define vsf_pyal_raise(__fmt, ...)                  mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT(__fmt), ##__VA_ARGS__)

#define mp_obj_is_iterable(o)                       MP_OBJ_TYPE_HAS_SLOT(mp_obj_get_type(o), iter)

// enhancement to original implementation

// for non-const object type, can be used to inherit from builtin class
#define MP_DEFINE_OBJ_TYPE_EXPAND(x) x

#define MP_DEFINE_OBJ_TYPE_NARGS_0(_struct_type, _typename, _name, _flags) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags }
#define MP_DEFINE_OBJ_TYPE_NARGS_1(_struct_type, _typename, _name, _flags, f1, v1) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slots = { v1, } }
#define MP_DEFINE_OBJ_TYPE_NARGS_2(_struct_type, _typename, _name, _flags, f1, v1, f2, v2) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slots = { v1, v2, } }
#define MP_DEFINE_OBJ_TYPE_NARGS_3(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slots = { v1, v2, v3, } }
#define MP_DEFINE_OBJ_TYPE_NARGS_4(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slots = { v1, v2, v3, v4, } }
#define MP_DEFINE_OBJ_TYPE_NARGS_5(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slots = { v1, v2, v3, v4, v5, } }
#define MP_DEFINE_OBJ_TYPE_NARGS_6(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slots = { v1, v2, v3, v4, v5, v6, } }
#define MP_DEFINE_OBJ_TYPE_NARGS_7(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slots = { v1, v2, v3, v4, v5, v6, v7, } }
#define MP_DEFINE_OBJ_TYPE_NARGS_8(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7, f8, v8) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slot_index_##f8 = 8, .slots = { v1, v2, v3, v4, v5, v6, v7, v8, } }
#define MP_DEFINE_OBJ_TYPE_NARGS_9(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7, f8, v8, f9, v9) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slot_index_##f8 = 8, .slot_index_##f9 = 9, .slots = { v1, v2, v3, v4, v5, v6, v7, v8, v9, } }
#define MP_DEFINE_OBJ_TYPE_NARGS_10(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7, f8, v8, f9, v9, f10, v10) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slot_index_##f8 = 8, .slot_index_##f9 = 9, .slot_index_##f10 = 10, .slots = { v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, } }
#define MP_DEFINE_OBJ_TYPE_NARGS_11(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7, f8, v8, f9, v9, f10, v10, f11, v11) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slot_index_##f8 = 8, .slot_index_##f9 = 9, .slot_index_##f10 = 10, .slot_index_##f11 = 11, .slots = { v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, } }
#define MP_DEFINE_OBJ_TYPE_NARGS_12(_struct_type, _typename, _name, _flags, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6, f7, v7, f8, v8, f9, v9, f10, v10, f11, v11, f12, v12) _struct_type _typename = { .base = { &mp_type_type }, .name = _name, .flags = _flags, .slot_index_##f1 = 1, .slot_index_##f2 = 2, .slot_index_##f3 = 3, .slot_index_##f4 = 4, .slot_index_##f5 = 5, .slot_index_##f6 = 6, .slot_index_##f7 = 7, .slot_index_##f8 = 8, .slot_index_##f9 = 9, .slot_index_##f10 = 10, .slot_index_##f11 = 11, .slot_index_##f12 = 12, .slots = { v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, } }

#define MP_DEFINE_OBJ_TYPE_NARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, N, ...) MP_DEFINE_OBJ_TYPE_NARGS_##N
#define MP_DEFINE_OBJ_TYPE(...) MP_DEFINE_OBJ_TYPE_EXPAND(MP_DEFINE_OBJ_TYPE_NARGS(__VA_ARGS__, _INV, 12, _INV, 11, _INV, 10, _INV, 9, _INV, 8, _INV, 7, _INV, 6, _INV, 5, _INV, 4, _INV, 3, _INV, 2, _INV, 1, _INV, 0)(mp_obj_type_t, __VA_ARGS__))

#define vsf_pyal_root_strkey_addobj(__key_str, __obj)   vsf_pyal_dictobj_strkey_set(&MP_STATE_VM(dict_main), (__key_str), (__obj))
#define vsf_pyal_root_strkey_getobj(__key_str)      vsf_pyal_dictobj_strkey_get(&MP_STATE_VM(dict_main), (__key_str))
#define vsf_pyal_root_strkey_del(__key_str)         vsf_pyal_dictobj_strkey_del(&MP_STATE_VM(dict_main), (__key_str))

#define vsf_pyal_root_objkey_addobj(__keyobj, __obj)    vsf_pyal_dictobj_objkey_set(&MP_STATE_VM(dict_main), (__keyobj), (__obj))
#define vsf_pyal_root_objkey_getobj(__keyobj)       vsf_pyal_dictobj_objkey_get(&MP_STATE_VM(dict_main), (__keyobj))
#define vsf_pyal_root_objkey_del(__keyobj)          vsf_pyal_dictobj_objkey_del(&MP_STATE_VM(dict_main), (__keyobj))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
