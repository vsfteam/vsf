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

#ifndef __HAL_DRIVER_FIFO2REQ_USART_H__
#define __HAL_DRIVER_FIFO2REQ_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if (VSF_HAL_USE_USART == ENABLED) && (VSF_HAL_USE_FIFO2REQ_USART == ENABLED)

#if defined(__VSF_HAL_FIFO2REQ_USART_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

// undef after include vsf_template_i2c.h
#define VSF_USART_CFG_DEC_PREFIX              vsf_fifo2req
#include "hal/driver/common/template/vsf_template_usart.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// TODO: add usart rx idle or timeout support
// TODO: support 9 bit mode

typedef struct vsf_fifo2req_usart_item_t {
    void           * buffer;
    uint32_t         max_count;
    uint32_t         count;
} vsf_fifo2req_usart_item_t;

vsf_class(vsf_fifo2req_usart_t) {
    private_member(
#if VSF_I2C_CFG_IMPLEMENT_OP == ENABLED
        vsf_usart_t vsf_usart;
#endif
        vsf_fifo2req_usart_item_t rx;
        vsf_fifo2req_usart_item_t tx;

        vsf_usart_isr_t isr;

        em_usart_irq_mask_t irq_mask;

        //uint8_t data_length;
    )

    public_member(
        vsf_usart_t *real_usart_ptr;
    )
};



/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif

#endif
/* EOF */