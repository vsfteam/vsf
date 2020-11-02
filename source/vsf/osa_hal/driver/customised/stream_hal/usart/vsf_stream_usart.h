/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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

#ifndef __OSA_HAL_DRIVER_STREAM_USART_H__
#define __OSA_HAL_DRIVER_STREAM_USART_H__

/*============================ INCLUDES ======================================*/
#include "osa_hal/vsf_osa_hal_cfg.h"

#if VSF_USE_STREAM_HAL == ENABLED && VSF_USE_STREAM_USART == ENABLED

#include "hal/vsf_hal.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_STREAM_USART_CLASS_IMPLEMENT)
#   undef __VSF_STREAM_USART_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_STREAM_USART_CLASS_INHERIT)
#   undef __VSF_STREAM_USART_CLASS_INHERIT
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

// To use stream_usart, vsf_stream_usart_irq and vsf_stream_usart_t instance MUST
//  be set ad isr when calling vsf_usart_init

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_stream_usart_t)

def_simple_class(vsf_stream_usart_t) {
    public_member(
        vsf_usart_t *usart;
        vsf_stream_t *stream_tx;
        vsf_stream_t *stream_rx;
    )
    private_member(
        uint32_t cur_tx_len;
        uint8_t *cur_tx_buf;
        uint8_t rxbuf;
        bool is_txing;
        bool is_rxing;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_stream_usart_init(vsf_stream_usart_t *stream_usart);
extern void vsf_stream_usart_fini(vsf_stream_usart_t *stream_usart);

extern void vsf_stream_usart_irq(void *param, vsf_usart_t *usart, em_usart_irq_mask_t irq_mask);

#endif
#endif

/* EOF */
