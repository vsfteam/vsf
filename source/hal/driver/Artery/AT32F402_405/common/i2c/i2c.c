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

#if VSF_HAL_USE_I2C == ENABLED

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_I2C_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${I2C_IP}_I2C_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_I2C_CFG_MULTI_CLASS
#   define VSF_HW_I2C_CFG_MULTI_CLASS           VSF_I2C_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_I2C_CFG_IMP_PREFIX                  vsf_hw
#define VSF_I2C_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) {
#if VSF_HW_I2C_CFG_MULTI_CLASS == ENABLED
    vsf_i2c_t               vsf_i2c;
#endif
    i2c_type                *reg;
    vsf_i2c_isr_t           isr;
    const vsf_hw_clk_t      *clk;
    vsf_i2c_irq_mask_t      irq_mask;
    vsf_hw_peripheral_en_t  en;
    vsf_hw_peripheral_rst_t rst;
    uint8_t                 *cur_buffer;
    vsf_i2c_cmd_t           cmd;
    vsf_i2c_cmd_t           cur_cmd;
    uint16_t                count;
    uint16_t                cur_count;
    uint8_t                 event_irqn;
    uint8_t                 error_irqn;
    uint8_t                 virtual_irq_mask;
} VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t);
// HW end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw i2c only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_init)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    vsf_i2c_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    i2c_type *reg = i2c_ptr->reg;
    vsf_i2c_mode_t mode = cfg_ptr->mode;

    vsf_hw_peripheral_enable(i2c_ptr->en);

    uint32_t cycles_per_clk = vsf_hw_clk_get_freq_hz(i2c_ptr->clk) / cfg_ptr->clock_hz;
    uint32_t low_cycles, high_cycles;
    cycles_per_clk -= 2 * (0x0F + 3);   // use 0x0F as I2C_CTRL1.DFLT
    if ((mode & VSF_I2C_SPEED_MASK) == VSF_I2C_SPEED_STANDARD_MODE) {
        // Tlow:Thigh = 1:1
        low_cycles = high_cycles = cycles_per_clk / 2;
    } else {
        // Tlow:Thigh = 16:9
        cycles_per_clk /= 25;
        low_cycles = cycles_per_clk * 16;
        high_cycles = cycles_per_clk * 9;
    }

    uint32_t div = low_cycles >> 8;
    if (low_cycles & 0xFF) {
        div++;
    }
    if (div > 0x100) {
        // i2c clock is too low to support
        VSF_HAL_ASSERT(false);
        return VSF_ERR_NOT_SUPPORT;
    }
    low_cycles /= div;
    high_cycles /= div;
    div--;

    uint32_t scld = (low_cycles + 1) / 2;
    scld = vsf_min(scld, 16) - 1;
    uint32_t enabled = reg->ctrl1_bit.i2cen;

    if (enabled) {
        reg->ctrl1_bit.i2cen = 0;
    }
    reg->ctrl1 = 0xF << 8;      // I2C_CTRL1.DFLT = 0x0F, I2C_CTRL1.STRETCH = 0
    reg->clkctrl =  ((div & 0x0F) << 28) | ((div & 0xF0) << 20)     // div
                |   (scld << 20) | (0 << 16)                        // scld | sdad(0)
                |   ((high_cycles - 1) << 8) | (low_cycles - 1);    // sclh | scll
    if ((cfg_ptr->mode & VSF_I2C_MODE_MASK) == VSF_I2C_MODE_SLAVE) {
        reg->oaddr1 =   cfg_ptr->slave_addr
                    |   (((cfg_ptr->mode & VSF_I2C_ADDR_MASK) == VSF_I2C_ADDR_10_BITS) ? 1 << 10 : 0)
                    |   (1 << 15);
    }
    if (enabled) {
        reg->ctrl1_bit.i2cen = 1;
    }

    vsf_i2c_isr_t *isr_ptr = &cfg_ptr->isr;
    i2c_ptr->isr = *isr_ptr;
    if (cfg_ptr->isr.handler_fn != NULL) {
        NVIC_SetPriority(i2c_ptr->event_irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(i2c_ptr->event_irqn);
        NVIC_SetPriority(i2c_ptr->error_irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(i2c_ptr->error_irqn);
    } else {
        NVIC_DisableIRQ(i2c_ptr->event_irqn);
        NVIC_DisableIRQ(i2c_ptr->error_irqn);
    }
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_fini)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
}

fsm_rt_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_enable)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    i2c_ptr->reg->ctrl1_bit.i2cen = 1;
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_disable)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    i2c_ptr->reg->ctrl1_bit.i2cen = 0;
    return fsm_rt_cpl;
}

static void VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_update)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    vsf_i2c_irq_mask_t irq_mask = i2c_ptr->irq_mask;
    vsf_i2c_irq_mask_t virtual_mask = irq_mask & __VSF_HW_I2C_VIRTUAL_MASK;
    irq_mask &= ~__VSF_HW_I2C_VIRTUAL_MASK;

    if (virtual_mask & (VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK | VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT)) {
        irq_mask |= 1 << 4;     // ACKFAIL
    }
    i2c_ptr->reg->ctrl1 = (i2c_ptr->reg->ctrl1 & ~__VSF_HW_I2C_REAL_MASK) | irq_mask;
}

void VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_enable)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    vsf_i2c_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT((irq_mask & ~VSF_I2C_IRQ_ALL_BITS_MASK) == 0);

    i2c_ptr->irq_mask |= irq_mask;
    VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_update)(i2c_ptr);
}

void VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_disable)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    vsf_i2c_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT((irq_mask & ~VSF_I2C_IRQ_ALL_BITS_MASK) == 0);

    i2c_ptr->irq_mask &= ~irq_mask;
    VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_update)(i2c_ptr);
}

vsf_i2c_status_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_status)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return (vsf_i2c_status_t) {
        .is_busy                = !!i2c_ptr->reg->sts_bit.busyf,
    };
}

vsf_err_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_request)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    uint16_t address,
    vsf_i2c_cmd_t cmd,
    uint_fast16_t count,
    uint8_t *buffer_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(cmd != 0);

    i2c_type *reg = i2c_ptr->reg;
    i2c_ptr->cmd = cmd;
    i2c_ptr->count = i2c_ptr->cur_count = count;
    i2c_ptr->cur_buffer = buffer_ptr;
    bool is_read = (cmd & VSF_I2C_CMD_RW_MASK) == VSF_I2C_CMD_READ;
    bool need_stop = !!(cmd & VSF_I2C_CMD_STOP);
    bool set_stop_now = need_stop && !(cmd & VSF_I2C_CMD_START) && !count;

    i2c_ptr->cur_cmd = cmd & ~VSF_I2C_CMD_BITS_MASK;
    if (need_stop) {
        i2c_ptr->cur_cmd &= ~VSF_I2C_CMD_STOP;
    }
    if (!count) {
        i2c_ptr->cur_cmd &= ~VSF_I2C_CMD_RW_MASK;
    }
    bool rlden = !!(count > 255);

    if (need_stop) {
        reg->ctrl1_bit.stopien = 1;
    }
    // TODO: use DMA
    if (count && i2c_ptr->isr.handler_fn != NULL) {
        if (is_read) {
            reg->ctrl1_bit.rdien = 1;
        } else {
            reg->ctrl1_bit.tdien = 1;
        }
    }

    reg->ctrl2 =    address & 0x3FF                                                         // SADDR
                |   (is_read ? 1 << 10 : 0)                                                 // DIR
                |   (((cmd & VSF_I2C_CMD_BITS_MASK) == VSF_I2C_CMD_10_BITS) ? 1 << 11 : 0)  // ADDR10
                |   ((cmd & VSF_I2C_CMD_START) ? 1 << 13 : 0)                               // GENSTART
                |   (set_stop_now ? 1 << 14 : 0)                                            // GENSTOP
                |   (!set_stop_now && need_stop ? 1 << 25: 0)                               // ASTOPEN
                |   ((rlden ? 255 : count) << 16)                                           // CNT
                |   (rlden << 24);                                                          // RLDEN

    return VSF_ERR_NONE;
}

fsm_rt_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_fifo_transfer)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    uint16_t address,
    vsf_i2c_cmd_t cmd,
    uint_fast16_t count,
    uint8_t *buffer_ptr,
    vsf_i2c_cmd_t *cur_cmd_ptr,
    uint_fast16_t *offset_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);

    return fsm_rt_err;
}

uint_fast16_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_get_transferred_count)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return i2c_ptr->count - i2c_ptr->cur_count;
}

vsf_err_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_slave_request)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    bool transmit_or_receive,
    uint_fast16_t count,
    uint8_t *buffer_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(0 != count);
    return VSF_ERR_NONE;
}

uint_fast16_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_slave_fifo_transfer)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    bool transmit_or_receive,
    uint_fast16_t count,
    uint8_t *buffer_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(0 != count);
    return 0;
}

uint_fast16_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_slave_get_transferred_count)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return 0;
}

static void VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_irqhandler)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);

    vsf_i2c_irq_mask_t irq_mask = VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_clear)(i2c_ptr, i2c_ptr->irq_mask);
    vsf_i2c_isr_t *isr_ptr = &i2c_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_i2c_t *)i2c_ptr, irq_mask);
    }
}

/*\note Implementation of APIs below is optional, because there is default implementation in i2c_template.inc.
 *      VSF_I2C_CFG_REIMPLEMENT_API_XXXX can be defined to ENABLED to re-write the default implementation for better performance.
 *
 *      The list of APIs and configuration:
 *      VSF_I2C_CFG_REIMPLEMENT_API_GET_CONFIGURATION for i2c_get_configuration.
 *          Default implementation will trigger assertion and return VSF_ERR_NOT_SUPPORT.
 *      VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY for i2c_capability.
 *          Default implementation will return capability structure using VSF_I2C_CFG_CAPABILITY_xxx configuration macros.
 *      VSF_I2C_CFG_REIMPLEMENT_API_CTRL for i2c_ctrl.
 *          Default implementation will return VSF_ERR_NOT_SUPPORT.
 *      VSF_I2C_CFG_REIMPLEMENT_API_IRQ_CLEAR for i2c_irq_clear.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 */

vsf_i2c_capability_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_capability)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    return (vsf_i2c_capability_t) {
        .irq_mask                   = VSF_I2C_IRQ_ALL_BITS_MASK,
        .support_restart            = 1,
        .support_no_start           = 1,
        .support_no_stop            = 1,
        .max_transfer_size          = 0,
        .min_transfer_size          = 0,
    };
}

vsf_err_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_ctrl)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) * i2c_ptr,
    vsf_i2c_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);

    return VSF_ERR_NONE;
}

vsf_i2c_irq_mask_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_clear)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    vsf_i2c_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);

    i2c_type *reg = i2c_ptr->reg;
    vsf_i2c_irq_mask_t irq_mask_orig = reg->sts;
    vsf_i2c_irq_mask_t virtual_irq_mask = i2c_ptr->virtual_irq_mask << 24;

    reg->clr = irq_mask_orig & 0x3F38 & irq_mask;
    if (i2c_ptr->cmd != 0) {
        // request mode
        vsf_i2c_cmd_t cur_cmd = i2c_ptr->cur_cmd;
        if (irq_mask_orig & (1 << 4)) {             // ACKFAIL
            if (cur_cmd & VSF_I2C_CMD_START) {
                virtual_irq_mask |= VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK;
            } else {
                virtual_irq_mask |= VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT;
            }
            if (irq_mask & (VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK | VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT)) {
                reg->clr = (1 << 4);
            }
        } else if (irq_mask_orig & (1 << 1)) {      // TDIS
            reg->clr |= 1 << 1;
            reg->txdt = *i2c_ptr->cur_buffer++;
            i2c_ptr->cur_count--;
        }
        if (irq_mask_orig & (1 << 2)) {             // RDBF
            reg->clr |= 1 << 2;
            *i2c_ptr->cur_buffer++ = reg->rxdt;
            i2c_ptr->cur_count--;
        }
        if (irq_mask_orig & (1 << 6)) {             // TDC
            reg->clr |= 1 << 6;
            if (i2c_ptr->cur_count) {
                if (i2c_ptr->cur_count > 255) {
                    reg->ctrl2_bit.cnt = 255;
                } else {
                    reg->ctrl2_bit.rlden = 0;
                    reg->ctrl2_bit.cnt = i2c_ptr->cur_count;
                }
            } else {
                reg->ctrl1_bit.tdien = 0;
                reg->ctrl1_bit.rdien = 0;
                if (!(i2c_ptr->cmd & VSF_I2C_CMD_STOP)) {
                    virtual_irq_mask |= VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;
                }
            }
        }
        if (irq_mask_orig & (1 << 5)) {             // STOPF
            virtual_irq_mask |= VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;
            reg->clr = 1 << 5;
        }
        if (virtual_irq_mask & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
            i2c_ptr->cmd = 0;
        }
    } else {
        // fifo mode
    }

    i2c_ptr->virtual_irq_mask = (virtual_irq_mask & ~irq_mask) >> 24;
    return (irq_mask_orig | virtual_irq_mask) & irq_mask;
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw i2c only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY          ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_API_CTRL                ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_API_IRQ_CLEAR           ENABLED

#define VSF_I2C_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t)                                \
         VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c, __IDX) = {                  \
        .reg        = VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX, _REG),\
        .clk        = &VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX, _CLK),\
        .en         = VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX, _EN),\
        .rst        = VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX, _RST),\
        .event_irqn = VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX, _EVENT_IRQN),\
        .error_irqn = VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX, _ERROR_IRQN),\
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_MCONNECT(VSF_HW_INTERRUPT, VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX, _ERROR_IRQN))(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_irqhandler)(              \
            &VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }                                                                           \
    void VSF_MCONNECT(VSF_HW_INTERRUPT, VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX, _EVENT_IRQN))(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_irqhandler)(              \
            &VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/i2c/i2c_template.inc"
// HW end

#endif /* VSF_HAL_USE_I2C */
