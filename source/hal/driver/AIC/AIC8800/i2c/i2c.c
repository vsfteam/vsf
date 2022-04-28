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

#define VSF_I2C_CFG_PREFIX                  vsf_hw
#define VSF_I2C_CFG_UPPERCASE_PREFIX        VSF_HW

/*============================ INCLUDES ======================================*/

#include "./i2c.h"

#if VSF_HAL_USE_I2C == ENABLED

#include "../vendor/plf/aic8800/src/driver/sysctrl/reg_sysctrl.h"
#include "../vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"
#include "../vendor/plf/aic8800/src/driver/i2cm/reg_i2cm.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_i2c_const_t {
    AIC_I2CM_TypeDef *reg;

    IRQn_Type irqn;

    // TODO: move to pm
    uint32_t pclk;
} vsf_hw_i2c_const_t;

typedef struct vsf_hw_i2c_t {
#if VSF_I2C_CFG_IMPLEMENT_OP == ENABLED
    vsf_i2c_t vsf_i2c;
#endif

    const vsf_hw_i2c_const_t *i2c_const;
    vsf_i2c_isr_t isr;

    i2c_cfg_t           cfg;
    em_i2c_irq_mask_t   irq_mask;
    uint8_t             is_enabled :1;

    struct {
        uint16_t count;
        uint8_t *buffer;
    } rec;

} vsf_hw_i2c_t;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_hw_i2c_irq_handler(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    const vsf_hw_i2c_const_t * hw_i2c_const = hw_i2c_ptr->i2c_const;
    VSF_HAL_ASSERT(NULL != hw_i2c_const);

    uint16_t count = hw_i2c_ptr->rec.count;
    if (count > 0) {
        uint8_t *buffer = hw_i2c_ptr->rec.buffer;
        VSF_HAL_ASSERT(NULL != buffer);
        for (int i = 0; i < count; i++) {
            buffer[i] = (hw_i2c_const->reg->IR & 0xff);
        }
    }

    em_i2c_irq_mask_t irq_mask;
    if (0 != hw_i2c_const->reg->LR) {
        irq_mask = I2C_IRQ_MASK_MASTER_NACK_DETECT;
    } else {
        irq_mask = I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;
    }

    hw_i2c_const->reg->SR &= ~SR_INT_BIT;
    hw_i2c_const->reg->CR = 0;
    hw_i2c_const->reg->RR = RR_RST_BIT;

    irq_mask &= hw_i2c_ptr->irq_mask;
    if (irq_mask && (NULL != hw_i2c_ptr->cfg.isr.handler_fn)) {
        hw_i2c_ptr->cfg.isr.handler_fn(hw_i2c_ptr->cfg.isr.target_ptr,
                                       (vsf_i2c_t *)hw_i2c_ptr,
                                       irq_mask);
    }
}

vsf_err_t vsf_hw_i2c_init(vsf_hw_i2c_t *hw_i2c_ptr, i2c_cfg_t *cfg_ptr)
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
    // always enable interrupt to support more than 16 bytes request
    NVIC_SetPriority(hw_i2c_const->irqn, (uint32_t)hw_i2c_ptr->isr.prio);
    NVIC_EnableIRQ(hw_i2c_const->irqn);

    return VSF_ERR_NONE;
}

void vsf_hw_i2c_fini(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);

    cpusysctrl_pclkmd_set(CSC_PCLKME_I2CM_EN_BIT);
}

fsm_rt_t vsf_hw_i2c_enable(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);

    hw_i2c_ptr->is_enabled = true;

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_i2c_disable(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);

    hw_i2c_ptr->is_enabled = false;

    return fsm_rt_cpl;
}

void vsf_hw_i2c_irq_enable(vsf_hw_i2c_t *hw_i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    VSF_HAL_ASSERT((irq_mask & ~I2C_IRQ_MASK_MASTER_ALL) == 0);

    hw_i2c_ptr->irq_mask |= irq_mask;
}

void vsf_hw_i2c_irq_disable(vsf_hw_i2c_t *hw_i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    const vsf_hw_i2c_const_t * hw_i2c_const = hw_i2c_ptr->i2c_const;
    VSF_HAL_ASSERT(NULL != hw_i2c_const);
    VSF_HAL_ASSERT((irq_mask & ~I2C_IRQ_MASK_MASTER_ALL) == 0);

    hw_i2c_ptr->irq_mask &= ~irq_mask;
    if (hw_i2c_ptr->irq_mask == 0) {
        NVIC_DisableIRQ(hw_i2c_const->irqn);
    }
}

i2c_status_t vsf_hw_i2c_status(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);

    i2c_status_t status = {
        .use_as__peripheral_status_t.is_busy = false,
    };

    return status;
}

vsf_err_t vsf_hw_i2c_master_request(vsf_hw_i2c_t *hw_i2c_ptr,
                                    uint16_t address,
                                    em_i2c_cmd_t cmd,
                                    uint16_t count,
                                    uint8_t *buffer)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    const vsf_hw_i2c_const_t * hw_i2c_const = hw_i2c_ptr->i2c_const;
    VSF_HAL_ASSERT(NULL != hw_i2c_const);
    VSF_HAL_ASSERT(0 != count);

    VSF_HAL_ASSERT(hw_i2c_ptr->is_enabled);
    if ((count == 0) || (count > 16)) {         // aic8800 hardware does not support
        VSF_ASSERT(0);
        return VSF_ERR_NOT_SUPPORT;
    }

    // only support:
    //  1. start + stop
    //  2. start + restart
    bool is_start = (cmd & I2C_CMD_START) == I2C_CMD_START;
    bool is_stop = (cmd & I2C_CMD_STOP) == I2C_CMD_STOP;
    bool is_restart = (cmd & I2C_CMD_RESTAR) == I2C_CMD_RESTAR;
    if (!is_start) {
        VSF_ASSERT(0);
        return VSF_ERR_NOT_SUPPORT;
    }
    if (!is_stop && !is_restart) {
        VSF_ASSERT(0);
        return VSF_ERR_NOT_SUPPORT;
    }

    hw_i2c_const->reg->SR &= ~(0x01UL << 1);
    hw_i2c_const->reg->SAR = SAR_SLAVE_ADDR_FILED(address);

    uint32_t cr_value =  CR_INT_ENABLE_BIT | CR_OMO_ENABLE_BIT | CR_ENABLE_BIT
                       | CR_DE_TH_FILED(0x1) | CR_DF_TH_FILED(0x1)
                       | (cmd & __I2C_CMD_HW_MASK);

    if ((cmd & I2C_CMD_RW_MASK) == I2C_CMD_WRITE) {
        hw_i2c_ptr->rec.buffer = NULL;
        hw_i2c_ptr->rec.count = 0;
        for (int i = 0; i < count; i++) {
            hw_i2c_const->reg->OR = buffer[i];
        }
    } else {
        hw_i2c_ptr->rec.buffer = buffer;
        hw_i2c_ptr->rec.count = count;
    }
    hw_i2c_const->reg->LR = count;
    hw_i2c_const->reg->CR = cr_value;

    return VSF_ERR_NONE;
}


/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_I2C_CFG_IMP_LV0(__count, __hal_op)                                  \
    static const vsf_hw_i2c_const_t __vsf_hw_i2c ##__count ## _const = {        \
        .reg  = VSF_HW_I2C ## __count ## _REG,                                  \
        .irqn = VSF_HW_I2C ## __count ## _IRQ_IDX,                              \
        .pclk = VSF_HW_I2C ## __count ## _PCLKME_EN_BIT,                        \
    };                                                                          \
    vsf_hw_i2c_t vsf_hw_i2c ##__count = {                                       \
        .i2c_const = &__vsf_hw_i2c ##__count ## _const,                         \
        __hal_op                                                                \
    };                                                                          \
    void VSF_HW_I2C ## __count ## _IRQ(void)                                    \
    {                                                                           \
        __vsf_hw_i2c_irq_handler(&vsf_hw_i2c##__count);                         \
    }
#include "hal/driver/common/i2c/i2c_template.inc"

#endif /* VSF_HAL_USE_I2C */
