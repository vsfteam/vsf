/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                  *
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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SPI == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${SPI_IP}_SPI_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${SPI_IP}_SPI_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${SPI_IP}_SPI_CLASS_IMPLEMENT
// IPCore end
// HW using ${SPI_IP} IPCore driver
#define __VSF_HAL_${SPI_IP}_SPI_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

/*\note VSF_HW_SPI_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${SPI_IP}_SPI_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_SPI_CFG_MULTI_CLASS
#   define VSF_HW_SPI_CFG_MULTI_CLASS           VSF_SPI_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_SPI_CFG_IMP_PREFIX                  vsf_hw
#define VSF_SPI_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end
// IPCore
#define VSF_SPI_CFG_IMP_PREFIX                  vsf_${spi_ip}
#define VSF_SPI_CFG_IMP_UPCASE_PREFIX           VSF_${SPI_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) {
#if VSF_HW_SPI_CFG_MULTI_CLASS == ENABLED
    vsf_spi_t               vsf_spi;
#endif
    void                    *reg;
    vsf_spi_isr_t           isr;
} VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t);
// HW end


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw spi only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_init)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != spi_ptr) && (NULL != cfg_ptr));
    // configure according to cfg_ptr
    spi_ptr->isr = cfg_ptr->isr;
    // configure interrupt according to cfg_ptr->isr
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_fini)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
}

fsm_rt_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_enable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_disable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_SPI_IRQ_CFG_IMP_PREFIX, _spi_irq_enable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    sf_spi_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
}

void VSF_MCONNECT(VSF_SPI_IRQ_CFG_IMP_PREFIX, _spi_irq_disable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
}

vsf_spi_status_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_status)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return (vsf_spi_status_t) {
        .value = 0,
    }
}

void VSF_MCONNECT(VSF_SPI_CS_CFG_IMP_PREFIX, _spi_cs_active)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast8_t index
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
}

void VSF_MCONNECT(VSF_SPI_CS_CFG_IMP_PREFIX, _spi_cs_inactive)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast8_t index
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
}

void VSF_MCONNECT(VSF_SPI_FIFO_CFG_IMP_PREFIX, _spi_fifo_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    void *out_buffer_ptr,
    uint_fast32_t* out_offset_ptr,
    void *in_buffer_ptr,
    uint_fast32_t* in_offset_ptr,
    uint_fast32_t cnt
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
                               out_buffer_ptr, out_offset_ptr,
                               in_buffer_ptr, in_offset_ptr,
                               cnt);
}

vsf_err_t VSF_MCONNECT(VSF_SPI_REQUEST_CFG_IMP_PREFIX, _spi_request_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    void *out_buffer_ptr,
    void *in_buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CANCEL_CFG_IMP_PREFIX, _spi_cancel_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SPI_GET_TRANSFERRED_CFG_IMP_PREFIX, _spi_get_transferred_count)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast32_t *tx_count,
    uint_fast32_t *rx_count
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
}

vsf_spi_capability_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_capability)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return (vsf_spi_capability_t) {
        .irq_mask = VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX,
        .support_hardware_cs = 1,
        .support_software_cs = 1,
        .cs_count            = 1,
        .max_clock_hz        = 100 * 1000 * 1000,
        .min_clock_hz;       = 100 * 1000,
    };
}

static void VSF_MCONNECT(__, VSF_SPI_CFG_IMP_PREFIX, _spi_irqhandler)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(NULL != spi_ptr);

    vsf_spi_irq_mask_t irq_mask = GET_IRQ_MASK(spi_ptr);
    vsf_spi_isr_t *isr_ptr = &spi_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_spi_t *)spi_ptr, irq_mask);
    }
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw spi only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_SPI_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t)                                \
        VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi, __IDX) = {                   \
        .reg                = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,_REG_),\
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_SPI_CFG_IMP_PREFIX, _spi_irqhandler)(              \
            &VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/spi/spi_template.inc"
// HW end

#endif /* VSF_HAL_USE_SPI */
