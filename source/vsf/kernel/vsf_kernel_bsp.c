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

#include "./vsf_kernel_common.h"
#include "./vsf_eda.h"
#include "./vsf_evtq.h"
#include "./vsf_os.h"

/*============================ MACROS ========================================*/
#ifndef VSF_OS_EVTQ_SWI_NUM
#   define VSF_OS_EVTQ_SWI_NUM                  2
#endif

#define __VSF_OS_EVTQ_SWI_PRIO_INIT(__index, __unused)                          \
    VSF_ARCH_PRIO_##__index,

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if     VSF_OS_CFG_RUN_MAIN_AS_THREAD == ENABLED                                \
    &&  VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
declare_vsf_thread(app_main_thread_t)
def_vsf_thread(app_main_thread_t, VSF_OS_MAIN_STACK_SIZE)
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern void __vsf_main_entry(void);
extern int main(void);
ROOT
const vsf_kernel_resource_t * vsf_kernel_get_resource_on_init(void)
{

#if VSF_CFG_EVTQ_EN == ENABLED
    static const vsf_arch_priority_t __vsf_os_priority[VSF_OS_EVTQ_NUM] = {
        MREPEAT(VSF_OS_EVTQ_SWI_NUM, __VSF_OS_EVTQ_SWI_PRIO_INIT, NULL)
    };
    
#   if defined(VSF_CFG_EVTQ_LIST) && defined(VSF_OS_EVTQ_POOL_SIZE)
    static NO_INIT vsf_pool_block(vsf_evt_node_pool)    
        __evt_node_buffer[VSF_OS_EVTQ_POOL_SIZE];    
#   endif
#endif

#if     VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED \
    &&  defined(VSF_TASK_DEFAULT_FRAME_POOL_SIZE)
    static NO_INIT vsf_pool_block(vsf_eda_frame_pool) 
        __vsf_eda_frame_buffer[VSF_TASK_DEFAULT_FRAME_POOL_SIZE];
#endif

    static const vsf_kernel_resource_t res = {
#if VSF_CFG_EVTQ_EN == ENABLED
        .arch = {
            .os_priorities_ptr = __vsf_os_priority,
            .priority_cnt = UBOUND(__vsf_os_priority),
        },
#   if defined(VSF_CFG_EVTQ_LIST) && defined(VSF_OS_EVTQ_POOL_SIZE)
        .evt_queue = {
            .nodes_buf_ptr = __evt_node_buffer,
            .node_cnt = UBOUND(__evt_node_buffer),
        },
#   endif
#endif

#if     VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED \
    &&  defined(VSF_TASK_DEFAULT_FRAME_POOL_SIZE)
        .frame_stack = {
            .frame_buf_ptr = __vsf_eda_frame_buffer,
            .frame_cnt = UBOUND(__vsf_eda_frame_buffer),
        },
#endif

    };
    
    return &res;
}

uint32_t vsf_arch_req___systimer_freq___from_usr(void)
{
    return SYSTEM_FREQ;
}

#if     VSF_OS_CFG_RUN_MAIN_AS_THREAD == ENABLED                                \
    &&  VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
implement_vsf_thread(app_main_thread_t)
{
    main();
}

#endif

ROOT void __post_vsf_kernel_init(void)
{
#if     VSF_OS_CFG_RUN_MAIN_AS_THREAD == ENABLED                                \
    &&  VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    static NO_INIT app_main_thread_t __app_main;
    init_vsf_thread(app_main_thread_t, &__app_main, vsf_priority_0);
#else
    main();
#endif
}

#if VSF_USE_HEAP == ENABLED
WEAK vsf_mem_t vsf_service_req___heap_memory_buffer__from_usr(void)
{
#ifndef VSF_HEAP_SIZE
#   warning \
VSF_USE_HEAP is enabled but VSF_HEAP_SIZE hasn't been defined. You can define \
this macro in vsf_usr_cfg.h or you can call vsf_heap_add()/vsf_heap_add_memory()\
 to add memory buffers to heap.
    return (vsf_mem_t){0};
#else
    NO_INIT static uint_fast8_t s_chHeapBuffer[
        (VSF_HEAP_SIZE + sizeof(uint_fast8_t) - 1) / sizeof(uint_fast8_t)];
    return (vsf_mem_t){
        .pchSrc = (uint8_t *)s_chHeapBuffer, 
        .nSize = sizeof(s_chHeapBuffer)
    };
#endif
}
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
 
#if __IS_COMPILER_ARM_COMPILER_6__ || __IS_COMPILER_ARM_COMPILER_5__
#   ifdef __MICROLIB
__attribute__((noreturn, section(".ARM.Collect$$$$000000FF")))
void _main_init (void);
void _main_init (void) 
{
    __vsf_main_entry();
    while(1);
}
#   else
void  _platform_post_lib_init(void)
{
    __vsf_main_entry();
}
#   endif

#elif __IS_COMPILER_IAR__
extern int  __low_level_init(void);
extern void __iar_data_init3(void);
extern void exit(int arg);
__root
__noreturn __stackless void __cmain(void) 
{
  
  if (__low_level_init() != 0) {
    __iar_data_init3();
  }
  __vsf_main_entry();
  exit(0);
}
#elif __IS_COMPILER_GCC__
/*
ROOT void _start(void)
{
    __vsf_main_entry();
    while(1);
}
*/
#else

#warning please call __vsf_main_entry() before entering the main.
#endif

/*EOF*/
