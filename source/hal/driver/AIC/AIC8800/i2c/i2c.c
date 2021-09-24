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

#include "./i2c.h"
#if VSF_HAL_USE_I2C == ENABLED
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/sysctrl/reg_sysctrl.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_i2c_t vsf_i2c0 = {
    .REG_PARAM = (AIC_I2CM_TypeDef *)(0x40102000),
    .recall_info = {
        .data_offset = 0,
    },
};

void I2CM_IRQHandler(void)
{
    em_i2c_irq_mask_t irq_mask = 0;
    for (int i = 0; i < vsf_i2c0.data_length; i++) {
        vsf_i2c0.data[i] = (vsf_i2c0.REG_PARAM->IR & 0xff);
    }
    if (0 != vsf_i2c0.REG_PARAM->LR) {
        irq_mask |= I2C_IRQ_MASK_MASTER_ADDRESS_NACK;
    } else {
        irq_mask |= I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;
    }
    vsf_i2c0.REG_PARAM->SR &= ~SR_INT_BIT;
    vsf_i2c0.REG_PARAM->CR  = 0;
    vsf_i2c0.REG_PARAM->RR = RR_RST_BIT;
    if (16 >= vsf_i2c0.recall_info.data_size) {
        vsf_i2c0.recall_info.data_offset = 0;
        if (NULL != vsf_i2c0.cfg.isr.handler_fn) {
            vsf_i2c0.cfg.isr.handler_fn(vsf_i2c0.cfg.isr.target_ptr,
                                        &vsf_i2c0,
                                        irq_mask & vsf_i2c0.irq_mask);
        }
    } else {
        vsf_i2c_master_request(&vsf_i2c0,
                               vsf_i2c0.recall_info.address,
                               vsf_i2c0.recall_info.cmd,
                               vsf_i2c0.recall_info.data_size - 16,
                               vsf_i2c0.data + 16);
    }
}

static vsf_err_t __vsf_i2c_init(vsf_i2c_t *i2c_ptr, i2c_cfg_t *cfg_ptr)
{
    uint32_t pclk, div, div0, div1;

    cpusysctrl_pclkme_set(CSC_PCLKME_I2CM_EN_BIT);

    pclk = sysctrl_clock_get(SYS_PCLK);
    div = pclk / 4 / i2c_ptr->cfg.clock_hz;
    div0 = div & 0xFFFF;
    div1 = (div >> 16) & 0x3FF;
    i2c_ptr->REG_PARAM->DR[0] = DR_DIV0_0_FILED(div0) | DR_DIV0_1_FILED(div0);
    i2c_ptr->REG_PARAM->DR[1] = DR_DIV1_0_FILED(div1) | DR_DIV1_1_FIELD(div1);

    i2c_ptr->REG_PARAM->SCR   = 0x200;
    NVIC_SetVector(I2CM_IRQn, (uint32_t)I2CM_IRQHandler);
    NVIC_SetPriority(I2CM_IRQn, i2c_ptr->cfg.isr.prio);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_i2c_init(vsf_i2c_t *i2c_ptr, i2c_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != i2c_ptr) && (NULL != cfg_ptr));
    i2c_ptr->cfg = *cfg_ptr;
    return __vsf_i2c_init(i2c_ptr, cfg_ptr);
}

void vsf_i2c_fini(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    cpusysctrl_pclkmd_set(CSC_PCLKME_I2CM_EN_BIT);
}

fsm_rt_t vsf_i2c_enable(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    i2c_ptr->status.status_bool.is_enabled = true;
    return fsm_rt_cpl;
}

fsm_rt_t vsf_i2c_disable(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    i2c_ptr->status.status_bool.is_enabled = false;
    return fsm_rt_cpl;
}

void vsf_i2c_irq_enable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    if (irq_mask & I2C_IRQ_MASK_MASTER_ALL) {
        i2c_ptr->irq_mask = irq_mask & I2C_IRQ_MASK_MASTER_ALL;
        i2c_ptr->status.status_bool.irq_enabled = true;
        NVIC_EnableIRQ(I2CM_IRQn);
    }
}

void vsf_i2c_irq_disable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    i2c_ptr->irq_mask &= ~(irq_mask & I2C_IRQ_MASK_MASTER_ALL);
    if (I2C_IRQ_MASK_MASTER_ALL == (irq_mask & I2C_IRQ_MASK_MASTER_ALL)) {
        i2c_ptr->status.status_bool.irq_enabled = false;
        NVIC_DisableIRQ(I2CM_IRQn);
    }
}

i2c_status_t vsf_i2c_status(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return i2c_ptr->status;
}

vsf_err_t vsf_i2c_master_request(   vsf_i2c_t *i2c_ptr,
                                    uint16_t address,
                                    em_i2c_cmd_t cmd,
                                    uint16_t count,
                                    uint8_t *buffer_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(0 != count);//Chip hardware does not support
    if (!i2c_ptr->status.status_bool.is_enabled) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    if (    (!(cmd & I2C_CMD_START))
        ||      ((!(cmd & I2C_CMD_STOP))
            &&  (!(cmd & I2C_CMD_RESTAR)))) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    cmd &= ((~I2C_CMD_START) | (~I2C_CMD_STOP));

    i2c_ptr->data = buffer_ptr;
    i2c_ptr->REG_PARAM->SR &= ~(0x01UL << 1);
    i2c_ptr->REG_PARAM->SAR = SAR_SLAVE_ADDR_FILED(address);

    i2c_ptr->recall_info.data_size = count;
    i2c_ptr->recall_info.address = address;
    i2c_ptr->recall_info.cmd = cmd;
    i2c_ptr->recall_info.data_offset += 16;

    if (!(cmd & I2C_CMD_READ)) {
        for (int i = 0; i < count; i++) {
            i2c_ptr->REG_PARAM->OR = buffer_ptr[i];
        }
        i2c_ptr->data_length = 0;
    }
    if (16 < count) {//Chip hardware does not support
        cmd = cmd | I2C_CMD_RESTAR;
        i2c_ptr->REG_PARAM->LR  = 16;
        if (cmd & I2C_CMD_READ) {
            i2c_ptr->data_length = 16;
        }
    } else {
        i2c_ptr->REG_PARAM->LR  = count;
        if (cmd & I2C_CMD_READ) {
            i2c_ptr->data_length = count;
        }
    }

    i2c_ptr->REG_PARAM->CR  =       (i2c_ptr->status.status_bool.irq_enabled << 1)
                                |   CR_OMO_ENABLE_BIT
                                |   CR_START_BIT
                                |   CR_ENABLE_BIT
                                |   CR_DE_TH_FILED(0x1)
                                |   CR_DF_TH_FILED(0x1)
                                |   (cmd & I2C_ALL_CMD);
    return VSF_ERR_NONE;
}
#if VSF_HAL_I2C_IMP_MULTIPLEX_I2C == ENABLED
#   include "hal/driver/common/i2c/i2c_multiplex/__i2c_multiplex_common.inc"
#endif
#endif /* VSF_HAL_USE_I2C */