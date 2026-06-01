/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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
 *****************************************************************************/

#ifndef __VSF_TEST_USART_H__
#define __VSF_TEST_USART_H__

/*============================ INCLUDES ======================================*/

#   include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"

#if     defined(__VSF_TEST_USART_CLASS_IMPLEMENT)
#   undef __VSF_TEST_USART_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#   include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO: hw_flow_control test uses internal loopback (LBE) which
// has no portable HAL API. Disabled until rewritten with portable abstractions.
#ifndef VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE
#   define VSF_TEST_USART_HW_FLOW_CONTROL_ENABLE  DISABLED
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

//! \brief 最大支持的用例数量
#define VSF_TEST_USART_CASE_MAX_COUNT   16

//! \brief 编译开关：默认启用 baud TX 场景
#ifndef VSF_TEST_USART_TX_BAUD_ENABLE
#   define VSF_TEST_USART_TX_BAUD_ENABLE     ENABLED
#endif

//! \brief 编译开关：默认启用 mode TX 场景
#ifndef VSF_TEST_USART_TX_MODE_ENABLE
#   define VSF_TEST_USART_TX_MODE_ENABLE     ENABLED
#endif

//! \brief 编译开关：默认启用 data RX 场景
#ifndef VSF_TEST_USART_RX_DATA_ENABLE
#   define VSF_TEST_USART_RX_DATA_ENABLE     ENABLED
#endif

//! \brief 编译开关：默认启用 baud RX 场景
#ifndef VSF_TEST_USART_RX_BAUD_ENABLE
#   define VSF_TEST_USART_RX_BAUD_ENABLE     ENABLED
#endif

//! \brief 编译开关：默认启用 mode RX 场景
#ifndef VSF_TEST_USART_RX_MODE_ENABLE
#   define VSF_TEST_USART_RX_MODE_ENABLE     ENABLED
#endif

//! \brief 编译开关：默认启用 IRQ RX 场景
#ifndef VSF_TEST_USART_RX_IRQ_ENABLE
#   define VSF_TEST_USART_RX_IRQ_ENABLE      ENABLED
#endif

//! \brief 编译开关：默认启用 timeout RX 场景
#ifndef VSF_TEST_USART_RX_TIMEOUT_ENABLE
#   define VSF_TEST_USART_RX_TIMEOUT_ENABLE  ENABLED
#endif

//! \brief 编译开关：默认启用 parity error RX 场景
#ifndef VSF_TEST_USART_RX_PARITY_ERROR_ENABLE
#   define VSF_TEST_USART_RX_PARITY_ERROR_ENABLE  ENABLED
#endif

//! \brief 编译开关：默认启用 frame error RX 场景
#ifndef VSF_TEST_USART_RX_FRAME_ERROR_ENABLE
#   define VSF_TEST_USART_RX_FRAME_ERROR_ENABLE   ENABLED
#endif
#ifndef VSF_TEST_USART_RX_BREAK_ERROR_ENABLE
#   define VSF_TEST_USART_RX_BREAK_ERROR_ENABLE   DISABLED
#endif
#ifndef VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE
#   define VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE DISABLED
#endif
#ifndef VSF_TEST_USART_BREAK_SIGNAL_ENABLE
#   define VSF_TEST_USART_BREAK_SIGNAL_ENABLE     DISABLED
#endif

//! \brief 编译开关：TX FIFO threshold IRQ + ISR refill (gap-fill PRD)
#ifndef VSF_TEST_USART_TX_FIFO_IRQ_ENABLE
#   define VSF_TEST_USART_TX_FIFO_IRQ_ENABLE      DISABLED
#endif

//! \brief 编译开关：pure RX FIFO threshold IRQ (gap-fill PRD)
#ifndef VSF_TEST_USART_RX_FIFO_IRQ_ENABLE
#   define VSF_TEST_USART_RX_FIFO_IRQ_ENABLE      DISABLED
#endif

//! \brief 编译开关：fifo2req_usart adapter request_tx + TX_CPL IRQ (gap-fill PRD)
#ifndef VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE
#   define VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE   DISABLED
#endif

//! \brief 编译开关：fifo2req_usart adapter request_rx + RX_CPL IRQ (gap-fill PRD)
#ifndef VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE
#   define VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE   DISABLED
#endif

//! \brief 编译开关：cancel_tx/cancel_rx + get_count partial (gap-fill PRD)
#ifndef VSF_TEST_USART_REQUEST_CANCEL_ENABLE
#   define VSF_TEST_USART_REQUEST_CANCEL_ENABLE   DISABLED
#endif

#ifndef VSF_TEST_USART_RX_BULK_IRQ_ENABLE
#   define VSF_TEST_USART_RX_BULK_IRQ_ENABLE      DISABLED
#endif

#ifndef VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE
#   define VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE DISABLED
#endif

#ifndef VSF_TEST_USART_ENABLE
#   define VSF_TEST_USART_ENABLE    ENABLED
#endif

#include "suite/vsf_test_usart_request_rx_irq.h"
#include "suite/vsf_test_usart_request_tx_irq.h"
#include "suite/vsf_test_usart_rx_bulk_irq.h"
#include "suite/vsf_test_usart_rx_data.h"
#include "suite/vsf_test_usart_rx_fifo_irq.h"
#include "suite/vsf_test_usart_rx_fifo_threshold.h"
#include "suite/vsf_test_usart_tx_fifo_irq.h"

#include "suite/vsf_test_usart_baud.h"
#include "suite/vsf_test_usart_break_signal.h"
#include "suite/vsf_test_usart_hw_flow_control.h"
#include "suite/vsf_test_usart_mode.h"
#include "suite/vsf_test_usart_request_cancel.h"
#include "suite/vsf_test_usart_rx_baud.h"
#include "suite/vsf_test_usart_rx_error.h"
#include "suite/vsf_test_usart_rx_irq.h"
#include "suite/vsf_test_usart_rx_mode.h"
#include "suite/vsf_test_usart_rx_timeout.h"
typedef union {
#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
    vsf_test_usart_request_rx_irq_data_t usart_request_rx_irq;
#endif
#if VSF_TEST_USART_REQUEST_TX_IRQ_ENABLE == ENABLED
    vsf_test_usart_request_tx_irq_data_t usart_request_tx_irq;
#endif
#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
    vsf_test_usart_rx_bulk_irq_data_t usart_rx_bulk_irq;
#endif
#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
    vsf_test_usart_rx_data_data_t usart_rx_data;
#endif
#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
    vsf_test_usart_rx_fifo_irq_data_t usart_rx_fifo_irq;
#endif
#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
    vsf_test_usart_rx_fifo_threshold_data_t usart_rx_fifo_threshold;
#endif
#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
    vsf_test_usart_tx_fifo_irq_data_t usart_tx_fifo_irq;
#endif
} vsf_test_usart_data_t;
#ifdef __cplusplus
}
#endif
#endif /* __VSF_TEST_USART_H__ */
/* EOF */