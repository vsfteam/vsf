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

#ifndef __HAL_DRIVER_${DEVICE}_I2C_H__
#define __HAL_DRIVER_${DEVICE}_I2C_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_I2C == ENABLED

/*\note Incudes CAN ONLY be put here. */

/*\note I2C can be a driver to a actual device, or to a IPCore driver/emulated driver.
 *      If current driver is for a device, instance structure is not open to user.
 *      Otherwise instance structure should be open to user,
 *          in this case include the code between "IPCore" and "IPCore end" comments.
 */

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

/*\note hw I2C driver can reimplement vsf_i2c_mode_t/vsf_i2c_cmd_t/vsf_i2c_irq_mask_t/vsf_i2c_status_t.
 *      To enable reimplementation, please enable macro below:
 *          VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE for vsf_i2c_mode_t
 *          VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD for vsf_i2c_cmd_t
 *          VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS for vsf_i2c_status_t
 *          VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_i2c_irq_mask_t
 *      Reimplementation is used for optimization hw drivers, reimplement the bit mask according to hw registers.
 *      DO NOT reimplement these in non-hw drivers.
 */

#define VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE       ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD        ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS     ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK   ENABLED

/*============================ TYPES =========================================*/

typedef enum vsf_i2c_mode_t {
    VSF_I2C_MODE_MASTER                         = (0x1ul << 28),
    VSF_I2C_MODE_SLAVE                          = (0x0ul << 28),

    VSF_I2C_SPEED_STANDARD_MODE                 = (0x0ul << 29),
    VSF_I2C_SPEED_FAST_MODE                     = (0x1ul << 29),
    VSF_I2C_SPEED_FAST_MODE_PLUS                = (0x2ul << 29),
    VSF_I2C_SPEED_HIGH_SPEED_MODE               = (0x3ul << 29),

    VSF_I2C_ADDR_7_BITS                         = (0x0ul << 31),
    VSF_I2C_ADDR_10_BITS                        = (0x1ul << 31),
} vsf_i2c_mode_t;

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
} vsf_i2c_cmd_t;

typedef enum vsf_i2c_irq_mask_t {
    VSF_I2C_IRQ_MASK_MASTER_STARTED             = (0x1ul <<  0),
    VSF_I2C_IRQ_MASK_MASTER_STOPPED             = (0x1ul <<  1),
    VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT         = (0x1ul <<  2),    // for multi master
    VSF_I2C_IRQ_MASK_MASTER_NACK_DETECT         = (0x1ul <<  4),
    VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST    = (0x1ul <<  5),
    VSF_I2C_IRQ_MASK_MASTER_TX_EMPTY            = (0x1ul <<  6),
    VSF_I2C_IRQ_MASK_MASTER_ERROR               = (0x1ul <<  7),

    VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE   = (0x1ul <<  8),
    VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK        = (0x1ul <<  9),
} vsf_i2c_irq_mask_t;

/*\note It's not obligated to inherit from vsf_peripheral_status_t.
 *      If not, there MUST be a is_busy bit in vsf_i2c_status_t.
 */

typedef struct vsf_i2c_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        uint32_t value;
    };
} vsf_i2c_status_t;

/*============================ INCLUDES ======================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* __HAL_DRIVER_${DEVICE}_I2C_H__ */
#endif
/* EOF */
