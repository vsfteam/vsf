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

#ifndef __VSF_TEMPLATE_USART_H__
#define __VSF_TEMPLATE_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name class: usart_t
//! @{
def_interface(i_usart_t)
    union {
        implement(i_peripheral_t);
        struct {
            vsf_usart_status_t     (*Status)(void);
            vsf_usart_capability_t (*Capability)(void);
        } USART;
    };
    vsf_err_t (*Init)(vsf_usart_cfg_t *pCfg);

    //! Irq
    struct {
        void (*Enable)(vsf_usart_irq_mask_t tEventMask);
        void (*Disable)(vsf_usart_irq_mask_t tEventMask);
    } Irq;

    //! fifo access
    struct {
        //!< read from fifo
        uint_fast16_t (*Read)(void *pBuffer, uint_fast16_t nCount);
        //!< write to fifo
        uint_fast16_t (*Write)(void *pBuffer, uint_fast16_t nCount);
        //!< flush fifo
        bool (*Flush)(void);
    } FIFO;

    struct {
        vsf_async_block_access_t Read;
        vsf_async_block_access_t Write;
    } Block;

end_def_interface(i_usart_t)
//! @}


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif

#endif
/* EOF */
