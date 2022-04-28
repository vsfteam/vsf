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

#define VSF_USART_CFG_PREFIX                 vsf_hw
#define VSF_USART_CFG_UPPERCASE_PREFIX       VSF_HW

/*============================ INCLUDES ======================================*/

#include "./uart.h"

#if VSF_HAL_USE_USART == ENABLED

// for vsf_trace
#include "service/vsf_service.h"
#include <Windows.h>

/*============================ MACROS ========================================*/

#ifndef vsf_hw_usart_CFG_TRACE_EN
#   define vsf_hw_usart_CFG_TRACE_EN                   DISABLED
#endif

#ifndef vsf_hw_usart_CFG_TRACE_FUNCTION
#   define    vsf_hw_usart_CFG_TRACE_FUNCTION          DISABLED
#endif

#ifndef vsf_hw_usart_CFG_TRACE_IRQ
#   define vsf_hw_usart_CFG_TRACE_IRQ                  DISABLED
#endif

#ifndef vsf_hw_usart_CFG_TRACE_CALLSTACK
#   define vsf_hw_usart_CFG_TRACE_CALLSTACK            DISABLED
#endif

#ifndef vsf_hw_usart_CFG_TRACE_SYSTIMER
#   define vsf_hw_usart_CFG_TRACE_SYSTIMER             DISABLED
#endif

#ifndef vsf_hw_usart_CFG_TRACE_STATUS
#   define vsf_hw_usart_CFG_TRACE_STATUS               DISABLED
#endif

#ifndef vsf_hw_usart_CFG_START_PORT
#   define vsf_hw_usart_CFG_START_PORT                 1
#endif

#ifndef vsf_hw_usart_CFG_END_PORT
#   define vsf_hw_usart_CFG_END_PORT                   255
#endif

#ifndef vsf_hw_usart_CFG_INPUT_BUFFER
#   define vsf_hw_usart_CFG_INPUT_BUFFER               1024
#endif

#ifndef vsf_hw_usart_CFG_OUTPUT_BUFFER
#   define vsf_hw_usart_CFG_OUTPUT_BUFFER              1024
#endif

#ifndef vsf_hw_usart_CFG_TRACE_IRQ_PEND
#   define vsf_hw_usart_CFG_TRACE_IRQ_PEND             "irq_request_pend:"
#endif

#ifndef vsf_hw_usart_CFG_TRACE_IRQ_GOING
#   define vsf_hw_usart_CFG_TRACE_IRQ_GOING            "irq_request_going:"
#endif

#undef vsf_hw_usart_CFG_TRACE_FUNC
#if vsf_hw_usart_CFG_TRACE_EN == ENABLED
#   define vsf_hw_usart_CFG_TRACE_FUNC(...)            printf(__VA_ARGS__)
#else
#   define vsf_hw_usart_CFG_TRACE_FUNC(...)
#endif

#undef vsf_hw_usart_tRACE_HEADER
#define vsf_hw_usart_tRACE_HEADER                      "[x86/x64]:"

#undef vsf_hw_usart_trace_function
#if vsf_hw_usart_CFG_TRACE_FUNCTION == ENABLED
#   define vsf_hw_usart_trace_function(...)                                        \
    do{                                                                         \
        vsf_hw_usart_CFG_TRACE_FUNC("%s" vsf_hw_usart_tRACE_HEADER,                   \
                        __vsf_x86_trace_color[vsf_hw_usart_tRACE_FUNCTION_COLOR]); \
        vsf_hw_usart_CFG_TRACE_FUNC(__VA_ARGS__);                                  \
        vsf_hw_usart_CFG_TRACE_FUNC(VSF_TRACE_CFG_LINEEND);                        \
        vsf_hw_usart_CFG_TRACE_FUNC("%s",                                          \
        __vsf_x86_trace_color[vsf_hw_usart_tRACE_RESET_COLOR]);                    \
    }while(0)
#else
#   define vsf_hw_usart_trace_function(...)
#endif // vsf_hw_usart_CFG_TRACE_FUNCTION == ENABLED

#undef vsf_hw_usart_trace_irq
#if vsf_hw_usart_CFG_TRACE_IRQ == ENABLED
#   define vsf_hw_usart_trace_irq(...)                                             \
    do{                                                                         \
        vsf_hw_usart_CFG_TRACE_FUNC("%s" vsf_hw_usart_tRACE_HEADER,                   \
                        __vsf_x86_trace_color[vsf_hw_usart_tRACE_IRQ_COLOR]);      \
        vsf_hw_usart_CFG_TRACE_FUNC(__VA_ARGS__);                                  \
        vsf_hw_usart_CFG_TRACE_FUNC(VSF_TRACE_CFG_LINEEND);                        \
        vsf_hw_usart_CFG_TRACE_FUNC(                                               \
        __vsf_x86_trace_color[vsf_hw_usart_tRACE_RESET_COLOR]);                    \
    }while(0)
#else
#   define vsf_hw_usart_trace_irq(...)
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

typedef struct vsf_hw_usart_t {
    em_usart_irq_mask_t                 enable_flag;
    uint8_t                             com_port;
    uint8_t                             com_status;
    uint8_t                             cancel_status;
    uint8_t                             init_flag : 1;
    usart_cfg_t                         cfg;
    uint8_t                             *buf;
    uint_fast32_t                       buf_size;
    int_fast32_t                        sended_buf_size;
    uint8_t                             *rec_buf;
    uint_fast32_t                       rec_buf_size;
    int_fast32_t                        reced_buf_size;
    vsf_arch_irq_thread_t               irq_init_thread;
    HANDLE                              handle_com;
    vsf_arch_irq_thread_t               irq_read_thread;
    vsf_arch_irq_thread_t               irq_write_thread;
    vsf_arch_irq_request_t              irq_read_request;
    vsf_arch_irq_request_t              irq_write_request;
    vsf_arch_irq_request_t              *irp_cancel_read_request;
    vsf_arch_irq_request_t              *irp_cancel_write_request;
} vsf_hw_usart_t;

typedef struct x86_usart_win_t {
    vsf_arch_irq_thread_t               irq_scan_thread;
    vsf_arch_irq_request_t              irq_scan_request;
    uint8_t                             *ret;
    uint8_t                             is_busy : 1;
    uint8_t                             is_scan_inited : 1;
    vsf_arch_irq_request_t              irq_init_request;
    struct {
        uint8_t                         port[VSF_HW_USART_COUNT];
        uint8_t                         num;
    };
} x86_usart_win_t;

/*============================ PROTOTYPES ====================================*/

static void __vk_usart_read_event_thread(void *arg);
static void __vk_usart_write_event_thread(void *arg);

/*============================ LOCAL VARIABLES ===============================*/

#if     vsf_hw_usart_CFG_TRACE_IRQ == ENABLED                                      \
    ||  vsf_hw_usart_CFG_TRACE_CALLSTACK == ENABLED                                \
    ||  vsf_hw_usart_CFG_TRACE_SYSTIMER == ENABLED                                 \
    ||  vsf_hw_usart_CFG_TRACE_STATUS == ENABLED                                   \
    ||  vsf_hw_usart_CFG_TRACE_FUNCTION == ENABLED
enum {
    vsf_hw_usart_tRACE_CALLSTACK_COLOR,
    vsf_hw_usart_tRACE_IRQ_COLOR,
    vsf_hw_usart_tRACE_SYSTIMER_COLOR,
    vsf_hw_usart_tRACE_STATUS_COLOR,
    vsf_hw_usart_tRACE_FUNCTION_COLOR,
    vsf_hw_usart_tRACE_RESET_COLOR,
};
static const char *__vsf_x86_trace_color[] = {
    [vsf_hw_usart_tRACE_CALLSTACK_COLOR] = "\033[1;32m",
    [vsf_hw_usart_tRACE_IRQ_COLOR]         = "\033[1;33m",
    [vsf_hw_usart_tRACE_SYSTIMER_COLOR]    = "\033[1;35m",
    [vsf_hw_usart_tRACE_STATUS_COLOR]      = "\033[1;36m",
    [vsf_hw_usart_tRACE_FUNCTION_COLOR]    = "\033[1;34m",
    [vsf_hw_usart_tRACE_RESET_COLOR]       = "\033[1;37m",
};
#endif

static x86_usart_win_t __x86_usart_win = {
    .is_busy = 1,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static DWORD  __calculate_rx_timeout(vsf_hw_usart_t *hw_usart, uint32_t times)
{
    if (0 == times) {
        return 0;
    }
    uint8_t bits = 1;//start bit
    DWORD ret_timeout = 0;
    bits += hw_usart->cfg.mode & USART_9_BIT_LENGTH ? 9 : 8;
    bits += hw_usart->cfg.mode & USART_2_STOPBIT ? 2 : 1;
    ret_timeout = (DWORD)(1000 / (double)(hw_usart->cfg.baudrate) * bits * times);
    return ret_timeout;
}
static void __vk_usart_scan_event_thread(void *arg)
{
    vsf_hw_usart_trace_irq("%s(0x"VSF_TRACE_POINTER_HEX")This hardware thread will not exit", __FUNCTION__, arg);
    vsf_arch_irq_thread_t *irq_thread = arg;
    x86_usart_win_t *pret = container_of(irq_thread, x86_usart_win_t, irq_scan_thread);
    __vsf_arch_irq_set_background(irq_thread);

    HANDLE hCom;
    char file[16] = {0};
    uint8_t i, j;
    for (i = 0; i < VSF_HW_USART_COUNT; i++) {
        __x86_usart_win.port[i] = 0;
    }
    __x86_usart_win.num = 0;
    while (1) {
        vsf_hw_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_PEND"[%s]line(%d)", __FUNCTION__, __LINE__);
        __vsf_arch_irq_request_pend(&pret->irq_scan_request);
        vsf_hw_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_GOING"[%s]line(%d)", __FUNCTION__, __LINE__);
        for (i = vsf_hw_usart_CFG_START_PORT; (i < vsf_hw_usart_CFG_END_PORT) && (__x86_usart_win.num < VSF_HW_USART_COUNT); i++) {
            sprintf(file, "\\\\.\\com%d", i);
            hCom = CreateFileA(file, GENERIC_WRITE | GENERIC_READ, 0, NULL,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
            vsf_hw_usart_trace_irq("[%s]line(%d)CreateFileA[return("VSF_TRACE_POINTER_HEX")]", __FUNCTION__, __LINE__, hCom);
            if (hCom != INVALID_HANDLE_VALUE) {
                for (j = 0; j < VSF_HW_USART_COUNT; j++) {//check
                    vsf_hw_usart_trace_irq("[%s]line(%d)check{__x86_usart_win.port[%d] = (%d)}", __FUNCTION__, __LINE__, j, i);
                    if (__x86_usart_win.port[j] == i) {
                        goto close_handle;
                    }
                }
                for (j = 0; j < VSF_HW_USART_COUNT; j++) {//push
                    vsf_hw_usart_trace_irq("[%s]line(%d)push{__x86_usart_win.port[%d] = (%d)}", __FUNCTION__, __LINE__, j, i);
                    if (__x86_usart_win.port[j] == 0) {
                        __x86_usart_win.port[j] = i;
                        __x86_usart_win.num++;
                        break;
                    }
                }
            } else if (ERROR_ACCESS_DENIED == GetLastError()) {
                goto close_handle;
            } else {
                for (j = 0; j < VSF_HW_USART_COUNT; j++) {//pop
                    vsf_hw_usart_trace_irq("[%s]line(%d)pop{__x86_usart_win.port[%d] = (%d)}", __FUNCTION__, __LINE__, j, i);
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
        for (int i=0;i < VSF_HW_USART_COUNT;i++) {
            vsf_hw_usart_CFG_TRACE_FUNC("[%s]line(%d)com%d---vsf_usart%d\n", __FUNCTION__, __LINE__, __x86_usart_win.port[i], i);
        }
    }
    __vsf_arch_irq_fini(irq_thread);
}

static vsf_err_t __usart_init(vsf_hw_usart_t *hw_usart)
{
    bool res_bool;
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, hw_usart);
    if (!__x86_usart_win.port[hw_usart->com_port]) {
        vsf_trace_error("initialization failed!This hardware thread will be terminated!\n");
        vsf_hw_usart_trace_function("%s line(%d)exited VSF_ERR_FAIL", __FUNCTION__, __LINE__);
        res_bool = CloseHandle(hw_usart->handle_com);
        hw_usart->handle_com = INVALID_HANDLE_VALUE;
        return VSF_ERR_FAIL;
    }
    HANDLE handle_com;
#if vsf_hw_usart_CFG_TRACE_EN == ENABLED
    DWORD res_dword = 0;
#endif
    char file[16] = {0};

    sprintf(file, "\\\\.\\com%d", __x86_usart_win.port[hw_usart->com_port]);
    vsf_hw_usart_CFG_TRACE_FUNC("[%s]line(%d)file(%s)", __FUNCTION__, __LINE__, file);
    handle_com = CreateFileA(file, GENERIC_WRITE | GENERIC_READ, 0, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
#if vsf_hw_usart_CFG_TRACE_EN == ENABLED
    res_dword = GetLastError();
#endif
    vsf_hw_usart_CFG_TRACE_FUNC("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, res_dword);
    hw_usart->handle_com = handle_com;
    res_bool = SetupComm(handle_com,
                         vsf_hw_usart_CFG_INPUT_BUFFER,
                         vsf_hw_usart_CFG_OUTPUT_BUFFER);
    if (!res_bool) {
#if vsf_hw_usart_CFG_TRACE_EN == ENABLED
        res_dword = GetLastError();
#endif
        vsf_hw_usart_CFG_TRACE_FUNC("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, res_dword);
        vsf_trace_warning("cannot setup %s--vsf_usart%d\n", file, hw_usart->com_port);
        vsf_hw_usart_trace_function("%s line(%d)exited VSF_ERR_FAIL", __FUNCTION__, __LINE__);
        res_bool = CloseHandle(hw_usart->handle_com);
        hw_usart->handle_com = INVALID_HANDLE_VALUE;
        return VSF_ERR_FAIL;
    }
    COMMTIMEOUTS timeOuts;
    timeOuts.ReadIntervalTimeout = __calculate_rx_timeout(hw_usart, hw_usart->cfg.rx_timeout);
    timeOuts.ReadTotalTimeoutConstant = 1;
    timeOuts.ReadTotalTimeoutMultiplier = 1;
    res_bool = SetCommTimeouts(handle_com, &timeOuts);
    if (!res_bool) {
#if vsf_hw_usart_CFG_TRACE_EN == ENABLED
        res_dword = GetLastError();
#endif
        vsf_hw_usart_CFG_TRACE_FUNC("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, res_dword);
        vsf_trace_warning("cannot set CommTimeouts %s--vsf_usart%d\n", file, hw_usart->com_port);
        vsf_hw_usart_trace_function("%s line(%d)exited VSF_ERR_FAIL", __FUNCTION__, __LINE__);
        res_bool = CloseHandle(hw_usart->handle_com);
        hw_usart->handle_com = INVALID_HANDLE_VALUE;
        return VSF_ERR_FAIL;
    }

    DCB dcb;
    res_bool = GetCommState(handle_com, &dcb);
    if (!res_bool) {
#if vsf_hw_usart_CFG_TRACE_EN == ENABLED
        res_dword = GetLastError();
#endif
        vsf_hw_usart_CFG_TRACE_FUNC("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, res_dword);
        vsf_trace_warning("cannot get CommState %s--vsf_usart%d\n", file, hw_usart->com_port);
        vsf_hw_usart_trace_function("%s line(%d)exited VSF_ERR_FAIL", __FUNCTION__, __LINE__);
        res_bool = CloseHandle(hw_usart->handle_com);
        hw_usart->handle_com = INVALID_HANDLE_VALUE;
        return VSF_ERR_FAIL;
    }
    dcb.fAbortOnError = FALSE;
    dcb.BaudRate = hw_usart->cfg.baudrate;
    dcb.ByteSize = USART_9_BIT_LENGTH & hw_usart->cfg.mode ? 9 : 8;
    dcb.Parity = USART_ODD_PARITY & hw_usart->cfg.mode ? 1 :
        (USART_EVEN_PARITY & hw_usart->cfg.mode ? 2 : 0);
    dcb.StopBits = USART_2_STOPBIT & hw_usart->cfg.mode ? 2 : 0;
    dcb.fRtsControl = FALSE;
    dcb.fOutxCtsFlow = FALSE;
    if ((USART_RTS_CTS_HWCONTROL & hw_usart->cfg.mode) == USART_RTS_CTS_HWCONTROL) {
        dcb.fRtsControl = TRUE;
        dcb.fOutxCtsFlow = TRUE;
    } else if ((USART_CTS_HWCONTROL & hw_usart->cfg.mode) == USART_CTS_HWCONTROL) {
        dcb.fOutxCtsFlow = TRUE;
    } else if ((USART_RTS_HWCONTROL & hw_usart->cfg.mode) == USART_RTS_HWCONTROL) {
        dcb.fRtsControl = TRUE;
    }
    res_bool = SetCommState(handle_com, &dcb);
    if (!res_bool) {
#if vsf_hw_usart_CFG_TRACE_EN == ENABLED
        res_dword = GetLastError();
#endif
        vsf_hw_usart_CFG_TRACE_FUNC("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, res_dword);
        vsf_trace_warning("cannot set CommState %s--vsf_usart%d\n", file, hw_usart->com_port);
        vsf_hw_usart_trace_function("%s line(%d)exited VSF_ERR_FAIL", __FUNCTION__, __LINE__);
        res_bool = CloseHandle(hw_usart->handle_com);
        hw_usart->handle_com = INVALID_HANDLE_VALUE;
        return VSF_ERR_FAIL;
    }

    PurgeComm(handle_com, PURGE_TXCLEAR | PURGE_RXCLEAR);
    ClearCommError(handle_com, NULL, NULL);

    hw_usart->com_status &= (~USART_INIT_IS_BUSY);

    if (hw_usart->init_flag) {
        hw_usart->init_flag = 0;
        __vsf_arch_irq_request_init(&hw_usart->irq_write_request);
        __vsf_arch_irq_request_init(&hw_usart->irq_read_request);
        /* start read */
        __vsf_arch_irq_init(&hw_usart->irq_read_thread, "vsf_usart_event\n",
                            __vk_usart_read_event_thread, hw_usart->cfg.isr.prio);
        /* start write */
        __vsf_arch_irq_init(&hw_usart->irq_write_thread, "vsf_usart_event\n",
                            __vk_usart_write_event_thread, hw_usart->cfg.isr.prio);
    } else {
        hw_usart->com_status &= ~USART_RESET;
    }
    vsf_hw_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
    return VSF_ERR_NONE;
}

static void __vk_usart_read_event_thread(void *arg)
{
    vsf_hw_usart_trace_irq("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, arg);
    vsf_arch_irq_thread_t *irq_thread = arg;
    vsf_hw_usart_t *hw_usart = container_of(irq_thread, vsf_hw_usart_t, irq_read_thread);

    __vsf_arch_irq_set_background(irq_thread);

    BOOL read_ret = false;
    DWORD read_len = 0;
    DWORD read_real_len = 0;
    OVERLAPPED overLapped;

    while (1) {
    pend_read:
        vsf_hw_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_PEND"[%s]line(%d)", __FUNCTION__, __LINE__);
        __vsf_arch_irq_request_pend(&hw_usart->irq_read_request);
        vsf_hw_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_GOING"[%s]line(%d)", __FUNCTION__, __LINE__);
        read_real_len = 0;
        hw_usart->reced_buf_size = read_real_len;
    rec_buf_lenth_insufficient:
        if (NULL != hw_usart->irp_cancel_read_request) {
            __vsf_arch_irq_request_send(hw_usart->irp_cancel_read_request);
            vsf_hw_usart_trace_irq("[%s]line(%d)read will cancel", __FUNCTION__, __LINE__);
            goto pend_read;
        }
        memset(&overLapped, 0, sizeof(OVERLAPPED));
        read_len = 0;
        overLapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        read_ret = ReadFile(hw_usart->handle_com, hw_usart->rec_buf + read_real_len,
                            hw_usart->rec_buf_size - read_real_len, &read_len, &overLapped);
        if (!read_ret) {
            DWORD rres = GetLastError();
            vsf_hw_usart_trace_irq("[%s]line(%d)GetLastError(%lu)", __FUNCTION__, __LINE__, rres);
            if (rres == ERROR_IO_PENDING) {
                GetOverlappedResult(hw_usart->handle_com, &overLapped, &read_len, TRUE);
            } else {
                if ((hw_usart->enable_flag & USART_IRQ_MASK_RX_ERR) && NULL != hw_usart->cfg.isr.handler_fn) {
                    hw_usart->enable_flag &= ~USART_IRQ_MASK_TX_ERR;
                    CloseHandle(hw_usart->handle_com);
                    hw_usart->handle_com = INVALID_HANDLE_VALUE;
                    hw_usart->com_status &= (~USART_READ_IS_BUSY);
                    vsf_hw_usart_irq_disable(hw_usart, 0XFF);
                    vsf_hw_usart_disable(hw_usart);
                    vsf_hw_usart_trace_irq(VSF_TRACE_POINTER_HEX"-->|com%d|  [%s]line(%d)read err call("VSF_TRACE_POINTER_HEX")",
                                        hw_usart, hw_usart->com_port, __FUNCTION__, __LINE__, hw_usart->cfg.isr.handler_fn);
                    __vsf_arch_irq_start(irq_thread);
                    hw_usart->cfg.isr.handler_fn(hw_usart->cfg.isr.target_ptr, (vsf_usart_t *)hw_usart, USART_IRQ_MASK_ERR);
                    __vsf_arch_irq_end(irq_thread, false);
                } else {
                    vsf_trace_error(VSF_TRACE_POINTER_HEX"-->|com%d|  error with read!%s%d\n",
                              hw_usart, hw_usart->com_port, __FILE__, __LINE__);
                }
                goto pend_read;
            }
        } else {
            if (!GetLastError()) {
                vsf_hw_usart_trace_irq("read to the file eof!%s%d\n", __FILE__, __LINE__);
            } else {
                if ((hw_usart->enable_flag & USART_IRQ_MASK_RX_ERR) && NULL != hw_usart->cfg.isr.handler_fn) {
                    hw_usart->enable_flag &= ~USART_IRQ_MASK_TX_ERR;
                    CloseHandle(hw_usart->handle_com);
                    hw_usart->handle_com = INVALID_HANDLE_VALUE;
                    hw_usart->com_status &= (~USART_READ_IS_BUSY);

                    vsf_hw_usart_irq_disable(hw_usart, 0XFF);
                    vsf_hw_usart_disable(hw_usart);
                    vsf_hw_usart_trace_irq(VSF_TRACE_POINTER_HEX"-->|com%d|  [%s]line(%d)read err call("VSF_TRACE_POINTER_HEX")",
                                        hw_usart, hw_usart->com_port, __FUNCTION__, __LINE__, hw_usart->cfg.isr.handler_fn);
                    __vsf_arch_irq_start(irq_thread);
                    hw_usart->cfg.isr.handler_fn(hw_usart->cfg.isr.target_ptr, (vsf_usart_t *)hw_usart, USART_IRQ_MASK_ERR);
                    __vsf_arch_irq_end(irq_thread, false);
                } else {
                    vsf_trace_error(VSF_TRACE_POINTER_HEX"-->|com%d|  error with read!%s%d\n", hw_usart, hw_usart->com_port, __FILE__, __LINE__);
                }
                goto pend_read;
            }
        }
        read_real_len += read_len;
        if (hw_usart->rec_buf_size != read_real_len) {
            hw_usart->reced_buf_size = read_real_len;
            goto rec_buf_lenth_insufficient;
        }
        vsf_hw_usart_trace_irq("read_thread onece!the recbuf:\n");
        for (int temp = 0; temp < hw_usart->rec_buf_size; temp++) {
            vsf_hw_usart_CFG_TRACE_FUNC("%x  ", *(hw_usart->rec_buf + temp));
        }
        vsf_hw_usart_CFG_TRACE_FUNC("\n");
        hw_usart->com_status &= (~USART_READ_IS_BUSY);
        if ((hw_usart->enable_flag & USART_IRQ_MASK_RX_CPL) && (NULL != hw_usart->cfg.isr.handler_fn)) {
            vsf_hw_usart_trace_irq("%s(%d) call %p\n", __FUNCTION__, __LINE__, hw_usart->cfg.isr.handler_fn);
            __vsf_arch_irq_start(irq_thread);
            hw_usart->cancel_status |= CANCEL_READ_CLP;
            hw_usart->cfg.isr.handler_fn(hw_usart->cfg.isr.target_ptr, (vsf_usart_t *)hw_usart, USART_IRQ_MASK_RX_CPL);
            __vsf_arch_irq_end(irq_thread, false);

        }
        hw_usart->reced_buf_size = read_real_len;
        if (NULL != hw_usart->irp_cancel_read_request) {
            __vsf_arch_irq_request_send(hw_usart->irp_cancel_read_request);
            vsf_hw_usart_trace_irq("[%s]line(%d)read will cancel", __FUNCTION__, __LINE__);
            goto pend_read;
        }
    }
    __vsf_arch_irq_fini(irq_thread);
}

static void __vk_usart_write_event_thread(void *arg)
{
    vsf_hw_usart_trace_irq("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, arg);
    vsf_arch_irq_thread_t *irq_thread = arg;
    vsf_hw_usart_t *hw_usart = container_of(irq_thread, vsf_hw_usart_t, irq_write_thread);

    __vsf_arch_irq_set_background(irq_thread);

    BOOL write_ret;
    unsigned long write_len = 0;
    OVERLAPPED overLapped;

    while (1) {
    pend_write:
        vsf_hw_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_PEND"[%s]line(%d)", __FUNCTION__, __LINE__);
        __vsf_arch_irq_request_pend(&hw_usart->irq_write_request);
        vsf_hw_usart_trace_irq(VSF_USART_CFG_TRACE_IRQ_GOING"[%s]line(%d)", __FUNCTION__, __LINE__);
        write_len = 0;
        hw_usart->sended_buf_size = write_len;

        if (NULL != hw_usart->irp_cancel_write_request) {
            __vsf_arch_irq_request_send(hw_usart->irp_cancel_write_request);
            vsf_hw_usart_trace_irq("[%s]line(%d)write will cancel", __FUNCTION__, __LINE__);
            goto pend_write;
        }
        vsf_hw_usart_trace_irq("The data to be write(%p) is:\n", hw_usart->buf);
        if (NULL != hw_usart->buf) {
            for (int temp = 0; temp < hw_usart->buf_size; temp++) {
                vsf_hw_usart_CFG_TRACE_FUNC("%x  ", *(hw_usart->buf + temp));
            }
            vsf_hw_usart_CFG_TRACE_FUNC("\n");
        }
        memset(&overLapped, 0, sizeof(OVERLAPPED));
        overLapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        write_ret = WriteFile(hw_usart->handle_com, hw_usart->buf,
                              hw_usart->buf_size, &write_len, &overLapped);
        if (!write_ret) {
            if (GetLastError() == ERROR_IO_PENDING) {
            //tips: some virtual serial ports will be stuck in the following api
                GetOverlappedResult(hw_usart->handle_com, &overLapped, &write_len, TRUE);
            }
        } else {
            if ((hw_usart->enable_flag & USART_IRQ_MASK_TX_ERR) && NULL != hw_usart->cfg.isr.handler_fn) {
                hw_usart->enable_flag &= ~USART_IRQ_MASK_RX_ERR;

                CloseHandle(hw_usart->handle_com);
                hw_usart->handle_com = INVALID_HANDLE_VALUE;
                hw_usart->com_status &= (~USART_WRITE_IS_BUSY);
                vsf_hw_usart_irq_disable(hw_usart, 0XFF);
                vsf_hw_usart_disable(hw_usart);

                vsf_hw_usart_trace_irq(VSF_TRACE_POINTER_HEX"-->|com%d|  [%s]line(%d)write err call("VSF_TRACE_POINTER_HEX")",
                                    hw_usart, hw_usart->com_port, __FUNCTION__, __LINE__, hw_usart->cfg.isr.handler_fn);
                __vsf_arch_irq_start(irq_thread);
                hw_usart->cfg.isr.handler_fn(hw_usart->cfg.isr.target_ptr, (vsf_usart_t *)hw_usart, USART_IRQ_MASK_ERR);
                __vsf_arch_irq_end(irq_thread, false);
            }
            goto pend_write;
        }
        vsf_hw_usart_trace_irq("The data  writed(%p) is:\n", hw_usart->buf);
        if (NULL != hw_usart->buf) {
            for (int temp = 0; temp < hw_usart->buf_size; temp++) {
                vsf_hw_usart_CFG_TRACE_FUNC("%x  ", *(hw_usart->buf + temp));
            }
            vsf_hw_usart_CFG_TRACE_FUNC("\n");
        }
        hw_usart->com_status &= (~USART_WRITE_IS_BUSY);

        if ((hw_usart->enable_flag & USART_IRQ_MASK_TX_CPL) && (NULL != hw_usart->cfg.isr.handler_fn)) {
            vsf_hw_usart_trace_irq("%s(%d) call %p\n", __FUNCTION__, __LINE__, hw_usart->cfg.isr.handler_fn);
            __vsf_arch_irq_start(irq_thread);
            hw_usart->cancel_status |= CANCEL_WRITE_CLP;
            hw_usart->cfg.isr.handler_fn(hw_usart->cfg.isr.target_ptr, (vsf_usart_t *)hw_usart, USART_IRQ_MASK_TX_CPL);
            __vsf_arch_irq_end(irq_thread, false);
        }
        hw_usart->sended_buf_size = write_len;
        if (NULL != hw_usart->irp_cancel_write_request) {
            __vsf_arch_irq_request_send(hw_usart->irp_cancel_write_request);
            vsf_hw_usart_trace_irq("[%s]line(%d)write will cancel", __FUNCTION__, __LINE__);
            goto pend_write;
        }
    }
    __vsf_arch_irq_fini(irq_thread);
}

vsf_err_t vsf_hw_usart_init(vsf_hw_usart_t *hw_usart, usart_cfg_t *cfg)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX",0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, hw_usart, cfg);
    VSF_HAL_ASSERT(cfg != NULL);
    VSF_HAL_ASSERT(hw_usart != NULL);
    if (__x86_usart_win.is_busy) {
        vsf_hw_usart_trace_function("%s exited VSF_ERR_FAIL", __FUNCTION__);
        return VSF_ERR_FAIL;
    }
    if (NULL == hw_usart->handle_com || INVALID_HANDLE_VALUE == hw_usart->handle_com) {
        hw_usart->com_status |= USART_INIT_IS_BUSY;
    } else {
        uint8_t res;
        hw_usart->com_status = USART_INIT_IS_BUSY | USART_RESET;
        while ((hw_usart->com_status & USART_READ_IS_BUSY) && (hw_usart->com_status & USART_WRITE_IS_BUSY));
        vsf_hw_usart_CFG_TRACE_FUNC("CloseHandle:%p\n", hw_usart->handle_com);
        bool b = CloseHandle(hw_usart->handle_com);
        (void)b;
        vsf_hw_usart_CFG_TRACE_FUNC("CloseHandle:%d^^%p\n", b, hw_usart->handle_com);
        vsf_hw_usart_get_can_used_port(&res);
        while (vsf_hw_usart_port_is_busy());
    }
    hw_usart->handle_com = INVALID_HANDLE_VALUE;

    hw_usart->buf = NULL;
    hw_usart->cfg = *cfg;
    hw_usart->enable_flag = 0X00;

    return __usart_init(hw_usart);
}

fsm_rt_t vsf_hw_usart_enable(vsf_hw_usart_t *hw_usart)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, hw_usart);
    VSF_HAL_ASSERT(hw_usart != NULL);
    hw_usart->com_status |= USART_ENABLE;
    vsf_hw_usart_trace_function("%s exited fsm_rt_cpl", __FUNCTION__);
    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_usart_disable(vsf_hw_usart_t *hw_usart)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, hw_usart);
    VSF_HAL_ASSERT(hw_usart != NULL);
    hw_usart->com_status &= USART_DISABLE;
    vsf_hw_usart_trace_function("%s exited fsm_rt_cpl", __FUNCTION__);
    return fsm_rt_cpl;
}

void vsf_hw_usart_irq_enable(vsf_hw_usart_t *hw_usart, em_usart_irq_mask_t irq_mask)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX",%d)", __FUNCTION__, hw_usart, irq_mask);
    VSF_HAL_ASSERT(hw_usart != NULL);
    VSF_HAL_ASSERT(hw_usart->cfg.isr.handler_fn != NULL);
    hw_usart->enable_flag |= irq_mask;
    vsf_hw_usart_trace_function("%s exited", __FUNCTION__);
}

void vsf_hw_usart_irq_disable(vsf_hw_usart_t *hw_usart, em_usart_irq_mask_t irq_mask)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX",%d)", __FUNCTION__, hw_usart, irq_mask);
    VSF_HAL_ASSERT(hw_usart != NULL);
    hw_usart->enable_flag &= ~irq_mask;
    vsf_hw_usart_trace_function("%s exited", __FUNCTION__);
}

usart_status_t vsf_hw_usart_status(vsf_hw_usart_t *hw_usart)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, hw_usart);
    VSF_HAL_ASSERT(hw_usart != NULL);
    usart_status_t ret;
    if ((hw_usart->com_status & USART_INIT_IS_BUSY)
        || (hw_usart->com_status & USART_READ_IS_BUSY)
        || (hw_usart->com_status & USART_WRITE_IS_BUSY)) {
        ret.is_busy = 1;
    } else {
        ret.is_busy = 0;
    }
    vsf_hw_usart_trace_function("%s exited", __FUNCTION__);
    return ret;
}

uint_fast16_t vsf_hw_usart_fifo_read(vsf_hw_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t number)
{
    return 0;
    //TODO:
    /*
    if (!(hw_usart->com_status & USART_ENABLE)) {
        return false;
    }
    if (hw_usart->com_status & USART_READ_IS_BUSY) {
        return false;
    }
    if (hw_usart->com_status & USART_INIT_IS_BUSY) {
        return fsm_rt_asyn;
    }
#if vsf_hw_usart_CFG_TRACE_EN == ENABLED
    vsf_hw_usart_CFG_TRACE_FUNC("vsf_usart_read_byte:file:%sline:%d", __FILE__, __LINE__);
#endif // vsf_hw_usart_CFG_TRACE_EN

    hw_usart->com_status |= USART_READ_IS_BUSY;
    hw_usart->on_status.is_busy = 1;

    //hw_usart->com_status |= USART_READ_BYTE;
    hw_usart->buf = buffer_ptr;
    hw_usart->buf_size = buffer_ptr;
    __vsf_arch_irq_request_send(&hw_usart->irq_read_request);

    return true;
    */
}

uint_fast16_t vsf_hw_usart_fifo_write(vsf_hw_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t number)
{
    //TODO:
    return 0;
    /*
    if (!(hw_usart->com_status & USART_ENABLE)) {
        return false;
    }
    if (hw_usart->com_status & USART_WRITE_IS_BUSY) {
        return false;
    }
    if (hw_usart->com_status & USART_INIT_IS_BUSY) {
        return fsm_rt_asyn;
    }
#if vsf_hw_usart_CFG_TRACE_EN == ENABLED
    vsf_hw_usart_CFG_TRACE_FUNC("vsf_usart_write_byte:file:%sline:%d", __FILE__, __LINE__);
#endif // vsf_hw_usart_CFG_TRACE_EN

    hw_usart->com_status |= USART_WRITE_IS_BUSY;
    hw_usart->on_status.is_busy = 1;

    //hw_usart->com_status |= USART_SEND_BYTE;
    hw_usart->buf = &byte;
    hw_usart->buf_size = 1;
    __vsf_arch_irq_request_send(&hw_usart->irq_write_request);

    return true;
    */
}

bool vsf_hw_usart_fifo_flush(vsf_hw_usart_t *usart_ptr)
{
    //TODO:
    return false;
}

vsf_err_t vsf_hw_usart_request_rx(vsf_hw_usart_t *hw_usart, void *buffer, uint_fast32_t number)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX",0x"VSF_TRACE_POINTER_HEX",%d)", __FUNCTION__, hw_usart, buffer, number);
    VSF_HAL_ASSERT(hw_usart != NULL);

    if (!(hw_usart->com_status & USART_ENABLE)) {
        vsf_hw_usart_trace_function("%s exited VSF_ERR_NOT_ACCESSABLE", __FUNCTION__);
        return VSF_ERR_NOT_ACCESSABLE;
    }
    if (hw_usart->com_status & USART_READ_IS_BUSY) {
        vsf_hw_usart_trace_function("%s exited VSF_ERR_REQ_ALREADY_REGISTERED", __FUNCTION__);
        return VSF_ERR_REQ_ALREADY_REGISTERED;
    }
    hw_usart->com_status |= USART_READ_IS_BUSY;

    hw_usart->rec_buf = buffer;
    hw_usart->rec_buf_size = number;
    __vsf_arch_irq_request_send(&hw_usart->irq_read_request);
    vsf_hw_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_usart_request_tx(vsf_hw_usart_t *hw_usart, void *buffer, uint_fast32_t number)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX",0x"VSF_TRACE_POINTER_HEX",%d)", __FUNCTION__, hw_usart, buffer, number);
    VSF_HAL_ASSERT(hw_usart != NULL);
    if (!(hw_usart->com_status & USART_ENABLE)) {
        vsf_hw_usart_trace_function("%s exited VSF_ERR_NOT_ACCESSABLE", __FUNCTION__);
        return VSF_ERR_NOT_ACCESSABLE;
    }
    if (hw_usart->com_status & USART_WRITE_IS_BUSY) {
        vsf_hw_usart_trace_function("%s exited VSF_ERR_REQ_ALREADY_REGISTERED", __FUNCTION__);
        return VSF_ERR_REQ_ALREADY_REGISTERED;
    }
    hw_usart->com_status |= USART_WRITE_IS_BUSY;

    hw_usart->buf = buffer;
    hw_usart->buf_size = number;
    __vsf_arch_irq_request_send(&hw_usart->irq_write_request);
    vsf_hw_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
    return VSF_ERR_NONE;
}

bool vsf_hw_usart_port_is_busy()
{
    vsf_hw_usart_trace_function("%s", __FUNCTION__);
    vsf_hw_usart_trace_function("%s exited %s", __FUNCTION__, __x86_usart_win.is_busy == 1 ? "true" : "false");
    return __x86_usart_win.is_busy == 1;
}

void vsf_hw_usart_get_can_used_port(uint8_t *available_number_port)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, available_number_port);
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
    vsf_hw_usart_trace_function("%s exited", __FUNCTION__);
}

bool vsf_hw_usart_get_com_num(vsf_usart_win_expression_t arr[], uint8_t size)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")size(%d)", __FUNCTION__, &arr, size);
    vsf_hw_usart_CFG_TRACE_FUNC("%s(%d)calling\n", __FUNCTION__, __LINE__);
    if (__x86_usart_win.is_busy) {
        vsf_hw_usart_CFG_TRACE_FUNC("__x86_usart_win: is busy %sline:%d\n", __FILE__, __LINE__);
        vsf_hw_usart_trace_function("%s exited false", __FUNCTION__);
        return false;
    }
    static vsf_hw_usart_t *__vsf_usart_win[] = {
        &vsf_hw_usart0,&vsf_hw_usart1,&vsf_hw_usart2,&vsf_hw_usart3,
        &vsf_hw_usart4,&vsf_hw_usart5,&vsf_hw_usart6,&vsf_hw_usart7
    };
    uint8_t i, j = 0;
    for (i = 0; i < VSF_HW_USART_COUNT; i++) {
        if (0 != __x86_usart_win.port[i]) {
            arr[j].win_serial_port_num = __x86_usart_win.port[i];
            arr[j].vsf_usart_instance_ptr = (vsf_usart_t *)__vsf_usart_win[i];
            j++;
        }
    }
#if vsf_hw_usart_CFG_TRACE_EN == ENABLED
    for (int temp = 0; temp < VSF_HW_USART_COUNT; temp++) {
        if (__x86_usart_win.port[temp] != 0) {
            vsf_hw_usart_CFG_TRACE_FUNC("com%d---vsf_usart%d("VSF_TRACE_POINTER_HEX")\n",
                                     __x86_usart_win.port[temp], temp, __vsf_usart_win[temp]);
        }
    }
#endif // vsf_hw_usart_CFG_TRACE_EN
#if vsf_hw_usart_CFG_TRACE_EN == ENABLED
    vsf_hw_usart_trace_function("%s exited true", __FUNCTION__);
#endif
    return true;
}

vsf_err_t vsf_hw_usart_cancel_rx(vsf_hw_usart_t *hw_usart)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, hw_usart);
    VSF_HAL_ASSERT(hw_usart != NULL);
    if (NULL == hw_usart->handle_com) {
        vsf_hw_usart_trace_function("%s exited VSF_ERR_NOT_AVAILABLE", __FUNCTION__);
        return VSF_ERR_NONE;
    }
    if (NULL != hw_usart->irp_cancel_read_request) {
        vsf_hw_usart_trace_function("%s exited VSF_ERR_NOT_ACCESSABLE", __FUNCTION__);
        return VSF_ERR_NOT_ACCESSABLE;
    }
    if (hw_usart->com_status & USART_INIT_IS_BUSY) {
        vsf_hw_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
        return VSF_ERR_NONE;
    }
    if (!(hw_usart->com_status & USART_READ_IS_BUSY)) {
        vsf_hw_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
        return VSF_ERR_NONE;
    }
    if (hw_usart->cancel_status & CANCEL_READ_CLP) {
        hw_usart->cancel_status &= ~CANCEL_READ_CLP;
        vsf_hw_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
        return VSF_ERR_NONE;
    }
    vsf_arch_irq_request_t read_cancel_request = {0};
    __vsf_arch_irq_request_init(&read_cancel_request);

    hw_usart->irp_cancel_read_request = &read_cancel_request;

    __vsf_arch_irq_request_pend(hw_usart->irp_cancel_read_request);
    hw_usart->irp_cancel_read_request = NULL;
    vsf_hw_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_usart_cancel_tx(vsf_hw_usart_t *hw_usart)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, hw_usart);
    VSF_HAL_ASSERT(hw_usart != NULL);
    if (NULL == hw_usart->handle_com) {
        vsf_hw_usart_trace_function("%s line(%d) exited VSF_ERR_NOT_AVAILABLE", __FUNCTION__, __LINE__);
        return VSF_ERR_NONE;
    }
    if (NULL != hw_usart->irp_cancel_write_request) {
        vsf_hw_usart_trace_function("%s line(%d) exited VSF_ERR_NOT_ACCESSABLE", __FUNCTION__, __LINE__);
        return VSF_ERR_NOT_ACCESSABLE;
    }
    if (hw_usart->com_status & USART_INIT_IS_BUSY) {
        vsf_hw_usart_trace_function("%s line(%d) exited VSF_ERR_NOT_ACCESSABLE", __FUNCTION__, __LINE__);
        return VSF_ERR_NOT_ACCESSABLE;
    }
    if (!(hw_usart->com_status & USART_WRITE_IS_BUSY)) {
        vsf_hw_usart_trace_function("%s line(%d) exited VSF_ERR_NONE", __FUNCTION__, __LINE__);
        return VSF_ERR_NONE;
    }
    if (hw_usart->cancel_status & CANCEL_WRITE_CLP) {
        hw_usart->cancel_status &= ~CANCEL_WRITE_CLP;
        vsf_hw_usart_trace_function("%s line(%d) exited VSF_ERR_NONE", __FUNCTION__, __LINE__);
        return VSF_ERR_NONE;
    }
    vsf_arch_irq_request_t write_cancel_request = {0};
    __vsf_arch_irq_request_init(&write_cancel_request);
    hw_usart->irp_cancel_write_request = &write_cancel_request;

    __vsf_arch_irq_request_pend(hw_usart->irp_cancel_write_request);
    hw_usart->irp_cancel_write_request = NULL;
    vsf_hw_usart_trace_function("%s exited VSF_ERR_NONE", __FUNCTION__);
    return VSF_ERR_NONE;
}

int_fast32_t vsf_hw_usart_get_rx_count(vsf_hw_usart_t *hw_usart)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, hw_usart);
    VSF_HAL_ASSERT(hw_usart != NULL);
    vsf_hw_usart_trace_function("%s exited reced buffer size(%d)", __FUNCTION__, hw_usart->reced_buf_size);
    return hw_usart->reced_buf_size;
}

int_fast32_t vsf_hw_usart_get_tx_count(vsf_hw_usart_t *hw_usart)
{
    vsf_hw_usart_trace_function("%s(0x"VSF_TRACE_POINTER_HEX")", __FUNCTION__, hw_usart);
    VSF_HAL_ASSERT(hw_usart != NULL);
    vsf_hw_usart_trace_function("%s exited sended buffer size(%d)", __FUNCTION__, hw_usart->sended_buf_size);
    return hw_usart->sended_buf_size;
}

/*============================ INCLUDES ======================================*/

#define VSF_USART_CFG_IMP_LV0(__count, __dont_care)                             \
    vsf_hw_usart_t vsf_hw_usart ## __count = {                                  \
        .com_port                          = __count,                           \
        .com_status                        = USART_INIT_IS_BUSY,                \
        .handle_com                        = NULL,                              \
        .irp_cancel_read_request           = NULL,                              \
        .irp_cancel_write_request          = NULL,                              \
        .cancel_status                     = 0X00,                              \
        .init_flag                         = 1,                                 \
    };
#include "hal/driver/common/usart/usart_template.inc"

#endif
