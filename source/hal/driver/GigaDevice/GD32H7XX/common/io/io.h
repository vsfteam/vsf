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

#ifndef __HAL_DRIVER_GIGADEVICE_GD32H7XX_IO_H__
#define __HAL_DRIVER_GIGADEVICE_GD32H7XX_IO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_IO == ENABLED

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 *
 *      Usually, there is no IPCore driver for IO.
 */

/*\note Includes CAN ONLY be put here. */

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// HW
/*\note hw IO driver can reimplement vsf_io_mode_t.
 *      To enable reimplementation, please enable macro below:
 *          VSF_IO_CFG_REIMPLEMENT_TYPE_MODE for vsf_io_mode_t
 *      Reimplementation is used for optimization hw drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_IO_CFG_REIMPLEMENT_TYPE_MODE        ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW, not for emulated drivers
typedef enum vsf_io_mode_t {
    // 0..1, GPIO_CTL: INPUT(0)/OUTPUT(1)/AF(2)/ANALOG(3)
    // 2: GPIO_OMODE: PUSHPULL(0)/OPENDRAIN(1)
    VSF_IO_INPUT                                = (0 << 0),     //!< enable input mode
    VSF_IO_ANALOG                               = (3 << 0),     //!< enable analog function
    VSF_IO_OUTPUT_PUSH_PULL                     = (1 << 4) | (0 << 2),  //!< enable output push-pull mode
    VSF_IO_OUTPUT_OPEN_DRAIN                    = (1 << 4) | (1 << 2),  //!< enable output open-drain mode
    VSF_IO_AF                                   = (2 << 0),     //!< enable AF mode
    VSF_IO_EXTI                                 = VSF_IO_INPUT,

    // 3..4, GPIO_PUD: FLOATING(0)/PULLUP(1)/PULLDOWN(2)
    VSF_IO_NO_PULL_UP_DOWN                      = (0 << 3),     //!< enable floating
    VSF_IO_PULL_UP                              = (1 << 3),     //!< enable pull-up resistor
    VSF_IO_PULL_DOWN                            = (2 << 3),     //!< enable pull-down resistor

    // 5..6, GPIO_OSPD: 12M(0)/60M(1)/85M(2)/100/220M(3)
    VSF_IO_SPEED_12MHZ                          = (0 << 5),
    VSF_IO_SPEED_60MHZ                          = (1 << 5),
    VSF_IO_SPEED_85MHZ                          = (2 << 5),
    VSF_IO_SPEED_100MHZ_220MHZ                  = (3 << 5),
    VSF_IO_SPEED_MASK                           = (3 << 5),
#define VSF_IO_SPEED_12MHZ                      VSF_IO_SPEED_12MHZ
#define VSF_IO_SPEED_60MHZ                      VSF_IO_SPEED_60MHZ
#define VSF_IO_SPEED_85MHZ                      VSF_IO_SPEED_85MHZ
#define VSF_IO_SPEED_100MHZ_220MHZ              VSF_IO_SPEED_100MHZ_220MHZ
#define VSF_IO_SPEED_MASK                       VSF_IO_SPEED_MASK

    // TODO: add input fileter modes


    __VSF_HW_IO_MODE_ALL_BITS                   = 0x7F,
} vsf_io_mode_t;
// HW end

/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_IO
#endif      // __HAL_DRIVER_GIGADEVICE_GD32H7XX_IO_H__
/* EOF */
