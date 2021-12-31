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

#include "vsf.h"

#if     VSF_USE_DISTBUS == ENABLED                                              \
    &&  APP_USE_DISTBUS_DEMO == ENABLED && APP_DISTBUS_DEMO_CFG_USART == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

bool __user_distbus_init(void *p, void (*on_inited)(void *p))
{
    return true;
}

bool __user_distbus_send(uint8_t *buffer, uint_fast32_t size, void *p, void (*on_sent)(void *p))
{
    return false;
}

bool __user_distbus_recv(uint8_t *buffer, uint_fast32_t size, void *p, void (*on_recv)(void *p))
{
    return false;
}

#endif
