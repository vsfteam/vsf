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
#include "./vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED
#define __VSF_EDA_CLASS_INHERIT__
#include "./vsf_kernel_common.h"
#include "./vsf_eda.h"
#include "./vsf_evtq.h"
#include "./vsf_os.h"
#include "./task/vsf_task.h"
#include "service/vsf_service.h"
// for vsf_arch_wakeup
#include "hal/arch/vsf_arch.h"
// for vsf_hal_init
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

#ifdef VSF_ARCH_ENTRY_NO_PENDING
#   ifndef VSF_ARCH_LIMIT_NO_SET_STACK
#       error VSF_ARCH_ENTRY_NO_PENDING depends on VSF_ARCH_LIMIT_NO_SET_STACK
#   endif
#   ifdef VSF_ARCH_IRQ_SUPPORT_STACK
#       error TODO: add configuration to set stack of main_thread
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_os_t {
#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
#   if defined(__VSF_OS_CFG_EVTQ_LIST)
    vsf_pool(vsf_evt_node_pool) node_pool;
#   endif
#endif
#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
    vsf_pool(vsf_eda_frame_pool) eda_frame_pool;
#endif
#if VSF_KERNEL_CFG_CPU_USAGE == ENABLED
    vsf_cpu_usage_t usage;
#endif
    const vsf_kernel_resource_t *res_ptr;

#ifdef VSF_ARCH_ENTRY_NO_PENDING
    vsf_arch_irq_thread_t main_thread;
#endif
} vsf_os_t;

/*============================ LOCAL VARIABLES ===============================*/

#if __VSF_OS_SWI_NUM > 0
static bool __vsf_os_is_inited = false;
#endif
static NO_INIT vsf_os_t __vsf_os;

/*============================ PROTOTYPES ====================================*/

extern void vsf_kernel_err_report(vsf_kernel_error_t err);
extern void vsf_plug_in_on_kernel_idle(void);
extern void vsf_plug_in_for_kernel_diagnosis(void);
extern void __post_vsf_kernel_init(void);

extern vsf_err_t vsf_kernel_start(void);

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
SECTION(".text.vsf.kernel.__vsf_set_cur_evtq")
extern vsf_evtq_t *__vsf_set_cur_evtq(vsf_evtq_t *new_ptr);
extern vsf_err_t vsf_evtq_poll(vsf_evtq_t *this_ptr);
#endif

extern const vsf_kernel_resource_t * vsf_kernel_get_resource_on_init(void);

#if __VSF_OS_SWI_NUM > 0
static vsf_protect_t __vsf_protect_region_sched_enter(void);
static void __vsf_protect_region_sched_leave(vsf_protect_t orig);
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if __VSF_OS_SWI_NUM > 0
const vsf_protect_region_t vsf_protect_region_sched = {
    .enter  = __vsf_protect_region_sched_enter,
    .leave  = __vsf_protect_region_sched_leave,
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
//implement_vsf_pool(vsf_eda_frame_pool, __vsf_eda_frame_t)
#define __name vsf_eda_frame_pool
#define __type __vsf_eda_frame_t
#include "service/pool/impl_vsf_pool.inc"


#ifndef WEAK_VSF_EDA_NEW_FRAME
SECTION(".text.vsf.kernel.vsf_eda_new_frame")
WEAK(vsf_eda_new_frame)
__vsf_eda_frame_t * vsf_eda_new_frame(size_t local_size)
{
    //! make sure local_size is aligned with sizeof(uintalu_t);
    local_size = (local_size + sizeof(uintalu_t) - 1) & ~ (sizeof(uintalu_t) - 1);
    /* todo: add smart pool support in the future */
#if 0
    __vsf_eda_frame_t *frame =
            VSF_POOL_ALLOC(vsf_eda_frame_pool, &__vsf_os.eda_frame_pool);
#else
    __vsf_eda_frame_t *frame =
            vsf_heap_malloc_aligned(sizeof(__vsf_eda_frame_t) + local_size + sizeof(uintalu_t),
                                    sizeof(uintalu_t));
#endif

    if (frame != NULL) {
        //! this is important, don't remove it.
        memset(frame, 0, sizeof(__vsf_eda_frame_t) + local_size + sizeof(uintalu_t));

        //! add watermark for local buffer overflow detection,
        //! please never remove this!!! as local size could be zero
        *(uintalu_t *)
            (   (uintptr_t)frame
            +   sizeof(__vsf_eda_frame_t)
            +   local_size) = 0xDEADBEEF;

        frame->state.local_size = local_size;
        //vsf_slist_init_node(__vsf_eda_frame_t, use_as__vsf_slist_node_t, frame);
    }
    return frame;
}
#endif

#ifndef WEAK_VSF_EDA_FREE_FRAME
SECTION(".text.vsf.kernel.vsf_eda_free_frame")
WEAK(vsf_eda_free_frame)
void vsf_eda_free_frame(__vsf_eda_frame_t *frame)
{
    /* todo: add smart pool support in the future */
#if 0
    VSF_POOL_FREE(vsf_eda_frame_pool, &__vsf_os.eda_frame_pool, frame);
#else
    vsf_heap_free(frame);
#endif
}
#endif
#endif      // __VSF_KERNEL_CFG_EDA_FRAME_POOL

#ifdef __VSF_OS_CFG_EVTQ_LIST
//implement_vsf_pool( vsf_evt_node_pool, vsf_evt_node_t)
#define __name vsf_evt_node_pool
#define __type vsf_evt_node_t
#include "service/pool/impl_vsf_pool.inc"
#endif

static void __vsf_kernel_os_init(void)
{
    memset(&__vsf_os, 0, sizeof(__vsf_os));

    __vsf_os.res_ptr = vsf_kernel_get_resource_on_init();
    VSF_KERNEL_ASSERT(NULL != __vsf_os.res_ptr);


#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
    do {
    #if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
        VSF_POOL_PREPARE(vsf_eda_frame_pool, &(__vsf_eda.eda_frame_pool))
            NULL,                                                               //!< target
            (vsf_protect_region_t *)&vsf_protect_region_sched,                  //!< region
        END_VSF_POOL_PREPARE(vsf_eda_frame_pool)
    #else
        VSF_POOL_PREPARE(vsf_eda_frame_pool, &(__vsf_os.eda_frame_pool),
            .target_ptr = (uintptr_t)NULL,
            .region_ptr = (vsf_protect_region_t *)&vsf_protect_region_sched,
        );
    #endif

        if (    (NULL == __vsf_os.res_ptr->frame_stack.frame_buf_ptr)
            ||  (0 == __vsf_os.res_ptr->frame_stack.frame_cnt)) {
            break;
        }

        VSF_POOL_ADD_BUFFER(
            vsf_eda_frame_pool,
            &(__vsf_os.eda_frame_pool),
            __vsf_os.res_ptr->frame_stack.frame_buf_ptr,
            sizeof(vsf_pool_item(vsf_eda_frame_pool)) * __vsf_os.res_ptr->frame_stack.frame_cnt
        );
    } while(0);
#endif

//#if __VSF_OS_SWI_NUM > 0
    {
    //! configure systimer priority (using higest+1 or highest)
    #if __VSF_OS_SWI_NUM > 0
        vsf_arch_prio_t priorit =
            __vsf_os.res_ptr->arch.os_swi_priorities_ptr[
                __vsf_os.res_ptr->arch.swi_priority_cnt - 1];
    #else
        vsf_arch_prio_t priorit = vsf_arch_prio_highest;
    #endif

        vsf_kernel_cfg_t cfg = {
            __vsf_os.res_ptr->arch.sched_prio.highest,                          //!< highest priority
            priorit,                                                            //!< systimer priority
        };
        vsf_kernel_init(&cfg);
    }
//#endif

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
#   ifdef __VSF_OS_CFG_EVTQ_LIST
    do {
    #if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
        VSF_POOL_PREPARE(vsf_evt_node_pool, (&__vsf_os.node_pool))
            (uintptr_t)&__vsf_os,                                               //! target
            (vsf_protect_region_t *)&vsf_protect_region_int
        END_VSF_POOL_PREPARE(vsf_evt_node_pool)
    #else
        VSF_POOL_PREPARE(vsf_evt_node_pool, (&__vsf_os.node_pool),
            .target_ptr = (uintptr_t)&__vsf_os,
            .region_ptr = (vsf_protect_region_t *)&vsf_protect_region_int,
        );
    #endif
        if  (   (NULL == __vsf_os.res_ptr->evt_queue.nodes_buf_ptr)
            ||  (0 == __vsf_os.res_ptr->evt_queue.node_cnt)) {
            break;
        }
        VSF_POOL_ADD_BUFFER(vsf_evt_node_pool,
                            (&__vsf_os.node_pool),
                            __vsf_os.res_ptr->evt_queue.nodes_buf_ptr,
                            __vsf_os.res_ptr->evt_queue.node_cnt
                                * sizeof(vsf_pool_item(vsf_evt_node_pool)));

    } while(0);
#   endif
#endif
}

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
static void __vsf_os_evtq_swi_handler(void *p)
{
    vsf_evtq_t *pcur, *pold;

    VSF_KERNEL_ASSERT(p != NULL);

    pcur = (vsf_evtq_t *)p;

#if VSF_KERNEL_CFG_TRACE == ENABLED
    if (!pcur->is_isr_info_sent) {
        pcur->is_isr_info_sent = true;

        char name[3 + 4 + 1] = "swi";
        int swi_idx = pcur - __vsf_os.res_ptr->evt_queue.queue_array;
        VSF_KERNEL_ASSERT(swi_idx < __vsf_os.res_ptr->evt_queue.queue_cnt);
        itoa(swi_idx, &name[3], 10);
        vsf_kernel_trace_isr_info(vsf_get_interrupt_id(), name);
    }
    vsf_kernel_trace_isr_enter(vsf_get_interrupt_id());
#endif

    pold = __vsf_set_cur_evtq(pcur);

#if VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
    // fixme: NO NEED to protect, because pold->cur.eda will not preempt current context
    vsf_systimer_tick_t start_tick = 0;
    if (pold != NULL) {
        if (pold->cur.is_timing) {
            start_tick = vsf_systimer_get_tick();
        }
    }
#endif

    vsf_evtq_poll(pcur);

#if VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
    if (pold != NULL) {
        if (pold->cur.is_timing) {
            pold->cur.preempted_ticks += vsf_systimer_get_elapsed(start_tick);
        }
    }
#endif

    __vsf_set_cur_evtq(pold);

#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_isr_leave(vsf_get_interrupt_id());
#endif
}

vsf_evtq_t *__vsf_os_evtq_get(vsf_prio_t priority)
{
    if ((priority >= 0) && (priority < __vsf_os.res_ptr->evt_queue.queue_cnt)) {
        return &__vsf_os.res_ptr->evt_queue.queue_array[priority];
    }
    return NULL;
}

vsf_prio_t __vsf_os_evtq_get_priority(vsf_evtq_t *this_ptr)
{
    uint_fast8_t index = this_ptr - __vsf_os.res_ptr->evt_queue.queue_array;
    VSF_KERNEL_ASSERT(      (this_ptr != NULL)
                        &&  (index < __vsf_os.res_ptr->evt_queue.queue_cnt));

    return (vsf_prio_t)index;
}

vsf_err_t __vsf_os_evtq_set_priority(vsf_evtq_t *this_ptr, vsf_prio_t priority)
{
#if defined(__VSF_OS_SWI_PRIORITY_BEGIN)
    uint_fast8_t index = this_ptr - __vsf_os.res_ptr->evt_queue.queue_array;
    VSF_KERNEL_ASSERT(      (this_ptr != NULL)
                        &&  (index < __vsf_os.res_ptr->evt_queue.queue_cnt));
#endif

#if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
    if (vsf_prio_0 == priority) {
        vsf_kernel_err_report(VSF_KERNEL_ERR_INVALID_USAGE);
    }
#endif

#if defined(__VSF_OS_SWI_PRIORITY_BEGIN)
    if (priority >= __vsf_os.res_ptr->arch.sched_prio.begin) {
        priority -= __vsf_os.res_ptr->arch.sched_prio.begin;
        index -= __vsf_os.res_ptr->arch.sched_prio.begin;
        return vsf_swi_init(
                index,
                __vsf_os.res_ptr->arch.os_swi_priorities_ptr[priority],
                &__vsf_os_evtq_swi_handler, this_ptr);
    }
#endif

    return VSF_ERR_FAIL;
}

#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-compare"
#endif

vsf_err_t __vsf_os_evtq_init(vsf_evtq_t *this_ptr)
{
#if defined(__VSF_OS_SWI_PRIORITY_BEGIN)
    uint_fast8_t index = this_ptr - __vsf_os.res_ptr->evt_queue.queue_array;
    VSF_KERNEL_ASSERT(      (this_ptr != NULL)
                        &&  (index < __vsf_os.res_ptr->evt_queue.queue_cnt));

    if (index >= __vsf_os.res_ptr->arch.sched_prio.begin) {
        __vsf_os_evtq_set_priority(this_ptr, (vsf_prio_t)index);
    }
#endif

    return VSF_ERR_NONE;
}

vsf_err_t __vsf_os_evtq_activate(vsf_evtq_t *this_ptr)
{
#if defined(__VSF_OS_SWI_PRIORITY_BEGIN)
    uint_fast8_t index = this_ptr - __vsf_os.res_ptr->evt_queue.queue_array;
    VSF_KERNEL_ASSERT(      (this_ptr != NULL)
                        &&  (index < __vsf_os.res_ptr->evt_queue.queue_cnt));

    if (index >= __vsf_os.res_ptr->arch.sched_prio.begin) {
        index -= __vsf_os.res_ptr->arch.sched_prio.begin;
        vsf_swi_trigger(index);
    }
#endif

    vsf_arch_wakeup();
    return VSF_ERR_NONE;
}

#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#pragma clang diagnostic pop
#endif


#ifdef __VSF_OS_CFG_EVTQ_LIST
vsf_evt_node_t *__vsf_os_alloc_evt_node(void)
{
    return VSF_POOL_ALLOC(vsf_evt_node_pool, &__vsf_os.node_pool);
}

void __vsf_os_free_evt_node(vsf_evt_node_t *pnode)
{
    VSF_POOL_FREE(vsf_evt_node_pool, &__vsf_os.node_pool, pnode);
}
#endif
#endif

#if __VSF_OS_SWI_NUM > 0
vsf_sched_lock_status_t vsf_forced_sched_lock(void)
{
    if (__vsf_os_is_inited) {
        return vsf_set_base_priority(
            __vsf_os.res_ptr->arch.os_swi_priorities_ptr[
                __vsf_os.res_ptr->arch.swi_priority_cnt - 1]);
    }
    return vsf_arch_prio_0;
}

void vsf_forced_sched_unlock(vsf_sched_lock_status_t origlevel)
{
    if (__vsf_os_is_inited) {
        vsf_set_base_priority(origlevel);
    }
}

static void __vsf_code_region_forced_sched_on_enter(void *pobj, void *plocal)
{
    vsf_sched_lock_status_t *pstate = (vsf_sched_lock_status_t *)plocal;
    VSF_UNUSED_PARAM(pobj);
    VSF_UNUSED_PARAM(plocal);

    VSF_KERNEL_ASSERT(NULL != plocal);
    (*pstate) = vsf_sched_lock();
}

static void __vsf_code_region_forced_sched_on_leave(void *pobj,void *plocal)
{
    vsf_sched_lock_status_t *pstate = (vsf_sched_lock_status_t *)plocal;

    VSF_UNUSED_PARAM(pobj);
    VSF_UNUSED_PARAM(plocal);

    VSF_KERNEL_ASSERT(NULL != plocal);
    vsf_sched_unlock(*pstate);
}

static vsf_protect_t __vsf_protect_region_sched_enter(void)
{
    return vsf_protect_sched();
}

static void __vsf_protect_region_sched_leave(vsf_protect_t orig)
{
    vsf_unprotect_sched(orig);
}

static const i_code_region_t __vsf_i_code_region_forced_sched_safe = {
    .local_obj_size =   sizeof(vsf_sched_lock_status_t),
    .OnEnter =          &__vsf_code_region_forced_sched_on_enter,
    .OnLeave =          &__vsf_code_region_forced_sched_on_leave,
};

const code_region_t VSF_FORCED_SCHED_SAFE_CODE_REGION = {
    .methods_ptr = (i_code_region_t *)&__vsf_i_code_region_forced_sched_safe,
};
#endif

// vsf_sleep can only be called in vsf_plug_in_on_kernel_idle
void vsf_sleep(void)
{
#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_idle();
#endif

#if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED && __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    vsf_disable_interrupt();
    if (vsf_evtq_is_empty(&__vsf_os.res_ptr->evt_queue.queue_array[0])) {
#elif VSF_KERNEL_CFG_CPU_USAGE == ENABLED
        vsf_disable_interrupt();
#endif

#if VSF_KERNEL_CFG_CPU_USAGE == ENABLED
        vsf_systimer_tick_t start_tick = vsf_systimer_get_tick();
        bool is_timing = false;
        if (__vsf_os.usage.ctx != NULL) {
            is_timing = true;
        }
#endif

        // vsf_arch_sleep will not enable interrupt
        vsf_arch_sleep(0);

#if VSF_KERNEL_CFG_CPU_USAGE == ENABLED
        start_tick = vsf_systimer_get_elapsed(start_tick);
        __vsf_os.usage.ticks += start_tick;
        if ((__vsf_os.usage.ctx != NULL) && is_timing) {
            __vsf_os.usage.ctx->ticks += start_tick;
        }
#endif

#if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED && __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    }
    vsf_enable_interrupt();
#elif VSF_KERNEL_CFG_CPU_USAGE == ENABLED
        vsf_enable_interrupt();
#endif
}

#if VSF_KERNEL_CFG_CPU_USAGE == ENABLED
void vsf_cpu_usage_start(vsf_cpu_usage_ctx_t *ctx)
{
    vsf_protect_t origlevel = vsf_protect_int();
        ctx->ticks = 0;
        ctx->duration = vsf_systimer_get_tick();

        VSF_KERNEL_ASSERT(NULL == __vsf_os.usage.ctx);
        __vsf_os.usage.ctx = ctx;
    vsf_unprotect_int(origlevel);
}
void vsf_cpu_usage_stop(void)
{
    vsf_protect_t origlevel = vsf_protect_int();
        vsf_cpu_usage_ctx_t  *ctx = __vsf_os.usage.ctx;
        VSF_KERNEL_ASSERT(NULL != ctx);
        __vsf_os.usage.ctx = NULL;

        ctx->duration = vsf_systimer_get_elapsed(ctx->duration);
        ctx->ticks = ctx->duration - ctx->ticks;
    vsf_unprotect_int(origlevel);
}
#endif

#ifndef WEAK_VSF_PLUG_IN_ON_KERNEL_IDLE
WEAK(vsf_plug_in_on_kernel_idle)
void vsf_plug_in_on_kernel_idle(void)
{
    vsf_sleep();
}
#endif

#ifndef WEAK_VSF_PLUG_IN_FOR_KERNEL_DIAGNOSIS
WEAK(vsf_plug_in_for_kernel_diagnosis)
void vsf_plug_in_for_kernel_diagnosis(void)
{
    //! doing nothing here
}
#endif

#ifndef WEAK___POST_VSF_KERNEL_INIT
WEAK(__post_vsf_kernel_init)
void __post_vsf_kernel_init(void)
{
}
#endif

void __vsf_kernel_os_run_priority(vsf_prio_t priority)
{
#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    __vsf_os_evtq_swi_handler(&__vsf_os.res_ptr->evt_queue.queue_array[priority]);
#endif
}

#ifdef __VSF_WORKAROUND_IAR_CPP__
// if __VSF_WORKAROUND_IAR_CPP__ is defined,
//  vsf_heap_add will be called before __vsf_os_is_inited is initlailized,
//  and vsf_heap_add will call scheduler_protect, which need __vsf_os_is_inited
void __vsf_kernel_os_raw_init(void)
{
    __vsf_os_is_inited = false;
}
#endif

void __vsf_kernel_os_start(void)
{
    // arch should be initialized here because service can depend on arch
    vsf_arch_init();

    /*
     *    Some code will require scheduler protect, which is not available
     *  before __vsf_os_is_inited is being set to true. So disable interrupt
     *  here, and enable after __vsf_os_is_inited is set.
     */

    // vsf_disable_interrupt depend on arch, so it's called after vsf_arch_init
    vsf_disable_interrupt();

    vsf_service_init();
    // hal, actually driver in hal, can depend on service, so it's called after vsf_service_init
    vsf_hal_init();

    __vsf_kernel_os_init();
    // resources for scheduler protect is ready, set __vsf_os_is_inited and enable interrupt
#if __VSF_OS_SWI_NUM > 0
    __vsf_os_is_inited = true;
#endif
    vsf_enable_interrupt();

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    {
        vsf_evtq_t *pevtq = &__vsf_os.res_ptr->evt_queue.queue_array[0];
#   ifdef __VSF_OS_CFG_EVTQ_ARRAY
        uint_fast16_t node_size = (__vsf_os.res_ptr->evt_queue.node_bit_sz);
#   endif
        uint_fast16_t i;

        for (i = 0;
            i < __vsf_os.res_ptr->evt_queue.queue_cnt;
            i++, pevtq++) {
#   ifdef __VSF_OS_CFG_EVTQ_ARRAY
            uint_fast16_t temp = 1 << node_size;
            vsf_evt_node_t *node =
                (vsf_evt_node_t *)__vsf_os.res_ptr->evt_queue.nodes + i * temp;
            memset( node,0, sizeof(vsf_evt_node_t) * temp );
            pevtq->node = node;
            pevtq->bitsize = node_size;
#   endif
            vsf_evtq_init(pevtq);
#   if VSF_KERNEL_CFG_TRACE == ENABLED
            pevtq->is_isr_info_sent = true;
#   endif
        }
        __vsf_set_cur_evtq(NULL);
    }
#endif

    vsf_kernel_start();

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    {
        vsf_evtq_t *pevtq = &__vsf_os.res_ptr->evt_queue.queue_array[0];
        uint_fast16_t i;

        for (i = 0;
            i < __vsf_os.res_ptr->evt_queue.queue_cnt;
            i++, pevtq++) {
#   if VSF_KERNEL_CFG_TRACE == ENABLED
            pevtq->is_isr_info_sent = false;
#   endif
        }
    }
#endif

    vsf_osa_hal_init();
    __post_vsf_kernel_init();
}

#ifdef VSF_ARCH_ENTRY_NO_PENDING
// dummy main
int main(void)
{
    return 0;
}

static void __vsf_main_thread(void *arg)
{
    while (1) {
#   if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
        __vsf_kernel_os_run_priority(vsf_prio_0);
#   endif
        vsf_plug_in_for_kernel_diagnosis(); //!< customised kernel diagnosis
        vsf_plug_in_on_kernel_idle();       //!< user defined idle task
    }
}
#endif

void __vsf_main_entry(void)
{
    __vsf_kernel_os_start();

#ifdef VSF_ARCH_ENTRY_NO_PENDING
    __vsf_kernel_host_thread_init(&__vsf_os.main_thread, "main", __vsf_main_thread, vsf_arch_prio_0, NULL, 0);
#else
    while (1) {
#   if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
        __vsf_kernel_os_run_priority(vsf_prio_0);
#   endif
        vsf_plug_in_for_kernel_diagnosis(); //!< customised kernel diagnosis
        vsf_plug_in_on_kernel_idle();       //!< user defined idle task
    }
#endif
}


#endif


