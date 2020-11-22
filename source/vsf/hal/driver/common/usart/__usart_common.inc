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
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#ifdef VSF_HAL_USART_IMP_REQUEST_BY_FIFO

vsf_err_t vsf_usart_request_rx(vsf_usart_t *usart, void *buffer, uint_fast32_t count)
{
    VSF_HAL_ASSERT((NULL != usart) && (NULL != buffer) && (count > 0));

    // TODO: implement by fifo APIs
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vsf_usart_request_tx(vsf_usart_t *usart, void *buffer, uint_fast32_t count)
{
    VSF_HAL_ASSERT((NULL != usart) && (NULL != buffer) && (count > 0));

    // TODO: implement by fifo APIs
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vsf_usart_cancel_rx(vsf_usart_t *usart)
{
    VSF_HAL_ASSERT(NULL != usart);

    // TODO: implement by fifo APIs
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vsf_usart_cancel_tx(vsf_usart_t *usart)
{
    VSF_HAL_ASSERT(NULL != usart);

    // TODO: implement by fifo APIs
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

int_fast32_t vsf_usart_get_rx_count(vsf_usart_t *usart)
{
    VSF_HAL_ASSERT(false);
    return -1;
}

int_fast32_t vsf_usart_get_tx_count(vsf_usart_t *usart)
{
    VSF_HAL_ASSERT(false);
    return -1;
}

#endif

#if USART_MAX_PORT >= 0 && VSF_HAL_USE_USART0 == ENABLED && (USART_PORT_MASK & (1 << 0))
__vsf_hw_usart_imp_lv0(0, NULL)
#endif
#if USART_MAX_PORT >= 1 && VSF_HAL_USE_USART1 == ENABLED && (USART_PORT_MASK & (1 << 1))
__vsf_hw_usart_imp_lv0(1, NULL)
#endif
#if USART_MAX_PORT >= 2 && VSF_HAL_USE_USART2 == ENABLED && (USART_PORT_MASK & (1 << 2))
__vsf_hw_usart_imp_lv0(2, NULL)
#endif
#if USART_MAX_PORT >= 3 && VSF_HAL_USE_USART3 == ENABLED && (USART_PORT_MASK & (1 << 3))
__vsf_hw_usart_imp_lv0(3, NULL)
#endif
#if USART_MAX_PORT >= 4 && VSF_HAL_USE_USART4 == ENABLED && (USART_PORT_MASK & (1 << 4))
__vsf_hw_usart_imp_lv0(4, NULL)
#endif
#if USART_MAX_PORT >= 5 && VSF_HAL_USE_USART5 == ENABLED && (USART_PORT_MASK & (1 << 5))
__vsf_hw_usart_imp_lv0(5, NULL)
#endif
#if USART_MAX_PORT >= 6 && VSF_HAL_USE_USART6 == ENABLED && (USART_PORT_MASK & (1 << 6))
__vsf_hw_usart_imp_lv0(6, NULL)
#endif
#if USART_MAX_PORT >= 7 && VSF_HAL_USE_USART7 == ENABLED && (USART_PORT_MASK & (1 << 7))
__vsf_hw_usart_imp_lv0(7, NULL)
#endif

/*EOF*/