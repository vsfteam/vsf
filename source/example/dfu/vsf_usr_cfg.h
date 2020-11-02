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

#define APP_CFG_USBD_VID                0xA7A8
#define APP_CFG_USBD_PID                0x2348
#define APP_CFG_USBD_EP0_SIZE           64
#define APP_CFG_USBD_VENDOR_STR         u"vsf"
#define APP_CFG_USBD_PRODUCT_STR        u"vsf_dfu"
#define APP_CFG_USBD_SERIAL_STR         u"0000"
#define APP_CFG_USBD_WEBUSB_URL         "devanlai.github.io/webdfu/dfu-util/"

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//http://www.keil.com/support/man/docs/uv4/uv4_ut_configwizard.htm

//! \name kernel resource configuration
//! @{

//  <h> Kernel Resource Configuration

//      <h> Kernal event-driven system configuration
//          <o>Maximum event pool size
//          <i>Simon, please add description here...
//#define VSF_OS_CFG_EVTQ_POOL_SIZE                   16

//          <o>Event Bits <4-8>
//          <i>Simon, please add description here...
#define VSF_OS_CFG_EVTQ_BITSIZE                     4

//          <o>The number of preemptive priorities <1-4>
//          <i>Simon, please add description here...
#define VSF_OS_CFG_PRIORITY_NUM                     1
//      </h>

#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                 DISABLED

//      <o>The default eda stack frame pool size <1-65535>
//      <i>The default eda stack frame pool is shared among all eda tasks.
//#define VSF_OS_CFG_DEFAULT_TASK_FRAME_POOL_SIZE     16
//  </h>


//! @}

//! \name kernel feature configuration
//! @{
//  <h> Kernel Feature Configuration
//      <c1>Enable Inter-task synchronisation (vsf_sync_t)
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_SYNC                 DISABLED
//      </c>

//      <h> Schedule Policy 
//          <c1>Enable Preemption
//          <i>Simon, please add description here...
#define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED DISABLED
//          </c>

//          <c1>Enable Dynamic Task Priority
//          <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY     DISABLED
//          </c>
//      </h>

//      <c1>Enable eda to call other edas
//      <i>If this feature is enabled, eda is capable to call other eda based tasks, i.e. pure-eda, vsf_task, vsf_pt, simple_fsm and etc.
#define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL         DISABLED
//      </c>

//      <c1>Enable Timer-integrated tasks (vsf_teda_t)
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER            DISABLED
//      </c>
#define VSF_KERNEL_CFG_CALLBACK_TIMER               DISABLED

//      <c1>Enable message queue support
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE            DISABLED
//      </c>

//      <c1>Enable Bitmap Event support
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT         DISABLED
//      </c>

//      <c1>Enable kernel tracing
//      <i>Simon, please add description here...
//#define VSF_KERNEL_CFG_TRACE                        ENABLED
//      </c>

//      <h> Main Function
//          <o>Main Stack Size              <128-65536:8>
//          <i>When main function is configured as a thread, this option controls the size of the stack.
//#define VSF_OS_CFG_MAIN_STACK_SIZE                  2048

//          <c1>Run main as a thread
//          <i>This feature will run main function as a thread. RTOS thread support must be enabled. 
#define VSF_OS_CFG_MAIN_MODE                        VSF_OS_CFG_MAIN_MODE_IDLE
//          </c>

//      </h>
//      <h> Shell Configuration
//          <c1>Enable default VSF simple shell
//          <i>The default VSF simple shell provides an simple, easy and unified way to use kernel resources
#define VSF_USE_KERNEL_SIMPLE_SHELL                 DISABLED
//          </c>
//      </h>

//      <h> Task Form Configuration
//          <c1>Enable the VSF Co-oprative task support
//          <i>Enable this feature will provide cooperative task support, the task can be written as RTOS, PT and etc. The stack is shared and the call depth will be constant. 
#define VSF_KERNEL_CFG_EDA_SUPPORT_FSM              DISABLED
//          </c>
//          <c1>Enable the RTOS thread support
//          <i>Enable this feature will provide RTOS style of task support,i.e. tasks will have dedicated stacks
#define VSF_KERNEL_CFG_SUPPORT_THREAD               DISABLED
//          </c>
//          <c1>Enable the protoThread support
//          <i>Enable this feature will provide protoThread style of task support,i.e. tasks will share the same system stack
#define VSF_KERNEL_CFG_EDA_SUPPORT_PT               DISABLED
//          </c>
//      </h>
#define VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE     DISABLED
//  </h>
//! @}

#define VSF_USE_USB_DEVICE                          ENABLED
#   define VSF_USBD_CFG_USE_EDA                     DISABLED
#   define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_0
#   define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_0
#   define VSF_USBD_CFG_RAW_MODE                    ENABLED
#   define VSF_USBD_CFG_STREAM_EN                   DISABLED

#define VSF_USE_TRACE                               ENABLED
#define VSF_USE_SIMPLE_STREAM                       ENABLED

// heap is enabled by default, disable manually
#define VSF_USE_HEAP                                DISABLED

#if __M484__
#define VSF_SYSTIMER_FREQ                           (192000000ul)
#define VSF_DEBUGGER_CFG_CONSOLE                    VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE

#   if VSF_USE_USB_DEVICE == ENABLED

#       define APP_CFG_USBD_SPEED                   USB_DC_SPEED_HIGH
#       define VSF_USBD_CFG_DRV_LV0                 m480_usbd_hs
#       define VSF_USBD_CFG_DRV_OBJ                 USB_DC0

#   endif

#elif __WIN__

#   define VSF_SYSTIMER_FREQ                        (0ul)
#   define VSF_HAL_USE_DEBUG_STREAM                 ENABLED

#   if VSF_USE_USB_DEVICE == ENABLED
#       define VSF_USBD_USE_DCD_USBIP               ENABLED
#       define APP_CFG_USBD_SPEED                   USB_DC_SPEED_HIGH

#       define VSF_USBD_CFG_DRV_LV0_OO
#           define VSF_USBD_CFG_DRV_LV0_OO_PREFIX   vk_usbip_usbd
#           define VSF_USBD_CFG_DRV_LV0_OO_OBJ      usrapp_usbd_common.usbip_dcd
#       define VSF_CFG_USER_HEADER                  "usrapp_usbd_common.h"

        // usbip_dcd depends on timer support
#       undef VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#       define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER     ENABLED

        // usbip_dcd depends on heap
#       undef VSF_USE_HEAP
#       define VSF_USE_HEAP                         ENABLED
#   endif

//  weak

#   if VSF_USE_USB_DEVICE == ENABLED
#       define WEAK_VSF_USBD_NOTIFY_USER
#   endif

#   define WEAK_VSF_KERNEL_ERR_REPORT_EXTERN                                    \
        extern void vsf_kernel_err_report(vsf_kernel_error_t err);
#   define WEAK_VSF_KERNEL_ERR_REPORT(__ERR)                                    \
        vsf_kernel_err_report(__ERR)

#   define WEAK___POST_VSF_KERNEL_INIT_EXTERN                                   \
        extern void __post_vsf_kernel_init(void);
#   define WEAK___POST_VSF_KERNEL_INIT()                                        \
        __post_vsf_kernel_init()

#   define WEAK_VSF_DRIVER_INIT_EXTERN                                          \
        bool vsf_driver_init(void);
#   define WEAK_VSF_DRIVER_INIT()                                               \
        vsf_driver_init()

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#       define WEAK_VSF_SYSTIMER_EVTHANDLER_EXTERN                              \
            extern void vsf_systimer_evthandler(vsf_systimer_cnt_t tick);
#       define WEAK_VSF_SYSTIMER_EVTHANDLER(__TICK)                             \
            vsf_systimer_evthandler(__TICK)
#   endif

#   if VSF_USE_HEAP == ENABLED
#       define WEAK_VSF_HEAP_MALLOC_ALIGNED_EXTERN                              \
            extern void * vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment);
#       define WEAK_VSF_HEAP_MALLOC_ALIGNED(__SIZE, __ALIGNMENT)                \
            vsf_heap_malloc_aligned((__SIZE), (__ALIGNMENT))
#   endif

#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */