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

#if defined(__WIN__)
//! GetSystemTimeAsFileTime has 100ns resolution, which is 10MHz
#   define VSF_SYSTIMER_RESOLUTION      (10 * 1000 * 1000)
#endif

#define VSF_HEAP_SIZE                   0x800
#define VSF_HEAP_CFG_MCB_MAGIC_EN       ENABLED

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//http://www.keil.com/support/man/docs/uv4/uv4_ut_configwizard.htm

//! \name kernel resource configuration
//! @{

//  <h> Kernel Resource Configuration

//      <h> Kernal event-driven system configuration
//          <o>Maximum event pool size
//          <i>Simon, please add description here...
//#define VSF_OS_CFG_EVTQ_POOL_SIZE           16

//          <o>Event Bits <4-8>
//          <i>Simon, please add description here...
#define VSF_OS_CFG_EVTQ_BITSIZE                 4

//          <o>The number of event queues   <1-4>
//          <i>Simon, please add description here...
#define VSF_OS_CFG_PRIORITY_NUM                 1
//      </h>
//#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE             ENABLED

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
#define VSF_KERNEL_CFG_SUPPORT_SYNC                 ENABLED
//      </c>

//      <h> Schedule Policy 
//          <c1>Enable Preemption
//          <i>Simon, please add description here...
#define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED               ENABLED
//          </c>

//          <c1>Enable Dynamic Task Priority
//          <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY     ENABLED
//          </c>
//      </h>

//      <c1>Enable eda to call other edas
//      <i>If this feature is enabled, eda is capable to call other eda based tasks, i.e. pure-eda, vsf_task, vsf_pt, simple_fsm and etc.
#define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL ENABLED
//      </c>

//      <c1>Enable Timer-integrated tasks (vsf_teda_t)
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER            ENABLED
//      </c>
#define VSF_KERNEL_CFG_CALLBACK_TIMER               ENABLED

//      <c1>Enable message queue support
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE            ENABLED
//      </c>

//      <c1>Enable Bitmap Event support
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT         ENABLED
//      </c>

//      <c1>Enable kernel tracing
//      <i>Simon, please add description here...
//#define VSF_KERNEL_CFG_TRACE                        ENABLED
//      </c>

//      <h> Main Function
//          <o>Main Stack Size              <128-65536:8>
//          <i>When main function is configured as a thread, this option controls the size of the stack.
#define VSF_OS_CFG_MAIN_STACK_SIZE                  2048

//          <c1>Run main as a thread
//          <i>This feature will run main function as a thread. RTOS thread support must be enabled. 
#define VSF_OS_CFG_MAIN_MODE                        VSF_OS_CFG_MAIN_MODE_IDLE
//          </c>
//      </h>
//      <h> Shell Configuration
//          <c1>Enable default VSF simple shell
//          <i>The default VSF simple shell provides an simple, easy and unified way to use kernel resources
#define VSF_USE_KERNEL_SIMPLE_SHELL                 ENABLED
//          </c>
//      </h>

//      <h> Task Form Configuration
//          <c1>Enable the VSF Co-oprative task support
//          <i>Enable this feature will provide cooperative task support, the task can be written as RTOS, PT and etc. The stack is shared and the call depth will be constant. 
#define VSF_KERNEL_CFG_EDA_SUPPORT_FSM              ENABLED
//          </c>
//          <c1>Enable the RTOS thread support
//          <i>Enable this feature will provide RTOS style of task support,i.e. tasks will have dedicated stacks
#define VSF_KERNEL_CFG_SUPPORT_THREAD               ENABLED
//          </c>
//          <c1>Enable the protoThread support
//          <i>Enable this feature will provide protoThread style of task support,i.e. tasks will share the same system stack
#define VSF_KERNEL_CFG_EDA_SUPPORT_PT               ENABLED
//          </c>
//      </h>
#define VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE     ENABLED
//  </h>
//! @}


#define VSF_USE_UI                          ENABLED
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
#   define VSF_USBD_USE_DCD_DWCOTG          ENABLED
#define VSF_USBD_USE_UVC                    ENABLED
#   define VSF_USBD_UVC_CFG_TRACE_EN        ENABLED

#define VSF_USE_TCPIP                       ENABLED
#define VSFIP_CFG_NETIF_HEADLEN             64

#define VSF_USE_TRACE                       DISABLED
#if defined(__WIN__) || defined(__LINUX__)
#   define VSF_TRACE_CFG_COLOR_EN           ENABLED
#endif

#define VSFVM_CFG_RUNTIME_EN                ENABLED
#define VSFVM_CFG_COMPILER_EN               ENABLED
#define VSFVM_LEXER_DEBUG_EN                DISABLED
#define VSFVM_PARSER_DEBUG_EN               DISABLED
#define VSFVM_COMPILER_DEBUG_EN             DISABLED
#define VSFVM_RUNTIME_DEBUG_EN              DISABLED

#define VSF_USE_PBUF                        DISABLED
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
            .target_ptr = &g_tGenericPBUFPool,                                    \
            .ID = VSF_PBUF_ADAPTER_CDC_SRC,                                     \
            .methods_ptr = &VSF_PBUF_ADAPTER_METHODS_STREAM_SRC,                  \
        }
#endif

/*----------------------------------------------------------------------------*
 * Regarget Weak interface                                                    *
 *----------------------------------------------------------------------------*/
#if defined(__WIN__)
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




#define WEAK_VSF_INPUT_ON_EVT_EVTERN                                            \
        extern void vsf_input_on_evt(vsf_input_type_t type, vsf_input_evt_t *evt);
#define WEAK_VSF_INPUT_ON_EVT(__TYPE, __EVT)                                    \
        vsf_input_on_evt((__TYPE), (__EVT))



#define WEAK_VSF_HEAP_MALLOC_ALIGNED_EXTERN                                     \
        extern void * vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment);
#define WEAK_VSF_HEAP_MALLOC_ALIGNED(__SIZE, __ALIGNMENT)                       \
        vsf_heap_malloc_aligned((__SIZE), (__ALIGNMENT))




#define WEAK_VSF_USBH_BTHCI_ON_NEW_EXTERN                                       \
        extern void vsf_usbh_bthci_on_new(void *dev, vsf_usbh_dev_id_t *id);
#define WEAK_VSF_USBH_BTHCI_ON_NEW(__DEV, __ID)                                 \
        vsf_usbh_bthci_on_new((__DEV), (__ID))

#define WEAK_VSF_USBH_BTHCI_ON_DEL_EXTERN                                       \
        extern void vsf_usbh_bthci_on_del(void *dev);
#define WEAK_VSF_USBH_BTHCI_ON_DEL(__DEV)                                       \
        vsf_usbh_bthci_on_del((__DEV))

#define WEAK_VSF_USBH_BTHCI_ON_PACKET_EXTERN                                    \
        extern void vsf_usbh_bthci_on_packet(void *dev, uint8_t type, uint8_t *packet, uint16_t size);
#define WEAK_VSF_USBH_BTHCI_ON_PACKET(__DEV, __TYPE, __PACKET, __SIZE)          \
        vsf_usbh_bthci_on_packet((__DEV), (__TYPE), (__PACKET), (__SIZE))


#define WEAK_VSF_BLUETOOTH_H2_ON_NEW_EXTERN                                     \
        extern vsf_err_t vsf_bluetooth_h2_on_new(void *dev, vsf_usbh_dev_id_t *id);
#define WEAK_VSF_BLUETOOTH_H2_ON_NEW(__DEV, __ID)                               \
        vsf_bluetooth_h2_on_new((__DEV), (__ID))
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
