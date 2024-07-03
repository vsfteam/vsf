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
    VSF_IO_PULL_UP                              = (0 << 0),     //!< enable pull-up resistor
    VSF_IO_PULL_DOWN                            = (1 << 0),     //!< enable pull-down resistor
    VSF_IO_OPEN_DRAIN                           = (2 << 0),     //!< enable open-drain mode
    VSF_IO_ANALOG                               = (3 << 0),     //!< enable analog function

    VSF_IO_NORMAL_INPUT                         = (0 << 3),     //!< normal input pin level
    VSF_IO_INVERT_INPUT                         = (1 << 3),     //!< inverted input pin level
    VSF_IO_DISABLE_INPUT                        = (1 << 4),     //!< disable input

    VSF_IO_FILTER_BYPASS                        = (0 << 5),     //!< filter is bypassed
    VSF_IO_FILTER_2CLK                          = (1 << 5),     //!< levels should keep 2 clks
    VSF_IO_FILTER_4CLK                          = (2 << 5),     //!< levels should keep 4 clks
    VSF_IO_FILTER_8CLK                          = (3 << 5),     //!< levels should keep 8 clks

    VSF_IO_FILTER_CLK_SRC0                      = (0 << 7),     //!< select clock src 0 for filter
    VSF_IO_FILTER_CLK_SRC1                      = (1 << 7),     //!< select clock src 1 for filter
    VSF_IO_FILTER_CLK_SRC2                      = (2 << 7),     //!< select clock src 2 for filter
    VSF_IO_FILTER_CLK_SRC3                      = (3 << 7),     //!< select clock src 3 for filter
    VSF_IO_FILTER_CLK_SRC4                      = (4 << 7),     //!< select clock src 4 for filter
    VSF_IO_FILTER_CLK_SRC5                      = (5 << 7),     //!< select clock src 5 for filter
    VSF_IO_FILTER_CLK_SRC6                      = (6 << 7),     //!< select clock src 6 for filter
    VSF_IO_FILTER_CLK_SRC7                      = (7 << 7),     //!< select clock src 7 for filter

    VSF_IO_HIGH_DRIVE_STRENGTH                  = (1 << 10),    //!< enable high drive strength
    VSF_IO_HIGH_DRIVE_NO_STRENGTH               = (1 << 10),    //!< enable high drive strength

    VSF_IO_INTERRUPT_DISABLED                   = (0 << 11),
    VSF_IO_INTERRUPT_ENABLED                    = (1 << 11),
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
