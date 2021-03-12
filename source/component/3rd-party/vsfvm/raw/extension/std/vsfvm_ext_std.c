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

#include "../../vsf_vm_cfg.h"

#if (VSFVM_CFG_RUNTIME_EN == ENABLED) || (VSFVM_CFG_COMPILER_EN == ENABLED)

#define __VSFVM_RUNTIME_CLASS_INHERIT__
#include "../../common/vsfvm_common.h"
#include "../../runtime/vsfvm_runtime.h"
#include "./vsfvm_ext_std.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsfvm_ext_array_t {
    uint16_t dimension;
    uint16_t ele_size;
    uint32_t *dim_size;
    union {
        void *buffer;
        uint8_t *buf8;
        uint16_t *buf16;
        uint32_t *buf32;
        vsfvm_instance_t **inst;
    };
} vsfvm_ext_array_t;

enum {
    VSFVM_STD_EXTFUNC_PRINT = 0,
    VSFVM_STD_EXTFUNC_MEMSET,
    VSFVM_STD_EXTFUNC_MEMCPY,
    VSFVM_STD_EXTFUNC_ARRAY_CREATE,
    VSFVM_STD_EXTFUNC_ARRAY_GET,
    VSFVM_STD_EXTFUNC_ARRAY_SET,
    VSFVM_STD_EXTFUNC_BUFFER_CREATE,
    VSFVM_STD_EXTFUNC_BUFFER_GET_SIZE,
    VSFVM_STD_EXTFUNC_BUFFER_PACK,
    VSFVM_STD_EXTFUNC_BUFFER_PARSE,
    VSFVM_STD_EXTFUNC_POINTER_CREATE,
    VSFVM_STD_EXTFUNC_POINTER_SET,
    VSFVM_STD_EXTFUNC_POINTER_GET,
    VSFVM_STD_EXTFUNC_STRING_CREATE,
    VSFVM_STD_EXTFUNC_NUM,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vsfvm_ext_t __vsfvm_ext_std;

/*============================ PROTOTYPES ====================================*/

// for instance array
extern void vsfvm_instance_ref(vsfvm_instance_t *inst);
extern bool vsfvm_instance_deref(vsfvm_instance_t *inst);

/*============================ IMPLEMENTATION ================================*/

#if VSFVM_CFG_RUNTIME_EN == ENABLED
static vsfvm_ret_t __vsfvm_ext_print(vsfvm_thread_t *thread)
{
    vsfvm_var_t *var;
    int_fast32_t size;
    uint8_t *buffer;

    for (uint_fast8_t i = 0; i < thread->func.argc; i++) {
        var = vsfvm_get_func_argu_ref(thread, i);
        if (!var) { return VSFVM_RET_ERROR; }

        switch (var->type) {
        case VSFVM_VAR_TYPE_VALUE:
            vsf_trace_info("%d", var->value);
            break;
        case VSFVM_VAR_TYPE_RESOURCES:
            size = vsfvm_get_res(thread->script, var->value, &buffer);
            if (size < 0) {
                return VSFVM_RET_ERROR;
            }
            vsf_trace_info("%s", buffer);
            break;
        case VSFVM_VAR_TYPE_FUNCTION:
            vsf_trace_info("function@%d", var->uval16);
            break;
        case VSFVM_VAR_TYPE_INSTANCE:
            if (var->inst != NULL) {
                if (var->inst->c->op.print != NULL) {
                    var->inst->c->op.print(var->inst);
                } else {
                    vsf_trace_buffer(VSF_TRACE_INFO, var->inst->buffer,
                        var->inst->size, VSF_TRACE_DF_U8_16);
                }
            } else {
                vsf_trace_info("NULL");
            }
            break;
        default:
            vsf_trace_info("unknown type(%d)", var->type);
        }
    }
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_memset(vsfvm_thread_t *thread)
{
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *value = vsfvm_get_func_argu_ref(thread, 1);
    vsfvm_var_t *size = vsfvm_get_func_argu_ref(thread, 2);

    if (!thiz || !thiz->inst || (thiz->type != VSFVM_VAR_TYPE_INSTANCE)) {
        return VSFVM_RET_ERROR;
    }

    memset(thiz->inst->buffer, value->uval8, size->uval32);
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_memcpy(vsfvm_thread_t *thread)
{
    vsfvm_var_t *dst = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *src = vsfvm_get_func_argu_ref(thread, 1);
    vsfvm_var_t *size = vsfvm_get_func_argu_ref(thread, 2);

    if (    !src || !src->inst || !dst || !dst->inst
        ||  (src->type != VSFVM_VAR_TYPE_INSTANCE)
        ||  (dst->type != VSFVM_VAR_TYPE_INSTANCE)) {
        return VSFVM_RET_ERROR;
    }

    memcpy(dst->inst->buffer, src->inst->buffer, size->uval32);
    return VSFVM_RET_FINISHED;
}



static vsfvm_ret_t __vsfvm_ext_array_create(vsfvm_thread_t *thread)
{
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vsfvm_var_t *dimension = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *ele_size = vsfvm_get_func_argu_ref(thread, 1);
    vsfvm_var_t *var;
    vsfvm_ext_array_t *arr;
    uint_fast32_t size, buffsize, dim;

    if (    !dimension || (thread->func.argc != (2 + dimension->uval8))
        ||  (   (ele_size->uval8 != 0) && (ele_size->uval8 != 1)
             && (ele_size->uval8 != 2) && (ele_size->uval8 != 4))) {
        return VSFVM_RET_ERROR;
    }
    dim = dimension->uval8;

    size = sizeof(*arr) + dim * sizeof(uint32_t);
    buffsize = ele_size->uval8 > 0 ? ele_size->uval8 : sizeof(vsfvm_instance_t *);
    for (uint_fast8_t i = 0; i < dim; i++) {
        var = vsfvm_get_func_argu_ref(thread, i + 2);
        if (!var) { return VSFVM_RET_ERROR; }
        buffsize *= var->uval8;
    }
    size += buffsize;

    if (vsfvm_var_alloc_instance(thread, result, size, &vsfvm_ext_array)) {
        return VSFVM_RET_ERROR;
    }
    arr = result->inst->obj_ptr;
    arr->dimension = dim;
    arr->ele_size = ele_size->uval8;
    arr->dim_size = (uint32_t *)&arr[1];
    arr->buffer = &arr->dim_size[arr->dimension];
    for (uint_fast8_t i = 0; i < dim; i++) {
        var = vsfvm_get_func_argu_ref(thread, i + 2);
        arr->dim_size[i] = var->uval8;
    }
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_array_get(vsfvm_thread_t *thread)
{
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *var;
    vsfvm_ext_array_t *arr;
    uint_fast32_t pos, size, allsize, value;

    if (!thiz || !vsfvm_var_instance_of(thiz, &vsfvm_ext_array)) {
        return VSFVM_RET_ERROR;
    }
    arr = thiz->inst->obj_ptr;
    if (thread->func.argc != (1 + arr->dimension)) {
        return VSFVM_RET_ERROR;
    }

    pos = 0;
    allsize = 1;
    for (uint_fast8_t i = 0; i < arr->dimension; i++) {
        var = vsfvm_get_func_argu_ref(thread, i + 1);
        size = 1;
        for (uint8_t j = i + 1; j < arr->dimension; j++) {
            size *= arr->dim_size[j];
        }
        pos += size * var->uval8;
        allsize *= arr->dim_size[i];
    }
    if (pos >= allsize) {
        return VSFVM_RET_ERROR;
    }

    if (0 == arr->ele_size) {
        vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_INSTANCE, (intptr_t)arr->inst[pos]);
    } else {
        value = 0;
        switch (arr->ele_size) {
        case 1: value = arr->buf8[pos];     break;
        case 2: value = arr->buf16[pos];   break;
        case 4: value = arr->buf32[pos];   break;
        }
        vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, value);
    }
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_array_set(vsfvm_thread_t *thread)
{
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *var;
    vsfvm_ext_array_t *arr;
    uint_fast32_t pos, size, allsize;

    if (!thiz || !vsfvm_var_instance_of(thiz, &vsfvm_ext_array)) {
        return VSFVM_RET_ERROR;
    }
    arr = thiz->inst->obj_ptr;
    if (thread->func.argc < (1 + arr->dimension)) {
        return VSFVM_RET_ERROR;
    }

    pos = 0;
    allsize = 1;
    for (uint_fast8_t i = 0; i < arr->dimension; i++) {
        var = vsfvm_get_func_argu_ref(thread, i + 1);
        size = 1;
        for (uint8_t j = i + 1; j < arr->dimension; j++) {
            size *= arr->dim_size[j];
        }
        pos += size * var->uval8;
        allsize *= arr->dim_size[i];
    }
    for (uint_fast8_t i = 1 + arr->dimension; i < thread->func.argc; i++) {
        var = vsfvm_get_func_argu_ref(thread, i);
        if (pos >= allsize) { return VSFVM_RET_ERROR; }
        switch (arr->ele_size) {
        case 0:
            if (arr->inst[pos] != NULL) {
                vsfvm_instance_deref(arr->inst[pos]);
            }
            vsfvm_instance_ref(var->inst);
            arr->inst[pos++] = var->inst;
            break;
        case 1: arr->buf8[pos++] = var->uval8;      break;
        case 2: arr->buf16[pos++] = var->uval16;    break;
        case 4: arr->buf32[pos++] = var->uval32;    break;
        }
    }
    return VSFVM_RET_FINISHED;
}

static void __vsfvm_ext_array_print(vsfvm_instance_t *inst)
{
    
}

static vsfvm_ret_t __vsfvm_ext_buffer_create(vsfvm_thread_t *thread)
{
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vsfvm_var_t *arg0 = vsfvm_get_func_argu_ref(thread, 0);
    uint8_t *buffer;

    if (!arg0) { return VSFVM_RET_INVALID_PARAM; }
    if (arg0->type == VSFVM_VAR_TYPE_INSTANCE) {
        if (thread->func.argc != 2) {
            return VSFVM_RET_INVALID_PARAM;
        } else {
            vsfvm_var_t *arg1 = vsfvm_get_func_argu_ref(thread, 1);
            uint8_t *ptr = arg0->inst->buffer;

            if (vsfvm_var_alloc_instance(thread, result, 0, &vsfvm_ext_buffer)) {
                return VSFVM_RET_ERROR;
            }
            result->inst->buffer = ptr;
            result->inst->size = arg1->uval32;
        }
    } else {
        if (thread->func.argc != 1) {
            return VSFVM_RET_INVALID_PARAM;
        } else {
            uint32_t size = arg0->uval32;
            if (vsfvm_var_alloc_instance(thread, result, size, &vsfvm_ext_buffer)) {
                return VSFVM_RET_ERROR;
            }
            buffer = result->inst->buffer;
            memset(buffer, 0, size);
        }
    }
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_buffer_get_size(vsfvm_thread_t *thread)
{
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    uint_fast32_t size;

    if (!thiz || !vsfvm_var_instance_of(thiz, &vsfvm_ext_buffer)) {
        return VSFVM_RET_INVALID_PARAM;
    }

    size = thiz->inst->size;
    vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, size);
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_buffer_pack(vsfvm_thread_t *thread)
{
    // TODO: implement pack function
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_buffer_parse(vsfvm_thread_t *thread)
{
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    uint_fast8_t radix = vsfvm_get_func_argu_ref(thread, 1)->uval8;
    vsfvm_var_t *hex = vsfvm_get_func_argu_ref(thread, 2);
    char *str;
    uint8_t *buf = thiz->inst->buffer;
    int pos = 0;

    if (NULL == buf) {
        return VSFVM_RET_ERROR;
    }

    switch (hex->type) {
    case VSFVM_VAR_TYPE_RESOURCES:
        if (vsfvm_get_res(thread->script, hex->value, (uint8_t **)&str) < 0) {
            return VSFVM_RET_ERROR;
        }
        break;
    case VSFVM_VAR_TYPE_INSTANCE:
        if (NULL == hex->inst) {
            return VSFVM_RET_ERROR;
        }
        str = (char *)hex->inst->buffer;
        break;
    }

    while ((str != NULL) && (pos < thiz->inst->size)) {
        buf[pos++] = strtoul(str, &str, radix);
    }
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_pointer_create(vsfvm_thread_t *thread)
{
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vsfvm_var_t *arg0 = vsfvm_get_func_argu_ref(thread, 0);

    if (!arg0) { return VSFVM_RET_INVALID_PARAM; }
    if (arg0->type == VSFVM_VAR_TYPE_INSTANCE) {
        if (thread->func.argc != 3) {
            return VSFVM_RET_INVALID_PARAM;
        } else {
            vsfvm_var_t *offset = vsfvm_get_func_argu_ref(thread, 1);
            vsfvm_var_t *size = vsfvm_get_func_argu_ref(thread, 2);
            uint8_t *addr = arg0->inst->buffer;

            if ((size->uval32 != 1) && (size->uval32 != 2) && (size->uval32 != 4)) {
                return VSFVM_RET_INVALID_PARAM;
            }
            if (vsfvm_var_alloc_instance(thread, result, 0, &vsfvm_ext_pointer)) {
                return VSFVM_RET_ERROR;
            }
            result->inst->buffer = &addr[offset->uval32 * size->uval32];
            result->inst->size = size->uval32;
        }
    } else {
        if (thread->func.argc != 2) {
            return VSFVM_RET_INVALID_PARAM;
        } else {
            vsfvm_var_t *size = vsfvm_get_func_argu_ref(thread, 1);
            uint8_t *addr = (uint8_t *)arg0->uval32;

            if ((size->uval32 != 1) && (size->uval32 != 2) && (size->uval32 != 4)) {
                return VSFVM_RET_INVALID_PARAM;
            }
            if (vsfvm_var_alloc_instance(thread, result, 0, &vsfvm_ext_pointer)) {
                return VSFVM_RET_ERROR;
            }
            result->inst->buffer = addr;
            result->inst->size = size->uval32;
        }
    }
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_pointer_set(vsfvm_thread_t *thread)
{
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *offset = vsfvm_get_func_argu_ref(thread, 1);
    vsfvm_var_t *value = vsfvm_get_func_argu_ref(thread, 2);
    vsfvm_instance_t *inst;

    if (!thiz || !vsfvm_var_instance_of(thiz, &vsfvm_ext_pointer)) {
        return VSFVM_RET_INVALID_PARAM;
    }

    inst = thiz->inst;
    switch (inst->size) {
    case 1:
        ((uint8_t *)inst->buffer)[offset->uval32] = value->uval8;
        break;
    case 2:
        ((uint16_t *)inst->buffer)[offset->uval32] = value->uval16;
        break;
    case 4:
        ((uint32_t *)inst->buffer)[offset->uval32] = value->uval32;
        break;
    default:
        return VSFVM_RET_INVALID_PARAM;
    }
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_pointer_get(vsfvm_thread_t *thread)
{
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *offset = vsfvm_get_func_argu_ref(thread, 1);
    vsfvm_instance_t *inst;
    uint_fast32_t value;

    if (!thiz || !vsfvm_var_instance_of(thiz, &vsfvm_ext_pointer)) {
        return VSFVM_RET_INVALID_PARAM;
    }

    inst = thiz->inst;
    switch (inst->size) {
    case 1:
        value = ((uint8_t *)inst->buffer)[offset->uval32];
        break;
    case 2:
        value = ((uint16_t *)inst->buffer)[offset->uval32];
        break;
    case 4:
        value = ((uint32_t *)inst->buffer)[offset->uval32];
        break;
    default:
        return VSFVM_RET_INVALID_PARAM;
    }
    vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, value);
    return VSFVM_RET_FINISHED;
}

static void __vsfvm_ext_string_print(vsfvm_instance_t *inst)
{
    vsf_trace_info("%s", (char *)inst->buffer);
}

static vsfvm_ret_t __vsfvm_ext_string_create(vsfvm_thread_t *thread)
{
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vsfvm_var_t *buffer = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *offset = vsfvm_get_func_argu_ref(thread, 1);

    if (buffer->type != VSFVM_VAR_TYPE_INSTANCE) {
        return VSFVM_RET_INVALID_PARAM;
    }

    char *str = (char *)&buffer->inst->buffer[offset->uval32];
    if (vsfvm_var_alloc_instance(thread, result, 0, &vsfvm_ext_string)) {
        return VSFVM_RET_ERROR;
    }
    result->inst->buffer = (uint8_t *)str;
    return VSFVM_RET_FINISHED;
}
#endif

const vsfvm_class_t vsfvm_ext_array = {
#if VSFVM_CFG_COMPILER_EN == ENABLED
    .name = "array",
#endif
#if VSFVM_CFG_RUNTIME_EN == ENABLED
    .type = VSFVM_CLASS_ARRAY,
    .op.print = __vsfvm_ext_array_print,
#endif
};
const vsfvm_class_t vsfvm_ext_buffer = {
#if VSFVM_CFG_COMPILER_EN == ENABLED
    .name = "buffer",
#endif
#if VSFVM_CFG_RUNTIME_EN == ENABLED
    .type = VSFVM_CLASS_BUFFER,
#endif
};
const vsfvm_class_t vsfvm_ext_pointer = {
#if VSFVM_CFG_COMPILER_EN == ENABLED
    .name = "pointer",
#endif
#if VSFVM_CFG_RUNTIME_EN == ENABLED
    .type = VSFVM_CLASS_POINTER,
#endif
};
const vsfvm_class_t vsfvm_ext_string = {
#if VSFVM_CFG_COMPILER_EN == ENABLED
    .name = "string",
#endif
#if VSFVM_CFG_RUNTIME_EN == ENABLED
    .type = VSFVM_CLASS_STRING,
    .op.print = __vsfvm_ext_string_print,
#endif
};

#if VSFVM_CFG_COMPILER_EN == ENABLED
static const vsfvm_ext_op_t __vsfvm_ext_std_op;
static const vsfvm_lexer_sym_t __vsfvm_ext_std_sym[] = {
    VSFVM_LEXERSYM_CONST("NULL", &__vsfvm_ext_std_op, NULL, 0),
    VSFVM_LEXERSYM_CONST("true", &__vsfvm_ext_std_op, NULL, 1),
    VSFVM_LEXERSYM_CONST("false", &__vsfvm_ext_std_op, NULL, 0),

    VSFVM_LEXERSYM_EXTFUNC("print", &__vsfvm_ext_std_op, NULL, NULL, -1, VSFVM_STD_EXTFUNC_PRINT),
    VSFVM_LEXERSYM_EXTFUNC("memset", &__vsfvm_ext_std_op, NULL, NULL, 3, VSFVM_STD_EXTFUNC_MEMSET),
    VSFVM_LEXERSYM_EXTFUNC("memcpy", &__vsfvm_ext_std_op, NULL, NULL, 3, VSFVM_STD_EXTFUNC_MEMCPY),

    VSFVM_LEXERSYM_CLASS("array", &__vsfvm_ext_std_op, &vsfvm_ext_array),
    VSFVM_LEXERSYM_EXTFUNC("array_create", &__vsfvm_ext_std_op, NULL, &vsfvm_ext_array, -1, VSFVM_STD_EXTFUNC_ARRAY_CREATE),
    VSFVM_LEXERSYM_EXTFUNC("array_get", &__vsfvm_ext_std_op, &vsfvm_ext_array, NULL, -1, VSFVM_STD_EXTFUNC_ARRAY_GET),
    VSFVM_LEXERSYM_EXTFUNC("array_set", &__vsfvm_ext_std_op, &vsfvm_ext_array, &vsfvm_ext_array, -1, VSFVM_STD_EXTFUNC_ARRAY_SET),

    VSFVM_LEXERSYM_CLASS("buffer", &__vsfvm_ext_std_op, &vsfvm_ext_buffer),
    VSFVM_LEXERSYM_EXTFUNC("buffer_create", &__vsfvm_ext_std_op, NULL, &vsfvm_ext_buffer, -1, VSFVM_STD_EXTFUNC_BUFFER_CREATE),
    VSFVM_LEXERSYM_EXTFUNC("buffer_get_size", &__vsfvm_ext_std_op, &vsfvm_ext_buffer, NULL, 1, VSFVM_STD_EXTFUNC_BUFFER_GET_SIZE),
    VSFVM_LEXERSYM_EXTFUNC("buffer_pack", &__vsfvm_ext_std_op, &vsfvm_ext_buffer, NULL, -1, VSFVM_STD_EXTFUNC_BUFFER_PACK),
    VSFVM_LEXERSYM_EXTFUNC("buffer_parse", &__vsfvm_ext_std_op, &vsfvm_ext_buffer, NULL, 3, VSFVM_STD_EXTFUNC_BUFFER_PARSE),

    VSFVM_LEXERSYM_CLASS("pointer", &__vsfvm_ext_std_op, &vsfvm_ext_pointer),
    VSFVM_LEXERSYM_EXTFUNC("pointer_create", &__vsfvm_ext_std_op, NULL, &vsfvm_ext_pointer, -1, VSFVM_STD_EXTFUNC_POINTER_CREATE),
    VSFVM_LEXERSYM_EXTFUNC("pointer_set", &__vsfvm_ext_std_op, &vsfvm_ext_pointer, &vsfvm_ext_pointer, 3, VSFVM_STD_EXTFUNC_POINTER_SET),
    VSFVM_LEXERSYM_EXTFUNC("pointer_get", &__vsfvm_ext_std_op, &vsfvm_ext_pointer, NULL, 2, VSFVM_STD_EXTFUNC_POINTER_GET),

    VSFVM_LEXERSYM_CLASS("string", &__vsfvm_ext_std_op, &vsfvm_ext_string),
    VSFVM_LEXERSYM_EXTFUNC("string_create", &__vsfvm_ext_std_op, NULL, &vsfvm_ext_string, 2, VSFVM_STD_EXTFUNC_STRING_CREATE),
};
#endif

#if VSFVM_CFG_RUNTIME_EN == ENABLED
static const vsfvm_extfunc_t __vsfvm_ext_std_func[VSFVM_STD_EXTFUNC_NUM] = {
    [VSFVM_STD_EXTFUNC_PRINT] = VSFVM_EXTFUNC(__vsfvm_ext_print, -1),
    [VSFVM_STD_EXTFUNC_MEMSET] = VSFVM_EXTFUNC(__vsfvm_ext_memset, 3),
    [VSFVM_STD_EXTFUNC_MEMCPY] = VSFVM_EXTFUNC(__vsfvm_ext_memcpy, 3),
    // array class
    [VSFVM_STD_EXTFUNC_ARRAY_CREATE] = VSFVM_EXTFUNC(__vsfvm_ext_array_create, -1),
    [VSFVM_STD_EXTFUNC_ARRAY_GET] = VSFVM_EXTFUNC(__vsfvm_ext_array_get, -1),
    [VSFVM_STD_EXTFUNC_ARRAY_SET] = VSFVM_EXTFUNC(__vsfvm_ext_array_set, -1),
    // buffer class
    [VSFVM_STD_EXTFUNC_BUFFER_CREATE] = VSFVM_EXTFUNC(__vsfvm_ext_buffer_create, -1),
    [VSFVM_STD_EXTFUNC_BUFFER_GET_SIZE] = VSFVM_EXTFUNC(__vsfvm_ext_buffer_get_size, 1),
    [VSFVM_STD_EXTFUNC_BUFFER_PACK] = VSFVM_EXTFUNC(__vsfvm_ext_buffer_pack, -1),
    [VSFVM_STD_EXTFUNC_BUFFER_PARSE] = VSFVM_EXTFUNC(__vsfvm_ext_buffer_parse, 3),
    // pointer class
    [VSFVM_STD_EXTFUNC_POINTER_CREATE] = VSFVM_EXTFUNC(__vsfvm_ext_pointer_create, -1),
    [VSFVM_STD_EXTFUNC_POINTER_SET] = VSFVM_EXTFUNC(__vsfvm_ext_pointer_set, 3),
    [VSFVM_STD_EXTFUNC_POINTER_GET] = VSFVM_EXTFUNC(__vsfvm_ext_pointer_get, 2),
    // string class
    [VSFVM_STD_EXTFUNC_STRING_CREATE] = VSFVM_EXTFUNC(__vsfvm_ext_string_create, 2),
};
#endif

static const vsfvm_ext_op_t __vsfvm_ext_std_op = {
#if VSFVM_CFG_COMPILER_EN == ENABLED
    .name = "std",
    .sym = __vsfvm_ext_std_sym,
    .sym_num = dimof(__vsfvm_ext_std_sym),
#endif
#if VSFVM_CFG_RUNTIME_EN == ENABLED
    .init = NULL,
    .fini = NULL,
    .func = (vsfvm_extfunc_t *)__vsfvm_ext_std_func,
#endif
    .func_num = dimof(__vsfvm_ext_std_func),
};

void vsfvm_ext_register_std(void)
{
    __vsfvm_ext_std.op = &__vsfvm_ext_std_op;
    vsfvm_register_ext(&__vsfvm_ext_std);
}

#endif      // VSFVM_CFG_RUNTIME_EN || VSFVM_CFG_COMPILER_EN
