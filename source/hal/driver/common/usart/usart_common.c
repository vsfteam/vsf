/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                  *
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

#define VSF_USART_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_USART == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#if VSF_USART_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_usart_init(vsf_usart_t *usart_ptr, vsf_usart_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->init != NULL);

    return usart_ptr->op->init(usart_ptr, cfg_ptr);
}

void vsf_usart_fini(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->init != NULL);

    usart_ptr->op->fini(usart_ptr);
}

fsm_rt_t vsf_usart_enable(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->enable != NULL);

    return usart_ptr->op->enable(usart_ptr);
}

fsm_rt_t vsf_usart_disable(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->disable != NULL);

    return usart_ptr->op->disable(usart_ptr);
}

void vsf_usart_irq_enable(vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->irq_enable != NULL);

    usart_ptr->op->irq_enable(usart_ptr, irq_mask);
}

void vsf_usart_irq_disable(vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->irq_disable != NULL);

    usart_ptr->op->irq_disable(usart_ptr, irq_mask);
}

vsf_usart_status_t vsf_usart_status(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->status != NULL);

    return usart_ptr->op->status(usart_ptr);
}

vsf_usart_capability_t vsf_usart_capability(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->capability != NULL);

    return usart_ptr->op->capability(usart_ptr);
}

uint_fast16_t vsf_usart_rxfifo_get_data_count(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->rxfifo_get_data_count != NULL);

    return usart_ptr->op->rxfifo_get_data_count(usart_ptr);
}

uint_fast16_t vsf_usart_rxfifo_read(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->rxfifo_read != NULL);

    return usart_ptr->op->rxfifo_read(usart_ptr, buffer_ptr, count);
}

uint_fast16_t vsf_usart_txfifo_get_free_count(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->txfifo_get_free_count != NULL);

    return usart_ptr->op->txfifo_get_free_count(usart_ptr);
}

uint_fast16_t vsf_usart_txfifo_write(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->txfifo_write != NULL);

    return usart_ptr->op->txfifo_write(usart_ptr, buffer_ptr, count);
}

vsf_err_t vsf_usart_request_rx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->request_rx != NULL);

    return usart_ptr->op->request_rx(usart_ptr, buffer_ptr, count);
}

vsf_err_t vsf_usart_request_tx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->request_tx != NULL);

    return usart_ptr->op->request_tx(usart_ptr, buffer_ptr, count);
}

vsf_err_t vsf_usart_cancel_rx(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->cancel_rx != NULL);

    return usart_ptr->op->cancel_rx(usart_ptr);
}

vsf_err_t vsf_usart_cancel_tx(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->cancel_tx != NULL);

    return usart_ptr->op->cancel_tx(usart_ptr);
}

int_fast32_t vsf_usart_get_rx_count(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->get_rx_count != NULL);

    return usart_ptr->op->get_rx_count(usart_ptr);
}

int_fast32_t vsf_usart_get_tx_count(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->get_tx_count != NULL);

    return usart_ptr->op->get_tx_count(usart_ptr);
}

vsf_err_t vsf_usart_ctrl(vsf_usart_t *usart_ptr, vsf_usart_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr->op != NULL);
    VSF_HAL_ASSERT(usart_ptr->op->ctrl != NULL);

    return usart_ptr->op->ctrl(usart_ptr, ctrl, param);
}

#endif /* VSF_USART_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_USART == ENABLED */
