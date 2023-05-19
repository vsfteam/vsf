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
#define __VSF_DISP_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#   include "shell/sys/linux/include/errno.h"
#   include "shell/sys/linux/include/poll.h"
#   include "shell/sys/linux/include/termios.h"
#   include "shell/sys/linux/include/sys/ioctl.h"
#   include "shell/sys/linux/include/sys/stat.h"
#   include "shell/sys/linux/include/linux/serial.h"
#   include "shell/sys/linux/include/linux/input.h"
#   include "shell/sys/linux/include/linux/fb.h"
#   include "shell/sys/linux/include/linux/fs.h"
#   include "shell/sys/linux/include/linux/i2c.h"
#   include "shell/sys/linux/include/linux/i2c-dev.h"
#   include "shell/sys/linux/include/linux/spi/spidev.h"
#else
#   include <unistd.h>
#   include <errno.h>
#   include <poll.h>
#   include <termios.h>
#   include <sys/ioctl.h>
#   include <sys/stat.h>
#   include <linux/serial.h>
#   include <linux/input.h>
#   include <linux/fb.h>
#   include <linux/fs.h>
#   include <linux/i2c.h>
#   include <linux/i2c-dev.h>
#   include <linux/spi/spidev.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "shell/sys/linux/include/simple_libc/stdio.h"
#else
#   include <stdio.h>
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_LINUX_I2C_CFG_ARCH_PRIO
#   define VSF_LINUX_I2C_CFG_ARCH_PRIO          vsf_arch_prio_0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

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
    return vsf_linux_fs_bind_target_ex(path, NULL, NULL,
            (vsf_peda_evthandler_t)vsf_peda_func(__vk_devfs_rand_read), NULL,
            VSF_FILE_ATTR_READ, (uint64_t)-1);
}
#endif

#if VSF_HAL_USE_USART == ENABLED

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
    implement(vsf_linux_term_priv_t)
    implement(vsf_eda_t)
    struct serial_struct ss;
    vsf_eda_t *eda_pending_tx;

    // use vsf_fifo_stream_t because it doesn't need protect.
    //  so stream APIs can be called directly in isr
    implement(vsf_fifo_stream_t)
    uint8_t __buffer[VSF_LINUX_DEVFS_UART_CFG_RX_BUFSIZE];
} vsf_linux_uart_priv_t;

static const uint32_t __vsf_linux_uart_baudrates[] = {
#define __enum_baudrates(__b)       [VSF_MCONNECT2(B, __b)] = __b,
    VSF_MFOREACH(__enum_baudrates,
        50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200,
        38400, 57600, 115200, 230400, 460800, 500000, 576000, 921600, 1000000,
        1152000, 1500000, 2000000, 2500000, 3000000, 3500000, 4000000
    )
};

static uint_fast32_t __vsf_linux_uart_rx(vsf_usart_t *uart, vsf_linux_uart_priv_t *priv)
{
    uint8_t *buffer;
    uint_fast32_t buflen, all_read_size = 0;

    while (vsf_usart_rxfifo_get_data_count(uart) > 0) {
        buflen = vsf_stream_get_wbuf(&priv->use_as__vsf_stream_t, &buffer);
        if (!buflen) {
            vsf_trace_error("term: uart rx buffer overflow, please increase VSF_LINUX_DEVFS_UART_CFG_RX_BUFSIZE\n");
            break;
        } else {
            all_read_size += vsf_stream_write(&priv->use_as__vsf_stream_t, NULL, vsf_usart_rxfifo_read(uart, buffer, buflen));
        }
    }

    return all_read_size;
}

static void __vsf_linux_uart_isrhandler(void *target, vsf_usart_t *uart,
        vsf_usart_irq_mask_t irq_mask)
{
    vsf_linux_uart_priv_t *priv = (vsf_linux_uart_priv_t *)target;
    vsf_eda_t *eda = NULL;

    if (irq_mask & VSF_USART_IRQ_MASK_RX) {
        if (__vsf_linux_uart_rx(uart, priv) > 0) {
            eda = &priv->use_as__vsf_eda_t;
        }
    }
    if (irq_mask & VSF_USART_IRQ_MASK_TX_CPL) {
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
        vsf_protect_t orig = vsf_protect_sched();
        if (vsf_stream_get_data_size(priv->stream_rx)) {
            vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, orig);
        } else {
            vsf_unprotect_sched(orig);
        }
    }
}

static void __vsf_linux_uart_config(vsf_linux_uart_priv_t *priv)
{
    vsf_usart_t *uart = (vsf_usart_t *)(((vk_vfs_file_t *)(priv->file))->f.param);
    struct termios *term = &priv->termios;
    uint32_t baudrate;
    uint32_t mode = 0;

    if (priv->ss.flags & ASYNC_SPD_CUST) {
        baudrate = priv->ss.baud_base / priv->ss.custom_divisor;
    } else {
        if (term->c_ospeed != term->c_ispeed) {
            vsf_trace_error("term: doesnot support different input/output speed\n");
            return;
        }

        if ((0 == term->c_ospeed) || (term->c_ospeed >= dimof(__vsf_linux_uart_baudrates))) {
            vsf_trace_error("term: baudrate does not supported\n");
            return;
        }

        baudrate = __vsf_linux_uart_baudrates[term->c_ospeed];
    }

    switch (term->c_cflag & CSIZE) {
    case CS5:   mode |= VSF_USART_5_BIT_LENGTH; break;
    case CS6:   mode |= VSF_USART_6_BIT_LENGTH; break;
    case CS7:   mode |= VSF_USART_7_BIT_LENGTH; break;
    case CS8:   mode |= VSF_USART_8_BIT_LENGTH; break;
    default:    vsf_trace_error("term: bit length does not supported\n");  return;
    }

    if (term->c_cflag & PARENB) {
        if (term->c_cflag & PARODD) {
            mode |= VSF_USART_ODD_PARITY;
        } else {
            mode |= VSF_USART_EVEN_PARITY;
        }
    } else {
        mode |= VSF_USART_NO_PARITY;
    }
    if (term->c_cflag & CSTOPB) {
        mode |= VSF_USART_2_STOPBIT;
    } else {
        mode |= VSF_USART_1_STOPBIT;
    }

    vsf_usart_irq_disable(uart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_TX_CPL);
    vsf_usart_disable(uart);
    vsf_usart_init(uart, & (vsf_usart_cfg_t) {
        .mode               = mode,
        .baudrate           = baudrate,
        .rx_timeout         = 0,
        .isr                = {
            .handler_fn     = __vsf_linux_uart_isrhandler,
            .target_ptr     = priv,
            .prio           = VSF_LINUX_DEVFS_UART_CFG_PRIO,
        },
    });
    vsf_usart_enable(uart);
    vsf_usart_irq_enable(uart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_TX_CPL);
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

static void __vsf_linux_uart_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_uart_priv_t *priv = (vsf_linux_uart_priv_t *)sfd->priv;

    priv->subop = sfd->op;
    sfd->op = &vsf_linux_term_fdop;
    vsf_linux_term_fdop.fn_init(sfd);

    priv->op = &vsf_fifo_stream_op;
    priv->buffer = priv->__buffer;
    priv->size = sizeof(priv->__buffer);
    priv->stream_rx = &priv->use_as__vsf_stream_t;

    vsf_stream_connect_tx(priv->stream_rx);

    // DO NOT call __vsf_linux_rx_stream_init
    //  event is trigger in __vsf_linux_uart_evthandler, not in stream evthandler
    vsf_stream_connect_rx(priv->stream_rx);
    __vsf_linux_uart_evthandler(&priv->use_as__vsf_eda_t, VSF_EVT_USER);
    __vsf_linux_uart_config(priv);

    priv->fn.evthandler = __vsf_linux_uart_evthandler;
    vsf_eda_init(&priv->use_as__vsf_eda_t, vsf_prio_highest);
}

static int __vsf_linux_uart_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    vsf_linux_uart_priv_t *priv = (vsf_linux_uart_priv_t *)sfd->priv;
    union {
        uintptr_t arg;
        struct serial_struct *ss;
    } arg_union;
    arg_union.arg = arg;

    switch (cmd) {
    case TIOCGSERIAL:
        *arg_union.ss = priv->ss;
        break;
    case TIOCSSERIAL:
        priv->ss = *arg_union.ss;
    case FIONREAD:
        *(int *)arg = vsf_stream_get_data_size(priv->stream_rx);
        break;
    case TCSETS:
        __vsf_linux_uart_config(priv);
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
    VSF_LINUX_ASSERT(priv->eda_pending_tx != NULL);
    vsf_usart_request_tx(uart, (void *)buf, count);
    vsf_thread_wfe(VSF_EVT_USER);
    return count;
}

extern ssize_t __vsf_linux_stream_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static const vsf_linux_fd_op_t __vsf_linux_uart_fdop = {
    .priv_size          = sizeof(vsf_linux_uart_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_init            = __vsf_linux_uart_init,
    .fn_fcntl           = __vsf_linux_uart_fcntl,
    .fn_read            = __vsf_linux_stream_read,
    .fn_write           = __vsf_linux_uart_write,
};

int vsf_linux_fs_bind_uart(char *path, vsf_usart_t *uart)
{
    return vsf_linux_fs_bind_target_ex(path, uart, &__vsf_linux_uart_fdop,
                NULL, NULL,
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE | VSF_FILE_ATTR_TTY, 0);
}
#endif

#if VSF_HAL_USE_I2C == ENABLED

typedef struct vsf_linux_i2c_priv_t {
    implement(vsf_linux_fs_priv_t)

    struct {
        uint16_t addr;
        uint16_t flags;
        vsf_eda_t *pending_eda;
        vsf_i2c_cfg_t cfg;
        vsf_i2c_irq_mask_t irq_mask;
    } i2c;
} vsf_linux_i2c_priv_t;

static ssize_t __vsf_linux_i2c_master_request(vsf_linux_i2c_priv_t *priv,
            vsf_i2c_cmd_t cmd, uint16_t count, uint8_t *buf)
{
    vsf_i2c_t *i2c = (vsf_i2c_t *)(((vk_vfs_file_t *)(priv->file))->f.param);
    VSF_LINUX_ASSERT(NULL == priv->i2c.pending_eda);
    priv->i2c.pending_eda = vsf_eda_get_cur();
    VSF_LINUX_ASSERT(priv->i2c.pending_eda != NULL);
    if (vsf_i2c_master_request(i2c, priv->i2c.addr, cmd, count, buf) != VSF_ERR_NONE) {
        priv->i2c.pending_eda = NULL;
        return -1;
    }
    vsf_thread_wfe(VSF_EVT_USER);
    if (priv->i2c.irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
        return vsf_i2c_get_transferred_count(i2c);
    }
    errno = EIO;
    return -1;
}

static ssize_t __vsf_linux_i2c_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    return __vsf_linux_i2c_master_request((vsf_linux_i2c_priv_t *)sfd->priv,
        VSF_I2C_CMD_START | VSF_I2C_CMD_READ | VSF_I2C_CMD_STOP, count, (uint8_t *)buf);
}

static ssize_t __vsf_linux_i2c_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    return __vsf_linux_i2c_master_request((vsf_linux_i2c_priv_t *)sfd->priv,
        VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP, count, (uint8_t *)buf);
}

static int __vsf_linux_i2c_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    vsf_linux_i2c_priv_t *priv = (vsf_linux_i2c_priv_t *)sfd->priv;
    union {
        struct i2c_rdwr_ioctl_data *rdwr_arg;
        struct i2c_smbus_ioctl_data *data_arg;
    } u;
    vsf_i2c_cmd_t i2c_cmd;
    ssize_t size;

    switch (cmd) {
    case I2C_SLAVE:
        if ((arg > 0x3FF) || (!(priv->flags & I2C_M_TEN) && (arg > 0x7F))) {
            return -1;
        }
        priv->i2c.addr = (uint16_t)arg;
        break;
    case I2C_TENBIT:
        if (arg) {
            priv->i2c.flags |= I2C_M_TEN;
        } else {
            priv->i2c.flags &= ~I2C_M_TEN;
        }
        break;
    case I2C_FUNCS:
        *(unsigned long *)arg = I2C_FUNC_I2C | I2C_FUNC_NOSTART
                            |   I2C_FUNC_SMBUS_QUICK
                            |   I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA
                            |   I2C_FUNC_SMBUS_I2C_BLOCK;
        break;
    case I2C_RDWR:
        u.rdwr_arg = (struct i2c_rdwr_ioctl_data *)arg;
        // TODO
        break;
    case I2C_SMBUS:
        u.data_arg = (struct i2c_smbus_ioctl_data*)arg;
        i2c_cmd = VSF_I2C_CMD_START | VSF_I2C_CMD_STOP;
        switch (u.data_arg->size) {
        case I2C_SMBUS_QUICK:
            // S Address R/W# A P
            size = __vsf_linux_i2c_master_request(priv,
                i2c_cmd | (u.data_arg->read_write ? VSF_I2C_CMD_READ : VSF_I2C_CMD_WRITE),
                0, NULL);
            return size < 0 ? size : 0;
        case I2C_SMBUS_BYTE:
            // S Address R/W# A byte N P
            size = __vsf_linux_i2c_master_request(priv,
                i2c_cmd | (u.data_arg->read_write ? VSF_I2C_CMD_READ : VSF_I2C_CMD_WRITE),
                1, &u.data_arg->data->byte);
            return size < 0 ? size : 0;
        case I2C_SMBUS_BYTE_DATA:
            // I2C_SMBUS_READ:  S Address W A command A Sr Address R A byte N P
            // I2C_SMBUS_WRITE: S Address W A command A byte A P
            return -ENOTSUPP;
        case I2C_SMBUS_WORD_DATA:
            // I2C_SMBUS_READ:  S Address W A command A Sr Address R A low_byte A high_byte N P
            // I2C_SMBUS_WRITE: S Address W A command A low_byte A high_byte A P
            return -ENOTSUPP;
        case I2C_SMBUS_BLOCK_DATA:
            // I2C_SMBUS_READ:  S Address W A command A Sr Address R A count A byte0 A .... N P
            // I2C_SMBUS_WRITE: S Address W A command A count A byte0 A .... A P
            return -ENOTSUPP;
        case I2C_SMBUS_I2C_BLOCK_BROKEN:
        case I2C_SMBUS_I2C_BLOCK_DATA:
            // I2C_SMBUS_READ:  S Address W A command A Sr Address R A byte0 A .... N P
            // I2C_SMBUS_WRITE: S Address W A command A byte0 A .... A P
            size = __vsf_linux_i2c_master_request(priv,
                VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE,
                1, &u.data_arg->command);
            if (1 == size) {
                size = __vsf_linux_i2c_master_request(priv,
                    VSF_I2C_CMD_STOP | (u.data_arg->read_write ?
                        VSF_I2C_CMD_RESTART | VSF_I2C_CMD_READ
                    :   VSF_I2C_CMD_WRITE),
                    u.data_arg->data->block[0], &u.data_arg->data->block[1]);
            }
            return size < 0 ? size : 0;
        }
        break;
    case I2C_PEC:
        return -ENOTSUPP;
    }
    return 0;
}

static void __vsf_linux_i2c_isrhandler(void *target_ptr, vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    vsf_linux_i2c_priv_t *priv = (vsf_linux_i2c_priv_t *)target_ptr;
    vsf_eda_t *pending_eda = priv->i2c.pending_eda;

    priv->i2c.irq_mask = irq_mask;
    priv->i2c.pending_eda = NULL;
    if (pending_eda != NULL) {
        vsf_eda_post_evt(pending_eda, VSF_EVT_USER);
    }
}

static void __vsf_linux_i2c_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_i2c_priv_t *priv = (vsf_linux_i2c_priv_t *)sfd->priv;
    vsf_i2c_t *i2c = (vsf_i2c_t *)(((vk_vfs_file_t *)(priv->file))->f.param);

    priv->i2c.cfg.mode = VSF_I2C_MODE_MASTER | VSF_I2C_SPEED_STANDARD_MODE | VSF_I2C_ADDR_7_BITS;
    priv->i2c.cfg.clock_hz = 100 * 1000;
    priv->i2c.cfg.isr.handler_fn = __vsf_linux_i2c_isrhandler;
    priv->i2c.cfg.isr.target_ptr = priv;
    priv->i2c.cfg.isr.prio = VSF_LINUX_I2C_CFG_ARCH_PRIO;
    vsf_i2c_init(i2c, &priv->i2c.cfg);
    while (fsm_rt_cpl != vsf_i2c_enable(i2c));
    vsf_i2c_irq_enable(i2c, VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                        |   VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                        |   VSF_I2C_IRQ_MASK_MASTER_NACK_DETECT);
}

static const vsf_linux_fd_op_t __vsf_linux_i2c_fdop = {
    .priv_size          = sizeof(vsf_linux_i2c_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_init            = __vsf_linux_i2c_init,
    .fn_fcntl           = __vsf_linux_i2c_fcntl,
    .fn_read            = __vsf_linux_i2c_read,
    .fn_write           = __vsf_linux_i2c_write,
};

int vsf_linux_fs_bind_i2c(char *path, vsf_i2c_t *i2c)
{
    return vsf_linux_fs_bind_target_ex(path, i2c, &__vsf_linux_i2c_fdop,
                NULL, NULL,
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
}
#endif

#if VSF_HAL_USE_SPI == ENABLED

typedef struct vsf_linux_spi_priv_t {
    implement(vsf_linux_fs_priv_t)

    uint32_t mode;
    uint32_t speed;
    uint8_t bits_per_word;
    uint8_t cs_index;

    uint8_t xfer_size;
    uint8_t xfer_pos;
    struct spi_ioc_transfer *xfer_ptr;
    vsf_eda_t *xfer_eda;
} vsf_linux_spi_priv_t;

static bool __vsf_linux_spi_transfer(vsf_linux_spi_priv_t *priv)
{
    vsf_spi_t *spi = (vsf_spi_t *)(((vk_vfs_file_t *)(priv->file))->f.param);

    if (priv->xfer_pos < priv->xfer_size) {
        struct spi_ioc_transfer *cur_trans = priv->xfer_ptr + priv->xfer_pos;
        // TODO: support cur_trans->bits_per_word
        // TODO: support cur_trans->speed_hz
        VSF_HAL_ASSERT(priv->xfer_ptr != NULL);
        vsf_err_t result = vsf_spi_request_transfer(spi, (void *)cur_trans->tx_buf, (void *)cur_trans->rx_buf, cur_trans->len);
        VSF_LINUX_ASSERT(result == VSF_ERR_NONE);
        return true;
    } else {
        return false;
    }
}

static void __vsf_linux_spi_isrhandler(void *target, vsf_spi_t *spi,
        vsf_spi_irq_mask_t irq_mask)
{
    if (irq_mask & VSF_SPI_IRQ_MASK_CPL) {
        vsf_linux_spi_priv_t *priv = (vsf_linux_spi_priv_t *)target;
        priv->xfer_pos++;
        // TODO: support cur_trans->delay_usecs
        // TODO: support cur_trans->cs_change
        if (!__vsf_linux_spi_transfer(priv)) {
            vsf_eda_t *eda = priv->xfer_eda;
            VSF_LINUX_ASSERT(eda != NULL);
            priv->xfer_eda = NULL;
            vsf_spi_cs_inactive(spi, priv->cs_index);
            vsf_eda_post_evt(eda, VSF_EVT_USER);
        }
    }
}

static vsf_err_t __vsf_linux_spi_config(vsf_linux_spi_priv_t *priv)
{
    vsf_err_t result;
    vsf_spi_t *spi = (vsf_spi_t *)(((vk_vfs_file_t *)(priv->file))->f.param);

    vsf_spi_mode_t mode = 0;
    switch (priv->mode & (SPI_CPOL | SPI_CPHA)) {
    case SPI_MODE_0:
        mode |= VSF_SPI_CLOCK_MODE_0;
        break;
    case SPI_MODE_1:
        mode |= VSF_SPI_CLOCK_MODE_1;
        break;
    case SPI_MODE_2:
        mode |= VSF_SPI_CLOCK_MODE_2;
        break;
    case SPI_MODE_3:
        mode |= VSF_SPI_CLOCK_MODE_3;
        break;
    }
    mode |= vsf_spi_data_bits_to_mode(priv->bits_per_word);
    // TODO: Update when hal/spi vsf_spi_mode_t support lsb/msb
    // TODO: Confirming the behavior of SPI_NO_CS
    vsf_spi_cfg_t cfg    = {
        .mode               = mode,
        .clock_hz           = priv->speed,
        .isr                = {
            .handler_fn     = __vsf_linux_spi_isrhandler,
            .target_ptr     = priv,
            .prio           = VSF_LINUX_DEVFS_UART_CFG_PRIO,
        },
    };

    vsf_spi_irq_disable(spi, VSF_SPI_IRQ_MASK_CPL);
    vsf_spi_disable(spi);
    result = vsf_spi_init(spi, &cfg);
    vsf_spi_enable(spi);
    vsf_spi_irq_enable(spi, VSF_SPI_IRQ_MASK_CPL);

    vsf_spi_capability_t cap = vsf_spi_capability(spi);
    VSF_LINUX_ASSERT(priv->cs_index < cap.cs_count);

    if (priv->mode & SPI_CS_HIGH) {
        vsf_spi_cs_inactive(spi, priv->cs_index);
    } else {
        vsf_spi_cs_active(spi, priv->cs_index);
    }

    return result;
}

static void __vsf_linux_spi_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_spi_priv_t *priv = (vsf_linux_spi_priv_t *)sfd->priv;
    vsf_spi_t *spi = (vsf_spi_t *)(((vk_vfs_file_t *)(priv->file))->f.param);

    priv->mode = SPI_MODE_3 | SPI_LSB_FIRST | SPI_CS_HIGH;
    priv->speed = 1 * 1000 * 1000;
    priv->bits_per_word = 8;

    vk_file_t *file = priv->file;
    char *cs_str = strchr(file->name, '.');
    VSF_LINUX_ASSERT(cs_str != NULL);
    priv->cs_index = atoi(cs_str + 1);

    vsf_err_t result = __vsf_linux_spi_config(priv);
    VSF_LINUX_ASSERT(result == VSF_ERR_NONE);
}

static int __vsf_linux_spi_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    vsf_linux_spi_priv_t *priv = (vsf_linux_spi_priv_t *)sfd->priv;
    vsf_spi_t *spi = (vsf_spi_t *)(((vk_vfs_file_t *)(priv->file))->f.param);

    if (_IOC_TYPE(cmd) != SPI_IOC_MAGIC) {
        return -1;
    }

    union {
        uintptr_t arg;
        uint8_t  *mode_u8;
        uint32_t *mode_u32;
        uint8_t  *lsb;
        uint8_t  *bits_per_word;
        uint32_t *speed;
        struct spi_ioc_transfer *xfer_ptr;
    } arg_union;
    arg_union.arg = arg;

    int cmd_nr = _IOC_NR(cmd);
    switch (cmd) {
    case SPI_IOC_RD_MODE:
        *arg_union.mode_u8 = priv->mode;
        break;
    case SPI_IOC_RD_MODE32:
        *arg_union.mode_u32 = priv->mode;
        break;
    case SPI_IOC_RD_LSB_FIRST:
        *arg_union.lsb = (priv->mode & SPI_LSB_FIRST) ? 1 : 0;
        break;
    case SPI_IOC_RD_BITS_PER_WORD:
        *arg_union.bits_per_word = priv->bits_per_word;
        break;
    case SPI_IOC_RD_MAX_SPEED_HZ:
        *arg_union.speed = priv->speed;
        break;

    case SPI_IOC_WR_MODE:
        priv->mode = *arg_union.mode_u8;
        break;
    case SPI_IOC_WR_MODE32:
        priv->mode = *arg_union.mode_u32;
        break;
    case SPI_IOC_WR_LSB_FIRST:
        if (arg_union.lsb) {
            priv->mode |= SPI_LSB_FIRST;
        } else {
            priv->mode &= ~SPI_LSB_FIRST;
        }
        break;
    case SPI_IOC_WR_BITS_PER_WORD:
        priv->bits_per_word = *arg_union.bits_per_word;
        break;
    case SPI_IOC_WR_MAX_SPEED_HZ:
        priv->speed = *arg_union.speed;
        break;

    default: {
            uint8_t size = _IOC_SIZE(cmd);
            if (size >= 1) {
                VSF_LINUX_ASSERT(priv->xfer_ptr == NULL);
                VSF_LINUX_ASSERT(priv->xfer_size == 0);

                priv->xfer_pos = 0;
                priv->xfer_size = size;
                priv->xfer_ptr = arg_union.xfer_ptr;

                priv->xfer_eda = vsf_eda_get_cur();
                VSF_LINUX_ASSERT(priv->xfer_eda != NULL);

                vsf_spi_cs_active(spi, priv->cs_index);
                __vsf_linux_spi_transfer(priv);
                vsf_thread_wfe(VSF_EVT_USER);
                return 0;
            } else {
                return -1;
            }
        }
    }

    if (_IOC_DIR(cmd) == _IOC_WRITE) {
        if (VSF_ERR_NONE != __vsf_linux_spi_config(priv)) {
            return -1;
        }
    }
    return 0;
}

static const vsf_linux_fd_op_t __vsf_linux_spi_fdop = {
    .priv_size          = sizeof(vsf_linux_spi_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_init            = __vsf_linux_spi_init,
    .fn_fcntl           = __vsf_linux_spi_fcntl,
};

int vsf_linux_fs_bind_spi(char *path, vsf_spi_t *spi)
{
    return vsf_linux_fs_bind_target_ex(path, spi, &__vsf_linux_spi_fdop,
                NULL, NULL,
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
}
#endif

#if VSF_USE_MAL == ENABLED

typedef struct vsf_linux_mal_priv_t {
    implement(vsf_linux_fs_priv_t)
} vsf_linux_mal_priv_t;

static int __vsf_linux_mal_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    vsf_linux_mal_priv_t *priv = (vsf_linux_mal_priv_t *)sfd->priv;
    vk_mal_t *mal = (vk_mal_t *)(((vk_vfs_file_t *)(priv->file))->f.param);

    switch (cmd) {
    case BLKGETSIZE64:
    case BLKBSZGET: {
            uint_fast32_t blksz = vk_mal_blksz(mal, 0, 0, VSF_MAL_OP_ERASE);
            if (!blksz) {
                blksz = vk_mal_blksz(mal, 0, 0, VSF_MAL_OP_WRITE);
            }
            if (BLKGETSIZE64 == cmd) {
                *(uint64_t *)arg = blksz;
            } else {
                *(int *)arg = blksz;
            }
        }
        break;
    case BLKBSZSET:
        return -1;
    }
    return 0;
}

static ssize_t __vsf_linux_mal_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_mal_priv_t *priv = (vsf_linux_mal_priv_t *)sfd->priv;
    vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)priv->file;
    vk_mal_t *mal = (vk_mal_t *)(((vk_vfs_file_t *)(priv->file))->f.param);
    uint_fast32_t blksz = vk_mal_blksz(mal, vfs_file->pos, count, VSF_MAL_OP_READ);

    if (count % blksz) {
        return -1;
    }
    vk_mal_read(mal, vfs_file->pos, count, (uint8_t *)buf);
    return vsf_eda_get_return_value();
}

static ssize_t __vsf_linux_mal_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_mal_priv_t *priv = (vsf_linux_mal_priv_t *)sfd->priv;
    vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)priv->file;
    vk_mal_t *mal = (vk_mal_t *)(((vk_vfs_file_t *)(priv->file))->f.param);
    uint_fast32_t blksz = vk_mal_blksz(mal, vfs_file->pos, count, VSF_MAL_OP_WRITE);

    if (count % blksz) {
        return -1;
    }
    vk_mal_write(mal, vfs_file->pos, count, (uint8_t *)buf);
    return vsf_eda_get_return_value();
}

static const vsf_linux_fd_op_t __vsf_linux_mal_fdop = {
    .priv_size          = sizeof(vsf_linux_mal_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_fcntl           = __vsf_linux_mal_fcntl,
    .fn_read            = __vsf_linux_mal_read,
    .fn_write           = __vsf_linux_mal_write,
};

int vsf_linux_fs_bind_mal(char *path, vk_mal_t *mal)
{
    return vsf_linux_fs_bind_target_ex(path, mal, &__vsf_linux_mal_fdop,
                NULL, NULL,
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE | VSF_FILE_ATTR_BLK, mal->size);
}
#endif

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_CFG_REGISTRATION_MECHANISM == ENABLED

#ifndef VSF_LINUX_DEVFS_INPUT_CFG_EVENT_POLL_SIZE
#   define VSF_LINUX_DEVFS_INPUT_CFG_EVENT_POLL_SIZE        64
#endif

typedef struct vsf_linux_input_event_t {
    vsf_slist_node_t node;
    struct input_event evt;
} vsf_linux_input_event_t;

dcl_vsf_pool(vsf_linux_input_event_pool)
def_vsf_pool(vsf_linux_input_event_pool, vsf_linux_input_event_t)
#define __name vsf_linux_input_event_pool
#define __type vsf_linux_input_event_t
#include "service/pool/impl_vsf_pool.inc"

typedef struct vsf_linux_input_priv_t {
    implement(vsf_linux_fs_priv_t)
    vk_input_notifier_t notifier;

    vsf_pool(vsf_linux_input_event_pool) event_pool;
    vsf_linux_input_event_t event_buffer[VSF_LINUX_DEVFS_INPUT_CFG_EVENT_POLL_SIZE];
    vsf_slist_queue_t event_queue;
} vsf_linux_input_priv_t;

static void __linux_input_from_vsf_input(struct input_event *input_event, vk_input_type_t type, vk_input_evt_t *evt)
{
    vsf_systimer_tick_t us = vsf_systimer_get_us();
    input_event->time.tv_usec = us % 1000;
    input_event->time.tv_sec = us / 1000;
    input_event->type = type;

    switch (type) {
    case VSF_INPUT_TYPE_KEYBOARD:
        input_event->code = vsf_input_keyboard_get_keycode(evt);
        input_event->value = vsf_input_keyboard_is_down(evt) ? 1 : 0;
        break;
    }
}

static void __vsf_input_from_linux_input(vk_input_type_t *type, vk_input_evt_t *evt, struct input_event *input_event)
{
    // TODO:
}

static void __vsf_linux_input_on_event(vk_input_notifier_t *notifier, vk_input_type_t type, vk_input_evt_t *evt)
{
    vsf_linux_input_priv_t *input_priv = container_of(notifier, vsf_linux_input_priv_t, notifier);
    vsf_linux_input_event_t *event = VSF_POOL_ALLOC(vsf_linux_input_event_pool, &input_priv->event_pool);
    if (NULL == event) {
        vsf_trace_warning("fail to allocate linux_event\n");
        return;
    }

    __linux_input_from_vsf_input(&event->evt, type, evt);

    vsf_protect_t orig = vsf_protect_int();
        vsf_slist_queue_enqueue(vsf_linux_input_event_t, node, &input_priv->event_queue, event);
    vsf_unprotect_int(orig);
    vsf_linux_fd_set_status(&input_priv->use_as__vsf_linux_fd_priv_t, POLLIN, vsf_protect_sched());
}

static void __vsf_linux_input_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_input_priv_t *input_priv = (vsf_linux_input_priv_t *)sfd->priv;

    vk_input_notifier_t *notifier = (vk_input_notifier_t *)(((vk_vfs_file_t *)(input_priv->file))->f.param);
    input_priv->notifier.dev = notifier->dev;
    input_priv->notifier.mask = notifier->mask;

    if (input_priv->notifier.mask != 0) {
        input_priv->notifier.on_evt = __vsf_linux_input_on_event;
        vk_input_notifier_register(&input_priv->notifier);
    }

    VSF_POOL_PREPARE(vsf_linux_input_event_pool, &input_priv->event_pool,
        .region_ptr = (vsf_protect_region_t *)&vsf_protect_region_int,
    );
    VSF_POOL_ADD_BUFFER(vsf_linux_input_event_pool, &input_priv->event_pool,
        input_priv->event_buffer, sizeof(input_priv->event_buffer));
}

static int __vsf_linux_input_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    return 0;
}

static int __vsf_linux_input_stat(vsf_linux_fd_t *sfd, struct stat *buf)
{
    buf->st_mode = S_IFCHR;
    return 0;
}

static ssize_t __vsf_linux_input_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    VSF_LINUX_ASSERT(!(count % sizeof(struct input_event)));
    vsf_linux_input_priv_t *input_priv = (vsf_linux_input_priv_t *)sfd->priv;
    struct input_event *linux_input_event = (struct input_event *)buf;
    vsf_linux_input_event_t *event;
    size_t read_count = 0;
    vsf_protect_t orig;

again:
    while (read_count < count) {
        orig = vsf_protect_int();
            vsf_slist_queue_dequeue(vsf_linux_input_event_t, node, &input_priv->event_queue, event);
        vsf_unprotect_int(orig);

        if (NULL == event) {
            vsf_linux_fd_clear_status(&input_priv->use_as__vsf_linux_fd_priv_t, POLLIN, vsf_protect_sched());
            break;
        }

        *linux_input_event++ = event->evt;
        VSF_POOL_FREE(vsf_linux_input_event_pool, &input_priv->event_pool, event);
        read_count += sizeof(struct input_event);
    }

    if (!read_count) {
        if (vsf_linux_fd_is_block(sfd)) {
            vsf_linux_trigger_t trig;
            vsf_linux_trigger_init(&trig);

            if (!vsf_linux_fd_pend_events(&input_priv->use_as__vsf_linux_fd_priv_t, POLLIN, &trig, vsf_protect_sched())) {
                // triggered by signal
                return -1;
            }
            goto again;
        } else {
            errno = EAGAIN;
            return -1;
        }
    }

    return read_count;
}

static ssize_t __vsf_linux_input_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    VSF_LINUX_ASSERT(!(count % sizeof(struct input_event)));
    struct input_event *linux_input_event = (struct input_event *)buf;
    size_t written_count = 0;
    vk_input_type_t vsf_input_type;
    vk_input_evt_t vsf_input_event;

    while (written_count < count) {
        __vsf_input_from_linux_input(&vsf_input_type, &vsf_input_event, linux_input_event++);
        vsf_input_on_evt(vsf_input_type, &vsf_input_event);
        written_count += sizeof(struct input_event);
    }

    return written_count;
}

static int __vsf_linux_input_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_input_priv_t *input_priv = (vsf_linux_input_priv_t *)sfd->priv;

    if (input_priv->notifier.mask != 0) {
        vk_input_notifier_unregister(&input_priv->notifier);
    }
    return 0;
}

static const vsf_linux_fd_op_t __vsf_linux_input_fdop = {
    .priv_size          = sizeof(vsf_linux_input_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_init            = __vsf_linux_input_init,
    .fn_fcntl           = __vsf_linux_input_fcntl,
    .fn_read            = __vsf_linux_input_read,
    .fn_write           = __vsf_linux_input_write,
    .fn_close           = __vsf_linux_input_close,
    .fn_stat            = __vsf_linux_input_stat,
};

int vsf_linux_fs_bind_input(char *path, vk_input_notifier_t *notifier)
{
    return vsf_linux_fs_bind_target_ex(
                path, notifier, &__vsf_linux_input_fdop, NULL, NULL,
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
}
#endif

#if VSF_USE_UI == ENABLED
typedef struct vsf_linux_fb_priv_t {
    implement(vsf_linux_fs_priv_t)
    void *front_buffer;
#if VSF_DISP_USE_FB == ENABLED
    bool is_disp_fb;
#endif
    vsf_trig_t fresh_trigger;
    vsf_teda_t fresh_task;
    vsf_eda_t *eda_pending;
} vsf_linux_fb_priv_t;

static void __vsf_linux_disp_on_ready(vk_disp_t *disp)
{
    vsf_eda_post_evt((vsf_eda_t *)disp->ui_data, VSF_EVT_USER);
}

static void __vsf_linux_disp_fresh_task(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_linux_fb_priv_t *fb_priv = container_of(eda, vsf_linux_fb_priv_t, fresh_task);
    vk_disp_t *disp = (vk_disp_t *)(((vk_vfs_file_t *)(fb_priv->file))->f.param);

    switch (evt) {
    case VSF_EVT_RETURN:
        if (fb_priv->eda_pending != NULL) {
            vsf_eda_t *eda_pending = fb_priv->eda_pending;
            fb_priv->eda_pending = NULL;
            vsf_eda_post_evt(eda_pending, VSF_EVT_USER);
        }
        // fall through
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != vsf_eda_trig_wait(&fb_priv->fresh_trigger, vsf_systimer_ms_to_tick(33))) {
            break;
        }
        // fall through
    case VSF_EVT_TIMER:
    case VSF_EVT_SYNC:
        if (VSF_SYNC_PENDING == vsf_eda_sync_get_reason(&fb_priv->fresh_trigger, evt)) {
            break;
        }

        vk_disp_refresh(disp, NULL, fb_priv->front_buffer);
        break;
    }
}

static void __vsf_linux_fb_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_fb_priv_t *fb_priv = (vsf_linux_fb_priv_t *)sfd->priv;
    vk_disp_t *disp = (vk_disp_t *)(((vk_vfs_file_t *)(fb_priv->file))->f.param);

    disp->ui_on_ready = __vsf_linux_disp_on_ready;
    disp->ui_data = vsf_eda_get_cur();
    VSF_LINUX_ASSERT(disp->ui_data != NULL);
    vk_disp_init(disp);
    vsf_thread_wfe(VSF_EVT_USER);

#if VSF_DISP_USE_FB == ENABLED
    fb_priv->is_disp_fb = disp->param.drv == &vk_disp_drv_fb;
    if (fb_priv->is_disp_fb) {
        fb_priv->front_buffer = vk_disp_fb_get_front_buffer(disp);
        return;
    } else
#endif
    {
        vsf_eda_trig_init(&fb_priv->fresh_trigger, false, true);
        fb_priv->fresh_task.fn.evthandler = __vsf_linux_disp_fresh_task;
        disp->ui_data = &fb_priv->fresh_task;
        vsf_teda_init(&fb_priv->fresh_task);
    }

    uint_fast32_t frame_size = disp->param.height * disp->param.width * vsf_disp_get_pixel_bytesize(disp);
    fb_priv->front_buffer = malloc(frame_size);
    VSF_LINUX_ASSERT(fb_priv->front_buffer != NULL);
}

static int __vsf_linux_fb_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    vsf_linux_fb_priv_t *fb_priv = (vsf_linux_fb_priv_t *)sfd->priv;
    vk_disp_t *disp = (vk_disp_t *)(((vk_vfs_file_t *)(fb_priv->file))->f.param);
#if VSF_DISP_USE_FB == ENABLED
    vk_disp_fb_t *disp_fb = (vk_disp_fb_t *)disp;
#endif
    // TODO: add pitch to disp_fb
    uint_fast32_t pitch_size = disp->param.width * vsf_disp_get_pixel_bytesize(disp);
    uint_fast32_t frame_size = disp->param.height * pitch_size;

    switch (cmd) {
    case FBIOGET_VSCREENINFO: {
            struct fb_var_screeninfo *info = (struct fb_var_screeninfo *)arg;
            const struct fb_bitfield *bitfields = NULL;
            memset(info, 0, sizeof(*info));
            info->xres = disp->param.width;
            info->yres = disp->param.height;
            info->xres_virtual = info->xres;
            info->yres_virtual = info->yres;
            switch (disp->param.color) {
            case VSF_DISP_COLOR_RGB565: {
                    static const struct fb_bitfield __bitfiled_rgb565[4] = {{11, 5, 0}, {5, 6, 0}, {0, 5, 0}, {0, 0, 0}};
                    bitfields = __bitfiled_rgb565;
                }
                break;
            case VSF_DISP_COLOR_ARGB8888: {
                    static const struct fb_bitfield __bitfield_arg8888[4] = {{16, 8, 0}, {8, 8, 0}, {0, 8, 0}, {24, 8, 0}};
                    bitfields = __bitfield_arg8888;
                }
                break;
            default:
                vsf_trace_warning("fb: color not supported" VSF_TRACE_CFG_LINEEND);
                break;
            }
            if (bitfields != NULL) {
                info->red = bitfields[0];
                info->green = bitfields[1];
                info->blue = bitfields[2];
                info->transp = bitfields[3];
            }
#if VSF_DISP_USE_FB == ENABLED
            if (fb_priv->is_disp_fb) {
                info->yres_virtual *= disp_fb->fb.num;
            }
#endif
            info->bits_per_pixel = vsf_disp_get_pixel_bitsize(disp);
            // todo: parse fb_bitfield red/green/blue/transp
//            switch (disp->param.color) {
//            }
        }
        break;
    case FBIOPUT_VSCREENINFO: {
            struct fb_var_screeninfo *info = (struct fb_var_screeninfo *)arg;
            VSF_UNUSED_PARAM(info);
        }
        break;
    case FBIOGET_FSCREENINFO: {
            struct fb_fix_screeninfo *info = (struct fb_fix_screeninfo *)arg;
            strcpy(info->id, "vsf_disp_fb");
#if VSF_DISP_USE_FB == ENABLED
            if (fb_priv->is_disp_fb) {
                info->smem_start = (uintptr_t)disp_fb->fb.buffer;
                info->smem_len = disp_fb->fb.size * disp_fb->fb.num;
            } else
#endif
            {
                info->smem_start = (uintptr_t)fb_priv->front_buffer;
                info->smem_len = frame_size;
            }
            info->line_length = pitch_size;
        }
        break;
    case FBIOPAN_DISPLAY: {
            struct fb_var_screeninfo *info = (struct fb_var_screeninfo *)arg;
#if VSF_DISP_USE_FB == ENABLED
            uint_fast8_t frame_number = fb_priv->is_disp_fb ? disp_fb->fb.num : 1;
#else
            uint_fast8_t frame_number = 1;
#endif
            uint_fast8_t frame_idx = info->yoffset / frame_size;
            if ((info->yoffset % frame_size) || (frame_idx >= frame_number)) {
                return -1;
            }

#if VSF_DISP_USE_FB == ENABLED
            if (fb_priv->is_disp_fb) {
                vk_disp_fb_set_front_buffer(disp, frame_idx);
            } else
#endif
            {
                VSF_LINUX_ASSERT(NULL == fb_priv->eda_pending);
                fb_priv->eda_pending = vsf_eda_get_cur();
                VSF_LINUX_ASSERT(fb_priv->eda_pending != NULL);
                vsf_thread_wfe(VSF_EVT_USER);
            }
        }
        break;
    case FBIO_WAITFORVSYNC:
        // TODO
        break;
    }
    return 0;
}

static int __vsf_linux_fb_stat(vsf_linux_fd_t *sfd, struct stat *buf)
{
    buf->st_mode = S_IFBLK;
    return 0;
}

static ssize_t __vsf_linux_fb_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_fb_priv_t *fb_priv = (vsf_linux_fb_priv_t *)sfd->priv;
    vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)fb_priv->file;
    vk_disp_t *disp = (vk_disp_t *)vfs_file->f.param;
    uint_fast32_t frame_size = disp->param.height * disp->param.width * vsf_disp_get_pixel_bytesize(disp);

    if (vfs_file->pos + count >= frame_size) {
        count = frame_size - vfs_file->pos;
    }

    memcpy(buf, (void *)((uint8_t *)fb_priv->front_buffer + vfs_file->pos), count);
    return count;
}

static ssize_t __vsf_linux_fb_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_fb_priv_t *fb_priv = (vsf_linux_fb_priv_t *)sfd->priv;
    vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)fb_priv->file;
    vk_disp_t *disp = (vk_disp_t *)vfs_file->f.param;
    uint_fast32_t frame_size = disp->param.height * disp->param.width * vsf_disp_get_pixel_bytesize(disp);

    if (vfs_file->pos + count >= frame_size) {
        count = frame_size - vfs_file->pos;
    }

    memcpy((void *)((uint8_t *)fb_priv->front_buffer + vfs_file->pos), buf, count);
#if VSF_DISP_USE_FB == ENABLED
    if (!fb_priv->is_disp_fb)
#endif
    {
        VSF_LINUX_ASSERT(NULL == fb_priv->eda_pending);
        fb_priv->eda_pending = vsf_eda_get_cur();
        VSF_LINUX_ASSERT(fb_priv->eda_pending != NULL);
        vsf_thread_wfe(VSF_EVT_USER);
    }
    return count;
}

static int __vsf_linux_fb_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_fb_priv_t *fb_priv = (vsf_linux_fb_priv_t *)sfd->priv;
    vsf_eda_fini(&fb_priv->fresh_task.use_as__vsf_eda_t);
    if (    (fb_priv->front_buffer != NULL)
#if VSF_DISP_USE_FB == ENABLED
        &&  (fb_priv->is_disp_fb)
#endif
        ) {
        free(fb_priv->front_buffer);
    }
    fb_priv->front_buffer = NULL;
    return 0;
}

static void * __vsf_linux_fb_mmap(vsf_linux_fd_t *sfd, off64_t offset, size_t len, uint_fast32_t feature)
{
    vsf_linux_fb_priv_t *fb_priv = (vsf_linux_fb_priv_t *)sfd->priv;
    vk_disp_t *disp = (vk_disp_t *)(((vk_vfs_file_t *)(fb_priv->file))->f.param);

#if VSF_DISP_USE_FB == ENABLED
    if (fb_priv->is_disp_fb) {
        fb_priv->front_buffer = vk_disp_fb_set_front_buffer(disp, 0);
        return (void *)((uintptr_t)fb_priv->front_buffer + offset);
    }
#endif
    uint_fast32_t frame_size = disp->param.height * disp->param.width * vsf_disp_get_pixel_bytesize(disp);
    if ((offset + len) > frame_size) {
        return NULL;
    }
    return (void *)((uintptr_t)fb_priv->front_buffer + offset);
}

static int __vsf_linux_fb_msync(vsf_linux_fd_t *sfd, void *buffer)
{
    vsf_linux_fb_priv_t *fb_priv = (vsf_linux_fb_priv_t *)sfd->priv;

#if VSF_DISP_USE_FB == ENABLED
    if (fb_priv->is_disp_fb) {
        return 0;
    } else
#endif
    {
        VSF_LINUX_ASSERT(buffer == fb_priv->front_buffer);
        VSF_LINUX_ASSERT(NULL == fb_priv->eda_pending);
        fb_priv->eda_pending = vsf_eda_get_cur();
        VSF_LINUX_ASSERT(fb_priv->eda_pending != NULL);
        vsf_thread_wfe(VSF_EVT_USER);
    }
    return 0;
}

static const vsf_linux_fd_op_t __vsf_linux_fb_fdop = {
    .priv_size          = sizeof(vsf_linux_fb_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_init            = __vsf_linux_fb_init,
    .fn_fcntl           = __vsf_linux_fb_fcntl,
    .fn_stat            = __vsf_linux_fb_stat,
    .fn_read            = __vsf_linux_fb_read,
    .fn_write           = __vsf_linux_fb_write,
    .fn_close           = __vsf_linux_fb_close,
    .fn_mmap            = __vsf_linux_fb_mmap,
    .fn_msync           = __vsf_linux_fb_msync,
};

int vsf_linux_fs_bind_disp(char *path, vk_disp_t *disp)
{
    return vsf_linux_fs_bind_target_ex(
                path, disp, &__vsf_linux_fb_fdop, NULL, NULL,
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
}
#endif

#if VSF_HAL_USE_GPIO == ENABLED

typedef struct vsf_linux_gpio_priv_t {
    implement(vsf_linux_fs_priv_t)

    vsf_gpio_t *port;
    uint8_t pin;
} vsf_linux_gpio_priv_t;

static const vsf_linux_fd_op_t __vsf_linux_gpio_fdop;

static void __vsf_linux_gpio_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_gpio_priv_t *gpio_priv = (vsf_linux_gpio_priv_t *)sfd->priv;
    vk_file_t *file = gpio_priv->file;

    if (    !strcmp(file->name, "direction")
        ||  !strcmp(file->name, "value")) {
        // initialize hw and pin in gpio_priv
        vk_file_t *export_file = vk_file_get_parent(file), *tmp_file;
        VSF_LINUX_ASSERT(export_file != NULL);
        int pin = atoi(&export_file->name[4]);
        tmp_file = vk_file_get_parent(export_file);
        vk_file_close(export_file);
        VSF_LINUX_ASSERT(tmp_file != NULL);
        vk_file_open(tmp_file, "export", &export_file);
        if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
            vk_file_close(tmp_file);
            VSF_LINUX_ASSERT(false);
            return;
        }
        vk_file_close(tmp_file);

        vsf_linux_gpio_chip_t *gpio_chip = ((vk_vfs_file_t *)export_file)->f.param;
        vsf_gpio_capability_t cap;
        for (uint8_t i = 0; i < gpio_chip->port_num; i++) {
            cap = vsf_gpio_capability(gpio_chip->ports[i]);
            if (pin < cap.pin_count) {
                VSF_LINUX_ASSERT(cap.avail_pin_mask & (1 << pin));
                gpio_priv->port = gpio_chip->ports[i];
                gpio_priv->pin = pin;
                break;
            }
            pin -= cap.pin_count;
        }
        VSF_LINUX_ASSERT(gpio_priv->port != NULL);
    }
}

static ssize_t __vsf_linux_gpio_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_gpio_priv_t *gpio_priv = (vsf_linux_gpio_priv_t *)sfd->priv;
    if (!strcmp(gpio_priv->file->name, "value")) {
        uint32_t value = vsf_gpio_read(gpio_priv->port);
        if (value & (1 << gpio_priv->pin)) {
            ((char *)buf)[0] = '1';
        } else {
            ((char *)buf)[0] = '0';
        }
        return 1;
    }
    return 0;
}

static ssize_t __vsf_linux_gpio_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    extern vk_file_t * __vsf_linux_fs_get_file_ex(vk_file_t *dir, const char *pathname);

    vsf_linux_gpio_priv_t *gpio_priv = (vsf_linux_gpio_priv_t *)sfd->priv;
    vk_file_t *file = gpio_priv->file;

    if (!strcmp(file->name, "export")) {
        char path[4 + 3 + 1];
        vk_file_t *dir = vk_file_get_parent(file), *gpio_dir;
        int pathlen = strlen("gpio"), err;

        strcpy(path, "gpio");
        memcpy(&path[pathlen], buf, count);
        path[pathlen + count] = '\0';

        gpio_dir = __vsf_linux_fs_get_file_ex(dir, path);
        if (gpio_dir != NULL) {
            fprintf(stderr, "%s already exported\r\n", path);
            goto fail_close_gpio_dir;
        }

        vk_file_create(dir, path, VSF_FILE_ATTR_DIRECTORY | VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE);
        if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
            fprintf(stderr, "fail to create dir %s\r\n", path);
            goto fail_close_gpio_dir;
        }
        vk_file_open(dir, path, &gpio_dir);
        if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
            fprintf(stderr, "fail to open dir %s\r\n", path);
            goto fail_close_dir;
        }
        vk_file_close(dir);

        err = vsf_linux_fs_bind_target_relative((vk_vfs_file_t *)gpio_dir, "direction",
                NULL, &__vsf_linux_gpio_fdop, VSF_FILE_ATTR_WRITE, 0);
        if (err != 0) {
            fprintf(stderr, "fail to bind %s\r\n", path);
            goto fail_close_gpio_dir;
        }
        err = vsf_linux_fs_bind_target_relative((vk_vfs_file_t *)gpio_dir, "value",
                NULL, &__vsf_linux_gpio_fdop, VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
        if (err != 0) {
            fprintf(stderr, "fail to bind %s\r\n", path);
            goto fail_close_gpio_dir;
        }
        vk_file_close(gpio_dir);
        return count;
    fail_close_dir:
        vk_file_close(dir);
    fail_close_gpio_dir:
        vk_file_close(gpio_dir);
        return -1;
    } else if (!strcmp(file->name, "unexport")) {
        char path[4 + 3 + 1];
        vk_file_t *dir = vk_file_get_parent(file), *gpio_dir;
        int pathlen = strlen("gpio");

        strcpy(path, "gpio");
        memcpy(&path[pathlen], buf, count);
        pathlen += count;
        path[pathlen] = '\0';

        gpio_dir = __vsf_linux_fs_get_file_ex(dir, path);
        if (NULL == gpio_dir) {
            fprintf(stderr, "%s not exists\r\n", path);
            return -1;
        }

        vk_file_unlink(gpio_dir, "direction");
        vk_file_unlink(gpio_dir, "value");
        vk_file_unlink(gpio_dir, "edge");
        vk_file_unlink(gpio_dir, "active_low");
        vk_file_close(gpio_dir);
        vk_file_unlink(dir, path);
        vk_file_close(dir);
        rmdir(path);
    } else if (!strcmp(file->name, "direction")) {
        if (!strcmp((char *)buf, "in")) {
            vsf_gpio_set_input(gpio_priv->port, 1 << gpio_priv->pin);
        } else if (!strcmp((char *)buf, "out")) {
            vsf_gpio_set_output(gpio_priv->port, 1 << gpio_priv->pin);
        } else if (!strcmp((char *)buf, "high")) {
            vsf_gpio_set(gpio_priv->port, 1 << gpio_priv->pin);
            vsf_gpio_set_output(gpio_priv->port, 1 << gpio_priv->pin);
        } else if (!strcmp((char *)buf, "low")) {
            vsf_gpio_clear(gpio_priv->port, 1 << gpio_priv->pin);
            vsf_gpio_set_output(gpio_priv->port, 1 << gpio_priv->pin);
        }
    } else if (!strcmp(file->name, "value")) {
        char value = *(char *)buf;
        if ('0' == value) {
            vsf_gpio_clear(gpio_priv->port, 1 << gpio_priv->pin);
        } else {
            vsf_gpio_set(gpio_priv->port, 1 << gpio_priv->pin);
        }
    }
    return count;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

static const vsf_linux_fd_op_t __vsf_linux_gpio_fdop = {
    .priv_size          = sizeof(vsf_linux_gpio_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_init            = __vsf_linux_gpio_init,
    .fn_read            = __vsf_linux_gpio_read,
    .fn_write           = __vsf_linux_gpio_write,
};

int vsf_linux_fs_bind_gpio(char *path, vsf_linux_gpio_chip_t *gpio_chip)
{
    char pathbuf[PATH_MAX];
    int pathlen = strlen(path), err;
    memcpy(pathbuf, path, pathlen + 1);
    pathbuf[pathlen++] = '/';

    mkdirs(path, 0);
    strcpy(&pathbuf[pathlen], "export");
    err = vsf_linux_fs_bind_target_ex(pathbuf, gpio_chip,
                &__vsf_linux_gpio_fdop, NULL, NULL, VSF_FILE_ATTR_WRITE, 0);
    if (err != 0) {
        fprintf(stderr, "fail to bind %s\r\n", pathbuf);
        return err;
    }
    strcpy(&pathbuf[pathlen], "unexport");
    err = vsf_linux_fs_bind_target_ex(pathbuf, gpio_chip,
                &__vsf_linux_gpio_fdop, NULL, NULL, VSF_FILE_ATTR_WRITE, 0);
    if (err != 0) {
        fprintf(stderr, "fail to bind %s\r\n", pathbuf);
        return err;
    }
    return 0;
}

#if VSF_HW_GPIO_COUNT > 0
int vsf_linux_fs_bind_gpio_hw(char *path)
{
#define __VSF_LINUX_DEF_GPIO_PORT(__N, __UNUSED)                                \
                (vsf_gpio_t *)&VSF_MCONNECT(vsf_hw_gpio, __N),
#define describe_vsf_linux_gpio_chip_hw(__name)                                 \
    struct VSF_MCONNECT(__name, _gpio_chip_t) {                                 \
        uint8_t port_num;                                                       \
        vsf_gpio_t * ports[VSF_HW_GPIO_COUNT];                                  \
    } static const __name = {                                                   \
        .port_num     = VSF_HW_GPIO_COUNT,                                      \
        .ports        = {                                                       \
            VSF_MREPEAT(VSF_HW_GPIO_COUNT, __VSF_LINUX_DEF_GPIO_PORT, NULL)     \
        },                                                                      \
    };
    describe_vsf_linux_gpio_chip_hw(__vsf_linux_gpio_chip_hw)
    return vsf_linux_fs_bind_gpio(path, (vsf_linux_gpio_chip_t *)&__vsf_linux_gpio_chip_hw);
}
#endif      // VSF_HW_GPIO_COUNT > 0
#endif      // VSF_HAL_USE_GPIO

static ssize_t __vsf_linux_null_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    return EOF;
}

static ssize_t __vsf_linux_null_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    return count;
}

static int __vsf_linux_null_stat(vsf_linux_fd_t *sfd, struct stat *buf)
{
    buf->st_mode = S_IFCHR;
    return 0;
}

static const vsf_linux_fd_op_t __vsf_linux_null_fdop = {
    .priv_size          = sizeof(vsf_linux_fs_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_read            = __vsf_linux_null_read,
    .fn_write           = __vsf_linux_null_write,
    .fn_stat            = __vsf_linux_null_stat,
};

static ssize_t __vsf_linux_zero_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    memset(buf, 0, count);
    return count;
}

static const vsf_linux_fd_op_t __vsf_linux_zero_fdop = {
    .priv_size          = sizeof(vsf_linux_fs_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_read            = __vsf_linux_zero_read,
    .fn_write           = __vsf_linux_null_write,
    .fn_stat            = __vsf_linux_null_stat,
};

int vsf_linux_devfs_init(void)
{
    int err = mkdir("/dev", 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir /dev\r\n");
        return err;
    }

    err = vsf_linux_fs_bind_target_ex("/dev/null", NULL, &__vsf_linux_null_fdop,
                NULL, NULL, VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
    if (err != 0) {
        fprintf(stderr, "fail to bind /dev/null\r\n");
        return err;
    }

    err = vsf_linux_fs_bind_target_ex("/dev/zero", NULL, &__vsf_linux_zero_fdop,
                NULL, NULL, VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
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

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#endif
