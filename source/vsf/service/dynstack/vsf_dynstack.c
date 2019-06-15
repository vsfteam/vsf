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

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_DYNARR == ENABLED

#include "./vsf_dynstack.h"
#include "../dynarr/vsf_dynarr.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_dynstack_init(vsf_dynstack_t *stack)
{
    stack->sp = 0;
    return vsf_dynarr_init(&stack->use_as__vsf_dynarr_t);
}

void vsf_dynstack_reset(vsf_dynstack_t *stack)
{
    stack->sp = 0;
    vsf_dynarr_fini(&stack->use_as__vsf_dynarr_t);
}

void vsf_dynstack_fini(vsf_dynstack_t *stack)
{
    vsf_dynstack_reset(stack);
}

void * vsf_dynstack_get(vsf_dynstack_t *stack, uint_fast32_t offset)
{
    if (offset >= stack->sp) {
        return NULL;
    }
    return vsf_dynarr_get(&stack->use_as__vsf_dynarr_t, stack->sp - offset - 1);
}

void * vsf_dynstack_pop(vsf_dynstack_t *stack, uint_fast16_t num)
{
    if ((stack->sp >= num) && (num > 0)) {
        void *out = vsf_dynstack_get(stack, num - 1);
        stack->sp -= num;
        return out;
    }
    return NULL;
}

vsf_err_t vsf_dynstack_push(vsf_dynstack_t *stack, void *item, uint_fast16_t num)
{
    uint_fast32_t stack_size = vsf_dynarr_get_size(&stack->use_as__vsf_dynarr_t);
    void *in;

    if (stack_size < (stack->sp + num)) {
        if (vsf_dynarr_set_size(&stack->use_as__vsf_dynarr_t, stack->sp + num) < 0) {
            return VSF_ERR_FAIL;
        }
    }

    stack->sp += num;
    while (num) {
        in = vsf_dynstack_get(stack, num - 1);
        memcpy(in, item, stack->item_size);
        num--;
    }
    return VSF_ERR_NONE;
}

vsf_err_t vsf_dynstack_push_ext(vsf_dynstack_t *stack, void *ptr, uint_fast32_t len)
{
    uint32_t *item = ptr;
    uint_fast32_t num = (len + 3) >> 2;
    uint_fast32_t item_size = stack->item_size >> 2;

    num = (num + item_size - 1) / item_size;
    while (num--) {
        if (vsf_dynstack_push(stack, item, 1) < 0) {
            return VSF_ERR_FAIL;
        }
        item += item_size;
    }
    return VSF_ERR_NONE;
}

vsf_err_t vsf_dynstack_pop_ext(vsf_dynstack_t *stack, void *ptr, uint_fast32_t len)
{
    uint_fast32_t num = (len + 3) >> 2;

    if (num) {
        uint32_t *item = ptr, *out;
        uint_fast32_t item_size = stack->item_size >> 2;
        int i;

        item += num - 1;
        while (num) {
            out = vsf_dynstack_pop(stack, 1);
            if (!out) {
                return VSF_ERR_FAIL;
            }

            for (i = (num % item_size) ? (num % item_size) - 1 : item_size - 1;
                (i >= 0) && (num > 0); i--, num--) {
                *item-- = out[i];
            }
        }
    }
    return VSF_ERR_NONE;
}

#endif      // VSF_USE_DYNARR
