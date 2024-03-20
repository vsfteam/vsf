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

#if VSF_HAL_USE_USART == ENABLED

#if defined(__VSF_HAL_USE_FIFO2REQ_USART_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ MACROS ========================================*/

#ifndef VSF_FIFO2REQ_USART_CFG_MULTI_CLASS
#   define VSF_FIFO2REQ_USART_CFG_MULTI_CLASS   VSF_USART_CFG_MULTI_CLASS
#endif

#if VSF_FIFO2REQ_USART_CFG_MULTI_CLASS == ENABLED
#   define __describe_fifo2req_usart_op()       .op = &vsf_fifo2req_usart_op,
#else
#   define __describe_fifo2req_usart_op()
#endif

#define __describe_fifo2req_usart(__prefix, __name, __usart)                    \
    VSF_MCONNECT(__prefix, _usart_t) __name = {                                 \
        __describe_fifo2req_usart_op()                                          \
        .usart = (vsf_usart_t *) & __usart,                                     \
    };

#define describe_fifo2req_usart(__prefix, __name, __usart)                      \
            __describe_fifo2req_usart(__prefix, __name, __usart)

/*============================ TYPES =========================================*/

// TODO: add usart rx idle or timeout support
// TODO: support 9 bit mode

typedef struct vsf_fifo2req_usart_item_t {
    void     * buffer;
    uint32_t   max_count;
    uint32_t   count;
} vsf_fifo2req_usart_item_t;

vsf_class(vsf_fifo2req_usart_t) {
    public_member(
#if VSF_FIFO2REQ_USART_CFG_MULTI_CLASS == ENABLED
        implement(vsf_usart_t)
#endif
        vsf_usart_t * usart;
    )

    private_member(
        vsf_fifo2req_usart_item_t rx;
        vsf_fifo2req_usart_item_t tx;

        vsf_usart_isr_t isr;
        vsf_usart_irq_mask_t irq_mask;
    )
};

/*============================ INCLUDES ======================================*/

#define VSF_USART_CFG_DEC_PREFIX              vsf_fifo2req
#define VSF_USART_CFG_DEC_UPCASE_PREFIX       VSF_FIFO2REQ
#define VSF_USART_CFG_DEC_EXTERN_OP           ENABLED
#include "hal/driver/common/usart/usart_template.h"

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif

#endif
/* EOF */
