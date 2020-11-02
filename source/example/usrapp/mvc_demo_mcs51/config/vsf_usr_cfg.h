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


//! \note Top Level Application Configuration

#ifndef __TOP_APP_CFG_H__
#define __TOP_APP_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define ASSERT(__VAR__)                     if (!(__VAR__)) {while(1);};
//#define ASSERT(...)

#define VSF_HEAP_SIZE                   0x800
#define VSF_HEAP_CFG_MCB_MAGIC_EN       ENABLED

#define VSF_SYSTIMER_FREQ               (50ul * 1000 * 1000)
#define VSF_SYSTIMER_RESOLUTION         (1ul * 1000 * 1000)

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//http://www.keil.com/support/man/docs/uv4/uv4_ut_configwizard.htm

//! \name kernel resource configuration
//! @{

//  <h> Kernel Resource Configuration

//      <h> Kernal event-driven system configuration
//          <o>Maximum event pool size
//          <i>Simon, please add description here...
//#define VSF_OS_CFG_EVTQ_POOL_SIZE               16

//          <o>Event Bits <4-8>
//          <i>Simon, please add description here...
#define VSF_OS_CFG_EVTQ_BITSIZE                 4

//          <o>The number of event queues   <1-4>
//          <i>Simon, please add description here...
#define VSF_OS_CFG_PRIORITY_NUM                 1
//      </h>

#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE             ENABLED

//      <o>The default vsf_task_t stack frame pool size <1-65535>
//      <i>The default stack pool is shared among all vsf tasks which do not specify a private frame pool. For such case, the pool size should be at least twice of the maximum number of simultaneously running vsf tasks.
//#define VSF_OS_CFG_DEFAULT_TASK_FRAME_POOL_SIZE    16
//  </h>


//! @}

//! \name kernel feature configuration
//! @{
//  <h> Kernel Feature Configuration
//      <c1>Enable Inter-task synchronisation (vsf_sync_t)
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_SYNC             DISABLED
//      </c>

//      <h> Schedule Policy
//          <c1>Enable Preemption
//          <i>Simon, please add description here...
#define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED           ENABLED
//          </c>

//          <c1>Enable Dynamic Task Priority
//          <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY DISABLED
//          </c>
//      </h>

//      <c1>Enable eda to call other edas
//      <i>If this feature is enabled, eda is capable to call other eda based tasks, i.e. pure-eda, vsf_task, vsf_pt, simple_fsm and etc.
#define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL     DISABLED
//      </c>

//      <c1>Enable Timer-integrated tasks (vsf_teda_t)
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER        ENABLED
//      </c>
#define VSF_KERNEL_CFG_CALLBACK_TIMER           ENABLED

//      <c1>Enable message queue support
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE        DISABLED
//      </c>

//      <c1>Enable Bitmap Event support
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT     DISABLED
//      </c>

//      <c1>Enable kernel tracing
//      <i>Simon, please add description here...
//#define VSF_KERNEL_CFG_TRACE                    ENABLED
//      </c>

//      <h> Main Function
//          <o>Main Stack Size              <128-65536:8>
//          <i>When main function is configured as a thread, this option controls the size of the stack.
#define VSF_OS_CFG_MAIN_STACK_SIZE              2048

//          <c1>Run main as a thread
//          <i>This feature will run main function as a thread. RTOS thread support must be enabled.
#define VSF_OS_CFG_MAIN_MODE                    VSF_OS_CFG_MAIN_MODE_IDLE
//          </c>
//      </h>
//      <h> Shell Configuration
//          <c1>Enable default VSF simple shell
//          <i>The default VSF simple shell provides an simple, easy and unified way to use kernel resources
#define VSF_USE_KERNEL_SIMPLE_SHELL             ENABLED
//          </c>
//      </h>

//      <h> Task Form Configuration
//          <c1>Enable the VSF Co-oprative task support
//          <i>Enable this feature will provide cooperative task support, the task can be written as RTOS, PT and etc. The stack is shared and the call depth will be constant.
#define VSF_KERNEL_CFG_EDA_SUPPORT_FSM          DISABLED
//          </c>
//          <c1>Enable the RTOS thread support
//          <i>Enable this feature will provide RTOS style of task support,i.e. tasks will have dedicated stacks
#define VSF_KERNEL_CFG_SUPPORT_THREAD           DISABLED
//          </c>
//          <c1>Enable the protoThread support
//          <i>Enable this feature will provide protoThread style of task support,i.e. tasks will share the same system stack
#define VSF_KERNEL_CFG_EDA_SUPPORT_PT           ENABLED
//          </c>
//      </h>
#define VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE ENABLED
//  </h>
//! @}

#define VSF_USE_HEAP                        DISABLED

#define VSF_USE_POOL                        ENABLED
#   define VSF_POOL_CFG_STATISTIC_MODE          DISABLED
#   define VSF_POOL_CFG_FEED_ON_HEAP            DISABLED
#   define VSF_POOL_CFG_SUPPORT_USER_OBJECT     DISABLED
#   define VSF_POOL_CFG_SUPPORT_USER_ITEM_INIT  DISABLED

#define VSF_USE_TRACE                       DISABLED
#   define VSF_TRACE_CFG_COLOR_EN           DISABLED

#define VSF_USE_STREAM                      DISABLED
#define VSF_USE_SIMPLE_STREAM               DISABLED

#define VSF_USE_INPUT                                   DISABLED
#define VSF_USE_UI                                      ENABLED
#   define VSF_USE_LVGL                                 DISABLED
#   define VSF_USE_DISP_DRV_SDL2                        DISABLED

#   define VSF_USE_TINY_GUI                             ENABLED
#       define VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL      VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW
#       define VSF_TGUI_CFG_COLOR_MODE                  VSF_TGUI_COLOR_ARGB_8888
#       define VSF_TGUI_CFG_SUPPORT_NAME_STRING         ENABLED         /* Enabled for debug */

#   define VSF_USE_MSG_TREE                             ENABLED
#       define VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING     ENABLED         /* Enabled for debug */
#       define VSF_MSGT_NODE_OFFSET_TYPE                int8_t

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"

#if __IS_COMPILER_51_KEIL__
#define VSF_POOL_LOCK()                     { vsf_gint_state_t gint_state = __disable_interrupt();
#define VSF_POOL_UNLOCK()                   __set_interrupt_state(gint_state);}

/*----------------------------------------------------------------------------*
 * Regarget Weak interface                                                    *
 *----------------------------------------------------------------------------*/

#define WEAK_VSF_DRIVER_INIT_EXTERN                                             \
        extern bool vsf_driver_init(void);
#define WEAK_VSF_DRIVER_INIT()                                                  \
        vsf_driver_init()

#define WEAK_VSF_KERNEL_ERR_REPORT_EXTERN                                       \
        extern void vsf_kernel_err_report(vsf_kernel_error_t err);
#define WEAK_VSF_KERNEL_ERR_REPORT(__ERR)                                       \
        vsf_kernel_err_report(__ERR)

#define WEAK___POST_VSF_KERNEL_INIT_EXTERN                                      \
        extern void __post_vsf_kernel_init(void);
#define WEAK___POST_VSF_KERNEL_INIT()                                           \
        __post_vsf_kernel_init()

#define WEAK_VSF_SYSTIMER_EVTHANDLER_EXTERN                                     \
        extern void vsf_systimer_evthandler(vsf_systimer_cnt_t tick);
#define WEAK_VSF_SYSTIMER_EVTHANDLER(__TICK)                                    \
        vsf_systimer_evthandler(__TICK)

#define WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR_EXTERN               \
        extern uint_fast32_t vsf_arch_req___systimer_resolution___from_usr(void);
#define WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR()                    \
        vsf_arch_req___systimer_resolution___from_usr()

#define WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR_EXTERN                     \
        extern uint_fast32_t vsf_arch_req___systimer_freq___from_usr(void);
#define WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR()                          \
        vsf_arch_req___systimer_freq___from_usr()




#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED
#   define WEAK_VSF_INPUT_ON_TOUCHSCREEN_EXTERN                                 \
        extern void vsf_input_on_touchscreen(vsf_touchscreen_evt_t *ts_evt);
#   define WEAK_VSF_INPUT_ON_TOUCHSCREEN(__TS_EVT)                              \
        vsf_input_on_touchscreen((__TS_EVT))
#endif

#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#include "vsf_tgui_cfg.h"

#endif
/* EOF */