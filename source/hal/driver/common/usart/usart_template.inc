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

#define vsf_real_usart_t                    VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_t)
#define vsf_real_usart_request_rx           VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_request_rx)
#define vsf_real_usart_cancel_rx            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_cancel_rx)
#define vsf_real_usart_get_rx_count         VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_get_rx_count)
#define vsf_real_usart_request_tx           VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_request_tx)
#define vsf_real_usart_cancel_tx            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_cancel_tx)
#define vsf_real_usart_get_tx_count         VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_get_tx_count)
#define vsf_real_usart_capability           VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_capability)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USART_CFG_REQUEST_EMPTY_IMPL == ENABLED
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

#ifndef VSF_USART_CFG_PREFIX
#   error "Please define VSF_USART_CFG_PREFIX in usart driver"
#endif

#ifndef VSF_USART_CFG_UPPERCASE_PREFIX
#   error "Please define VSF_USART_CFG_UPPERCASE_PREFIX in usart driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif // VSF_HAL_USE_USART == ENABLED
