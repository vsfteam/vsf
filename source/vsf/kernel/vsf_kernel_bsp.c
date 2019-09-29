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
#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED
#include "./vsf_kernel_common.h"
#include "./vsf_eda.h"
#include "./vsf_evtq.h"
#include "./vsf_os.h"

/*============================ MACROS ========================================*/

#define __VSF_OS_EVTQ_SWI_PRIO_INIT(__index, __unused)                          \
    VSF_ARCH_PRIO_##__index,

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if     VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD                     \
    &&  VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
declare_vsf_thread(app_main_thread_t)
def_vsf_thread(app_main_thread_t, VSF_OS_CFG_MAIN_STACK_SIZE)
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if !__IS_COMPILER_IAR__ && __IS_COMPILER_SUPPORT_GNUC_EXTENSION__
__attribute__((constructor(255)))
#endif
extern void __vsf_main_entry(void);
extern int main(void);
ROOT
const vsf_kernel_resource_t * vsf_kernel_get_resource_on_init(void)
{

#if __VSF_OS_SWI_NUM > 0
#define MFUNC_IN_U8_DEC_VALUE                   __VSF_OS_SWI_NUM
#include "utilities/preprocessor/mf_u8_dec2str.h"
    static const vsf_arch_prio_t __vsf_os_swi_priority[MFUNC_OUT_DEC_STR] = {
        MREPEAT(MFUNC_OUT_DEC_STR, __VSF_OS_EVTQ_SWI_PRIO_INIT, NULL)
    };
#endif

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED

    static NO_INIT vsf_evtq_t __vsf_os_evt_queue[VSF_OS_CFG_PRIORITY_NUM];
    
#   if defined(__VSF_OS_CFG_EVTQ_LIST) && defined(VSF_OS_CFG_EVTQ_POOL_SIZE)
    static NO_INIT vsf_pool_block(vsf_evt_node_pool)    
        __evt_node_buffer[VSF_OS_CFG_EVTQ_POOL_SIZE];    
#   endif

#   if defined(__VSF_OS_CFG_EVTQ_ARRAY)
    static NO_INIT vsf_evt_node_t 
        __vsf_os_nodes[VSF_OS_CFG_PRIORITY_NUM][1 << VSF_OS_CFG_EVTQ_BITSIZE];
#   endif
#endif

#if     __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED \
    &&  defined(VSF_OS_CFG_DEFAULT_TASK_FRAME_POOL_SIZE)
    static NO_INIT vsf_pool_block(vsf_eda_frame_pool) 
        __vsf_eda_frame_buffer[VSF_OS_CFG_DEFAULT_TASK_FRAME_POOL_SIZE];
#endif

    static const vsf_kernel_resource_t res = {
#if __VSF_OS_SWI_NUM > 0
        {
            __vsf_os_swi_priority,                  // os_swi_priorities_ptr
            UBOUND(__vsf_os_swi_priority),          // swi_priority_cnt
        },
#endif

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
        {
            __vsf_os_evt_queue,                     // queue_array
#   if defined(__VSF_OS_CFG_EVTQ_ARRAY)
            (vsf_evt_node_t **)__vsf_os_nodes,      // nodes
            VSF_OS_CFG_EVTQ_BITSIZE,                // node_bit_sz
#   endif
#   if defined(__VSF_OS_CFG_EVTQ_LIST)
#       if defined(VSF_OS_CFG_EVTQ_POOL_SIZE)
            __evt_node_buffer,                      // nodes_buf_ptr
            (uint16_t)UBOUND(__evt_node_buffer),    // node_cnt
#       else
            NULL,                                   // nodes_buf_ptr
            0,                                      // node_cnt
#       endif
#   endif    
            (uint16_t)UBOUND(__vsf_os_evt_queue),   // queue_cnt
        },

#endif

#if     __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED \
    &&  defined(VSF_OS_CFG_DEFAULT_TASK_FRAME_POOL_SIZE)
        {
            __vsf_eda_frame_buffer,                 // frame_buf_ptr
            UBOUND(__vsf_eda_frame_buffer),         // frame_cnt
        },
#endif

    };
    
    return &res;
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
uint32_t vsf_arch_req___systimer_freq___from_usr(void)
{
    return VSF_SYSTIMER_FREQ;
}

uint_fast32_t vsf_arch_req___systimer_resolution___from_usr(void)
{
    return VSF_SYSTIMER_RESOLUTION;
}
#endif

void vsf_kernel_err_report(vsf_kernel_error_t err)
{
    switch (err) {
        default:
        case VSF_KERNEL_ERR_NULL_EDA_PTR:
            /*! \note
            This should not happen. Two possible reasons could be:
            1. Forgeting to set VSF_OS_CFG_MAIN_MODE to VSF_OS_CFG_MAIN_MODE_THREAD
               and using vsf kernel APIs, e.g. vsf_delay_ms, vsf_sem_pend and etc.
            2. When VSF_OS_CFG_MAIN_MODE is not VSF_OS_CFG_MAIN_MODE_THREAD, using
               any vsf_eda_xxxx APIs. 
            */
            {
                vsf_gint_state_t gint_state = vsf_disable_interrupt(); 
                while(1);
                vsf_set_interrupt(gint_state);
            }
            break;
        case VSF_KERNEL_ERR_NONE:
            break;
    }
    
}


#if     VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD                     \
    &&  VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
implement_vsf_thread(app_main_thread_t)
{
    main();
}
#elif   VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_EDA
static void __app_main_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    main();
}
#endif

ROOT void __post_vsf_kernel_init(void)
{
#if     VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD                     \
    &&  VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    static NO_INIT app_main_thread_t __app_main;
    init_vsf_thread(app_main_thread_t, &__app_main, vsf_prio_0);
#elif   VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_EDA
    const vsf_eda_cfg_t cfg = {
        .evthandler = __app_main_evthandler,
        .priority = vsf_prio_0,
    };
#   if  VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    static NO_INIT vsf_teda_t __app_main;
    vsf_teda_init_ex(&__app_main, (vsf_eda_cfg_t *)&cfg);
#   else
    static NO_INIT vsf_eda_t __app_main;
    vsf_eda_init_ex(&__app_main, (vsf_eda_cfg_t *)&cfg);
#   endif
#elif   VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_IDLE
    main();
#elif   VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_NONE
#else
#   error Please define VSF_OS_CFG_MAIN_MODE!!!
#endif
}

#if VSF_USE_HEAP == ENABLED
#ifndef WEAK_VSF_SERVICE_REQ___HEAP_MEMORY_BUFFER___FROM_USR
WEAK(vsf_service_req___heap_memory_buffer___from_usr)
vsf_mem_t vsf_service_req___heap_memory_buffer___from_usr(void)
{
#ifndef VSF_HEAP_SIZE
#   warning \
"VSF_USE_HEAP is enabled but VSF_HEAP_SIZE hasn't been defined. You can define \
this macro in vsf_usr_cfg.h or you can call vsf_heap_add()/vsf_heap_add_memory()\
 to add memory buffers to heap."
    return (vsf_mem_t){0};
#else
    NO_INIT static uint_fast8_t s_chHeapBuffer[
        (VSF_HEAP_SIZE + sizeof(uint_fast8_t) - 1) / sizeof(uint_fast8_t)];
    
    return (vsf_mem_t){
        .PTR.pchSrc = (uint8_t *)s_chHeapBuffer, 
        .nSize = sizeof(s_chHeapBuffer)
    };
#endif
}
#endif
#endif

/*============================ IMPLEMENTATION ================================*/


#if __IS_COMPILER_ARM_COMPILER_6__
__asm(".global __use_no_semihosting\n\t");

#ifndef __MICROLIB
__asm(".global __ARM_use_no_argv\n\t");
#endif


#endif

/*----------------------------------------------------------------------------*
 * Compiler Specific Code to run __vsf_main_entry() before main()             *
 *----------------------------------------------------------------------------*/
#if __IS_COMPILER_IAR__
WEAK(__low_level_init)
char __low_level_init(void)
{
    return 1;
}

extern void __IAR_STARTUP_DATA_INIT(void);
extern void exit(int arg);
__root
__noreturn __stackless void __cmain(void) 
{
    if (__low_level_init() != 0) {
        __IAR_STARTUP_DATA_INIT();
    }
    __vsf_main_entry();
    exit(0);
}
#elif   __IS_COMPILER_GCC__                                                     \
    ||  __IS_COMPILER_LLVM__                                                    \
    ||  __IS_COMPILER_ARM_COMPILER_5__                                          \
    ||  __IS_COMPILER_ARM_COMPILER_6__

#else

#warning please call __vsf_main_entry() before entering the main.
#endif

#endif
/*EOF*/
