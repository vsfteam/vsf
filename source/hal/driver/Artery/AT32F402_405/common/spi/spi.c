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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SPI == ENABLED

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#include "hal/driver/vendor_driver.h"
// HW end

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

// When it's not a standard SPI device, you can rename the device prefix. At the same time, you need to update:
//  VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t)    -> VSF_MCONNECT(VSF_SPI_CFG_IMP_DEVICE_PREFIX, _t)
//  VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_init) -> VSF_MCONNECT(VSF_SPI_CFG_IMP_DEVICE_PREFIX, _init)
//  ....
//
//#define VSF_SPI_CFG_IMP_RENAME_DEVICE_PREFIX      ENABLED     // Enable renaming device prefix
//#define VSF_SPI_CFG_IMP_DEVICE_PREFIX             vsf_hw_qspi // Custom device prefix when renaming
//#define VSF_SPI_CFG_IMP_DEVICE_UPCASE_PREFIX      VSF_HW_QSPI // Uppercase version of custom device prefix

// HW end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) {
#if VSF_HW_SPI_CFG_MULTI_CLASS == ENABLED
    vsf_spi_t               vsf_spi;
#endif
    spi_type                *reg;
    vsf_spi_isr_t           isr;
    const vsf_hw_clk_t      *clk;
    vsf_i2c_irq_mask_t      irq_mask;
    vsf_hw_peripheral_en_t  en;
    vsf_hw_peripheral_rst_t rst;
    uint8_t                 irqn;
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

    vsf_hw_peripheral_enable(spi_ptr->en);

    spi_type *reg = spi_ptr->reg;
    reg->i2sctrl_bit.i2smsel = 0;

    uint32_t div = vsf_hw_clk_get_freq_hz(spi_ptr->clk) / cfg_ptr->clock_hz;
    div = vsf_max(div, 2);
    if (div > 1024) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_NOT_SUPPORT;
    } else if (3 == div) {
        reg->ctrl2_bit.mdiv3en = 1;
    } else {
        uint8_t msb = vsf_msb32(div) - 1;
        if (div & ((1 << msb) - 1)) {
            msb++;
        }
        reg->ctrl1_bit.mdiv_l = msb & 7;
        reg->ctrl2_bit.mdiv_h = msb >> 3;
    }

    reg->ctrl1 = (reg->ctrl1 & ~__VSF_HW_SPI_CTRL1_MASK) | ((cfg_ptr->mode >>  0) & 0xFFFF);
    reg->ctrl2 = (reg->ctrl2 & ~(__VSF_HW_SPI_CTRL2_MASK >> 16)) | ((cfg_ptr->mode >> 16) & 0xFFFF);

    vsf_spi_isr_t *isr_ptr = &cfg_ptr->isr;
    spi_ptr->isr = *isr_ptr;
    if (cfg_ptr->isr.handler_fn != NULL) {
        NVIC_SetPriority(spi_ptr->irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(spi_ptr->irqn);
    } else {
        NVIC_DisableIRQ(spi_ptr->irqn);
    }
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

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_enable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_type *reg = spi_ptr->reg;
    spi_ptr->irq_mask |= irq_mask;
    if (irq_mask & VSF_SPI_IRQ_MASK_RX) {
        reg->ctrl2_bit.rdbfie = 1;
    }
    if (irq_mask & VSF_SPI_IRQ_MASK_TX) {
        reg->ctrl2_bit.tdbeie = 1;
    }
    if (spi_ptr->irq_mask & (VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR | VSF_SPI_IRQ_MASK_CRC_ERR)) {
        reg->ctrl2_bit.errie = 1;
    }
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_disable)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_type *reg = spi_ptr->reg;
    spi_ptr->irq_mask &= ~irq_mask;
    if (irq_mask & VSF_SPI_IRQ_MASK_RX) {
        reg->ctrl2_bit.rdbfie = 0;
    }
    if (irq_mask & VSF_SPI_IRQ_MASK_TX) {
        reg->ctrl2_bit.tdbeie = 0;
    }
    if (!(spi_ptr->irq_mask & (VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR | VSF_SPI_IRQ_MASK_CRC_ERR))) {
        reg->ctrl2_bit.errie = 0;
    }
}

vsf_spi_status_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_status)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return (vsf_spi_status_t) {
        .is_busy = !!spi_ptr->reg->sts_bit.bf,
    };
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_active)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast8_t index
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(index == 0);

    spi_type *reg = spi_ptr->reg;
    if (!reg->ctrl1_bit.spien) {
        if (reg->ctrl1_bit.swcsen) {
            reg->ctrl1_bit.swcsil = 1;
        }
        reg->ctrl1_bit.spien = 1;
    }
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_inactive)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast8_t index
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(index == 0);

    spi_type *reg = spi_ptr->reg;
    if (reg->ctrl1_bit.spien) {
        reg->ctrl1_bit.spien = 0;
        if (reg->ctrl1_bit.swcsen) {
            reg->ctrl1_bit.swcsil = 0;
        }
    }
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_fifo_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    void *out_buffer_ptr,
    uint_fast32_t* out_offset_ptr,
    void *in_buffer_ptr,
    uint_fast32_t* in_offset_ptr,
    uint_fast32_t cnt
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    spi_type *reg = spi_ptr->reg;
    bool is_16bit = reg->ctrl1_bit.fbn;

    if (!reg->ctrl1_bit.spien) {
        VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_active)(spi_ptr, 0);
    }

    if (reg->sts_bit.rdbf) {
        uint32_t dt = reg->dt;
        if ((in_buffer_ptr != NULL) && (*in_offset_ptr < cnt)) {
            if (is_16bit) {
                ((uint16_t *)in_buffer_ptr)[(*in_offset_ptr)++] = dt;
            } else {
                ((uint8_t *)in_buffer_ptr)[(*in_offset_ptr)++] = dt;
            }
        }
    }
    if (!reg->sts_bit.bf) {
        if ((out_buffer_ptr != NULL)) {
            if (*out_offset_ptr < cnt) {
                if (is_16bit) {
                    reg->dt = ((uint16_t *)out_buffer_ptr)[(*out_offset_ptr)++];
                } else {
                    reg->dt = ((uint8_t *)out_buffer_ptr)[(*out_offset_ptr)++];
                }
            }
        } else /* if (in_buffer_ptr != NULL) */ {
            reg->dt = 0;
        }
    }

    if (    ((NULL == out_buffer_ptr) || (*out_offset_ptr == cnt))
        &&  ((NULL == in_buffer_ptr) || (*in_offset_ptr == cnt))) {
        VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_inactive)(spi_ptr, 0);
    }
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_request_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    void *out_buffer_ptr,
    void *in_buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cancel_transfer)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_get_transferred_count)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    uint_fast32_t *out_count,
    uint_fast32_t *in_count
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);
}

static void VSF_MCONNECT(__, VSF_SPI_CFG_IMP_PREFIX, _spi_irqhandler)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(NULL != spi_ptr);

    vsf_spi_irq_mask_t irq_mask = VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_clear)(spi_ptr, spi_ptr->irq_mask);
    vsf_spi_isr_t *isr_ptr = &spi_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_spi_t *)spi_ptr, irq_mask);
    }
}

/*\note Implementation of APIs below is optional, because there is default implementation in spi_template.inc.
 *      VSF_SPI_CFG_REIMPLEMENT_API_XXXX can be defined to ENABLED to re-write the default implementation for better performance.
 *
 *      The list of APIs and configuration:
 *      VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY for spi_capability.
 *          Default implementation will return hardware capability structure.
 *      VSF_SPI_CFG_REIMPLEMENT_API_GET_CONFIGURATION for spi_get_configuration.
 *          Default implementation will trigger assertion and return VSF_ERR_NOT_SUPPORT.
 *      VSF_SPI_CFG_REIMPLEMENT_API_CTRL for spi_ctrl.
 *          Default implementation will trigger assertion and return VSF_ERR_NOT_SUPPORT.
 *      VSF_SPI_CFG_REIMPLEMENT_API_IRQ_CLEAR for spi_irq_clear.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 */

vsf_spi_capability_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_capability)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    uint32_t clk_hz = vsf_hw_clk_get_freq_hz(spi_ptr->clk);
    return (vsf_spi_capability_t) {
        .irq_mask               = VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX
                                | VSF_SPI_IRQ_MASK_TX_CPL | VSF_SPI_IRQ_MASK_RX_CPL
                                | VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR
                                | VSF_SPI_IRQ_MASK_CRC_ERR,
        .support_hardware_cs    = 1,
        .support_software_cs    = 1,
        .cs_count               = 1,
        .max_clock_hz           = clk_hz / 2,
        .min_clock_hz           = clk_hz / 1024,
    };
}

vsf_err_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_ctrl)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_ctrl_t ctrl,
    void *param
) {
    VSF_HAL_ASSERT(spi_ptr != NULL);

    return VSF_ERR_NOT_SUPPORT;
}

vsf_spi_irq_mask_t VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_clear)(
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t) *spi_ptr,
    vsf_spi_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != spi_ptr);
    return 0;
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw spi only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY          ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_API_CTRL                ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_API_IRQ_CLEAR           ENABLED

#define VSF_SPI_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t)                                \
        VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi, __IDX) = {                   \
        .reg            = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,_REG),\
        .en             = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,_EN),\
        .rst            = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,_RST),\
        .clk            = &VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,_CLK),\
        .irqn           = VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX,_IRQN),\
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_MCONNECT(VSF_HW_INTERRUPT, VSF_MCONNECT(VSF_SPI_CFG_IMP_UPCASE_PREFIX, _SPI, __IDX, _IRQN))(void)\
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
