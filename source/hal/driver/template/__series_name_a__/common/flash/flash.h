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
 *  See the License for the flashecific language governing permissions and   *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __HAL_DRIVER_${DEVICE/FLASH_IP}_FLASH_H__
#define __HAL_DRIVER_${DEVICE/FLASH_IP}_FLASH_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_FLASH == ENABLED

#include "../__device.h"

/*\note Incudes CAN ONLY be put here. */

/*\note Refer to template/README.md for usage cases.
 *      It's recommended to use blackbox mode for peripherial drivers, leave only reimplementation part open.
 *      Otherwise class structure, MULTI_CLASS configuration and class APIs should be open to user, and no reimplementation part.
 */

// IPCore
#if     defined(__VSF_HAL_${FLASH_IP}_FLASH_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${FLASH_IP}_FLASH_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${FLASH_IP}_FLASH_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_FLASH_CFG_MULTI_CLASS should be in flash.c.
 */

// IPCore
#ifndef VSF_${FLASH_IP}_FLASH_CFG_MULTI_CLASS
#   define VSF_${FLASH_IP}_FLASH_CFG_MULTI_CLASS    VSF_FLASH_CFG_MULTI_CLASS
#endif
// IPCore end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

// IPCore
/*\note Extern APIs for ip core diriver.
 *      There is no requirement about how APIs of IPCore drivers should be implemented.
 *      Just consider the simplicity for drivers of actual device.
 */
// IPCore end

#ifdef __cplusplus
}
#endif

#endif /* VSF_HAL_USE_FLASH */
#endif
/* EOF */
