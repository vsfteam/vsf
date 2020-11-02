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

#define ASSERT(...)                     if (!(__VA_ARGS__)) {while(1);};
//#define ASSERT(...)

#define APP_CFG_USBD_VID                A7A8
#define APP_CFG_USBD_PID                2348

#define VSF_SYSTIMER_FREQ               (192000000ul)

#define VSF_HEAP_SIZE                   (16*1024)
#define VSF_HEAP_CFG_MCB_MAGIC_EN       ENABLED

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//http://www.keil.com/support/man/docs/uv4/uv4_ut_configwizard.htm

//! \name kernel resource configuration
//! @{

//  <h> Kernel Resource Configuration

//      <h> Kernal event-driven system configuration
//          <o>Maximum event pool size
//          <i>Simon, please add description here...
//#define VSF_OS_CFG_EVTQ_POOL_SIZE                       16

//          <o>Event Bits <4-8>
//          <i>Simon, please add description here...
#define VSF_OS_CFG_EVTQ_BITSIZE                         4

//          <o>The number of preemptive priorities <1-4>
//          <i>Simon, please add description here...
#define VSF_OS_CFG_PRIORITY_NUM                         10
//      </h>

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
#define VSF_KERNEL_CFG_SUPPORT_SYNC                     ENABLED
//      </c>

//      <h> Schedule Policy 
//          <c1>Enable Preemption
//          <i>Simon, please add description here...
#define VSF_OS_CFG_SUPPORT_PREMPT                       ENABLED
//          </c>

//          <c1>Enable Dynamic Task Priority
//          <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY         ENABLED
//          </c>
//      </h>

//      <c1>Enable eda to call other edas
//      <i>If this feature is enabled, eda is capable to call other eda based tasks, i.e. pure-eda, vsf_task, vsf_pt, simple_fsm and etc.
#define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL             ENABLED
//      </c>

//      <c1>Enable Timer-integrated tasks (vsf_teda_t)
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER                ENABLED
//      </c>
#define VSF_KERNEL_CFG_CALLBACK_TIMER                   ENABLED

//      <c1>Enable message queue support
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE                ENABLED
//      </c>

//      <c1>Enable Bitmap Event support
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT             ENABLED
//      </c>

//      <c1>Enable kernel tracing
//      <i>Simon, please add description here...
//#define VSF_KERNEL_CFG_TRACE          ENABLED
//      </c>

//      <h> Main Function
//          <o>Main Stack Size              <128-65536:8>
//          <i>When main function is configured as a thread, this option controls the size of the stack.
#define VSF_OS_CFG_MAIN_STACK_SIZE                      2048

//          <c1>Run main as a thread
//          <i>This feature will run main function as a thread. RTOS thread support must be enabled. 
#define VSF_OS_CFG_MAIN_MODE                            VSF_OS_CFG_MAIN_MODE_IDLE
//          </c>
//      </h>
//      <h> Shell Configuration
//          <c1>Enable default VSF simple shell
//          <i>The default VSF simple shell provides an simple, easy and unified way to use kernel resources
#define VSF_USE_KERNEL_SIMPLE_SHELL                     ENABLED
//          </c>
//      </h>

//      <h> Task Form Configuration
//          <c1>Enable the VSF Co-oprative task support
//          <i>Enable this feature will provide cooperative task support, the task can be written as RTOS, PT and etc. The stack is shared and the call depth will be constant. 
#define VSF_KERNEL_CFG_EDA_SUPPORT_FSM                  ENABLED
//          </c>
//          <c1>Enable the RTOS thread support
//          <i>Enable this feature will provide RTOS style of task support,i.e. tasks will have dedicated stacks
#define VSF_KERNEL_CFG_SUPPORT_THREAD                   ENABLED
//          </c>
//          <c1>Enable the protoThread support
//          <i>Enable this feature will provide protoThread style of task support,i.e. tasks will share the same system stack
#define VSF_KERNEL_CFG_EDA_SUPPORT_PT                   ENABLED
//          </c>
//      </h>
#define VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE         ENABLED
//  </h>
//! @}


#define VSF_USE_AV                          ENABLED

#define VSF_USE_INPUT                       ENABLED
#define VSF_INPUT_USE_HID                   ENABLED

#define VSF_USE_USB_HOST                    ENABLED
#define VSF_USBH_USE_HUB                    ENABLED
#define VSF_USBH_USE_ECM                    ENABLED
#define VSF_USBH_USE_HID                    ENABLED
#define VSF_USBH_USE_BTHCI                  ENABLED
#define VSF_USBH_USE_HCD_OHCI               ENABLED

#define VSF_USE_USB_DEVICE                  ENABLED
#define VSF_USBD_USE_UVC                    ENABLED
#   define VSF_USBD_UVC_CFG_TRACE_EN        ENABLED

#define VSF_USE_TCPIP                       ENABLED
#define VSFIP_CFG_NETIF_HEADLEN             64

#define VSF_USE_TRACE                       ENABLED
#define VSF_DEBUGGER_CFG_CONSOLE            VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE

#define VSF_USE_PBUF                        ENABLED
#define VSF_PBUF_CFG_INDIRECT_RW_SUPPORT    DISABLED


#define VSF_USE_STREAM                      DISABLED
#define VSF_USE_SIMPLE_STREAM               ENABLED

#if VSF_USE_STREAM == ENABLED
#define VSF_SERVICE_CFG_INSERTION                                               \
    extern vsf_pbuf_pool_t  g_tGenericPBUFPool;
//#define VSF_SERVICE_CFG_DEPENDENCY      

enum {
    VSF_PBUF_ADAPTER_CDC_SRC = 1,
};

#define VSF_PBUF_ADAPTERS                                                       \
        {                                                                       \
            .ptTarget = &g_tGenericPBUFPool,                                    \
            .ID = VSF_PBUF_ADAPTER_CDC_SRC,                                     \
            .piMethods = &VSF_PBUF_ADAPTER_METHODS_STREAM_SRC,                  \
        }
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */