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
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "../driver.h"

#if VSF_HAL_USE_I2C == ENABLED

#include "hal/vsf_hal.h"

// for resets_hw
#include "hardware/structs/resets.h"
// for I2C IRQn
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_I2C_CFG_MULTI_CLASS
#   define VSF_HW_I2C_CFG_MULTI_CLASS                       VSF_I2C_CFG_MULTI_CLASS
#endif

#define VSF_I2C_CFG_IMP_PREFIX                              vsf_hw
#define VSF_I2C_CFG_IMP_UPCASE_PREFIX                       VSF_HW

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) {
    implement(vsf_dw_apb_i2c_t)
    IRQn_Type irqn;
    uint32_t  rst_bit;
} VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t);

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_init)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    vsf_i2c_cfg_t *cfg_ptr
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    uint32_t rst_bit = i2c_ptr->rst_bit;
    resets_hw->reset = resets_hw->reset | rst_bit;
    // spin-wait: wait for reset to assert
    while (resets_hw->reset_done & rst_bit);
    resets_hw->reset = resets_hw->reset & ~rst_bit;
    // spin-wait: wait for reset to de-assert
    while (!(resets_hw->reset_done & rst_bit));

    vsf_err_t err = vsf_dw_apb_i2c_init(&i2c_ptr->use_as__vsf_dw_apb_i2c_t, cfg_ptr, clock_get_hz(clk_sys));
    if (VSF_ERR_NONE == err) {
        if (cfg_ptr->isr.handler_fn != NULL) {
            NVIC_SetPriority(i2c_ptr->irqn, (uint32_t)cfg_ptr->isr.prio);
            NVIC_EnableIRQ(i2c_ptr->irqn);
        } else {
            NVIC_DisableIRQ(i2c_ptr->irqn);
        }
    }
    return err;
}

void VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_fini)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);

    NVIC_DisableIRQ(i2c_ptr->irqn);
    vsf_dw_apb_i2c_fini(&i2c_ptr->use_as__vsf_dw_apb_i2c_t);
    resets_hw->reset = resets_hw->reset | i2c_ptr->rst_bit;
}

fsm_rt_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_enable)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);

    return vsf_dw_apb_i2c_enable(&i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

fsm_rt_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_disable)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return vsf_dw_apb_i2c_disable(&i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

void VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_enable)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    vsf_i2c_irq_mask_t irq_mask
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    vsf_dw_apb_i2c_irq_enable(&i2c_ptr->use_as__vsf_dw_apb_i2c_t, irq_mask);
}

void VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_disable)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    vsf_i2c_irq_mask_t irq_mask
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    vsf_dw_apb_i2c_irq_disable(&i2c_ptr->use_as__vsf_dw_apb_i2c_t, irq_mask);
}

vsf_i2c_status_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_status)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return vsf_dw_apb_i2c_status(&i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

vsf_err_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_request)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    uint16_t address,
    vsf_i2c_cmd_t cmd,
    uint_fast16_t count,
    uint8_t *buffer
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(0 != count);

    return vsf_dw_apb_i2c_master_request(&i2c_ptr->use_as__vsf_dw_apb_i2c_t,
        address, cmd, count, buffer);
}

uint_fast16_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_get_transferred_count)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
)
{
    VSF_HAL_ASSERT(i2c_ptr != NULL);
    return vsf_dw_apb_i2c_master_get_transferred_count(&i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

// New API split from get_transferred_count (template updated)
uint_fast16_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_get_transferred_count)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
)
{
    return VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_get_transferred_count)(i2c_ptr);
}

// Delegate to DW_apb IPCore master_fifo_transfer
fsm_rt_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_fifo_transfer)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    uint16_t address,
    vsf_i2c_cmd_t cmd,
    uint_fast16_t count,
    uint8_t *buffer_ptr,
    vsf_i2c_cmd_t *cur_cmd_ptr,
    uint_fast16_t *offset_ptr
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(0 != count);

    return vsf_dw_apb_i2c_master_fifo_transfer(
        &i2c_ptr->use_as__vsf_dw_apb_i2c_t,
        address, cmd, count, buffer_ptr,
        cur_cmd_ptr, offset_ptr);
}

// Delegate to DW_apb IPCore slave mode
uint_fast16_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_slave_fifo_transfer)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    bool transmit_or_receive,
    uint_fast16_t count,
    uint8_t *buffer_ptr
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return vsf_dw_apb_i2c_slave_fifo_transfer(
        &i2c_ptr->use_as__vsf_dw_apb_i2c_t,
        transmit_or_receive, count, buffer_ptr);
}

vsf_err_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_slave_request)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    bool transmit_or_receive,
    uint_fast16_t count,
    uint8_t *buffer_ptr
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return vsf_dw_apb_i2c_slave_request(
        &i2c_ptr->use_as__vsf_dw_apb_i2c_t,
        transmit_or_receive, count, buffer_ptr);
}

uint_fast16_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_slave_get_transferred_count)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return vsf_dw_apb_i2c_slave_get_transferred_count(
        &i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

vsf_i2c_capability_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_capability)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
)
{
    return vsf_dw_apb_i2c_capability(&i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

static void VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_irqhandler)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    vsf_dw_apb_i2c_irqhandler(&i2c_ptr->use_as__vsf_dw_apb_i2c_t);
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_I2C_CFG_IRQ_MASK_CHECK_UNIQUE               VSF_HAL_CHECK_MODE_STRICT
#define VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY          ENABLED

#define VSF_I2C_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t)                                \
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c, __IDX) = {                       \
        .reg = (vsf_dw_apb_i2c_reg_t *)VSF_MCONNECT(                            \
            VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX, _REG),                  \
        .irqn =                                                                 \
            VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX, _IRQN),    \
        .rst_bit = VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX,     \
                                _RST_BIT),                                      \
        __HAL_OP};                                                              \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX, _I2C, __IDX,  \
                                   _IRQHandler)(void) {                         \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_irqhandler)(              \
            &VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c, __IDX));                \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/i2c/i2c_template.inc"

#endif /* VSF_HAL_USE_I2C */
/* EOF */
