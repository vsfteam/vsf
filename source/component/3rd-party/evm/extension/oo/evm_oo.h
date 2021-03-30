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

#ifndef __EVM_OO_H__
#define __EVM_OO_H__

/*============================ INCLUDES ======================================*/

#include "vsf_cfg.h"

#if VSF_USE_EVM == ENABLED

#include "evm_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define EVM_CLASS_VT_METHOD(__NAME, __FUNC)     {.name = (__NAME), .type = EVM_CLASS_VTT_METHOD, .v.fn = (__FUNC)}
#define EVM_CLASS_VT_INTEGER(__NAME, __INT)     {.name = (__NAME), .type = EVM_CLASS_VTT_INTEGER, .v.num_int = (__INT)}
#define EVM_CLASS_VT_INSTANCE(__NAME, __CLASS)  {.name = (__NAME), .type = EVM_CLASS_VTT_INSTANCE, .v.c = (__CLASS)}
#define EVM_CLASS_VT_STRUCT(__NAME)             {.name = (__NAME), .type = EVM_CLASS_VTT_STRUCT}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __evm_struct_foreach(__macro, __e, __pobj, __ptr, __x, ...)             \
    VSF_MCAT2(__evm_struct_foreach_, VSF_IS_EMPTY(__VA_ARGS__))                 \
        (__macro, (__e), (__pobj), (__ptr), __x, __VA_ARGS__)
#define __evm_struct_foreach_0(__macro, __e, __pobj, __ptr, __x, ...)           \
    __macro((__e), (__pobj), (__ptr), __x)                                      \
    __VSF_MOBSTRUCT(__evm_struct_foreach_i)()(__macro, (__e), (__pobj), (__ptr), __VA_ARGS__)
#define __evm_struct_foreach_1(__macro, __e, __pobj, __ptr, __x, ...)           \
    __macro((__e), (__pobj), (__ptr), __x)
#define __evm_struct_foreach_i()                __evm_struct_foreach
#define evm_struct_foreach(__macro, __e, __pobj, __ptr, ...)                    \
    __VSF_MEXPAND(__evm_struct_foreach(__macro, (__e), (__pobj), (__ptr), __VA_ARGS__))

#define __evm_struct_set_value(__e, __pobj, __ptr, __member)                    \
    evm_prop_set_value((__e), (__pobj), STR(__member), evm_mk_number((__ptr)->__member));
#define evm_struct_create(__e, __ptr, __more, ...)                              \
    ({                                                                          \
        evm_val_t * __pobj = evm_object_create((__e), GC_OBJECT, __PLOOC_VA_NUM_ARGS(__VA_ARGS__) + (__more), 0);\
        if (__pobj != NULL) {                                                   \
            evm_struct_foreach(__evm_struct_set_value, (__e), (__pobj), (__ptr), __VA_ARGS__)\
        }                                                                       \
        __pobj;                                                                 \
    })

/*============================ TYPES =========================================*/

typedef struct evm_class_t evm_class_t;

typedef enum evm_class_vt_type_t {
    EVM_CLASS_VTT_METHOD,
    EVM_CLASS_VTT_INTEGER,
    EVM_CLASS_VTT_INSTANCE,
    EVM_CLASS_VTT_STRUCT,
} evm_class_vt_type_t;

typedef struct evm_class_vt_t {
    const char *name;
    evm_class_vt_type_t type;
    union {
        int num_int;
        evm_val_t (*fn)(evm_t *, evm_val_t *, int, evm_val_t *);
        evm_class_t *c;
    } v;
} evm_class_vt_t;

struct evm_class_t {
    char *name;
    const evm_class_t *parent;
    const evm_class_vt_t *vt;
    uint32_t vt_len;
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern evm_val_t * evm_instance_create(evm_t *e, const evm_class_t *c);

#ifdef __cplusplus
}
#endif

#endif      // __EVM_OO_H__
#endif      // VSF_USE_EVM
