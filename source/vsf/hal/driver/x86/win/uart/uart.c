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

/*============================ INCLUDES ======================================*/

#include "./uart.h"

#if VSF_HAL_USE_USART == ENABLED

// for vsf_trace
#include "service/vsf_service.h"
#include <Windows.h>

/*============================ MACROS ========================================*/

#ifndef VSF_USART_CFG_TRACE_EN
#   define VSF_USART_CFG_TRACE_EN                   DISABLED
#endif

#ifndef VSF_USART_CFG_TRACE_FUNCTION
#   define    VSF_USART_CFG_TRACE_FUNCTION          DISABLED
#endif

#ifndef VSF_USART_CFG_TRACE_IRQ
#   define VSF_USART_CFG_TRACE_IRQ                  DISABLED
#endif

#ifndef VSF_USART_CFG_TRACE_CALLSTACK
#   define VSF_USART_CFG_TRACE_CALLSTACK            DISABLED
#endif

#ifndef VSF_USART_CFG_TRACE_SYSTIMER
#   define VSF_USART_CFG_TRACE_SYSTIMER             DISABLED
#endif

#ifndef VSF_USART_CFG_TRACE_STATUS
#   define VSF_USART_CFG_TRACE_STATUS               DISABLED
#endif

#ifndef VSF_USART_CFG_START_PORT
#   define VSF_USART_CFG_START_PORT                 1
#endif

#ifndef VSF_USART_CFG_END_PORT
#   define VSF_USART_CFG_END_PORT                   255
#endif

#ifndef VSF_USART_CFG_INPUT_BUFFER
#   define VSF_USART_CFG_INPUT_BUFFER               1024
#endif

#ifndef VSF_USART_CFG_OUTPUT_BUFFER
#   define VSF_USART_CFG_OUTPUT_BUFFER              1024
#endif

#ifndef VSF_USART_CFG_TRACE_IRQ_PEND
#   define VSF_USART_CFG_TRACE_IRQ_PEND             "irq_request_pend:"
#endif

#ifndef VSF_USART_CFG_TRACE_IRQ_GOING
#   define VSF_USART_CFG_TRACE_IRQ_GOING            "irq_request_going:"
#endif

#undef VSF_USART_CFG_TRACE_FUNC
#if VSF_USART_CFG_TRACE_EN == ENABLED
#   define VSF_USART_CFG_TRACE_FUNC(...)            printf(__VA_ARGS__)
#else
#   define VSF_USART_CFG_TRACE_FUNC(...)
#endif

#undef VSF_USART_TRACE_HEADER
#define VSF_USART_TRACE_HEADER                      "[x86/x64]:"

#define __USART_LOCAL(__N,__DONT_CARE)                                          \
    vsf_usart_t vsf_usart##__N          = {                                     \
        .com_port                       = __N,                                  \
        .com_status                     = USART_INIT_IS_BUSY,                   \
        .handle_com                     = NULL,                                 \
        .irp_cancel_read_request        = NULL,                                 \
        .irp_cancel_write_request       = NULL,                                 \
        .cancel_status                  = 0X00,                                 \
        .init_flag                      = 1,                                    \
    };


#undef vsf_usart_trace_function
#if VSF_USART_CFG_TRACE_FUNCTION == ENABLED
#   define vsf_usart_trace_function(...)                                        \
    do{                                                                         \
        VSF_USART_CFG_TRACE_FUNC("%s" VSF_USART_TRACE_HEADER,                   \
                        __vsf_x86_trace_color[VSF_USART_TRACE_FUNCTION_COLOR]); \
        VSF_USART_CFG_TRACE_FUNC(__VA_ARGS__);                                  \
        VSF_USART_CFG_TRACE_FUNC(VSF_TRACE_CFG_LINEEND);                        \
        VSF_USART_CFG_TRACE_FUNC("%s",                                          \
        __vsf_x86_trace_color[VSF_USART_TRACE_RESET_COLOR]);                    \
    }while(0)
#else
#   define vsf_usart_trace_function(...)
#endif // VSF_USART_CFG_TRACE_FUNCTION == ENABLED

#undef vsf_usart_trace_irq
#if VSF_USART_CFG_TRACE_IRQ == ENABLED
#   define vsf_usart_trace_irq(...)                                             \
    do{                                                                         \
        VSF_USART_CFG_TRACE_FUNC("%s" VSF_USART_TRACE_HEADER,                   \
                        __vsf_x86_trace_color[VSF_USART_TRACE_IRQ_COLOR]);      \
        VSF_USART_CFG_TRACE_FUNC(__VA_ARGS__);                                  \
        VSF_USART_CFG_TRACE_FUNC(VSF_TRACE_CFG_LINEEND);                        \
        VSF_USART_CFG_TRACE_FUNC(                                               \
        __vsf_x86_trace_color[VSF_USART_TRACE_RESET_COLOR]);                    \
    }while(0)
#else
#   define vsf_usart_trace_irq(...)
#endif

/*============================ TYPES =========================================*/

typedef enum em_usart_status_t {
    USART_INIT_IS_BUSY                  = 0X80U,
    USART_READ_IS_BUSY                  = 0X40U,
    USART_WRITE_IS_BUSY                 = 0X20U,
    USART_RESET                         = 0X10U,

    USART_ENABLE                        = 0X08U,
    USART_DISABLE                       = 0XF7U,

    USART_SEND_BLOCK                    = 0X02U,
    USART_SEND_BYTE                     = 0X00U,
    USART_READ_BLOCK                    = 0X01U,
    USART_READ_BYTE                     = 0X00U,
} em_usart_status_t;

typedef enum usart_cancel_status_t {
    WILL_CANCEL_WRITE                   = 0X80U,
    CANCELED_WRITE                      = 0X40U,

    WILL_CANCEL_READ                    = 0X20U,
    CANCELED_READ                       = 0X10U,

    CANCEL_READ_CLP                     = 0X08U,
    CANCEL_WRITE_CLP                    = 0X04U,

    CANCEL_INIT                         = 0X00U,
} usart_cancel_status_t;

struct vsf_usart_t {
    em_usart_irq_mask_t                 enable_flag;
    uint8_t                             com_port;
    uint8_t                             com_status;
    uint8_t                             cancel_status;
    uint8_t                             init_flag : 1;
    usart_cfg_t                         cfg;
    uint8_t                             *buf;
    uint_fast32_t                       buf_size;
    intalu_t                            sended_buf_size;
    uint8_t                             *rec_buf;
    uint_fast32_t                       rec_buf_size;
    intalu_t                            reced_buf_size;
    vsf_arch_irq_thread_t               irq_init_thread;
    HANDLE                              handle_com;
    vsf_arch_irq_thread_t               irq_read_thread;
    vsf_arch_irq_thread_t               irq_write_thread;
    vsf_arch_irq_request_t              irq_read_request;
    vsf_arch_irq_request_t              irq_write_request;
    vsf_arch_irq_request_t              *irp_cancel_read_request;
    vsf_arch_irq_request_t              *irp_cancel_write_request;
};

typedef struct x86_usart_win_t {
    vsf_arch_irq_thread_t               irq_scan_thread;
    vsf_arch_irq_request_t              irq_scan_request;
    uint8_t                             *ret;
    uint8_t                             is_busy : 1;
    uint8_t                             is_scan_inited : 1;
    vsf_arch_irq_request_t              irq_init_request;
    struct {
        uint8_t                         port[VSF_USART_CFG_PORT_NUM];
        uint8_t                         num;
    };
} x86_usart_win_t;

/*============================ PROTOTYPES ====================================*/

static void __vk_usart_read_event_thread(void *arg);
static void __vk_usart_write_event_thread(void *arg);

/*============================ LOCAL VARIABLES ===============================*/

#if     VSF_USART_CFG_TRACE_IRQ == ENABLED                                      \
    ||  VSF_USART_CFG_TRACE_CALLSTACK == ENABLED                                \
    ||  VSF_USART_CFG_TRACE_SYSTIMER == ENABLED                                 \
    ||  VSF_USART_CFG_TRACE_STATUS == ENABLED                                   \
    ||  VSF_USART_CFG_TRACE_FUNCTION == ENABLED
enum {
    VSF_USART_TRACE_CALLSTACK_COLOR,
    VSF_USART_TRACE_IRQ_COLOR,
    VSF_USART_TRACE_SYSTIMER_COLOR,
    VSF_USART_TRACE_STATUS_COLOR,
    VSF_USART_TRACE_FUNCTION_COLOR,
    VSF_USART_TRACE_RESET_COLOR,
};
static const char *__vsf_x86_trace_color[] = {
    [VSF_USART_TRACE_CALLSTACK_COLOR] = "\033[1;32m",
    [VSF_USART_TRACE_IRQ_COLOR]         = "\033[1;33m",
    [VSF_USART_TRACE_SYSTIMER_COLOR]    = "\033[1;35m",
    [VSF_USART_TRACE_STATUS_COLOR]      = "\033[1;36m",
    [VSF_USART_TRACE_FUNCTION_COLOR]    = "\033[1;34m",
    [VSF_USART_TRACE_RESET_COLOR]       = "\033[1;37m",
};
#endif

static x86_usart_win_t __x86_usart_win = {
    .is_busy = 1,
};
/*============================ GLOBAL VARIABLES ==============================*/
REPEAT_MACRO(VSF_USART_CFG_PORT_NUM, __USART_LOCAL, NULL)
/*============================ IMPLEMENTATION ================================*/
static DWORD  __calculate_rx_timeout(vsf_usart_t *usart, uint32_t times)
{
    if (0 == times) {
        return 0;
    }
    uint8_t bits = 1;//start bit
    DWORD ret_timeout = 0;
    bits += usart->cfg.mode & USART_9_BIT_LENGTH ? 9 : 8;
    bits += usart->cfg.mode & USART_2_STOPBIT ? 2 : 1;
    ret_timeout = (DWORD)(1000 / (double)(usart->cfg.baudrate) * bits * times);
    return ret_timeout;
}
static void __vk_usart_scan_event_thread(void *arg)
{
    vsf_usart_trace_irq("%s(0x"VSF_TRACE_POINTER_HEX")This hardware thread will not exit", __FUNCTION__, arg);
    vsf_arch_irq_thread_t *irq_thread = arg;
    x86_usart_win_t *pret = container_of(irq_thread, x86_usart_win_t, irq_scan_thread);
    __vsf_arch_irq_set_background(irq_thread);

    HANDLE hCom;
    char file[16] = {0};
    uint8_t i, j;
    for (i = 0; i < VSF_USART_CFG_PORT_NUM; i++) {
        __x86_usart_win.port[i] = 0;
    }
    __x86_usart_win.num = 0;
    while (1) {
        vsf_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_PEND"[%s]line(%d)", __FUNCTION__, __LINE__);
        __vsf_arch_irq_request_pend(&pret->irq_scan_request);
        vsf_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_GOING"[%s]line(%d)", __FUNCTION__, __LINE__);
        for (i = VSF_USART_CFG_START_PORT; (i < VSF_USART_CFG_END_PORT) && (__x86_usart_win.num < VSF_USART_CFG_PORT_NUM); i++) {
            sprintf(file, "\\\\.\\com%d", i);
            hCom = CreateFileA(file, GENERIC_WRITE | GENERIC_READ, 0, NULL,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
            vsf_usart_trace_irq("[%s]line(%d)CreateFileA[return("VSF_TRACE_POINTER_HEX")]", __FUNCTION__, __LINE__, hCom);
            if (hCom != INVALID_HANDLE_VALUE) {
                for (j = 0; j < VSF_USART_CFG_PORT_NUM; j++) {//check
                    vsf_usart_trace_irq("[%s]line(%d)check{__x86_usart_win.port[%d] = (%d)}", __FUNCTION__, __LINE__, j, i);
                    if (__x86_usart_win.port[j] == i) {
                        goto close_handle;
                    }
                }
                for (j = 0; j < VSF_USART_CFG_PORT_NUM; j++) {//push
                    vsf_usart_trace_irq("[%s]line(%d)push{__x86_usart_win.port[%d] = (%d)}", __FUNCTION__, __LINE__, j, i);
                    if (__x86_usart_win.port[j] == 0) {
                        __x86_usart_win.port[j] = i;
                        __x86_usart_win.num++;
                        break;
                    }
                }
            } else if (ERROR_ACCESS_DENIED == GetLastError()) {
                goto close_handle;
            } else {
                for (j = 0; j < VSF_USART_CFG_PORT_NUM; j++) {//pop
                    vsf_usart_trace_irq("[%s]line(%d)pop{__x86_usart_win.port[%d] = (%d)}", __FUNCTION__, __LINE__, j, i);
                    if (__x86_usart_win.port[j] == i) {
                        __x86_usart_win.port[j] = 0;
                        __x86_usart_win.num--;
                        break;
                    }
                }
            }
        close_handle:
            CloseHandle(hCom);
            *__x86_usart_win.ret = __x86_usart_win.num;
        }
        __x86_usart_win.is_busy = 0;
        for (int i=0;i < VSF_USART_CFG_PORT_NUM;i++) {
            VSF_USART_CFG_TRACE_FUNC("[%s]line(%d)com%d---vsf_usart%d\n", __FUNCTION__, __LINE__, __x86_usart_win.port[i], i);
        }
    }
    __vsf_arch_irq_fini(irq_thread);
}

static vsf_err_t __usart_init(vsf_usart_t *usart)
{
    bool res_bool;
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, usart);
    if (!__x86_usart_win.port[usart->com_port]) {
        vsf_trace_error("initialization failed!This hardware thread will be terminated!\n");
        vsf_usart_trace_function("%s line(%d)exited VSF_ERR_FAIL", __FUNCTION__, __LINE__);
        res_bool = CloseHandle(usart->handle_com);
        usart->handle_com = INVALID_HANDLE_VALUE;
        return VSF_ERR_FAIL;
    }
    HANDLE handle_com;
#if VSF_USART_CFG_TRACE_EN == ENABLED
    DWORD res_dword = 0;
#endif
    char file[16] = {0};

    sprintf(file, "\\\\.\\com%d", __x86_usart_win.port[usart->com_port]);
    VSF_USART_CFG_TRACE_FUNC("[%s]line(%d)file(%s)", __FUNCTION__, __LINE__, file);
    handle_com = CreateFileA(file, GENERIC_WRITE | GENERIC_READ, 0, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
#if VSF_USART_CFG_TRACE_EN == ENABLED
    res_dword = GetLastError();
#endif
    VSF_USART_CFG_TRACE_FUNC("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, res_dword);
    usart->handle_com = handle_com;
    res_bool = SetupComm(handle_com,
                         VSF_USART_CFG_INPUT_BUFFER,
                         VSF_USART_CFG_OUTPUT_BUFFER);
    if (!res_bool) {
#if VSF_USART_CFG_TRACE_EN == ENABLED
        res_dword = GetLastError();
#endif
        VSF_USART_CFG_TRACE_FUNC("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, res_dword);
        vsf_trace_warning("cannot setup %s--vsf_usart%d\n", file, usart->com_port);
        vsf_usart_trace_function("%s line(%d)exited VSF_ERR_FAIL", __FUNCTION__, __LINE__);
        res_bool = CloseHandle(usart->handle_com);
        usart->handle_com = INVALID_HANDLE_VALUE;
        return VSF_ERR_FAIL;
    }
    COMMTIMEOUTS timeOuts;
    timeOuts.ReadIntervalTimeout = __calculate_rx_timeout(usart, usart->cfg.rx_timeout);
    timeOuts.ReadTotalTimeoutConstant = 1;
    timeOuts.ReadTotalTimeoutMultiplier = 1;
    res_bool = SetCommTimeouts(handle_com, &timeOuts);
    if (!res_bool) {
#if VSF_USART_CFG_TRACE_EN == ENABLED
        res_dword = GetLastError();
#endif
        VSF_USART_CFG_TRACE_FUNC("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, res_dword);
        vsf_trace_warning("cannot set CommTimeouts %s--vsf_usart%d\n", file, usart->com_port);
        vsf_usart_trace_function("%s line(%d)exited VSF_ERR_FAIL", __FUNCTION__, __LINE__);
        res_bool = CloseHandle(usart->handle_com);
        usart->handle_com = INVALID_HANDLE_VALUE;
        return VSF_ERR_FAIL;
    }

    DCB dcb;
    res_bool = GetCommState(handle_com, &dcb);
    if (!res_bool) {
#if VSF_USART_CFG_TRACE_EN == ENABLED
        res_dword = GetLastError();
#endif
        VSF_USART_CFG_TRACE_FUNC("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, res_dword);
        vsf_trace_warning("cannot get CommState %s--vsf_usart%d\n", file, usart->com_port);
        vsf_usart_trace_function("%s line(%d)exited VSF_ERR_FAIL", __FUNCTION__, __LINE__);
        res_bool = CloseHandle(usart->handle_com);
        usart->handle_com = INVALID_HANDLE_VALUE;
        return VSF_ERR_FAIL;
    }
    dcb.fAbortOnError = FALSE;
    dcb.BaudRate = usart->cfg.baudrate;
    dcb.ByteSize = USART_9_BIT_LENGTH & usart->cfg.mode ? 9 : 8;
    dcb.Parity = USART_ODD_PARITY & usart->cfg.mode ? 1 :
        (USART_EVEN_PARITY & usart->cfg.mode ? 2 : 0);
    dcb.StopBits = USART_2_STOPBIT & usart->cfg.mode ? 2 : 0;
    dcb.fRtsControl = FALSE;
    dcb.fOutxCtsFlow = FALSE;
    if ((USART_RTS_CTS_HWCONTROL & usart->cfg.mode) == USART_RTS_CTS_HWCONTROL) {
        dcb.fRtsControl = TRUE;
        dcb.fOutxCtsFlow = TRUE;
    } else if ((USART_CTS_HWCONTROL & usart->cfg.mode) == USART_CTS_HWCONTROL) {
        dcb.fOutxCtsFlow = TRUE;
    } else if ((USART_RTS_HWCONTROL & usart->cfg.mode) == USART_RTS_HWCONTROL) {
        dcb.fRtsControl = TRUE;
    }
    res_bool = SetCommState(handle_com, &dcb);
    if (!res_bool) {
#if VSF_USART_CFG_TRACE_EN == ENABLED
        res_dword = GetLastError();
#endif
        VSF_USART_CFG_TRACE_FUNC("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, res_dword);
        vsf_trace_warning("cannot set CommState %s--vsf_usart%d\n", file, usart->com_port);
        vsf_usart_trace_function("%s line(%d)exited VSF_ERR_FAIL", __FUNCTION__, __LINE__);
        res_bool = CloseHandle(usart->handle_com);
        usart->handle_com = INVALID_HANDLE_VALUE;
        return VSF_ERR_FAIL;
    }

    PurgeComm(handle_com, PURGE_TXCLEAR | PURGE_RXCLEAR);
    ClearCommError(handle_com, NULL, NULL);

    usart->com_status &= (~USART_INIT_IS_BUSY);

    if (usart->init_flag) {
        usart->init_flag = 0;
        __vsf_arch_irq_request_init(&usart->irq_write_request);
        __vsf_arch_irq_request_init(&usart->irq_read_request);
        /* start read */
        __vsf_arch_irq_init(&usart->irq_read_thread, "vsf_usart_event\n",
                            __vk_usart_read_event_thread, usart->cfg.isr.prio);
        /* start write */
        __vsf_arch_irq_init(&usart->irq_write_thread, "vsf_usart_event\n",
                            __vk_usart_write_event_thread, usart->cfg.isr.prio);
    } else {
        usart->com_status &= ~USART_RESET;
    }
    vsf_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
    return VSF_ERR_NONE;
}

static void __vk_usart_read_event_thread(void *arg)
{
    vsf_usart_trace_irq("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, arg);
    vsf_arch_irq_thread_t *irq_thread = arg;
    vsf_usart_t *usart = container_of(irq_thread, vsf_usart_t, irq_read_thread);

    __vsf_arch_irq_set_background(irq_thread);

    BOOL read_ret = false;
    DWORD read_len = 0;
    DWORD read_real_len = 0;
    OVERLAPPED overLapped;

    while (1) {
    pend_read:
        vsf_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_PEND"[%s]line(%d)", __FUNCTION__, __LINE__);
        __vsf_arch_irq_request_pend(&usart->irq_read_request);
        vsf_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_GOING"[%s]line(%d)", __FUNCTION__, __LINE__);
        read_real_len = 0;
        usart->reced_buf_size = read_real_len;
    rec_buf_lenth_insufficient:
        if (NULL != usart->irp_cancel_read_request) {
            __vsf_arch_irq_request_send(usart->irp_cancel_read_request);
            vsf_usart_trace_irq("[%s]line(%d)read will cancel", __FUNCTION__, __LINE__);
            goto pend_read;
        }
        memset(&overLapped, 0, sizeof(OVERLAPPED));
        read_len = 0;
        overLapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        read_ret = ReadFile(usart->handle_com, usart->rec_buf + read_real_len,
                            usart->rec_buf_size - read_real_len, &read_len, &overLapped);
        if (!read_ret) {
            DWORD rres = GetLastError();
            vsf_usart_trace_irq("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, rres);
            if (rres == ERROR_IO_PENDING) {
                GetOverlappedResult(usart->handle_com, &overLapped, &read_len, TRUE);
            } else {
                if ((usart->enable_flag & USART_IRQ_MASK_RX_ERR) && NULL != usart->cfg.isr.handler_fn) {
                    usart->enable_flag &= ~USART_IRQ_MASK_TX_ERR;
                    CloseHandle(usart->handle_com);
                    usart->handle_com = INVALID_HANDLE_VALUE;
                    usart->com_status &= (~USART_READ_IS_BUSY);
                    vsf_usart_irq_disable(usart, 0XFF);
                    vsf_usart_disable(usart);
                    vsf_usart_trace_irq(VSF_TRACE_POINTER_HEX"-->|com%d|  [%s]line(%d)read err call("VSF_TRACE_POINTER_HEX")",
                                        usart, usart->com_port, __FUNCTION__, __LINE__, usart->cfg.isr.handler_fn);
                    __vsf_arch_irq_start(irq_thread);
                    usart->cfg.isr.handler_fn(usart->cfg.isr.target_ptr, usart, USART_IRQ_MASK_ERR);
                    __vsf_arch_irq_end(irq_thread, false);
                } else {
                    vsf_trace_error(VSF_TRACE_POINTER_HEX"-->|com%d|  error with read!%s%d\n",
                              usart, usart->com_port, __FILE__, __LINE__);
                }
                goto pend_read;
            }
        } else {
            if (!GetLastError()) {
                vsf_usart_trace_irq("read to the file eof!%s%d\n", __FILE__, __LINE__);
            } else {
                if ((usart->enable_flag & USART_IRQ_MASK_RX_ERR) && NULL != usart->cfg.isr.handler_fn) {
                    usart->enable_flag &= ~USART_IRQ_MASK_TX_ERR;
                    CloseHandle(usart->handle_com);
                    usart->handle_com = INVALID_HANDLE_VALUE;
                    usart->com_status &= (~USART_READ_IS_BUSY);

                    vsf_usart_irq_disable(usart, 0XFF);
                    vsf_usart_disable(usart);
                    vsf_usart_trace_irq(VSF_TRACE_POINTER_HEX"-->|com%d|  [%s]line(%d)read err call("VSF_TRACE_POINTER_HEX")",
                                        usart, usart->com_port, __FUNCTION__, __LINE__, usart->cfg.isr.handler_fn);
                    __vsf_arch_irq_start(irq_thread);
                    usart->cfg.isr.handler_fn(usart->cfg.isr.target_ptr, usart, USART_IRQ_MASK_ERR);
                    __vsf_arch_irq_end(irq_thread, false);
                } else {
                    vsf_trace_error(VSF_TRACE_POINTER_HEX"-->|com%d|  error with read!%s%d\n", usart, usart->com_port, __FILE__, __LINE__);
                }
                goto pend_read;
            }
        }
        read_real_len += read_len;
        if (usart->rec_buf_size != read_real_len) {
            usart->reced_buf_size = read_real_len;
            goto rec_buf_lenth_insufficient;
        }
        vsf_usart_trace_irq("read_thread onece!the recbuf:\n");
        for (int temp = 0; temp < usart->rec_buf_size; temp++) {
            VSF_USART_CFG_TRACE_FUNC("%x  ", *(usart->rec_buf + temp));
        }
        VSF_USART_CFG_TRACE_FUNC("\n");
        usart->com_status &= (~USART_READ_IS_BUSY);
        if ((usart->enable_flag & USART_IRQ_MASK_RX_CPL) && (NULL != usart->cfg.isr.handler_fn)) {
            vsf_usart_trace_irq("%s(%d) call %p\n", __FUNCTION__, __LINE__, usart->cfg.isr.handler_fn);
            __vsf_arch_irq_start(irq_thread);
            usart->cancel_status |= CANCEL_READ_CLP;
            usart->cfg.isr.handler_fn(usart->cfg.isr.target_ptr, usart, USART_IRQ_MASK_RX_CPL);
            __vsf_arch_irq_end(irq_thread, false);

        }
        usart->reced_buf_size = read_real_len;
        if (NULL != usart->irp_cancel_read_request) {
            __vsf_arch_irq_request_send(usart->irp_cancel_read_request);
            vsf_usart_trace_irq("[%s]line(%d)read will cancel", __FUNCTION__, __LINE__);
            goto pend_read;
        }
    }
    __vsf_arch_irq_fini(irq_thread);
}

static void __vk_usart_write_event_thread(void *arg)
{
    vsf_usart_trace_irq("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, arg);
    vsf_arch_irq_thread_t *irq_thread = arg;
    vsf_usart_t *usart = container_of(irq_thread, vsf_usart_t, irq_write_thread);

    __vsf_arch_irq_set_background(irq_thread);

    BOOL write_ret;
    unsigned long write_len = 0;
    OVERLAPPED overLapped;

    while (1) {
    pend_write:
        vsf_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_PEND"[%s]line(%d)", __FUNCTION__, __LINE__);
        __vsf_arch_irq_request_pend(&usart->irq_write_request);
        vsf_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_GOING"[%s]line(%d)", __FUNCTION__, __LINE__);
        write_len = 0;
        usart->sended_buf_size = write_len;

        if (NULL != usart->irp_cancel_write_request) {
            __vsf_arch_irq_request_send(usart->irp_cancel_write_request);
            vsf_usart_trace_irq("[%s]line(%d)write will cancel", __FUNCTION__, __LINE__);
            goto pend_write;
        }
        vsf_usart_trace_irq("The data to be write(%p) is:\n", usart->buf);
        if (NULL != usart->buf) {
            for (int temp = 0; temp < usart->buf_size; temp++) {
                VSF_USART_CFG_TRACE_FUNC("%x  ", *(usart->buf + temp));
            }
            VSF_USART_CFG_TRACE_FUNC("\n");
        }
        memset(&overLapped, 0, sizeof(OVERLAPPED));
        overLapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        write_ret = WriteFile(usart->handle_com, usart->buf,
                              usart->buf_size, &write_len, &overLapped);
        if (!write_ret) {
            if (GetLastError() == ERROR_IO_PENDING) {
            //tips: some virtual serial ports will be stuck in the following api
                GetOverlappedResult(usart->handle_com, &overLapped, &write_len, TRUE);
            }
        } else {
            if ((usart->enable_flag & USART_IRQ_MASK_TX_ERR) && NULL != usart->cfg.isr.handler_fn) {
                usart->enable_flag &= ~USART_IRQ_MASK_RX_ERR;

                CloseHandle(usart->handle_com);
                usart->handle_com = INVALID_HANDLE_VALUE;
                usart->com_status &= (~USART_WRITE_IS_BUSY);
                vsf_usart_irq_disable(usart, 0XFF);
                vsf_usart_disable(usart);

                vsf_usart_trace_irq(VSF_TRACE_POINTER_HEX"-->|com%d|  [%s]line(%d)write err call("VSF_TRACE_POINTER_HEX")",
                                    usart, usart->com_port, __FUNCTION__, __LINE__, usart->cfg.isr.handler_fn);
                __vsf_arch_irq_start(irq_thread);
                usart->cfg.isr.handler_fn(usart->cfg.isr.target_ptr, usart, USART_IRQ_MASK_ERR);
                __vsf_arch_irq_end(irq_thread, false);
            }
            goto pend_write;
        }
        vsf_usart_trace_irq("The data  writed(%p) is:\n", usart->buf);
        if (NULL != usart->buf) {
            for (int temp = 0; temp < usart->buf_size; temp++) {
                VSF_USART_CFG_TRACE_FUNC("%x  ", *(usart->buf + temp));
            }
            VSF_USART_CFG_TRACE_FUNC("\n");
        }
        usart->com_status &= (~USART_WRITE_IS_BUSY);

        if ((usart->enable_flag & USART_IRQ_MASK_TX_CPL) && (NULL != usart->cfg.isr.handler_fn)) {
            vsf_usart_trace_irq("%s(%d) call %p\n", __FUNCTION__, __LINE__, usart->cfg.isr.handler_fn);
            __vsf_arch_irq_start(irq_thread);
            usart->cancel_status |= CANCEL_WRITE_CLP;
            usart->cfg.isr.handler_fn(usart->cfg.isr.target_ptr, usart, USART_IRQ_MASK_TX_CPL);
            __vsf_arch_irq_end(irq_thread, false);
        }
        usart->sended_buf_size = write_len;
        if (NULL != usart->irp_cancel_write_request) {
            __vsf_arch_irq_request_send(usart->irp_cancel_write_request);
            vsf_usart_trace_irq("[%s]line(%d)write will cancel", __FUNCTION__, __LINE__);
            goto pend_write;
        }
    }
    __vsf_arch_irq_fini(irq_thread);
}

vsf_err_t vsf_usart_init(vsf_usart_t *usart, usart_cfg_t *cfg)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX",0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, usart, cfg);
    bool b;
    VSF_HAL_ASSERT(cfg != NULL);
    VSF_HAL_ASSERT(usart != NULL);
    if (__x86_usart_win.is_busy) {
        vsf_usart_trace_function("%s exited VSF_ERR_FAIL", __FUNCTION__);
        return VSF_ERR_FAIL;
    }
    if (NULL == usart->handle_com || INVALID_HANDLE_VALUE == usart->handle_com) {
        usart->com_status |= USART_INIT_IS_BUSY;
    } else {
        uint8_t res;
        usart->com_status = USART_INIT_IS_BUSY | USART_RESET;
        while ((usart->com_status & USART_READ_IS_BUSY) && (usart->com_status & USART_WRITE_IS_BUSY));
        VSF_USART_CFG_TRACE_FUNC("CloseHandle:%p\n", usart->handle_com);
        b = CloseHandle(usart->handle_com);
        VSF_USART_CFG_TRACE_FUNC("CloseHandle:%d^^%p\n", b, usart->handle_com);
        vsf_usart_get_can_used_port(&res);
        while (vsf_usart_port_isbusy());
    }
    usart->handle_com = INVALID_HANDLE_VALUE;

    usart->buf = NULL;
    usart->cfg = *cfg;
    usart->enable_flag = 0X00;

    return __usart_init(usart);
}

fsm_rt_t vsf_usart_enable(vsf_usart_t *usart)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, usart);
    VSF_HAL_ASSERT(usart != NULL);
    usart->com_status |= USART_ENABLE;
    vsf_usart_trace_function("%s exited fsm_rt_cpl", __FUNCTION__);
    return fsm_rt_cpl;
}

fsm_rt_t vsf_usart_disable(vsf_usart_t *usart)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, usart);
    VSF_HAL_ASSERT(usart != NULL);
    usart->com_status &= USART_DISABLE;
    vsf_usart_trace_function("%s exited fsm_rt_cpl", __FUNCTION__);
    return fsm_rt_cpl;
}

void vsf_usart_irq_enable(vsf_usart_t *usart, em_usart_irq_mask_t irq_mask)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX",%d)", __FUNCTION__, usart, irq_mask);
    VSF_HAL_ASSERT(usart != NULL);
    VSF_HAL_ASSERT(usart->cfg.isr.handler_fn != NULL);
    usart->enable_flag |= irq_mask;
    vsf_usart_trace_function("%s exited", __FUNCTION__);
}

void vsf_usart_irq_disable(vsf_usart_t *usart, em_usart_irq_mask_t irq_mask)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX",%d)", __FUNCTION__, usart, irq_mask);
    VSF_HAL_ASSERT(usart != NULL);
    usart->enable_flag &= ~irq_mask;
    vsf_usart_trace_function("%s exited", __FUNCTION__);
}

usart_status_t vsf_usart_status(vsf_usart_t *usart)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, usart);
    VSF_HAL_ASSERT(usart != NULL);
    usart_status_t ret;
    if ((usart->com_status & USART_INIT_IS_BUSY)
        || (usart->com_status & USART_READ_IS_BUSY)
        || (usart->com_status & USART_WRITE_IS_BUSY)) {
        ret.is_busy = 1;
    } else {
        ret.is_busy = 0;
    }
    vsf_usart_trace_function("%s exited", __FUNCTION__);
    return ret;
}

uint_fast16_t vsf_usart_fifo_read(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t number)
{
    return 0;
    //TODO:
    /*
    if (!(usart->com_status & USART_ENABLE)) {
        return false;
    }
    if (usart->com_status & USART_READ_IS_BUSY) {
        return false;
    }
    if (usart->com_status & USART_INIT_IS_BUSY) {
        return fsm_rt_asyn;
    }
#if VSF_USART_CFG_TRACE_EN == ENABLED
    VSF_USART_CFG_TRACE_FUNC("vsf_usart_read_byte:file:%sline:%d", __FILE__, __LINE__);
#endif // VSF_USART_CFG_TRACE_EN

    usart->com_status |= USART_READ_IS_BUSY;
    usart->on_status.is_busy = 1;

    //usart->com_status |= USART_READ_BYTE;
    usart->buf = buffer_ptr;
    usart->buf_size = buffer_ptr;
    __vsf_arch_irq_request_send(&usart->irq_read_request);

    return true;
    */
}

uint_fast16_t vsf_usart_fifo_write(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t number)
{
    //TODO:
    return 0;
    /*
    if (!(usart->com_status & USART_ENABLE)) {
        return false;
    }
    if (usart->com_status & USART_WRITE_IS_BUSY) {
        return false;
    }
    if (usart->com_status & USART_INIT_IS_BUSY) {
        return fsm_rt_asyn;
    }
#if VSF_USART_CFG_TRACE_EN == ENABLED
    VSF_USART_CFG_TRACE_FUNC("vsf_usart_write_byte:file:%sline:%d", __FILE__, __LINE__);
#endif // VSF_USART_CFG_TRACE_EN

    usart->com_status |= USART_WRITE_IS_BUSY;
    usart->on_status.is_busy = 1;

    //usart->com_status |= USART_SEND_BYTE;
    usart->buf = &byte;
    usart->buf_size = 1;
    __vsf_arch_irq_request_send(&usart->irq_write_request);

    return true;
    */
}

bool vsf_usart_fifo_flush(vsf_usart_t *usart_ptr)
{
    //TODO:
    return false;
}

vsf_err_t vsf_usart_request_rx(vsf_usart_t *usart, void *buffer, uint_fast32_t number)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX",0x"VSF_TRACE_POINTER_HEX",%d)", __FUNCTION__, usart, buffer, number);
    VSF_HAL_ASSERT(usart != NULL);

    if (!(usart->com_status & USART_ENABLE)) {
        vsf_usart_trace_function("%s exited VSF_ERR_NOT_ACCESSABLE", __FUNCTION__);
        return VSF_ERR_NOT_ACCESSABLE;
    }
    if (usart->com_status & USART_READ_IS_BUSY) {
        vsf_usart_trace_function("%s exited VSF_ERR_REQ_ALREADY_REGISTERED", __FUNCTION__);
        return VSF_ERR_REQ_ALREADY_REGISTERED;
    }
    usart->com_status |= USART_READ_IS_BUSY;

    usart->rec_buf = buffer;
    usart->rec_buf_size = number;
    __vsf_arch_irq_request_send(&usart->irq_read_request);
    vsf_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_usart_request_tx(vsf_usart_t *usart, void *buffer, uint_fast32_t number)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX",0x"VSF_TRACE_POINTER_HEX",%d)", __FUNCTION__, usart, buffer, number);
    VSF_HAL_ASSERT(usart != NULL);
    if (!(usart->com_status & USART_ENABLE)) {
        vsf_usart_trace_function("%s exited VSF_ERR_NOT_ACCESSABLE", __FUNCTION__);
        return VSF_ERR_NOT_ACCESSABLE;
    }
    if (usart->com_status & USART_WRITE_IS_BUSY) {
        vsf_usart_trace_function("%s exited VSF_ERR_REQ_ALREADY_REGISTERED", __FUNCTION__);
        return VSF_ERR_REQ_ALREADY_REGISTERED;
    }
    usart->com_status |= USART_WRITE_IS_BUSY;

    usart->buf = buffer;
    usart->buf_size = number;
    __vsf_arch_irq_request_send(&usart->irq_write_request);
    vsf_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
    return VSF_ERR_NONE;
}

bool vsf_usart_port_isbusy()
{
    vsf_usart_trace_function("%s", __FUNCTION__);
    vsf_usart_trace_function("%s exited %s", __FUNCTION__, __x86_usart_win.is_busy == 1 ? "true" : "false");
    return __x86_usart_win.is_busy == 1;
}

void vsf_usart_get_can_used_port(uint8_t *available_number_port)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, available_number_port);
    __x86_usart_win.ret = available_number_port;
    __x86_usart_win.is_busy = 1;
    if (!__x86_usart_win.is_scan_inited) {
        __x86_usart_win.is_scan_inited = true;
        __vsf_arch_irq_request_init(&__x86_usart_win.irq_scan_request);
        __vsf_arch_irq_request_init(&__x86_usart_win.irq_init_request);
        __vsf_arch_irq_init(&__x86_usart_win.irq_scan_thread, "vsf_usart_event\n",
                            __vk_usart_scan_event_thread, vsf_arch_prio_0);
    }
    __vsf_arch_irq_request_send(&__x86_usart_win.irq_scan_request);
    vsf_usart_trace_function("%s exited", __FUNCTION__);
}

bool vsf_usart_get_com_num(vsf_usart_win_expression_t arr[], uint8_t size)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")size(%d)", __FUNCTION__, &arr, size);
    VSF_USART_CFG_TRACE_FUNC("%s(%d)calling\n", __FUNCTION__, __LINE__);
    if (__x86_usart_win.is_busy) {
        VSF_USART_CFG_TRACE_FUNC("__x86_usart_win: is busy %sline:%d\n", __FILE__, __LINE__);
        vsf_usart_trace_function("%s exited false", __FUNCTION__);
        return false;
    }
    static vsf_usart_t *__vsf_usart_win[] = {
        &vsf_usart0,&vsf_usart1,&vsf_usart2,&vsf_usart3,
        &vsf_usart4,&vsf_usart5,&vsf_usart6,&vsf_usart7
    };
    uint8_t i, j = 0;
    for (i = 0; i < VSF_USART_CFG_PORT_NUM; i++) {
        if (0 != __x86_usart_win.port[i]) {
            arr[j].win_serial_port_num = __x86_usart_win.port[i];
            arr[j].vsf_usart_instance_ptr = __vsf_usart_win[i];
            j++;
        }
    }
#if VSF_USART_CFG_TRACE_EN == ENABLED
    for (int temp = 0; temp < VSF_USART_CFG_PORT_NUM; temp++) {
        if (__x86_usart_win.port[temp] != 0) {
            VSF_USART_CFG_TRACE_FUNC("com%d---vsf_usart%d("VSF_TRACE_POINTER_HEX")\n",
                                     __x86_usart_win.port[temp], temp, __vsf_usart_win[temp]);
        }
    }
#endif // VSF_USART_CFG_TRACE_EN
#if VSF_USART_CFG_TRACE_EN == ENABLED
    vsf_usart_trace_function("%s exited true", __FUNCTION__);
#endif
    return true;
}

vsf_err_t vsf_usart_cancel_rx(vsf_usart_t *usart)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, usart);
    VSF_HAL_ASSERT(usart != NULL);
    if (NULL == usart->handle_com) {
        vsf_usart_trace_function("%s exited VSF_ERR_NOT_AVAILABLE", __FUNCTION__);
        return VSF_ERR_NONE;
    }
    if (NULL != usart->irp_cancel_read_request) {
        vsf_usart_trace_function("%s exited VSF_ERR_NOT_ACCESSABLE", __FUNCTION__);
        return VSF_ERR_NOT_ACCESSABLE;
    }
    if (usart->com_status & USART_INIT_IS_BUSY) {
        vsf_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
        return VSF_ERR_NONE;
    }
    if (!(usart->com_status & USART_READ_IS_BUSY)) {
        vsf_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
        return VSF_ERR_NONE;
    }
    if (usart->cancel_status & CANCEL_READ_CLP) {
        usart->cancel_status &= ~CANCEL_READ_CLP;
        vsf_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
        return VSF_ERR_NONE;
    }
    vsf_arch_irq_request_t read_cancel_request = {0};
    __vsf_arch_irq_request_init(&read_cancel_request);

    usart->irp_cancel_read_request = &read_cancel_request;

    __vsf_arch_irq_request_pend(usart->irp_cancel_read_request);
    usart->irp_cancel_read_request = NULL;
    vsf_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_usart_cancel_tx(vsf_usart_t *usart)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, usart);
    VSF_HAL_ASSERT(usart != NULL);
    if (NULL == usart->handle_com) {
        vsf_usart_trace_function("%s line(%d) exited VSF_ERR_NOT_AVAILABLE", __FUNCTION__, __LINE__);
        return VSF_ERR_NONE;
    }
    if (NULL != usart->irp_cancel_write_request) {
        vsf_usart_trace_function("%s line(%d) exited VSF_ERR_NOT_ACCESSABLE", __FUNCTION__, __LINE__);
        return VSF_ERR_NOT_ACCESSABLE;
    }
    if (usart->com_status & USART_INIT_IS_BUSY) {
        vsf_usart_trace_function("%s line(%d) exited VSF_ERR_NOT_ACCESSABLE", __FUNCTION__, __LINE__);
        return VSF_ERR_NOT_ACCESSABLE;
    }
    if (!(usart->com_status & USART_WRITE_IS_BUSY)) {
        vsf_usart_trace_function("%s line(%d) exited VSF_ERR_NONE", __FUNCTION__, __LINE__);
        return VSF_ERR_NONE;
    }
    if (usart->cancel_status & CANCEL_WRITE_CLP) {
        usart->cancel_status &= ~CANCEL_WRITE_CLP;
        vsf_usart_trace_function("%s line(%d) exited VSF_ERR_NONE", __FUNCTION__, __LINE__);
        return VSF_ERR_NONE;
    }
    vsf_arch_irq_request_t write_cancel_request = {0};
    __vsf_arch_irq_request_init(&write_cancel_request);
    usart->irp_cancel_write_request = &write_cancel_request;

    __vsf_arch_irq_request_pend(usart->irp_cancel_write_request);
    usart->irp_cancel_write_request = NULL;
    vsf_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
    return VSF_ERR_NONE;
}

intalu_t vsf_usart_get_rx_count(vsf_usart_t *usart)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, usart);
    VSF_HAL_ASSERT(usart != NULL);
    vsf_usart_trace_function("%s exited reced buffer size(%d)", __FUNCTION__, usart->reced_buf_size);
    return usart->reced_buf_size;
}

intalu_t vsf_usart_get_tx_count(vsf_usart_t *usart)
{
    vsf_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, usart);
    VSF_HAL_ASSERT(usart != NULL);
    vsf_usart_trace_function("%s exited sended buffer size(%d)", __FUNCTION__, usart->sended_buf_size);
    return usart->sended_buf_size;
}
#endif
