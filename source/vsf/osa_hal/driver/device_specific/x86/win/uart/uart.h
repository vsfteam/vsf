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

#ifndef __OSA_HAL_X86_WIN_USART_H__
#define __OSA_HAL_X86_WIN_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
#define VSF_TRACE_CFG_PROTECT_LEVEL  interrupt
enum em_usart_mode_t {
    USART_8_BIT_LENGTH      = 0x0000U,
    USART_9_BIT_LENGTH      = 0x1000U,
    
    USART_1_STOPBIT         = 0x0000U,
    USART_2_STOPBIT         = 0x2000U,
    
    USART_NO_PARITY         = 0x0000U,
    USART_EVEN_PARITY       = 0x0400U,
    USART_ODD_PARITY        = 0x0600U,

    USART_NO_HWCONTROL      = 0x0000U,
    USART_RTS_HWCONTROL     = 0x0100U,
    USART_CTS_HWCONTROL     = 0x0200U,
    USART_RTS_CTS_HWCONTROL = 0x0300U,
    
/*  USART_RX_MODE           = 0x0004U,  */
/*  USART_TX_MODE           = 0x0008U,  */
/*  USART_TX_RX_MODE        = 0x000CU   */
};
/*    vsf_usart_t          */   
/*    uint8_t com_status   */
enum em_usart_status_t{
    /* Hardware enable */
    USART_ENABLE            = 0X00U,
    USART_DISABLE           = 0X01U,

    /* Serial port is busy */
    USART_IS_BUSY           = 0X02U,

    /* Performing read or write */
    USART_EVT_RD            = 0X04U,
    USART_EVT_WD            = 0X08U,
};

struct usart_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t    is_busy     : 1;
            uint32_t    evt_status  : 8;
        };
    };
};


struct vsf_usart_t {
    usart_evt_status_t able_flag;
    uint8_t com_num;
    uint8_t com_status;
    usart_cfg_t *cfg;
    uint8_t *buf;
    uint8_t user_fn_able;
    vsf_usart_evt_t *user_fn_rd_onebyt;
    vsf_usart_evt_t *user_fn_rd_block;
    vsf_usart_evt_t *user_fn_wd_onebyt;
    vsf_usart_evt_t *user_fn_wd_block;
    uint_fast32_t buf_size;
    HANDLE handle_com;
    vsf_arch_irq_thread_t irq_thread;
    vsf_arch_irq_request_t irq_request;
};

/*============================ PROTOTYPES ====================================*/
/* USART with serial numbers "10" and higher are not currently supported */

extern vsf_err_t vsf_usart_init(vsf_usart_t *usart, usart_cfg_t *cfg);
extern fsm_rt_t vsf_usart_enable(vsf_usart_t *usart);
extern fsm_rt_t vsf_usart_disable(vsf_usart_t *usart);
extern usart_status_t vsf_usart_status(vsf_usart_t *usart);
extern bool vsf_usart_read_byte(vsf_usart_t *usart, uint8_t *pbyte);
extern bool vsf_usart_write_byte(vsf_usart_t *usart, uint_fast8_t byte);
extern fsm_rt_t vsf_usart_request_read(vsf_usart_t *usart, uint8_t *buffer, uint_fast32_t size);
extern fsm_rt_t vsf_usart_request_write(vsf_usart_t *usart, uint8_t *buffer, uint_fast32_t size);

extern void               vsf_usart_evt_register(vsf_usart_t *usart_ptr, vsf_usart_evt_type_t type, vsf_usart_evt_t event);
extern usart_evt_status_t vsf_usart_evt_enable(vsf_usart_t *usart_ptr, usart_evt_status_t event_mask);
extern usart_evt_status_t vsf_usart_evt_disable(vsf_usart_t *usart_ptr, usart_evt_status_t event_mask);
extern void               vsf_usart_evt_resume(vsf_usart_t *usart_ptr, usart_evt_status_t event_status);

#endif      // __OSA_HAL_X86_WIN_USART_H__
