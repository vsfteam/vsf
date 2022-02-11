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

#include "hal/driver/common/template/vsf_template_hal_driver.h"
#if VSF_USART_CFG_FIFO_TO_REQUEST == ENABLED
#   include "hal/driver/common/usart/usart_request.h"
#endif

/*============================ MACROS ========================================*/
#if VSF_HAL_USE_USART == ENABLED

#ifndef VSF_USART_CFG_IMPLEMENT_OP
#   define VSF_USART_CFG_IMPLEMENT_OP                   DISABLED
#endif

#ifndef VSF_USART_CFG_INSTANCE_PREFIX
#   define VSF_USART_CFG_INSTANCE_PREFIX                vsf_hw
#endif

#define VSF_USART_CFG_FIFO2REQ_PREFIX                   vsf_fifo2req

#if VSF_USART_CFG_MULTI_CLASS == DISABLED
#   define VSF_USART_OP
#   define VSF_USART_FIFO2REQ_OP
#else
#   define ____VSF_USART_OP(__p)                        .vsf_usart.op = & __ ## __p ## _usart_op,
#   define __VSF_USART_OP(__p)                          ____VSF_USART_OP(__p)
#   define VSF_USART_OP                                 __VSF_USART_OP(VSF_USART_CFG_INSTANCE_PREFIX)
#   define VSF_USART_FIFO2REQ_OP                        __VSF_USART_OP(VSF_USART_CFG_FIFO2REQ_PREFIX)

#   define ____VSF_USART_OP_VAR(__p)                    __ ## __p ## _usart_op
#   define __VSF_USART_OP_VAR(__p)                      ____VSF_USART_OP_VAR(__p)
#   define VSF_USART_OP_VAR                             __VSF_USART_OP_VAR(VSF_USART_CFG_INSTANCE_PREFIX)
#   define VSF_USART_FIFO2REQ_OP_VAR                    __VSF_USART_OP_VAR(VSF_USART_CFG_FIFO2REQ_PREFIX)
#endif

#ifndef VSF_USART_CFG_IMPLEMENT_REQUEST_BY_FIFO
#   define VSF_USART_CFG_IMPLEMENT_REQUEST_BY_FIFO      DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_USART_FN_INIT               VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_init)
#define VSF_USART_FN_ENABLE             VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_enable)
#define VSF_USART_FN_DISABLE            VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_disable)
#define VSF_USART_FN_IRQ_ENABLE         VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_irq_enable)
#define VSF_USART_FN_IRQ_DISABLE        VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_irq_disable)
#define VSF_USART_FN_STATUS             VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_status)
#define VSF_USART_FN_FIFO_READ          VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_fifo_read)
#define VSF_USART_FN_FIFO_WRITE         VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_fifo_write)
#define VSF_USART_FN_REQUEST_RX         VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_request_rx)
#define VSF_USART_FN_REQUEST_TX         VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_request_tx)
#define VSF_USART_FN_CANCEL_RX          VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_cancel_rx)
#define VSF_USART_FN_CANCEL_TX          VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_cancel_tx)
#define VSF_USART_FN_GET_RX_COUNT       VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_get_rx_count)
#define VSF_USART_FN_GET_TX_COUNT       VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_get_tx_count)
#define VSF_USART_FN_GET_DATA_LENGTH    VSF_MCONNECT(VSF_USART_CFG_INSTANCE_PREFIX, _usart_get_data_length)

#define __VSF_USART_BASE_APIS(__prefix_name)                                                                                   \
    __VSF_USART_API(__prefix_name, vsf_err_t,      init,         vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)                 \
    __VSF_USART_API(__prefix_name, fsm_rt_t,       enable,       vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, fsm_rt_t,       disable,      vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, void,           irq_enable,   vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)         \
    __VSF_USART_API(__prefix_name, void,           irq_disable,  vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)         \
    __VSF_USART_API(__prefix_name, usart_status_t, status,       vsf_usart_t *usart_ptr)

#define __VSF_USART_FIFO_APIS(__prefix_name)                                                                                   \
    __VSF_USART_API(__prefix_name, uint_fast16_t,  fifo_read,    vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)\
    __VSF_USART_API(__prefix_name, uint_fast16_t,  fifo_write,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)

#define __VSF_USART_REQUEST_APIS(__prefix_name)                                                                                \
    __VSF_USART_API(__prefix_name, vsf_err_t,      init,         vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)                 \
    __VSF_USART_API(__prefix_name, fsm_rt_t,       enable,       vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, fsm_rt_t,       disable,      vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, void,           irq_enable,   vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)         \
    __VSF_USART_API(__prefix_name, void,           irq_disable,  vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)         \
    __VSF_USART_API(__prefix_name, usart_status_t, status,       vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, uint_fast16_t,  fifo_read,    vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)\
    __VSF_USART_API(__prefix_name, uint_fast16_t,  fifo_write,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)\
    __VSF_USART_API(__prefix_name, vsf_err_t,      request_rx,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)\
    __VSF_USART_API(__prefix_name, vsf_err_t,      request_tx,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)\
    __VSF_USART_API(__prefix_name, vsf_err_t,      cancel_rx,    vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, vsf_err_t,      cancel_tx,    vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, int_fast32_t,   get_rx_count, vsf_usart_t *usart_ptr)                                       \
    __VSF_USART_API(__prefix_name, int_fast32_t,   get_tx_count, vsf_usart_t *usart_ptr)

#define __VSF_USART_FULL_APIS(__prefix_name)                                                                                   \
    __VSF_USART_BASE_APIS(__prefix_name)                                                                                       \
    __VSF_USART_FIFO_APIS(__prefix_name)                                                                                       \
    __VSF_USART_REQUEST_APIS(__prefix_name)

/*============================ PROTOTYPES ====================================*/

#undef   __VSF_USART_API
#define  __VSF_USART_API(__prefix_name, __return, __name, ...)                   \
    VSF_TEMPLATE_HAL_API_EXTERN(__prefix_name, _usart_, __return, __name, __VA_ARGS__)


#if VSF_USART_CFG_IMPLEMENT_REQUEST_BY_FIFO == DISABLED
    __VSF_USART_FULL_APIS(VSF_USART_CFG_INSTANCE_PREFIX)
#else
    __VSF_USART_BASE_APIS(VSF_USART_CFG_INSTANCE_PREFIX)
    __VSF_USART_FIFO_APIS(VSF_USART_CFG_INSTANCE_PREFIX)
    __VSF_USART_FULL_APIS(VSF_USART_CFG_FIFO2REQ_PREFIX)
#endif

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_USART_CFG_MULTI_CLASS == ENABLED && VSF_USART_CFG_IMPLEMENT_OP == ENABLED

#   undef   __VSF_USART_API
#   define  __VSF_USART_API(__prefix_name, __return, __name, ...)                   \
        VSF_TEMPLATE_HAL_API_OP(__prefix_name, _usart_, __return, __name, __VA_ARGS__)


#if VSF_USART_CFG_IMPLEMENT_REQUEST_BY_FIFO == DISABLED
static const vsf_usart_op_t VSF_USART_OP_VAR = {
    __VSF_USART_FULL_APIS(VSF_USART_CFG_INSTANCE_PREFIX)
};
#else
static const vsf_usart_op_t VSF_USART_OP_VAR = {
    __VSF_USART_BASE_APIS(VSF_USART_CFG_INSTANCE_PREFIX)
    __VSF_USART_FIFO_APIS(VSF_USART_CFG_INSTANCE_PREFIX)
};
static const vsf_usart_op_t VSF_USART_FIFO2REQ_OP_VAR = {
    __VSF_USART_FULL_APIS(VSF_USART_CFG_FIFO2REQ_PREFIX)
};
#endif
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#ifdef VSF_USART_CFG_USART_COUNT
#   ifndef VSF_USART_CFG_USART_MASK
#       define VSF_USART_CFG_USART_MASK     ((1ul << VSF_USART_CFG_USART_COUNT) - 1)
#   endif

#   if VSF_USART_CFG_USART_MASK & (1 << 0)
        VSF_USART_CFG_USART_IMP_LV0(0, NULL)
#   endif
#   if VSF_USART_CFG_USART_MASK & (1 << 1)
        VSF_USART_CFG_USART_IMP_LV0(1, NULL)
#   endif
#   if VSF_USART_CFG_USART_MASK & (1 << 2)
        VSF_USART_CFG_USART_IMP_LV0(2, NULL)
#   endif
#   if VSF_USART_CFG_USART_MASK & (1 << 3)
        VSF_USART_CFG_USART_IMP_LV0(3, NULL)
#   endif
#   if VSF_USART_CFG_USART_MASK & (1 << 4)
        VSF_USART_CFG_USART_IMP_LV0(4, NULL)
#   endif
#   if VSF_USART_CFG_USART_MASK & (1 << 5)
        VSF_USART_CFG_USART_IMP_LV0(5, NULL)
#   endif
#   if VSF_USART_CFG_USART_MASK & (1 << 6)
        VSF_USART_CFG_USART_IMP_LV0(6, NULL)
#   endif
#   if VSF_USART_CFG_USART_MASK & (1 << 7)
        VSF_USART_CFG_USART_IMP_LV0(7, NULL)
#   endif

#   undef VSF_USART_CFG_USART_COUNT
#   undef VSF_USART_CFG_USART_MASK
#   undef VSF_USART_CFG_USART_IMP_LV0
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ IMPLEMENTATION ================================*/


#endif // VSF_HAL_USE_USART == ENABLED
