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

#if VSF_HAL_USE_USART == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#ifndef VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY              DISALBED
#endif

#if VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY == ENABLED
#   ifndef VSF_USART_CFG_CAPABILITY_IRQ_MASK
#       define VSF_USART_CFG_CAPABILITY_IRQ_MASK                VSF_USART_IRQ_ALL_BITS_MASK
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_MAX_BAUDRATE
#       define VSF_USART_CFG_CAPABILITY_MAX_BAUDRATE            921600
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_MIN_BAUDRATE
#       define VSF_USART_CFG_CAPABILITY_MIN_BAUDRATE            4800
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_MAX_TX_FIFO_COUNTER
#       define VSF_USART_CFG_CAPABILITY_MAX_TX_FIFO_COUNTER     64
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_MAX_RX_FIFO_COUNTER
#       define VSF_USART_CFG_CAPABILITY_MAX_RX_FIFO_COUNTER     64
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_MAX_DATA_BITS
#       define VSF_USART_CFG_CAPABILITY_MAX_DATA_BITS           9
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_MIN_DATA_BITS
#       define VSF_USART_CFG_CAPABILITY_MIN_DATA_BITS           8
#   endif
#   ifndef VSF_USART_CFG_CAPABILITY_SUPPORT_RX_TIMEOUT
#       define VSF_USART_CFG_CAPABILITY_SUPPORT_RX_TIMEOUT      1
#   endif
#endif

#define vsf_real_usart_t                    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t)
#define vsf_real_usart_request_rx           VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_rx)
#define vsf_real_usart_cancel_rx            VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_rx)
#define vsf_real_usart_get_rx_count         VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_rx_count)
#define vsf_real_usart_request_tx           VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_tx)
#define vsf_real_usart_cancel_tx            VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_tx)
#define vsf_real_usart_get_tx_count         VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_tx_count)
#define vsf_real_usart_capability           VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_capability)

/*============================ IMPLEMENTATION ================================*/

#if VSF_USART_CFG_IMP_FIFO_TO_REQUEST == ENABLED

vsf_err_t vsf_real_usart_request_rx(vsf_real_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}

vsf_err_t vsf_real_usart_cancel_rx(vsf_real_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}

int_fast32_t vsf_real_usart_get_rx_count(vsf_real_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(0);
    return -1;
}

vsf_err_t vsf_real_usart_request_tx(vsf_real_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}

vsf_err_t vsf_real_usart_cancel_tx(vsf_real_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}

int_fast32_t vsf_real_usart_get_tx_count(vsf_real_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(0);
    return -1;
}

#endif

#if VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY == ENABLED
vsf_usart_capability_t vsf_real_usart_capability(vsf_real_usart_t *usart_ptr)
{
    vsf_usart_capability_t usart_capability = {
        .irq_mask               = VSF_USART_CFG_CAPABILITY_IRQ_MASK,
        .max_baudrate           = VSF_USART_CFG_CAPABILITY_MAX_BAUDRATE,
        .min_baudrate           = VSF_USART_CFG_CAPABILITY_MIN_BAUDRATE,
        .max_tx_fifo_counter    = VSF_USART_CFG_CAPABILITY_MAX_TX_FIFO_COUNTER,
        .max_rx_fifo_counter    = VSF_USART_CFG_CAPABILITY_MAX_RX_FIFO_COUNTER,
        .max_data_bits          = VSF_USART_CFG_CAPABILITY_MAX_DATA_BITS,
        .min_data_bits          = VSF_USART_CFG_CAPABILITY_MIN_DATA_BITS,
        .support_rx_timeout     = VSF_USART_CFG_CAPABILITY_SUPPORT_RX_TIMEOUT,
    };

    return usart_capability;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_USART_CFG_CAPABILITY_IRQ_MASK
#undef VSF_USART_CFG_CAPABILITY_MAX_BAUDRATE
#undef VSF_USART_CFG_CAPABILITY_MIN_BAUDRATE
#undef VSF_USART_CFG_CAPABILITY_MAX_TX_FIFO_COUNTER
#undef VSF_USART_CFG_CAPABILITY_MAX_RX_FIFO_COUNTER
#undef VSF_USART_CFG_CAPABILITY_MAX_DATA_BITS
#undef VSF_USART_CFG_CAPABILITY_MIN_DATA_BITS
#undef VSF_USART_CFG_CAPABILITY_SUPPORT_RX_TIMEOUT
#undef VSF_USART_CFG_IMP_FIFO_TO_REQUEST
#undef vsf_real_usart_t
#undef vsf_real_usart_init
#undef vsf_real_usart_fini
#undef vsf_real_usart_rxfifo_read
#undef vsf_real_usart_txfifo_write
#undef vsf_real_usart_irq_enable
#undef vsf_real_usart_irq_disable
#undef vsf_real_usart_request_rx
#undef vsf_real_usart_cancel_rx
#undef vsf_real_usart_get_rx_count
#undef vsf_real_usart_request_tx
#undef vsf_real_usart_cancel_tx
#undef vsf_real_usart_get_tx_count
#undef vsf_real_usart_capability

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                     _usart
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME              _USART

#ifndef VSF_USART_CFG_IMP_PREFIX
#   error "Please define VSF_USART_CFG_IMP_PREFIX in usart driver"
#endif

//#ifndef VSF_USART_CFG_IMP_UPCASE_PREFIX
//#   error "Please define VSF_USART_CFG_IMP_UPCASE_PREFIX in usart driver"
//#endif

#ifndef VSF_USART_CFG_IMP_INSTANCE_PREFIX
#   define VSF_USART_CFG_IMP_INSTANCE_PREFIX         VSF_USART_CFG_IMP_PREFIX
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_USART_CFG_IMP_PREFIX
#undef VSF_USART_CFG_IMP_INSTANCE_PREFIX
#undef VSF_USART_CFG_IMP_UPCASE_PREFIX
#undef VSF_USART_CFG_IMP_LV0

#endif  /* VSF_HAL_USE_USART */
