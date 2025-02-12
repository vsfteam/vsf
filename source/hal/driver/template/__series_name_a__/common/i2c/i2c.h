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

#ifndef __HAL_DRIVER_${SERIES/I2C_IP}_I2C_H__
#define __HAL_DRIVER_${SERIES/I2C_IP}_I2C_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_I2C == ENABLED

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
#if     defined(__VSF_HAL_${I2C_IP}_I2C_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${I2C_IP}_I2C_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${I2C_IP}_I2C_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_I2C_CFG_MULTI_CLASS should be in i2c.c.
 */

// IPCore
#ifndef VSF_${I2C_IP}_I2C_CFG_MULTI_CLASS
#   define VSF_${I2C_IP}_I2C_CFG_MULTI_CLASS    VSF_I2C_CFG_MULTI_CLASS
#endif
// IPCore end

// HW
/*\note hw I2C driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE for vsf_i2c_mode_t
 *          VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS for vsf_i2c_status_t
 *          VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_i2c_irq_mask_t
 *          VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD for vsf_i2c_cmd_t
 *          VSF_I2C_CFG_REIMPLEMENT_TYPE_CTRL for vsf_i2c_ctrl_t
 *          VSF_I2C_CFG_REIMPLEMENT_TYPE_CFG for vsf_i2c_cfg_t
 *          VSF_I2C_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_i2c_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD          ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_CTRL         ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_CFG          ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_CAPABILITY   ENABLED
// HW end

/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${i2c_ip}_i2c_t) {
#if VSF_${I2C_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_i2c_t               vsf_i2c;
    )
#endif

/*\note You can add more member in vsf_${i2c_ip}_i2c_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${i2c_ip}_iwc_reg_t *reg;
        vsf_i2c_isr_t           isr;
    )
};
// IPCore end

// HW/IPCore, not for emulated drivers
#if VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_i2c_mode_t {
    VSF_I2C_MODE_MASTER                         = (0x1ul << 28),
    VSF_I2C_MODE_SLAVE                          = (0x0ul << 28),

    VSF_I2C_SPEED_STANDARD_MODE                 = (0x0ul << 29),
    VSF_I2C_SPEED_FAST_MODE                     = (0x1ul << 29),
    VSF_I2C_SPEED_FAST_MODE_PLUS                = (0x2ul << 29),
    VSF_I2C_SPEED_HIGH_SPEED_MODE               = (0x3ul << 29),

    VSF_I2C_ADDR_7_BITS                         = (0x0ul << 31),
    VSF_I2C_ADDR_10_BITS                        = (0x1ul << 31),

    // more vendor specified irq_mask can be added here
} vsf_i2c_mode_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD == ENABLED
typedef enum vsf_i2c_cmd_t {
    VSF_I2C_CMD_WRITE                           = (0x00ul << 0),
    VSF_I2C_CMD_READ                            = (0x01ul << 0),

    VSF_I2C_CMD_START                           = (0x00ul << 28),
    VSF_I2C_CMD_NO_START                        = (0x01ul << 28),

    VSF_I2C_CMD_STOP                            = (0x00ul << 29),
    VSF_I2C_CMD_RESTART                         = (0x01ul << 30),
    VSF_I2C_CMD_NO_STOP_RESTART                 = (0x01ul << 30),

    VSF_I2C_CMD_7_BITS                          = (0x00ul << 31),
    VSF_I2C_CMD_10_BITS                         = (0x01ul << 31),

    // more vendor specified commands can be added here
} vsf_i2c_cmd_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_i2c_irq_mask_t {
    VSF_I2C_IRQ_MASK_MASTER_TX                      = (0x1ul << 0),
    VSF_I2C_IRQ_MASK_MASTER_RX                      = (0x1ul << 1),
    VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE       = (0x1ul << 2),
    VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST        = (0x1ul << 3),
    VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK            = (0x1ul << 4),
    VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT          = (0x1ul << 5),
    VSF_I2C_IRQ_MASK_MASTER_ERR                     = (0x1ul << 6),
    VSF_I2C_IRQ_MASK_MASTER_START_OR_RESTART_DETECT = (0x1ul << 7),
    VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT             = (0x1ul << 8),
    VSF_I2C_IRQ_MASK_SLAVE_START_OR_RESTART_DETECT  = (0x1ul << 9),
    VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT              = (0x1ul << 10),
    VSF_I2C_IRQ_MASK_SLAVE_TX                       = (0x1ul << 11),
    VSF_I2C_IRQ_MASK_SLAVE_RX                       = (0x1ul << 12),
    VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE        = (0x1ul << 13),
    VSF_I2C_IRQ_MASK_SLAVE_ERR                      = (0x1ul << 14),

    // more vendor specified irq_masks can be added here
} vsf_i2c_irq_mask_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS == ENABLED
/*\note It's not obligated to inherit from vsf_peripheral_status_t.
 *      If not, there MUST be a is_busy bit in vsf_i2c_status_t.
 */

typedef struct vsf_i2c_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy    : 1;
        };
        uint32_t value;
    };
} vsf_i2c_status_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_CTRL == ENABLED
typedef enum vsf_i2c_ctrl_t {
    __VSF_I2C_CTRL_DUMMP = 0,
} vsf_i2c_ctrl_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_i2c_t vsf_i2c_t;
typedef void vsf_i2c_isr_handler_t(void *target_ptr, vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask);
typedef struct vsf_i2c_isr_t {
    vsf_i2c_isr_handler_t *handler_fn;
    void *target_ptr;
    vsf_arch_prio_t prio;
} vsf_i2c_isr_t;
typedef struct vsf_i2c_cfg_t {
    vsf_i2c_mode_t mode;
    uint32_t clock_hz;
    vsf_i2c_isr_t isr;
    uint_fast16_t slave_addr;
} vsf_i2c_cfg_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_i2c_capability_t {
#if VSF_I2C_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_i2c_irq_mask_t irq_mask;
    uint8_t support_no_start        : 1;
    uint8_t support_no_stop : 1;
    uint8_t support_restart         : 1;
    uint_fast16_t max_transfer_size;
    uint_fast16_t min_transfer_size;

    // more vendor specified capability can be added here
} vsf_i2c_capability_t;
#endif
// HW/IPCore end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

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
#undef __VSF_HAL_${I2C_IP}_I2C_CLASS_IMPLEMENT
#undef __VSF_HAL_${I2C_IP}_I2C_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_I2C
#endif      // __HAL_DRIVER_${SERIES/I2C_IP}_I2C_H__
/* EOF */
