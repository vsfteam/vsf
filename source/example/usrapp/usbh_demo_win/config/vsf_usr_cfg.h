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

#define VSF_HEAP_CFG_MCB_MAGIC_EN       ENABLED
#define VSF_HEAP_SIZE                   0x10000

#define VSF_SYSTIMER_FREQ               (192000000ul)

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
#define VSF_OS_CFG_PRIORITY_NUM                 1
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

#define VSF_USE_USB_HOST                                ENABLED
#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 DISABLED
#define VSF_USBH_USE_HCD_WINUSB                         ENABLED
#   define VSF_WINUSB_HCD_CFG_REMOVE_ON_ERROR           ENABLED
#   define VSF_WINUSB_HCD_CFG_DEV_NUM                   7
#   define VSF_WINUSB_HCD_DEV0_VID                      0x0A12      // CSR8510 bthci
#   define VSF_WINUSB_HCD_DEV0_PID                      0x0001
#   define VSF_WINUSB_HCD_DEV1_VID                      0x0A5C      // BCM20702A0 bthci
#   define VSF_WINUSB_HCD_DEV1_PID                      0x21E8
#   define VSF_WINUSB_HCD_DEV2_VID                      0x054C      // DS4
#   define VSF_WINUSB_HCD_DEV2_PID                      0x05C4
#   define VSF_WINUSB_HCD_DEV3_VID                      0x054C      // DS4
#   define VSF_WINUSB_HCD_DEV3_PID                      0x09CC
#   define VSF_WINUSB_HCD_DEV4_VID                      0x057E      // NSPRO
#   define VSF_WINUSB_HCD_DEV4_PID                      0x2009
#   define VSF_WINUSB_HCD_DEV5_VID                      0x045E      // XB360
#   define VSF_WINUSB_HCD_DEV5_PID                      0x028E
#   define VSF_WINUSB_HCD_DEV6_VID                      0x045E      // XB1
#   define VSF_WINUSB_HCD_DEV6_PID                      0x02EA
#define VSF_USBH_USE_HUB                                DISABLED
#define VSF_USBH_USE_ECM                                DISABLED
#define VSF_USBH_USE_BTHCI                              ENABLED
#define VSF_USBH_USE_HID                                ENABLED
#define VSF_USBH_USE_DS4                                ENABLED
#define VSF_USBH_USE_NSPRO                              ENABLED
#define VSF_USBH_USE_XB360                              ENABLED
#define VSF_USBH_USE_XB1                                ENABLED

#define VSF_USE_INPUT                                   ENABLED
#define VSF_INPUT_USE_HID                               ENABLED
#define VSF_INPUT_USE_DS4                               ENABLED
#define VSF_INPUT_USE_NSPRO                             ENABLED
#define VSF_INPUT_USE_XB360                             ENABLED
#define VSF_INPUT_USE_XB1                               ENABLED

#define VSF_USE_TRACE                                   ENABLED
#define VSF_TRACE_CFG_COLOR_EN                          ENABLED
#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED

#define VSF_USE_UI                                      ENABLED
#   define VSF_USE_LVGL                                 ENABLED
#   define VSF_DISP_USE_SDL2                            ENABLED
#       define VSF_DISP_SDL2_CFG_MOUSE_AS_TOUCHSCREEN   ENABLED

#define VSF_USE_BTSTACK                                 ENABLED





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


/*----------------------------------------------------------------------------*
 * Regarget Weak interface                                                    *
 *----------------------------------------------------------------------------*/

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

#define WEAK_VSF_DRIVER_INIT_EXTERN                                             \
        bool vsf_driver_init(void);
#define WEAK_VSF_DRIVER_INIT()                                                  \
        vsf_driver_init()




#define WEAK_VSF_INPUT_ON_EVT_EXTERN                                            \
        extern void vsf_input_on_evt(vk_input_type_t type, vk_input_evt_t *evt);
#define WEAK_VSF_INPUT_ON_EVT(__TYPE, __EVT)                                    \
        vsf_input_on_evt((__TYPE), (__EVT))

#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED
#   define WEAK_VSF_INPUT_ON_TOUCHSCREEN_EXTERN                                 \
        extern void vsf_input_on_touchscreen(vk_touchscreen_evt_t *ts_evt);
#   define WEAK_VSF_INPUT_ON_TOUCHSCREEN(__TS_EVT)                              \
        vsf_input_on_touchscreen((__TS_EVT))

#   define WEAK_VSF_INPUT_ON_GAMEPAD_EXTERN                                     \
        extern void vsf_input_on_gamepad(vk_gamepad_evt_t *gamepad_evt);
#   define WEAK_VSF_INPUT_ON_GAMEPAD(__GAMEPAD_EVT)                             \
        vsf_input_on_gamepad((__GAMEPAD_EVT))
#endif



#define WEAK_VSF_HEAP_MALLOC_ALIGNED_EXTERN                                     \
        extern void * vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment);
#define WEAK_VSF_HEAP_MALLOC_ALIGNED(__SIZE, __ALIGNMENT)                       \
        vsf_heap_malloc_aligned((__SIZE), (__ALIGNMENT))




#define WEAK_VSF_USBH_BTHCI_ON_NEW_EXTERN                                       \
        extern void vsf_usbh_bthci_on_new(void *dev, vk_usbh_dev_id_t *id);
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
        extern vsf_err_t vsf_bluetooth_h2_on_new(void *dev, vk_usbh_dev_id_t *id);
#define WEAK_VSF_BLUETOOTH_H2_ON_NEW(__DEV, __ID)                               \
        vsf_bluetooth_h2_on_new((__DEV), (__ID))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
