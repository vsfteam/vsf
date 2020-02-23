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


//! \note User Level Application Configuration

#ifndef __VSF_USR_CFG_H__
#define __VSF_USR_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

// Application configure
#define APP_CFG_USE_LINUX_DEMO                          ENABLED
#   define APP_CFG_USE_LINUX_LIBUSB_DEMO                ENABLED
#   define APP_CFG_USE_LINUX_MOUNT_FILE_DEMO            ENABLED
#define APP_CFG_USE_USBH_DEMO                           ENABLED
#define APP_CFG_USE_USBD_DEMO                           ENABLED
//#   define USBD_DEMO_USE_CDC                            ENABLED
#   define USBD_DEMO_USE_MSC                            ENABLED
#define APP_CFG_USE_SCSI_DEMO                           ENABLED
#define APP_CFG_USE_AUDIO_DEMO                          ENABLED
#define APP_CFG_USE_TGUI_DEMO                           ENABLED

// 3rd-party demos
#define APP_CFG_USE_AWTK_DEMO                           ENABLED
#define APP_CFG_USE_NNOM_DEMO                           DISABLED
#define APP_CFG_USE_LVGL_DEMO                           DISABLED
#define APP_CFG_USE_BTSTACK_DEMO                        DISABLED


// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED

#define VSF_USE_USB_HOST                                ENABLED
#   define VSF_USE_USB_HOST_LIBUSB                      ENABLED
#   define VSF_USE_USB_HOST_HID                         ENABLED
#   define VSF_USE_USB_HOST_DS4                         ENABLED
#   define VSF_USE_USB_HOST_NSPRO                       ENABLED
#   define VSF_USE_USB_HOST_XB360                       ENABLED
#   define VSF_USE_USB_HOST_XB1                         ENABLED
#   define VSF_USE_USB_HOST_MSC                         ENABLED

#if APP_CFG_USE_BTSTACK_DEMO == ENABLED
#   define VSF_USE_USB_HOST_BTHCI                       ENABLED
#   define VSF_USE_BTSTACK                              ENABLED
#endif

#define VSF_USE_AUDIO                                   ENABLED
#   define VSF_USE_DECODER_WAV                          ENABLED
#   define VSF_AUDIO_CFG_USE_PLAY                       ENABLED
#   define VSF_AUDIO_CFG_USE_CATURE                     DISABLED
#   define VSF_USE_WINSOUND                             ENABLED
#       define VSF_WINSOUND_CFG_TRACE                   DISABLED

#define VSF_USE_UI                                      ENABLED
#if APP_CFG_USE_AWTK_DEMO == ENABLED
#   define VSF_USE_UI_AWTK                              ENABLED
#endif
#if APP_CFG_USE_LVGL_DEMO == ENABLED
#   define VSF_USE_UI_LVGL                              ENABLED
#endif

// UI runs in vsf_prio_0, other modules runs above vsf_prio_0
#if VSF_USE_UI == ENABLED
#   define VSF_USBH_CFG_EDA_PRIORITY                    vsf_prio_1
#   define APP_CFG_USBH_HW_PRIO                         vsf_arch_prio_1
#endif

#define VSF_USE_INPUT                                   ENABLED
#   define VSF_USE_INPUT_HID                            ENABLED
#   define VSF_USE_INPUT_DS4                            ENABLED
#   define VSF_USE_INPUT_NSPRO                          ENABLED
#   define VSF_USE_INPUT_XB360                          ENABLED
#   define VSF_USE_INPUT_XB1                            ENABLED

// VSF_USE_USB_DEVICE will be enabled if target chip supports USBD
//#define VSF_USE_USB_DEVICE                              ENABLED
#   define VSF_USBD_CFG_USE_EDA                         ENABLED
#   define VSF_USE_USB_DEVICE_CDCACM                    ENABLED
#   define VSF_USE_USB_DEVICE_MSC                       ENABLED
#   define APP_CFG_USBD_VID                             0xA7A8
#   define APP_CFG_USBD_PID                             0x2348

#define VSF_USE_MAL                                     ENABLED
#   define VSF_USE_MEM_MAL                              ENABLED
#   define VSF_USE_FAKEFAT32_MAL                        ENABLED
#   define VSF_USE_SCSI_MAL                             ENABLED
#   define VSF_USE_FILE_MAL                             ENABLED

#define VSF_USE_SCSI                                    ENABLED
#   define VSF_USE_MAL_SCSI                             ENABLED

#define VSF_USE_FS                                      ENABLED
#   define VSF_USE_MEMFS                                ENABLED
#   define VSF_USE_FATFS                                ENABLED

#define VSF_USE_TRACE                                   ENABLED

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_USE_LINUX_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_USE_LINUX_BUSYBOX                        ENABLED

#ifndef USRAPP_CFG_LINUX_TTY_DEBUT_STREAM
#   define USRAPP_CFG_LINUX_TTY_DEBUG_STREAM            0
#   define USRAPP_CFG_LINUX_TTY_UART                    1
#   define USRAPP_CFG_LINUX_TTY_CDC                     2
#endif
#   define USRAPP_CFG_LINUX_TTY                         USRAPP_CFG_LINUX_TTY_DEBUG_STREAM

#define VSF_USE_SERVICE_STREAM                          DISABLED
#define VSF_USE_SERVICE_VSFSTREAM                       ENABLED


#ifndef USRAPP_CFG_DCD_TYPE_DEFAULT
#   define USRAPP_CFG_DCD_TYPE_DEFAULT                  0
#   define USRAPP_CFG_DCD_TYPE_DWCOTG                   1
#   define USRAPP_CFG_DCD_TYPE_MUSB_FDRC                2
#endif

#define USRAPP_CFG_FAKEFAT32                            ENABLED

#if     defined(__M484__)
#   define ASSERT(...)                                  if (!(__VA_ARGS__)) {while(1);};
//#   define ASSERT(...)

#   define VSF_DEBUGGER_CFG_CONSOLE                     VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE
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
#   define VSF_USE_USB_HOST_HUB                         ENABLED
#   define VSF_USE_USB_HOST_HCD_OHCI                    ENABLED

#   define USRAPP_CFG_USBD_DEV                          VSF_USB_DC0

#   define VSF_LINUX_CFG_STACKSIZE                      2048
#   define VSF_TRACE_CFG_COLOR_EN                       DISABLED
#   define VSH_HAS_COLOR                                0
#elif   defined(__NUC505__)
#   define ASSERT(...)                                  if (!(__VA_ARGS__)) {while(1);};
//#   define ASSERT(...)

#   define VSF_DEBUGGER_CFG_CONSOLE                     VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE
#   define VSF_HEAP_SIZE                                0x4000
#   define VSF_SYSTIMER_FREQ                            (96000000ul)

#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 ENABLED
#   define VSF_USE_USB_HOST_HUB                         ENABLED
#   define VSF_USE_USB_HOST_HCD_OHCI                    ENABLED
#elif   defined(__WIN__)

// TODO: include for clang only
#   pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#   pragma clang diagnostic ignored "-Wmicrosoft-include"
#   pragma clang diagnostic ignored "-Winconsistent-dllimport"
#   pragma clang diagnostic ignored "-Wimplicit-function-declaration"

#   ifdef __CPU_X64__
#       error x64 is currently not supported
#   endif

#   define ASSERT(...)                                  assert(__VA_ARGS__)
//#   define ASSERT(...)

#   define VSF_HAL_USE_DEBUG_STREAM                     ENABLED
#   define VSF_HEAP_SIZE                                0x1000000
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   12      // 4K alignment

#   define VSF_SYSTIMER_FREQ                            (0ul)

#   define VSF_USE_USB_DEVICE                           ENABLED
#       define VSF_USE_USB_DEVICE_DCD_USBIP             ENABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG           ENABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG_TRAFFIC   DISABLED
#           define VSF_USBIP_SERVER_CFG_DEBUG_URB       ENABLED
#       define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_1
#       define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_1
#       define USRAPP_CFG_USBD_DEV                      VSF_USB_DC0

#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 DISABLED
#   define VSF_USE_USB_HOST_HUB                         DISABLED
#   define VSF_USE_USB_HOST_HCD_WINUSB                  ENABLED
#       define VSF_WINUSB_HCD_CFG_DEV_NUM               8
#       define VSF_WINUSB_HCD_DEV0_VID                  0x0A12      // CSR8510 bthci
#       define VSF_WINUSB_HCD_DEV0_PID                  0x0001
#       define VSF_WINUSB_HCD_DEV1_VID                  0x0A5C      // BCM20702 bthci
#       define VSF_WINUSB_HCD_DEV1_PID                  0x21E8
#       define VSF_WINUSB_HCD_DEV2_VID                  0x04F2      // UVC
#       define VSF_WINUSB_HCD_DEV2_PID                  0xB130
#       define VSF_WINUSB_HCD_DEV3_VID                  0x054C      // DS4
#       define VSF_WINUSB_HCD_DEV3_PID                  0x05C4
#       define VSF_WINUSB_HCD_DEV4_VID                  0x054C      // DS4
#       define VSF_WINUSB_HCD_DEV4_PID                  0x09CC
#       define VSF_WINUSB_HCD_DEV5_VID                  0x057E      // NSPRO
#       define VSF_WINUSB_HCD_DEV5_PID                  0x2009
#       define VSF_WINUSB_HCD_DEV6_VID                  0x045E      // XB360
#       define VSF_WINUSB_HCD_DEV6_PID                  0x028E
#       define VSF_WINUSB_HCD_DEV7_VID                  0x045E      // XB1
#       define VSF_WINUSB_HCD_DEV7_PID                  0x02EA
// set VSF_WINUSB_HCD_CFG_DEV_NUM to 9 and uncomment DEV8
//  to test usbh_msc/scsi/mal/fs drivers
//#       define VSF_WINUSB_HCD_DEV8_VID                  0xA7A8      // usbd_demo
//#       define VSF_WINUSB_HCD_DEV8_PID                  0x2348

#   define VSF_USE_WINFS                                ENABLED

#   define VSF_USE_DISP_DRV_SDL2                        ENABLED
#       define VSF_DISP_DRV_SDL2_CFG_MOUSE_AS_TOUCHSCREEN   ENABLED
#       define VSF_DISP_DRV_SDL2_CFG_HW_PRIORITY        vsf_arch_prio_1
#       define APP_DISP_SDL2_HEIGHT                     600
#       define APP_DISP_SDL2_WIDTH                      800
#       define APP_DISP_SDL2_COLOR                      VSF_DISP_COLOR_RGB565
#       define APP_DISP_SDL2_AMPLIFIER                  1

#   define VSF_LINUX_CFG_STACKSIZE                      32768
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

#   define WEAK_VSF_DRIVER_INIT_EXTERN                                          \
        bool vsf_driver_init(void);
#   define WEAK_VSF_DRIVER_INIT()                                               \
        vsf_driver_init()

#   define WEAK_VSF_HEAP_MALLOC_ALIGNED_EXTERN                                  \
        extern void * vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment);
#   define WEAK_VSF_HEAP_MALLOC_ALIGNED(__SIZE, __ALIGNMENT)                    \
        vsf_heap_malloc_aligned((__SIZE), (__ALIGNMENT))




#   if VSF_USE_LINUX == ENABLED
#       define WEAK_VSF_LINUX_CREATE_FHS_EXTERN                                 \
            extern int vsf_linux_create_fhs(void);
#       define WEAK_VSF_LINUX_CREATE_FHS()                                      \
            vsf_linux_create_fhs()
#   endif

#   if VSF_USE_BTSTACK == ENABLED && VSF_USE_USB_HOST_BTHCI == ENABLED
#       define WEAK_VSF_USBH_BTHCI_ON_NEW_EXTERN                                \
            extern void vsf_usbh_bthci_on_new(void *dev, vk_usbh_dev_id_t *id);
#       define WEAK_VSF_USBH_BTHCI_ON_NEW(__DEV, __ID)                          \
            vsf_usbh_bthci_on_new((__DEV), (__ID))

#       define WEAK_VSF_USBH_BTHCI_ON_DEL_EXTERN                                \
            extern void vsf_usbh_bthci_on_del(void *dev);
#       define WEAK_VSF_USBH_BTHCI_ON_DEL(__DEV)                                \
            vsf_usbh_bthci_on_del((__DEV))

#       define WEAK_VSF_USBH_BTHCI_ON_PACKET_EXTERN                             \
            extern void vsf_usbh_bthci_on_packet(void *dev, uint8_t type, uint8_t *packet, uint16_t size);
#       define WEAK_VSF_USBH_BTHCI_ON_PACKET(__DEV, __TYPE, __PACKET, __SIZE)   \
            vsf_usbh_bthci_on_packet((__DEV), (__TYPE), (__PACKET), (__SIZE))


#       define WEAK_VSF_BLUETOOTH_H2_ON_NEW_EXTERN                              \
            extern vsf_err_t vsf_bluetooth_h2_on_new(void *dev, vk_usbh_dev_id_t *id);
#       define WEAK_VSF_BLUETOOTH_H2_ON_NEW(__DEV, __ID)                        \
            vsf_bluetooth_h2_on_new((__DEV), (__ID))
#   endif

#define WEAK_VSF_SCSI_ON_NEW
#define WEAK_VSF_SCSI_ON_DELETE

#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#if APP_CFG_USE_TGUI_DEMO == ENABLED
#   include "vsf_tgui_cfg.h"
#endif

#endif
/* EOF */
