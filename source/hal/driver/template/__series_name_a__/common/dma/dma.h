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

// HW
/*\note hw DMA driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_MODE for vsf_dma_mode_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_STATUS for vsf_dma_status_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_dma_irq_mask_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_CTRL for vsf_dma_ctrl_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_CFG for vsf_dma_cfg_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_dma_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_DMA_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_CTRL         ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_CFG          ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY   ENABLED


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${dma_ip}_dma_t) {
#if VSF_${DMA_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_dma_t               vsf_dma;
    )
#endif

/*\note You can add more member in vsf_${dma_ip}_dma_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${dma_ip}_dma_reg_t *reg;
        vsf_dma_isr_t           isr;
    )
};
// IPCore end


// HW/IPCore, not for emulated drivers
#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE == ENABLED
typedef enum vsf_dma_channel_mode_t {
    VSF_DMA_MEMORY_TO_MEMORY        = (0x00 << 0),
    VSF_DMA_MEMORY_TO_PERIPHERAL    = (0x01 << 0),
    VSF_DMA_PERIPHERA_TO_MEMORY     = (0x02 << 0),
    VSF_DMA_PERIPHERA_TO_PERIPHERAL = (0x03 << 0),

    VSF_DMA_SRC_ADDR_INCREMENT      = (0x00 << 2),
    VSF_DMA_SRC_ADDR_DECREMENT      = (0x01 << 2),
    VSF_DMA_SRC_ADDR_NO_CHANGE      = (0x02 << 2),

    VSF_DMA_DST_ADDR_INCREMENT      = (0x00 << 4),
    VSF_DMA_DST_ADDR_DECREMENT      = (0x01 << 4),
    VSF_DMA_DST_ADDR_NO_CHANGE      = (0x02 << 4),

    VSF_DMA_SRC_WIDTH_BYTE_1        = (0x00 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_2       = (0x01 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_4       = (0x02 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_8       = (0x03 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_16      = (0x04 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_32      = (0x05 << 6),

    VSF_DMA_DST_WIDTH_BYTE_1        = (0x00 << 10),
    VSF_DMA_DST_WIDTH_BYTES_2       = (0x01 << 10),
    VSF_DMA_DST_WIDTH_BYTES_4       = (0x02 << 10),
    VSF_DMA_DST_WIDTH_BYTES_8       = (0x03 << 10),
    VSF_DMA_DST_WIDTH_BYTES_16      = (0x04 << 10),
    VSF_DMA_DST_WIDTH_BYTES_32      = (0x05 << 10),

    VSF_DMA_SRC_BURST_LENGTH_1       = (0x00 << 14),
    VSF_DMA_SRC_BURST_LENGTH_2       = (0x01 << 14),
    VSF_DMA_SRC_BURST_LENGTH_4       = (0x02 << 14),
    VSF_DMA_SRC_BURST_LENGTH_8       = (0x03 << 14),
    VSF_DMA_SRC_BURST_LENGTH_16      = (0x04 << 14),
    VSF_DMA_SRC_BURST_LENGTH_32      = (0x05 << 14),
    VSF_DMA_SRC_BURST_LENGTH_64      = (0x05 << 14),
    VSF_DMA_SRC_BURST_LENGTH_128     = (0x06 << 14),

    VSF_DMA_DST_BURST_LENGTH_1       = (0x00 << 18),
    VSF_DMA_DST_BURST_LENGTH_2       = (0x01 << 18),
    VSF_DMA_DST_BURST_LENGTH_4       = (0x02 << 18),
    VSF_DMA_DST_BURST_LENGTH_8       = (0x03 << 18),
    VSF_DMA_DST_BURST_LENGTH_16      = (0x04 << 18),
    VSF_DMA_DST_BURST_LENGTH_32      = (0x05 << 18),
    VSF_DMA_DST_BURST_LENGTH_64      = (0x05 << 18),
    VSF_DMA_DST_BURST_LENGTH_128     = (0x06 << 18),

    VSF_DMA_PRIOPIRY_LOW             = (0x00 << 22),
    VSF_DMA_PRIOPIRY_MEDIUM          = (0x01 << 22),
    VSF_DMA_PRIOPIRY_HIGH            = (0x02 << 22),
    VSF_DMA_PRIOPIRY_VERY_HIGH       = (0x03 << 22),
} vsf_dma_channel_mode_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_dma_irq_mask_t {
    VSF_DMA_IRQ_MASK_CPL        = (0x1ul << 0),
    VSF_DMA_IRQ_MASK_HALF_CPL   = (0x1ul << 1),
    VSF_DMA_IRQ_MASK_ERROR      = (0x1ul << 2),
} vsf_dma_irq_mask_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG == ENABLED
typedef struct vsf_dma_t vsf_dma_t;
typedef void vsf_dma_isr_handler_t(void *target_ptr, vsf_dma_t *dma_ptr, int8_t channel, vsf_dma_irq_mask_t irq_mask);
typedef struct vsf_dma_isr_t {
    vsf_dma_isr_handler_t *handler_fn;
    void                  *target_ptr;
} vsf_dma_isr_t;
typedef struct vsf_dma_channel_cfg_t {
    vsf_dma_channel_mode_t  mode;
    vsf_dma_isr_t           isr;
    uint8_t                 src_idx;
    uint8_t                 dst_idx;
} vsf_dma_channel_cfg_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_STATUS == ENABLED
typedef struct vsf_dma_channel_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_dma_channel_status_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_dma_capability_t {
#if VSF_DMA_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_dma_irq_mask_t irq_mask;
    uint32_t max_request_count;
    uint8_t channel_count;
} vsf_dma_capability_t;
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
#undef __VSF_HAL_${DMA_IP}_DMA_CLASS_IMPLEMENT
#undef __VSF_HAL_${DMA_IP}_DMA_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_DMA
#endif      // __HAL_DRIVER_${SERIES/DMA_IP}_DMA_H__
/* EOF */
