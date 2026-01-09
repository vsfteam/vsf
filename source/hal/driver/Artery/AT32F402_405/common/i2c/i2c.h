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

#ifndef __HAL_DRIVER_ARTERY_AT32F402_405_I2C_H__
#define __HAL_DRIVER_ARTERY_AT32F402_405_I2C_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_I2C == ENABLED

// HW/IPCore
/**
 * \note When vsf_peripheral_status_t is inherited, vsf_template_hal_driver.h needs to be included
 */
#include "hal/driver/common/template/vsf_template_hal_driver.h"
// HW/IPCore end

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Includes CAN ONLY be put here. */
/*\note If current header is for a peripheral driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

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

#define VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK           ENABLED
#define VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD                ENABLED
// HW end

/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD == ENABLED
typedef enum vsf_i2c_cmd_t {
    // 10: I2C_CTRL2.DIR(10)
    VSF_I2C_CMD_WRITE                                   = (0 << 10),
    VSF_I2C_CMD_READ                                    = (1 << 10),

    // 13: I2C_CTRL2.GENSTART(13)
    VSF_I2C_CMD_START                                   = (1 << 13),
    VSF_I2C_CMD_NO_START                                = (0 << 13),
    VSF_I2C_CMD_RESTART                                 = (1 << 13),
    VSF_I2C_CMD_NO_RESTART                              = (1 << 13),
    // 14: I2C_CTRL2.GENSTOP(14)
    VSF_I2C_CMD_STOP                                    = (1 << 14),
    VSF_I2C_CMD_NO_STOP                                 = (0 << 14),

    // 11: I2C_CTRL2.ADDR10(11)
    VSF_I2C_CMD_7_BITS                                  = (0 << 11),
    VSF_I2C_CMD_10_BITS                                 = (1 << 11),

    // more vendor specified commands can be added here

    __VSF_HW_I2C_CMD_MASK                               = VSF_I2C_CMD_READ
                                                        | VSF_I2C_CMD_WRITE
                                                        | VSF_I2C_CMD_START
                                                        | VSF_I2C_CMD_RESTART
                                                        | VSF_I2C_CMD_STOP
                                                        | VSF_I2C_CMD_7_BITS
                                                        | VSF_I2C_CMD_10_BITS,
} vsf_i2c_cmd_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_i2c_irq_mask_t {
    // 1: I2C_STS.TDIS(1)
    VSF_I2C_IRQ_MASK_MASTER_TX                          = (1 << 1),
    // 2: I2C_STS.RDBF(2)
    VSF_I2C_IRQ_MASK_MASTER_RX                          = (1 << 2),
    // 24: virtual
    VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE           = (1 << 24),
    // 9: I2C_STS.ARLOST(9)
    VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST            = (1 << 9),
    // 25: virtual, I2C_STS.ACKFAILF(4)|!I2C_STS.ADDRF(3)
    VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK                = (1 << 25),
    // 26: virtual, I2C_STS.ACKFAILF(4)|I2C_STS.ADDRF(3)
    VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT              = (1 << 26),

    // below 2 are not supported
    VSF_I2C_IRQ_MASK_MASTER_START_OR_RESTART_DETECT     = (1 << 29),
    VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT                 = (1 << 30),

    // 27: virtual
    VSF_I2C_IRQ_MASK_SLAVE_START_OR_RESTART_DETECT      = (1 << 27),
    // 5: I2C_STS.STOPF(5)
    VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT                  = (1 << 5),
    // 3: I2C_STS.ADDRF(3)
    VSF_I2C_IRQ_MASK_SLAVE_ADDRESS_ACK                  = (1 << 3),
    VSF_I2C_IRQ_MASK_SLAVE_TX                           = VSF_I2C_IRQ_MASK_MASTER_TX,
    VSF_I2C_IRQ_MASK_SLAVE_RX                           = VSF_I2C_IRQ_MASK_MASTER_RX,
    // 28: virtual
    VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE            = (1 << 28),

    // more vendor specified irq_masks can be added here

    __VSF_HW_I2C_VIRTUAL_MASK                           = VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                                                        | VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                                                        | VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT
                                                        | VSF_I2C_IRQ_MASK_MASTER_START_OR_RESTART_DETECT
                                                        | VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE,
    __VSF_HW_I2C_REAL_MASK                              = VSF_I2C_IRQ_MASK_MASTER_TX
                                                        | VSF_I2C_IRQ_MASK_MASTER_RX
                                                        | VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST
                                                        | (1 << 4)      // ACKFAIL
                                                        | VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT
                                                        | VSF_I2C_IRQ_MASK_SLAVE_ADDRESS_ACK,
} vsf_i2c_irq_mask_t;
#endif
// HW/IPCore end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_I2C
#endif      // __HAL_DRIVER_ARTERY_AT32F402_405_I2C_H__
/* EOF */
