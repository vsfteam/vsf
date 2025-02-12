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

// for I2C IRQn
#include "RP2040.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_I2C_CFG_MULTI_CLASS
#   define VSF_HW_I2C_CFG_MULTI_CLASS                       VSF_I2C_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_i2c_t {
    implement(vsf_dw_apb_i2c_t)
    IRQn_Type irqn;
} vsf_hw_i2c_t;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_i2c_init(vsf_hw_i2c_t *hw_i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    vsf_err_t err = vsf_dw_apb_i2c_init(&hw_i2c_ptr->use_as__vsf_dw_apb_i2c_t, cfg_ptr, clock_get_hz(clk_sys));
    if ((VSF_ERR_NONE == err) && (cfg_ptr->isr.handler_fn != NULL)) {
        NVIC_SetPriority(hw_i2c_ptr->irqn, (uint32_t)cfg_ptr->isr.prio);
        NVIC_EnableIRQ(hw_i2c_ptr->irqn);
    }
    return err;
}

void vsf_hw_i2c_fini(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);

    NVIC_DisableIRQ(hw_i2c_ptr->irqn);
    vsf_dw_apb_i2c_fini(&hw_i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

fsm_rt_t vsf_hw_i2c_enable(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);

    return vsf_dw_apb_i2c_enable(&hw_i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

fsm_rt_t vsf_hw_i2c_disable(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    return vsf_dw_apb_i2c_disable(&hw_i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

void vsf_hw_i2c_irq_enable(vsf_hw_i2c_t *hw_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    vsf_dw_apb_i2c_irq_enable(&hw_i2c_ptr->use_as__vsf_dw_apb_i2c_t, irq_mask);
}

void vsf_hw_i2c_irq_disable(vsf_hw_i2c_t *hw_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);

    irq_mask = vsf_dw_apb_i2c_irq_disable(&hw_i2c_ptr->use_as__vsf_dw_apb_i2c_t, irq_mask);
    if (0 == irq_mask) {
        NVIC_DisableIRQ(hw_i2c_ptr->irqn);
    }
}

vsf_i2c_status_t vsf_hw_i2c_status(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    return vsf_dw_apb_i2c_status(&hw_i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

vsf_err_t vsf_hw_i2c_master_request(vsf_hw_i2c_t *hw_i2c_ptr, uint16_t address,
        vsf_i2c_cmd_t cmd, uint_fast16_t count, uint8_t *buffer)
{
    VSF_HAL_ASSERT(NULL != hw_i2c_ptr);
    VSF_HAL_ASSERT(0 != count);

    return vsf_dw_apb_i2c_master_request(&hw_i2c_ptr->use_as__vsf_dw_apb_i2c_t,
        address, cmd, count, buffer);
}

uint_fast16_t vsf_hw_i2c_get_transferred_count(vsf_hw_i2c_t *hw_i2c_ptr)
{
    VSF_HAL_ASSERT(hw_i2c_ptr != NULL);
    return vsf_dw_apb_i2c_get_transferred_count(&hw_i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

vsf_i2c_capability_t vsf_hw_i2c_capability(vsf_hw_i2c_t *hw_i2c_ptr)
{
    return vsf_dw_apb_i2c_capability(&hw_i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#define VSF_I2C_CFG_IMP_PREFIX                  vsf_hw
#define VSF_I2C_CFG_IMP_UPCASE_PREFIX           VSF_HW
#define VSF_I2C_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    vsf_hw_i2c_t vsf_hw_i2c ## __IDX = {                                        \
        .reg  = (vsf_dw_apb_i2c_reg_t *)VSF_HW_I2C ## __IDX ## _REG,            \
        .irqn = VSF_HW_I2C ## __IDX ## _IRQN,                                   \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_HW_I2C ## __IDX ## _IRQHandler(void)                               \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        vsf_dw_apb_i2c_irqhandler(&vsf_hw_i2c ## __IDX .use_as__vsf_dw_apb_i2c_t);\
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/i2c/i2c_template.inc"

#endif /* VSF_HAL_USE_I2C */
