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

/*============================ MACROS ========================================*/
#if VSF_HAL_USE_SPI == ENABLED

#ifndef VSF_SPI_CFG_IMPLEMENT_OP
#   define VSF_SPI_CFG_IMPLEMENT_OP                   DISABLED
#endif

#ifndef VSF_SPI_CFG_INSTANCE_PREFIX
#   define VSF_SPI_CFG_INSTANCE_PREFIX                vsf_hw
#endif

#if VSF_SPI_CFG_MULTI_INSTANCES == DISABLED
#   define VSF_SPI_OP
#else
#   define ____VSF_SPI_OP(__p)                        .vsf_spi.op = & __ ## __p ## _spi_op,
#   define __VSF_SPI_OP(__p)                          ____VSF_SPI_OP(__p)
#   define VSF_SPI_OP                                 __VSF_SPI_OP(VSF_SPI_CFG_INSTANCE_PREFIX)

#   define ____VSF_SPI_OP_VAR(__p)                    __ ## __p ## _spi_op
#   define __VSF_SPI_OP_VAR(__p)                      ____VSF_SPI_OP_VAR(__p)
#   define VSF_SPI_OP_VAR                             __VSF_SPI_OP_VAR(VSF_SPI_CFG_INSTANCE_PREFIX)
#endif

#ifndef VSF_SPI_CFG_IMPLEMENT_REQUEST_BY_FIFO
#   define VSF_SPI_CFG_IMPLEMENT_REQUEST_BY_FIFO      DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_SPI_FN_INIT               VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_init)
#define VSF_SPI_FN_ENABLE             VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_enable)
#define VSF_SPI_FN_DISABLE            VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_disable)
#define VSF_SPI_FN_IRQ_ENABLE         VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_irq_enable)
#define VSF_SPI_FN_IRQ_DISABLE        VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_irq_disable)
#define VSF_SPI_FN_STATUS             VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_status)
#define VSF_SPI_FN_FIFO_READ          VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_fifo_read)
#define VSF_SPI_FN_FIFO_WRITE         VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_fifo_write)
#define VSF_SPI_FN_REQUEST_RX         VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_request_rx)
#define VSF_SPI_FN_REQUEST_TX         VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_request_tx)
#define VSF_SPI_FN_CANCEL_RX          VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_cancel_rx)
#define VSF_SPI_FN_CANCEL_TX          VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_cancel_tx)
#define VSF_SPI_FN_GET_RX_COUNT       VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_get_rx_count)
#define VSF_SPI_FN_GET_TX_COUNT       VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_get_tx_count)
#define VSF_SPI_FN_GET_DATA_LENGTH    VSF_MCONNECT(VSF_SPI_CFG_INSTANCE_PREFIX, _spi_get_data_length)

#define __VSF_SPI_APIS(__prefix_name)                                                                                   \
    __VSF_SPI_API(__prefix_name, vsf_err_t,    init,                 vsf_spi_t *spi_ptr, spi_cfg_t *cfg_ptr)            \
    __VSF_SPI_API(__prefix_name, fsm_rt_t,     enable,               vsf_spi_t *spi_ptr)                                \
    __VSF_SPI_API(__prefix_name, fsm_rt_t,     disable,              vsf_spi_t *spi_ptr)                                \
    __VSF_SPI_API(__prefix_name, void,         irq_enable,           vsf_spi_t *spi_ptr, em_spi_irq_mask_t irq_mask)    \
    __VSF_SPI_API(__prefix_name, void,         irq_disable,          vsf_spi_t *spi_ptr, em_spi_irq_mask_t irq_mask)    \
    __VSF_SPI_API(__prefix_name, spi_status_t, status,               vsf_spi_t *spi_ptr)                                \
    __VSF_SPI_API(__prefix_name, void,         cs_active,            vsf_spi_t *spi_ptr, uint_fast8_t index)            \
    __VSF_SPI_API(__prefix_name, void,         cs_inactive,          vsf_spi_t *spi_ptr, uint_fast8_t index)            \
    __VSF_SPI_API(__prefix_name, void,         fifo_transfer,        vsf_spi_t *spi_ptr, void *out_buffer_ptr,          \
                                                                     uint_fast32_t *out_count_ptr, void *in_buffer_ptr, \
                                                                     uint_fast32_t *in_count_ptr)                       \
    __VSF_SPI_API(__prefix_name, bool,         fifo_flush,           vsf_spi_t *spi_ptr)                                \
    __VSF_SPI_API(__prefix_name, vsf_err_t,    request_transfer,     vsf_spi_t *spi_ptr, void *out_buffer_ptr,          \
                                                                     void *in_buffer_ptr, uint_fast32_t count)          \
    __VSF_SPI_API(__prefix_name, vsf_err_t,    cancel_transfer,      vsf_spi_t *spi_ptr)                                \
    __VSF_SPI_API(__prefix_name, int_fast32_t, get_transfered_count, vsf_spi_t *spi_ptr)

/*============================ PROTOTYPES ====================================*/

#undef   __VSF_SPI_API
#define  __VSF_SPI_API(__prefix_name, __return, __name, ...)                   \
    VSF_TEMPLATE_HAL_API_EXTERN(__prefix_name, _spi_, __return, __name, __VA_ARGS__)

__VSF_SPI_APIS(VSF_SPI_CFG_INSTANCE_PREFIX)

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_SPI_CFG_MULTI_INSTANCES == ENABLED && VSF_SPI_CFG_IMPLEMENT_OP == ENABLED

#   undef   __VSF_SPI_API
#   define  __VSF_SPI_API(__prefix_name, __return, __name, ...)                   \
        VSF_TEMPLATE_HAL_API_OP(__prefix_name, _spi_, __return, __name, __VA_ARGS__)

static const vsf_spi_op_t VSF_SPI_OP_VAR = {
    __VSF_SPI_APIS(VSF_SPI_CFG_INSTANCE_PREFIX)
};

#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ IMPLEMENTATION ================================*/


#endif // VSF_HAL_USE_SPI == ENABLED
