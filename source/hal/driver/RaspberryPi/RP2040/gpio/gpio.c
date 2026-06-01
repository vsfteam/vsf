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

#include "./gpio.h"

#if VSF_HAL_USE_GPIO == ENABLED

#include "hal/vsf_hal.h"

#include "hal/driver/vendor_driver.h"   /* for NVIC helper prototypes */

/*============================ MACROS ========================================*/

#ifndef VSF_HW_GPIO_CFG_MULTI_CLASS
#   define VSF_HW_GPIO_CFG_MULTI_CLASS              VSF_GPIO_CFG_MULTI_CLASS
#endif

#define VSF_GPIO_CFG_IMP_PREFIX                     vsf_hw
#define VSF_GPIO_CFG_IMP_UPCASE_PREFIX              VSF_HW

#define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY             ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_GET_PIN_CONFIGURATION  ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_READ_OUTPUT_REGISTER   ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET         ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR       ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_SET                    ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR                  ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT              ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT             ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION       ENABLED

#define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET      1
#define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR    1
#define VSF_GPIO_CFG_CAPABILITY_CAN_READ_IN_GPIO_OUTPUT_MODE 1

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) {
#if VSF_HW_GPIO_CFG_MULTI_CLASS == ENABLED
    vsf_gpio_t              vsf_gpio;
#endif
    /* IRQn for IO_BANK0 (parameterized via device.h, not hardcoded). */
    uint32_t                irqn;
    /* Track which pins were configured as OPEN_DRAIN so that gpio_write/set/
     * clear can emulate OD via SIO.gpio_oe (drive low / float for high).
     */
    vsf_gpio_pin_mask_t     open_drain_mask;

    /* EXTI: store the per-pin trigger bits and the user handler so the
     * IO_BANK0 ISR can dispatch. */
    vsf_gpio_exti_irq_cfg_t exti_cfg;
    /* Per-pin EXTI trigger bits, indexed by pin number. Each entry holds the
     * 4-bit mask (LEVEL_LOW=0, LEVEL_HIGH=1, EDGE_LOW=2, EDGE_HIGH=3) within
     * the IO_BANK0 INTR / PROC0_INTE registers. */
    uint8_t                 exti_trigger[VSF_HW_GPIO_PIN_COUNT];
} VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t);

/*============================ IMPLEMENTATION ================================*/

/*============================ IMPLEMENTATION ================================*/

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_port_config_pins)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT((pin_mask & ~VSF_HW_GPIO_PIN_MASK) == 0);

    vsf_gpio_mode_t base = cfg_ptr->mode & VSF_GPIO_MODE_MASK;

    /* FUNCSEL from bits [4:0]; overridden by alternate_function for AF. */
    uint32_t funcsel = cfg_ptr->mode & __VSF_HW_GPIO_FUNCSEL_MASK;
    if ((cfg_ptr->mode >> __VSF_HW_GPIO_IS_AF_POS) & 1) {
        funcsel = cfg_ptr->alternate_function & __VSF_HW_GPIO_FUNCSEL_MASK;
    }

    /* Bits [7:6] of base directly encode PADS IE/OD. */
    uint32_t pads = __RP2040_PADS_DEFAULT | (base & (__RP2040_PADS_IE | __RP2040_PADS_OD));
    uint32_t pull = (cfg_ptr->mode >> __VSF_HW_GPIO_PULL_POS) & __VSF_HW_GPIO_PULL_MASK;
    if (pull == 1) {
        pads |= __RP2040_PADS_PUE;
    } else if (pull == 2) {
        pads |= __RP2040_PADS_PDE;
    }

    bool is_output = (cfg_ptr->mode >> __VSF_HW_GPIO_IS_OUTPUT_POS) & 1;

    for (uint32_t i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        vsf_gpio_pin_mask_t bit = (vsf_gpio_pin_mask_t)1u << i;
        if (!(pin_mask & bit)) {
            continue;
        }
        pads_bank0_hw->io[i] = pads;
        io_bank0_hw->io[i].ctrl = funcsel;
    }

    /* Track open-drain pins for software emulation. */
    if (base == VSF_GPIO_OUTPUT_OPEN_DRAIN) {
        hw_gpio_ptr->open_drain_mask |= pin_mask;
    } else {
        hw_gpio_ptr->open_drain_mask &= ~pin_mask;
    }

    /* Track EXTI trigger bits per pin. */
    uint8_t trig = 0;
    if (base == VSF_GPIO_EXTI) {
        trig = (cfg_ptr->mode >> __VSF_HW_GPIO_EXTI_TRIG_POS) & __VSF_HW_GPIO_EXTI_TRIG_MASK;
    }
    for (uint32_t i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        if (pin_mask & ((vsf_gpio_pin_mask_t)1u << i)) {
            hw_gpio_ptr->exti_trigger[i] = trig;
        }
    }

    /* For SIO (FUNCSEL=__VSF_HW_GPIO_FUNCSEL_SIO), set OE per direction.
     * AF/ANALOG leave OE alone.
     */
    if (funcsel == __VSF_HW_GPIO_FUNCSEL_SIO) {
        if (is_output && base != VSF_GPIO_OUTPUT_OPEN_DRAIN) {
            sio_hw->gpio_oe_set = pin_mask;
        } else {
            sio_hw->gpio_oe_clr = pin_mask;
        }
    }

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_get_pin_configuration)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    uint16_t pin_index,
    vsf_gpio_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(pin_index < VSF_HW_GPIO_PIN_COUNT);

    uint32_t funcsel = io_bank0_hw->io[pin_index].ctrl & __VSF_HW_GPIO_FUNCSEL_MASK;
    uint32_t pads    = pads_bank0_hw->io[pin_index];
    vsf_gpio_pin_mask_t bit = (vsf_gpio_pin_mask_t)1u << pin_index;

    /* Re-derive mode from registers + driver-side open-drain tracking. */
    vsf_gpio_mode_t mode;
    if (funcsel == __VSF_HW_GPIO_FUNCSEL_NULL) {
        mode = VSF_GPIO_ANALOG;
    } else if (funcsel != __VSF_HW_GPIO_FUNCSEL_SIO) {
        mode = VSF_GPIO_AF;
    } else if (hw_gpio_ptr->open_drain_mask & bit) {
        mode = VSF_GPIO_OUTPUT_OPEN_DRAIN;
    } else if (sio_hw->gpio_oe & bit) {
        mode = VSF_GPIO_OUTPUT_PUSH_PULL;
    } else {
        mode = VSF_GPIO_INPUT;
    }

    if (pads & __RP2040_PADS_PUE) {
        mode |= VSF_GPIO_PULL_UP;
    } else if (pads & __RP2040_PADS_PDE) {
        mode |= VSF_GPIO_PULL_DOWN;
    } else {
        mode |= VSF_GPIO_NO_PULL_UP_DOWN;
    }

    cfg_ptr->mode               = mode;
    cfg_ptr->alternate_function = (funcsel == __VSF_HW_GPIO_FUNCSEL_SIO
                                   || funcsel == __VSF_HW_GPIO_FUNCSEL_NULL)
                                  ? 0 : funcsel;
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_set_direction)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_pin_mask_t direction_mask
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);

    vsf_gpio_pin_mask_t out_mask = pin_mask & direction_mask;
    vsf_gpio_pin_mask_t in_mask  = pin_mask & ~direction_mask;
    if (out_mask) {
        sio_hw->gpio_oe_set = out_mask;
    }
    if (in_mask) {
        sio_hw->gpio_oe_clr = in_mask;
    }
}

vsf_gpio_pin_mask_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_get_direction)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    return sio_hw->gpio_oe & pin_mask;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_set_input)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    sio_hw->gpio_oe_clr = pin_mask;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_set_output)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    sio_hw->gpio_oe_set = pin_mask;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_switch_direction)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    sio_hw->gpio_oe_togl = pin_mask;
}

vsf_gpio_pin_mask_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_read)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    return sio_hw->gpio_in;
}

vsf_gpio_pin_mask_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_read_output_register)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    return sio_hw->gpio_out;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_write)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_pin_mask_t value
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);

    /* Push-pull pins in pin_mask: drive via gpio_out atomic set/clr. */
    vsf_gpio_pin_mask_t pp_mask = pin_mask & ~hw_gpio_ptr->open_drain_mask;
    if (pp_mask) {
        vsf_gpio_pin_mask_t set_bits = pp_mask & value;
        vsf_gpio_pin_mask_t clr_bits = pp_mask & ~value;
        if (set_bits) sio_hw->gpio_set = set_bits;
        if (clr_bits) sio_hw->gpio_clr = clr_bits;
    }
    /* Open-drain pins: pre-program gpio_out=0, toggle OE to drive (low) or
     * float (high). gpio_out for OD pins stays at 0 across calls.
     */
    vsf_gpio_pin_mask_t od_mask = pin_mask & hw_gpio_ptr->open_drain_mask;
    if (od_mask) {
        sio_hw->gpio_clr = od_mask;
        vsf_gpio_pin_mask_t drive_low = od_mask & ~value;
        vsf_gpio_pin_mask_t release   = od_mask & value;
        if (drive_low) sio_hw->gpio_oe_set = drive_low;
        if (release)   sio_hw->gpio_oe_clr = release;
    }
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_set)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_write)(hw_gpio_ptr, pin_mask, pin_mask);
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_clear)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_write)(hw_gpio_ptr, pin_mask, 0);
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_toggle)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);

    vsf_gpio_pin_mask_t pp_mask = pin_mask & ~hw_gpio_ptr->open_drain_mask;
    if (pp_mask) {
        sio_hw->gpio_togl = pp_mask;
    }
    /* OD pins toggle direction (drive ↔ float) since gpio_out stays at 0. */
    vsf_gpio_pin_mask_t od_mask = pin_mask & hw_gpio_ptr->open_drain_mask;
    if (od_mask) {
        sio_hw->gpio_oe_togl = od_mask;
    }
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_output_and_set)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    /* SIO atomic: program gpio_out before enabling OE, so the pin transitions
     * from input (float) directly to output-high with no intermediate state.
     */
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    sio_hw->gpio_set    = pin_mask & ~hw_gpio_ptr->open_drain_mask;
    sio_hw->gpio_oe_set = pin_mask & ~hw_gpio_ptr->open_drain_mask;
    /* For OD pins, "set" means release (float), i.e. clear OE. */
    sio_hw->gpio_oe_clr = pin_mask & hw_gpio_ptr->open_drain_mask;
}

void VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_output_and_clear)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    sio_hw->gpio_clr    = pin_mask;
    sio_hw->gpio_oe_set = pin_mask;
}

vsf_gpio_capability_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_capability)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr
) {
    return (vsf_gpio_capability_t){
        .is_async                       = 0,
        .support_output_and_set         = 1,
        .support_output_and_clear       = 1,
        .support_interrupt              = 1,
        .can_read_in_gpio_output_mode   = 1,
        .can_read_in_alternate_mode     = 1,
        .pin_count                      = VSF_HW_GPIO_PIN_COUNT,
        .pin_mask                       = VSF_HW_GPIO_PIN_MASK,
    };
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_config)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_exti_irq_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    hw_gpio_ptr->exti_cfg = *cfg_ptr;
    NVIC_SetPriority(hw_gpio_ptr->irqn, (uint32_t)cfg_ptr->prio);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_get_configuration)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_exti_irq_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    *cfg_ptr = hw_gpio_ptr->exti_cfg;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_enable)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    for (uint32_t i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        if (!(pin_mask & ((vsf_gpio_pin_mask_t)1u << i))) { continue; }
        uint8_t trig = hw_gpio_ptr->exti_trigger[i];
        if (trig == 0) { continue; }
        uint32_t reg_index = i >> 3;
        uint32_t bit_shift = (i & 7) * 4;
        /* Clear any stale edge status before enabling. */
        io_bank0_hw->intr[reg_index] = (uint32_t)(trig & 0xC) << bit_shift;
        io_bank0_hw->proc0_irq_ctrl.inte[reg_index] |= (uint32_t)trig << bit_shift;
    }
    NVIC_EnableIRQ(hw_gpio_ptr->irqn);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_disable)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    for (uint32_t i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        if (!(pin_mask & ((vsf_gpio_pin_mask_t)1u << i))) { continue; }
        uint32_t reg_index = i >> 3;
        uint32_t bit_shift = (i & 7) * 4;
        io_bank0_hw->proc0_irq_ctrl.inte[reg_index] &= ~(0xFu << bit_shift);
    }
    /* Leave NVIC enabled — other pins may still need IRQs. */
    return VSF_ERR_NONE;
}

vsf_gpio_pin_mask_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_exti_irq_clear)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_pin_mask_t pin_mask
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    vsf_gpio_pin_mask_t pending = 0;
    for (uint32_t i = 0; i < VSF_HW_GPIO_PIN_COUNT; i++) {
        if (!(pin_mask & ((vsf_gpio_pin_mask_t)1u << i))) { continue; }
        uint32_t reg_index = i >> 3;
        uint32_t bit_shift = (i & 7) * 4;
        uint32_t pin_status = (io_bank0_hw->intr[reg_index] >> bit_shift) & 0xFu;
        if (pin_status) {
            pending |= (vsf_gpio_pin_mask_t)1u << i;
        }
        /* Only edge bits are write-1-to-clear; level bits auto-track. */
        io_bank0_hw->intr[reg_index] = (uint32_t)(pin_status & 0xC) << bit_shift;
    }
    return pending;
}

/* IO_BANK0 IRQ handler. Aggregates all 30 GPIO interrupts.
 * Name is fixed by RP2040 vector table — not parameterizable. */
void IO_BANK0_IRQHandler(void)  // quality: allow-hardcoded-irq
{
    uintptr_t ctx = vsf_hal_irq_enter();
    vsf_gpio_pin_mask_t fired = 0;
    /* Read all 4 INTS words; one bit per (pin, type). Reduce to pin mask
     * and clear edge bits via INTR (level bits auto-track). */
    for (uint32_t reg_index = 0; reg_index < 4; reg_index++) {
        uint32_t status = io_bank0_hw->proc0_irq_ctrl.ints[reg_index];
        if (!status) { continue; }
        /* Clear edge bits we observed (level bits self-clear when condition ends). */
        io_bank0_hw->intr[reg_index] = status & 0xCCCCCCCCu;
        for (uint32_t slot = 0; slot < 8; slot++) {
            if (status & (0xFu << (slot * 4))) {
                uint32_t pin = reg_index * 8 + slot;
                if (pin < VSF_HW_GPIO_PIN_COUNT) {
                    fired |= (vsf_gpio_pin_mask_t)1u << pin;
                }
            }
        }
    }
    if (fired && vsf_hw_gpio0.exti_cfg.handler_fn != NULL) {
        vsf_hw_gpio0.exti_cfg.handler_fn(vsf_hw_gpio0.exti_cfg.target_ptr,
                                         (vsf_gpio_t *)&vsf_hw_gpio0, fired);
    }
    vsf_hal_irq_leave(ctx);
}

vsf_err_t VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_ctrl)(
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t) *hw_gpio_ptr,
    vsf_gpio_ctrl_t ctrl,
    void *param
) {
    VSF_HAL_ASSERT(NULL != hw_gpio_ptr);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_REIMPLEMENT_API_CTRL                       ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_GET_CONFIGURATION ENABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_EXTI_IRQ_CLEAR             ENABLED

#define VSF_GPIO_CFG_IMP_LV0(__IDX, __HAL_OP)                                   \
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t)                              \
    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio, __IDX) = {                     \
        .irqn =                                                                 \
            VSF_MCONNECT(VSF_GPIO_CFG_IMP_UPCASE_PREFIX, _GPIO, __IDX, _IRQN),  \
        __HAL_OP};

#include "hal/driver/common/gpio/gpio_template.inc"

#endif      /* VSF_HAL_USE_GPIO */
