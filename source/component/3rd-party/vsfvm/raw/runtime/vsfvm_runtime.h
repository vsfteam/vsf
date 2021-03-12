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

#ifndef __VSFVM_RUNTIME_H__
#define __VSFVM_RUNTIME_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_vm_cfg.h"

#if VSFVM_CFG_RUNTIME_EN == ENABLED

#include "kernel/vsf_kernel.h"
#include "../common/vsfvm_common.h"

#if     defined(__VSFVM_RUNTIME_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSFVM_RUNTIME_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vsfvm_runtime_t)
dcl_simple_class(vsfvm_runtime_script_t)
dcl_simple_class(vsfvm_thread_t)
dcl_simple_class(vsfvm_runtime_callback_t)

typedef enum vsfvm_var_type_t {
    VSFVM_VAR_TYPE_VALUE = 0,
    VSFVM_VAR_TYPE_RESOURCES,
    VSFVM_VAR_TYPE_REFERENCE,
    VSFVM_VAR_TYPE_FUNCTION,
    VSFVM_VAR_TYPE_INSTANCE,
    VSFVM_VAR_TYPE_USER,
} vsfvm_var_type_t;

typedef struct vsfvm_var_t {
    union {
        intptr_t value;
        int32_t ival;
        uint32_t uval;
        int32_t ival32;
        uint32_t uval32;
        int16_t ival16;
        uint16_t uval16;
        int8_t ival8;
        uint8_t uval8;
        float f;
        void *ptr;
        vsfvm_instance_t *inst;
    };
    vsfvm_var_type_t type;
} vsfvm_var_t;

typedef enum vsfvm_ret_t {
    VSFVM_RET_INVALID_PARAM = -4,
    VSFVM_RET_STACK_FAIL = -3,
    VSFVM_RET_DIV0 = -2,
    VSFVM_RET_ERROR = -1,
    VSFVM_RET_FINISHED = 0,
    VSFVM_RET_PEND,
    VSFVM_RET_GOON,
} vsfvm_ret_t;

#ifdef VSFVM_CFG_RUNTIME_STACK_SIZE
typedef struct vsfvm_stack_t {
    vsfvm_var_t var[VSFVM_CFG_RUNTIME_STACK_SIZE];
    uint32_t sp;
    uint32_t size;
} vsfvm_stack_t;
#else
#define vsfvm_stack_t        vsf_dynstack_t
#endif

typedef struct vsfvm_runtime_func_ctx_t {
    uint8_t argc;

    VSFVM_CODE_FUNCTION_ID_t type;
    union {
        vsfvm_extfunc_handler_t handler;
        uint32_t pc;
    };

    uint32_t arg_reg;
    uint32_t auto_reg;
    uint32_t expression_sp;
} vsfvm_runtime_func_ctx_t;

def_simple_class(vsfvm_thread_t) {
    which (
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        implement(vsf_teda_t)
#else
        implement(vsf_eda_t)
#endif
    )

    protected_member(
        vsfvm_runtime_script_t *script;
        vsfvm_runtime_func_ctx_t func;
        vsf_slist_node_t thread_node;
    )

    private_member(
        vsfvm_runtime_t *runtime;

        vsfvm_stack_t stack;
        uint32_t max_sp;
        vsf_slist_node_t ready_node;
    )
};

typedef enum vsfvm_runtime_state_t {
    VSFVM_SCRIPTSTAT_UNKNOWN,
    VSFVM_SCRIPTSTAT_RUNNING,
    VSFVM_SCRIPTSTAT_ERROR,
    VSFVM_SCRIPTSTAT_FINING,
    VSFVM_SCRIPTSTAT_FINIED,
} vsfvm_runtime_state_t;

def_simple_class(vsfvm_runtime_script_t) {

    public_member(
        const void *token;
        struct {
            uint_fast8_t argc;
            vsfvm_var_t *argv;
        } param;
    )

    protected_member(
        vsf_slist_t thread_list;
    )

    private_member(
        uint32_t lvar_pos;
        vsfvm_runtime_state_t state;
        vsf_slist_node_t script_node;
        vsfvm_thread_t *root_thread;
    )
};

def_simple_class(vsfvm_runtime_t) {
    private_member(
        vsf_slist_t script_list;
        vsf_slist_t to_ready_list;
        vsf_slist_t ready_list;
    )
};

def_simple_class(vsfvm_runtime_callback_t) {
    private_member(
        vsfvm_runtime_t *runtime;
        vsfvm_runtime_script_t *script;
        uint16_t func_pos;
        uint8_t argu_num;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern int vsfvm_runtime_init(vsfvm_runtime_t *runtime);
extern int vsfvm_runtime_fini(vsfvm_runtime_t *runtime);
extern int vsfvm_runtime_poll(vsfvm_runtime_t *runtime);
extern int vsfvm_runtime_gc(vsfvm_runtime_t *runtime);
extern bool vsfvm_runtime_is_thread_pending(vsfvm_runtime_t *runtime);
extern void vsfvm_thread_ready(vsfvm_thread_t *thread);

extern int vsfvm_runtime_script_init(vsfvm_runtime_t *runtime, vsfvm_runtime_script_t *script);
extern int vsfvm_runtime_script_fini(vsfvm_runtime_t *runtime, vsfvm_runtime_script_t *script);

#if defined(__VSFVM_RUNTIME_CLASS_INHERIT__) || defined(__VSFVM_RUNTIME_CLASS_IMPLEMENT)
extern vsfvm_var_t * vsfvm_get_func_argu(vsfvm_thread_t *thread, uint_fast8_t idx);
extern vsfvm_var_t * vsfvm_get_func_argu_ref(vsfvm_thread_t *thread, uint_fast8_t idx);
extern vsfvm_var_t * vsfvm_thread_stack_get(vsfvm_thread_t *thread, uint_fast32_t offset);
extern int vsfvm_thread_stack_push(vsfvm_thread_t *thread, intptr_t value, vsfvm_var_type_t type, uint_fast32_t num);
extern vsfvm_var_t * vsfvm_thread_stack_pop(vsfvm_thread_t *thread, uint_fast32_t num);

extern vsfvm_var_t * vsfvm_get_ref(vsfvm_thread_t *thread, vsfvm_var_t *var);
extern int_fast32_t vsfvm_get_res(vsfvm_runtime_script_t *script, uint_fast32_t offset, uint8_t **buffer);

extern vsfvm_instance_t * vsfvm_instance_alloc(uint_fast32_t size, const vsfvm_class_t *c);
extern void vsfvm_instance_free(vsfvm_instance_t *inst);
extern bool vsfvm_instance_of(vsfvm_instance_t *inst, const vsfvm_class_t *c);

extern bool vsfvm_var_instance_of(vsfvm_var_t *var, const vsfvm_class_t *c);
extern vsfvm_ret_t vsfvm_var_alloc_instance(vsfvm_thread_t *thread, vsfvm_var_t *var, uint_fast32_t size, const vsfvm_class_t *c);
extern vsfvm_ret_t vsfvm_var_free_instance(vsfvm_thread_t *thread, vsfvm_var_t *var);
extern void vsfvm_var_set(vsfvm_thread_t *thread, vsfvm_var_t *var, vsfvm_var_type_t type, intptr_t value);

extern vsfvm_thread_t * vsfvm_thread_init(vsfvm_runtime_t *runtime,
    vsfvm_runtime_script_t *script, uint_fast16_t start_pos, uint_fast8_t argc,
    vsfvm_thread_t *orig_thread, vsfvm_var_t *argv);
extern vsfvm_ret_t vsfvm_thread_run(vsfvm_runtime_t *runtime, vsfvm_thread_t *thread);

extern bool vsfvm_runtime_register_callback(vsfvm_runtime_callback_t *callback, vsfvm_thread_t *thread, vsfvm_var_t *func);
extern vsfvm_thread_t * vsfvm_runtime_call_callback(vsfvm_runtime_callback_t *callback, uint_fast8_t argc, vsfvm_var_t *argv);
#endif

#undef __VSFVM_RUNTIME_CLASS_IMPLEMENT
#undef __VSFVM_RUNTIME_CLASS_INHERIT__

#endif      // VSFVM_CFG_RUNTIME_EN
#endif      // __VSFVM_RUNTIME_H__
