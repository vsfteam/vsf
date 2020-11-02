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

#define VSF_USE_HEAP                    ENABLED
#define VSF_HEAP_SIZE                   0x800
#define VSF_HEAP_CFG_MCB_MAGIC_EN       ENABLED

#define VSF_SYSTIMER_FREQ               (192000000ul)

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
//      </h>

#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                         DISABLED

//      <o>The default eda stack frame pool size <1-65535>
//      <i>The default eda stack frame pool is shared among all eda tasks.
//#define VSF_OS_CFG_DEFAULT_TASK_FRAME_POOL_SIZE         16
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
#define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL         ENABLED
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

#define VSF_USE_FS                          ENABLED
#   define VSF_FS_USE_MEMFS                 ENABLED
#   define VSF_FS_USE_FATFS                 ENABLED

#define VSF_USE_INPUT                       DISABLED
#define VSF_INPUT_USE_HID                   ENABLED
#define VSF_INPUT_USE_DS4                   ENABLED

#define VSF_USE_AV                          DISABLED

#define VSF_USE_MAL                         ENABLED
#   define VSF_MAL_USE_MEM_MAL              ENABLED
#   define VSF_MAL_USE_FAKEFAT32_MAL        ENABLED
#define VSF_USE_SCSI                        ENABLED
#   define VSF_SCSI_USE_MAL_SCSI            ENABLED

#define VSF_USE_USB_HOST                    DISABLED
#define VSF_USBH_USE_HUB                    ENABLED
#define VSF_USBH_USE_ECM                    ENABLED
#define VSF_USBH_USE_HID                    ENABLED
#define VSF_USBH_USE_BTHCI                  ENABLED
#define VSF_USBH_USE_DS4                    ENABLED
#define VSF_USBH_USE_HCD_OHCI               ENABLED
//#define VSF_USBH_USE_HCD_MUSB_FDRC          ENABLED
#define VSF_USBH_CFG_EDA_PRIORITY           vsf_prio_8

#define VSF_USE_USB_DEVICE                  ENABLED
#define VSF_USBD_CFG_USE_EDA                ENABLED
#   define VSF_USBD_USE_MSC                 ENABLED
#   define VSF_USBD_USE_CDCACM              DISABLED
#   define VSF_USBD_USE_UVC                 DISABLED
#       define VSF_USBD_UVC_CFG_TRACE_EN    ENABLED

#define VSF_USE_TCPIP                       DISABLED

#define VSF_USE_UI                          DISABLED
#   define VSF_USE_LVGL                     ENABLED
#define VSF_USE_DISP_DRV_USBD_UVC           ENABLED

#define VSF_USE_PBUF                        ENABLED
#   define VSF_PBUF_CFG_INDIRECT_RW_SUPPORT DISABLED


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

#ifndef USRAPP_CFG_DCD_TYPE_DEFAULT
#   define USRAPP_CFG_DCD_TYPE_DEFAULT      0
#   define USRAPP_CFG_DCD_TYPE_DWCOTG       1
#   define USRAPP_CFG_DCD_TYPE_MUSB_FDRC    2
#endif

#include "component/usb/common/usb_common.h"
#if     defined(__GD32VF103__)
#   define VSF_USE_TRACE                    DISABLED

#   define VSF_OS_CFG_PRIORITY_NUM          1

#   define VSF_USBD_USE_DCD_DWCOTG          ENABLED
#   define VSF_USBD_CFG_EDA_PRIORITY        vsf_prio_0
#   define VSF_USBD_CFG_HW_PRIORITY         vsf_arch_prio_0

#   define USRAPP_CFG_USBD_SPEED            USB_SPEED_FULL
#   define USRAPP_CFG_CDC_NUM               1
#   define USRAPP_CFG_DCD_TYPE              USRAPP_CFG_DCD_TYPE_DWCOTG
#elif   defined(__M484__)
#   define VSF_USE_TRACE                    ENABLED
#   define VSF_DEBUGGER_CFG_CONSOLE         VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE

#   define VSF_OS_CFG_PRIORITY_NUM          10

#   define VSF_USBD_CFG_EDA_PRIORITY        vsf_prio_9
#   define VSF_USBD_CFG_HW_PRIORITY         vsf_arch_prio_9

#   define USRAPP_CFG_USBD_SPEED            USB_SPEED_HIGH
#   define USRAPP_CFG_CDC_NUM               3
#   define USRAPP_CFG_DCD_TYPE              USRAPP_CFG_DCD_TYPE_DEFAULT
#   define USRAPP_CFG_MSC_SIZE              (128 * 1024)
#else
#   define VSF_USBD_CFG_EDA_PRIORITY        vsf_prio_0
#   define VSF_USBD_CFG_HW_PRIORITY         vsf_arch_prio_0

#   define USRAPP_CFG_USBD_SPEED            USB_SPEED_FULL
#   define USRAPP_CFG_DCD_TYPE              USRAPP_CFG_DCD_TYPE_DEFAULT
#endif

#if USRAPP_CFG_DCD_TYPE == USRAPP_CFG_DCD_TYPE_DWCOTG
#   define VSF_DWCOTG_DCD_CFG_AUTO_BUFFER_INIT  ENABLED
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
