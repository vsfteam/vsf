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

#if VSF_HAL_USE_ETH == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ ETHROS ========================================*/

#define vsf_real_eth_t                      VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t)
#define vsf_real_eth_capability             VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_capability)
#define vsf_real_eth_get_configuration      VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_get_configuration)

#ifndef VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#ifndef VSF_ETH_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_ETH_CFG_REIMPLEMENT_API_GET_CONFIGURATION        DISABLED
#endif

#ifdef VSF_ETH_CFG_IMP_REMAP_PREFIX
#   undef VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY               ENABLED
#   undef VSF_ETH_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_ETH_CFG_REIMPLEMENT_API_GET_CONFIGURATION        ENABLED
#   undef VSF_ETH_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_ETH_CFG_REIMPLEMENT_API_IRQ_CLEAR                ENABLED
#endif

#if VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_eth_capability_t vsf_real_eth_capability(vsf_real_eth_t *eth_ptr)
{
    vsf_eth_capability_t eth_capability = {
        0
    };

    return eth_capability;
}
#endif

#if VSF_ETH_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_eth_get_configuration(vsf_real_eth_t *eth_ptr, vsf_eth_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != eth_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_ETH_CFG_REIMPLEMENT_API_IRQ_CLEAR == DISABLED
vsf_eth_irq_mask_t vsf_real_eth_irq_clear(vsf_real_eth_t *eth_ptr, vsf_eth_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != eth_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return 0;
}
#endif

/*============================ ETHROS ========================================*/

#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_DESC
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_BUF_MODE
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_CAPABILITY
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_CFG
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_CTRL
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_MODE
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_PHY_MODE
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_RECV_BUF_DESC
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_SEND_BUF_DESC
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_RECV_BUF
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_SG_SEND_BUF
#undef VSF_ETH_CFG_REIMPLEMENT_TYPE_STATUS
#undef VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_ETH_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_ETH_CFG_REIMPLEMENT_API_IRQ_CLEAR

#undef vsf_real_eth_t
#undef vsf_real_eth_capability
#undef vsf_real_eth_get_configuration
#undef vsf_real_eth_irq_clear
#undef vsf_real_eth_phy_read_reg
#undef vsf_real_eth_phy_write_reg
#undef vsf_real_eth_phy_set_mode
#undef vsf_real_eth_phy_get_link_status

/*============================ ETHROS ========================================*/

#ifdef VSF_ETH_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_eth_t                    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_t)
#   define vsf_imp_eth_init                 VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_init)
#   define vsf_imp_eth_fini                 VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_fini)
#   define vsf_imp_eth_enable               VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_enable)
#   define vsf_imp_eth_disable              VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_disable)
#   define vsf_imp_eth_capability           VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_capability)
#   define vsf_imp_eth_status               VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_status)
#   define vsf_imp_eth_send_request         VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_send_request)
#   define vsf_imp_eth_recv_request         VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_recv_request)
#   define vsf_imp_eth_send_sg_request      VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_send_sg_request)
#   define vsf_imp_eth_recv_sg_request      VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_recv_sg_request)
#   define vsf_imp_eth_ctrl                 VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_ctrl)
#   define vsf_imp_eth_phy_read_reg         VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_phy_read_reg)
#   define vsf_imp_eth_phy_write_reg        VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_phy_write_reg)
#   define vsf_imp_eth_phy_set_mode         VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_phy_set_mode)
#   define vsf_imp_eth_phy_get_link_status  VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_phy_get_link_status)
#   define vsf_imp_eth_get_configuration    VSF_MCONNECT(VSF_ETH_CFG_IMP_PREFIX, _eth_get_configuration)

#   define vsf_remap_eth_t                  VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_t)
#   define vsf_remap_eth_init               VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_init)
#   define vsf_remap_eth_fini               VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_fini)
#   define vsf_remap_eth_enable             VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_enable)
#   define vsf_remap_eth_disable            VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_disable)
#   define vsf_remap_eth_capability         VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_capability)
#   define vsf_remap_eth_status             VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_status)
#   define vsf_remap_eth_send_request       VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_send_request)
#   define vsf_remap_eth_recv_request       VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_recv_request)
#   define vsf_remap_eth_send_sg_request    VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_send_sg_request)
#   define vsf_remap_eth_recv_sg_request    VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_recv_sg_request)
#   define vsf_remap_eth_ctrl               VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_ctrl)
#   define vsf_remap_eth_phy_read_reg       VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_phy_read_reg)
#   define vsf_remap_eth_phy_write_reg      VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_phy_write_reg)
#   define vsf_remap_eth_phy_set_mode       VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_phy_set_mode)
#   define vsf_remap_eth_phy_get_link_status VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_phy_get_link_status)
#   define vsf_remap_eth_get_configuration  VSF_MCONNECT(VSF_ETH_CFG_IMP_REMAP_PREFIX, _eth_get_configuration)

#   define VSF_ETH_CFG_IMP_REMAP_FUNCTIONS                                          \
        vsf_err_t vsf_imp_eth_init(vsf_imp_eth_t *eth_ptr, vsf_eth_cfg_t *cfg_ptr)  \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_init(eth_ptr, cfg_ptr);                            \
        }                                                                           \
        void vsf_imp_eth_fini(vsf_imp_eth_t *eth_ptr)                               \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            vsf_remap_eth_fini(eth_ptr);                                            \
        }                                                                           \
        fsm_rt_t vsf_imp_eth_enable(vsf_imp_eth_t *eth_ptr)                         \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_enable(eth_ptr);                                   \
        }                                                                           \
        fsm_rt_t vsf_imp_eth_disable(vsf_imp_eth_t *eth_ptr)                        \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_disable(eth_ptr);                                  \
        }                                                                           \
        vsf_eth_status_t vsf_imp_eth_status(vsf_imp_eth_t *eth_ptr)                 \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_status(eth_ptr);                                   \
        }                                                                           \
        vsf_eth_capability_t vsf_imp_eth_capability(vsf_imp_eth_t *eth_ptr)         \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_capability(eth_ptr);                               \
        }                                                                           \
        vsf_err_t vsf_imp_eth_send_request(vsf_imp_eth_t *eth_ptr,                  \
            vsf_eth_send_buf_desc_t *buf_ptr)                                       \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_send_request(eth_ptr, buf_ptr);                    \
        }                                                                           \
        vsf_err_t vsf_imp_eth_recv_request(vsf_imp_eth_t *eth_ptr,                  \
            vsf_eth_recv_buf_desc_t *buf_ptr)                                       \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_recv_request(eth_ptr, buf_ptr);                    \
        }                                                                           \
        vsf_err_t vsf_imp_eth_send_sg_request(vsf_imp_eth_t *eth_ptr,               \
            vsf_eth_send_sg_buf_desc_t *buf_ptr, uint32_t sg_count)                 \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_send_sg_request(eth_ptr, buf_ptr, sg_count);       \
        }                                                                           \
        vsf_err_t vsf_imp_eth_recv_sg_request(vsf_imp_eth_t *eth_ptr,               \
            vsf_eth_recv_sg_buf_desc_t *buf_ptr, uint32_t sg_count)                 \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_recv_sg_request(eth_ptr, buf_ptr, sg_count);       \
        }                                                                           \
        vsf_err_t vsf_imp_eth_ctrl(vsf_imp_eth_t *eth_ptr,                          \
            vsf_eth_ctrl_t ctrl, void *param)                                       \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_ctrl(eth_ptr, ctrl, param);                        \
        }                                                                           \
        vsf_eth_capability_t vsf_imp_eth_capability(vsf_imp_eth_t *eth_ptr)         \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_capability(eth_ptr);                               \
        }                                                                           \
        vsf_err_t vsf_imp_eth_phy_read_reg(vsf_imp_eth_t *eth_ptr,                  \
            uint16_t reg_addr, uint32_t *value_ptr)                                 \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_phy_read_reg(eth_ptr, reg_addr, value_ptr);        \
        }                                                                           \
        vsf_err_t vsf_imp_eth_phy_write_reg(vsf_imp_eth_t *eth_ptr,                 \
            uint16_t reg_addr, uint32_t value)                                      \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_phy_write_reg(eth_ptr, reg_addr, value);           \
        }                                                                           \
        vsf_err_t vsf_imp_eth_phy_set_mode(vsf_imp_eth_t *eth_ptr,            \
            vsf_eth_mode_t mode)                                                    \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_phy_set_mode(eth_ptr, mode);                 \
        }                                                                           \
        vsf_err_t vsf_imp_eth_phy_get_link_status(vsf_imp_eth_t *eth_ptr,           \
            vsf_eth_mode_t * phy_mode_ptr)                                          \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_phy_get_link_status(eth_ptr, phy_mode_ptr);        \
        }                                                                           \
                                                                                    \
        vsf_err_t vsf_imp_eth_get_configuration(vsf_imp_eth_t *eth_ptr,             \
                                                vsf_eth_cfg_t *cfg_ptr)             \
        {                                                                           \
            VSF_HAL_ASSERT(eth_ptr != NULL);                                        \
            return vsf_remap_eth_get_configuration(eth_ptr, cfg_ptr);               \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _eth
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _ETH

#if !defined(VSF_ETH_CFG_IMP_PREFIX) && !defined(VSF_ETH_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_ETH_CFG_IMP_PREFIX in eth driver"
#endif

#if !defined(VSF_ETH_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_ETH_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_ETH_CFG_IMP_UPCASE_PREFIX in eth driver"
#endif

#ifndef VSF_ETH_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_ETH_CFG_IMP_COUNT_MASK_PREFIX        VSF_ETH_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_ETH_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_ETH_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_ETH_CFG_IMP_PREFIX
#undef VSF_ETH_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_ETH_CFG_IMP_UPCASE_PREFIX
#undef VSF_ETH_CFG_IMP_DEVICE_PREFIX
#undef VSF_ETH_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_ETH_CFG_IMP_LV0
#undef VSF_ETH_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_ETH_CFG_IMP_HAS_OP
#undef VSF_ETH_CFG_IMP_EXTERN_OP

#undef vsf_imp_eth_t
#undef vsf_imp_eth_init
#undef vsf_imp_eth_fini
#undef vsf_imp_eth_enable
#undef vsf_imp_eth_disable
#undef vsf_imp_eth_status
#undef vsf_imp_eth_capability
#undef vsf_imp_eth_send_request
#undef vsf_imp_eth_recv_request
#undef vsf_imp_eth_send_sg_request
#undef vsf_imp_eth_recv_sg_request
#undef vsf_imp_eth_ctrl
#undef vsf_imp_eth_phy_read_reg
#undef vsf_imp_eth_phy_write_reg
#undef vsf_imp_eth_phy_set_mode
#undef vsf_imp_eth_phy_get_link_status
#undef vsf_imp_eth_get_configuration

#undef vsf_remap_eth_t
#undef vsf_remap_eth_init
#undef vsf_remap_eth_fini
#undef vsf_remap_eth_enable
#undef vsf_remap_eth_disable
#undef vsf_remap_eth_status
#undef vsf_remap_eth_capability
#undef vsf_remap_eth_send_request
#undef vsf_remap_eth_recv_request
#undef vsf_remap_eth_send_sg_request
#undef vsf_remap_eth_recv_sg_request
#undef vsf_remap_eth_ctrl
#undef vsf_remap_eth_phy_read_reg
#undef vsf_remap_eth_phy_write_reg
#undef vsf_remap_eth_phy_set_mode
#undef vsf_remap_eth_phy_get_link_status
#undef vsf_remap_eth_get_configuration

#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME

/*============================ STATIC ASSERTIONS ==============================*/

/* User-extensible macros for custom mode and IRQ mask values
 * Users can define these macros before including this template to append
 * their custom enum values to the uniqueness checks.
 *
 * Check modes for custom values:
 *   VSF_ETH_CFG_MODE_CHECK_UNIQUE - Check mode for custom mode values
 *     Default: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *   VSF_ETH_CFG_IRQ_MASK_CHECK_UNIQUE - Check mode for custom IRQ mask values
 *     Default: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *
 * Example usage in vendor driver:
 *   #define VSF_ETH_CUSTOM_MODE_VALUES           MY_CUSTOM_MODE1, MY_CUSTOM_MODE2
 *   #define VSF_ETH_CFG_MODE_CHECK_UNIQUE        VSF_HAL_CHECK_MODE_LOOSE
 *   #define VSF_ETH_CUSTOM_IRQ_MASK_VALUES       MY_CUSTOM_IRQ1, MY_CUSTOM_IRQ2
 *   #define VSF_ETH_CFG_IRQ_MASK_CHECK_UNIQUE    VSF_HAL_CHECK_MODE_STRICT
 *   #include "hal/driver/common/eth/eth_template.inc"
 */


#ifdef VSF_ETH_CFG_MODE_CHECK_UNIQUE
/* Default mode lists - can be redefined by users before including this file */
#ifndef VSF_ETH_CHECK_UNIQUE_MODE_OFFLOAD_MODES
#   define VSF_ETH_CHECK_UNIQUE_MODE_OFFLOAD_MODES \
        VSF_ETH_MODE_TX_CHECKSUM_OFFLOAD, \
        VSF_ETH_MODE_RX_CHECKSUM_OFFLOAD
#endif

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check ETH offload mode uniqueness - all offload modes should have different values
 * Mandatory: VSF_ETH_MODE_TX_CHECKSUM_OFFLOAD, VSF_ETH_MODE_RX_CHECKSUM_OFFLOAD
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_ETH_CFG_MODE_CHECK_UNIQUE, VSF_ETH_CHECK_UNIQUE_MODE_OFFLOAD_MODES);

#ifdef VSF_ETH_CUSTOM_MODE_VALUES
/* Check uniqueness among custom mode values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_ETH_CFG_MODE_CHECK_UNIQUE, VSF_ETH_CUSTOM_MODE_VALUES);
#endif

VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_ETH_CFG_MODE_CHECK_UNIQUE,
    /* Mandatory modes - always included */
    VSF_ETH_MODE_TX_CHECKSUM_OFFLOAD, VSF_ETH_MODE_RX_CHECKSUM_OFFLOAD
    /* Standard Optional modes - conditionally included */
#if __VSF_ETH_CFG_SUPPORT_STANDARD_OPTIONAL
#ifdef VSF_ETH_MODE_UNICAST_FILTER
    , VSF_ETH_MODE_UNICAST_FILTER
#endif
#ifdef VSF_ETH_MODE_MULTICAST_FILTER
    , VSF_ETH_MODE_MULTICAST_FILTER
#endif
#ifdef VSF_ETH_MODE_BROADCAST_FILTER
    , VSF_ETH_MODE_BROADCAST_FILTER
#endif
#ifdef VSF_ETH_MODE_VLAN_TAGGING
    , VSF_ETH_MODE_VLAN_TAGGING
#endif
#ifdef VSF_ETH_MODE_VLAN_FILTERING
    , VSF_ETH_MODE_VLAN_FILTERING
#endif
#ifdef VSF_ETH_MODE_VLAN_STRIPPING
    , VSF_ETH_MODE_VLAN_STRIPPING
#endif
#ifdef VSF_ETH_MODE_VLAN_INSERTION
    , VSF_ETH_MODE_VLAN_INSERTION
#endif
#ifdef VSF_ETH_MODE_JUMBO_FRAME
    , VSF_ETH_MODE_JUMBO_FRAME
#endif
#ifdef VSF_ETH_MODE_FLOW_CONTROL
    , VSF_ETH_MODE_FLOW_CONTROL
#endif
#ifdef VSF_ETH_MODE_PAUSE_FRAME
    , VSF_ETH_MODE_PAUSE_FRAME
#endif
#ifdef VSF_ETH_MODE_PRIORITY_QUEUE
    , VSF_ETH_MODE_PRIORITY_QUEUE
#endif
#ifdef VSF_ETH_MODE_QOS
    , VSF_ETH_MODE_QOS
#endif
#ifdef VSF_ETH_MODE_PTP_TIMESTAMP
    , VSF_ETH_MODE_PTP_TIMESTAMP
#endif
#ifdef VSF_ETH_MODE_TCP_SEGMENTATION
    , VSF_ETH_MODE_TCP_SEGMENTATION
#endif
#ifdef VSF_ETH_MODE_LARGE_RECEIVE
    , VSF_ETH_MODE_LARGE_RECEIVE
#endif
#endif
    /* User-defined modes - appended by user configuration */
#ifdef VSF_ETH_CUSTOM_MODE_VALUES
    , VSF_ETH_CUSTOM_MODE_VALUES
#endif
);

#undef VSF_ETH_CHECK_UNIQUE_MODE_OFFLOAD_MODES
#endif  /* VSF_ETH_CFG_MODE_CHECK_UNIQUE */

/* ==================== IRQ MASK UNIQUENESS CHECKS ==================== */

#ifdef VSF_ETH_CFG_IRQ_MASK_CHECK_UNIQUE

#ifdef VSF_ETH_CUSTOM_IRQ_MASK_VALUES
/* Check uniqueness among custom IRQ mask values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_ETH_CFG_IRQ_MASK_CHECK_UNIQUE, VSF_ETH_CUSTOM_IRQ_MASK_VALUES);
#endif

/* Check uniqueness within vsf_eth_irq_mask_t enum */

/* Check IRQ mask uniqueness - all IRQ mask bits should have different values
 * Mandatory: VSF_ETH_IRQ_MASK_RX_AVAILABLE, VSF_ETH_IRQ_MASK_TX_COMPLETE,
 *            VSF_ETH_IRQ_MASK_SG_RX_AVAILABLE, VSF_ETH_IRQ_MASK_SG_TX_COMPLETE,
 *            VSF_ETH_IRQ_MASK_PHY_LINK_CHANGE, VSF_ETH_IRQ_MASK_ERROR
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_ETH_CFG_IRQ_MASK_CHECK_UNIQUE,
    /* Mandatory IRQ masks - always included */
    VSF_ETH_IRQ_MASK_RX_AVAILABLE,
    VSF_ETH_IRQ_MASK_TX_COMPLETE,
    VSF_ETH_IRQ_MASK_SG_RX_AVAILABLE,
    VSF_ETH_IRQ_MASK_SG_TX_COMPLETE,
    VSF_ETH_IRQ_MASK_PHY_LINK_CHANGE,
    VSF_ETH_IRQ_MASK_ERROR
    /* User-defined IRQ masks - appended by user configuration */
#ifdef VSF_ETH_CUSTOM_IRQ_MASK_VALUES
    , VSF_ETH_CUSTOM_IRQ_MASK_VALUES
#endif
);
#endif  /* VSF_ETH_CFG_IRQ_MASK_CHECK_UNIQUE */

#undef VSF_ETH_CFG_MODE_CHECK_UNIQUE
#undef VSF_ETH_CFG_IRQ_MASK_CHECK_UNIQUE
#undef VSF_ETH_CUSTOM_MODE_VALUES
#undef VSF_ETH_CUSTOM_IRQ_MASK_VALUES

/* ==================== PHY MODE UNIQUENESS CHECKS ==================== */

#ifdef VSF_ETH_CFG_PHY_MODE_CHECK_UNIQUE
/* Default PHY mode lists - can be redefined by users before including this file */
#ifndef VSF_ETH_CHECK_UNIQUE_PHY_MODE_SPEED_MODES
#   define VSF_ETH_CHECK_UNIQUE_PHY_MODE_SPEED_MODES \
        VSF_ETH_PHY_MODE_SPEED_10M, \
        VSF_ETH_PHY_MODE_SPEED_100M, \
        VSF_ETH_PHY_MODE_SPEED_1000M
#endif

#ifndef VSF_ETH_CHECK_UNIQUE_PHY_MODE_DUPLEX_MODES
#   define VSF_ETH_CHECK_UNIQUE_PHY_MODE_DUPLEX_MODES \
        VSF_ETH_PHY_MODE_DUPLEX_HALF, \
        VSF_ETH_PHY_MODE_DUPLEX_FULL
#endif

#ifndef VSF_ETH_CHECK_UNIQUE_PHY_MODE_LINK_MODES
#   define VSF_ETH_CHECK_UNIQUE_PHY_MODE_LINK_MODES \
        VSF_ETH_PHY_MODE_LINK_UP
#endif

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check PHY speed mode uniqueness - all speed modes should have different values
 * Mandatory: VSF_ETH_PHY_MODE_SPEED_10M, VSF_ETH_PHY_MODE_SPEED_100M, VSF_ETH_PHY_MODE_SPEED_1000M
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_ETH_CFG_PHY_MODE_CHECK_UNIQUE, VSF_ETH_CHECK_UNIQUE_PHY_MODE_SPEED_MODES);

/* Check PHY duplex mode uniqueness - all duplex modes should have different values
 * Mandatory: VSF_ETH_PHY_MODE_DUPLEX_HALF, VSF_ETH_PHY_MODE_DUPLEX_FULL
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_ETH_CFG_PHY_MODE_CHECK_UNIQUE, VSF_ETH_CHECK_UNIQUE_PHY_MODE_DUPLEX_MODES);

/* Check PHY link mode uniqueness - all link modes should have different values
 * Mandatory: VSF_ETH_PHY_MODE_LINK_UP
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_ETH_CFG_PHY_MODE_CHECK_UNIQUE, VSF_ETH_CHECK_UNIQUE_PHY_MODE_LINK_MODES);

#ifdef VSF_ETH_CUSTOM_PHY_MODE_VALUES
/* Check uniqueness among custom PHY mode values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_ETH_CFG_PHY_MODE_CHECK_UNIQUE, VSF_ETH_CUSTOM_PHY_MODE_VALUES);
#endif

VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_ETH_CFG_PHY_MODE_CHECK_UNIQUE,
    /* Mandatory PHY modes - always included */
    VSF_ETH_PHY_MODE_SPEED_10M, VSF_ETH_PHY_MODE_SPEED_100M, VSF_ETH_PHY_MODE_SPEED_1000M,
    VSF_ETH_PHY_MODE_DUPLEX_HALF, VSF_ETH_PHY_MODE_DUPLEX_FULL,
    VSF_ETH_PHY_MODE_LINK_UP
    /* User-defined PHY modes - appended by user configuration */
#ifdef VSF_ETH_CUSTOM_PHY_MODE_VALUES
    , VSF_ETH_CUSTOM_PHY_MODE_VALUES
#endif
);

#undef VSF_ETH_CHECK_UNIQUE_PHY_MODE_SPEED_MODES
#undef VSF_ETH_CHECK_UNIQUE_PHY_MODE_DUPLEX_MODES
#undef VSF_ETH_CHECK_UNIQUE_PHY_MODE_LINK_MODES
#endif  /* VSF_ETH_CFG_PHY_MODE_CHECK_UNIQUE */

/* ==================== BUF MODE UNIQUENESS CHECKS ==================== */

#ifdef VSF_ETH_CFG_BUF_MODE_CHECK_UNIQUE
/* Default buf mode lists - can be redefined by users before including this file */

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check buf mode uniqueness - all buf modes should have different values
 * Mandatory: VSF_ETH_BUF_MODE_TX_CHECKSUM_OFFLOAD, VSF_ETH_BUF_MODE_RX_CHECKSUM_OFFLOAD
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_ETH_CFG_BUF_MODE_CHECK_UNIQUE,
    VSF_ETH_BUF_MODE_TX_CHECKSUM_OFFLOAD,
    VSF_ETH_BUF_MODE_RX_CHECKSUM_OFFLOAD
);

#ifdef VSF_ETH_CUSTOM_BUF_MODE_VALUES
/* Check uniqueness among custom buf mode values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_ETH_CFG_BUF_MODE_CHECK_UNIQUE, VSF_ETH_CUSTOM_BUF_MODE_VALUES);
#endif

VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_ETH_CFG_BUF_MODE_CHECK_UNIQUE,
    /* Mandatory buf modes - always included */
    VSF_ETH_BUF_MODE_TX_CHECKSUM_OFFLOAD, VSF_ETH_BUF_MODE_RX_CHECKSUM_OFFLOAD
    /* User-defined buf modes - appended by user configuration */
#ifdef VSF_ETH_CUSTOM_BUF_MODE_VALUES
    , VSF_ETH_CUSTOM_BUF_MODE_VALUES
#endif
);
#endif  /* VSF_ETH_CFG_BUF_MODE_CHECK_UNIQUE */

/* ==================== CTRL UNIQUENESS CHECKS ==================== */

#ifdef VSF_ETH_CFG_CTRL_CHECK_UNIQUE

#ifdef VSF_ETH_CUSTOM_CTRL_VALUES
/* Check uniqueness among custom ctrl values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_ETH_CFG_CTRL_CHECK_UNIQUE, VSF_ETH_CUSTOM_CTRL_VALUES);
#endif

/* Check ctrl uniqueness - all ctrl values should have different values
 * Mandatory: VSF_ETH_CTRL_GET_MAC_ADDRESS
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_ETH_CFG_CTRL_CHECK_UNIQUE,
    /* Mandatory ctrls - always included */
    VSF_ETH_CTRL_GET_MAC_ADDRESS
    /* User-defined ctrls - appended by user configuration */
#ifdef VSF_ETH_CUSTOM_CTRL_VALUES
    , VSF_ETH_CUSTOM_CTRL_VALUES
#endif
);
#endif  /* VSF_ETH_CFG_CTRL_CHECK_UNIQUE */

#undef VSF_ETH_CFG_PHY_MODE_CHECK_UNIQUE
#undef VSF_ETH_CFG_BUF_MODE_CHECK_UNIQUE
#undef VSF_ETH_CFG_CTRL_CHECK_UNIQUE
#undef VSF_ETH_CUSTOM_PHY_MODE_VALUES
#undef VSF_ETH_CUSTOM_BUF_MODE_VALUES
#undef VSF_ETH_CUSTOM_CTRL_VALUES

#endif  /* VSF_HAL_USE_ETH */
