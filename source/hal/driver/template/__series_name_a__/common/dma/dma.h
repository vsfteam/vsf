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

#ifndef __HAL_DRIVER_${SERIES/DMA_IP}_DMA_H__
#define __HAL_DRIVER_${SERIES/DMA_IP}_DMA_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_DMA == ENABLED

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripherial drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Incudes CAN ONLY be put here. */
/*\note If current header is for a peripherial driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

// IPCore
#if     defined(__VSF_HAL_${DMA_IP}_DMA_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${DMA_IP}_DMA_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${DMA_IP}_DMA_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_DMA_CFG_MULTI_CLASS should be in dma.c.
 */

// IPCore
#ifndef VSF_${DMA_IP}_DMA_CFG_MULTI_CLASS
#   define VSF_${DMA_IP}_DMA_CFG_MULTI_CLASS    VSF_DMA_CFG_MULTI_CLASS
#endif
// IPCore end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${dma_ip}_dma_t) {
#if VSF_${DMA_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_dma_t               vsf_dma;
    )
#endif

/*\note You can add more memmber in vsf_${dma_ip}_dma_t instance.
 *      For members accessable from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${dma_ip}_dma_reg_t *reg;
        vsf_dma_isr_t           isr;
    )
};
// IPCore end

/*============================ INCLUDES ======================================*/

// IPCore
/*\note Extern APIs for ip core diriver.
 *      There is no requirement about how APIs of IPCore drivers should be implemented.
 *      Just consider the simplicity for actual peripherial drivers.
 */
// IPCore end

#ifdef __cplusplus
}
#endif

// IPCore
#undef __VSF_HAL_${DMA_IP}_DMA_CLASS_IMPLEMENT
#undef __VSF_HAL_${DMA_IP}_DMA_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_DMA
#endif      // __HAL_DRIVER_${SERIES/DMA_IP}_DMA_H__
/* EOF */