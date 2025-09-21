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

#include "hal/vsf_hal.h"

#if VSF_HAL_USE_USART == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_USART_CLASS_IMPLEMENT
#include "./vsf_remapped_usart.h"

#if VSF_USART_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_USART_CFG_MULTI_CLASS == ENABLED
const vsf_usart_op_t vsf_remapped_usart_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_USART_APIS(vsf_remapped_usart)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_usart_init(vsf_remapped_usart_t *usart, vsf_usart_cfg_t *cfg)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_init(usart->target, cfg);
}

void vsf_remapped_usart_fini(vsf_remapped_usart_t *usart)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    vsf_usart_fini(usart->target);
}

vsf_err_t vsf_remapped_usart_get_configuration(vsf_remapped_usart_t *usart, vsf_usart_cfg_t *cfg)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_get_configuration(usart->target, cfg);
}

fsm_rt_t vsf_remapped_usart_enable(vsf_remapped_usart_t *usart)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_enable(usart->target);
}

fsm_rt_t vsf_remapped_usart_disable(vsf_remapped_usart_t *usart)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_disable(usart->target);
}

void vsf_remapped_usart_irq_enable(vsf_remapped_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    vsf_usart_irq_enable(usart->target, irq_mask);
}

void vsf_remapped_usart_irq_disable(vsf_remapped_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    vsf_usart_irq_disable(usart->target, irq_mask);
}

vsf_usart_status_t vsf_remapped_usart_status(vsf_remapped_usart_t *usart)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_status(usart->target);
}

vsf_usart_capability_t vsf_remapped_usart_capability(vsf_remapped_usart_t *usart)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_capability(usart->target);
}

uint_fast16_t vsf_remapped_usart_rxfifo_get_data_count(vsf_remapped_usart_t *usart)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_rxfifo_get_data_count(usart->target);
}

uint_fast16_t vsf_remapped_usart_rxfifo_read(vsf_remapped_usart_t *usart, void *buffer, uint_fast16_t count)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_rxfifo_read(usart->target, buffer, count);
}

uint_fast16_t vsf_remapped_usart_txfifo_get_free_count(vsf_remapped_usart_t *usart)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_txfifo_get_free_count(usart->target);
}

uint_fast16_t vsf_remapped_usart_txfifo_write(vsf_remapped_usart_t *usart, void *buffer, uint_fast16_t count)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_txfifo_write(usart->target, buffer, count);
}

vsf_err_t vsf_remapped_usart_request_rx(vsf_remapped_usart_t *usart, void *buffer, uint_fast32_t count)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_request_rx(usart->target, buffer, count);
}

vsf_err_t vsf_remapped_usart_request_tx(vsf_remapped_usart_t *usart, void *buffer, uint_fast32_t count)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_request_tx(usart->target, buffer, count);
}

vsf_err_t vsf_remapped_usart_cancel_rx(vsf_remapped_usart_t *usart)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_cancel_rx(usart->target);
}

vsf_err_t vsf_remapped_usart_cancel_tx(vsf_remapped_usart_t *usart)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_cancel_tx(usart->target);
}

int_fast32_t vsf_remapped_usart_get_rx_count(vsf_remapped_usart_t *usart)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_get_rx_count(usart->target);
}

int_fast32_t vsf_remapped_usart_get_tx_count(vsf_remapped_usart_t *usart)
{
    VSF_HAL_ASSERT((usart != NULL) && (usart->target != NULL));
    return vsf_usart_get_tx_count(usart->target);
}

vsf_err_t vsf_remapped_usart_ctrl(vsf_remapped_usart_t *usart_ptr, vsf_usart_ctrl_t ctrl, void *param)
{
    return vsf_usart_ctrl(usart_ptr, ctrl, param);
}


#endif
#endif
