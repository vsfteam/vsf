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


/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_ETH == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${ETH_IP}_ETH_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${ETH_IP}_ETH_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${ETH_IP}_ETH_CLASS_IMPLEMENT
// IPCore end
// HW using ${ETH_IP} IPCore driver
#define __VSF_HAL_${ETH_IP}_ETH_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ ETHROS ========================================*/

/*\note VSF_HW_ETH_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${ETH_IP}_ETH_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_ETH_CFG_MULTI_CLASS
#   define VSF_HW_ETH_CFG_MULTI_CLASS           VSF_ETH_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_ETH_CFG_IMP_PREFIX                  vsf_hw
#define VSF_ETH_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end
// IPCore
#define VSF_ETH_CFG_IMP_PREFIX                  vsf_${eth_ip}
#define VSF_ETH_CFG_IMP_UPCASE_PREFIX           VSF_${ETH_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) {
#if VSF_HW_ETH_CFG_MULTI_CLASS == ENABLED
    vsf_eth_t               vsf_eth;
#endif
    void                    *reg;
    vsf_eth_isr_t           isr;
} VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t);
// HW end


/*============================ ETHROS ========================================*/
/*============================ ETHROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw eth only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_init)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    vsf_eth_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != eth_ptr) && (NULL != cfg_ptr));
    // configure according to cfg_ptr
    eth_ptr->isr = cfg_ptr->isr;
    // configure interrupt according to cfg_ptr->isr
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_fini)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr
) {
    VSF_HAL_ASSERT(eth_ptr != NULL);
}

vsf_err_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_get_configuration)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    vsf_eth_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != eth_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // TODO: Implement hardware-specific configuration reading
    // Read current ETH configuration from hardware registers

    // Template implementation returns default configuration
    cfg_ptr->isr = eth_ptr->isr;
    // Add other configuration fields as needed

    return VSF_ERR_NONE;
}

fsm_rt_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_enable)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr
) {
    VSF_HAL_ASSERT(eth_ptr != NULL);

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_disable)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr
) {
    VSF_HAL_ASSERT(eth_ptr != NULL);

    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_irq_enable)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    vsf_eth_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT((NULL != eth_ptr) && (NULL != eth_ptr->reg));
    // configure interrupt according to irq_mask
}

void VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_irq_disable)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    vsf_eth_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT((NULL != eth_ptr) && (NULL != eth_ptr->reg));
    // disable interrupt according to irq_mask
}

vsf_eth_capability_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_capability)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr
) {
    VSF_HAL_ASSERT(eth_ptr != NULL);

    return (vsf_eth_capability_t){
        .support_modes                          = 0,
        .support_phy_modes                      =
            VSF_ETH_PHY_MODE_SPEED_10M | VSF_ETH_PHY_MODE_SPEED_100M |
            VSF_ETH_PHY_MODE_SPEED_1000M | VSF_ETH_PHY_MODE_DUPLEX_HALF |
            VSF_ETH_PHY_MODE_DUPLEX_FULL,
        .support_irq_mask                       =
            VSF_ETH_IRQ_MASK_RX_AVAILABLE | VSF_ETH_IRQ_MASK_TX_COMPLETE |
            VSF_ETH_IRQ_MASK_SG_RX_AVAILABLE | VSF_ETH_IRQ_MASK_SG_TX_COMPLETE |
            VSF_ETH_IRQ_MASK_PHY_LINK_CHANGE | VSF_ETH_IRQ_MASK_ERROR,
        .is_send_buf_releasable_immediately     = 0,
        .is_sg_send_buf_releasable_immediately  = 0,
    };
}

vsf_eth_status_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_status)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr
) {
    vsf_eth_status_t status = {
        .is_rx_available = 0,
        .is_tx_ready = 0,
    };

    return status;
}

vsf_err_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_send_request)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    vsf_eth_send_buf_desc_t *buf_ptr
) {
    VSF_HAL_ASSERT((NULL != eth_ptr) && (NULL != buf_ptr));

    // send data according to buf_ptr
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_recv_request)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    vsf_eth_recv_buf_desc_t *buf_ptr
) {
    VSF_HAL_ASSERT((NULL != eth_ptr) && (NULL != buf_ptr));

    // receive data into buf_ptr
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_send_sg_request)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    vsf_eth_send_sg_buf_desc_t *buf_ptr,
    uint32_t sg_count
) {
    VSF_HAL_ASSERT((NULL != eth_ptr) && (NULL != buf_ptr));

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_recv_sg_request)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    vsf_eth_recv_sg_buf_desc_t *buf_ptr,
    uint32_t sg_count
) {
    VSF_HAL_ASSERT((NULL != eth_ptr) && (NULL != buf_ptr));

    return VSF_ERR_NONE;
}


static void VSF_MCONNECT(__, VSF_ETH_CFG_IMP_PREFIX, _eth_irqhandler)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr
) {
    VSF_HAL_ASSERT(NULL != eth_ptr);

    vsf_eth_isr_t *isr_ptr = &eth_ptr->isr;
    if (isr_ptr->handler_fn != NULL) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_eth_t *)eth_ptr, 0);
    }
}

vsf_err_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_ctrl)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    vsf_eth_ctrl_t ctrl,
    void *param
) {
    VSF_HAL_ASSERT((NULL != eth_ptr) && (NULL != param));

    switch (ctrl) {
    case VSF_ETH_CTRL_GET_MAC_ADDRESS:
        break;
    default:
        return VSF_ERR_NOT_SUPPORT;
    }

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_phy_read_reg)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    uint16_t reg_addr,
    uint32_t *value_ptr
) {
    VSF_HAL_ASSERT((NULL != eth_ptr) && (NULL != value_ptr));

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_phy_write_reg)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    uint16_t reg_addr,
    uint32_t value
) {
    VSF_HAL_ASSERT(NULL != eth_ptr);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_phy_set_mode)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    vsf_eth_phy_mode_t mode
) {
    VSF_HAL_ASSERT(NULL != eth_ptr);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_phy_get_link_status)(
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t) *eth_ptr,
    vsf_eth_phy_mode_t *phy_mode_ptr
) {
    VSF_HAL_ASSERT(NULL != eth_ptr);

    return VSF_ERR_NONE;
}
// HW end

/*============================ ETHROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw eth only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY      ENABLED
#define VSF_ETH_CFG_REIMPLEMENT_API_GET_CONFIGURATION ENABLED
#define VSF_ETH_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t)                                \
        VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth, __IDX) = {                   \
        .reg                = VSF_MCONNECT(VSF_ETH_CFG_IMP_UPCASE_PREFIX, _ETH, __IDX,_REG),\
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_ETH_CFG_IMP_UPCASE_PREFIX, _ETH, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_ETH_CFG_IMP_PREFIX, _eth_irqhandler)(              \
            &VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/eth/eth_template.inc"
// HW end

#endif /* VSF_HAL_USE_ETH */
