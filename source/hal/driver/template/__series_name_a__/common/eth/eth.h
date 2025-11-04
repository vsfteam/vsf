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

#ifndef __HAL_DRIVER_${SERIES/ETH_IP}_ETH_H__
#define __HAL_DRIVER_${SERIES/ETH_IP}_ETH_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_ETH == ENABLED

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

// IPCore
#if     defined(__VSF_HAL_${ETH_IP}_ETH_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_${ETH_IP}_ETH_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ ETHROS ========================================*/

/*\note VSF_${ETH_IP}_ETH_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_ETH_CFG_MULTI_CLASS should be in eth.c.
 */

// IPCore
#ifndef VSF_${ETH_IP}_ETH_CFG_MULTI_CLASS
#   define VSF_${ETH_IP}_ETH_CFG_MULTI_CLASS    VSF_ETH_CFG_MULTI_CLASS
#endif
// IPCore end

// HW
/*\note hw ETH driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_PHY_MODE for vsf_eth_phy_mode_t
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_MODE for vsf_eth_mode_t
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_eth_irq_mask_t
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_CTRL for vsf_eth_ctrl_t
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_CFG for vsf_eth_cfg_t
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_STATUS for vsf_eth_status_t
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_eth_capability_t
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_MODE for vsf_eth_buf_mode_t
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_DESC for vsf_eth_buf_desc_t
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_SEND_BUF_DESC for vsf_eth_send_buf_desc_t
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_RECV_BUF_DESC for vsf_eth_recv_buf_desc_t
 *          VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_BUF_DESC for vsf_eth_sg_buf_desc_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_ETH_CFG_REIMPLEMENT_TYPE_PHY_MODE       ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_TYPE_MODE           ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_TYPE_IRQ_MASK       ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_TYPE_CTRL           ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_TYPE_CFG            ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_TYPE_STATUS         ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_TYPE_CAPABILITY     ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_MODE       ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_DESC       ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_TYPE_SEND_BUF_DESC  ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_TYPE_RECV_BUF_DESC  ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_BUF_DESC    ENABLED
// HW end

/*============================ ETHROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_${eth_ip}_eth_t) {
#if VSF_${ETH_IP}_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_eth_t               vsf_eth;
    )
#endif

/*\note You can add more member in vsf_${eth_ip}_eth_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_${eth_ip}_eth_reg_t *reg;
        vsf_eth_isr_t           isr;
    )
};
// IPCore end

// HW/IPCore, not for emulated drivers
#if VSF_ETH_CFG_REIMPLEMENT_TYPE_PHY_MODE == ENABLED
typedef enum vsf_eth_phy_mode_t {
    VSF_ETH_PHY_MODE_SPEED_10M        = 1 << 0,
    VSF_ETH_PHY_MODE_SPEED_100M       = 1 << 1,
    VSF_ETH_PHY_MODE_SPEED_1000M      = 1 << 2,

    VSF_ETH_PHY_MODE_DUPLEX_HALF      = 1 << 3,
    VSF_ETH_PHY_MODE_DUPLEX_FULL      = 1 << 4,

    // Add more vendor-specific modes here
} vsf_eth_phy_mode_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_eth_mode_t {
    VSF_ETH_MODE_TX_CHECKSUM_OFFLOAD = 1 << 5,
    VSF_ETH_MODE_RX_CHECKSUM_OFFLOAD = 1 << 6,

    // Add more vendor-specific modes here
} vsf_eth_mode_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_eth_irq_mask_t {
    VSF_ETH_IRQ_MASK_RX_AVAILABLE    = (1 << 0),
    VSF_ETH_IRQ_MASK_TX_COMPLETE     = (1 << 1),
    VSF_ETH_IRQ_MASK_SG_RX_AVAILABLE = (1 << 2),
    VSF_ETH_IRQ_MASK_SG_TX_COMPLETE  = (1 << 3),
    VSF_ETH_IRQ_MASK_PHY_LINK_CHANGE = (1 << 4),
    VSF_ETH_IRQ_MASK_ERROR           = (1 << 5),

    // Add more vendor-specific interrupt masks here
} vsf_eth_irq_mask_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_CTRL == ENABLED
typedef enum vsf_eth_ctrl_t {
    VSF_ETH_CTRL_GET_MAC_ADDRESS,

    // Add more vendor-specific control commands here
} vsf_eth_ctrl_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_eth_t vsf_eth_t;
typedef void vsf_eth_isr_handler_t(void *target_ptr, vsf_eth_t *eth_ptr,
                                   vsf_eth_irq_mask_t irq_mask);
typedef struct vsf_eth_isr_t {
    vsf_eth_isr_handler_t  *handler_fn;
    void                   *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_eth_isr_t;
typedef struct vsf_eth_cfg_t {
    vsf_eth_mode_t      mode;
    vsf_eth_phy_mode_t  phy_mode;
    uint8_t             mac_address[6];
    vsf_eth_isr_t       isr;
} vsf_eth_cfg_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_STATUS == ENABLED
typedef struct vsf_eth_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        uint32_t value;
        struct {
            uint32_t is_rx_available         : 1;
            uint32_t is_tx_ready             : 1;
        };
    };
} vsf_eth_status_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_eth_capability_t {
#if VSF_ETH_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif

    vsf_eth_mode_t      support_modes;
    vsf_eth_phy_mode_t  support_phy_modes;
    vsf_eth_irq_mask_t  support_irq_mask;

    union {
        uint32_t value;
        struct {
            uint32_t is_send_buf_releasable_immediately : 1;
            uint32_t is_sg_send_buf_releasable_immediately : 1;
        };
    };

    // Add more vendor-specific capabilities here
} vsf_eth_capability_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_MODE == ENABLED
typedef enum vsf_eth_buf_mode_t {
    VSF_ETH_BUF_MODE_TX_CHECKSUM_OFFLOAD = 1 << 0,
    VSF_ETH_BUF_MODE_RX_CHECKSUM_OFFLOAD = 1 << 1,

    // Add more vendor-specific buffer modes here
} vsf_eth_buf_mode_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_DESC == ENABLED
typedef struct vsf_eth_buf_desc_t {
    vsf_eth_buf_mode_t mode;
    void              *payload;
    uint32_t           size;
} vsf_eth_buf_desc_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_SEND_BUF_DESC == ENABLED
typedef vsf_eth_buf_desc_t vsf_eth_send_buf_desc_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_RECV_BUF_DESC == ENABLED
typedef vsf_eth_buf_desc_t vsf_eth_recv_buf_desc_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_SEND_BUF == DISABLED
typedef vsf_eth_buf_desc_t vsf_eth_send_sg_buf_desc_t;
#endif

#if VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_RECV_BUF == DISABLED
typedef vsf_eth_buf_desc_t vsf_eth_recv_sg_buf_desc_t;
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
#undef __VSF_HAL_${ETH_IP}_ETH_CLASS_IMPLEMENT
#undef __VSF_HAL_${ETH_IP}_ETH_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_ETH
#endif      // __HAL_DRIVER_${SERIES/ETH_IP}_ETH_H__
/* EOF */
