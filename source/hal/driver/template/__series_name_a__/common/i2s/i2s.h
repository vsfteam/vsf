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

#ifndef __HAL_DRIVER_${SERIES/I2S_IP}_I2S_H__
#define __HAL_DRIVER_${SERIES/I2S_IP}_I2S_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_I2S == ENABLED

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
#if     defined(__VSF_HAL_${I2S_IP}_I2S_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${I2S_IP}_I2S_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_${I2S_IP}_I2S_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_I2S_CFG_MULTI_CLASS should be in i2s.c.
 */

// IPCore
#ifndef VSF_${I2S_IP}_I2S_CFG_MULTI_CLASS
#   define VSF_${I2S_IP}_I2S_CFG_MULTI_CLASS    VSF_I2S_CFG_MULTI_CLASS
#endif
// IPCore end

// HW
/*\note hw I2S driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_I2S_CFG_REIMPLEMENT_TYPE_MODE for vsf_i2s_mode_t
 *          VSF_I2S_CFG_REIMPLEMENT_TYPE_STATUS for vsf_i2s_status_t
 *          VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_i2s_irq_mask_t
 *          VSF_I2S_CFG_REIMPLEMENT_TYPE_CTRL for vsf_i2s_ctrl_t
 *          VSF_I2S_CFG_REIMPLEMENT_TYPE_CFG for vsf_i2s_cfg_t
 *          VSF_I2S_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_i2s_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_I2S_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_I2S_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_I2S_CFG_REIMPLEMENT_TYPE_CTRL         ENABLED
#define VSF_I2S_CFG_REIMPLEMENT_TYPE_CFG          ENABLED
#define VSF_I2S_CFG_REIMPLEMENT_TYPE_CAPABILITY   ENABLED
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${i2s_ip}_i2s_t) {
#if VSF_${I2S_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_i2s_t               vsf_i2s;
    )
#endif

/*\note You can add more member in vsf_${i2s_ip}_i2s_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${i2s_ip}_i2s_reg_t *reg;
        vsf_i2s_isr_t           isr;
    )
};
// IPCore end

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
#undef __VSF_HAL_${I2S_IP}_I2S_CLASS_IMPLEMENT
#undef __VSF_HAL_${I2S_IP}_I2S_CLASS_INHERIT__
// IPCore end

// HW/IPCore, not for emulated drivers
#if VSF_I2S_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_i2s_mode_t {
    VSF_I2S_MODE_MASTER             = (0x01ul << 0),
    VSF_I2S_MODE_SLAVE              = (0x00ul << 0),

    VSF_I2S_DATA_BITLEN_16          = (0x01ul << 1),
    VSF_I2S_DATA_BITLEN_24          = (0x02ul << 1),
    VSF_I2S_DATA_BITLEN_32          = (0x03ul << 1),

    VSF_I2S_FRAME_BITLEN_16         = (0x01ul << 3),
    VSF_I2S_FRAME_BITLEN_24         = (0x02ul << 3),
    VSF_I2S_FRAME_BITLEN_32         = (0x03ul << 3),

    VSF_I2S_STANDARD_PHILIPS        = (0x01ul << 5),
    VSF_I2S_STANDARD_MSB            = (0x02ul << 5),
    VSF_I2S_STANDARD_LSB            = (0x03ul << 5),

    VSF_I2S_LRCK_POL                = (0x01ul << 7),
    VSF_I2S_BCK_POL                 = (0x01ul << 8),
    VSF_I2S_MCLK_OUTPUT             = (0x01ul << 9),
} vsf_i2s_mode_t;
#endif

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_i2s_irq_mask_t {
    VSF_I2S_IRQ_MASK_TX_TGL_BUFFER  = (0x1ul <<  0),
    VSF_I2S_IRQ_MASK_RX_TGL_BUFFER  = (0x1ul <<  1),
} vsf_i2s_irq_mask_t;
#endif

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_STATUS == ENABLED
typedef struct vsf_i2s_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        uint32_t value;
    };
} vsf_i2s_status_t;
#endif

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_i2s_capability_t {
#if VSF_I2S_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    struct {
        bool is_src_supported;
        bool is_dbuffer_supported;
    } i2s_capability;
} vsf_i2s_capability_t;
#endif

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_i2s_t vsf_i2s_t;
typedef void vsf_i2s_isr_handler_t(void *target_ptr, vsf_i2s_t *i2s_ptr, vsf_i2s_irq_mask_t irq_mask);
typedef struct vsf_i2s_isr_t {
    vsf_i2s_isr_handler_t *handler_fn;
    void                  *target_ptr;
    vsf_arch_prio_t        prio;
} vsf_i2s_isr_t;
typedef struct vsf_i2s_cfg_t {
    vsf_i2s_mode_t mode;
    uint32_t data_sample_rate;
    uint32_t hw_sample_rate;
    uint8_t *buffer;
    uint16_t buffer_size;
    uint8_t channel_num;
    vsf_i2s_isr_t isr;
} vsf_i2s_cfg_t;
#endif
// HW/IPCore end

#endif      // VSF_HAL_USE_I2S
#endif      // __HAL_DRIVER_${SERIES/I2S_IP}_I2S_H__
/* EOF */
