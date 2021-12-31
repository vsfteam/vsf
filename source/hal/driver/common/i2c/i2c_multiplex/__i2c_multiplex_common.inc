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


/*============================ INCLUDES ======================================*/

#define __VSF_I2C_MULTIPLEX_CLASS_IMPLEMENT

#if VSF_HAL_I2C_IMP_MULTIPLEX_I2C == ENABLED

#include "./__i2c_multiplex_common.h"

/*============================ MACROS ========================================*/

#ifndef VSF_MULTIPLEX_I2C_PROTECT_LEVEL
#   define VSF_MULTIPLEX_I2C_PROTECT_LEVEL  interrupt
#endif
#define vsf_multiplex_i2c_protect           vsf_protect(VSF_MULTIPLEX_I2C_PROTECT_LEVEL)
#define vsf_multiplex_i2c_unprotect         vsf_unprotect(VSF_MULTIPLEX_I2C_PROTECT_LEVEL)

#ifndef I2C_MULTIPLEX_CFG_DEBUG
#   define I2C_MULTIPLEX_CFG_DEBUG          DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __MULTIPLEXER_I2C_DEF(__N)                                              \
    vsf_i2c_multiplexer_t __MULTIPLEXER##__N = {                                \
        .request_slist = {                                                      \
            .head = {                                                           \
                .next = NULL,                                                   \
            },                                                                  \
            .tail = {                                                           \
                .next = NULL,                                                   \
            },                                                                  \
        },                                                                      \
        .i2c_ptr = &i2c_instance(__N)                                           \
    };

#define __VSF_MULTIPLEX_I2C_DEF(__N, __MI2C)                                    \
    vsf_i2c_multiplex_t __MI2C##__N = {                                         \
        .multiplexer = &__MULTIPLEXER,                                          \
        .use_as__request_info_t = {                                             \
            .request_node = {                                                   \
                .next = NULL,                                                   \
            },                                                                  \
            .cfg = {                                                            \
                .isr = {                                                        \
                    .handler_fn = NULL,                                         \
                    .target_ptr = NULL,                                         \
                },                                                              \
            },                                                                  \
            .buffer_ptr = NULL,                                                 \
            .status = {                                                         \
                    .is_enabled = false,                                        \
                    .is_irq_enabled = false,                                    \
            },                                                                  \
        },                                                                      \
    };

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vsf_i2c_master_multiplex_request(vsf_i2c_multiplex_t *i2c_multiplex, request_info_t *request);

static void vsf_i2c_multiplex_isrhandler(void *target_ptr,
                                         vsf_i2c_t *i2c_ptr,
                                         uint32_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != target_ptr)
    vsf_err_t err;
    request_info_t *request = NULL;
    vsf_slist_queue_peek(request_info_t,
                         request_node,
                         &((vsf_i2c_multiplex_t *)(target_ptr))->multiplexer->request_slist,
                         request);
    vsf_i2c_multiplex_t *i2c_multiplex = container_of(request, vsf_i2c_multiplex_t, use_as__request_info_t);

    i2c_multiplex->status.is_in_queue = false;
    i2c_multiplex->status.is_busy = false;

    (i2c_multiplex->cmd & I2C_CMD_STOP) ?
        (i2c_multiplex->status.is_restar = false) :
         (i2c_multiplex->status.is_restar = true);
    if (i2c_multiplex->cfg.isr.handler_fn != NULL) {
        i2c_multiplex->cfg.isr.handler_fn(i2c_multiplex->cfg.isr.target_ptr,
                                          i2c_ptr, irq_mask);
    }
    if (!(i2c_multiplex->cmd & I2C_CMD_STOP)) {
        i2c_multiplex->status.is_in_queue = true;
        return;
    }
    vsf_slist_queue_dequeue(request_info_t,
                            request_node,
                            &i2c_multiplex->multiplexer->request_slist,
                            request);
    if (vsf_slist_queue_is_empty(&i2c_multiplex->multiplexer->request_slist)) {
        i2c_multiplex->multiplexer->is_busy = false;
        return;
    }
    vsf_slist_queue_peek(request_info_t,
                         request_node,
                         &((vsf_i2c_multiplex_t *)(target_ptr))->multiplexer->request_slist,
                         request);
    i2c_multiplex = container_of(request, vsf_i2c_multiplex_t, use_as__request_info_t);

    i2c_multiplex->status.is_in_queue = false;
    i2c_multiplex->status.is_busy = false;
    err = __vsf_i2c_master_multiplex_request(i2c_multiplex, request);
    if (VSF_ERR_NONE != err) {
        //todo
        if (i2c_multiplex->cfg.isr.handler_fn != NULL) {
            i2c_multiplex->cfg.isr.handler_fn(i2c_multiplex->cfg.isr.target_ptr,
                                              i2c_ptr, I2C_IRQ_MASK_MASTER_ERROR);
        }
    }
}

vsf_err_t vsf_i2c_multiplex_init(vsf_i2c_multiplex_t *i2c_multiplex, i2c_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_multiplex);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(NULL != i2c_multiplex->multiplexer);

    i2c_multiplex->status.is_init = true;
    i2c_multiplex->cfg = *cfg_ptr;
    cfg_ptr->isr.handler_fn = vsf_i2c_multiplex_isrhandler;
    cfg_ptr->isr.target_ptr = i2c_multiplex;
    return vsf_i2c_init(i2c_multiplex->multiplexer->i2c_ptr, cfg_ptr);
}

void vsf_i2c_multiplex_fini(vsf_i2c_multiplex_t *i2c_multiplex)
{
    VSF_HAL_ASSERT(NULL != i2c_multiplex);
    VSF_HAL_ASSERT(NULL != i2c_multiplex->multiplexer);

    i2c_multiplex->status.is_init = false;
    vsf_i2c_fini(i2c_multiplex->multiplexer->i2c_ptr);
}

fsm_rt_t vsf_i2c_multiplex_enable(vsf_i2c_multiplex_t *i2c_multiplex)
{
    VSF_HAL_ASSERT(NULL != i2c_multiplex);
    VSF_HAL_ASSERT(NULL != i2c_multiplex->multiplexer);
    if (!i2c_multiplex->status.is_init) {
        return fsm_rt_err;
    }
    i2c_multiplex->status.is_enabled = true;
    return vsf_i2c_enable(i2c_multiplex->multiplexer->i2c_ptr);
}

fsm_rt_t vsf_i2c_multiplex_disable(vsf_i2c_multiplex_t *i2c_multiplex)
{
    VSF_HAL_ASSERT(NULL != i2c_multiplex);
    VSF_HAL_ASSERT(NULL != i2c_multiplex->multiplexer);
    if (!i2c_multiplex->status.is_init) {
        return fsm_rt_err;
    }
    i2c_multiplex->status.is_enabled = false;
    return vsf_i2c_disable(i2c_multiplex->multiplexer->i2c_ptr);
}

void vsf_i2c_multiplex_irq_enable(vsf_i2c_multiplex_t *i2c_multiplex, em_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != i2c_multiplex);
    VSF_HAL_ASSERT(NULL != i2c_multiplex->multiplexer);

    if (!i2c_multiplex->status.is_init) {
        return;
    }
    i2c_multiplex->status.is_irq_enabled = true;
    i2c_multiplex->irq_mask = irq_mask;
    vsf_i2c_irq_enable(i2c_multiplex->multiplexer->i2c_ptr, irq_mask);
}

void vsf_i2c_multiplex_irq_disable(vsf_i2c_multiplex_t *i2c_multiplex, em_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != i2c_multiplex);
    VSF_HAL_ASSERT(NULL != i2c_multiplex->multiplexer);

    if (!i2c_multiplex->status.is_init) {
        return;
    }
    i2c_multiplex->status.is_irq_enabled = false;
    vsf_i2c_irq_disable(i2c_multiplex->multiplexer->i2c_ptr, irq_mask);
}

i2c_type_status vsf_i2c_multiplex_status(vsf_i2c_multiplex_t *i2c_multiplex)
{
    VSF_HAL_ASSERT(NULL != i2c_multiplex);
    VSF_HAL_ASSERT(NULL != i2c_multiplex->multiplexer);

    if (!i2c_multiplex->status.is_init) {
        return (i2c_type_status){0};
    }
    return vsf_i2c_status(i2c_multiplex->multiplexer->i2c_ptr);
}

static vsf_err_t __vsf_i2c_master_multiplex_request(vsf_i2c_multiplex_t *i2c_multiplex, request_info_t *request)
{
    vsf_err_t err;
    fsm_rt_t fsm;
    i2c_cfg_t cfg = i2c_multiplex->cfg;
    fsm = vsf_i2c_multiplex_disable(i2c_multiplex);
    if (fsm_rt_cpl != fsm) {
        return VSF_ERR_FAIL;
    }
    err = vsf_i2c_multiplex_init(i2c_multiplex, &cfg);
    if (VSF_ERR_NONE != err) {
        return err;
    }
    fsm = vsf_i2c_multiplex_enable(i2c_multiplex);
    if (fsm_rt_cpl != fsm) {
        return VSF_ERR_FAIL;
    }
    vsf_i2c_multiplex_irq_enable(i2c_multiplex, i2c_multiplex->irq_mask);
    return vsf_i2c_master_request(i2c_multiplex->multiplexer->i2c_ptr,
                                  request->address,
                                  request->cmd,
                                  request->count,
                                  request->buffer_ptr);
}

vsf_err_t vsf_i2c_master_multiplex_request( vsf_i2c_multiplex_t *i2c_multiplex,
                                            uint16_t address,
                                            em_i2c_cmd_t cmd,
                                            uint16_t count,
                                            uint8_t *buffer_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_multiplex);
    VSF_HAL_ASSERT(NULL != i2c_multiplex->multiplexer);

    vsf_i2c_multiplexer_t *multiplexer = i2c_multiplex->multiplexer;
    request_info_t *request = &i2c_multiplex->use_as__request_info_t;
    vsf_err_t err = VSF_ERR_NONE;

    if (    (i2c_multiplex->status.is_busy)
        ||  (NULL == buffer_ptr)) {
        VSF_HAL_ASSERT(false);//todo
        return VSF_ERR_FAIL;
    }
    if (!i2c_multiplex->status.is_enabled) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    i2c_multiplex->status.is_busy = true;
    request->cmd = cmd;
    request->address = address;
    request->count = count;
    request->buffer_ptr = buffer_ptr;

#if I2C_MULTIPLEX_CFG_DEBUG == ENABLED
        request_info_t *temp_req_info = (request_info_t *)(&multiplexer->request_slist.head)->request_node.next;
        while (NULL != temp_req_info) {
            if (temp_req_info->address == address) {
                VSF_HAL_ASSERT(false);
                return VSF_ERR_FAIL;
            }
            temp_req_info = temp_req_info->request_node.next;
        }
#endif

    vsf_protect_t state = vsf_multiplex_i2c_protect();

    if (!i2c_multiplex->status.is_in_queue) {
        i2c_multiplex->status.is_in_queue = true;

        vsf_slist_queue_enqueue(request_info_t,
                                request_node,
                                &multiplexer->request_slist,
                                request);
        if (i2c_multiplex->multiplexer->is_busy) {//real hardware is busy
            vsf_multiplex_i2c_unprotect(state);
            return err;
        }
    } else if (!i2c_multiplex->status.is_restar) {
        VSF_HAL_ASSERT(false);//todo
        return VSF_ERR_FAIL;
    }
    i2c_multiplex->multiplexer->is_busy = true;
    err = __vsf_i2c_master_multiplex_request(i2c_multiplex, request);
    vsf_multiplex_i2c_unprotect(state);
    return err;
}

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C0 == ENABLED && (I2C_PORT_MASK & (1 << 0))
#   ifndef VSF_HAL_I2C0_MULTIPLEX_CNT
#       define VSF_HAL_I2C0_MULTIPLEX_CNT   2
#   endif
__MULTIPLEXER_I2C_DEF(0)
#undef __MULTIPLEXER
#define __MULTIPLEXER                   __MULTIPLEXER0
VSF_MREPEAT(VSF_HAL_I2C0_MULTIPLEX_CNT, __VSF_MULTIPLEX_I2C_DEF, VSF_I2C0_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C1 == ENABLED && (I2C_PORT_MASK & (1 << 1))
#   ifndef VSF_HAL_I2C1_MULTIPLEX_CNT
#       define VSF_HAL_I2C1_MULTIPLEX_CNT 1
#   endif
__MULTIPLEXER_I2C_DEF(1)
#undef __MULTIPLEXER
#define __MULTIPLEXER                   __MULTIPLEXER1
VSF_MREPEAT(VSF_HAL_I2C1_MULTIPLEX_CNT, __VSF_MULTIPLEX_I2C_DEF, VSF_I2C1_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C2 == ENABLED && (I2C_PORT_MASK & (1 << 2))
#   ifndef VSF_HAL_I2C2_MULTIPLEX_CNT
#       define VSF_HAL_I2C2_MULTIPLEX_CNT 1
#   endif
__MULTIPLEXER_I2C_DEF(2)
#undef __MULTIPLEXER
#define __MULTIPLEXER                   __MULTIPLEXER2
VSF_MREPEAT(VSF_HAL_I2C2_MULTIPLEX_CNT, __VSF_MULTIPLEX_I2C_DEF, VSF_I2C2_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C3 == ENABLED && (I2C_PORT_MASK & (1 << 3))
#   ifndef VSF_HAL_I2C3_MULTIPLEX_CNT
#       define VSF_HAL_I2C3_MULTIPLEX_CNT 1
#   endif
__MULTIPLEXER_I2C_DEF(3)
#undef __MULTIPLEXER
#define __MULTIPLEXER                   __MULTIPLEXER3
VSF_MREPEAT(VSF_HAL_I2C3_MULTIPLEX_CNT, __VSF_MULTIPLEX_I2C_DEF, VSF_I2C3_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C4 == ENABLED && (I2C_PORT_MASK & (1 << 4))
#   ifndef VSF_HAL_I2C4_MULTIPLEX_CNT
#       define VSF_HAL_I2C4_MULTIPLEX_CNT 1
#   endif
__MULTIPLEXER_I2C_DEF(4)
#undef __MULTIPLEXER
#define __MULTIPLEXER                   __MULTIPLEXER4
VSF_MREPEAT(VSF_HAL_I2C4_MULTIPLEX_CNT, __VSF_MULTIPLEX_I2C_DEF, VSF_I2C4_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C5 == ENABLED && (I2C_PORT_MASK & (1 << 5))
#   ifndef VSF_HAL_I2C5_MULTIPLEX_CNT
#       define VSF_HAL_I2C5_MULTIPLEX_CNT 1
#   endif
__MULTIPLEXER_I2C_DEF(5)
#undef __MULTIPLEXER
#define __MULTIPLEXER                   __MULTIPLEXER5
VSF_MREPEAT(VSF_HAL_I2C5_MULTIPLEX_CNT, __VSF_MULTIPLEX_I2C_DEF, VSF_I2C5_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C6 == ENABLED && (I2C_PORT_MASK & (1 << 6))
#   ifndef VSF_HAL_I2C6_MULTIPLEX_CNT
#       define VSF_HAL_I2C6_MULTIPLEX_CNT 1
#   endif
__MULTIPLEXER_I2C_DEF(6)
#undef __MULTIPLEXER
#define __MULTIPLEXER                   __MULTIPLEXER6
VSF_MREPEAT(VSF_HAL_I2C6_MULTIPLEX_CNT, __VSF_MULTIPLEX_I2C_DEF, VSF_I2C6_MULTIPLEX)
#endif

#if I2C_MAX_PORT >= 0 && VSF_HAL_USE_I2C7 == ENABLED && (I2C_PORT_MASK & (1 << 7))
#   ifndef VSF_HAL_I2C7_MULTIPLEX_CNT
#       define VSF_HAL_I2C7_MULTIPLEX_CNT 1
#   endif
__MULTIPLEXER_I2C_DEF(7)
#undef __MULTIPLEXER
#define __MULTIPLEXER                   __MULTIPLEXER7
VSF_MREPEAT(VSF_HAL_I2C7_MULTIPLEX_CNT, __VSF_MULTIPLEX_I2C_DEF, VSF_I2C7_MULTIPLEX)
#endif

#endif //!VSF_HAL_I2C_IMP_MULTIPLEX_I2C
