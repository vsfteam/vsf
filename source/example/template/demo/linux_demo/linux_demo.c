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

#if APP_CFG_USE_LINUX_DEMO == ENABLED

#define VSF_LINUX_INHERIT
#include "vsf.h"

#include "../common/usrapp_common.h"
#include "shell/sys/linux/vsf_linux.h"
#include "shell/sys/linux/port/busybox/busybox.h"
#if VSF_USE_LINUX_LIBUSB == ENABLED && APP_CFG_USE_LINUX_LIBUSB_DEMO == ENABLED
#   include <libusb.h>
#endif
#include <sys/mount.h>

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

#if VSF_USE_LINUX_LIBUSB == ENABLED && APP_CFG_USE_LINUX_LIBUSB_DEMO == ENABLED
extern int lsusb_main(int argc, char *argv[]);
#endif

#if APP_CFG_USE_NNOM_DEMO == ENABLED
extern int nnom_main(int argc, char *argv[]);
#endif

#if APP_CFG_USE_AWTK_DEMO == ENABLED
extern int awtk_main(int argc, char *argv[]);
#endif

#if APP_CFG_USE_USBH_DEMO == ENABLED
extern int usbh_main(int argc, char *argv[]);
#endif

#if APP_CFG_USE_LVGL_DEMO == ENABLED
extern int lvgl_main(int argc, char *argv[]);
#endif

#if APP_CFG_USE_TGUI_DEMO == ENABLED
extern int tgui_main(int argc, char *argv[]);
#endif

#if APP_CFG_USE_AUDIO_DEMO == ENABLED
extern int audio_play_main(int argc, char *argv[]);
#endif

#if     APP_CFG_USE_USBD_DEMO == ENABLED                                        \
    &&  (   (APP_CFG_USE_LINUX_DEMO != ENABLED)                                 \
        ||  (   (APP_CFG_USE_LINUX_DEMO == ENABLED)                             \
            &&  (USRAPP_CFG_LINUX_TTY != USRAPP_CFG_LINUX_TTY_CDC)))
int usbd_main(int argc, char *argv[]);
#endif

#if APP_CFG_USE_LINUX_MOUNT_FILE_DEMO == ENABLED
extern int mount_file_main(int argc, char *argv[]);
#endif

/*============================ IMPLEMENTATION ================================*/

int vsf_linux_create_fhs(void)
{
    int fd;

#if APP_CFG_USE_USBH_DEMO == ENABLED
    usbh_main(0, NULL);
#endif

#if VSF_USE_LINUX_BUSYBOX == ENABLED
    busybox_install();
#endif

#if VSF_USE_MAL == ENABLED && VSF_USE_FAKEFAT32_MAL == ENABLED                  \
    && VSF_USE_FS == ENABLED && VSF_USE_FATFS == ENABLED
    vk_mal_init(&usrapp_common.mal.fakefat32.use_as__vk_mal_t);
    if (mkdir("/fatfs", 0)) {
        return -1;
    }
    fd = open("/fatfs", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "fatfs", NULL, 0, &usrapp_common.mal.fakefat32.use_as__vk_mal_t);
    }
#endif

#if VSF_USE_FS == ENABLED && VSF_USE_MEMFS == ENABLED
    if (mkdir("/memfs", 0)) {
        return -1;
    }
    fd = open("/memfs", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "/memfs", &vk_memfs_op, 0, &usrapp_common.fs.memfs_info);
    }
#endif

#if VSF_USE_FS == ENABLED && VSF_USE_WINFS == ENABLED
    if (mkdir("/winfs", 0)) {
        return -1;
    }
    fd = open("/winfs", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "/winfs", &vk_winfs_op, 0, &usrapp_common.fs.winfs_info);
    }
#endif

#if APP_CFG_USE_SCSI_DEMO == ENABLED
    if (mkdir("/scsi", 0)) {
        return -1;
    }
#endif

    // install executables
#if VSF_USE_LINUX_LIBUSB == ENABLED && APP_CFG_USE_LINUX_LIBUSB_DEMO == ENABLED
    busybox_bind("/sbin/lsusb", lsusb_main);
    vsf_linux_libusb_startup();
#endif

#if APP_CFG_USE_NNOM_DEMO == ENABLED
    busybox_bind("/sbin/nnom", nnom_main);
#endif
#if APP_CFG_USE_AWTK_DEMO == ENABLED
    busybox_bind("/sbin/awtk", awtk_main);
#endif
#if APP_CFG_USE_LVGL_DEMO == ENABLED
    busybox_bind("/sbin/lvgl", lvgl_main);
#endif
#if APP_CFG_USE_TGUI_DEMO == ENABLED
    busybox_bind("/sbin/tgui", tgui_main);
#endif
#if APP_CFG_USE_AUDIO_DEMO == ENABLED
    busybox_bind("/sbin/play_audio", audio_play_main);
#endif
#if     APP_CFG_USE_USBD_DEMO == ENABLED                                        \
    &&  (   (APP_CFG_USE_LINUX_DEMO != ENABLED)                                 \
        ||  (   (APP_CFG_USE_LINUX_DEMO == ENABLED)                             \
            &&  (USRAPP_CFG_LINUX_TTY != USRAPP_CFG_LINUX_TTY_CDC)))
    busybox_bind("/sbin/usbd", usbd_main);
#endif
#if APP_CFG_USE_LINUX_MOUNT_FILE_DEMO == ENABLED
    busybox_bind("/sbin/mount_file", mount_file_main);
#endif

    return 0;
}

#if VSF_USE_USB_DEVICE == ENABLED && USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_CDC
describe_mem_stream(user_usbd_cdc_acm_stream_rx, USRAPP_CFG_CDC_RX_STREAM_SIZE)
describe_mem_stream(user_usbd_cdc_acm_stream_tx, USRAPP_CFG_CDC_TX_STREAM_SIZE)

describe_usbd(user_usbd_cdc, APP_CFG_USBD_VID, APP_CFG_USBD_PID, 0x0409, USB_DC_SPEED_HIGH)
    usbd_common_desc(user_usbd_cdc, u"VSF-USBD-Simplest", u"SimonQian", u"1.0.0", 64, USB_DESC_CDC_ACM_IAD_LEN, USB_CDC_ACM_IFS_NUM, USB_CONFIG_ATT_WAKEUP, 100)
        cdc_acm_desc(user_usbd_cdc, 0, 0, 1, 2, 2, 512, 16)
    usbd_func_desc(user_usbd_cdc)
        usbd_func_str_desc(user_usbd_cdc, 0, u"VSF-CDC")
    usbd_std_desc_table(user_usbd_cdc)
        usbd_func_str_desc_table(user_usbd_cdc, 0)
    usbd_func(user_usbd_cdc)
        cdc_acm_func(user_usbd_cdc, 0, 1, 2, 2, &user_usbd_cdc_acm_stream_rx, &user_usbd_cdc_acm_stream_tx, USB_CDC_ACM_LINECODE(115200, 8, USB_CDC_ACM_PARITY_NONE, USB_CDC_ACM_STOPBIT_1))
    usbd_ifs(user_usbd_cdc)
        cdc_acm_ifs(user_usbd_cdc, 0)
end_describe_usbd(user_usbd_cdc, USRAPP_CFG_USBD_DEV)

#elif defined(VSF_DEBUG_STREAM_NEED_POOL)
void vsf_plug_in_on_kernel_idle(void)
{
    VSF_DEBUG_STREAM_POLL();
}
#endif

// TODO: SDL require that main need argc and argv
int main(int argc, char *argv[])
{
#if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif
#endif

#if VSF_USE_USB_DEVICE == ENABLED && USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_CDC
    vsf_stream_init(&user_usbd_cdc_acm_stream_rx.use_as__vsf_stream_t);
    vsf_stream_init(&user_usbd_cdc_acm_stream_tx.use_as__vsf_stream_t);
    vk_usbd_init(&user_usbd_cdc);
    vk_usbd_connect(&user_usbd_cdc);
#elif USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_UART
    // TODO: initialize uart stream
#endif

    vsf_trace(VSF_TRACE_INFO, "start linux..." VSF_TRACE_CFG_LINEEND);

    vsf_stream_t *stream_tx, *stream_rx;
#if VSF_USE_USB_DEVICE == ENABLED && USRAPP_CFG_LINUX_TTY == USRAPP_CFG_LINUX_TTY_CDC
    stream_tx = (vsf_stream_t *)&user_usbd_cdc_acm_stream_tx.use_as__vsf_stream_t;
    stream_rx = (vsf_stream_t *)&user_usbd_cdc_acm_stream_rx.use_as__vsf_stream_t;
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

#endif      // APP_CFG_USE_LINUX_DEMO

/* EOF */
