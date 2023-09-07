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

/*============================ MACROS ========================================*/

// features

#undef VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
#define VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK          0
#define VSF_PYAL_FEATURE_MODULE_IS_DYN              0

/*============================ MACROFIED FUNCTIONS ===========================*/

// arg, used as element of list/dict etc

typedef mp_obj_t                                    vsf_pyal_arg_t;

#define vsf_pyal_newarg_str(__str)                  mp_obj_new_str((const char *)(__str), strlen(__str))
#define vsf_pyal_strarg_get_str(__str_arg)          vsf_pyal_strobj_get_str(__str_arg)
#define vsf_pyal_newarg_bytes(__data, __len)        mp_obj_new_bytes((const byte *)(__data), __len)
#define vsf_pyal_free_arg(__arg)

// object

typedef mp_obj_t                                    vsf_pyal_obj_t;
#define VSF_PYAL_OBJ_NULL                           MP_OBJ_NULL

// list

#define vsf_pyal_newlist(__num, __args_ptr)         mp_obj_new_list((__num), (__args_ptr))
#define vsf_pyal_listobj_append(__listobj, __arg, __free_arg)                   \
                                                    mp_obj_list_append((__listobj), (__arg))

// intobj/strobj...

#define vsf_pyal_newobj_int(__value)                mp_obj_new_int(__value)
#define vsf_pyal_intobj_get_int(__intobj)           mp_obj_get_int(__intobj)
#define vsf_pyal_newobj_str(__str)                  mp_obj_new_str((const char *)(__str), strlen(__str))
#define vsf_pyal_strobj_get_str(__strojb)           mp_obj_str_get_str(__strojb)
#define vsf_pyal_newobj_bytes(__data, __len)        mp_obj_new_bytes((const byte *)(__data), __len)
#define vsf_pyal_bytesobj_get_data(__bytesobj, __len_ptr)                       \
                                                    (uint8_t *)mp_obj_str_get_data((__bytesobj), (__len_ptr))

typedef struct _mp_obj_file_t {
    mp_obj_base_t base;
    FILE *f;
} mp_obj_file_t;
extern const mp_obj_type_t mp_type_fileio;
extern const mp_obj_type_t mp_type_textio;
#define vsf_pyal_newobj_file(__file, __is_text)                                 \
    ({                                                                          \
        mp_obj_file_t *fileobj = m_new_obj_with_finaliser(mp_obj_file_t);       \
        fileobj->base.type = (__is_text) ? &mp_type_textio : & mp_type_fileio;  \
        fileobj->f = (__file);                                                  \
        MP_OBJ_FROM_PTR(fileobj);                                               \
    })
#define vsf_pyal_fileobj_get_file(__fileobj)        ((mp_obj_file_t *)(__fileobj))->f

// typle

#define vsf_pyal_newtuple(__size, __args)           mp_obj_new_tuple((__size), (__args))

// dict

typedef mp_obj_dict_op_t vsf_pyal_dict_evt_t;
#define VSF_PYAL_DICT_EVT_ON_UPDATE                 DICT_OP_UPDATE
#define VSF_PYAL_DICT_EVT_ON_DELETE                 DICT_OP_DELETE

typedef mp_obj_t                                    vsf_pyal_dict_key_t;
#define vsf_pyal_dictkey_get_str(__dict_key)        vsf_pyal_strobj_get_str(__dict_key)

#define vsf_pyal_newdict()                          mp_obj_new_dict(0)

#define vsf_pyal_static_dict_cb(__name, __on_update)                            \
    mp_obj_dict_t __name MICROPY_OBJ_BASE_ALIGNMENT = {                         \
        .base = {                                                               \
            .type = &mp_type_dict,                                              \
        },                                                                      \
        .map = {                                                                \
            .all_keys_are_qstrs = 1,                                            \
        },                                                                      \
        .on_update = (__on_update),                                             \
    }
#define vsf_pyal_static_dict(__name)                vsf_pyal_static_dict_cb(__name, NULL)
#define vsf_pyal_dictobj_set_on_update(__dict_obj, __on_update)                 \
    ({                                                                          \
        mp_obj_dict_t *self = MP_OBJ_TO_PTR(__dict_obj);                        \
        self->on_update = (__on_update);                                        \
    })
#define vsf_pyal_dictobj_set(__dict_obj, __key_str, __value_arg, __free_arg)    \
    mp_obj_dict_store((__dict_obj), vsf_pyal_newarg_str(__key_str), (__value_arg))

// module

#if VSF_PYAL_FEATURE_MODULE_IS_DYN
#   define vsf_pyal_module_add_obj(__mod, __name, __obj)
#endif

#define vsf_pyal_funcarg_strobj                     vsf_pyal_arg_t
#define vsf_pyal_funcarg_strobj_get_str(__arg)      vsf_pyal_strarg_get_str(__arg)
#define vsf_pyal_funcarg_var(__name)                size_t __name ## _num, const mp_obj_t *__name ## _arr
#define vsf_pyal_funcarg_var_num(__name)            __name ## _num
#define vsf_pyal_funcarg_var_is_int(__name, __idx)  mp_obj_is_int((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_is_str(__name, __idx)  mp_obj_is_str((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_get_str(__name, __idx) vsf_pyal_strobj_get_str((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_get_int(__name, __idx) vsf_pyal_intobj_get_int((__name ## _arr)[__idx])
#define vsf_pyal_funcarg_var_get_obj(__name, __idx) ((__name ## _arr)[__idx])

#define vsf_pyal_func_void_return_t                 mp_obj_t
#define vsf_pyal_func_void_return()                 return mp_const_none

#define vsf_pyal_module_func_var_imp(__name, __func, __ret_type, min_arg, max_arg, ...)\
    static __ret_type __name ## _ ## __func(__VA_ARGS__);                       \
    MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_vfs_ ## __func ## _obj, min_arg, max_arg, __name ## _ ## __func);\
    static __ret_type __name ## _ ## __func(__VA_ARGS__)
#define VSF_PYAL_MODULE_FUNCARG_OBJ_0               MP_DEFINE_CONST_FUN_OBJ_0
#define VSF_PYAL_MODULE_FUNCARG_OBJ_1               MP_DEFINE_CONST_FUN_OBJ_1
#define VSF_PYAL_MODULE_FUNCARG_OBJ_2               MP_DEFINE_CONST_FUN_OBJ_2
#define vsf_pyal_module_func_fix_imp(__name, __func, __func_type, __ret_type, ...)\
    static __ret_type __name ## _ ## __func(__VA_ARGS__);                       \
    __func_type(mp_vfs_ ## __func ## _obj, __name ## _ ## __func);              \
    static __ret_type __name ## _ ## __func(__VA_ARGS__)
#define vsf_pyal_module_func_init_imp(__name)                                   \
    vsf_pyal_module_func_fix_imp(__name, __init__, VSF_PYAL_MODULE_FUNCARG_OBJ_0, vsf_pyal_obj_t, void)
#define vsf_pyal_module_func_init_return()

#define vsf_pyal_module_int(__mod, __name, __value) { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_INT(__value) }
#define vsf_pyal_module_str(__mod, __name, __str)   { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_QSTR(MP_QSTR_ ## __str) }
#define vsf_pyal_module_func(__mod, __name)         { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&mp_vfs_ ## __name ## _obj) }
#define vsf_pyal_module_dict(__mod, __name, __dict) { MP_ROM_QSTR(MP_QSTR_ ## __name), MP_ROM_PTR(&__dict) }
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

// APIs

#define vsf_pyal_raise(__fmt, ...)                  mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT(__fmt), ##__VA_ARGS__)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
