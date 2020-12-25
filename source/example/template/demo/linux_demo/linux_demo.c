/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless requir by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf_cfg.h"

#if APP_USE_LINUX_DEMO == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#else
#   include <unistd.h>
#endif

#include "../common/usrapp_common.h"

#if VSF_LINUX_USE_LIBUSB == ENABLED && APP_USE_LINUX_LIBUSB_DEMO == ENABLED
#   if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#       include "shell/sys/linux/include/libusb.h"
#   else
#       include <libusb.h>
#   endif
#endif

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/sys/mount.h"
#else
#   include <sys/mount.h>
#endif

#if VSF_USE_SDL2 == ENABLED
#   include <SDL.h>
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_USB_DEVICE == ENABLED
#   ifndef USRAPP_CFG_CDC_TX_STREAM_SIZE
#       define USRAPP_CFG_CDC_TX_STREAM_SIZE            1024
#   endif

#   ifndef USRAPP_CFG_CDC_RX_STREAM_SIZE
#       define USRAPP_CFG_CDC_RX_STREAM_SIZE            1024
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_LINUX_USE_LIBUSB == ENABLED && APP_USE_LINUX_LIBUSB_DEMO == ENABLED
extern int lsusb_main(int argc, char *argv[]);
#endif

#if APP_USE_NNOM_DEMO == ENABLED
extern int nnom_main(int argc, char *argv[]);
#endif

#if APP_USE_AWTK_DEMO == ENABLED
extern int awtk_main(int argc, char *argv[]);
#endif

#if APP_USE_USBH_DEMO == ENABLED
extern int usbh_main(int argc, char *argv[]);
#endif

#if APP_USE_VSFIP_DEMO == ENABLED && VSF_USE_VSFIP == ENABLED
extern int vsfip_main(int argc, char *argv[]);
#endif

#if APP_USE_LWIP_DEMO == ENABLED && VSF_USE_LWIP == ENABLED
extern int lwip_main(int argc, char *argv[]);
#endif

#if APP_USE_LVGL_DEMO == ENABLED
extern int lvgl_main(int argc, char *argv[]);
#endif

#if APP_USE_TGUI_DEMO == ENABLED
extern int tgui_main(int argc, char *argv[]);
#endif

#if APP_USE_GATO_DEMO == ENABLED
extern int gato_main(int argc, char *argv[]);
#endif

#if APP_USE_NUKLEAR_DEMO == ENABLED
extern int nuklear_main(int argc, char *argv[]);
#endif

#if APP_USE_AUDIO_DEMO == ENABLED
extern int audio_play_main(int argc, char *argv[]);
#endif

#if APP_USE_BTSTACK_DEMO == ENABLED
extern int btstack_scan_main(int argc, char *argv[]);
#endif

#if APP_USE_USBD_DEMO == ENABLED

#   if APP_USE_USBD_CDC_DEMO == ENABLED
extern int usbd_cdc_main(int argc, char *argv[]);
#   endif
#   if APP_USE_USBD_UVC_DEMO == ENABLED
extern int usbd_uvc_main(int argc, char *argv[]);
#   endif
#   if APP_USE_USBD_UAC_DEMO == ENABLED
extern int usbd_uac_main(int argc, char *argv[]);
#   endif
#   if APP_USE_USBD_MSC_DEMO == ENABLED
extern int usbd_msc_main(int argc, char *argv[]);
#   endif
#   if APP_USE_USBD_USER_DEMO == ENABLED
extern int usbd_user_main(int argc, char *argv[]);
#   endif
#endif

#if APP_USE_LINUX_MOUNT_FILE_DEMO == ENABLED
extern int mount_file_main(int argc, char *argv[]);
#endif

#if APP_USE_LINUX_DEMO == ENABLED && APP_USE_VSFVM_DEMO == ENABLED
extern int vsfvm_main(int argc, char *argv[]);
#endif

#if APP_USE_SDL2_DEMO == ENABLED
extern int sdl2_main(int argc, char *argv[]);
#endif

#if APP_USE_CPP_DEMO == ENABLED
extern int cpp_main(int argc, char *argv[]);
#endif

#if APP_USE_XBOOT_XUI_DEMO == ENABLED
extern int xui_main(int argc, char **argv);
#endif

#if APP_USE_FREETYPE_DEMO == ENABLED
extern void freetype_demo_init(void);
#endif

#if APP_USE_KERNEL_TEST == ENABLED
extern int kernel_sem_test_main(int argc, char *argv[]);
#endif

#if APP_USE_JSON_DEMO == ENABLED
extern int json_main(int argc, char *argv[]);
#endif

#if APP_USE_HAL_DEMO == ENABLED
#   if APP_USE_HAL_USART_DEMO == ENABLED && VSF_HAL_USE_USART == ENABLED
extern int usart_main(int argc, char *argv[]);
#   endif
#endif

#if APP_USE_STREAM_HAL_DEMO == ENABLED
#   if APP_USE_STREAM_USART_DEMO == ENABLED
extern int stream_usart_main(int argc, char *argv[]);
#   endif
#endif

#if APP_USE_SOCKET_DEMO == ENABLED && VSF_USE_TCPIP == ENABLED
extern int socket_main(int argc, char *argv[]);
#endif

#if APP_USE_EVM_DEMO == ENABLED
extern int evm_demo(int argc, char *argv[]);
#endif

/*============================ IMPLEMENTATION ================================*/

int vsf_linux_create_fhs(void)
{
    int fd;

    busybox_install();

    // 1. hardware driver related demo
#if APP_USE_USBH_DEMO == ENABLED
    usbh_main(0, NULL);
#endif

    // 2. fs
#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_FAKEFAT32_MAL == ENABLED              \
    && VSF_USE_FS == ENABLED && VSF_FS_USE_FATFS == ENABLED
    vk_mal_init(&usrapp_common.mal.fakefat32.use_as__vk_mal_t);

#   if USRAPP_CFG_FAKEFAT32_SECTOR_SIZE == 512
    if (mkdir("/fatfs", 0)) {
        return -1;
    }
    fd = open("/fatfs", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "fatfs", NULL, 0, &usrapp_common.mal.fakefat32.use_as__vk_mal_t);
    }
#   else
    #       warning fat with non - 512 sector size, current driver is not supported
#   endif
#endif

#if VSF_USE_FS == ENABLED && VSF_FS_USE_MEMFS == ENABLED
    if (mkdir("/memfs", 0)) {
        return -1;
    }
    fd = open("/memfs", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "/memfs", &vk_memfs_op, 0, &usrapp_common.fs.memfs_info);
    }
#endif

#if VSF_USE_FS == ENABLED && VSF_FS_USE_WINFS == ENABLED
    if (mkdir("/winfs", 0)) {
        return -1;
    }
    fd = open("/winfs", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "/winfs", &vk_winfs_op, 0, &usrapp_common.fs.winfs_info);
    }
#endif

#if APP_USE_SCSI_DEMO == ENABLED
    if (mkdir("/scsi", 0)) {
        return -1;
    }
#endif

    // 3. demos depends on fs after all fs mounted
#if APP_USE_FREETYPE_DEMO == ENABLED
    freetype_demo_init();
#endif

    // 4. install executables
#if VSF_LINUX_USE_LIBUSB == ENABLED && APP_USE_LINUX_LIBUSB_DEMO == ENABLED
    busybox_bind("/sbin/lsusb", lsusb_main);
    vsf_linux_libusb_startup();
#endif

#if APP_USE_NNOM_DEMO == ENABLED
    busybox_bind("/sbin/nnom", nnom_main);
#endif
#if APP_USE_XBOOT_XUI_DEMO == ENABLED
    busybox_bind("/sbin/xui", xui_main);
#endif
#if APP_USE_AWTK_DEMO == ENABLED
    busybox_bind("/sbin/awtk", awtk_main);
#endif
#if APP_USE_LVGL_DEMO == ENABLED
    busybox_bind("/sbin/lvgl", lvgl_main);
#endif
#if APP_USE_TGUI_DEMO == ENABLED
    busybox_bind("/sbin/tgui", tgui_main);
#endif
#if APP_USE_GATO_DEMO == ENABLED
    busybox_bind("/sbin/gato", gato_main);
#endif
#if APP_USE_NUKLEAR_DEMO == ENABLED
    busybox_bind("/sbin/nuklear", nuklear_main);
#endif
#if APP_USE_AUDIO_DEMO == ENABLED
    busybox_bind("/sbin/play_audio", audio_play_main);
#endif
#if APP_USE_BTSTACK_DEMO == ENABLED
    busybox_bind("/sbin/btscan", btstack_scan_main);
#endif
#if APP_USE_LINUX_DEMO == ENABLED && APP_USE_VSFVM_DEMO == ENABLED
    busybox_bind("/sbin/vsfvm", vsfvm_main);
#endif
#if APP_USE_USBD_DEMO == ENABLED
#   if APP_USE_USBD_CDC_DEMO == ENABLED
    busybox_bind("/sbin/usbd_cdc", usbd_cdc_main);
#   endif
#   if APP_USE_USBD_UVC_DEMO == ENABLED
    busybox_bind("/sbin/usbd_uvc", usbd_uvc_main);
#   endif
#   if APP_USE_USBD_UAC_DEMO == ENABLED
    busybox_bind("/sbin/usbd_uac", usbd_uac_main);
#   endif
#   if APP_USE_USBD_MSC_DEMO == ENABLED
    busybox_bind("/sbin/usbd_msc", usbd_msc_main);
#   endif
#   if APP_USE_USBD_USER_DEMO == ENABLED
    busybox_bind("/sbin/usbd_user", usbd_user_main);
#   endif
#endif
#if APP_USE_LINUX_MOUNT_FILE_DEMO == ENABLED
    busybox_bind("/sbin/mount_file", mount_file_main);
#endif
#if APP_USE_CPP_DEMO == ENABLED
    busybox_bind("/sbin/cpp_test", cpp_main);
#endif

#if VSF_USE_SDL2 == ENABLED
    vsf_sdl2_init(&usrapp_ui_common.disp.use_as__vk_disp_t);
#endif
#if APP_USE_SDL2_DEMO == ENABLED
    busybox_bind("/sbin/sdl2", sdl2_main);
#endif
#if APP_USE_VSFIP_DEMO == ENABLED && VSF_USE_VSFIP == ENABLED
    busybox_bind("/sbin/vsfip", vsfip_main);
#endif
#if APP_USE_LWIP_DEMO == ENABLED && VSF_USE_LWIP == ENABLED
    busybox_bind("/sbin/lwip", lwip_main);
#endif
#if APP_USE_KERNEL_TEST == ENABLED
    busybox_bind("/sbin/sem_test", kernel_sem_test_main);
#endif
#if APP_USE_JSON_DEMO == ENABLED
    busybox_bind("/sbin/json", json_main);
#endif
#if APP_USE_HAL_DEMO == ENABLED
#   if APP_USE_HAL_USART_DEMO == ENABLED && VSF_HAL_USE_USART == ENABLED
    busybox_bind("/sbin/usart", usart_main);
#   endif
#endif
#if APP_USE_STREAM_HAL_DEMO == ENABLED
#   if APP_USE_STREAM_USART_DEMO == ENABLED
    busybox_bind("/sbin/stream_usart", stream_usart_main);
#   endif
#endif
#if APP_USE_SOCKET_DEMO == ENABLED && VSF_USE_TCPIP == ENABLED
    busybox_bind("/sbin/socket", socket_main);
#endif
#if APP_USE_EVM_DEMO == ENABLED
    busybox_bind("/sbin/evm", evm_demo);
#endif

    return 0;
}

#if VSF_USE_USB_DEVICE == ENABLED && USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_CDC

// __APP_CFG_CDC_BULK_SIZE is for internal usage
#if USRAPP_CFG_USBD_SPEED == USB_DC_SPEED_HIGH
#   define __APP_CFG_CDC_BULK_SIZE          512
#else
#   define __APP_CFG_CDC_BULK_SIZE          64
#endif

describe_mem_stream(__user_usbd_cdc_acm_stream_rx, USRAPP_CFG_CDC_RX_STREAM_SIZE)
describe_mem_stream(__user_usbd_cdc_acm_stream_tx, USRAPP_CFG_CDC_TX_STREAM_SIZE)

describe_usbd(__user_usbd_cdc, APP_CFG_USBD_VID, APP_CFG_USBD_PID, USB_DC_SPEED_HIGH)
    usbd_common_desc(__user_usbd_cdc, u"VSF-USBD-Simplest", u"SimonQian", u"1.0.0", 64, USB_DESC_CDC_ACM_IAD_LEN, USB_CDC_ACM_IFS_NUM, USB_CONFIG_ATT_WAKEUP, 100)
        cdc_acm_desc(__user_usbd_cdc, 0, 0, 1, 2, 2, __APP_CFG_CDC_BULK_SIZE, 16)
    usbd_func_desc(__user_usbd_cdc)
        usbd_func_str_desc(__user_usbd_cdc, 0, u"VSF-CDC")
    usbd_std_desc_table(__user_usbd_cdc)
        usbd_func_str_desc_table(__user_usbd_cdc, 0)
    usbd_func(__user_usbd_cdc)
        cdc_acm_func(__user_usbd_cdc, 0, 1, 2, 2, &__user_usbd_cdc_acm_stream_rx, &__user_usbd_cdc_acm_stream_tx, USB_CDC_ACM_LINECODE(115200, 8, USB_CDC_ACM_PARITY_NONE, USB_CDC_ACM_STOPBIT_1))
    usbd_ifs(__user_usbd_cdc)
        cdc_acm_ifs(__user_usbd_cdc, 0)
end_describe_usbd(__user_usbd_cdc, USRAPP_CFG_USBD_DEV)

void vsf_plug_in_on_kernel_idle(void)
{
#   if APP_USE_VSFVM_DEMO == ENABLED
    extern void vsfvm_user_poll(void);
    vsfvm_user_poll();
#   endif
    vsf_driver_poll();
}

#elif defined(VSF_DEBUG_STREAM_NEED_POLL)

void vsf_plug_in_on_kernel_idle(void)
{
#   if APP_USE_VSFVM_DEMO == ENABLED
    extern void vsfvm_user_poll(void);
    vsfvm_user_poll();
#   endif
    VSF_DEBUG_STREAM_POLL();
    vsf_driver_poll();
}

#elif APP_USE_VSFVM_DEMO == ENABLED

void vsf_plug_in_on_kernel_idle(void)
{
    extern void vsfvm_user_poll(void);
    vsfvm_user_poll();
    vsf_driver_poll();
}

#endif

// TODO: SDL require that main need argc and argv
int VSF_USER_ENTRY(int argc, char *argv[])
{
#if VSF_USE_USB_DEVICE == ENABLED && USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_CDC
    vsf_stream_init(&__user_usbd_cdc_acm_stream_rx.use_as__vsf_stream_t);
    vsf_stream_init(&__user_usbd_cdc_acm_stream_tx.use_as__vsf_stream_t);
    vk_usbd_init(&__user_usbd_cdc);
    vk_usbd_connect(&__user_usbd_cdc);
#elif USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_UART
    // TODO: initialize uart stream
#elif USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_DEBUG_STREAM
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_RX);
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_TX);
#endif

#if VSF_USE_TRACE == ENABLED
#   if VSF_USE_USB_DEVICE == ENABLED && USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_CDC
        vsf_start_trace(&__user_usbd_cdc_acm_stream_tx.use_as__vsf_stream_t);
#   else
        vsf_start_trace();
#   endif

#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif
#endif
    vsf_trace(VSF_TRACE_INFO, "start linux..." VSF_TRACE_CFG_LINEEND);

    vsf_stream_t *stream_tx, *stream_rx;
#if VSF_USE_USB_DEVICE == ENABLED && USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_CDC
    stream_tx = (vsf_stream_t *)&__user_usbd_cdc_acm_stream_tx.use_as__vsf_stream_t;
    stream_rx = (vsf_stream_t *)&__user_usbd_cdc_acm_stream_rx.use_as__vsf_stream_t;
#elif USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_UART
    // TODO:
    stream_tx = (vsf_stream_t *)&UART_STREAM_TX;
    stream_rx = (vsf_stream_t *)&UART_STREAM_RX;
#elif USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_DEBUG_STREAM
    stream_tx = (vsf_stream_t *)&VSF_DEBUG_STREAM_TX;
    stream_rx = (vsf_stream_t *)&VSF_DEBUG_STREAM_RX;
#endif

    vsf_linux_stdio_stream_t stream = {
        .in     = stream_rx,
        .out    = stream_tx,
        .err    = stream_tx,
    };
    vsf_linux_init(&stream);
    return 0;
}

#endif      // APP_USE_LINUX_DEMO

/* EOF */
