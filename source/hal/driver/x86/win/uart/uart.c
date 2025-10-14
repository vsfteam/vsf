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

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#include "../driver.h"

#if VSF_HAL_USE_USART == ENABLED

// for vsf_trace
#include "service/vsf_service.h"
#include <Windows.h>
#include <SetupAPI.h>

#pragma comment (lib, "SetupAPI.lib")
#pragma comment (lib, "advapi32.lib")

/*============================ MACROS ========================================*/

#ifndef VSF_HOSTOS_USART_CFG_MULTI_CLASS
#   define VSF_HOSTOS_USART_CFG_MULTI_CLASS         VSF_USART_CFG_MULTI_CLASS
#endif

#ifndef VSF_HOSTOS_USART_CFG_TX_BLOCK
#   ifdef VSF_USART_CFG_TX_BLOCK
#       define VSF_HOSTOS_USART_CFG_TX_BLOCK        VSF_USART_CFG_TX_BLOCK
#   else
#       define VSF_HOSTOS_USART_CFG_TX_BLOCK        DISABLED
#   endif
#endif

#ifndef VSF_HOSTOS_USART_CFG_FIFO_SIZE
#   define VSF_HOSTOS_USART_CFG_FIFO_SIZE           1024
#endif

#ifndef VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL
#   define VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL  interrupt
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_usart_protect                         vsf_protect(VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL)
#define __vsf_usart_unprotect                       vsf_unprotect(VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/

typedef struct vsf_hostos_usart_t {
#if VSF_HOSTOS_USART_CFG_MULTI_CLASS == ENABLED
    vsf_usart_t                         vsf_usart;
#endif

    HANDLE                              handle;
    uint8_t                             port_idx;
    bool                                irq_started;
    bool                                is_to_exit;
    vsf_usart_irq_mask_t                enable_flag;
    vsf_arch_prio_t                     prio;
    uint32_t                            irq_mask;

    struct {
        void                            *param;
        vsf_usart_isr_handler_t         *isrhandler;
    } irq;
    struct {
        vsf_arch_irq_thread_t           irq_thread;
        vsf_arch_irq_request_t          irq_request;
        vsf_mem_stream_t                stream;
        uint8_t                         buffer[VSF_HOSTOS_USART_CFG_FIFO_SIZE];
        bool                            exited;
    } rx;
    struct {
        vsf_arch_irq_thread_t           irq_thread;
        vsf_arch_irq_request_t          irq_request;
        vsf_arch_irq_request_t          *irq_request_notifier;
        vsf_mem_stream_t                stream;
        uint8_t                         buffer[VSF_HOSTOS_USART_CFG_FIFO_SIZE];
        bool                            is_pending;
        bool                            need_tx_irq;
        bool                            exited;
    } tx;
} vsf_hostos_usart_t;

typedef struct vsf_hostos_usart_port_t {
    uint32_t                            ports_mask;
    vsf_hostos_usart_t                  * const (*ports)[VSF_HOSTOS_USART_COUNT];
    vsf_fifo2req_usart_t                * const (*fifo2req_ports)[VSF_HOSTOS_USART_COUNT];
} vsf_hostos_usart_port_t;

/*============================ PROTOTYPES ====================================*/

/*============================ INCLUDES ======================================*/

#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#define VSF_USART_CFG_IMP_PREFIX                    vsf_hostos
#define VSF_USART_CFG_IMP_UPCASE_PREFIX             VSF_HOSTOS
#define VSF_USART_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    vsf_hostos_usart_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, __IDX) = {\
        .handle                            = INVALID_HANDLE_VALUE,              \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/usart/usart_template.inc"

#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_REQUEST       ENABLED
#define VSF_USART_CFG_IMP_PREFIX                    VSF_HOSTOS_USART_PREFIX
#define VSF_USART_CFG_IMP_UPCASE_PREFIX             VSF_HOSTOS_USART_UNCASE_PREFIX
#define VSF_USART_CFG_IMP_COUNT_MASK_PREFIX         VSF_HOSTOS
#define VSF_USART_CFG_IMP_REMAP_PREFIX              vsf_fifo2req
#define VSF_USART_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    describe_fifo2req_usart(                                                    \
        VSF_USART_CFG_IMP_PREFIX,                                               \
        VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, __IDX),                  \
        VSF_MCONNECT(vsf_hostos_usart, __IDX))
#include "hal/driver/common/usart/usart_template.inc"

/*============================ LOCAL VARIABLES ===============================*/

static vsf_hostos_usart_port_t __vsf_hostos_usart_port = {
    .ports_mask = 0,
    .ports = &vsf_hostos_usarts,
    .fifo2req_ports = (vsf_fifo2req_usart_t * const (*)[32])&VSF_MCONNECT(VSF_HOSTOS_USART_PREFIX, _usarts),
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static char *__ansi_to_utf8(const char *ansiStr, int ansiLen) {
    int wideLen = MultiByteToWideChar(CP_ACP, 0, ansiStr, ansiLen, NULL, 0);
    wchar_t *wideStr = (wchar_t *)malloc(wideLen * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, ansiStr, ansiLen, wideStr, wideLen);

    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
    char *utf8Str = (char *)malloc(utf8Len);
    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, utf8Len, NULL, NULL);

    free(wideStr);
    return utf8Str;
}

uint8_t vsf_hostos_usart_scan_devices(vsf_hostos_usart_device_t *devices, uint8_t device_num)
{
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (INVALID_HANDLE_VALUE == hDevInfo) {
        return 0;
    }

    SP_DEVINFO_DATA devInfoData = {
        .cbSize     = sizeof(SP_DEVINFO_DATA),
    };
    DWORD result = 0;
    DWORD dwCount;
    HKEY hDevKey;
    int port_idx;
    char portName[256];
    char portFriendlyName[256];
    uint32_t mask = 0;
    uint8_t usart_num = 0;

    vsf_hostos_usart_t * const (*ports)[VSF_HOSTOS_USART_COUNT] = __vsf_hostos_usart_port.ports;

    while ((usart_num < device_num) && SetupDiEnumDeviceInfo(hDevInfo, result++, &devInfoData)) {
        hDevKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (hDevKey != INVALID_HANDLE_VALUE) {
            dwCount = dimof(portName);
            RegQueryValueExA(hDevKey, "PortName", NULL, NULL, (PBYTE)portName, &dwCount);
            RegCloseKey(hDevKey);
            SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)portFriendlyName, sizeof(portFriendlyName), NULL);

            port_idx = 0;
            if (    (1 == sscanf(portName, "COM%d", &port_idx))
                &&  (port_idx != 0) && (port_idx < dimof(*ports))) {

                mask |= 1 << port_idx;
                if (0 == (*ports)[port_idx]->port_idx) {
                     (*ports)[port_idx]->port_idx = port_idx;
                }

                if (devices != NULL) {
                    devices[usart_num].instance = (vsf_usart_t *)(*__vsf_hostos_usart_port.fifo2req_ports)[port_idx];

                    char *name_utf8 = __ansi_to_utf8(portName, sizeof(portName));
                    strncpy(devices[usart_num].name, name_utf8, sizeof(devices[usart_num].name));
                    free(name_utf8);

                    name_utf8 = __ansi_to_utf8(portFriendlyName, sizeof(portFriendlyName));
                    strncpy(devices[usart_num].friendly_name, name_utf8, sizeof(devices[usart_num].friendly_name));
                    __vsf_arch_trace(0, "[hal_win] usart%-2d 0x%p %s\n", port_idx, (*ports)[port_idx], name_utf8);
                    free(name_utf8);

                    usart_num++;
                }
            }
        }
    }
    SetupDiDestroyDeviceInfoList(hDevInfo);

    uint32_t changed_mask = __vsf_hostos_usart_port.ports_mask ^ mask;
    while (changed_mask) {
        port_idx = vsf_ffs32(changed_mask);
        changed_mask &= ~(1 << port_idx);

        if (!((1 << port_idx) & mask)) {
             (*ports)[port_idx]->port_idx = 0;
            if ( (*ports)[port_idx]->handle != INVALID_HANDLE_VALUE) {
                CloseHandle( (*ports)[port_idx]->handle);
                 (*ports)[port_idx]->handle = INVALID_HANDLE_VALUE;
            }
        }
    }
    __vsf_hostos_usart_port.ports_mask = mask;
    return usart_num;
}

static void __vsf_hostos_usart_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vsf_hostos_usart_t *hostos_usart = param;
    if (VSF_STREAM_ON_IN == evt) {
        __vsf_arch_irq_request_send(&hostos_usart->tx.irq_request);
    } else if (VSF_STREAM_ON_OUT == evt) {
        __vsf_arch_irq_request_send(&hostos_usart->rx.irq_request);
    }
}

static void __vsf_hostos_usart_rx_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vsf_hostos_usart_t *hostos_usart = vsf_container_of(irq_thread, vsf_hostos_usart_t, rx.irq_thread);

    OVERLAPPED overlapped = {
        .hEvent     = CreateEvent(NULL, TRUE, FALSE, NULL),
    };
    COMSTAT comstat;
    BOOL result;
    DWORD size, actual_size, last_error, evt_mask;
    uint8_t *buffer;

    __vsf_arch_irq_set_background(irq_thread);
    while (!hostos_usart->is_to_exit) {
        __vsf_arch_irq_start(irq_thread);
        size = vsf_stream_get_wbuf(&hostos_usart->rx.stream.use_as__vsf_stream_t, &buffer);
        __vsf_arch_irq_end(irq_thread, false);
        if (0 == size) {
            __vsf_arch_irq_request_send(&hostos_usart->rx.irq_request);
            continue;
        }

        ResetEvent(overlapped.hEvent);
        overlapped.Internal = 0;
        overlapped.InternalHigh = 0;
        overlapped.Offset = 0;
        overlapped.OffsetHigh = 0;

    check_comm_stat:
        if (!ClearCommError(hostos_usart->handle, NULL, &comstat)) {
            goto fail_and_exit;
        }
        if (0 == comstat.cbInQue) {
            result = WaitCommEvent(hostos_usart->handle, &evt_mask, &overlapped);
            if (!result && (GetLastError() != ERROR_IO_PENDING)) {
                goto fail_and_exit;
            }
            while (true) {
                result = GetOverlappedResultEx(hostos_usart->handle, &overlapped, &actual_size, 100, FALSE);
                if (hostos_usart->is_to_exit) {
                    goto exit_rx;
                }

                if (result != 0) {
                    break;
                } else {
                    last_error = GetLastError();
                    if (    (ERROR_IO_INCOMPLETE != last_error)
                        &&  (WAIT_IO_COMPLETION != last_error)
                        &&  (WAIT_TIMEOUT != last_error)) {
                        goto fail_and_exit;
                    }
                }
            }
            goto check_comm_stat;
        }

        size = vsf_min(size, comstat.cbInQue);
        ResetEvent(overlapped.hEvent);
        overlapped.Internal = 0;
        overlapped.InternalHigh = 0;
        overlapped.Offset = 0;
        overlapped.OffsetHigh = 0;
        result = ReadFile(hostos_usart->handle, buffer, size, &actual_size, &overlapped);
        if (!result && (GetLastError() != ERROR_IO_PENDING)) {
            goto fail_and_exit;
        }
        while (true) {
            result = GetOverlappedResultEx(hostos_usart->handle, &overlapped, &actual_size, 100, FALSE);
            if (hostos_usart->is_to_exit) {
                goto exit_rx;
            }

            if (result != 0) {
                break;
            } else {
                last_error = GetLastError();
                if (    (ERROR_IO_INCOMPLETE != last_error)
                    &&  (WAIT_IO_COMPLETION != last_error)
                    &&  (WAIT_TIMEOUT != last_error)) {
                    goto fail_and_exit;
                }
            }
        }

        __vsf_arch_irq_start(irq_thread);
        vsf_stream_write(&hostos_usart->rx.stream.use_as__vsf_stream_t, NULL, actual_size);
        if ((hostos_usart->irq.isrhandler != NULL) && (hostos_usart->irq_mask & VSF_USART_IRQ_MASK_RX)) {
            hostos_usart->irq.isrhandler(hostos_usart->irq.param, (vsf_usart_t *)hostos_usart, VSF_USART_IRQ_MASK_RX);
        }
        __vsf_arch_irq_end(irq_thread, false);
    }

fail_and_exit:
    vsf_trace_error("hostos_usart: failed while receiving data\n");
    vsf_hostos_usart_scan_devices(NULL, VSF_HOSTOS_USART_COUNT);
exit_rx:
    CloseHandle(overlapped.hEvent);
    __vsf_arch_irq_fini(irq_thread);
    hostos_usart->rx.exited = true;
    hostos_usart->is_to_exit = true;
}

static void __vsf_hostos_usart_tx_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vsf_hostos_usart_t *hostos_usart = vsf_container_of(irq_thread, vsf_hostos_usart_t, tx.irq_thread);

    OVERLAPPED overlapped = {
        .hEvent     = CreateEvent(NULL, TRUE, FALSE, NULL),
    };
    BOOL result;
    DWORD size, actual_size, last_error;
    uint8_t *buffer;

    __vsf_arch_irq_set_background(irq_thread);
    while (1) {
        __vsf_arch_irq_request_pend(&hostos_usart->tx.irq_request);
        if (hostos_usart->is_to_exit) {
            goto exit_tx;
        }
        if (hostos_usart->tx.need_tx_irq) {
            hostos_usart->tx.need_tx_irq = false;
            goto run_tx_irq;
        }

    get_rbuf:
        __vsf_arch_irq_start(irq_thread);
        size = vsf_stream_get_rbuf(&hostos_usart->tx.stream.use_as__vsf_stream_t, &buffer);
        __vsf_arch_irq_end(irq_thread, false);

        if (size > 0) {
            ResetEvent(overlapped.hEvent);
            overlapped.Internal = 0;
            overlapped.InternalHigh = 0;
            overlapped.Offset = 0;
            overlapped.OffsetHigh = 0;
            result = WriteFile(hostos_usart->handle, buffer, size, &actual_size, &overlapped);
            if (!result && (GetLastError() != ERROR_IO_PENDING)) {
                goto fail_and_exit;
            }
            while (true) {
                result = GetOverlappedResultEx(hostos_usart->handle, &overlapped, &actual_size, 100, FALSE);
                if (hostos_usart->is_to_exit) {
                    goto exit_tx;
                }
                if (result != 0) {
                    break;
                } else {
                    last_error = GetLastError();
                    if (    (ERROR_IO_INCOMPLETE != last_error)
                        &&  (WAIT_IO_COMPLETION != last_error)
                        &&  (WAIT_TIMEOUT != last_error)) {
                        goto fail_and_exit;
                    }
                }
            }

            __vsf_arch_irq_start(irq_thread);
            vsf_stream_read(&hostos_usart->tx.stream.use_as__vsf_stream_t, NULL, actual_size);
            __vsf_arch_irq_end(irq_thread, false);

            goto get_rbuf;
        } else {
            if (hostos_usart->irq_mask & VSF_USART_IRQ_MASK_TX) {
        run_tx_irq:
                hostos_usart->tx.is_pending = false;
                __vsf_arch_irq_start(irq_thread);
                if (hostos_usart->irq.isrhandler != NULL) {
                    hostos_usart->irq.isrhandler(hostos_usart->irq.param, (vsf_usart_t *)hostos_usart, VSF_USART_IRQ_MASK_TX);
                }
                __vsf_arch_irq_end(irq_thread, false);
            } else {
                hostos_usart->tx.is_pending = true;
            }

            if (hostos_usart->tx.irq_request_notifier != NULL) {
                __vsf_arch_irq_request_send(hostos_usart->tx.irq_request_notifier);
            }
        }
    }

fail_and_exit:
    vsf_trace_error("hostos_usart: failed while sending data\n");
    vsf_hostos_usart_scan_devices(NULL, VSF_HOSTOS_USART_COUNT);
exit_tx:
    __vsf_arch_irq_fini(irq_thread);
    hostos_usart->tx.exited = true;
    hostos_usart->is_to_exit = true;
}

vsf_err_t vsf_hostos_usart_init(vsf_hostos_usart_t *hostos_usart, vsf_usart_cfg_t *cfg)
{
    VSF_HAL_ASSERT(cfg != NULL);
    VSF_HAL_ASSERT(hostos_usart != NULL);

    if (0 == hostos_usart->port_idx) {
        return VSF_ERR_NOT_AVAILABLE;
    }

    if (hostos_usart->handle != INVALID_HANDLE_VALUE) {
        CloseHandle(hostos_usart->handle);
        hostos_usart->handle = INVALID_HANDLE_VALUE;
    }

    char file[16];
    snprintf(file, sizeof(file), "\\\\.\\COM%d", hostos_usart->port_idx);
    hostos_usart->handle = CreateFileA(file, GENERIC_WRITE | GENERIC_READ, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
    if (INVALID_HANDLE_VALUE == hostos_usart->handle) {
        return VSF_ERR_NOT_ACCESSIBLE;
    }

    hostos_usart->prio = cfg->isr.prio;
    hostos_usart->irq.param = cfg->isr.target_ptr;
    hostos_usart->irq.isrhandler = cfg->isr.handler_fn;
    hostos_usart->irq_mask = 0;

    DCB dcb;
    if (!GetCommState(hostos_usart->handle, &dcb)) {
        goto close_and_fail;
    }
    dcb.fAbortOnError = FALSE;
    dcb.fBinary = TRUE;
    dcb.BaudRate = cfg->baudrate;

    switch (cfg->mode & VSF_USART_BIT_LENGTH_MASK) {
    case VSF_USART_8_BIT_LENGTH:    dcb.ByteSize = 8;               break;
    case VSF_USART_9_BIT_LENGTH:    dcb.ByteSize = 9;               break;
    default:
        VSF_HAL_ASSERT(0);
        goto close_and_fail;
    }
    dcb.fParity = TRUE;
    switch (cfg->mode & VSF_USART_PARITY_MASK) {
    case VSF_USART_NO_PARITY:       dcb.Parity = NOPARITY;          break;
    case VSF_USART_EVEN_PARITY:     dcb.Parity = EVENPARITY;        break;
    case VSF_USART_ODD_PARITY:      dcb.Parity = ODDPARITY;         break;
    default:
        VSF_HAL_ASSERT(false);
        goto close_and_fail;
    }
    switch (cfg->mode & VSF_USART_STOPBIT_MASK) {
    case VSF_USART_1_STOPBIT:       dcb.StopBits = ONESTOPBIT;      break;
    case VSF_USART_1_5_STOPBIT:     dcb.StopBits = ONE5STOPBITS;    break;
    case VSF_USART_2_STOPBIT:       dcb.StopBits = TWOSTOPBITS;     break;
    default:
        VSF_HAL_ASSERT(false);
        goto close_and_fail;
    }
    if (!SetCommState(hostos_usart->handle, &dcb)) {
        goto close_and_fail;
    }

    PurgeComm(hostos_usart->handle, PURGE_TXCLEAR | PURGE_RXCLEAR);
    ClearCommError(hostos_usart->handle, NULL, NULL);
    SetCommMask(hostos_usart->handle, EV_RXCHAR);
    return VSF_ERR_NONE;

close_and_fail:
    CloseHandle(hostos_usart->handle);
    hostos_usart->handle = INVALID_HANDLE_VALUE;
    return VSF_ERR_FAIL;
}

void vsf_hostos_usart_fini(vsf_hostos_usart_t *hostos_usart)
{
    VSF_HAL_ASSERT(hostos_usart != NULL);
    VSF_HAL_ASSERT(hostos_usart->handle != INVALID_HANDLE_VALUE);

    CloseHandle(hostos_usart->handle);
    hostos_usart->handle = INVALID_HANDLE_VALUE;
}

vsf_usart_capability_t vsf_hostos_usart_capability(vsf_hostos_usart_t *hostos_usart)
{
    vsf_usart_capability_t usart_capability = {
        .irq_mask = HOSTFS_USART_IRQ_ALL_BITS_MASK,
        .max_baudrate = 4000000,
        .min_baudrate = 50,
        .min_data_bits = 8,
        .max_data_bits = 9,
        .txfifo_depth = dimof(hostos_usart->tx.buffer),
        .rxfifo_depth = dimof(hostos_usart->rx.buffer),
        .support_rx_timeout = 0,
    };

    return usart_capability;
}

fsm_rt_t vsf_hostos_usart_enable(vsf_hostos_usart_t *hostos_usart)
{
    VSF_HAL_ASSERT(hostos_usart != NULL);
    if (0 == hostos_usart->port_idx) {
        return fsm_rt_err;
    }

    if (!hostos_usart->irq_started) {
        hostos_usart->rx.stream.op = &vsf_mem_stream_op;
        hostos_usart->rx.stream.buffer = hostos_usart->rx.buffer;
        hostos_usart->rx.stream.size = dimof(hostos_usart->rx.buffer);
        hostos_usart->rx.stream.tx.evthandler = __vsf_hostos_usart_evthandler;
        hostos_usart->rx.stream.tx.param = hostos_usart;
        hostos_usart->tx.stream.op = &vsf_mem_stream_op;
        hostos_usart->tx.stream.buffer = hostos_usart->tx.buffer;
        hostos_usart->tx.stream.size = dimof(hostos_usart->tx.buffer);
        hostos_usart->tx.stream.rx.evthandler = __vsf_hostos_usart_evthandler;
        hostos_usart->tx.stream.rx.param = hostos_usart;
        vsf_stream_init(&hostos_usart->rx.stream.use_as__vsf_stream_t);
        vsf_stream_init(&hostos_usart->tx.stream.use_as__vsf_stream_t);
        vsf_stream_connect_rx(&hostos_usart->tx.stream.use_as__vsf_stream_t);

        hostos_usart->irq_started = true;
        hostos_usart->is_to_exit = false;
        hostos_usart->tx.exited = false;
        hostos_usart->rx.exited = false;
        __vsf_arch_irq_request_init(&hostos_usart->rx.irq_request);
        __vsf_arch_irq_request_init(&hostos_usart->tx.irq_request);
        __vsf_arch_irq_init(&hostos_usart->rx.irq_thread, "hostos_usart_rx", __vsf_hostos_usart_rx_thread, hostos_usart->prio);
        __vsf_arch_irq_init(&hostos_usart->tx.irq_thread, "hostos_usart_tx", __vsf_hostos_usart_tx_thread, hostos_usart->prio);
    }
    return fsm_rt_cpl;
}

fsm_rt_t vsf_hostos_usart_disable(vsf_hostos_usart_t *hostos_usart)
{
    VSF_HAL_ASSERT(hostos_usart != NULL);
    if (0 == hostos_usart->port_idx) {
        return fsm_rt_err;
    }
    if (!hostos_usart->irq_started) {
        return fsm_rt_err;
    }

    hostos_usart->irq_started = false;
    hostos_usart->is_to_exit = true;

    __vsf_arch_irq_request_send(&hostos_usart->tx.irq_request);
    while (!hostos_usart->tx.exited);
    while (!hostos_usart->rx.exited);

    __vsf_arch_irq_request_fini(&hostos_usart->tx.irq_request);
    __vsf_arch_irq_request_fini(&hostos_usart->rx.irq_request);

    vsf_stream_fini(&hostos_usart->tx.stream.use_as__vsf_stream_t);
    vsf_stream_fini(&hostos_usart->rx.stream.use_as__vsf_stream_t);

    return fsm_rt_cpl;
}

void vsf_hostos_usart_irq_enable(vsf_hostos_usart_t *hostos_usart, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(hostos_usart != NULL);
    VSF_HAL_ASSERT((irq_mask & ~HOSTFS_USART_IRQ_ALL_BITS_MASK) == 0);

    if (0 == hostos_usart->port_idx) {
        return;
    }

    if (hostos_usart->tx.is_pending && (irq_mask & VSF_USART_IRQ_MASK_TX) && !(hostos_usart->irq_mask & VSF_USART_IRQ_MASK_TX)) {
        hostos_usart->tx.need_tx_irq = true;
        __vsf_arch_irq_request_send(&hostos_usart->tx.irq_request);
    }

    hostos_usart->irq_mask |= irq_mask;
}

void vsf_hostos_usart_irq_disable(vsf_hostos_usart_t *hostos_usart, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(hostos_usart != NULL);
    VSF_HAL_ASSERT((irq_mask & ~HOSTFS_USART_IRQ_ALL_BITS_MASK) == 0);

    if (0 == hostos_usart->port_idx) {
        return;
    }

    if (hostos_usart->tx.is_pending && (irq_mask & VSF_USART_IRQ_MASK_TX)) {
        hostos_usart->tx.need_tx_irq = false;
        hostos_usart->tx.is_pending = false;
        __vsf_arch_irq_request_send(&hostos_usart->tx.irq_request);
    }

    hostos_usart->irq_mask &= ~irq_mask;
}

vsf_usart_status_t vsf_hostos_usart_status(vsf_hostos_usart_t *hostos_usart)
{
    VSF_HAL_ASSERT(hostos_usart != NULL);

    return (vsf_usart_status_t){ 0 };
}

uint_fast16_t vsf_hostos_usart_rxfifo_get_data_count(vsf_hostos_usart_t *hostos_usart)
{
    VSF_HAL_ASSERT(hostos_usart != NULL);
    if (0 == hostos_usart->port_idx) {
        return 0;
    }
    return vsf_stream_get_data_size(&hostos_usart->rx.stream.use_as__vsf_stream_t);
}

uint_fast16_t vsf_hostos_usart_rxfifo_read(vsf_hostos_usart_t *hostos_usart, void *buffer, uint_fast16_t size)
{
    VSF_HAL_ASSERT(hostos_usart != NULL);
    VSF_HAL_ASSERT(buffer != NULL);
    VSF_HAL_ASSERT(size > 0);

    if (0 == hostos_usart->port_idx) {
        return 0;
    }
    return vsf_stream_read(&hostos_usart->rx.stream.use_as__vsf_stream_t, buffer, size);
}

uint_fast16_t vsf_hostos_usart_txfifo_get_free_count(vsf_hostos_usart_t *hostos_usart)
{
    VSF_HAL_ASSERT(hostos_usart != NULL);
    if (0 == hostos_usart->port_idx) {
        return 0;
    }
    return vsf_stream_get_free_size(&hostos_usart->tx.stream.use_as__vsf_stream_t);;
}

uint_fast16_t vsf_hostos_usart_txfifo_write(vsf_hostos_usart_t *hostos_usart, void *buffer, uint_fast16_t size)
{
    VSF_HAL_ASSERT(hostos_usart != NULL);
    VSF_HAL_ASSERT(buffer != NULL);
    VSF_HAL_ASSERT(size > 0);

    if (0 == hostos_usart->port_idx) {
        return 0;
    }

#if VSF_HOSTOS_USART_CFG_TX_BLOCK == ENABLED
    vsf_arch_irq_request_t notifier_request = { 0 };
    __vsf_arch_irq_request_init(&notifier_request);
    hostos_usart->tx.irq_request_notifier = &notifier_request;

    vsf_arch_irq_thread_t *irq_thread = __vsf_arch_irq_get_cur();
    uint_fast16_t total_written_size = 0, cur_written_size;
    while (size > 0) {
        cur_written_size = vsf_stream_write(&hostos_usart->tx.stream.use_as__vsf_stream_t, buffer, size);
        buffer += cur_written_size;
        size -= cur_written_size;
        total_written_size += cur_written_size;

        __vsf_arch_irq_end(irq_thread, false);
        __vsf_arch_irq_request_pend(hostos_usart->tx.irq_request_notifier);
        __vsf_arch_irq_start(irq_thread);
    }
    __vsf_arch_irq_request_fini(hostos_usart->tx.irq_request_notifier);
    hostos_usart->tx.irq_request_notifier = NULL;
    return total_written_size;
#else
    return vsf_stream_write(&hostos_usart->tx.stream.use_as__vsf_stream_t, buffer, size);
#endif
}

/*============================ GLOBAL VARIABLES ==============================*/

#endif
