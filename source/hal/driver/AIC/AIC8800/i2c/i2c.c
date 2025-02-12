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

#include "../driver.h"

#if VSF_HAL_USE_I2C == ENABLED

#include "hal/vsf_hal.h"
#include "../vendor/plf/aic8800/src/driver/sysctrl/reg_sysctrl.h"
#include "../vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"
#include "../vendor/plf/aic8800/src/driver/i2cm/reg_i2cm.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_I2C_CFG_MULTI_CLASS
#   define VSF_HW_I2C_CFG_MULTI_CLASS                       VSF_I2C_CFG_MULTI_CLASS
#endif

#ifndef VSF_HW_I2C_REQEUST_BUFFER_SIZE
#   define VSF_HW_I2C_REQEUST_BUFFER_SIZE                   3
#endif

#ifndef VSF_I2C_CFG_CAPABILITY_MAX_TRANSFER_SIZE
#   define VSF_I2C_CFG_CAPABILITY_MAX_TRANSFER_SIZE         16
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_i2c_const_t {
    AIC_I2CM_TypeDef *reg;

    IRQn_Type irqn;

    // TODO: move to pm
    uint32_t pclk;
} vsf_hw_i2c_const_t;

typedef struct vsf_hw_i2c_t {
#if VSF_HW_I2C_CFG_MULTI_CLASS == ENABLED
    vsf_i2c_t vsf_i2c;
#endif

    const vsf_hw_i2c_const_t *i2c_const;
    vsf_i2c_isr_t isr;

    vsf_i2c_cfg_t       cfg;
    vsf_i2c_irq_mask_t  irq_mask;

    struct {
        vsf_i2c_cmd_t cmd;
        uint16_t address;
        uint8_t buffer[VSF_HW_I2C_REQEUST_BUFFER_SIZE];
        uint8_t count;
    } write_request;

    struct {
        vsf_i2c_cmd_t cmd;
        uint8_t *buffer_ptr;
        uint16_t count;
        uint8_t is_pending;
    } read_request;

} vsf_hw_i2c_t;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void __i2c_master_request(vsf_hw_i2c_t *hw_i2c_ptr,
                                 uint16_t address,
                                 vsf_i2c_cmd_t cmd,
                                 uint16_t count,
                                 uint8_t *buffer);

/*============================ IMPLEMENTATION ================================*/

static void __vsf_hw_i2c_irq_handler_cb(vsf_hw_i2c_t *hw_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);

    vsf_i2c_irq_mask_t hw_irq_mask = irq_mask & hw_i2c_ptr->irq_mask;
    vsf_i2c_isr_t *isr_ptr = &hw_i2c_ptr->cfg.isr;
    if ((hw_irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_i2c_t *)hw_i2c_ptr, hw_irq_mask);
    }
}

static void __vsf_hw_i2c_irq_handler(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    const vsf_hw_i2c_const_t * hw_i2c_const = hw_i2c_ptr->i2c_const;
    VSF_HAL_ASSERT(NULL != hw_i2c_const);

    uint16_t count = hw_i2c_ptr->read_request.count;
    if (!hw_i2c_ptr->read_request.is_pending && count > 0) {
        uint8_t *buffer_ptr = hw_i2c_ptr->read_request.buffer_ptr;
        VSF_HAL_ASSERT(NULL != buffer_ptr);
        for (int i = 0; i < count; i++) {
            buffer_ptr[i] = (hw_i2c_const->reg->IR & 0xff);
        }
    }

    vsf_i2c_irq_mask_t irq_mask;
    if (0 != hw_i2c_const->reg->LR) {
        irq_mask = VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT;
    } else {
        irq_mask = VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;
    }

    hw_i2c_const->reg->SR &= ~SR_INT_BIT;
    hw_i2c_const->reg->CR = 0;
    hw_i2c_const->reg->RR = RR_RST_BIT;

    if (hw_i2c_ptr->read_request.is_pending) {
        hw_i2c_ptr->read_request.is_pending = false;
        if (irq_mask == VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
            __i2c_master_request(hw_i2c_ptr, hw_i2c_ptr->write_request.address,
                                             hw_i2c_ptr->read_request.cmd,
                                             hw_i2c_ptr->read_request.count,
                                             hw_i2c_ptr->read_request.buffer_ptr);
            return ;
        }
    }

    __vsf_hw_i2c_irq_handler_cb(hw_i2c_ptr, irq_mask);
}

vsf_err_t vsf_hw_i2c_init(vsf_hw_i2c_t *hw_i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    const vsf_hw_i2c_const_t * hw_i2c_const = hw_i2c_ptr->i2c_const;
    VSF_HAL_ASSERT(NULL != hw_i2c_const);

    hw_i2c_ptr->cfg = *cfg_ptr;

    uint32_t pclk, div, div0, div1;

    cpusysctrl_pclkme_set(CSC_PCLKME_I2CM_EN_BIT);

    pclk = sysctrl_clock_get(SYS_PCLK);
    div = pclk / 4 / hw_i2c_ptr->cfg.clock_hz;
    div0 = div & 0xFFFF;
    div1 = (div >> 16) & 0x3FF;
    hw_i2c_const->reg->DR[0] = DR_DIV0_0_FILED(div0) | DR_DIV0_1_FILED(div0);
    hw_i2c_const->reg->DR[1] = DR_DIV1_0_FILED(div1) | DR_DIV1_1_FIELD(div1);
    hw_i2c_const->reg->SCR   = 0x200;

    hw_i2c_ptr->isr = cfg_ptr->isr;
    NVIC_SetPriority(hw_i2c_const->irqn, (uint32_t)hw_i2c_ptr->isr.prio);
    NVIC_EnableIRQ(hw_i2c_const->irqn);

    return VSF_ERR_NONE;
}

void vsf_hw_i2c_fini(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    const vsf_hw_i2c_const_t * hw_i2c_const = hw_i2c_ptr->i2c_const;
    VSF_HAL_ASSERT(NULL != hw_i2c_const);

    NVIC_DisableIRQ(hw_i2c_const->irqn);
    cpusysctrl_pclkmd_set(CSC_PCLKME_I2CM_EN_BIT);
}

fsm_rt_t vsf_hw_i2c_enable(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_i2c_disable(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);

    return fsm_rt_cpl;
}

void vsf_hw_i2c_irq_enable(vsf_hw_i2c_t *hw_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    VSF_HAL_ASSERT((irq_mask & ~VSF_I2C_IRQ_ALL_BITS_MASK) == 0);

    hw_i2c_ptr->irq_mask |= irq_mask;
}

void vsf_hw_i2c_irq_disable(vsf_hw_i2c_t *hw_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    const vsf_hw_i2c_const_t * hw_i2c_const = hw_i2c_ptr->i2c_const;
    VSF_HAL_ASSERT(NULL != hw_i2c_const);
    VSF_HAL_ASSERT((irq_mask & ~VSF_I2C_IRQ_ALL_BITS_MASK) == 0);

    hw_i2c_ptr->irq_mask &= ~irq_mask;
    if (hw_i2c_ptr->irq_mask == 0) {
        NVIC_DisableIRQ(hw_i2c_const->irqn);
    }
}

vsf_i2c_status_t vsf_hw_i2c_status(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    const vsf_hw_i2c_const_t * hw_i2c_const = hw_i2c_ptr->i2c_const;

    bool is_busy = (hw_i2c_ptr->write_request.count != 0) ||
                   (hw_i2c_ptr->read_request.is_pending)  ||
                   (hw_i2c_const->reg->SR & SR_BUSY_BIT);

    vsf_i2c_status_t status = {
        .use_as__vsf_peripheral_status_t.is_busy = is_busy,
    };

    return status;
}

static void __i2c_master_request(vsf_hw_i2c_t *hw_i2c_ptr,
                                 uint16_t address,
                                 vsf_i2c_cmd_t cmd,
                                 uint16_t count,
                                 uint8_t *buffer)
{
    const vsf_hw_i2c_const_t * hw_i2c_const = hw_i2c_ptr->i2c_const;

    hw_i2c_const->reg->SR &= ~(0x01UL << 1);
    hw_i2c_const->reg->SAR = SAR_SLAVE_ADDR_FILED(address);

    uint32_t cr_value =  CR_INT_ENABLE_BIT | CR_OMO_ENABLE_BIT | CR_ENABLE_BIT
                       | CR_DE_TH_FILED(0x1) | CR_DF_TH_FILED(0x1)
                       | (cmd & __I2C_CMD_HW_MASK);

    if ((cmd & VSF_I2C_CMD_RW_MASK) == VSF_I2C_CMD_WRITE) {
        for (int i = 0; i < hw_i2c_ptr->write_request.count; i++) {
            hw_i2c_const->reg->OR = hw_i2c_ptr->write_request.buffer[i];
        }
        for (int i = 0; i < count; i++) {
            hw_i2c_const->reg->OR = buffer[i];
        }
        count += hw_i2c_ptr->write_request.count;
        hw_i2c_ptr->write_request.count = 0;
    } else {

    }
    hw_i2c_const->reg->LR = count;
    hw_i2c_const->reg->CR = cr_value;
}

vsf_err_t vsf_hw_i2c_master_request(vsf_hw_i2c_t *hw_i2c_ptr,
                                    uint16_t address,
                                    vsf_i2c_cmd_t cmd,
                                    uint_fast16_t count,
                                    uint8_t *buffer)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    const vsf_hw_i2c_const_t * hw_i2c_const = hw_i2c_ptr->i2c_const;
    VSF_HAL_ASSERT(NULL != hw_i2c_const);
    VSF_HAL_ASSERT(0 != count);

    if (count > VSF_I2C_CFG_CAPABILITY_MAX_TRANSFER_SIZE) {
        // aic8800 hardware does not support
        return VSF_ERR_NOT_SUPPORT;
    }

    // hardware only support:
    //  1. start + stop
    //  2. start + restart
    bool is_start = cmd & VSF_I2C_CMD_START;
    bool is_stop = cmd & VSF_I2C_CMD_STOP;
    bool is_restart = cmd & VSF_I2C_CMD_RESTART;
    bool is_read = (cmd & VSF_I2C_CMD_RW_MASK) == VSF_I2C_CMD_READ;

    if (hw_i2c_ptr->write_request.count == 0) {           // no unfinished request
        if (!is_start) {
            return VSF_ERR_NOT_SUPPORT;
        }

        if (is_stop || is_restart) {
            if (is_read) {
                hw_i2c_ptr->read_request.count = count;
                hw_i2c_ptr->read_request.buffer_ptr = buffer;
            }
            __i2c_master_request(hw_i2c_ptr, address, cmd, count, buffer);
            return VSF_ERR_NONE;
        }

        if (is_read) {
            return VSF_ERR_NOT_SUPPORT;
        }

        if (count > dimof(hw_i2c_ptr->write_request.buffer)) {
            return VSF_ERR_NOT_SUPPORT;
        }

        memcpy(hw_i2c_ptr->write_request.buffer, buffer, count);
        hw_i2c_ptr->write_request.cmd = cmd;
        hw_i2c_ptr->write_request.count = count;
        hw_i2c_ptr->write_request.address = address;

        __vsf_hw_i2c_irq_handler_cb(hw_i2c_ptr, VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);

        return VSF_ERR_NONE;
    } else {                                    // last request is pending
        if (!is_stop && !is_restart) {
            return VSF_ERR_NOT_SUPPORT;
        }

        if (is_read) {                          // write then read
            VSF_HAL_ASSERT(hw_i2c_ptr->write_request.address == address);
            VSF_HAL_ASSERT(!hw_i2c_ptr->read_request.is_pending);

            hw_i2c_ptr->read_request.is_pending = true;
            hw_i2c_ptr->read_request.cmd = cmd;
            hw_i2c_ptr->read_request.count = count;
            hw_i2c_ptr->read_request.buffer_ptr = buffer;
            __i2c_master_request(hw_i2c_ptr, address, hw_i2c_ptr->write_request.cmd | VSF_I2C_CMD_RESTART, 0, NULL);
            return VSF_ERR_NONE;
        } else {                                // write then write
            if (hw_i2c_ptr->write_request.count + count > VSF_I2C_CFG_CAPABILITY_MAX_TRANSFER_SIZE) {
                hw_i2c_ptr->write_request.count = 0;
                return VSF_ERR_NOT_SUPPORT;
            }

            __i2c_master_request(hw_i2c_ptr, address, cmd | VSF_I2C_CMD_START, count, buffer);
            return VSF_ERR_NONE;
        }
    }
}

uint_fast32_t vsf_hw_i2c_get_transferred_count(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(hw_i2c_ptr != NULL);

	// TODO
    VSF_HAL_ASSERT(0);
    return 0;
}

vsf_i2c_capability_t vsf_hw_i2c_capability(vsf_hw_i2c_t *i2c_ptr)
{
    vsf_i2c_capability_t i2c_capability = {
        .irq_mask = 1,
        .support_restart = 1,
        .support_no_start = 0,
        .support_no_stop = 1,
        .max_transfer_size = 0xFFFF,
    };
    return i2c_capability;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY  ENABLED
#define VSF_I2C_CFG_IMP_PREFIX                  vsf_hw
#define VSF_I2C_CFG_IMP_UPCASE_PREFIX           VSF_HW
#define VSF_I2C_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    static const vsf_hw_i2c_const_t __vsf_hw_i2c ## __IDX ## _const = {         \
        .reg  = VSF_HW_I2C ## __IDX ## _REG,                                    \
        .irqn = VSF_HW_I2C ## __IDX ## _IRQ_IDX,                                \
        .pclk = VSF_HW_I2C ## __IDX ## _PCLKME_EN_BIT,                          \
    };                                                                          \
    vsf_hw_i2c_t vsf_hw_i2c ## __IDX = {                                        \
        .i2c_const = &__vsf_hw_i2c ## __IDX ## _const,                          \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_HW_I2C ## __IDX ## _IRQ(void)                                      \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        __vsf_hw_i2c_irq_handler(&vsf_hw_i2c ## __IDX);                         \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/i2c/i2c_template.inc"

#endif /* VSF_HAL_USE_I2C */
