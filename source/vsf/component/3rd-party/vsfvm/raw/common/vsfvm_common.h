/*****************************************************************************
 *   Copyright(C)2009-2020 by SimonQian                                      *
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

#ifndef __VSFVM_COMMON_H__
#define __VSFVM_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_vm_cfg.h"

#include "./vsfvm_bytecode.h"

#include "utilities/vsf_utilities.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSFVM_CFG_RUNTIME_EN == ENABLED
#   define VSFVM_EXTFUNC(__HANDLER, __ARGU_NUM)                                 \
        {                                                                       \
            .handler = (__HANDLER),                                             \
            .argu_num = (__ARGU_NUM),                                           \
        }
#   define VSFVM_EXTVAR(__VALUE, __TYPE)                                        \
        {                                                                       \
            .value = (__VALUE),                                                 \
            .type = (__TYPE),                                                   \
        }

#   define VSFVM_EXTINST(__INST, __TYPE)                                        \
        {                                                                       \
            .inst = (__INST),                                                   \
            .type = (__TYPE),                                                   \
        }
#endif

#if VSFVM_CFG_COMPILER_EN == ENABLED

#   define VSFVM_LEXERSYM_EXTFUNC(__NAME, __EXTOP, __CLASS, __RETCLASS, __PARAM_NUM, __POS)\
        {                                                                       \
            .name = (__NAME),                                                   \
            .ext_op = (__EXTOP),                                                \
            .func.param_num = (__PARAM_NUM),                                    \
            .func.retc = (__RETCLASS),                                          \
            .func.pos = (__POS),                                                \
            .c = (__CLASS),                                                     \
            .type = VSFVM_LEXER_SYM_EXTFUNC,                                    \
        }
#   define VSFVM_LEXERSYM_EXTVAR(__NAME, __EXTOP, __CLASS, __POS)               \
        {                                                                       \
            .name = (__NAME),                                                   \
            .ext_op = (__EXTOP),                                                \
            .c = (__CLASS),                                                     \
            .var.pos = (__POS),                                                 \
            .type = VSFVM_LEXER_SYM_EXTVAR,                                     \
        }
#   define VSFVM_LEXERSYM_CLASS(__NAME, __EXTOP, __CLASS)                       \
        {                                                                       \
            .name = (__NAME),                                                   \
            .ext_op = (__EXTOP),                                                \
            .c = (__CLASS),                                                     \
            .type = VSFVM_LEXER_SYM_EXTCLASS,                                   \
        }
#   define VSFVM_LEXERSYM_CONST(__NAME, __EXTOP, __CLASS, __VALUE)              \
        {                                                                       \
            .name = (__NAME),                                                   \
            .ext_op = (__EXTOP),                                                \
            .ival = (__VALUE),                                                  \
            .c = (__CLASS),                                                     \
            .type = VSFVM_LEXER_SYM_CONST,                                      \
        }
#   define VSFVM_LEXERSYM_KEYWORKD(__NAME)                                      \
        {                                                                       \
            .name = (__NAME),                                                   \
            .type = VSFVM_LEXER_SYM_KEYWORD,                                    \
        }
#   define VSFVM_LEXERSYM_FUNCTION(__NAME, __PARAM_NUM)                         \
        {                                                                       \
            .name = (__NAME),                                                   \
            .func.param_num = (__PARAM_NUM),                                    \
            .type = VSFVM_LEXER_SYM_FUNCTION,                                   \
        }
#   define VSFVM_LEXERSYM_ALIAS(__NAME, __SYM)                                  \
        {                                                                       \
            .name = (__NAME),                                                   \
            .sym = (vsfvm_lexer_sym_t *)(__SYM),                                \
            .type = VSFVM_LEXER_SYM_ALIAS,                                      \
        }

#   define __vsfvm_pt_begin(__pt)                                               \
            enum {                                                              \
                COUNTER_OFFSET = __COUNTER__ + 1,                               \
            };                                                                  \
            switch ((__pt)->state) {                                            \
                case __COUNTER__ - COUNTER_OFFSET:

#   define __vsfvm_pt_entry(__pt)                                               \
            (__pt)->state = (__COUNTER__ - COUNTER_OFFSET + 1) >> 1;            \
            case (__COUNTER__ - COUNTER_OFFSET) >> 1:

#   define __vsfvm_pt_wait(__pt)			                                    \
            do {                                                                \
                evt = VSFVM_EVT_INVALID;                                        \
                __vsfvm_pt_entry(__pt);                                         \
                if (VSFVM_EVT_INVALID == evt) {                                 \
                    return VSF_ERR_NOT_READY;                                   \
                }                                                               \
            } while (0)

#   define __vsfvm_pt_end(__pt)         }

#   define vsfvm_pt_begin(__pt)         __vsfvm_pt_begin(__pt)
#   define vsfvm_pt_entry(__pt)         __vsfvm_pt_entry(__pt)
#   define vsfvm_pt_end(__pt)           __vsfvm_pt_end(__pt)
#   define vsfvm_pt_wait(__pt)          __vsfvm_pt_wait(__pt)

#endif

/*============================ TYPES =========================================*/

#if VSFVM_CFG_COMPILER_EN == ENABLED
typedef struct vsfvm_pt_t {
    int state;
} vsfvm_pt_t;

typedef enum vsfvm_pt_evt_t {
    VSFVM_EVT_INVALID,
    VSFVM_EVT_ON_TOKEN,
    VSFVM_EVT_ON_EXPR,
    VSFVM_EVT_ON_STMT,
} vsfvm_pt_evt_t;
#endif

typedef struct vsfvm_class_t vsfvm_class_t;
typedef struct vsfvm_instance_t vsfvm_instance_t;
typedef struct vsfvm_ext_op_t vsfvm_ext_op_t;

struct vsfvm_instance_t {
    implement_ex(vsf_mem_t, mem)
    uint32_t ref;
    const vsfvm_class_t *c;
};

#if VSFVM_CFG_RUNTIME_EN == ENABLED
typedef struct vsfvm_runtime_t vsfvm_runtime_t;
typedef struct vsfvm_thread_t vsfvm_thread_t;
typedef struct vsfvm_runtime_script_t vsfvm_runtime_script_t;
typedef enum vsfvm_ret_t vsfvm_ret_t;
typedef struct vsfvm_var_t vsfvm_var_t;
#endif

#if VSFVM_CFG_COMPILER_EN == ENABLED
#   include "../compiler/lexer/vsfvm_lexer.h"
#endif

typedef enum vsfvm_class_type_t {
    VSFVM_CLASS_USER,
    VSFVM_CLASS_STRING,
    VSFVM_CLASS_BUFFER,
    VSFVM_CLASS_ARRAY,
    VSFVM_CLASS_POINTER,
} vsfvm_class_type_t;

typedef struct vsfvm_class_op_t {
    void (*print)(vsfvm_instance_t *inst);
    void (*destroy)(vsfvm_instance_t *inst);
} vsfvm_class_op_t;

typedef struct vsfvm_class_t {
#if VSFVM_CFG_COMPILER_EN == ENABLED
    char *name;
#endif
#if VSFVM_CFG_RUNTIME_EN == ENABLED
    vsfvm_class_type_t type;
    vsfvm_class_op_t op;
#endif
} vsfvm_class_t;

#if VSFVM_CFG_RUNTIME_EN == ENABLED
typedef vsfvm_ret_t (*vsfvm_extfunc_handler_t)(vsfvm_thread_t *thread);
typedef struct vsfvm_extfunc_t {
    vsfvm_extfunc_handler_t handler;
    // if argu_num is -1, use dynamic paramter
    int16_t argu_num;
} vsfvm_extfunc_t;
#endif

struct vsfvm_ext_op_t {
#if VSFVM_CFG_COMPILER_EN == ENABLED
    char *name;
#endif
#if VSFVM_CFG_RUNTIME_EN == ENABLED
    int (*init)(vsfvm_runtime_t *runtime, vsfvm_runtime_script_t *script);
    int (*fini)(vsfvm_runtime_t *runtime, vsfvm_runtime_script_t *script);
    const vsfvm_extfunc_t *func;
    vsfvm_var_t *var;
#endif
#if VSFVM_CFG_COMPILER_EN == ENABLED
    const vsfvm_lexer_sym_t *sym;
    uint32_t sym_num;
#endif
    uint16_t func_num;
    uint16_t var_num;
};

typedef struct vsfvm_ext_t {
    const vsfvm_ext_op_t *op;
#if VSFVM_CFG_COMPILER_EN == ENABLED
    vsfvm_lexer_symarr_t symarr;
    uint16_t func_id;
    uint16_t var_id;
#endif
    vsf_slist_node_t ext_node;
} vsfvm_ext_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_slist_t vsfvm_ext_list;
int vsfvm_register_ext(vsfvm_ext_t *ext);

#endif        // __VSFVM_COMMON_H_INCLUDED__
