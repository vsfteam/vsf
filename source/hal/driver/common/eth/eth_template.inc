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

/*============================ ETHROS ========================================*/

#undef VSF_ETH_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_ETH_CFG_REIMPLEMENT_API_GET_CONFIGURATION

#undef vsf_real_eth_t
#undef vsf_real_eth_capability
#undef vsf_real_eth_get_configuration
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
        vsf_err_t vsf_imp_eth_get_configuration(vsf_imp_eth_t *eth_ptr, vsf_eth_cfg_t *cfg_ptr) \
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

#endif  /* VSF_HAL_USE_ETH */
