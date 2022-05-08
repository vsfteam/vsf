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
#include "../vsf_kernel_cfg.h"

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED && VSF_USE_KERNEL == ENABLED

#define __VSF_THREAD_CLASS_IMPLEMENT
#define __VSF_EDA_CLASS_INHERIT__
#include "../vsf_kernel_common.h"
#include "./vsf_thread.h"
#include "../vsf_os.h"

/*============================ MACROS ========================================*/

#ifdef VSF_ARCH_SETJMP
#   undef setjmp
#   define setjmp               VSF_ARCH_SETJMP
#endif
#ifdef VSF_ARCH_LONGJMP
#   undef longjmp
#   define longjmp              VSF_ARCH_LONGJMP
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

SECTION(".text.vsf.kernel.vsf_thread_get_cur")
vsf_thread_t *vsf_thread_get_cur(void)
{
    vsf_thread_t *thread_obj = (vsf_thread_t *)vsf_eda_get_cur();
#   if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
    VSF_KERNEL_ASSERT(thread_obj->flag.feature.is_stack_owner);
#   endif
    return thread_obj;
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

SECTION(".text.vsf.kernel.vsf_thread_exit")
void vsf_thread_exit(void)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    VSF_KERNEL_ASSERT(NULL != thread_obj->fn.frame);
    vsf_thread_cb_t *pthis = (vsf_thread_cb_t *)thread_obj->fn.frame->ptr.param;
#else
    vsf_thread_t *pthis = thread_obj;
#endif

    VSF_KERNEL_ASSERT(NULL != pthis);
#if VSF_KERNEL_CFG_THREAD_STACK_CHECK == ENABLED
    vsf_thread_stack_check();
#endif
    vsf_eda_return();
#ifdef VSF_ARCH_LIMIT_NO_SET_STACK
    __vsf_kernel_host_request_send(pthis->rep);
    __vsf_kernel_host_thread_exit(&pthis->host_thread);
#else
    longjmp(*(pthis->ret), -1);
#endif
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if VSF_KERNEL_CFG_EDA_FAST_SUB_CALL == ENABLED
void __vsf_eda_return_to_thread(vsf_eda_t *eda)
{
    VSF_KERNEL_ASSERT(NULL != eda->fn.frame);
    vsf_thread_cb_t *cb_obj = (vsf_thread_cb_t *)eda->fn.frame->ptr.param;
    VSF_KERNEL_ASSERT(NULL != cb_obj);

    if (cb_obj->ret != NULL) {
        longjmp(*(cb_obj->pos), VSF_EVT_RETURN);
    } else {
        __vsf_dispatch_evt(eda, VSF_EVT_RETURN);
    }
}
#endif

SECTION(".text.vsf.kernel.vsf_thread_wait")
static vsf_evt_t __vsf_thread_wait(vsf_thread_cb_t *pthis)
{
    vsf_evt_t curevt;

    VSF_KERNEL_ASSERT(pthis != NULL);

#   ifdef VSF_ARCH_LIMIT_NO_SET_STACK
    __vsf_kernel_host_request_send(pthis->rep);
    __vsf_kernel_host_request_pend(&pthis->req);
    curevt = pthis->evt;
#   else
    jmp_buf pos;

#       if VSF_KERNEL_CFG_THREAD_STACK_CHECK == ENABLED
    vsf_thread_stack_check();
#       endif

    pthis->pos = &pos;
    curevt = setjmp(*(pthis->pos));

    if (!curevt) {
        longjmp(*(pthis->ret), -1);
    }
    pthis->pos = NULL;
#   endif
    return curevt;
}
#endif

SECTION(".text.vsf.kernel.vsf_thread_wait")
vsf_evt_t vsf_thread_wait(void)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();
    vsf_evt_t curevt;

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    VSF_KERNEL_ASSERT(NULL != thread_obj->fn.frame);
    curevt = __vsf_thread_wait((vsf_thread_cb_t *)thread_obj->fn.frame->ptr.param);
#else
#    ifdef VSF_ARCH_LIMIT_NO_SET_STACK
    __vsf_kernel_host_request_send(thread_obj->rep);
    __vsf_kernel_host_request_pend(&thread_obj->req);
    curevt = thread_obj->evt;
#    else
    jmp_buf pos;

#       if VSF_KERNEL_CFG_THREAD_STACK_CHECK == ENABLED
    vsf_thread_stack_check();
#       endif

    thread_obj->pos = &pos;
    curevt = setjmp(*thread_obj->pos);
    if (!curevt) {
        longjmp(*(thread_obj->ret), -1);
    }
    thread_obj->pos = NULL;
#    endif
#endif

    return curevt;
}

SECTION(".text.vsf.kernel.vsf_thread_wait_for_evt")
void vsf_thread_wait_for_evt(vsf_evt_t evt)
{
    vsf_evt_t rcv_evt = vsf_thread_wait();
    VSF_UNUSED_PARAM(rcv_evt);
    /*! \note make sure there is no message ignored */
    VSF_KERNEL_ASSERT( rcv_evt == evt);
    //while(evt != vsf_thread_wait());
}

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
SECTION(".text.vsf.kernel.vsf_thread_wait_for_evt_msg")
uintptr_t vsf_thread_wait_for_evt_msg(vsf_evt_t evt)
{
    vsf_thread_wfe(evt);
    return (uintptr_t)vsf_eda_get_cur_msg();
}

SECTION(".text.vsf.kernel.vsf_thread_wait_for_evt_msg")
uintptr_t vsf_thread_wait_for_msg(void)
{
    return vsf_thread_wait_for_evt_msg(VSF_EVT_MESSAGE);
}
#else
SECTION(".text.vsf.kernel.vsf_thread_wait_for_evt_msg")
uintptr_t vsf_thread_wait_for_msg(void)
{
    vsf_thread_wait();
    return (uintptr_t)vsf_eda_get_cur_msg();
}
#endif


SECTION(".text.vsf.kernel.vsf_thread_sendevt")
void vsf_thread_sendevt(vsf_thread_t *thread_obj, vsf_evt_t evt)
{
    vsf_eda_post_evt((vsf_eda_t *)thread_obj, evt);
}

SECTION(".text.vsf.kernel.vsf_thread")
static void __vsf_thread_entry(void)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    VSF_KERNEL_ASSERT(NULL != thread_obj->fn.frame);
    vsf_thread_cb_t *pthis = (vsf_thread_cb_t *)thread_obj->fn.frame->ptr.param;
    VSF_KERNEL_ASSERT(NULL != pthis);

    pthis->entry((vsf_thread_cb_t *)thread_obj->fn.frame->ptr.param);
#else
    vsf_thread_t *pthis = thread_obj;
    pthis->entry(thread_obj);
#endif

#if VSF_KERNEL_CFG_THREAD_STACK_CHECK == ENABLED
    vsf_thread_stack_check();
#endif
    vsf_eda_return();
#ifdef VSF_ARCH_LIMIT_NO_SET_STACK
    __vsf_kernel_host_request_send(pthis->rep);
#else
    longjmp(*(pthis->ret), -1);
#endif
}

#ifdef VSF_ARCH_LIMIT_NO_SET_STACK
SECTION(".text.vsf.kernel.vsf_thread")
void __vsf_thread_host_thread(void *arg)
{
#   if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    class(vsf_thread_cb_t) *thread = container_of(arg, class(vsf_thread_cb_t), host_thread);
#   else
    class(vsf_thread_t) *thread = container_of(arg, class(vsf_thread_t), host_thread);
#   endif

    VSF_KERNEL_ASSERT(VSF_EVT_INIT == thread->evt);
    __vsf_thread_entry();
    __vsf_kernel_host_thread_exit(&thread->host_thread);
}
#endif

#if VSF_KERNEL_CFG_THREAD_STACK_CHECK == ENABLED
SECTION(".text.vsf.kernel.__vsf_thread_stack_check")
static void __vsf_thread_stack_check(uintptr_t stack)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();

#   if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    VSF_KERNEL_ASSERT(NULL != thread_obj->fn.frame);
    vsf_thread_cb_t *pthis = (vsf_thread_cb_t *)thread_obj->fn.frame->ptr.param;
    VSF_KERNEL_ASSERT(NULL != pthis);
#   else
    vsf_thread_t *pthis = thread_obj;
    VSF_KERNEL_ASSERT(NULL != pthis);
#   endif

    VSF_KERNEL_ASSERT(  (stack <= (uintptr_t)&pthis->stack[(pthis->stack_size >> 3)])
                    &&  (stack >= (uintptr_t)&pthis->stack[0]));
}

#   ifndef VSF_ARCH_LIMIT_NO_SET_STACK
SECTION(".text.vsf.kernel.vsf_thread_stack_check")
void vsf_thread_stack_check(void)
{
    uintptr_t stack = vsf_arch_get_stack();
    __vsf_thread_stack_check(stack);
}
#   else
SECTION(".text.vsf.kernel.vsf_thread_stack_check")
void vsf_thread_stack_check(void)
{
}
#   endif
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
SECTION(".text.vsf.kernel.vsf_thread")
//static void __vsf_thread_evthandler(vsf_thread_cb_t *target, vsf_evt_t evt)
static void __vsf_thread_evthandler(uintptr_t local, vsf_evt_t evt)
{
    VSF_KERNEL_ASSERT(local != (uintptr_t)NULL);
    vsf_thread_cb_t *pthis = (vsf_thread_cb_t *)(*(void **)((uintptr_t)local - sizeof(uintptr_t)));
    VSF_KERNEL_ASSERT(pthis != NULL);

#   ifdef VSF_ARCH_LIMIT_NO_SET_STACK
    vsf_arch_irq_request_t rep = { 0 };

    pthis->rep = &rep;
    __vsf_kernel_host_request_init(&rep);
    pthis->evt = evt;
    switch (evt) {
    case VSF_EVT_INIT:
        if (!pthis->is_inited) {
            pthis->is_inited = true;
            __vsf_kernel_host_request_init(&pthis->req);
            __vsf_kernel_host_thread_init(&pthis->host_thread, "vsf_thread", __vsf_thread_host_thread,
                -1, (VSF_ARCH_RTOS_STACK_T *)pthis->stack, pthis->stack_size / sizeof(VSF_ARCH_RTOS_STACK_T));
        } else {
            __vsf_kernel_host_thread_restart(&pthis->host_thread, &pthis->req);
        }
        // no need to send request because target thread is not pending for request after startup
        goto pend;
    default:
        __vsf_kernel_host_request_send(&pthis->req);
    pend:
        __vsf_kernel_host_request_pend(pthis->rep);
        break;
    }
    __vsf_kernel_host_request_fini(&rep);
    pthis->rep = NULL;
#   else
    jmp_buf ret;

    pthis->ret = &ret;
    if (!setjmp(ret)) {
        if (VSF_EVT_INIT == evt) {
            vsf_arch_set_stack((uintptr_t)(&pthis->stack[(pthis->stack_size >> 3)]));
            __vsf_thread_entry();
        } else {
#if VSF_KERNEL_CFG_THREAD_STACK_CHECK == ENABLED
            __vsf_thread_stack_check(VSF_KERNEL_GET_STACK_FROM_JMPBUF(pthis->pos));
#endif
            longjmp(*(pthis->pos), evt);
        }
    }
    pthis->ret = NULL;
#   endif
}
#else

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
#elif __IS_COMPILER_GCC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#endif

SECTION(".text.vsf.kernel.vsf_thread")
static void __vsf_thread_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_thread_t *pthis = (vsf_thread_t *)eda;
    VSF_KERNEL_ASSERT(pthis != NULL);

#   ifdef VSF_ARCH_LIMIT_NO_SET_STACK
    vsf_arch_irq_request_t rep = { 0 };

    pthis->rep = &rep;
    __vsf_kernel_host_request_init(&rep);
    pthis->evt = evt;
    switch (evt) {
    case VSF_EVT_INIT:
        if (!pthis->is_inited) {
            pthis->is_inited = true;
            __vsf_kernel_host_request_init(&pthis->req);
            __vsf_kernel_host_thread_init(&pthis->host_thread, "host_thread", __vsf_thread_host_thread,
                -1, (VSF_ARCH_RTOS_STACK_T *)pthis->stack, pthis->stack_size / sizeof(VSF_ARCH_RTOS_STACK_T));
        } else {
            __vsf_kernel_host_thread_restart(&pthis->host_thread, &pthis->req);
        }
        // no need to send request because target thread is not pending for request after startup
        goto pend;
    default:
        __vsf_kernel_host_request_send(&pthis->req);
    pend:
        __vsf_kernel_host_request_pend(pthis->rep);
        break;
    }
    __vsf_kernel_host_request_fini(&rep);
    pthis->rep = NULL;
#   else
    jmp_buf ret;

    pthis->ret = &ret;
    if (!setjmp(ret)) {
        if (VSF_EVT_INIT == evt) {
            vsf_arch_set_stack((uintptr_t)(&pthis->stack[(pthis->stack_size >> 3)]));
            __vsf_thread_entry();
        } else {
#if VSF_KERNEL_CFG_THREAD_STACK_CHECK == ENABLED
            __vsf_thread_stack_check(VSF_KERNEL_GET_STACK_FROM_JMPBUF(pthis->pos));
#endif
            longjmp(*pthis->pos, evt);
        }
    }
    pthis->ret = NULL;
#   endif
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#pragma GCC diagnostic pop
#endif

#endif

#if __IS_COMPILER_IAR__
//! bit mask appears to contain significant bits that do not affect the result
#   pragma diag_suppress=pa182,pe111
#endif

SECTION(".text.vsf.kernel.vsf_thread")
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
vsf_err_t vsf_thread_start( vsf_thread_t *pthis,
                            vsf_thread_cb_t *thread_cb,
                            vsf_prio_t priority)
{
    VSF_KERNEL_ASSERT(pthis != NULL && NULL != thread_cb);
    VSF_KERNEL_ASSERT(      (0 != thread_cb->stack_size)
                        &&  (NULL != thread_cb->stack)
                        &&  (NULL != thread_cb->entry));

    vsf_eda_cfg_t cfg = {
        .fn.param_evthandler    = (vsf_param_eda_evthandler_t)__vsf_thread_evthandler,
        .priority               = priority,
        .target                 = (uintptr_t)thread_cb,
        .feature                = {
            .is_stack_owner     = true,
            .is_use_frame       = true,
        },
#   if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
        .on_terminate           = pthis->on_terminate,
#   endif
    };

    if (thread_cb->stack_size < (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE + VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE)) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
    } else if (0 != (thread_cb->stack_size & (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE - 1))) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_PROVIDED_RESOURCE_NOT_ALIGNED;
    }

#ifdef VSF_ARCH_LIMIT_NO_SET_STACK
    thread_cb->is_inited = false;
#endif

    vsf_err_t err;
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    err = vsf_teda_start(&pthis->use_as__vsf_teda_t, &cfg);
#   else
    err = vsf_eda_start(&pthis->use_as__vsf_eda_t, &cfg);
#   endif

#   if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info(&pthis->use_as__vsf_eda_t, NULL, thread_cb->stack, thread_cb->stack_size);
#   endif

    return err;
}

#if __IS_COMPILER_IAR__
//! bit mask appears to contain significant bits that do not affect the result
#   pragma diag_warning=pa182,pe111
#endif

SECTION(".text.vsf.kernel.vk_eda_call_thread_prepare")
vsf_err_t vk_eda_call_thread_prepare(vsf_thread_cb_t *pthis,
                                        vsf_thread_prepare_cfg_t *cfg)
{
    VSF_KERNEL_ASSERT(NULL != cfg && NULL != pthis);
    VSF_KERNEL_ASSERT(    (NULL != cfg->entry)
                    &&    (NULL != cfg->stack)
                    &&    (cfg->stack_size >= 8));

    pthis->entry = cfg->entry;
    pthis->stack = cfg->stack;
    pthis->stack_size = cfg->stack_size;

    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vk_eda_call_thread")
vsf_err_t vk_eda_call_thread(vsf_thread_cb_t *thread_cb)
{
    VSF_KERNEL_ASSERT(NULL != thread_cb);
    vsf_err_t ret;
    __vsf_eda_frame_state_t state   = {
        .feature                    = {
            .is_stack_owner         = true,
        }
    };
    ret = __vsf_eda_call_eda_ex(   (uintptr_t)__vsf_thread_evthandler,
                                    (uintptr_t)thread_cb,
                                    state, true);

    return ret;
}


SECTION(".text.vsf.kernel.__vsf_thread_call_eda_ex")
static vsf_err_t __vsf_thread_call_eda_ex(  uintptr_t eda_handler,
                                            uintptr_t param,
                                            __vsf_eda_frame_state_t state,
                                            size_t local_buff_size,
                                            uintptr_t local_buff)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();

    VSF_KERNEL_ASSERT(NULL != thread_obj->fn.frame);
    vsf_thread_cb_t *cb_obj = (vsf_thread_cb_t *)thread_obj->fn.frame->ptr.param;
    VSF_KERNEL_ASSERT(NULL != cb_obj);

    vsf_err_t err;

#ifdef VSF_ARCH_LIMIT_NO_SET_STACK
    err = __vsf_eda_call_eda_ex_prepare(eda_handler, param, state, true);
    VSF_KERNEL_ASSERT(VSF_ERR_NONE == err);
    if ((uintptr_t)NULL != local_buff) {
        //! initialise local
        size_t size = vsf_min(state.local_size, local_buff_size);
        if (size > 0) {
            uintptr_t local = vsf_eda_get_local();
            memcpy((void *)local, (void *)local_buff, size);
        }
    }

#   if VSF_KERNEL_CFG_EDA_FAST_SUB_CALL == ENABLED
    extern void __vsf_dispatch_evt(vsf_eda_t *this_ptr, vsf_evt_t evt);
    __vsf_dispatch_evt((vsf_eda_t *)thread_obj, VSF_EVT_INIT);
#   else
    if (VSF_ERR_NONE != vsf_eda_post_evt((vsf_eda_t *)thread_obj, VSF_EVT_INIT)) {
        VSF_KERNEL_ASSERT(false);
    }
#   endif
    vsf_evt_t evt = __vsf_thread_wait(cb_obj);
    VSF_KERNEL_ASSERT(evt == VSF_EVT_RETURN);

#else
    jmp_buf pos;
    cb_obj->pos = &pos;
    if (!setjmp(*(cb_obj->pos))) {
        err = __vsf_eda_call_eda_ex_prepare(eda_handler, param, state, true);
        VSF_KERNEL_ASSERT(VSF_ERR_NONE == err);
        if ((uintptr_t)NULL != local_buff) {
            //! initialise local
            size_t size = vsf_min(state.local_size, local_buff_size);
            if (size > 0) {
                uintptr_t local = vsf_eda_get_local();
                memcpy((void *)local, (void *)local_buff, size);
            }
        }

#   if VSF_KERNEL_CFG_EDA_FAST_SUB_CALL == ENABLED
        extern void __vsf_dispatch_evt(vsf_eda_t *this_ptr, vsf_evt_t evt);
        __vsf_dispatch_evt((vsf_eda_t *)thread_obj, VSF_EVT_INIT);
#   else
        if (VSF_ERR_NONE != vsf_eda_post_evt((vsf_eda_t *)thread_obj, VSF_EVT_INIT)) {
            VSF_KERNEL_ASSERT(false);
        }
#   endif
#   if VSF_KERNEL_CFG_THREAD_STACK_CHECK == ENABLED
        // can not check stack here, because __vsf_eda_call_eda_ex_prepare switch
        //  current frame to eda context instead of thread context
//        vsf_thread_stack_check();
#   endif
        longjmp(*(cb_obj->ret), -1);
    }
    cb_obj->pos = NULL;
#endif
    // do not return err;
    //  because on some platform in high optimization, err will be wrong here
    //  and acutally, there is no error if run here
    return VSF_ERR_NONE;
}


SECTION(".text.vsf.kernel.vk_thread_call_eda")
vsf_err_t vk_thread_call_eda(   uintptr_t eda_handler,
                                uintptr_t param,
                                size_t local_size,
                                size_t local_buff_size,
                                uintptr_t local_buff)
{
    __vsf_eda_frame_state_t state   = {
        .local_size                 = local_size,
    };
    return __vsf_thread_call_eda_ex(eda_handler, param, state, local_buff_size, local_buff);
}

SECTION(".text.vsf.kernel.vsf_thread_call_thread")
vsf_err_t vk_thread_call_thread(vsf_thread_cb_t *pthis,
                                vsf_thread_prepare_cfg_t *cfg)
{
    VSF_KERNEL_ASSERT(NULL != cfg && NULL != pthis);

    __vsf_eda_frame_state_t state   = {
        .feature                    = {
            .is_stack_owner         = true,
        },
    };
    VSF_KERNEL_ASSERT(    (NULL != cfg->entry)
                    &&    (NULL != cfg->stack)
                    &&    (cfg->stack_size >= 8));

    pthis->entry = cfg->entry;
    pthis->stack = cfg->stack;
    pthis->stack_size = cfg->stack_size;

    return __vsf_thread_call_eda_ex((uintptr_t)__vsf_thread_evthandler,
                                    (uintptr_t)pthis, state, 0, (uintptr_t)NULL);
}

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED && VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
SECTION(".text.vsf.kernel.vk_thread_call_task")
fsm_rt_t vk_thread_call_task(vsf_task_entry_t task_handler, uintptr_t param, size_t local_size)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();

    VSF_KERNEL_ASSERT(NULL != ((vsf_eda_t *)thread_obj)->fn.frame);
    vsf_thread_cb_t *cb = (vsf_thread_cb_t *)((vsf_eda_t *)thread_obj)->fn.frame->ptr.param;
    VSF_KERNEL_ASSERT(NULL != cb);

    fsm_rt_t ret;

    while (1) {
        ret = __vsf_eda_call_task(task_handler, param, local_size);
        if (fsm_rt_on_going == ret) {
            __vsf_eda_yield();
        } else {
            break;
        }
        __vsf_thread_wait(cb);
    }

    return ret;
}
#   endif
#else
vsf_err_t vsf_thread_start(vsf_thread_t *pthis, vsf_prio_t priority)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    VSF_KERNEL_ASSERT(      (NULL != pthis->entry)
                        &&  (NULL != pthis->stack)
                        &&  (0 != pthis->stack_size));
    pthis->fn.evthandler = __vsf_thread_evthandler;

    if (pthis->stack_size < (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE + VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE)) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
    } else if (0 != (pthis->stack_size & (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE - 1))) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_PROVIDED_RESOURCE_NOT_ALIGNED;
    }

    vsf_err_t err;
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    err = vsf_teda_init(&pthis->use_as__vsf_teda_t, priority);
#   else
    err = vsf_eda_init(&pthis->use_as__vsf_eda_t, priority);
#   endif

#   if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info(&pthis->use_as__vsf_eda_t, NULL, pthis->stack, pthis->stack_size);
#   endif

    return err;
}
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
SECTION(".text.vsf.kernel.vsf_thread_delay")
void vsf_thread_delay(vsf_systimer_tick_t tick)
{
    vsf_teda_set_timer(tick);
    vsf_thread_wfe(VSF_EVT_TIMER);
}
#endif

SECTION(".text.vsf.kernel.vsf_thread_yield")
void vsf_thread_yield(void)
{
    __vsf_eda_yield();
    vsf_thread_wfe(VSF_EVT_YIELD);
}

#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
SECTION(".text.vsf.kernel.vsf_thread_set_priority")
vsf_prio_t vsf_thread_set_priority(vsf_prio_t priority)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();
    vsf_prio_t orig_prio = __vsf_eda_get_cur_priority(&thread_obj->use_as__vsf_eda_t);

    if (orig_prio != priority) {
        __vsf_eda_set_priority(&thread_obj->use_as__vsf_eda_t, priority);
        thread_obj->priority = priority;

        vsf_thread_yield();
    }
    return orig_prio;
}
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED

SECTION(".text.vsf.kernel.__vsf_thread_wait_for_sync")
vsf_sync_reason_t __vsf_thread_wait_for_sync(vsf_sync_t *sync, vsf_timeout_tick_t time_out)
{
    vsf_err_t err;
    vsf_sync_reason_t reason;

    err = vsf_eda_sync_decrease(sync, time_out);
    if (!err) { return VSF_SYNC_GET; }
    else if (err < 0) { return VSF_SYNC_FAIL; }
    else if (time_out != 0) {
        do {
            /*! \note there is a VSF_ASSERT() in __vsf_eda_sync_get_reason, which
             *!       validated the evt value. Hence, there is no need to assert
             *!       the evt value here.
             */
            reason = vsf_eda_sync_get_reason(sync, vsf_thread_wait());
        } while (reason == VSF_SYNC_PENDING);
        return reason;
    }
    return VSF_SYNC_TIMEOUT;
}

SECTION(".text.vsf.kernel.vsf_thread_mutex")
vsf_err_t vsf_thread_mutex_leave(vsf_mutex_t *mtx)
{
    vsf_err_t err = vsf_eda_mutex_leave(mtx);

#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
    vsf_eda_t *eda_self = vsf_eda_get_cur();
    // switch to original evtq right now if mutex boost priority of current task,
    //  or current task will switch to the original priority after current event is processed and returned
    if (eda_self->flag.state.is_new_prio) {
        // send and wait a VSF_EVT_YIELD event will make current event processed and returned
        vsf_thread_yield();
    }
#endif
    return err;
}

SECTION(".text.vsf.kernel.vsf_thread_queue")
vsf_sync_reason_t vsf_thread_queue_send(vsf_eda_queue_t *queue, void *node, vsf_timeout_tick_t timeout)
{
    vsf_err_t err = vsf_eda_queue_send(queue, node, timeout);
    if (VSF_ERR_NONE == err) {
        return VSF_SYNC_GET;
    } else if (0 == timeout) {
        return VSF_SYNC_TIMEOUT;
    }

    vsf_sync_reason_t reason;
    do {
        reason = vsf_eda_queue_send_get_reason(queue, vsf_thread_wait(), node);
    } while (reason == VSF_SYNC_PENDING);
    return reason;
}

SECTION(".text.vsf.kernel.vsf_thread_queue")
vsf_sync_reason_t vsf_thread_queue_recv(vsf_eda_queue_t *queue, void **node, vsf_timeout_tick_t timeout)
{
    vsf_err_t err = vsf_eda_queue_recv(queue, node, timeout);
    if (VSF_ERR_NONE == err) {
        return VSF_SYNC_GET;
    } else if (0 == timeout) {
        return VSF_SYNC_TIMEOUT;
    }

    vsf_sync_reason_t reason;
    do {
        reason = vsf_eda_queue_recv_get_reason(queue, vsf_thread_wait(), node);
    } while (reason == VSF_SYNC_PENDING);
    return reason;
}

#if VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED

SECTION(".text.vsf.kernel.vsf_thread_bmpevt_pend")
vsf_sync_reason_t vsf_thread_bmpevt_pend(
                    vsf_bmpevt_t *bmpevt,
                    vsf_bmpevt_pender_t *pender,
                    vsf_timeout_tick_t timeout)
{
    vsf_sync_reason_t reason;
    vsf_err_t err;
    vsf_evt_t evt;

    err = vsf_eda_bmpevt_pend(bmpevt, pender, timeout);
    if (!err) { return VSF_SYNC_GET;
    } else if (err < 0) { return VSF_SYNC_FAIL;
    } else if (timeout != 0) {
        while (1) {
            evt = vsf_thread_wait();
            /*! \note there is a VSF_ASSERT() in vsf_eda_bmpevt_poll, which
             *!       validated the evt value. Hence, there is no need to assert
             *!       the evt value here.
             */
            reason = vsf_eda_bmpevt_poll(bmpevt, pender, evt);
            if (reason != VSF_SYNC_PENDING) {
                return reason;
            }
        }
    }
    return VSF_SYNC_TIMEOUT;
}
#endif
#endif      // VSF_KERNEL_CFG_SUPPORT_SYNC

#endif
