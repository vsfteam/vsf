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

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_DEVFS == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_FS_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#   include "shell/sys/linux/include/poll.h"
#   include "shell/sys/linux/include/termios.h"
#   include "shell/sys/linux/include/sys/stat.h"
#else
#   include <unistd.h>
#   include <poll.h>
#   include <termios.h>
#   include <sys/stat.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "shell/sys/linux/include/simple_libc/stdio.h"
#else
#   include <stdio.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern int __vsf_linux_create_open_path(char *path);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_LINUX_DEVFS_USE_RAND == ENABLED
__vsf_component_peda_ifs_entry(__vk_devfs_rand_read, vk_file_read)
{
    vsf_peda_begin();

    uint_fast32_t size = vsf_local.size;
    uint8_t *buff = vsf_local.buff;

    // TODO: use hal trng instead of rand
    for (uint_fast32_t i = 0; i < size; i++) {
        buff[i] = (uint8_t)rand();
    }
    vsf_eda_return(size);
    vsf_peda_end();
}

int vsf_linux_fs_bind_rand(char *path)
{
    int err = vsf_linux_fs_bind_target_ex(path, NULL, NULL,
            (vsf_peda_evthandler_t)vsf_peda_func(__vk_devfs_rand_read), NULL,
            VSF_FILE_ATTR_READ, (uint64_t)-1);
    if (!err) {
        printf("%s bound.\r\n", path);
    }
    return err;
}
#endif

#if VSF_USE_MAL == ENABLED
#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif
__vsf_component_peda_ifs_entry(__vk_devfs_mal_read, vk_file_read)
{
    vsf_peda_begin();

    // GCC: -Wcast-align
    vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)&vsf_this;
    vk_mal_t *mal = vfs_file->f.param;

    switch (evt) {
    case VSF_EVT_INIT:
        vk_mal_read(mal, vfs_file->pos, vsf_local.size, vsf_local.buff);
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }

    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_devfs_mal_write, vk_file_write)
{
    vsf_peda_begin();

    // GCC: -Wcast-align
    vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)&vsf_this;
    vk_mal_t *mal = vfs_file->f.param;

    switch (evt) {
    case VSF_EVT_INIT:
        vk_mal_write(mal, vfs_file->pos, vsf_local.size, vsf_local.buff);
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }

    vsf_peda_end();
}
#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

int vsf_linux_fd_bind_mal(char *path, vk_mal_t *mal)
{
    return vsf_linux_fs_bind_target_ex(path, mal, NULL,
                (vsf_peda_evthandler_t)vsf_peda_func(__vk_devfs_mal_read),
                (vsf_peda_evthandler_t)vsf_peda_func(__vk_devfs_mal_write),
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE | VSF_FILE_ATTR_BLK, mal->size);
}
#endif

#if 0
//#if VSF_HAL_USE_USART == ENABLED

#   if VSF_USE_SIMPLE_STREAM != ENABLED
#       error VSF_USE_SIMPLE_STREAM MUST be enabled to support uart dev
#   endif
#   ifndef VSF_LINUX_DEVFS_UART_CFG_RX_BUFSIZE
#       define VSF_LINUX_DEVFS_UART_CFG_RX_BUFSIZE          64
#   endif
#   ifndef VSF_LINUX_DEVFS_UART_CFG_PRIO
#       define VSF_LINUX_DEVFS_UART_CFG_PRIO                vsf_arch_prio_0
#   endif

typedef struct vsf_linux_uart_priv_t {
    implement(vsf_linux_stream_priv_t)
    implement(vsf_eda_t)
    vsf_eda_t *eda_pending_tx;

    uint32_t mode;
    uint32_t baudrate;

    // use vsf_fifo_stream_t because it doesn't need protect.
    //  so stream APIs can be called directly in isr
    implement(vsf_fifo_stream_t)
    uint8_t __buffer[VSF_LINUX_DEVFS_UART_CFG_RX_BUFSIZE];
} vsf_linux_uart_priv_t;

static uint_fast32_t __vsf_linux_uart_rx(vsf_usart_t *uart, vsf_linux_uart_priv_t *priv)
{
    uint8_t *buffer;
    uint_fast32_t buflen, all_read_size = 0;

    while (vsf_uart_fifo_rx_size(uart) > 0) {
        buflen = vsf_stream_get_wbuf(&priv->use_as__vsf_stream_t, &buffer);

        if (!buflen) {
            vsf_trace_error("uart rx buffer overflow, please increase VSF_LINUX_DEVFS_UART_CFG_RX_BUFSIZE");
            break;
        } else {
            all_read_size += vsf_stream_write(&priv->use_as__vsf_stream_t, NULL, vsf_usart_fifo_read(uart, buffer, buflen));
        }
    }

    return all_read_size;
}

static void __vsf_linux_uart_isrhandler(void *target, vsf_usart_t *uart,
        em_usart_irq_mask_t irq_mask)
{
    vsf_linux_uart_priv_t *priv = (vsf_linux_uart_priv_t *)target;
    vsf_eda_t *eda = NULL;

    if (irq_mask & USART_IRQ_MASK_RX) {
        if (__vsf_linux_uart_rx(uart, priv) > 0) {
            eda = &priv->use_as__vsf_eda_t;
        }
    }
    if (irq_mask & USART_IRQ_MASK_TX_CPL) {
        eda = priv->eda_pending_tx;
        VSF_LINUX_ASSERT(eda != NULL);
        priv->eda_pending_tx = NULL;
    }

    if (eda != NULL) {
        vsf_eda_post_evt(eda, VSF_EVT_USER);
    }
}

static void __vsf_linux_uart_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    if (VSF_EVT_USER == evt) {
        vsf_linux_uart_priv_t *priv = container_of(eda, vsf_linux_uart_priv_t, use_as__vsf_eda_t);
        vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, vsf_protect_sched());
    }
}

static void __vsf_linux_uart_config(vsf_usart_t *uart, vsf_linux_uart_priv_t *priv)
{
    vsf_usart_disable(uart);
    vsf_usart_init(uart, & (usart_cfg_t) {
        .mode               = priv->mode,
        .baudrate           = priv->baudrate,
        .rx_timeout         = 0,
        .isr                = {
            .handler_fn     = __vsf_linux_uart_isrhandler,
            .target_ptr     = priv,
            .prio           = VSF_LINUX_DEVFS_UART_CFG_PRIO,
        },
    });
    vsf_usart_irq_enable(uart, USART_IRQ_MASK_RX);
    vsf_usart_enable(uart);
}

static void __vsf_linux_uart_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_uart_priv_t *priv = (vsf_linux_uart_priv_t *)sfd->priv;
    vsf_usart_t *uart = (vsf_usart_t *)(((vk_vfs_file_t *)(priv->file))->f.param);

    priv->op = &vsf_mem_stream_op;
    priv->buffer = priv->__buffer;
    priv->size = sizeof(priv->__buffer);
    priv->stream_rx = &priv->use_as__vsf_stream_t;
    priv->mode = USART_NO_PARITY | USART_1_STOPBIT | USART_8_BIT_LENGTH | USART_NO_HWCONTROL | USART_TX_ENABLE | USART_RX_ENABLE;
    priv->baudrate = 9600;

    vsf_stream_connect_tx(priv->stream_rx);
    extern void __vsf_linux_rx_stream_init(vsf_linux_fd_t *sfd, vsf_stream_t *stream);
    __vsf_linux_rx_stream_init(sfd, priv->stream_rx);

    __vsf_linux_uart_config(uart, priv);

    priv->fn.evthandler = __vsf_linux_uart_evthandler;
    vsf_eda_init(&priv->use_as__vsf_eda_t, vsf_prio_highest);
}

static int __vsf_linux_uart_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg)
{
    vsf_linux_uart_priv_t *priv = (vsf_linux_uart_priv_t *)sfd->priv;
    vsf_usart_t *uart = (vsf_usart_t *)(((vk_vfs_file_t *)(priv->file))->f.param);
    union {
        struct termios *termios;
    } uarg;

    switch (cmd) {
    case TCGETS:
        uarg.termios = (struct termios *)arg;
        uarg.termios->c_oflag       = OPOST | ONLCR;
        uarg.termios->c_lflag       = ECHO | ECHOE | ECHOK | ECHONL | ICANON;
        uarg.termios->c_cc[VMIN]    = 1;
        uarg.termios->c_cc[VERASE]  = 010;      // BS
        uarg.termios->c_cc[VWERASE] = 027;      // ETB
        uarg.termios->c_cc[VKILL]   = 025;      // NAK
//        uarg.termios->c_ispeed      = ;
//        uarg.termios->c_ospeed      = ;
        break;
    case TCSETS:
        break;
    }
    return 0;
}

static ssize_t __vsf_linux_uart_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_uart_priv_t *priv = (vsf_linux_uart_priv_t *)sfd->priv;
    vsf_usart_t *uart = (vsf_usart_t *)(((vk_vfs_file_t *)(priv->file))->f.param);

    VSF_LINUX_ASSERT(priv->eda_pending_tx == NULL);
    priv->eda_pending_tx = vsf_eda_get_cur();
    vsf_usart_request_tx(uart, (void *)buf, count);
    vsf_thread_wfe(VSF_EVT_USER);
    return count;
}

static int __vsf_linux_uart_close(vsf_linux_fd_t *sfd)
{
    return 0;
}

extern ssize_t __vsf_linux_stream_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static const vsf_linux_fd_op_t __vsf_linux_uart_fdop = {
    .priv_size          = sizeof(vsf_linux_uart_priv_t),
    .fn_init            = __vsf_linux_uart_init,
    .fn_fcntl           = __vsf_linux_uart_fcntl,
    .fn_read            = __vsf_linux_stream_read,
    .fn_write           = __vsf_linux_uart_write,
    .fn_close           = __vsf_linux_uart_close,
};

int vsf_linux_fd_bind_uart(char *path, vsf_usart_t *uart)
{
    return vsf_linux_fs_bind_target_ex(path, uart, &__vsf_linux_uart_fdop,
                NULL, NULL,
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE | VSF_FILE_ATTR_TTY, 0);
}
#endif

__vsf_component_peda_ifs_entry(__vk_devfs_null_write, vk_file_write)
{
    vsf_peda_begin();
    vsf_eda_return(vsf_local.size);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_devfs_zero_read, vk_file_read)
{
    vsf_peda_begin();
    memset(vsf_local.buff, 0, vsf_local.size);
    vsf_eda_return(vsf_local.size);
    vsf_peda_end();
}

int vsf_linux_devfs_init(void)
{
    int err = mkdir("/dev", 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir /dev\r\n");
        return err;
    }

    err = vsf_linux_fs_bind_target_ex("dev/null", NULL, NULL,
                NULL, (vsf_peda_evthandler_t)vsf_peda_func(__vk_devfs_null_write),
                VSF_FILE_ATTR_WRITE, 0);
    if (err != 0) {
        fprintf(stderr, "fail to bind /dev/null\r\n");
        return err;
    }

    err = vsf_linux_fs_bind_target_ex("dev/zero", NULL, NULL,
                (vsf_peda_evthandler_t)vsf_peda_func(__vk_devfs_zero_read), NULL,
                VSF_FILE_ATTR_READ, 0);
    if (err != 0) {
        fprintf(stderr, "fail to bind /dev/zero\r\n");
        return err;
    }

#if VSF_LINUX_DEVFS_USE_RAND == ENABLED
    // TODO: use hal trng instead of rand
    srand(vsf_systimer_get_tick());
    vsf_linux_fs_bind_rand("/dev/random");
    vsf_linux_fs_bind_rand("/dev/urandom");
#endif
    return 0;
}

#endif
