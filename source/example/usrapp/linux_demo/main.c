/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless requir by applicable law or agreed to in writing, software        *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf.h"

// define USRAPP_CF_XXXX and include usrapp_common.h
#if VSF_FS_USE_MEMFS == ENABLED
#   include "fakefat32.h"
#   define USRAPP_CFG_MEMFS_ROOT    __fakefat32_root
#endif
#if VSF_FS_USE_WINFS == ENABLED
#   define USRAPP_CFG_WINFS_ROOT    "winfs_root"
#endif
#include "../usrapp_common.h"

#define VSF_LINUX_INHERIT
#include "shell/sys/linux/vsf_linux.h"
#include "shell/sys/linux/port/busybox/busybox.h"
#if VSF_LINUX_USE_LIBUSB == ENABLED
#   include <libusb.h>
#endif
#include <sys/mount.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_LINUX_USE_LIBUSB == ENABLED
extern int lsusb_main(int argc, char *argv[]);
#endif

#if VSF_USE_USB_DEVICE == ENABLED
extern void usrapp_usbd_cdc_demo(void);
extern bool usrapp_usbd_cdc_demo_get_stream(int idx, vsf_stream_t **stream_tx, vsf_stream_t **stream_rx);
#endif

/*============================ IMPLEMENTATION ================================*/

int vsf_linux_create_fhs(void)
{
    int fd;

#if VSF_LINUX_USE_BUSYBOX == ENABLED
    busybox_install();
#endif

#if VSF_LINUX_USE_LIBUSB == ENABLED
    busybox_bind("/sbin/lsusb", lsusb_main);
    vsf_linux_libusb_startup();
#endif

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_FAKEFAT32_MAL == ENABLED              \
    && VSF_USE_FS == ENABLED && VSF_FS_USE_FATFS == ENABLED
    vk_mal_init(&__usrapp_common.mal.fakefat32.use_as__vk_mal_t);
    if (mkdir("/fatfs_fakefat32", 0)) {
        return -1;
    }
    fd = open("/fatfs_fakefat32", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "/fatfs_fakefat32", &vk_fatfs_op, 0, &__usrapp_common.fs.fatfs_info_fakefat32.use_as____vk_fatfs_info_t);
    }
#endif

#if VSF_USE_FS == ENABLED && VSF_FS_USE_MEMFS == ENABLED
    if (mkdir("/memfs", 0)) {
        return -1;
    }
    fd = open("/memfs", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "/memfs", &vk_memfs_op, 0, &__usrapp_common.fs.memfs_info);
    }
#endif

#if VSF_USE_FS == ENABLED && VSF_FS_USE_WINFS == ENABLED
    if (mkdir("/winfs", 0)) {
        return -1;
    }
    fd = open("/winfs", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "/winfs", &vk_winfs_op, 0, &__usrapp_common.fs.winfs_info);
    }
#endif
    return 0;
}

#if VSF_USE_USB_DEVICE != ENABLED && defined(VSF_DEBUG_STREAM_NEED_POLL)
void vsf_plug_in_on_kernel_idle(void)
{
    VSF_DEBUG_STREAM_POLL();
}
#endif

int main(void)
{
    __usrapp_common_init();
#if VSF_USE_USB_DEVICE == ENABLED
    usrapp_usbd_cdc_demo();
#endif

    vsf_trace(VSF_TRACE_INFO, "start linux..." VSF_TRACE_CFG_LINEEND);
    vk_fs_init();

    vsf_stream_t *stream_tx, *stream_rx;
#if VSF_USE_USB_DEVICE == ENABLED
    usrapp_usbd_cdc_demo_get_stream(0, &stream_tx, &stream_rx);
#else
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

/* EOF */
