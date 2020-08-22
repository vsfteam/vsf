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

#include "./uart.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_usart_init(vsf_usart_t *usart, usart_cfg_t *cfg)
{
}

fsm_rt_t vsf_usart_enable(vsf_usart_t *usart)
{
}

fsm_rt_t vsf_usart_disable(vsf_usart_t *usart)
{
}

usart_status_t vsf_usart_status(vsf_usart_t *usart)
{
}

bool vsf_usart_read_byte(vsf_usart_t *usart, uint8_t *pbyte)
{
}

bool vsf_usart_write_byte(vsf_usart_t *usart, uint_fast8_t byte)
{
}

fsm_rt_t vsf_usart_request_read(vsf_usart_t *usart, uint8_t *buffer, uint_fast32_t size)
{
}

fsm_rt_t vsf_usart_request_write(vsf_usart_t *usart, uint8_t *buffer, uint_fast32_t size)
{
}

void vsf_usart_evt_register(vsf_usart_evt_type_t type, vsf_usart_evt_t evt)
{
}

usart_evt_status_t vsf_usart_evt_enable(usart_evt_status_t evt_mask)
{
}

usart_evt_status_t vsf_usart_evt_disable(usart_evt_status_t evt_mask)
{
}

void vsf_usart_evt_resume(usart_evt_status_t evt_status)
{
}
