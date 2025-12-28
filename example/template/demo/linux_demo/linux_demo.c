/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

/*============================ INCLUDES ======================================*/

#include "vsf_cfg.h"

#if APP_USE_LINUX_DEMO == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#   include "shell/sys/linux/include/fcntl.h"
#else
#   include <unistd.h>
#   include <fcntl.h>
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
/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(vsf_board_init)
void vsf_board_init(void) {}

#if APP_USE_FREETYPE_DEMO == ENABLED
int freetype_main(int argc, char *argv[])
{
    extern void freetype_demo_init(void);
    freetype_demo_init();
}
#endif

int vsf_linux_create_fhs(void)
{
    int fd;

    // 0. devfs, busybox, etc
    vsf_linux_vfs_init();
    busybox_install();

    // 1. hardware driver related demo
    vsf_board_init();
#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_DEVFS == ENABLED && VSF_HW_GPIO_PORT_MASK > 0
    vsf_linux_fs_bind_gpio_hw("/sys/class/gpio");
#endif
#if APP_USE_USBH_DEMO == ENABLED
    extern int usbh_main(int argc, char *argv[]);
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

        vsf_linux_fsdata_auto_t fsdata = {
            .mal = &usrapp_common.mal.fakefat32.use_as__vk_mal_t,
        };
        mount(NULL, "fatfs", NULL, 0, &fsdata);
    }
#   else
#       warning fat with non - 512 sector size, current driver is not supported
#   endif
#endif

#if VSF_USE_FS == ENABLED && VSF_FS_USE_MEMFS == ENABLED && VSF_MAL_USE_FAKEFAT32_MAL == ENABLED
    if (mkdir("/memfs", 0)) {
        return -1;
    }
    fd = open("/memfs", 0);
    if (fd >= 0) {
        close(fd);
        mount(NULL, "/memfs", &vk_memfs_op, 0, &usrapp_common.fs.memfs_info);
    }
#endif

#if APP_USE_SCSI_DEMO == ENABLED
    if (mkdir("/scsi", 0)) {
        return -1;
    }
#endif

    // 3. demos depends on fs after all fs mounted
#if APP_USE_FREETYPE_DEMO == ENABLED
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/freetype", freetype_main);
#endif

    // 4. install executables
#if VSF_LINUX_USE_LIBUSB == ENABLED && APP_USE_LINUX_LIBUSB_DEMO == ENABLED
    extern int lsusb_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/lsusb", lsusb_main);
    vsf_linux_libusb_startup();
#endif

#if VSF_KERNEL_CFG_CPU_USAGE == ENABLED
    extern int cpu_usage_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/cpu", cpu_usage_main);
#endif

#if APP_USE_NNOM_DEMO == ENABLED
    extern int nnom_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/nnom", nnom_main);
#endif
#if APP_USE_XBOOT_XUI_DEMO == ENABLED
    extern int xui_main(int argc, char **argv);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/xui", xui_main);
#endif
#if APP_USE_LVGL_DEMO == ENABLED
    extern int lvgl_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/lvgl", lvgl_main);
#elif APP_USE_LVGL_TERMINAL_DEMO == ENABLED
    extern int lvgl_main(int argc, char *argv[]);
    lvgl_main(0, NULL);
#endif
#if APP_USE_TGUI_DEMO == ENABLED
    extern int tgui_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/tgui", tgui_main);
#endif
#if APP_USE_GATO_DEMO == ENABLED
    extern int gato_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/gato", gato_main);
#endif
#if APP_USE_NUKLEAR_DEMO == ENABLED
    extern int nuklear_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/nuklear", nuklear_main);
#endif
#if APP_USE_LLGUI_DEMO == ENABLED
    extern int llgui_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/llgui", llgui_main);
#endif
#if APP_USE_GUILITE_DEMO == ENABLED
    extern int guilite_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/guilite", guilite_main);
#endif
#if APP_USE_AUDIO_DEMO == ENABLED
    extern int audio_play_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/play_audio", audio_play_main);
#endif
#if APP_USE_BTSTACK_DEMO == ENABLED
    extern void btstack_install(void);
    btstack_install();
#endif
#if APP_USE_LINUX_DEMO == ENABLED && APP_USE_VSFVM_DEMO == ENABLED
    extern int vsfvm_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/vsfvm", vsfvm_main);
#endif
#if APP_USE_USBD_DEMO == ENABLED
#   if APP_USE_USBD_CDC_DEMO == ENABLED
    extern int usbd_cdc_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/usbd_cdc", usbd_cdc_main);
#   endif
#   if APP_USE_USBD_UVC_DEMO == ENABLED
    extern int usbd_uvc_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/usbd_uvc", usbd_uvc_main);
#   endif
#   if APP_USE_USBD_UAC_DEMO == ENABLED
    extern int usbd_uac_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/usbd_uac", usbd_uac_main);
#   endif
#   if APP_USE_USBD_MSC_DEMO == ENABLED
    extern int usbd_msc_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/usbd_msc", usbd_msc_main);
#   endif
#   if APP_USE_USBD_USER_DEMO == ENABLED
    extern int usbd_user_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/usbd_user", usbd_user_main);
#   endif
#endif
#if APP_USE_LINUX_MOUNT_DEMO == ENABLED
    extern int mount_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/mount", mount_main);
#endif
#if APP_USE_LINUX_HTTPD_DEMO == ENABLED
    extern int httpd_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/httpd", httpd_main);
#endif
#if APP_USE_CPP_DEMO == ENABLED
    extern int cpp_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/cpp_test", cpp_main);
#endif

#if APP_USE_DISP_DEMO == ENABLED
    extern int disp_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/disp", disp_main);
#endif
#if APP_USE_SDL2_DEMO == ENABLED
    extern int sdl2_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/sdl2", sdl2_main);
#endif
#if APP_USE_VSFIP_DEMO == ENABLED && VSF_USE_VSFIP == ENABLED
    extern int vsfip_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/vsfip", vsfip_main);
#endif
#if APP_USE_LWIP_DEMO == ENABLED && VSF_USE_LWIP == ENABLED
    extern int lwip_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/lwip", lwip_main);
#endif
#if APP_USE_KERNEL_TEST == ENABLED
    extern int kernel_eda_test_main(int argc, char *argv[]);
    extern int kernel_fsm_test_main(int argc, char *argv[]);
    extern int kernel_sem_test_main(int argc, char *argv[]);
    extern int kernel_cross_task_test_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/eda_test", kernel_eda_test_main);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/fsm_test", kernel_fsm_test_main);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/sem_test", kernel_sem_test_main);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/cross_task_test", kernel_cross_task_test_main);
#endif
#if APP_USE_JSON_DEMO == ENABLED
    extern int json_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/json", json_main);
#endif
#if APP_USE_HAL_DEMO == ENABLED
#   if APP_USE_HAL_ADC_DEMO == ENABLED && VSF_HAL_USE_ADC == ENABLED
    extern int adc_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/adc-test", adc_main);
#   endif
#   if APP_USE_HAL_DMA_DEMO == ENABLED && VSF_HAL_USE_DMA == ENABLED
    extern int dma_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/dma-test", dma_main);
#   endif
#   if APP_USE_HAL_FLASH_DEMO == ENABLED && VSF_HAL_USE_FLASH == ENABLED
    extern int flash_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/flash-test", flash_main);
#   endif
#   if APP_USE_HAL_GPIO_DEMO == ENABLED && VSF_HAL_USE_GPIO == ENABLED
    extern int gpio_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/gpio-test", gpio_main);
#   endif
#   if APP_USE_HAL_I2C_DEMO == ENABLED && VSF_HAL_USE_GPIO == ENABLED
    extern int i2c_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/i2c-test", i2c_main);
#   endif
#   if APP_USE_HAL_IO_DEMO == ENABLED && VSF_HAL_USE_GPIO == ENABLED
    extern int io_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/io-test", io_main);
#   endif
#   if APP_USE_HAL_PM_DEMO == ENABLED && VSF_HAL_USE_GPIO == ENABLED
    extern int pm_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/pm-test", pm_main);
#   endif
#   if APP_USE_HAL_PWM_DEMO == ENABLED && VSF_HAL_USE_PWM == ENABLED
    extern int pwm_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/pwm-test", pwm_main);
#   endif
#   if APP_USE_HAL_RNG_DEMO == ENABLED && VSF_HAL_USE_GPIO == ENABLED
    extern int rng_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/rng-test", rng_main);
#   endif
#   if APP_USE_HAL_RTC_DEMO == ENABLED && VSF_HAL_USE_GPIO == ENABLED
    extern int rtc_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/rtc-test", rtc_main);
#   endif
#   if APP_USE_HAL_SPI_DEMO == ENABLED && VSF_HAL_USE_GPIO == ENABLED
    extern int spi_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/spi-test", spi_main);
#   endif
#   if APP_USE_HAL_TIMER_DEMO == ENABLED && VSF_HAL_USE_TIMER == ENABLED
    extern int timer_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/timer-test", timer_main);
#   endif
#   if APP_USE_HAL_USART_DEMO == ENABLED && VSF_HAL_USE_USART == ENABLED
    extern int usart_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/usart-test", usart_main);
#   endif
#   if APP_USE_HAL_WDT_DEMO == ENABLED && VSF_HAL_USE_WDT == ENABLED
    extern int wdt_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/wdt-test", wdt_main);
#   endif

#endif
#if APP_USE_STREAM_HAL_DEMO == ENABLED
#   if APP_USE_STREAM_USART_DEMO == ENABLED
    extern int stream_usart_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/stream_usart", stream_usart_main);
#   endif
#endif
#if APP_USE_SOCKET_DEMO == ENABLED && VSF_USE_TCPIP == ENABLED
    extern int socket_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/socket", socket_main);
#endif
#if APP_USE_EVM_DEMO == ENABLED
    extern int evm_demo_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/evm", evm_demo_main);
#   if VSF_EVM_USE_LUA == ENABLED
    extern int evm_demo_luat_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/evm_lua", evm_demo_luat_main);
#   endif
#endif
#if APP_USE_LUA_DEMO == ENABLED
#   if VSF_EVM_USE_LUA != ENABLED
    extern int lua_main(int argc, char* argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/lua", lua_main);
#   endif
    extern int love_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/love", love_main);
#endif
#if APP_USE_COREMARK_DEMO == ENABLED
    extern int coremark_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/coremark", coremark_main);
#endif
#if APP_USE_DISTBUS_DEMO == ENABLED
    extern int distbus_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/distbus", distbus_main);
#endif
#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_ELF == ENABLED && APP_USE_LINUX_DYNLOADER_DEMO == ENABLED
    extern int dynloader_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/load", dynloader_main);
#endif

    // call vsf_arch_cpp_startup only after dependency of cpp initializer is ready
#ifdef __VSF_CPP__
    vsf_arch_cpp_startup();
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
#   if APP_USE_VSFVM_DEMO == ENABLED
    extern void vsfvm_user_poll(void);
    vsfvm_user_poll();
#   endif
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
    vsf_trace_info("start linux..." VSF_TRACE_CFG_LINEEND);

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
    vsf_err_t err = vsf_linux_init(&stream);
    VSF_UNUSED_PARAM(err);
    VSF_ASSERT(VSF_ERR_NONE == err);
    return 0;
}

#endif      // APP_USE_LINUX_DEMO

/* EOF */
