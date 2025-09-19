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

/*\note For IPCore drivers, define __VSF_HAL_${I2C_IP}_I2C_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${I2C_IP}_I2C_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${I2C_IP}_I2C_CLASS_IMPLEMENT
// IPCore end
// HW using ${I2C_IP} IPCore driver
#define __VSF_HAL_${I2C_IP}_I2C_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#define __VSF_HAL_SHOW_VENDOR_INFO__
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
// IPCore
#define VSF_I2C_CFG_IMP_PREFIX                  vsf_${i2c_ip}
#define VSF_I2C_CFG_IMP_UPCASE_PREFIX           VSF_${I2C_IP}
// IPCore end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) {
#if VSF_HW_I2C_CFG_MULTI_CLASS == ENABLED
    vsf_i2c_t               vsf_i2c;
#endif
    void                    *reg;
    vsf_i2c_isr_t           isr;
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

    // configure according to cfg_ptr
    i2c_ptr->isr = cfg_ptr->isr;
    // configure interrupt according to cfg_ptr->isr
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
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_disable)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_enable)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    vsf_i2c_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT((irq_mask & ~VSF_I2C_IRQ_ALL_BITS_MASK) == 0);
}

void VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_disable)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    vsf_i2c_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
}

vsf_i2c_status_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_status)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return (vsf_i2c_status_t) {
        .is_busy                = false,
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
    VSF_HAL_ASSERT(0 != count);
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_fifo_transfer)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr,
    uint16_t address,
    vsf_i2c_cmd_t cmd,
    uint_fast16_t count,
    uint8_t *buffer_ptr,
    vsf_i2c_cmd_t *cur_cmd_ptr,
    uint_fast16_t *offset_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(0 != count);
}

uint_fast16_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_get_transferred_count)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return 0;
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

vsf_i2c_capability_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_capability)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    return (vsf_i2c_capability_t) {
        .irq_mask                   = 0,
        .support_restart            = 0,
        .support_no_start           = 0,
        .support_no_stop    = 0,
        .max_transfer_size          = 0,
    };
}

vsf_err_t VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_ctrl)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) * i2c_ptr,
    vsf_i2c_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);

    return VSF_ERR_NONE;
}

static vsf_i2c_irq_mask_t VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_get_irq_mask)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    // implement this function in the device file
    VSF_HAL_ASSERT(0);
    return 0;
}

static void VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_irqhandler)(
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t) *i2c_ptr
) {
    VSF_HAL_ASSERT(NULL != i2c_ptr);

    vsf_i2c_irq_mask_t irq_mask = VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_get_irq_mask)(i2c_ptr);
    vsf_i2c_isr_t *isr_ptr = &i2c_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_i2c_t *)i2c_ptr, irq_mask);
    }
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw i2c only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY          ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_API_CTRL                ENABLED

#define VSF_I2C_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t)                                \
         VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c, __IDX) = {                  \
        .reg                = VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX,       \
                                           _I2C, __IDX, _REG),                  \
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_I2C_CFG_IMP_UPCASE_PREFIX,               \
                                   _I2C, __IDX, _IRQHandler)(void)              \
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
