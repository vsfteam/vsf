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

#ifndef __HAL_DRIVER_ARTERY_AT32F402_405_DMA_H__
#define __HAL_DRIVER_ARTERY_AT32F402_405_DMA_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_DMA == ENABLED

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
/*\note hw DMA driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_ADDR for vsf_dma_addr_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE for vsf_dma_channel_mode_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_dma_irq_mask_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_HINT for vsf_dma_channel_hint_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG for vsf_dma_channel_cfg_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_SG_CFG for vsf_dma_channel_sg_cfg_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_STATUS for vsf_dma_channel_status_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_CFG for vsf_dma_cfg_t
 *          VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_dma_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

// vsf_dma_addr_t: must be enabled if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_SG_CFG is enabled
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_ADDR           ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE   ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK       ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_HINT   ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG    ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_SG_CFG ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_STATUS ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_CFG            ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY     ENABLED


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
#if VSF_DMA_CFG_REIMPLEMENT_TYPE_ADDR == ENABLED
typedef uintptr_t vsf_dma_addr_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE == ENABLED
typedef enum vsf_dma_channel_mode_t {
    // 4|14: DMA_CxCTRL.DTS(4) | DMA_CxCTRL.M2M(14)
    VSF_DMA_MEMORY_TO_MEMORY                    = (1 << 14) | (0 << 4),
    VSF_DMA_MEMORY_TO_PERIPHERAL                = (0 << 14) | (1 << 4),
    VSF_DMA_PERIPHERAL_TO_MEMORY                = (0 << 14) | (0 << 4),

    // VSF_DMA_SRC_XXX and VSF_DMA_DST_XXX will be switched, according to dma mode by driver software
    // so VSF_DMA_SRC_XXX and VSF_DMA_DST_XXX are just place holder

    // 6: DMA_CxCTRL.PINCM(6)
    VSF_DMA_SRC_ADDR_INCREMENT                  = (1 << 6),
    VSF_DMA_SRC_ADDR_NO_CHANGE                  = (0 << 6),
    // 8..9: DMA_CxCTRL.PWIDTH(8..9)
    VSF_DMA_SRC_WIDTH_BYTE_1                    = (0 << 8),
    VSF_DMA_SRC_WIDTH_BYTES_2                   = (1 << 8),
    VSF_DMA_SRC_WIDTH_BYTES_4                   = (2 << 8),

    // 7: DMA_CxCTRL.MIMCM(7)
    VSF_DMA_DST_ADDR_INCREMENT                  = (1 << 7),
    VSF_DMA_DST_ADDR_NO_CHANGE                  = (0 << 7),
    // 10..11: DMA_CxCTRL.MWIDTH(10..11)
    VSF_DMA_DST_WIDTH_BYTE_1                    = (0 << 10),
    VSF_DMA_DST_WIDTH_BYTES_2                   = (1 << 10),
    VSF_DMA_DST_WIDTH_BYTES_4                   = (2 << 10),

    // 12..13: DMA_CxCTRL.CHPL(12..13)
    VSF_DMA_PRIORITY_LOW                        = (0x00 << 12),
    VSF_DMA_PRIORITY_MEDIUM                     = (0x01 << 12),
    VSF_DMA_PRIORITY_HIGH                       = (0x02 << 12),
    VSF_DMA_PRIORITY_VERY_HIGH                  = (0x03 << 12),

    // 16..26: vendor specified

    // 17: DMA_MUXCxCTRL.EVTGEN(9)
    VSF_DMA_EVENT                               = (1 << 17),
    // 24..26: DMA_MUXCxCTRL.SYNCEN(16) | DMA_MUXCxCTRL.SYNCPOL(17..18)
    VSF_DMA_SYNC_RISING                         = (1 << 24) | (1 << 25),
    VSF_DMA_SYNC_FALLING                        = (1 << 24) | (2 << 25),
    VSF_DMA_SYNC_RISING_AND_FALLING             = (1 << 24) | (3 << 25),



    // NOT SUPPORTED
    VSF_DMA_PERIPHERAL_TO_PERIPHERAL            = (1 << 16),
    VSF_DMA_SRC_ADDR_DECREMENT                  = (1 << 17),
    VSF_DMA_DST_ADDR_DECREMENT                  = (1 << 18),

    VSF_DMA_SRC_WIDTH_BYTES_8                   = (0 << 19),
    VSF_DMA_SRC_WIDTH_BYTES_16                  = (1 << 19),
    VSF_DMA_SRC_WIDTH_BYTES_32                  = (2 << 19),

    VSF_DMA_DST_WIDTH_BYTES_8                   = (0 << 21),
    VSF_DMA_DST_WIDTH_BYTES_16                  = (1 << 21),
    VSF_DMA_DST_WIDTH_BYTES_32                  = (2 << 21),

    VSF_DMA_SRC_BURST_LENGTH_1                  = (0 << 23),
    VSF_DMA_SRC_BURST_LENGTH_2                  = (1 << 23),
    VSF_DMA_SRC_BURST_LENGTH_4                  = (2 << 23),
    VSF_DMA_SRC_BURST_LENGTH_8                  = (3 << 23),
    VSF_DMA_SRC_BURST_LENGTH_16                 = (4 << 23),
    VSF_DMA_SRC_BURST_LENGTH_32                 = (5 << 23),
    VSF_DMA_SRC_BURST_LENGTH_64                 = (6 << 23),
    VSF_DMA_SRC_BURST_LENGTH_128                = (7 << 23),

    VSF_DMA_DST_BURST_LENGTH_1                  = (0 << 26),
    VSF_DMA_DST_BURST_LENGTH_2                  = (1 << 26),
    VSF_DMA_DST_BURST_LENGTH_4                  = (2 << 26),
    VSF_DMA_DST_BURST_LENGTH_8                  = (3 << 26),
    VSF_DMA_DST_BURST_LENGTH_16                 = (4 << 26),
    VSF_DMA_DST_BURST_LENGTH_32                 = (5 << 26),
    VSF_DMA_DST_BURST_LENGTH_64                 = (6 << 26),
    VSF_DMA_DST_BURST_LENGTH_128                = (7 << 26),

    /*\note DMA_CxCTRL register: control register mask for configuration bits (bits [14:4], excluding interrupt-related bits)
     *      This mask preserves configuration bits (DTS, PINCM, MIMCM, PWIDTH, MWIDTH, CHPL, M2M)
     *      while ensuring CHEN bit (bit 0) is cleared
     *      Note: Interrupt-related bits (bits 1-3, 5) are not included as they are not configured here
     *      Reference: RM_AT32F402_405_CH_V2.02 Section 9.5.3
     */
    VSF_HW_DMA_CTRL_CONFIG_MASK                 = VSF_DMA_MEMORY_TO_MEMORY |
                                                   VSF_DMA_MEMORY_TO_PERIPHERAL |
                                                   VSF_DMA_PERIPHERAL_TO_MEMORY |
                                                   VSF_DMA_SRC_ADDR_INCREMENT |
                                                   VSF_DMA_SRC_ADDR_NO_CHANGE |
                                                   VSF_DMA_SRC_WIDTH_BYTE_1 |
                                                   VSF_DMA_SRC_WIDTH_BYTES_2 |
                                                   VSF_DMA_SRC_WIDTH_BYTES_4 |
                                                   VSF_DMA_DST_ADDR_INCREMENT |
                                                   VSF_DMA_DST_ADDR_NO_CHANGE |
                                                   VSF_DMA_DST_WIDTH_BYTE_1 |
                                                   VSF_DMA_DST_WIDTH_BYTES_2 |
                                                   VSF_DMA_DST_WIDTH_BYTES_4 |
                                                   VSF_DMA_PRIORITY_LOW |
                                                   VSF_DMA_PRIORITY_MEDIUM |
                                                   VSF_DMA_PRIORITY_HIGH |
                                                   VSF_DMA_PRIORITY_VERY_HIGH,
} vsf_dma_channel_mode_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_dma_irq_mask_t {
    // 1: DMA_STS.FDTFx
    VSF_DMA_IRQ_MASK_CPL                        = (1 << 1),
    // 2: DMA_STS.HDTFx
    VSF_DMA_IRQ_MASK_HALF_CPL                   = (1 << 2),
    // 3: DMA_STS.DTERRFx
    VSF_DMA_IRQ_MASK_ERROR                      = (1 << 3),
} vsf_dma_irq_mask_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_HINT == ENABLED
typedef struct vsf_dma_channel_hint_t {
    int8_t                                      channel;
    uint8_t                                     request_line;
    vsf_arch_prio_t                             prio;
} vsf_dma_channel_hint_t;
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
    vsf_dma_irq_mask_t      irq_mask;
    vsf_arch_prio_t         prio;
    uint8_t                 src_request_idx;
    uint8_t                 dst_request_idx;

    // vendor
    // DMA_MUXCxCTRL.REQCNT(19..23)
    uint8_t                 sync_reqcnt;
    // DMA_MUXCxCTRL.SYNCSEL(24..28)
    uint8_t                 sync_signal;
} vsf_dma_channel_cfg_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_STATUS == ENABLED
typedef struct vsf_dma_channel_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_dma_channel_status_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_SG_CFG == ENABLED
typedef struct vsf_dma_channel_sg_desc_t {
    vsf_dma_channel_mode_t mode;
    vsf_dma_addr_t src_address;
    vsf_dma_addr_t dst_address;
    uint32_t count;
    vsf_dma_addr_t next;
} vsf_dma_channel_sg_desc_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_dma_cfg_t {
    vsf_arch_prio_t prio;
} vsf_dma_cfg_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_dma_capability_t {
#if VSF_DMA_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_dma_irq_mask_t irq_mask;
    uint8_t channel_count;
    uint8_t irq_count;
    vsf_dma_channel_mode_t supported_modes;
    uint32_t max_transfer_count;
    uint8_t addr_alignment;
    uint8_t support_scatter_gather : 1;
} vsf_dma_capability_t;
#endif

// HW/IPCore end

/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_DMA
#endif      // __HAL_DRIVER_ARTERY_AT32F402_405_DMA_H__
/* EOF */
