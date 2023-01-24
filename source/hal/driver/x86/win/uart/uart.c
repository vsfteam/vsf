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

#define VSF_USART_CFG_IMP_PREFIX                    vsf_hw
#define VSF_USART_CFG_IMP_UPCASE_PREFIX             VSF_HW
#define VSF_USART_CFG_FIFO_TO_REQUEST               ENABLED

/*============================ INCLUDES ======================================*/

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#include "./uart.h"

#if VSF_HAL_USE_USART == ENABLED

#if VSF_USART_CFG_FIFO_TO_REQUEST == ENABLED
#   include "hal/driver/common/usart/fifo2req_usart.h"
#   define  vsf_hw_usart_init                       __vsf_hw_usart_init
#   define  vsf_hw_usart_irq_enable                 __vsf_hw_usart_irq_enable
#   define  vsf_hw_usart_irq_disable                __vsf_hw_usart_irq_disable
#endif

// for vsf_trace
#include "service/vsf_service.h"
#include <Windows.h>
#include <SetupAPI.h>
#include <tchar.h>

#pragma comment (lib, "SetupAPI.lib")

/*============================ MACROS ========================================*/

#ifndef VSF_HW_USART_CFG_MULTI_CLASS
#   define VSF_HW_USART_CFG_MULTI_CLASS             VSF_USART_CFG_MULTI_CLASS
#endif

#ifndef VSF_WIN_USART_CFG_TX_BLOCK
#   define VSF_WIN_USART_CFG_TX_BLOCK               DISABLED
#endif

#ifndef VSF_WIN_USART_CFG_FIFO_SIZE
#   define VSF_WIN_USART_CFG_FIFO_SIZE              1024
#endif

#ifndef VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL
#   define VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL  interrupt
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_usart_protect                         vsf_protect(VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL)
#define __vsf_usart_unprotect                       vsf_unprotect(VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/

typedef struct vsf_hw_usart_t {
#if VSF_HW_USART_CFG_MULTI_CLASS == ENABLED
    vsf_usart_t                         vsf_usart;
#endif

    uint8_t                             port_idx;
    bool                                irq_started;
    bool                                is_to_exit;
    vsf_usart_irq_mask_t                enable_flag;
    vsf_arch_prio_t                     prio;
    uint32_t                            irq_mask;

    HANDLE                              handle;
    struct {
        vsf_arch_irq_thread_t           irq_thread;
        vsf_arch_irq_request_t          irq_request;
        vsf_mem_stream_t                stream;
        uint8_t                         buffer[VSF_WIN_USART_CFG_FIFO_SIZE];
        bool                            exited;
    } rx;
    struct {
        vsf_arch_irq_thread_t           irq_thread;
        vsf_arch_irq_request_t          irq_request;
        vsf_mem_stream_t                stream;
        uint8_t                         buffer[VSF_WIN_USART_CFG_FIFO_SIZE];
        bool                            is_pending;
        bool                            exited;
    } tx;

#if VSF_USART_CFG_FIFO_TO_REQUEST == ENABLED
    vsf_usart_fifo2req_t request;
#endif
} vsf_hw_usart_t;

typedef struct vsf_win_usart_t {
    uint32_t                            port_mask;
    // TODO: remove vsf_win_usart_t if vsf_hal has device array
    vsf_hw_usart_t                      *port[VSF_HW_USART_COUNT];
} vsf_win_usart_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static vsf_win_usart_t __vsf_win_usart  = {
    .port_mask                          = 0,
    .port                               = {
#define VSF_WIN_USART_INIT_PORT(__N, __VALUE)                                   \
            [(__N)]                     = &VSF_MCONNECT(vsf_hw_usart, __N),
        VSF_MREPEAT(VSF_HW_USART_COUNT, VSF_WIN_USART_INIT_PORT, NULL)
    },
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

uint8_t vsf_hw_usart_scan_devices(vsf_usart_win_device_t *devices, uint8_t device_num)
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

    while ((result < device_num) && SetupDiEnumDeviceInfo(hDevInfo, result, &devInfoData)) {
        hDevKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (hDevKey != INVALID_HANDLE_VALUE) {
            dwCount = dimof(portName);
            RegQueryValueExA(hDevKey, "PortName", NULL, NULL, (PBYTE)portName, &dwCount);
            RegCloseKey(hDevKey);
            SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)portFriendlyName, sizeof(portFriendlyName), NULL);

            if (    (1 == sscanf(portName, "COM%d", &port_idx))
                &&  (port_idx != 0) && (port_idx < dimof(__vsf_win_usart.port))) {

                mask |= 1 << port_idx;
                if (0 == __vsf_win_usart.port[port_idx]->port_idx) {
                    __vsf_win_usart.port[port_idx]->port_idx = port_idx;
                    __vsf_arch_trace(0, "[hal_win] usart%-2d 0x%p %s\n", port_idx, __vsf_win_usart.port[port_idx], portFriendlyName);
                }

                if (devices != NULL) {
                    devices[result].instance = (vsf_usart_t *)__vsf_win_usart.port[port_idx];
                    devices[result].port = port_idx;
                }
                result++;
            }
        }
    }
    SetupDiDestroyDeviceInfoList(hDevInfo);

    uint32_t changed_mask = __vsf_win_usart.port_mask ^ mask;
    while (changed_mask) {
        port_idx = vsf_ffs32(changed_mask);
        changed_mask &= ~(1 << port_idx);

        if (!((1 << port_idx) & mask)) {
            __vsf_win_usart.port[port_idx]->port_idx = 0;
            if (__vsf_win_usart.port[port_idx]->handle != INVALID_HANDLE_VALUE) {
                CloseHandle(__vsf_win_usart.port[port_idx]->handle);
                __vsf_win_usart.port[port_idx]->handle = INVALID_HANDLE_VALUE;
            }
        }
    }
    __vsf_win_usart.port_mask = mask;
    return result;
}

static void __vsf_win_usart_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vsf_hw_usart_t *hw_usart = param;
    if (VSF_STREAM_ON_IN == evt) {
        __vsf_arch_irq_request_send(&hw_usart->tx.irq_request);
    } else if (VSF_STREAM_ON_OUT == evt) {
        __vsf_arch_irq_request_send(&hw_usart->rx.irq_request);
    }
}

static void __vsf_hw_usart_rx_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vsf_hw_usart_t *hw_usart = container_of(irq_thread, vsf_hw_usart_t, rx.irq_thread);

    OVERLAPPED overlapped = {
        .hEvent     = CreateEvent(NULL, TRUE, FALSE, NULL),
    };
    BOOL result;
    DWORD size, actual_size, last_error;
    uint8_t *buffer;

    __vsf_arch_irq_set_background(irq_thread);
    while (!hw_usart->is_to_exit) {
        __vsf_arch_irq_start(irq_thread);
        size = vsf_stream_get_wbuf(&hw_usart->rx.stream.use_as__vsf_stream_t, &buffer);
        __vsf_arch_irq_end(irq_thread, false);

        ResetEvent(overlapped.hEvent);
        result = ReadFile(hw_usart->handle, buffer, size, &actual_size, &overlapped);
        if (!result && (GetLastError() != ERROR_IO_PENDING)) {
        fail_and_exit:
            vsf_hw_usart_scan_devices(NULL, VSF_HW_USART_COUNT);
            goto exit_rx;
        }

        while (true) {
            result = GetOverlappedResultEx(hw_usart->handle, &overlapped, &actual_size, 100, FALSE);
            if (hw_usart->is_to_exit) {
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
        vsf_stream_write(&hw_usart->rx.stream.use_as__vsf_stream_t, NULL, actual_size);
        __vsf_arch_irq_end(irq_thread, false);
    }

exit_rx:
    CloseHandle(overlapped.hEvent);
    hw_usart->rx.exited = true;
    hw_usart->is_to_exit = true;
    __vsf_arch_irq_fini(irq_thread);
}

static void __vsf_hw_usart_tx_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vsf_hw_usart_t *hw_usart = container_of(irq_thread, vsf_hw_usart_t, tx.irq_thread);

    BOOL result;
    DWORD size;
    uint8_t *buffer;

    __vsf_arch_irq_set_background(irq_thread);
    while (!hw_usart->is_to_exit) {
        __vsf_arch_irq_request_pend(&hw_usart->tx.irq_request);

        __vsf_arch_irq_start(irq_thread);
        size = vsf_stream_get_rbuf(&hw_usart->tx.stream.use_as__vsf_stream_t, &buffer);
        __vsf_arch_irq_end(irq_thread, false);

        if (size > 0) {
            result = WriteFile(hw_usart->handle, buffer, size, NULL, NULL);
            if (!result) {
                vsf_hw_usart_scan_devices(NULL, VSF_HW_USART_COUNT);
                goto exit_tx;
            }
        }
    }

exit_tx:
    hw_usart->tx.exited = true;
    hw_usart->is_to_exit = true;
    __vsf_arch_irq_fini(irq_thread);
}

vsf_err_t vsf_hw_usart_init(vsf_hw_usart_t *hw_usart, vsf_usart_cfg_t *cfg)
{
    VSF_HAL_ASSERT(cfg != NULL);
    VSF_HAL_ASSERT(hw_usart != NULL);

    if (0 == hw_usart->port_idx) {
        return VSF_ERR_NOT_AVAILABLE;
    }

    if (hw_usart->handle != INVALID_HANDLE_VALUE) {
        CloseHandle(hw_usart->handle);
        hw_usart->handle = INVALID_HANDLE_VALUE;
    }

    char file[16];
    snprintf(file, sizeof(file), "\\\\.\\COM%d", hw_usart->port_idx);
    hw_usart->handle = CreateFileA(file, GENERIC_WRITE | GENERIC_READ, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
    if (INVALID_HANDLE_VALUE == hw_usart->handle) {
        return VSF_ERR_NOT_ACCESSIBLE;
    }

    hw_usart->prio = cfg->isr.prio;

    DCB dcb;
    if (!GetCommState(hw_usart->handle, &dcb)) {
        goto close_and_fail;
    }
    dcb.fAbortOnError = FALSE;
    dcb.fBinary = TRUE;
    dcb.BaudRate = cfg->baudrate;
    switch (cfg->mode & VSF_USART_BIT_LENGTH_MASK) {
    case VSF_USART_8_BIT_LENGTH:    dcb.ByteSize = 8;               break;
    case VSF_USART_9_BIT_LENGTH:    dcb.ByteSize = 9;               break;
    default:
        vsf_trace_error("hw_usart: bit length not supported" VSF_TRACE_CFG_LINEEND);
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
    }
    if (!SetCommState(hw_usart->handle, &dcb)) {
        goto close_and_fail;
    }

    PurgeComm(hw_usart->handle, PURGE_TXCLEAR | PURGE_RXCLEAR);
    ClearCommError(hw_usart->handle, NULL, NULL);
    return VSF_ERR_NONE;

close_and_fail:
    CloseHandle(hw_usart->handle);
    hw_usart->handle = INVALID_HANDLE_VALUE;
    return VSF_ERR_FAIL;
}

fsm_rt_t vsf_hw_usart_enable(vsf_hw_usart_t *hw_usart)
{
    VSF_HAL_ASSERT(hw_usart != NULL);
    if (0 == hw_usart->port_idx) {
        return fsm_rt_err;
    }

    if (!hw_usart->irq_started) {
        hw_usart->rx.stream.op = &vsf_mem_stream_op;
        hw_usart->rx.stream.buffer = hw_usart->rx.buffer;
        hw_usart->rx.stream.size = dimof(hw_usart->rx.buffer);
        hw_usart->rx.stream.tx.evthandler = __vsf_win_usart_evthandler;
        hw_usart->rx.stream.tx.param = hw_usart;
        hw_usart->tx.stream.op = &vsf_mem_stream_op;
        hw_usart->tx.stream.buffer = hw_usart->tx.buffer;
        hw_usart->tx.stream.size = dimof(hw_usart->tx.buffer);
        hw_usart->tx.stream.rx.evthandler = __vsf_win_usart_evthandler;
        hw_usart->tx.stream.rx.param = hw_usart;
        vsf_stream_init(&hw_usart->rx.stream.use_as__vsf_stream_t);
        vsf_stream_init(&hw_usart->tx.stream.use_as__vsf_stream_t);
        vsf_stream_connect_rx(&hw_usart->tx.stream.use_as__vsf_stream_t);

        hw_usart->irq_started = true;
        hw_usart->tx.exited = false;
        hw_usart->rx.exited = false;
        __vsf_arch_irq_request_init(&hw_usart->tx.irq_request);
        __vsf_arch_irq_init(&hw_usart->rx.irq_thread, "win_usart_rx", __vsf_hw_usart_rx_thread, hw_usart->prio);
        __vsf_arch_irq_init(&hw_usart->tx.irq_thread, "win_usart_tx", __vsf_hw_usart_tx_thread, hw_usart->prio);
    }
    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_usart_disable(vsf_hw_usart_t *hw_usart)
{
    VSF_HAL_ASSERT(hw_usart != NULL);
    if (0 == hw_usart->port_idx) {
        return fsm_rt_err;
    }

    if (hw_usart->irq_started) {
        hw_usart->is_to_exit = true;
    }
    return fsm_rt_cpl;
}

void vsf_hw_usart_irq_enable(vsf_hw_usart_t *hw_usart, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(hw_usart != NULL);
    if (0 == hw_usart->port_idx) {
        return;
    }
    hw_usart->irq_mask |= irq_mask;
}

void vsf_hw_usart_irq_disable(vsf_hw_usart_t *hw_usart, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(hw_usart != NULL);
    if (0 == hw_usart->port_idx) {
        return;
    }
    hw_usart->irq_mask &= ~irq_mask;
}

vsf_usart_status_t vsf_hw_usart_status(vsf_hw_usart_t *hw_usart)
{
    VSF_HAL_ASSERT(hw_usart != NULL);

    return (vsf_usart_status_t){ 0 };
}

uint_fast16_t vsf_hw_usart_rxfifo_get_data_count(vsf_hw_usart_t *hw_usart)
{
    VSF_HAL_ASSERT(hw_usart != NULL);
    if (0 == hw_usart->port_idx) {
        return 0;
    }
    return vsf_stream_get_data_size(&hw_usart->rx.stream.use_as__vsf_stream_t);
}

uint_fast16_t vsf_hw_usart_rxfifo_read(vsf_hw_usart_t *hw_usart, void *buffer, uint_fast16_t size)
{
    VSF_HAL_ASSERT(hw_usart != NULL);
    VSF_HAL_ASSERT(buffer != NULL);
    VSF_HAL_ASSERT(size > 0);

    if (0 == hw_usart->port_idx) {
        return 0;
    }
    return vsf_stream_read(&hw_usart->rx.stream.use_as__vsf_stream_t, buffer, size);
}

uint_fast16_t vsf_hw_usart_txfifo_get_free_count(vsf_hw_usart_t *hw_usart)
{
    VSF_HAL_ASSERT(hw_usart != NULL);
    if (0 == hw_usart->port_idx) {
        return 0;
    }
    return vsf_stream_get_free_size(&hw_usart->tx.stream.use_as__vsf_stream_t);;
}

uint_fast16_t vsf_hw_usart_txfifo_write(vsf_hw_usart_t *hw_usart, void *buffer, uint_fast16_t size)
{
    VSF_HAL_ASSERT(hw_usart != NULL);
    VSF_HAL_ASSERT(buffer != NULL);
    VSF_HAL_ASSERT(size > 0);

    if (0 == hw_usart->port_idx) {
        return 0;
    }
    return vsf_stream_write(&hw_usart->tx.stream.use_as__vsf_stream_t, buffer, size);
}

/*============================ INCLUDES ======================================*/

#if VSF_USART_CFG_FIFO_TO_REQUEST == ENABLED
#   define __USART_REQUEST_IMP                      VSF_USART_FIFO2REQ_IMP_LV0(VSF_USART_CFG_IMP_PREFIX)
#   undef  vsf_hw_usart_init
#   undef  vsf_hw_usart_irq_enable
#   undef  vsf_hw_usart_irq_disable
#else
#   define __USART_REQUEST_IMP
#endif

#define VSF_USART_CFG_IMP_LV0(__COUNT, __HAL_OP)                                \
    vsf_hw_usart_t vsf_hw_usart ## __COUNT = {                                  \
        .handle                            = INVALID_HANDLE_VALUE,              \
        __USART_REQUEST_IMP                                                     \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/usart/usart_template.inc"

#endif
