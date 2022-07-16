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

#ifndef VSF_USART_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_USART_CFG_REIMPLEMENT_CAPABILITY   DISABLED
#endif

#define vsf_real_usart_t                    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t)
#define vsf_real_usart_init                 VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)
#define vsf_real_usart_rxfifo_read          VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_read)
#define vsf_real_usart_txfifo_write         VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_write)
#define vsf_real_usart_irq_enable           VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_enable)
#define vsf_real_usart_irq_disable          VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_disable)
#define vsf_real_usart_request_rx           VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_rx)
#define vsf_real_usart_cancel_rx            VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_rx)
#define vsf_real_usart_get_rx_count         VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_rx_count)
#define vsf_real_usart_request_tx           VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_tx)
#define vsf_real_usart_cancel_tx            VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_tx)
#define vsf_real_usart_get_tx_count         VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_tx_count)
#define vsf_real_usart_capability           VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_capability)

#if VSF_USART_CFG_FIFO_TO_REQUEST == ENABLED
#   define __vsf_real_usart_init            VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_init)
#   define __vsf_real_usart_irq_enable      VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irq_enable)
#   define __vsf_real_usart_irq_disable     VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irq_disable)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USART_CFG_FIFO_TO_REQUEST == ENABLED

vsf_err_t vsf_real_usart_init(vsf_real_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)
{
    return vsf_usart_fifo2req_init(&usart_ptr->request,
                                   (vsf_usart_fifo2req_init_fn_t *)__vsf_real_usart_init,
                                   (vsf_usart_fifo2req_irq_fn_t  *)__vsf_real_usart_irq_enable,
                                   (vsf_usart_fifo2req_irq_fn_t  *)__vsf_real_usart_irq_disable,
                                   (vsf_usart_fifo2req_fifo_fn_t *)vsf_real_usart_rxfifo_read,
                                   (vsf_usart_fifo2req_fifo_fn_t *)vsf_real_usart_txfifo_write,
                                   (vsf_usart_t *)usart_ptr, cfg_ptr);
}

vsf_err_t vsf_real_usart_request_rx(vsf_real_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    return vsf_usart_fifo2req_request_rx(&usart_ptr->request, (vsf_usart_t *)usart_ptr, buffer_ptr, count);
}

vsf_err_t vsf_real_usart_cancel_rx(vsf_real_usart_t *usart_ptr)
{
    return vsf_usart_fifo2req_cancel_rx(&usart_ptr->request, (vsf_usart_t *)usart_ptr);
}

int_fast32_t vsf_real_usart_get_rx_count(vsf_real_usart_t *usart_ptr)
{
    return vsf_usart_fifo2req_get_rx_count(&usart_ptr->request, (vsf_usart_t *)usart_ptr);
}

vsf_err_t vsf_real_usart_request_tx(vsf_real_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    return vsf_usart_fifo2req_request_tx(&usart_ptr->request, (vsf_usart_t *)usart_ptr, buffer_ptr, count);
}

vsf_err_t vsf_real_usart_cancel_tx(vsf_real_usart_t *usart_ptr)
{
    return vsf_usart_fifo2req_cancel_tx(&usart_ptr->request, (vsf_usart_t *)usart_ptr);;
}

int_fast32_t vsf_real_usart_get_tx_count(vsf_real_usart_t *usart_ptr)
{
    return vsf_usart_fifo2req_get_tx_count(&usart_ptr->request, (vsf_usart_t *)usart_ptr);
}

void vsf_real_usart_irq_enable(vsf_real_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    vsf_usart_fifo2req_irq_enable(&usart_ptr->request, (vsf_usart_t *)usart_ptr, irq_mask);
}

void vsf_real_usart_irq_disable(vsf_real_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    vsf_usart_fifo2req_irq_disable(&usart_ptr->request, (vsf_usart_t *)usart_ptr, irq_mask);
}

#elif VSF_USART_CFG_REQUEST_EMPTY_IMPL == ENABLED

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

#if VSF_USART_CFG_REIMPLEMENT_CAPABILITY == DISABLED
usart_capability_t vsf_real_usart_capability(vsf_real_usart_t *usart_ptr)
{
    usart_capability_t usart_capability = {
        0,
    };

    return usart_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     _usart
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _USART

#ifndef VSF_USART_CFG_IMP_PREFIX
#   error "Please define VSF_USART_CFG_IMP_PREFIX in usart driver"
#endif

#ifndef VSF_USART_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_USART_CFG_IMP_UPCASE_PREFIX    in usart driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif // VSF_HAL_USE_USART == ENABLED
