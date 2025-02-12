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

#ifndef __HAL_DRIVER_${SERIES/TIMER_IP}_TIMER_H__
#define __HAL_DRIVER_${SERIES/TIMER_IP}_TIMER_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_TIMER == ENABLED

// HW/IPCore
/**
 * \note When vsf_peripheral_status_t is inherited, vsf_template_hal_driver.h needs to be included
 */
#include "hal/driver/common/template/vsf_template_hal_driver.h"
// HW/IPCore end

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Includes CAN ONLY be put here. */
/*\note If current header is for a peripheral driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

// IPCore
#if     defined(__VSF_HAL_${TIMER_IP}_TIMER_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${TIMER_IP}_TIMER_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${TIMER_IP}_TIMER_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_TIMER_CFG_MULTI_CLASS should be in timer.c.
 */

// IPCore
#ifndef VSF_${TIMER_IP}_TIMER_CFG_MULTI_CLASS
#   define VSF_${TIMER_IP}_TIMER_CFG_MULTI_CLASS    VSF_TIMER_CFG_MULTI_CLASS
#endif
// IPCore end

// HW
/*\note hw TIMER driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE for vsf_timer_channel_mode_t
 *          VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_timer_irq_mask_t
 *          VSF_TIMER_CFG_REIMPLEMENT_TYPE_STATUS for vsf_timer_status_t
 *          VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG for vsf_timer_cfg_t
 *          VSF_TIMER_CFG_REIMPLEMENT_TYPE_CTRL for vsf_timer_ctrl_t
 *          VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG for vsf_timer_channel_cfg_t
 *          VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CTRL for vsf_timer_channel_ctrl_t
 *          VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_REQUEST for vsf_timer_channel_request_t
 *          VSF_TIMER_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_timer_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE     ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_STATUS           ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK         ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG              ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG      ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CTRL             ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG      ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CTRL     ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_REQUEST  ENABLED
#define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CAPABILITY       ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${timer_ip}_timer_t) {
#if VSF_${TIMER_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_timer_t               vsf_timer;
    )
#endif

/*\note You can add more member in vsf_${timer_ip}_timer_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${timer_ip}_timer_reg_t *reg;
        vsf_timer_isr_t           isr;
    )
};
// IPCore end

// HW/IPCore, not for emulated drivers
#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE == ENABLED
typedef enum vsf_timer_channel_mode_t {
    VSF_TIMER_CHANNEL_MODE_BASE = (0x00 << 0),
    VSF_TIMER_BASE_ONESHOT = (0x00 << 1),
    VSF_TIMER_BASE_CONTINUES = (0x01 << 1),

    // more vendor specified modes can be added here
} vsf_timer_channel_mode_t;
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_timer_irq_mask_t {
    VSF_TIMER_IRQ_MASK_OVERFLOW = (0x01 << 0),

    // more vendor specified irq_masks can be added here
} vsf_timer_irq_mask_t;
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef enum vsf_timer_ctrl_t {
    __VSF_TIMER_CTRL_DUMMY = 0,

    // more vendor specified ctrl can be added here
} vsf_timer_ctrl_t;
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CTRL == ENABLED
typedef enum vsf_timer_channel_ctrl_t {
    __VSF_TIMER_CHANNEL_CTRL_DUMMY = 0,

    // more vendor specified channel ctrl can be added here
} vsf_timer_channel_ctrl_t;
#endif

/** \note These types usually don't need to be reimplemented in hardware drivers.
 * They can be reimplemented when existing configuration types don't meet requirements.
 */
#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_timer_t vsf_timer_t;
typedef void vsf_timer_isr_handler_t(void *target_ptr,
                                     vsf_timer_t *timer_ptr,
                                     vsf_timer_irq_mask_t irq_mask);

typedef struct vsf_timer_isr_t {
    vsf_timer_isr_handler_t *handler_fn;
    void *target_ptr;
    vsf_arch_prio_t prio;
} vsf_timer_isr_t;
typedef struct vsf_timer_cfg_t {
    uint32_t period;
    union {
        uint32_t freq;
        uint32_t min_freq;
    };
    vsf_timer_isr_t isr;
} vsf_timer_cfg_t;
#endif

/** \note These types usually don't need to be reimplemented in hardware drivers.
 * They can be reimplemented when existing channel configuration types don't meet requirements.
 */
#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG == ENABLED
typedef struct vsf_timer_channel_cfg_t {
    vsf_timer_channel_mode_t mode;
    uint32_t                 pulse;

    // more vendor specified channel cfg can be added here
} vsf_timer_channel_cfg_t;
#endif

/** \note These types usually don't need to be reimplemented in hardware drivers.
 * They can be reimplemented when existing channel request types don't meet requirements.
 */
#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_REQUEST == ENABLED
typedef struct vsf_timer_channel_request_t {
    uint16_t length;
    union {
        uint32_t *period_buffer;
        uint32_t *pulse_buffer;
        uint32_t *input_capture_buffer;
        struct {
            uint32_t *channel_a_buffer;
            uint32_t *channel_b_buffer;
        };
    };

    // more vendor specified channel request can be added here
} vsf_timer_channel_request_t;
#endif

/** \note These types usually don't need to be reimplemented in hardware drivers.
 * They can be reimplemented when existing status types don't meet requirements.
 */
#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_STATUS == ENABLED
typedef struct vsf_timer_status_t {
    union {
        struct {
            uint32_t is_busy         : 1;
        };
        uint32_t value;
    };

    // more vendor specified status can be added here
} vsf_timer_status_t;
#endif

/** \note These types usually don't need to be reimplemented in hardware drivers.
 * They can be reimplemented when existing capability types don't meet requirements.
 */
#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_timer_capability_t {
    vsf_timer_irq_mask_t irq_mask;
    uint8_t timer_bitlen;
    uint8_t channel_cnt;
    uint8_t support_pwm : 1;
    uint8_t support_output_compare : 1;
    uint8_t support_input_capture : 1;
    uint8_t support_one_pulse : 1;

    // more vendor specified capability can be added here
} vsf_timer_capability_t;
#endif
// HW/IPCore end

/*============================ INCLUDES ======================================*/

// IPCore
/*\note Extern APIs for ip core diriver.
 *      There is no requirement about how APIs of IPCore drivers should be implemented.
 *      Just consider the simplicity for actual peripheral drivers.
 */
// IPCore end

#ifdef __cplusplus
}
#endif

// IPCore
#undef __VSF_HAL_${TIMER_IP}_TIMER_CLASS_IMPLEMENT
#undef __VSF_HAL_${TIMER_IP}_TIMER_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_TIMER
#endif      // __HAL_DRIVER_${SERIES/TIMER_IP}_TIMER_H__
/* EOF */
