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

#include "hal/driver/common/template/vsf_template_hal_driver.h"

/*============================ MACROS ========================================*/
#if VSF_HAL_USE_USART == ENABLED

#ifndef VSF_USART_CFG_IMPLEMENT_OP
#   define VSF_USART_CFG_IMPLEMENT_OP                   DISABLED
#endif

#if VSF_USART_CFG_MULTI_INSTANCES == DISABLED
#   undef VSF_USART_CFG_INSTANCE_PREFIX
#   define VSF_USART_CFG_INSTANCE_PREFIX                vsf
#   define VSF_USART_OP
#else
#   ifndef VSF_USART_CFG_INSTANCE_PREFIX
#       define VSF_USART_CFG_INSTANCE_PREFIX            vsf_hw
#   endif
#   define ____VSF_USART_OP(__p) .vsf_usart.op = & __p ## _usart_op,
#   define __VSF_USART_OP(__p)   ____VSF_USART_OP(__p)
#   define VSF_USART_OP          __VSF_USART_OP(VSF_USART_CFG_INSTANCE_PREFIX)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define  __VSF_USART_FN(__prefix_name, __return, __name, ...)                   \
    VSF_TEMPLATE_HAL_API_DEFINE(__prefix_name, __return, __name, __VA_ARGS__)

#define VSF_USART_FN_INIT(__prefix_name)         __VSF_USART_FN(__prefix_name, vsf_err_t,      init,         vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)
#define VSF_USART_FN_ENABLE(__prefix_name)       __VSF_USART_FN(__prefix_name, fsm_rt_t,       enable,       vsf_usart_t *usart_ptr)
#define VSF_USART_FN_DISABLE(__prefix_name)      __VSF_USART_FN(__prefix_name, fsm_rt_t,       disable,      vsf_usart_t *usart_ptr)
#define VSF_USART_FN_IRQ_ENABLE(__prefix_name)   __VSF_USART_FN(__prefix_name, void,           irq_enable,   vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
#define VSF_USART_FN_IRQ_DISABLE(__prefix_name)  __VSF_USART_FN(__prefix_name, void,           irq_disable,  vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
#define VSF_USART_FN_STATUS(__prefix_name)       __VSF_USART_FN(__prefix_name, usart_status_t, status,       vsf_usart_t *usart_ptr)
#define VSF_USART_FN_FIFO_READ(__prefix_name)    __VSF_USART_FN(__prefix_name, uint_fast16_t,  fifo_read,    vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
#define VSF_USART_FN_FIFO_WRITE(__prefix_name)   __VSF_USART_FN(__prefix_name, uint_fast16_t,  fifo_write,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
#define VSF_USART_FN_FIFO_FLUSH(__prefix_name)   __VSF_USART_FN(__prefix_name, bool,           fifo_flush,   vsf_usart_t *usart_ptr)
#define VSF_USART_FN_REQUEST_RX(__prefix_name)   __VSF_USART_FN(__prefix_name, vsf_err_t,      request_rx,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
#define VSF_USART_FN_REQUEST_TX(__prefix_name)   __VSF_USART_FN(__prefix_name, vsf_err_t,      request_tx,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
#define VSF_USART_FN_CANCEL_RX(__prefix_name)    __VSF_USART_FN(__prefix_name, vsf_err_t,      cancel_rx,    vsf_usart_t *usart_ptr)
#define VSF_USART_FN_CANCEL_TX(__prefix_name)    __VSF_USART_FN(__prefix_name, vsf_err_t,      cancel_tx,    vsf_usart_t *usart_ptr)
#define VSF_USART_FN_GET_RX_COUNT(__prefix_name) __VSF_USART_FN(__prefix_name, int_fast32_t,   get_rx_count, vsf_usart_t *usart_ptr)
#define VSF_USART_FN_GET_TX_COUNT(__prefix_name) __VSF_USART_FN(__prefix_name, int_fast32_t,   get_tx_count, vsf_usart_t *usart_ptr)

#define __VSF_USART_APIS(__prefix_name)                                                                                        \
    __VSF_USART_API(__prefix_name, vsf_err_t,      init,         vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)                 \
    __VSF_USART_API(__prefix_name, fsm_rt_t,       enable,       vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, fsm_rt_t,       disable,      vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, void,           irq_enable,   vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)         \
    __VSF_USART_API(__prefix_name, void,           irq_disable,  vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)         \
    __VSF_USART_API(__prefix_name, usart_status_t, status,       vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, uint_fast16_t,  fifo_read,    vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)\
    __VSF_USART_API(__prefix_name, uint_fast16_t,  fifo_write,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)\
    __VSF_USART_API(__prefix_name, bool,           fifo_flush,   vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, vsf_err_t,      request_rx,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)\
    __VSF_USART_API(__prefix_name, vsf_err_t,      request_tx,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)\
    __VSF_USART_API(__prefix_name, vsf_err_t,      cancel_rx,    vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, vsf_err_t,      cancel_tx,    vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, int_fast32_t,   get_rx_count, vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, int_fast32_t,   get_tx_count, vsf_usart_t *usart_ptr)

/*============================ PROTOTYPES ====================================*/

#undef   __VSF_USART_API
#define  __VSF_USART_API(__prefix_name, __return, __name, ...)                   \
    VSF_TEMPLATE_HAL_API_EXTERN(__prefix_name, __return, __name, __VA_ARGS__)

__VSF_USART_APIS(VSF_USART_CFG_INSTANCE_PREFIX)

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_USART_CFG_MULTI_INSTANCES == ENABLED && VSF_USART_CFG_IMPLEMENT_OP == ENABLED
static const vsf_usart_op_t vsf_hw_usart_op = {

#undef   __VSF_USART_API
#define  __VSF_USART_API(__prefix_name, __return, __name, ...)                   \
    VSF_TEMPLATE_HAL_API_OP(__prefix_name, __return, __name, __VA_ARGS__)

    __VSF_USART_APIS(VSF_USART_CFG_INSTANCE_PREFIX)
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ IMPLEMENTATION ================================*/

#endif // VSF_HAL_USE_USART == ENABLED
