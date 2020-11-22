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
#include "utilities/vsf_utilities.h"
#include "./vsf_usart.h"

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_USART == ENABLED
#define VSF_USART_CFG_PROTECT_LEVEL    interrupt

#define __vsf_usart_protect            vsf_protect(VSF_USART_CFG_PROTECT_LEVEL)
#define __vsf_usart_unprotect          vsf_unprotect(VSF_USART_CFG_PROTECT_LEVEL)

/*============================ MACROFIED FUNCTIONS ===========================*/

/*usart_irqhandler*/
#define ____USART_LOCAL(__N, __DONT_CARE)                                       \
vsf_usart_t vsf_usart##__N = {                                                  \
    .reg_ptr = USART##__N,                                                      \
    .USART_IRQn = USART##__N##_IRQn,                                            \
};                                                                              \
ROOT void USART##__N##_IRQHandler(void)                                         \
{                                                                               \
    vsf_usart_irqhandler(&vsf_usart##__N);                                      \
}
#define __USART_LOCAL(__N, __DONT_CARE)                                         \
            ____USART_LOCAL(__N, __DONT_CARE)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


vsf_err_t vsf_usart_init(vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)
{
    ASSERT((NULL != usart_ptr) && (NULL != cfg_ptr));
    USART_TypeDef *pusart = usart_ptr->reg_ptr;

    float usart_div;
    //memset(usart_ptr, 0, sizeof(usart_ptr))

/*USART1_CLK_GPIO_NVIC*/
    if(USART1 == pusart) {
        RCC->APB2ENR |= USART1_CLK_EN|GPIOA_CLK_EN;

        GPIOA->CRH &= USART1_GPIO_MODE_CLEAR;
        GPIOA->CRH |= USART1_GPIO_MODE;

        usart_div = (float)PCLK2/(cfg_ptr->baudrate*16);
    }

/*USART2_CLK_GPIO_NVIC*/
    if(USART2 == pusart) {

        // should call
        //PM.Clock.SyncClock.Enable(usart_ptr->sync_clk_idx);
        // to replace the following
        RCC->APB1ENR |= USART2_CLK_EN;
        RCC->APB2ENR |= GPIOA_CLK_EN;

        // there should be no IO configuration here
        GPIOA->CRL &= USART2_GPIO_MODE_CLEAR;
        GPIOA->CRL |= USART2_GPIO_MODE;

        // should call
        //PM.Clock.Peripheral.Config(usart_ptr->pclk_idx, (pm_periph_asyn_clk_cfg_t) {...} );
        // to configure the PCLK for current usart, it is missing here

        // should call
        //PM.Clock.Peripheral.GetClock(usart_ptr->pclk_idx);
        // to replace the PCLKn register reading
        usart_div = (float)PCLK1/(cfg_ptr->baudrate*16);

        // should call
        //PM.Clock.Peripheral.Disable(usart_ptr->pclk_idx );
        // to disable PCLK for current usart

    }

/*USART3_CLK_GPIO_NVIC*/
    if(USART3 == pusart) {
        RCC->APB1ENR |= USART3_CLK_EN;
        RCC->APB2ENR |= GPIOB_CLK_EN;

        GPIOB->CRH &= USART3_GPIO_MODE_CLEAR;
        GPIOB->CRH |= USART3_GPIO_MODE;

        usart_div = (float)PCLK1/(cfg_ptr->baudrate*16);
    }

    HAL_NVIC_SetPriority(usart_ptr->USART_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(usart_ptr->USART_IRQn);

    //! calculate baudrate
    {
        uint16_t integer_part = (uint16_t)usart_div;
        uint16_t decimal_part = (uint16_t)((usart_div - integer_part) * 16);
        integer_part <<= 4;
        integer_part += decimal_part;

        pusart->BRR  = integer_part;
        pusart->CR1 |= cfg_ptr->mode;
        pusart->CR1 |= USART_EN;
    }

    // should call
    //PM.Clock.SyncClock.Disable(usart_ptr->sync_clk_idx);
    // to disable SyncClock for current USART, user should enable this usart
    // before use it

    return VSF_ERR_NONE;
}

/**********************************usart_enable**********************************/
fsm_rt_t vsf_usart_enable(vsf_usart_t *usart_ptr)
{
    ASSERT(NULL != usart_ptr);
    //! don't forget to enable sync clock and peripheral_clk here
    // should call
    //PM.Clock.SyncClock.Enable(usart_ptr->sync_clk_idx);
    // to enable sync clock

    // should call
    //PM.Clock.Peripheral.Enable(usart_ptr->pclk_idx );
    // to enable PCLK for current usart

    usart_ptr->reg_ptr->CR1 |= USART_EN;
    return fsm_rt_cpl;
}

/**********************************usart_disable*********************************/
fsm_rt_t vsf_usart_disable(vsf_usart_t *usart_ptr)
{
    ASSERT(NULL != usart_ptr);

    usart_ptr->reg_ptr->CR1 |= USART_DISEN;

    // should call
    //PM.Clock.Peripheral.Disable(usart_ptr->pclk_idx );
    // to disable PCLK for current usart

    // should call
    //PM.Clock.SyncClock.Disable(usart_ptr->sync_clk_idx);
    // to disable sync clock

    //! don't forget to disable sync clock and peripheral_clk here
    return fsm_rt_cpl;
}

/***********************************usart_status*********************************/
usart_status_t vsf_usart_status(vsf_usart_t *usart_ptr)
{
    ASSERT(NULL != usart_ptr);

    usart_status_t state_break = {
        .ip_is_busy = false,
        .bw_is_busy = false,
        .br_is_busy = false
    };

    if(usart_ptr->reg_ptr->SR & USART_SR_IDLE_FREE) {
        state_break.ip_is_busy = true;
    }

    if(false != usart_ptr->is_writing) {
        state_break.bw_is_busy = true;
    }

    if(false != usart_ptr->is_loading) {
        state_break.br_is_busy = true;
    }

    return state_break;
}

/*********************************usart_read_byte********************************/
bool vsf_usart_read_byte(vsf_usart_t *usart_ptr, uint8_t *byte_ptr)
{
    ASSERT((NULL != usart_ptr) && (NULL != byte_ptr));

    vsf_protect_t state = __vsf_usart_protect();

    if(usart_ptr->reg_ptr->SR & USART_SR_RXNE_FULL) {
        *byte_ptr = usart_ptr->reg_ptr->DR;

        usart_ptr->event_status.event_status |= VSF_USART_EVT_RX;
        if((usart_ptr->event_rx.handler_fn != NULL) && (usart_ptr->event_mask & VSF_USART_EVT_RX)) {
            usart_ptr->event_rx.handler_fn(usart_ptr->event_rx.target_ptr, usart_ptr, usart_ptr->event_status);
        }

        __vsf_usart_unprotect(state);
        return true;
    }

    __vsf_usart_unprotect(state);

    return false;
}

/*********************************usart_write_byte*******************************/
bool vsf_usart_write_byte(vsf_usart_t *usart_ptr, uint_fast8_t byte)
{
    ASSERT(NULL != usart_ptr);

    vsf_protect_t state;

    state = __vsf_usart_protect();

    if(usart_ptr->reg_ptr->SR & USART_SR_TXE_TRUE) {
        usart_ptr->reg_ptr->DR = byte;
    } else {
        __vsf_usart_unprotect(state);
        return false;
    }

    if(USART_SR_TC_TRUE == (usart_ptr->reg_ptr->SR & USART_SR_TC_TRUE)) {

        usart_ptr->event_status.event_status |= VSF_USART_EVT_TX;
        if((usart_ptr->event_tx.handler_fn != NULL) && (usart_ptr->event_mask & VSF_USART_EVT_TX)) {
            usart_ptr->event_tx.handler_fn(usart_ptr->event_tx.target_ptr, usart_ptr, usart_ptr->event_status);
        }

        __vsf_usart_unprotect(state);
        return true;
    }

    __vsf_usart_unprotect(state);

    return false;
}

/********************************usart_request_read******************************/
fsm_rt_t vsf_usart_request_read(vsf_usart_t *usart_ptr, uint8_t *buffer_ptr, uint_fast32_t size)
{
    ASSERT((NULL != usart_ptr) && (NULL != buffer_ptr) && (size > 0));

    vsf_protect_t state;

    state = __vsf_usart_protect();

    if(false == usart_ptr->is_loading) {
        usart_ptr->is_loading       = true;
        usart_ptr->read_buffer_ptr  = buffer_ptr;
        usart_ptr->read_size        = size - 1;
        usart_ptr->read_sizecounter = 0;
        usart_ptr->reg_ptr->CR1     |= USART_CR1_RXNEIE_EN;
    }

    __vsf_usart_unprotect(state);

    if(usart_ptr->read_sizecounter > usart_ptr->read_size) {
        usart_ptr->is_loading = false;

        usart_ptr->event_status.event_status |= VSF_USART_EVT_RCV_BLK_CPL;
        if((usart_ptr->event_rcv.handler_fn != NULL) && (usart_ptr->event_mask & VSF_USART_EVT_RCV_BLK_CPL)) {
            usart_ptr->event_rcv.handler_fn(usart_ptr->event_rcv.target_ptr, usart_ptr, usart_ptr->event_status);
        }

        return fsm_rt_cpl;
    }

    return fsm_rt_asyn;
}

/*******************************usart_request_write******************************/
fsm_rt_t vsf_usart_request_write(vsf_usart_t *usart_ptr, uint8_t *buffer_ptr, uint_fast32_t size)
{
    ASSERT((NULL != usart_ptr) && (NULL != buffer_ptr) && (size > 0));

    vsf_protect_t state;

    state = __vsf_usart_protect();

    if(false == usart_ptr->is_writing) {
        usart_ptr->is_writing        = true;
        usart_ptr->write_buffer_ptr  = buffer_ptr;
        usart_ptr->write_size        = size - 1;
        usart_ptr->write_sizecounter = 0;
        usart_ptr->reg_ptr->CR1      |= USART_CR1_TXEIE_EN;
    }

    __vsf_usart_unprotect(state);

    if(usart_ptr->write_sizecounter >= usart_ptr->write_size) {
        usart_ptr->is_writing = false;

        usart_ptr->event_status.event_status |= VSF_USART_EVT_SND_BLK_CPL;
        if((usart_ptr->event_send.handler_fn != NULL) && (usart_ptr->event_mask & VSF_USART_EVT_SND_BLK_CPL)) {
            usart_ptr->event_send.handler_fn(usart_ptr->event_send.target_ptr, usart_ptr, usart_ptr->event_status);
        }

        return fsm_rt_cpl;
    }

    return fsm_rt_asyn;
}

/*******************************vsf_usart_riqhandler*****************************/
void vsf_usart_irqhandler(vsf_usart_t *usart_ptr)
{
    if((usart_ptr->reg_ptr->SR & USART_SR_RXNE_FULL) && (false != usart_ptr->is_loading)) {
        usart_ptr->read_buffer_ptr[usart_ptr->read_sizecounter] = usart_ptr->reg_ptr->DR;
        if((usart_ptr->read_sizecounter++) >= usart_ptr->read_size) {
            usart_ptr->reg_ptr->CR1 &= USART_CR1_RXNEIE_DISEN;

            usart_ptr->is_loading = false;

            usart_ptr->event_status.event_status |= VSF_USART_EVT_RCV_BLK_CPL;
            if((usart_ptr->event_rcv.handler_fn != NULL) && (usart_ptr->event_mask & VSF_USART_EVT_RCV_BLK_CPL)) {
                usart_ptr->event_rcv.handler_fn(usart_ptr->event_rcv.target_ptr, usart_ptr, usart_ptr->event_status);
            }
        }
    }

    if((usart_ptr->reg_ptr->SR & USART_SR_TXE_TRUE) && (false != usart_ptr->is_writing)) {
        usart_ptr->reg_ptr->DR = usart_ptr->write_buffer_ptr[usart_ptr->write_sizecounter];
        if((usart_ptr->write_sizecounter++) >= usart_ptr->write_size) {
            usart_ptr->reg_ptr->CR1 &= USART_CR1_TXEIE_DISEN;

            usart_ptr->is_writing = false;

            usart_ptr->event_status.event_status |= VSF_USART_EVT_SND_BLK_CPL;
            if((usart_ptr->event_send.handler_fn != NULL) && (usart_ptr->event_mask & VSF_USART_EVT_SND_BLK_CPL)) {
                usart_ptr->event_send.handler_fn(usart_ptr->event_send.target_ptr, usart_ptr, usart_ptr->event_status);
            }
        }
    }
}

/*******************************vsf_event_usart_register***************************/
void vsf_usart_event_register(vsf_usart_t *usart_ptr, vsf_usart_event_type_t type, vsf_usart_event_t event)
{
    ASSERT(NULL != usart_ptr);

    switch(type) {
        case VSF_USART_EVT_RX:
            usart_ptr->event_rx = event;
            break;

        case VSF_USART_EVT_TX:
            usart_ptr->event_tx = event;
            break;

        case VSF_USART_EVT_RCV_BLK_CPL:
            usart_ptr->event_rcv = event;
            break;

        case VSF_USART_EVT_SND_BLK_CPL:
            usart_ptr->event_send = event;
            break;
    }
}

/*******************************vsf_usart_event_enable*****************************/
usart_event_status_t vsf_usart_event_enable(vsf_usart_t *usart_ptr, usart_event_status_t event_mask)
{
    ASSERT(NULL != usart_ptr);

    if(usart_ptr->event_mask != 0xFF) {
        usart_ptr->event_mask |= event_mask;
    }
    return usart_ptr->event_mask;
}

/*******************************vsf_usart_event_disable****************************/
usart_event_status_t vsf_usart_event_disable(vsf_usart_t *usart_ptr, usart_event_status_t event_mask)
{
    ASSERT(NULL != usart_ptr);

    if(usart_ptr->event_mask != 0x00) {
        usart_ptr->event_mask &= (~event_mask);
    }
    return usart_ptr->event_mask;
}

/*******************************vsf_usart_event_resume*****************************/
void vsf_usart_event_resume(vsf_usart_t *usart_ptr, usart_event_status_t event_status)
{
    ASSERT(NULL != usart_ptr);

    usart_ptr->event_mask = event_status;
}

#endif
