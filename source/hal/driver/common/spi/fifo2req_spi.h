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

#ifndef __HAL_DRIVER_FIFO2REQ_SPI_H__
#define __HAL_DRIVER_FIFO2REQ_SPI_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if (VSF_HAL_USE_SPI == ENABLED) && (VSF_HAL_USE_FIFO2REQ_SPI == ENABLED)

#if defined(__VSF_HAL_USE_FIFO2REQ_SPI_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#ifndef VSF_FIFO2REQ_SPI_CFG_MULTI_CLASS
#   define VSF_FIFO2REQ_SPI_CFG_MULTI_CLASS     VSF_SPI_CFG_MULTI_CLASS
#endif

//#define VSF_SPI_FIFO2REQ_IMP_LV0(__PREFIX)                                                               \


/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_FIFO2REQ_SPI_CFG_MULTI_CLASS == ENABLED
#   define __describe_fifo2req_spi_op()         .op = &vsf_fifo2req_spi_op,
#else
#   define __describe_fifo2req_spi_op()
#endif

#define __describe_fifo2req_spi(__name, __spi)                                  \
    vsf_fifo2req_spi_t __name = {                                               \
        __describe_fifo2req_spi_op()                                            \
        .spi = __spi,                                                           \
    };

#define describe_fifo2req_spi(__name, __spi)                                    \
            __describe_fifo2req_spi(__name, __spi)

/*============================ TYPES =========================================*/

vsf_class(vsf_fifo2req_spi_t) {
    public_member(
#if VSF_FIFO2REQ_SPI_CFG_MULTI_CLASS == ENABLED
        implement(vsf_spi_t)
#endif
        vsf_spi_t * spi;
    )

    private_member(
        struct {
            void          * buffer_ptr;
            uint_fast32_t   cnt;
            uint_fast32_t   offset;
        } out, in;
        
        vsf_spi_isr_t isr;
        vsf_spi_irq_mask_t irq_mask;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/

#define VSF_SPI_CFG_DEC_PREFIX              vsf_fifo2req
#define VSF_SPI_CFG_DEC_UPCASE_PREFIX       VSF_FIFO2REQ
#define VSF_SPI_CFG_DEC_EXTERN_OP           ENABLED
#include "hal/driver/common/spi/spi_template.h"

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif

#endif
/* EOF */
