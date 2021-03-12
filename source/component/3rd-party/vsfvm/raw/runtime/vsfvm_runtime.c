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

/*============================ INCLUDES ======================================*/

#include "../vsf_vm_cfg.h"

#if VSFVM_CFG_RUNTIME_EN == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSFVM_RUNTIME_CLASS_IMPLEMENT
#include "./vsfvm_runtime.h"
#include "../common/vsfvm_objdump.h"

#include "kernel/vsf_kernel.h"
#include "service/vsf_service.h"

/*============================ MACROS ========================================*/

#ifndef VSFVM_CFG_DYNARR_ITEM_BITLEN
#   define VSFVM_CFG_DYNARR_ITEM_BITLEN     4
#endif
#ifndef VSFVM_CFG_DYNARR_TABLE_BITLEN
#   define VSFVM_CFG_DYNARR_TABLE_BITLEN    4
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsfvm_thread_t * vsfvm_alloc_thread_imp(vsfvm_runtime_t *runtime);
extern void vsfvm_free_thread_imp(vsfvm_runtime_t *runtime, vsfvm_thread_t *thread);
extern vsfvm_bytecode_t vsfvm_get_bytecode_imp(const void *token, uint_fast32_t *pc);
extern int_fast32_t vsfvm_get_res_imp(const void *token, uint_fast32_t offset, uint8_t **buffer);

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSFVM_ALLOC_THREAD_IMP
WEAK(vsfvm_alloc_thread_imp)
vsfvm_thread_t * vsfvm_alloc_thread_imp(vsfvm_runtime_t *runtime)
{
    return (vsfvm_thread_t *)vsf_heap_malloc(sizeof(vsfvm_thread_t));
}
#endif

#ifndef WEAK_VSFVM_FREE_THREAD_IMP
WEAK(vsfvm_free_thread_imp)
void vsfvm_free_thread_imp(vsfvm_runtime_t *runtime, vsfvm_thread_t *thread)
{
    vsf_heap_free(thread);
}
#endif

#ifndef WEAK_VSFVM_GET_BYTECODE_IMP

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

WEAK(vsfvm_get_bytecode_imp)
vsfvm_bytecode_t vsfvm_get_bytecode_imp(const void *token, uint_fast32_t *pc)
{
    VSFVM_ASSERT(false);
    return VSFVM_CODE(VSFVM_CODE_TYPE_EOF, 0);
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

#endif

#ifndef WEAK_VSFVM_GET_RES_IMP

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

WEAK(vsfvm_get_res_imp)
int_fast32_t vsfvm_get_res_imp(const void *token, uint_fast32_t offset, uint8_t **buffer)
{
    VSFVM_ASSERT(false);
    return -1;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

#endif

int_fast32_t vsfvm_get_res(vsfvm_runtime_script_t *script, uint_fast32_t offset, uint8_t **buffer)
{
    return vsfvm_get_res_imp(script->token, offset, buffer);
}

static const vsfvm_extfunc_t * __vsfvm_get_extfunc(uint_fast16_t id)
{
    __vsf_slist_foreach_unsafe(vsfvm_ext_t, ext_node, &vsfvm_ext_list) {
        if (id < _->op->func_num) {
            return &_->op->func[id];
        }
        id -= _->op->func_num;
    }
    return NULL;
}

static vsfvm_var_t * __vsfvm_get_extvar(uint_fast16_t id)
{
    __vsf_slist_foreach_unsafe(vsfvm_ext_t, ext_node, &vsfvm_ext_list) {
        if (id < _->op->var_num) {
            return &_->op->var[id];
        }
        id -= _->op->var_num;
    }
    return NULL;
}

static vsfvm_var_t * __vsfvm_stack_get(vsfvm_stack_t *stack, uint_fast32_t offset)
{
#ifdef VSFVM_CFG_RUNTIME_STACK_SIZE
    uint_fast16_t id;
    if (offset >= stack->sp) {
        return NULL;
    }
    id = stack->sp - offset - 1;
    return &stack->var[id];
#else
    return vsf_dynstack_get(stack, offset);
#endif
}

static vsfvm_var_t * __vsfvm_stack_get_byidx(vsfvm_stack_t *stack, uint_fast32_t idx)
{
#ifdef VSFVM_CFG_RUNTIME_STACK_SIZE
    if (idx >= stack->sp) {
        return NULL;
    }
    return &stack->var[idx];
#else
    return vsf_dynarr_get(&stack->use_as__vsf_dynarr_t, idx);
#endif
}

static vsf_err_t __vsfvm_stack_push(vsfvm_stack_t *stack, vsfvm_var_t *var, uint_fast32_t num)
{
#ifdef VSFVM_CFG_RUNTIME_STACK_SIZE
    if (stack->size < (stack->sp + num)) {
        return VSF_ERR_FAIL;
    }
    for (uint_fast32_t i = 0; i < num; i++) {
        stack->var[stack->sp++] = *var;
    }
    return VSF_ERR_NONE;
#else
    return vsf_dynstack_push(stack, var, num);
#endif
}

static vsfvm_var_t * __vsfvm_stack_pop(vsfvm_stack_t *stack, uint_fast32_t num)
{
#ifdef VSFVM_CFG_RUNTIME_STACK_SIZE
    if ((stack->sp >= num) && (num > 0)) {
        stack->sp -= num;
        return &stack->var[stack->sp];
    }
    return NULL;
#else
    return vsf_dynstack_pop(stack, num);
#endif
}

static vsf_err_t __vsfvm_stack_push_ext(vsfvm_stack_t *stack, void *ptr, uint_fast32_t len)
{
#ifdef VSFVM_CFG_RUNTIME_STACK_SIZE
    uint_fast32_t item_size = sizeof(stack->var[0]);
    uint_fast32_t num = (len + item_size - 1) / item_size;
    if (stack->size < (stack->sp + num)) {
        return VSF_ERR_FAIL;
    }

    memcpy(&stack->var[stack->sp], ptr, len);
    stack->sp += num;
    return VSF_ERR_NONE;
#else
    return vsf_dynstack_push_ext(stack, ptr, len);
#endif
}

static vsf_err_t __vsfvm_stack_pop_ext(vsfvm_stack_t *stack, void *ptr, uint_fast32_t len)
{
#ifdef VSFVM_CFG_RUNTIME_STACK_SIZE
    uint_fast32_t item_size = sizeof(stack->var[0]);
    uint_fast32_t num = (len + item_size - 1) / item_size;
    if (stack->sp < num) {
        return VSF_ERR_FAIL;
    }

    stack->sp -= num;
    memcpy(ptr, &stack->var[stack->sp], len);
    return VSF_ERR_NONE;
#else
    return vsf_dynstack_pop_ext(stack, ptr, len);
#endif
}

static void __vsfvm_stack_fini(vsfvm_stack_t *stack)
{
#ifndef VSFVM_CFG_RUNTIME_STACK_SIZE
    vsf_dynstack_fini(stack);
#endif
}

static vsf_err_t __vsfvm_stack_init(vsfvm_stack_t *stack)
{
#ifdef VSFVM_CFG_RUNTIME_STACK_SIZE
    stack->sp = 0;
    stack->size = VSFVM_CFG_RUNTIME_STACK_SIZE;
    return VSF_ERR_NONE;
#else
    stack->item_size = sizeof(vsfvm_var_t);
    stack->item_num_per_buf_bitlen = VSFVM_CFG_DYNARR_ITEM_BITLEN;
    stack->buf_num_per_table_bitlen = VSFVM_CFG_DYNARR_TABLE_BITLEN;
    return vsf_dynstack_init(stack);
#endif
}

static vsfvm_var_t * __vsfvm_runtime_get_var(vsfvm_thread_t *thread, VSFVM_CODE_VARIABLE_POS_t pos, uint_fast16_t id)
{
    vsfvm_runtime_script_t *script = thread->script;

    switch (pos) {
    case VSFVM_CODE_VARIABLE_POS_LOCAL:
        thread = script->root_thread;
        id += script->lvar_pos;
        break;
    case VSFVM_CODE_VARIABLE_POS_STACK_BEGIN:
        break;
    case VSFVM_CODE_VARIABLE_POS_STACK_END:
        return __vsfvm_stack_get(&thread->stack, id);
    case VSFVM_CODE_VARIABLE_POS_FUNCARG:
        id = thread->func.arg_reg + id;
        break;
    case VSFVM_CODE_VARIABLE_POS_FUNCAUTO:
        id = thread->func.auto_reg + id;
        break;
    case VSFVM_CODE_VARIABLE_POS_EXT:
        return __vsfvm_get_extvar(id);
        break;
    default: return NULL;
    }
    return __vsfvm_stack_get_byidx(&thread->stack, id);
}

vsfvm_var_t * vsfvm_get_func_argu(vsfvm_thread_t *thread, uint_fast8_t idx)
{
    if (thread->func.argc <= idx) { return NULL; }
    return __vsfvm_runtime_get_var(thread, VSFVM_CODE_VARIABLE_POS_FUNCARG, idx);
}

vsfvm_var_t * vsfvm_get_func_argu_ref(vsfvm_thread_t *thread, uint_fast8_t idx)
{
    vsfvm_var_t *var = vsfvm_get_func_argu(thread, idx);
    if ((var != NULL) && (var->type == VSFVM_VAR_TYPE_REFERENCE)) {
        var = vsfvm_get_ref(thread, var);
    }
    return var;
}

// instance
bool vsfvm_instance_of(vsfvm_instance_t *inst, const vsfvm_class_t *c)
{
    return (inst != NULL) && (inst->c == c);
}

vsfvm_instance_t * vsfvm_instance_alloc(uint_fast32_t size, const vsfvm_class_t *c)
{
    vsfvm_instance_t *inst = vsf_heap_malloc(sizeof(vsfvm_instance_t) + size);
    if (inst) {
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
        vsf_trace_debug("alloc instance 0x%08X" VSF_TRACE_CFG_LINEEND, inst);
#endif
        memset(inst, 0, sizeof(vsfvm_instance_t) + size);

        inst->buffer = (uint8_t *)&inst[1];
        inst->size = size;
        inst->ref = 1;
        inst->c = c;
    }
    return inst;
}

void vsfvm_instance_free(vsfvm_instance_t *inst)
{
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
    vsf_trace_debug("free instance 0x%08X" VSF_TRACE_CFG_LINEEND, inst);
#endif
    if ((inst->c->op.destroy != NULL)) {
        inst->c->op.destroy(inst);
    }
    vsf_heap_free(inst);
}

void vsfvm_instance_ref(vsfvm_instance_t *inst)
{
    inst->ref++;
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
    vsf_trace_debug("var 0x%08X reference %d" VSF_TRACE_CFG_LINEEND, inst, inst->ref);
#endif
}

bool vsfvm_instance_deref(vsfvm_instance_t *inst)
{
    inst->ref--;
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
    vsf_trace_debug("var 0x%08X reference %d" VSF_TRACE_CFG_LINEEND, inst, inst->ref);
#endif
    if (!inst->ref) {
        vsfvm_instance_free(inst);
        return true;
    }
    return false;
}

bool vsfvm_var_instance_of(vsfvm_var_t *var, const vsfvm_class_t *c)
{
    return (var->type == VSFVM_VAR_TYPE_INSTANCE) && vsfvm_instance_of(var->inst, c);
}

static void __vsfvm_var_ref_instance(vsfvm_thread_t *thread, vsfvm_var_t *var)
{
    if (var->type == VSFVM_VAR_TYPE_REFERENCE) {
        var = vsfvm_get_ref(thread, var);
    }

    if ((var->type == VSFVM_VAR_TYPE_INSTANCE) && (var->inst != NULL)) {
        vsfvm_instance_ref(var->inst);
    }
}

static void __vsfvm_var_deref_instance(vsfvm_thread_t *thread, vsfvm_var_t *var)
{
    if (var->type == VSFVM_VAR_TYPE_REFERENCE) {
        var = vsfvm_get_ref(thread, var);
    }

    if ((var->type == VSFVM_VAR_TYPE_INSTANCE) && (var->inst != NULL)) {
        if (vsfvm_instance_deref(var->inst)) {
            var->inst = NULL;
        }
    }
}

vsfvm_ret_t vsfvm_var_alloc_instance(vsfvm_thread_t *thread, vsfvm_var_t *var, uint_fast32_t size, const vsfvm_class_t *c)
{
    __vsfvm_var_deref_instance(thread, var);
    var->type = VSFVM_VAR_TYPE_INSTANCE;
    var->inst = vsfvm_instance_alloc(size, c);
    return var->inst ? VSFVM_RET_FINISHED : VSFVM_RET_ERROR;
}

vsfvm_ret_t vsfvm_var_free_instance(vsfvm_thread_t *thread, vsfvm_var_t *var)
{
    if (var->type != VSFVM_VAR_TYPE_INSTANCE) {
        return VSFVM_RET_ERROR;
    }

    if (var->inst != NULL) {
        vsfvm_instance_free(var->inst);
        var->inst = NULL;
    }
    return VSFVM_RET_FINISHED;
}

void vsfvm_var_set(vsfvm_thread_t *thread, vsfvm_var_t *var, vsfvm_var_type_t type, intptr_t value)
{
    __vsfvm_var_deref_instance(thread, var);
    var->type = type;
    var->value = value;
    __vsfvm_var_ref_instance(thread, var);
}


vsfvm_var_t * vsfvm_thread_stack_pop(vsfvm_thread_t *thread, uint_fast32_t num)
{
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
    vsfvm_var_t *var = __vsfvm_stack_pop(&thread->stack, num);
    if (num) {
        vsf_trace_debug("pop stack = %d" VSF_TRACE_CFG_LINEEND, thread->stack.sp);
    }
    return var;
#else
    return __vsfvm_stack_pop(&thread->stack, num);
#endif
}

static int __vsfvm_thread_stack_pop_and_free(vsfvm_thread_t *thread, uint_fast32_t num)
{
    vsfvm_var_t *var;

    while (num-- > 0) {
        var = vsfvm_thread_stack_pop(thread, 1);
        if (!var) { return -1; }
        __vsfvm_var_deref_instance(thread, var);
    }
    return 0;
}

vsfvm_var_t * vsfvm_thread_stack_get(vsfvm_thread_t *thread, uint_fast32_t offset)
{
    return __vsfvm_stack_get(&thread->stack, offset);
}

int vsfvm_thread_stack_push(vsfvm_thread_t *thread, intptr_t value,
    vsfvm_var_type_t type, uint_fast32_t num)
{
    vsfvm_var_t var = { value, type };
    int err = __vsfvm_stack_push(&thread->stack, &var, num);
    if (!err) {
        if (thread->max_sp < thread->stack.sp) {
            thread->max_sp = thread->stack.sp;
        }
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
        vsf_trace_debug("push stack = %d" VSF_TRACE_CFG_LINEEND, thread->stack.sp);
#endif
    }
    return err;
}

static int __vsfvm_thread_stack_push_ref(vsfvm_thread_t *thread,
    intptr_t value, vsfvm_var_type_t type, uint_fast32_t num)
{
    VSFVM_CODE_VARIABLE_POS_t pos = (VSFVM_CODE_VARIABLE_POS_t)((value >> 16) & 0xFF);
    uint_fast16_t id = value & 0xFFFF;

    switch (pos) {
    case VSFVM_CODE_VARIABLE_POS_STACK_END:
        id = thread->stack.sp - id - 1;
        value = ((intptr_t)VSFVM_CODE_VARIABLE_POS_STACK_BEGIN << 16) | id;
        break;
    case VSFVM_CODE_VARIABLE_POS_FUNCARG:
        id = thread->func.arg_reg + id;
        value = ((intptr_t)VSFVM_CODE_VARIABLE_POS_STACK_BEGIN << 16) | id;
        break;
    case VSFVM_CODE_VARIABLE_POS_FUNCAUTO:
        id = thread->func.auto_reg + id;
        value = ((intptr_t)VSFVM_CODE_VARIABLE_POS_STACK_BEGIN << 16) | id;
        break;
    }
    return vsfvm_thread_stack_push(thread, value, type, num);
}

vsfvm_var_t * vsfvm_get_ref(vsfvm_thread_t *thread, vsfvm_var_t *var)
{
    VSFVM_CODE_VARIABLE_POS_t pos = (VSFVM_CODE_VARIABLE_POS_t)((var->value >> 16) & 0xFF);
    uint_fast16_t id = var->value & 0xFFFF;
    return __vsfvm_runtime_get_var(thread, pos, id);
}

static int __vsfvm_push_func(vsfvm_thread_t *thread)
{
    int err = __vsfvm_stack_push_ext(&thread->stack, &thread->func, sizeof(thread->func));
    if (!err) {
        if (thread->max_sp < thread->stack.sp) {
            thread->max_sp = thread->stack.sp;
        }
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
        vsf_trace_debug("push func ctx = %d" VSF_TRACE_CFG_LINEEND, thread->stack.sp);
#endif
    }
    return err;
}

static int __vsfvm_pop_func(vsfvm_thread_t *thread)
{
    vsfvm_runtime_func_ctx_t func;

    if (__vsfvm_stack_pop_ext(&thread->stack, &func, sizeof(func)) < 0) {
        return -1;
    }
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
    vsf_trace_debug("pop func ctx = %d" VSF_TRACE_CFG_LINEEND, thread->stack.sp);
#endif
    thread->func = func;
    return 0;
}

static void __vsfvm_thread_fini_imp(vsfvm_runtime_t *runtime, vsfvm_thread_t *thread)
{
    vsfvm_runtime_script_t *script = thread->script;
    vsfvm_var_t *var;

    do {
        var = vsfvm_thread_stack_pop(thread, 1);
        if (var != NULL) { __vsfvm_var_deref_instance(thread, var); }
    } while (var != NULL);
    __vsfvm_stack_fini(&thread->stack);
    vsf_slist_remove(vsfvm_thread_t, thread_node, &script->thread_list, thread);
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    vsf_eda_fini(&thread->use_as__vsf_teda_t.use_as__vsf_eda_t);
#else
    vsf_eda_fini(&thread->use_as__vsf_eda_t);
#endif
    vsfvm_free_thread_imp(runtime, thread);
}

static void __vsfvm_thread_fini(vsfvm_runtime_t *runtime, vsfvm_thread_t *thread)
{
    if (thread->script->root_thread != thread) {
        __vsfvm_thread_fini_imp(runtime, thread);
    }
}

vsfvm_ret_t vsfvm_thread_run(vsfvm_runtime_t *runtime, vsfvm_thread_t *thread)
{
    vsfvm_runtime_script_t *script = thread->script;
    vsfvm_runtime_func_ctx_t *func = &thread->func;
    vsfvm_var_t *arg1, *arg2, *var, *result;
    vsfvm_ret_t ret;
    vsfvm_bytecode_t token;
    uint_fast16_t arg16;
    uint_fast8_t id, arg8, type;

    while (1) {
        if (func->type == VSFVM_CODE_FUNCTION_EXT) {
            ret = func->handler(thread);
            if ((int)ret < 0) { return ret; }
            switch (ret) {
            case VSFVM_RET_PEND:        return ret;
            case VSFVM_RET_FINISHED:    goto do_return;
            case VSFVM_RET_GOON:        break;
            }
            continue;
        }

        token = vsfvm_get_bytecode_imp(script->token, &thread->func.pc);
        type = VSFVM_CODE_TYPE(token);
        id = VSFVM_CODE_ID(token);
        arg8 = VSFVM_CODE_ARG8(token);
        arg16 = VSFVM_CODE_ARG16(token);
        switch (type) {
        case VSFVM_CODE_TYPE_SYMBOL:
            if (!func->expression_sp) {
                func->expression_sp = thread->stack.sp;
            }

            if (id == VSFVM_CODE_SYMBOL_SEMICOLON) {
                if (thread->stack.sp != (func->expression_sp + 1)) {
                    return VSFVM_RET_ERROR;
                }
                func->expression_sp = 0;
                switch ((enum VSFVM_CODE_SYBMOL_SEMICOLIN_ID_t)arg8) {
                case VSFVM_CODE_SYMBOL_SEMICOLON_POP:
                    __vsfvm_thread_stack_pop_and_free(thread, 1);
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
                    vsf_trace_debug("expr end stack = %d" VSF_TRACE_CFG_LINEEND, thread->stack.sp);
#endif
                case VSFVM_CODE_SYMBOL_SEMICOLON_NOPOP:
                    break;
                default:
                    return VSFVM_RET_ERROR;
                }
            } else {
                if (id > VSFVM_CODE_SYMBOL_ASSIGN) {
                    return VSFVM_RET_ERROR;
                }

                if (id < VSFVM_CODE_SYMBOL_POSI) {
                    arg1 = result = vsfvm_thread_stack_get(thread, 0);
                    if (!arg1) { return VSFVM_RET_ERROR; }
                    if (arg1->type == VSFVM_VAR_TYPE_RESOURCES) {
                        arg1 = vsfvm_get_ref(thread, arg1);
                        if (!arg1) { return VSFVM_RET_ERROR; }
                    } else if (arg1->type == VSFVM_VAR_TYPE_RESOURCES) {
                        return VSFVM_RET_ERROR;
                    }

                    result->type = VSFVM_VAR_TYPE_VALUE;
                    switch (id) {
                    case VSFVM_CODE_SYMBOL_NOT: result->value = !arg1->value; break;
                    case VSFVM_CODE_SYMBOL_REV: result->value = ~arg1->value; break;
                    case VSFVM_CODE_SYMBOL_NEGA: result->value = -arg1->value; break;
                    case VSFVM_CODE_SYMBOL_POSI: result->value = arg1->value; break;
                    default: return VSFVM_RET_ERROR;
                    }
                } else {
                    arg2 = vsfvm_thread_stack_pop(thread, 1);
                    if (!arg2) { return VSFVM_RET_ERROR; }
                    if (arg2->type == VSFVM_VAR_TYPE_REFERENCE) {
                        arg2 = vsfvm_get_ref(thread, arg2);
                        if (!arg2) { return VSFVM_RET_ERROR; }
                    } else if (arg2->type == VSFVM_VAR_TYPE_RESOURCES) {
                        return VSFVM_RET_ERROR;
                    }

                    arg1 = result = vsfvm_thread_stack_get(thread, 0);
                    if (!arg1) { return VSFVM_RET_ERROR; }
                    if (id != VSFVM_CODE_SYMBOL_ASSIGN) {
                        if (arg1->type == VSFVM_VAR_TYPE_REFERENCE) {
                            arg1 = vsfvm_get_ref(thread, arg1);
                            if (!arg1) { return VSFVM_RET_ERROR; }
                        } else if (arg1->type == VSFVM_VAR_TYPE_RESOURCES) {
                            return VSFVM_RET_ERROR;
                        }
                    } else if (arg1->type != VSFVM_VAR_TYPE_REFERENCE) {
                        return VSFVM_RET_ERROR;
                    }

                    __vsfvm_var_deref_instance(thread, result);
                    switch (id) {
                    case VSFVM_CODE_SYMBOL_MUL:     result->value = arg1->value * arg2->value; break;
                    case VSFVM_CODE_SYMBOL_DIV:
                        if (!arg2->value) { return VSFVM_RET_DIV0; }
                        result->value = arg1->value / arg2->value;
                        break;
                    case VSFVM_CODE_SYMBOL_MOD:
                        if (!arg2->value) { return VSFVM_RET_DIV0; }
                        result->value = arg1->value % arg2->value;
                        break;
                    case VSFVM_CODE_SYMBOL_ADD:     result->value = arg1->value + arg2->value; break;
                    case VSFVM_CODE_SYMBOL_SUB:     result->value = arg1->value - arg2->value; break;
                    case VSFVM_CODE_SYMBOL_SHL:     result->value = arg1->value << arg2->value; break;
                    case VSFVM_CODE_SYMBOL_SHR:     result->value = arg1->value >> arg2->value; break;
                    case VSFVM_CODE_SYMBOL_AND:     result->value = arg1->value & arg2->value; break;
                    case VSFVM_CODE_SYMBOL_OR:      result->value = arg1->value | arg2->value; break;
                    case VSFVM_CODE_SYMBOL_XOR:     result->value = arg1->value ^ arg2->value; break;
                    case VSFVM_CODE_SYMBOL_EQ:      result->value = arg1->value == arg2->value; break;
                    case VSFVM_CODE_SYMBOL_NE:      result->value = arg1->value != arg2->value; break;
                    case VSFVM_CODE_SYMBOL_GT:      result->value = arg1->value > arg2->value; break;
                    case VSFVM_CODE_SYMBOL_GE:      result->value = arg1->value >= arg2->value; break;
                    case VSFVM_CODE_SYMBOL_LT:      result->value = arg1->value < arg2->value; break;
                    case VSFVM_CODE_SYMBOL_LE:      result->value = arg1->value <= arg2->value; break;
                    case VSFVM_CODE_SYMBOL_LAND:    result->value = arg1->value && arg2->value; break;
                    case VSFVM_CODE_SYMBOL_LOR:     result->value = arg1->value || arg2->value; break;
                    case VSFVM_CODE_SYMBOL_LXOR:    result->value = !!arg1->value != !!arg2->value; break;
                    case VSFVM_CODE_SYMBOL_COMMA:   result->value = arg2->value; break;
                    case VSFVM_CODE_SYMBOL_ASSIGN:
                        var = vsfvm_get_ref(thread, arg1);
                        if (!var) { return VSFVM_RET_ERROR; }

                        __vsfvm_var_deref_instance(thread, var);
                        result->value = var->value = arg2->value;
                        result->type = var->type = arg2->type;
                        __vsfvm_var_ref_instance(thread, var);
                        __vsfvm_var_ref_instance(thread, result);
                        __vsfvm_var_deref_instance(thread, arg2);
                        continue;
                    default:
                        return VSFVM_RET_ERROR;
                    }
                    result->type = VSFVM_VAR_TYPE_VALUE;
                    __vsfvm_var_deref_instance(thread, arg2);
                }
            }
            break;
        case VSFVM_CODE_TYPE_KEYWORD:
            switch (id) {
            case VSFVM_CODE_KEYWORD_var:
                if (vsfvm_thread_stack_push(thread, 0, (enum vsfvm_var_type_t)arg8, 1)) {
                    return VSFVM_RET_STACK_FAIL;
                }
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
                vsf_trace_debug("push var = %d" VSF_TRACE_CFG_LINEEND, thread->stack.sp);
#endif
                break;
            case VSFVM_CODE_KEYWORD_goto:
                __vsfvm_thread_stack_pop_and_free(thread, arg8);
            do_goto:
                func->pc += (int16_t)arg16;
                break;
            case VSFVM_CODE_KEYWORD_if:
                var = vsfvm_thread_stack_pop(thread, 1);
                if (!var->value) { goto do_goto; }
                break;
            case VSFVM_CODE_KEYWORD_return:
            do_return:
                __vsfvm_thread_stack_pop_and_free(thread, thread->stack.sp - func->auto_reg);
                // reserve one arg as return value
                arg8 = func->argc - 1;
                __vsfvm_pop_func(thread);
                __vsfvm_thread_stack_pop_and_free(thread, arg8);
                if (!func->pc) {
                    // __vsfvm_thread_fini will not free script->root_thread
                    __vsfvm_thread_fini(runtime, thread);
                    return VSFVM_RET_FINISHED;
                }
                break;
            case VSFVM_CODE_KEYWORD_breakpoint:
                func->pc--;
                return VSFVM_RET_PEND;
            default:
                return VSFVM_RET_ERROR;
            }
            break;
        case VSFVM_CODE_TYPE_NUMBER:
            if (!func->expression_sp) {
                func->expression_sp = thread->stack.sp;
            }

            if (vsfvm_thread_stack_push(thread, VSFVM_CODE_VALUE(token), VSFVM_VAR_TYPE_VALUE, 1)) {
                return VSFVM_RET_STACK_FAIL;
            }
            break;
        case VSFVM_CODE_TYPE_VARIABLE:
            if (!func->expression_sp) {
                func->expression_sp = thread->stack.sp;
            }

            switch (id) {
            case VSFVM_CODE_VARIABLE_NORMAL:
                arg1 = __vsfvm_runtime_get_var(thread, (VSFVM_CODE_VARIABLE_POS_t)arg8, arg16);
                if (!arg1) { return VSFVM_RET_ERROR; }
                if (arg1->type == VSFVM_VAR_TYPE_REFERENCE) {
                    arg1 = vsfvm_get_ref(thread, arg1);
                    if (!arg1) { return VSFVM_RET_ERROR; }
                }
                __vsfvm_var_ref_instance(thread, arg1);
                if (vsfvm_thread_stack_push(thread, arg1->value, arg1->type, 1)) {
                    return VSFVM_RET_STACK_FAIL;
                }
                break;
            case VSFVM_CODE_VARIABLE_REFERENCE:
            case VSFVM_CODE_VARIABLE_REFERENCE_NOTRACE:
                arg1 = __vsfvm_runtime_get_var(thread, (VSFVM_CODE_VARIABLE_POS_t)arg8, arg16);
                if (!arg1) { return VSFVM_RET_ERROR; }
                __vsfvm_var_ref_instance(thread, arg1);
                if (    (id != VSFVM_CODE_VARIABLE_REFERENCE_NOTRACE)
                    &&  (arg1->type == VSFVM_VAR_TYPE_REFERENCE)) {
                    if (vsfvm_thread_stack_push(thread, arg1->value,
                            VSFVM_VAR_TYPE_REFERENCE, 1)) {
                        return VSFVM_RET_STACK_FAIL;
                    }
                } else if (__vsfvm_thread_stack_push_ref(thread,
                        VSFVM_CODE_ARG24(token), VSFVM_VAR_TYPE_REFERENCE, 1)) {
                    return VSFVM_RET_STACK_FAIL;
                }
                break;
            case VSFVM_CODE_VARIABLE_RESOURCES:
                arg16 = func->pc + (int16_t)arg16;
                if (vsfvm_thread_stack_push(thread, arg16, VSFVM_VAR_TYPE_RESOURCES, 1)) {
                    return VSFVM_RET_STACK_FAIL;
                }
                break;
            case VSFVM_CODE_VARIABLE_FUNCTION:
                arg16 = func->pc + (int16_t)arg16;
                if (vsfvm_thread_stack_push(thread, ((intptr_t)arg8 << 16) + arg16,
                        VSFVM_VAR_TYPE_FUNCTION, 1)) {
                    return VSFVM_RET_STACK_FAIL;
                }
                break;
            default:
                return VSFVM_RET_ERROR;
            }
            break;
        case VSFVM_CODE_TYPE_FUNCTION:
            if (!func->expression_sp) {
                func->expression_sp = thread->stack.sp;
            }

            {
                const struct vsfvm_extfunc_t *ext;
                uint_fast32_t sp = thread->stack.sp - arg8;

                // at least one arg, for result value
                if (!arg8) {
                    arg8++;
                    if (vsfvm_thread_stack_push(thread, 0, VSFVM_VAR_TYPE_VALUE, 1)) {
                        return VSFVM_RET_STACK_FAIL;
                    }
                }

                if (__vsfvm_push_func(thread)) {
                    return VSFVM_RET_STACK_FAIL;
                }

                func->argc = arg8;
                func->type = (VSFVM_CODE_FUNCTION_ID_t)id;
                func->arg_reg = sp;
                func->auto_reg = thread->stack.sp;
                func->expression_sp = 0;
                switch (func->type) {
                case VSFVM_CODE_FUNCTION_SCRIPT:
                    func->pc += (int16_t)arg16;
                    break;
                case VSFVM_CODE_FUNCTION_EXT:
                    ext = __vsfvm_get_extfunc(arg16);
                    if (!ext) return VSFVM_RET_ERROR;
                    func->handler = ext->handler;
                    break;
                case VSFVM_CODE_FUNCTION_THREAD:
                    __vsfvm_pop_func(thread);
                    if (!vsfvm_thread_init(runtime, script, 0, arg8, thread, NULL)) {
                        return VSFVM_RET_ERROR;
                    }
                    break;
                default:
                    return VSFVM_RET_ERROR;
                }
                break;
            }
        case VSFVM_CODE_TYPE_EOF:
            return VSFVM_RET_FINISHED;
        }
    }
}

void vsfvm_thread_ready(vsfvm_thread_t *thread)
{
    vsf_protect_t origlevel = vsf_protect_scheduler();
        vsf_slist_append(vsfvm_thread_t, ready_node, &thread->runtime->to_ready_list, thread);
    vsf_unprotect_scheduler(origlevel);
}

vsfvm_thread_t * vsfvm_thread_init(vsfvm_runtime_t *runtime,
    vsfvm_runtime_script_t *script, uint_fast16_t start_pos, uint_fast8_t argc,
    vsfvm_thread_t *orig_thread, vsfvm_var_t *arg)
{
    vsfvm_thread_t *thread = vsfvm_alloc_thread_imp(runtime);
    vsfvm_runtime_func_ctx_t *func;
    vsfvm_var_t *var;

    if (NULL == thread) goto fail_alloc_thread;
    memset(thread, 0, sizeof(*thread));
    func = &thread->func;

    __vsfvm_stack_init(&thread->stack);

    func->arg_reg = thread->stack.sp;
    // stack layout while calling a function:
    //    arg(s)
    //    func context
    //    auto variable(s)
    if (orig_thread != NULL) {
        int_fast8_t argc_declared;

        argc--;        // first argu is FUNCTION_POS
        for (uint_fast8_t i = 0; i < argc; i++) {
            var = vsfvm_thread_stack_get(orig_thread, argc - i - 1);
            if (!var) { goto fail_stack; }
            if (var->type == VSFVM_VAR_TYPE_REFERENCE) {
                var = vsfvm_get_ref(orig_thread, var);
            }
            if (vsfvm_thread_stack_push(thread, var->value, var->type, 1)) {
                goto fail_stack;
            }
        }
        for (uint_fast8_t i = 0; i < argc; i++) {
            vsfvm_thread_stack_pop(orig_thread, 1);
        }

        // leave the first arg as return value in stack
        var = vsfvm_thread_stack_get(orig_thread, 0);
        if (!var || (var->type != VSFVM_VAR_TYPE_FUNCTION)) {
            goto fail_stack;
        }
        argc_declared = (int8_t)VSFVM_CODE_ARG8(var->uval32);
        if ((argc_declared >= 0) && (argc_declared != argc)) {
            goto fail_stack;
        }
        start_pos = VSFVM_CODE_ARG16(var->uval32);
    } else if (arg != NULL) {
        for (uint_fast8_t i = 0; i < argc; i++, arg++) {
            if (vsfvm_thread_stack_push(thread, arg->value, arg->type, 1)) {
                goto fail_stack;
            }
        }
    }
    if (__vsfvm_push_func(thread)) {
        goto fail_stack;
    }
    if (!script->lvar_pos) {
        script->lvar_pos = thread->stack.sp;
    }
    func->auto_reg = thread->stack.sp;

    func->argc = argc;
    func->type = VSFVM_CODE_FUNCTION_SCRIPT;
    func->pc = start_pos;

    thread->runtime = runtime;
    thread->script = script;
    vsf_slist_add_to_head(vsfvm_thread_t, thread_node, &script->thread_list, thread);
    vsfvm_thread_ready(thread);
    return thread;

fail_stack:
    __vsfvm_stack_fini(&thread->stack);
    vsfvm_free_thread_imp(runtime, thread);
fail_alloc_thread:
    return NULL;
}

int vsfvm_runtime_script_init(vsfvm_runtime_t *runtime, vsfvm_runtime_script_t *script)
{
    script->state = VSFVM_SCRIPTSTAT_RUNNING;
    script->lvar_pos = 0;
    vsf_slist_add_to_head(vsfvm_runtime_script_t, script_node, &runtime->script_list, script);

    script->root_thread = vsfvm_thread_init(runtime, script, 0, script->param.argc, NULL, script->param.argv);
    if (!script->root_thread) {
        vsfvm_runtime_script_fini(runtime, script);
        return -1;
    }
    return 0;
}

int vsfvm_runtime_script_fini(vsfvm_runtime_t *runtime, vsfvm_runtime_script_t *script)
{
    script->state = VSFVM_SCRIPTSTAT_FINIED;

    __vsf_slist_foreach_next_unsafe(vsfvm_thread_t, thread_node, &script->thread_list) {
        __vsfvm_thread_fini_imp(runtime, _);
    }
    vsf_slist_init(&script->thread_list);
    vsf_slist_remove(vsfvm_runtime_script_t, script_node, &runtime->script_list, script);
    return 0;
}

bool vsfvm_runtime_is_thread_pending(vsfvm_runtime_t *runtime)
{
    return !vsf_slist_is_empty(&runtime->to_ready_list);
}

int vsfvm_runtime_poll(vsfvm_runtime_t *runtime)
{
    vsfvm_runtime_script_t *script;
    int ret = 0;

    vsf_protect_t origlevel = vsf_protect_scheduler();
        __vsf_slist_foreach_next_unsafe(vsfvm_thread_t, ready_node, &runtime->to_ready_list) {
            vsf_slist_append(vsfvm_thread_t, ready_node, &runtime->ready_list, _);
        }
        vsf_slist_init(&runtime->to_ready_list);
    vsf_unprotect_scheduler(origlevel);

    __vsf_slist_foreach_next_unsafe(vsfvm_thread_t, ready_node, &runtime->ready_list) {
        script = _->script;
        switch (script->state) {
        case VSFVM_SCRIPTSTAT_RUNNING:
            if (vsfvm_thread_run(runtime, _) < 0) {
                script->state = VSFVM_SCRIPTSTAT_ERROR;
                VSFVM_ASSERT(false);
                break;
            }
            ret++;
            break;
        case VSFVM_SCRIPTSTAT_UNKNOWN:
        case VSFVM_SCRIPTSTAT_ERROR:
        case VSFVM_SCRIPTSTAT_FINING:
        case VSFVM_SCRIPTSTAT_FINIED:
            break;
        }
        vsf_slist_remove(vsfvm_thread_t, ready_node, &runtime->ready_list, _);
    }
    return ret;
}

int vsfvm_gc(vsfvm_runtime_t *runtime)
{
#ifndef VSFVM_CFG_RUNTIME_STACK_SIZE
    vsfvm_runtime_script_t *script;
    __vsf_slist_foreach_unsafe(vsfvm_runtime_script_t, script_node, &runtime->script_list) {
        script = _;
        __vsf_slist_foreach_unsafe(vsfvm_thread_t, thread_node, &script->thread_list) {
            vsf_dynarr_set_size(&_->stack.use_as__vsf_dynarr_t, _->max_sp);
            _->max_sp = _->stack.sp;
        }
    }
#endif
    return 0;
}

int vsfvm_runtime_init(vsfvm_runtime_t *runtime)
{
    return 0;
}

int vsfvm_runtime_fini(vsfvm_runtime_t *runtime)
{
    return 0;
}

bool vsfvm_runtime_register_callback(vsfvm_runtime_callback_t *callback, vsfvm_thread_t *thread, vsfvm_var_t *func)
{
    if (func->type == VSFVM_VAR_TYPE_FUNCTION) {
        callback->runtime = thread->runtime;
        callback->script = thread->script;
        callback->func_pos = func->uval16;
        callback->argu_num = func->uval32 >> 16;
        return true;
    }
    return false;
}

vsfvm_thread_t * vsfvm_runtime_call_callback(vsfvm_runtime_callback_t *callback, uint_fast8_t argc, vsfvm_var_t *argv)
{
    if (callback->argu_num != argc) {
        return NULL;
    }
    return vsfvm_thread_init(callback->runtime, callback->script, callback->func_pos, argc, NULL, argv);
}

#endif      // VSFVM_CFG_RUNTIME_EN
