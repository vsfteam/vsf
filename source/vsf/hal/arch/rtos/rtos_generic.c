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

#define __VSF_ARCH_RTOS_IMPLEMENT
#include "hal/arch/vsf_arch_abstraction.h"
#include "hal/arch/__vsf_arch_interface.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_ARCH_SWI_NUM > 0
typedef struct vsf_arch_swi_ctx_t {
    implement_ex(vsf_arch_irq_thread_with_stack_t, thread)
    implement(vsf_arch_irq_request_t)

    bool                            is_inited;
    vsf_arch_prio_t                 priority;

    vsf_irq_handler_t               *handler;
    void                            *param;
} vsf_arch_swi_ctx_t;
#endif

typedef struct vsf_rtos_t {
    struct {
        vsf_arch_irq_request_t      request;
    } wakeup;

    vsf_arch_prio_t                 prio_base;
#if VSF_ARCH_RTOS_CFG_MODE == VSF_ARCH_RTOS_MODE_REQUEST
    vsf_arch_irq_request_t          prio_change_req;
#endif

#if VSF_ARCH_SWI_NUM > 0
    vsf_arch_swi_ctx_t              swi[VSF_ARCH_SWI_NUM];
#endif
} vsf_rtos_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsf_rtos_t __vsf_rtos;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Infrastructure                                                             *
 *----------------------------------------------------------------------------*/

/*! \note initialize architecture specific service
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_arch_low_level_init(void)
{
    memset(&__vsf_rtos, 0, sizeof(__vsf_rtos));
    __vsf_rtos.prio_base = vsf_arch_prio_invalid;
    __vsf_arch_irq_request_init(&__vsf_rtos.wakeup.request, true);
#if VSF_ARCH_RTOS_CFG_MODE == VSF_ARCH_RTOS_MODE_REQUEST
    __vsf_arch_irq_request_init(&__vsf_rtos.prio_change_req, false);
#endif
    return __vsf_arch_model_low_level_init();
}

#if VSF_ARCH_SWI_NUM > 0
/*----------------------------------------------------------------------------*
 * SWI Implementation                                                         *
 *----------------------------------------------------------------------------*/

static void __vsf_arch_swi_thread(void *arg)
{
    vsf_arch_swi_ctx_t *ctx = container_of(arg, vsf_arch_swi_ctx_t, thread);

    while (1) {
        __vsf_arch_irq_request_pend(&ctx->use_as__vsf_arch_irq_request_t);
#if VSF_ARCH_RTOS_CFG_MODE == VSF_ARCH_RTOS_MODE_REQUEST
        while (ctx->priority <= __vsf_rtos.prio_base) {
            __vsf_arch_irq_request_pend(&__vsf_rtos.prio_change_req);
        }
#endif

        if (ctx->handler != NULL) {
            ctx->handler(ctx->param);
        }
    }
}

/*! \brief initialise a software interrupt
 *! \param idx the index of the software interrupt
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_arch_swi_init(uint_fast8_t idx, vsf_arch_prio_t priority, vsf_swi_handler_t *handler, void *param)
{
    if (idx < dimof(__vsf_rtos.swi)) {
        vsf_arch_swi_ctx_t *ctx = &__vsf_rtos.swi[idx];

        ctx->handler = handler;
        ctx->param = param;

        if (!ctx->is_inited) {
            char swi_name[11];

            ctx->is_inited = true;
            ctx->priority = priority;

            sprintf(swi_name, "vsf_swi%d", idx);
            __vsf_arch_irq_request_init(&ctx->use_as__vsf_arch_irq_request_t, true);
            __vsf_arch_irq_thread_start((vsf_arch_irq_thread_t *)&ctx->thread, swi_name,
                    __vsf_arch_swi_thread, priority, ctx->thread.stack, dimof(ctx->thread.stack));
#if VSF_ARCH_RTOS_CFG_MODE == VSF_ARCH_RTOS_MODE_SUSPEND_RESUME
            if (ctx->priority <= __vsf_rtos.prio_base) {
                __vsf_arch_irq_thread_suspend((vsf_arch_irq_thread_t *)&ctx->thread);
            }
#endif
        } else if (priority != ctx->priority) {
            ctx->priority = priority;
            __vsf_arch_irq_thread_set_priority((vsf_arch_irq_thread_t *)&ctx->thread, priority);
        }

        return VSF_ERR_NONE;
    }
    VSF_ARCH_ASSERT(false);
    return VSF_ERR_INVALID_PARAMETER;
}

/*! \brief trigger a software interrupt
 *! \param idx the index of the software interrupt
 */
void vsf_arch_swi_trigger(uint_fast8_t idx)
{
    if (idx < dimof(__vsf_rtos.swi)) {
        __vsf_arch_irq_request_send(&__vsf_rtos.swi[idx].use_as__vsf_arch_irq_request_t);
        return;
    }
    VSF_ARCH_ASSERT(false);
}

vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority)
{
    vsf_arch_prio_t orig = __vsf_rtos.prio_base;

    if (orig != priority) {
        __vsf_rtos.prio_base = priority;

#if VSF_ARCH_RTOS_CFG_MODE == VSF_ARCH_RTOS_MODE_SUSPEND_RESUME
        vsf_arch_swi_ctx_t *ctx = &__vsf_rtos.swi[0];
        // simply suspend thread with lower priority, resume thread with higher priority
        for (uint_fast8_t i = 0; i < dimof(__vsf_rtos.swi); i++, ctx++) {
            if (    ctx->is_inited
                // do not process current irq_thread, which has same priority as current thread
                && (    __vsf_arch_model_get_current_priority()
                    !=  __vsf_arch_irq_thread_get_priority((vsf_arch_irq_thread_t *)&ctx->thread))) {

                if (ctx->priority > priority) {
                    __vsf_arch_irq_thread_resume((vsf_arch_irq_thread_t *)&ctx->thread);
                } else {
                    __vsf_arch_irq_thread_suspend((vsf_arch_irq_thread_t *)&ctx->thread);
                }
            }
        }
#else
        __vsf_arch_irq_request_send(&__vsf_rtos.prio_change_req);
        __vsf_arch_irq_request_reset(&__vsf_rtos.prio_change_req);
#endif
    }
    return orig;
}
#endif

/*----------------------------------------------------------------------------*
 * Others: sleep, reset, etc                                                  *
 *----------------------------------------------------------------------------*/

void vsf_arch_sleep(uint32_t mode)
{
    UNUSED_PARAM(mode);
    vsf_enable_interrupt();
    __vsf_arch_irq_request_pend(&__vsf_rtos.wakeup.request);
    vsf_disable_interrupt();
}

void vsf_arch_wakeup(void)
{
    __vsf_arch_irq_request_send(&__vsf_rtos.wakeup.request);
}

/* EOF */
