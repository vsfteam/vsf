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

#ifndef __HAL_DRIVER_${SERIES/SDIO_IP}_SDIO_H__
#define __HAL_DRIVER_${SERIES/SDIO_IP}_SDIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SDIO == ENABLED

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
#if     defined(__VSF_HAL_${SDIO_IP}_SDIO_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${SDIO_IP}_SDIO_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${SDIO_IP}_SDIO_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_SDIO_CFG_MULTI_CLASS should be in sdio.c.
 */

// IPCore
#ifndef VSF_${SDIO_IP}_SDIO_CFG_MULTI_CLASS
#   define VSF_${SDIO_IP}_SDIO_CFG_MULTI_CLASS  VSF_SDIO_CFG_MULTI_CLASS
#endif
// IPCore end

// HW
/*\note hw SDIO driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_SDIO_CFG_REIMPLEMENT_TYPE_MODE for vsf_sdio_mode_t
 *          VSF_SDIO_CFG_REIMPLEMENT_TYPE_STATUS for vsf_sdio_status_t
 *          VSF_SDIO_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_sdio_irq_mask_t
 *          VSF_SDIO_CFG_REIMPLEMENT_TYPE_CTRL for vsf_sdio_ctrl_t
 *          VSF_SDIO_CFG_REIMPLEMENT_TYPE_CFG for vsf_sdio_cfg_t
 *          VSF_SDIO_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_sdio_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_SDIO_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_SDIO_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_SDIO_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_SDIO_CFG_REIMPLEMENT_TYPE_CTRL         ENABLED
#define VSF_SDIO_CFG_REIMPLEMENT_TYPE_CFG          ENABLED
#define VSF_SDIO_CFG_REIMPLEMENT_TYPE_CAPABILITY   ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${sdio_ip}_sdio_t) {
#if VSF_${SDIO_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_sdio_t                  vsf_sdio;
    )
#endif

/*\note You can add more member in vsf_${sdio_ip}_sdio_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${sdio_ip}_sdio_reg_t   *reg;
        vsf_sdio_isr_t              isr;
    )
};
// IPCore end

// HW/IPCore, not for emulated drivers
#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_sdio_mode_t {
    VSF_SDIO_MODE_HOST                  = (0x1ul << 0),
    VSF_SDIO_MODE_SLAVE                 = (0x0ul << 0),
    VSF_SDIO_MODE_MASK                  = (0x1ul << 0),
} vsf_sdio_mode_t;
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_REQOP == ENABLED
typedef enum vsf_sdio_reqop_t {
    SDIO_CMDOP_BYTE                 = (0ul << 0),
    SDIO_CMDOP_STREAM               = (1ul << 0),
    SDIO_CMDOP_SINGLE_BLOCK         = (2ul << 0),
    SDIO_CMDOP_MULTI_BLOCK          = (3ul << 0),
    SDIO_CMDOP_WRITE                = (1ul << 2),
    SDIO_CMDOP_READ                 = (0ul << 2),
    SDIO_CMDOP_RESP_BUSY            = (1ul << 3),
    __SDIO_CMDOP_RESP               = (1ul << 4),
    __SDIO_CMDOP_RESP_SHORT         = (1ul << 5),
    __SDIO_CMDOP_RESP_SHORT_CRC     = (2ul << 5),
    __SDIO_CMDOP_RESP_LONG_CRC      = (3ul << 5),
#define SDIO_RESP_NONE              0
#define SDIO_RESP_R1                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)
#define SDIO_RESP_R1B               (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC | SDIO_CMDOP_RESP_BUSY)
#define SDIO_RESP_R2                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_LONG_CRC)
#define SDIO_RESP_R3                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT)
#define SDIO_RESP_R4                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT)
#define SDIO_RESP_R5                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)
#define SDIO_RESP_R6                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)
#define SDIO_RESP_R7                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)
    SDIO_CMDOP_CLKHOLD              = (1ul << 7),
    SDIO_CMDOP_TRANS_STOP           = (1ul << 8),
} vsf_sdio_reqop_t;
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_sdio_irq_mask_t {
    // TODO: add irq mask for stream mode
    SDIO_IRQ_MASK_HOST_RESP_DONE    = (0x1ul <<  0),
    SDIO_IRQ_MASK_HOST_DATA_DONE    = (0x1ul <<  1),
    SDIO_IRQ_MASK_HOST_DATA_ABORT   = (0x1ul <<  2),        // aborted by CMD12
    SDIO_IRQ_MASK_HOST_ALL          = SDIO_IRQ_MASK_HOST_RESP_DONE
                                    | SDIO_IRQ_MASK_HOST_DATA_DONE
                                    | SDIO_IRQ_MASK_HOST_DATA_ABORT,
} vsf_sdio_irq_mask_t;
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_REQSTS == ENABLED
typedef enum vsf_sdio_reqsts_t {
    SDIO_REQSTS_DONE                = 0,
    SDIO_REQSTS_ERR_RESP_NONE       = (0x1ul <<  0),
    SDIO_REQSTS_ERR_RESP_CRC        = (0x1ul <<  1),
    SDIO_REQSTS_ERR_DATA_CRC        = (0x1ul <<  2),
    SDIO_REQSTS_DATA_BUSY           = (0x1ul <<  3),
    SDIO_REQSTS_BUSY                = (0x1ul <<  4),
    SDIO_REQSTS_ERR_MASK            = SDIO_REQSTS_ERR_RESP_NONE
                                    | SDIO_REQSTS_ERR_RESP_CRC
                                    | SDIO_REQSTS_ERR_DATA_CRC,
} vsf_sdio_reqsts_t;
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_STATUS == ENABLED
typedef struct vsf_sdio_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        vsf_sdio_reqsts_t req_status;
        vsf_sdio_irq_mask_t irq_status;
    };
} vsf_sdio_status_t;
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_sdio_t vsf_sdio_t;
typedef void vsf_sdio_isr_handler_t(void *target_ptr,
                                   vsf_sdio_t *sdio_ptr,
                                   vsf_sdio_irq_mask_t irq_mask,
                                   vsf_sdio_reqsts_t status,
                                   uint32_t resp[4]);
typedef struct vsf_sdio_isr_t {
    vsf_sdio_isr_handler_t *handler_fn;

    void                  *target_ptr;

    vsf_arch_prio_t        prio;

} vsf_sdio_isr_t;
typedef struct vsf_sdio_cfg_t {
    vsf_sdio_mode_t mode;

    vsf_sdio_isr_t isr;

} vsf_sdio_cfg_t;
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_sdio_capability_t {
#if VSF_SDIO_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    enum {
        SDIO_CAP_BUS_WIDTH_1        = (0x1ul <<  0),
        SDIO_CAP_BUS_WIDTH_4        = (0x1ul <<  1),
        SDIO_CAP_BUS_WIDTH_8        = (0x1ul <<  2),
    } bus_width;
    uint32_t max_freq_hz;
    uint16_t data_ptr_alignment;
    uint16_t data_size_alignment;
    bool support_ddr;
} vsf_sdio_capability_t;
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
#undef __VSF_HAL_${SDIO_IP}_SDIO_CLASS_IMPLEMENT
#undef __VSF_HAL_${SDIO_IP}_SDIO_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_SDIO
#endif      // __HAL_DRIVER_${SERIES/SDIO_IP}_SDIO_H__
/* EOF */
