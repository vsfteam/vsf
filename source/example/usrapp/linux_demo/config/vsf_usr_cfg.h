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

//          <o>The number of preemptive priorities <1-4>
//          <i>Simon, please add description here...
#define VSF_OS_CFG_PRIORITY_NUM                 2
//      </h>

//      <o>The default eda stack frame pool size <1-65535>
//      <i>The default eda stack frame pool is shared among all eda tasks.
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
#define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED                   ENABLED
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

//      <c1>Enable message queue support
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE                ENABLED
//      </c>
#define VSF_KERNEL_CFG_CALLBACK_TIMER                   ENABLED

//      <c1>Enable Bitmap Event support
//      <i>Simon, please add description here...
#define VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT             ENABLED
//      </c>

//      <c1>Enable kernel tracing
//      <i>Simon, please add description here...
//#define VSF_KERNEL_CFG_TRACE                          ENABLED
//      </c>

//      <h> Main Function
//          <o>Main Stack Size              <128-65536:8>
//          <i>When main function is configured as a thread, this option controls the size of the stack.
//#define VSF_OS_CFG_MAIN_STACK_SIZE                    2048

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
//  </h>
//! @}



#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED

#define VSF_USE_USB_HOST                                ENABLED
#   define VSF_USBH_USE_ECM                             ENABLED
#   define VSF_USBH_USE_LIBUSB                          ENABLED

// VSF_USE_USB_DEVICE will be enabled if target chip supports USBD
//#define VSF_USE_USB_DEVICE                              ENABLED
#   define VSF_USBD_CFG_USE_EDA                         ENABLED
#   define VSF_USBD_USE_CDCACM                          ENABLED
#   define APP_CFG_USBD_VID                             0xA7A8
#   define APP_CFG_USBD_PID                             0x2348

#define VSF_USE_MAL                                     ENABLED
#   define VSF_MAL_USE_MEM_MAL                          ENABLED
#   define VSF_MAL_USE_FAKEFAT32_MAL                    ENABLED

#define VSF_USE_FS                                      ENABLED
#   define VSF_FS_USE_MEMFS                             ENABLED
#   define VSF_FS_USE_FATFS                             ENABLED

#define VSF_USE_TRACE                                   ENABLED

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_LINUX_USE_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_LINUX_USE_BUSYBOX                        ENABLED



#define VSF_USE_PBUF                                    ENABLED
#define VSF_PBUF_CFG_INDIRECT_RW_SUPPORT                DISABLED
#define VSF_PBUF_CFG_SUPPORT_REF_COUNTING               DISABLED

#define VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE               DISABLED
#define VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION      DISABLED
#define VSF_STREAM_CFG_GENERAL_PBUF_POOL                DISABLED
#define GENERAL_PBUF_POOL_BLOCK_SIZE                    64
//#define GENERAL_PBUF_POOL_BLOCK_COUNT                   16
#define GENERAL_PBUF_POLL_PRIV_USER_COUNT               2

#define VSF_USE_STREAM                                  DISABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED

#if VSF_USE_STREAM == ENABLED

/* \note uncomment this part to add dedicated pbuf pool
enum {
#   if VSF_STREAM_CFG_GENERAL_PBUF_POOL == ENABLED
    VSF_PBUF_ADAPTER_GENERAL = 0,
#   endif
    VSF_PBUF_ADAPTER_XXXXX,
};

#   define VSF_SERVICE_CFG_INSERTION                                            \
    extern vsf_pbuf_pool_t  g_tGenericPBUFPool;

#     define VSF_SERVICE_CFG_DEPENDENCY



#   define VSF_PBUF_ADAPTERS                                                    \
        vsf_pbuf_pool_adapter(VSF_PBUF_ADAPTER_XXXXX, &g_tGenericPBUFPool)
*/


#endif


#ifndef USRAPP_CFG_DCD_TYPE_DEFAULT
#   define USRAPP_CFG_DCD_TYPE_DEFAULT      0
#   define USRAPP_CFG_DCD_TYPE_DWCOTG       1
#   define USRAPP_CFG_DCD_TYPE_MUSB_FDRC    2
#endif

#define USRAPP_CFG_FAKEFAT32                            ENABLED

#if     defined(__M484__)
#   define VSF_HEAP_SIZE                                0x10000
#   define VSF_SYSTIMER_FREQ                            (192000000ul)

#   define VSF_USE_USB_DEVICE                           ENABLED
#       define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_1
#       define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_1
#       define USRAPP_CFG_USBD_SPEED                    USB_SPEED_HIGH
#       define USRAPP_CFG_CDC_NUM                       1
#       define USRAPP_CFG_CDC_TX_STREAM_SIZE            1024
#       define USRAPP_CFG_CDC_RX_STREAM_SIZE            512
#       define USRAPP_CFG_DCD_TYPE                      USRAPP_CFG_DCD_TYPE_DEFAULT
#       define USRAPP_CFG_STREAM_ALIGN                  1

#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 ENABLED
#   define VSF_USBH_USE_HUB                             ENABLED
#   define VSF_USBH_USE_HCD_OHCI                        ENABLED

#   define VSF_LINUX_CFG_STACKSIZE                      2048
#   define VSF_TRACE_CFG_COLOR_EN                       DISABLED
#   define VSH_HAS_COLOR                                0

#   define VSF_DEBUGGER_CFG_CONSOLE                     VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE
#elif   defined(__NUC505__)
#   define VSF_HEAP_SIZE                                0x4000
#   define VSF_SYSTIMER_FREQ                            (96000000ul)

#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 ENABLED
#   define VSF_USBH_USE_HUB                             ENABLED
#   define VSF_USBH_USE_HCD_OHCI                        ENABLED

#   define VSF_DEBUGGER_CFG_CONSOLE                     VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE
#elif   defined(__STM32F730R8__)
#   define VSF_HEAP_SIZE                                0x4000
#   define VSF_SYSTIMER_FREQ                            (96000000ul)

#   undef VSF_USE_USB_HOST
#   define VSF_USE_USB_HOST                             DISABLED

#   define VSF_CFG_DEBUG_STREAM_TX_T                    vsf_mem_stream_t
#   define VSF_CFG_DEBUG_STREAM_RX_T                    vsf_mem_stream_t
#elif   defined(__WIN__)
#   define VSF_HAL_USE_DEBUG_STREAM                     ENABLED
#   define VSF_HEAP_SIZE                                0x100000
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   5

#   define VSF_SYSTIMER_FREQ                            (0ul)

#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 DISABLED
#   define VSF_USBH_USE_HUB                             DISABLED
#   define VSF_USBH_USE_HCD_WINUSB                      ENABLED
#       define VSF_WINUSB_HCD_CFG_DEV_NUM               3
#       define VSF_WINUSB_HCD_DEV0_VID                  0x0A12      // CSR8510 bthci
#       define VSF_WINUSB_HCD_DEV0_PID                  0x0001
#       define VSF_WINUSB_HCD_DEV1_VID                  0x0A5C      // BCM20702 bthci
#       define VSF_WINUSB_HCD_DEV1_PID                  0x21E8
#       define VSF_WINUSB_HCD_DEV2_VID                  0x04F2      // UVC
#       define VSF_WINUSB_HCD_DEV2_PID                  0xB130

#   define VSF_FS_USE_WINFS                             ENABLED

#   define VSF_LINUX_CFG_STACKSIZE                      8192
#   define VSF_TRACE_CFG_COLOR_EN                       ENABLED
#   define VSH_ECHO                                     1

/*----------------------------------------------------------------------------*
 * Regarget Weak interface                                                    *
 *----------------------------------------------------------------------------*/

#   define WEAK_VSF_KERNEL_ERR_REPORT_EXTERN                                    \
        extern void vsf_kernel_err_report(vsf_kernel_error_t err);
#   define WEAK_VSF_KERNEL_ERR_REPORT(__ERR)                                    \
        vsf_kernel_err_report(__ERR)

#   define WEAK___POST_VSF_KERNEL_INIT_EXTERN                                   \
        extern void __post_vsf_kernel_init(void);
#   define WEAK___POST_VSF_KERNEL_INIT()                                        \
        __post_vsf_kernel_init()

#   define WEAK_VSF_SYSTIMER_EVTHANDLER_EXTERN                                  \
        extern void vsf_systimer_evthandler(vsf_systimer_cnt_t tick);
#   define WEAK_VSF_SYSTIMER_EVTHANDLER(__TICK)                                 \
        vsf_systimer_evthandler(__TICK)

#   define WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR_EXTERN            \
        extern uint_fast32_t vsf_arch_req___systimer_resolution___from_usr(void);
#   define WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR()                 \
        vsf_arch_req___systimer_resolution___from_usr()

#   define WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR_EXTERN                  \
        extern uint_fast32_t vsf_arch_req___systimer_freq___from_usr(void);
#   define WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR()                       \
        vsf_arch_req___systimer_freq___from_usr()

#define WEAK_VSF_DRIVER_INIT_EXTERN                                             \
        bool vsf_driver_init(void);
#define WEAK_VSF_DRIVER_INIT()                                                  \
        vsf_driver_init()

#   define WEAK_VSF_HEAP_MALLOC_ALIGNED_EXTERN                                  \
        extern void * vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment);
#   define WEAK_VSF_HEAP_MALLOC_ALIGNED(__SIZE, __ALIGNMENT)                    \
        vsf_heap_malloc_aligned((__SIZE), (__ALIGNMENT))

#   define WEAK_VSF_LINUX_CREATE_FHS_EXTERN                                     \
        extern int vsf_linux_create_fhs(void);
#   define WEAK_VSF_LINUX_CREATE_FHS()                                          \
        vsf_linux_create_fhs()
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
