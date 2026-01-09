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

#if VSF_HAL_USE_I2S == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define vsf_real_i2s_t                                      VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t)
#define vsf_real_i2s_capability                             VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_capability)
#define vsf_real_i2s_get_configuration                      VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_get_configuration)
#define vsf_real_i2s_tx_get_configuration                   VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_tx_get_configuration)
#define vsf_real_i2s_rx_get_configuration                   VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_rx_get_configuration)

#ifndef VSF_I2S_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_I2S_CFG_REIMPLEMENT_API_GET_CONFIGURATION    DISABLED
#endif

#ifndef VSF_I2S_CFG_REIMPLEMENT_API_TX_GET_CONFIGURATION
#   define VSF_I2S_CFG_REIMPLEMENT_API_TX_GET_CONFIGURATION DISABLED
#endif

#ifndef VSF_I2S_CFG_REIMPLEMENT_API_RX_GET_CONFIGURATION
#   define VSF_I2S_CFG_REIMPLEMENT_API_RX_GET_CONFIGURATION DISABLED
#endif

#ifndef VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY          DISABLED
#endif

#ifdef VSF_I2S_CFG_IMP_REMAP_PREFIX
#   undef VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY           ENABLED
#   undef VSF_I2S_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_I2S_CFG_REIMPLEMENT_API_GET_CONFIGURATION    ENABLED
#   undef VSF_I2S_CFG_REIMPLEMENT_API_TX_GET_CONFIGURATION
#   define VSF_I2S_CFG_REIMPLEMENT_API_TX_GET_CONFIGURATION ENABLED
#   undef VSF_I2S_CFG_REIMPLEMENT_API_RX_GET_CONFIGURATION
#   define VSF_I2S_CFG_REIMPLEMENT_API_RX_GET_CONFIGURATION ENABLED
#endif

#if VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_I2S_CFG_CAPABILITY_IS_SRC_SUPPORTED
#       define VSF_I2S_CFG_CAPABILITY_IS_SRC_SUPPORTED      1
#   endif
#   ifndef VSF_I2S_CFG_CAPABILITY_IS_DBUFFER_SUPPORTED
#       define VSF_I2S_CFG_CAPABILITY_IS_DBUFFER_SUPPORTED  1
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_i2s_capability_t vsf_real_i2s_capability(vsf_real_i2s_t *i2s_ptr)
{
    vsf_i2s_capability_t i2s_capability = {
        .i2s_capability = {
            .is_src_supported       = VSF_I2S_CFG_CAPABILITY_IS_SRC_SUPPORTED
            .is_dbuffer_supported   = VSF_I2S_CFG_CAPABILITY_IS_DBUFFER_SUPPORTED
        }
    };

    return i2s_capability;
}
#endif

#if VSF_I2S_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_i2s_get_configuration(vsf_real_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != i2s_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(0); // Default implementation: not supported, trigger assertion
    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_I2S_CFG_REIMPLEMENT_API_TX_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_i2s_tx_get_configuration(vsf_real_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != i2s_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(0); // Default implementation: not supported, trigger assertion
    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_I2S_CFG_REIMPLEMENT_API_RX_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_i2s_rx_get_configuration(vsf_real_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != i2s_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(0); // Default implementation: not supported, trigger assertion
    return VSF_ERR_NOT_SUPPORT;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_I2S_CFG_REIMPLEMENT_TYPE_CAPABILITY
#undef VSF_I2S_CFG_REIMPLEMENT_TYPE_CFG
#undef VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#undef VSF_I2S_CFG_REIMPLEMENT_TYPE_MODE
#undef VSF_I2S_CFG_REIMPLEMENT_TYPE_STATUS
#undef VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_I2S_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_I2S_CFG_REIMPLEMENT_API_TX_GET_CONFIGURATION
#undef VSF_I2S_CFG_REIMPLEMENT_API_RX_GET_CONFIGURATION
#undef VSF_I2S_CFG_CAPABILITY_IS_SRC_SUPPORTED
#undef VSF_I2S_CFG_CAPABILITY_IS_DBUFFER_SUPPORTED

#undef vsf_real_i2s_t
#undef vsf_real_i2s_capability
#undef vsf_real_i2s_get_configuration
#undef vsf_real_i2s_tx_get_configuration
#undef vsf_real_i2s_rx_get_configuration

/*============================ MACROS ========================================*/

#ifdef VSF_I2S_CFG_IMP_REMAP_PREFIX
#   define vsf_imp_i2s_t                     VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t)
#   define vsf_imp_i2s_init                  VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_init)
#   define vsf_imp_i2s_enable                VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_enable)
#   define vsf_imp_i2s_disable               VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_disable)
#   define vsf_imp_i2s_status                VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_status)
#   define vsf_imp_i2s_capability            VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_capability)
#   define vsf_imp_i2s_tx_init               VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_tx_init)
#   define vsf_imp_i2s_tx_fini               VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_tx_fini)
#   define vsf_imp_i2s_tx_get_configuration  VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_tx_get_configuration)
#   define vsf_imp_i2s_tx_start              VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_tx_start)
#   define vsf_imp_i2s_rx_init               VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_rx_init)
#   define vsf_imp_i2s_rx_fini               VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_rx_fini)
#   define vsf_imp_i2s_rx_get_configuration  VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_rx_get_configuration)
#   define vsf_imp_i2s_rx_start              VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_rx_start)
#   define vsf_imp_i2s_get_configuration     VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_get_configuration)

#   define vsf_remap_i2s_t                   VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_t)
#   define vsf_remap_i2s_init                VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_init)
#   define vsf_remap_i2s_enable              VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_enable)
#   define vsf_remap_i2s_disable             VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_disable)
#   define vsf_remap_i2s_status              VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_status)
#   define vsf_remap_i2s_capability          VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_capability)
#   define vsf_remap_i2s_tx_init             VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_tx_init)
#   define vsf_remap_i2s_tx_fini             VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_tx_fini)
#   define vsf_remap_i2s_tx_get_configuration VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_tx_get_configuration)
#   define vsf_remap_i2s_tx_start            VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_tx_start)
#   define vsf_remap_i2s_rx_init             VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_rx_init)
#   define vsf_remap_i2s_rx_fini             VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_rx_fini)
#   define vsf_remap_i2s_rx_get_configuration VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_rx_get_configuration)
#   define vsf_remap_i2s_rx_start            VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_rx_start)
#   define vsf_remap_i2s_get_configuration   VSF_MCONNECT(VSF_I2S_CFG_IMP_REMAP_PREFIX, _i2s_get_configuration)

#   define VSF_I2S_CFG_IMP_REMAP_FUNCTIONS                                                                \
        vsf_err_t vsf_imp_i2s_init(vsf_imp_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)          \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            return vsf_remap_i2s_init(i2s_ptr, cfg_ptr);                                    \
        }                                                                                   \
        vsf_err_t vsf_imp_i2s_tx_init(vsf_imp_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)       \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            return vsf_remap_i2s_tx_init(i2s_ptr, cfg_ptr);                                 \
        }                                                                                   \
        void vsf_imp_i2s_tx_fini(vsf_imp_i2s_t *i2s_ptr)                                    \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            vsf_remap_i2s_tx_fini(i2s_ptr);                                                 \
        }                                                                                   \
        vsf_err_t vsf_imp_i2s_tx_get_configuration(vsf_imp_i2s_t *i2s_ptr,                  \
                                                   vsf_i2s_cfg_t *cfg_ptr)                  \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            VSF_HAL_ASSERT(cfg_ptr != NULL);                                                \
            return vsf_remap_i2s_tx_get_configuration(i2s_ptr, cfg_ptr);                    \
        }                                                                                   \
        vsf_err_t vsf_imp_i2s_tx_start(vsf_imp_i2s_t *i2s_ptr)                              \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            return vsf_remap_i2s_tx_start(i2s_ptr);                                         \
        }                                                                                   \
        vsf_err_t vsf_imp_i2s_rx_init(vsf_imp_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)       \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            return vsf_remap_i2s_rx_init(i2s_ptr, cfg_ptr);                                 \
        }                                                                                   \
        void vsf_imp_i2s_rx_fini(vsf_imp_i2s_t *i2s_ptr)                                    \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            vsf_remap_i2s_rx_fini(i2s_ptr);                                                 \
        }                                                                                   \
        vsf_err_t vsf_imp_i2s_rx_get_configuration(vsf_imp_i2s_t *i2s_ptr,                  \
                                                   vsf_i2s_cfg_t *cfg_ptr)                  \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            VSF_HAL_ASSERT(cfg_ptr != NULL);                                                \
            return vsf_remap_i2s_rx_get_configuration(i2s_ptr, cfg_ptr);                    \
        }                                                                                   \
        vsf_err_t vsf_imp_i2s_rx_start(vsf_imp_i2s_t *i2s_ptr)                              \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            return vsf_remap_i2s_rx_start(i2s_ptr);                                         \
        }                                                                                   \
        fsm_rt_t vsf_imp_i2s_enable(vsf_imp_i2s_t *i2s_ptr)                                 \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            return vsf_remap_i2s_enable(i2s_ptr);                                           \
        }                                                                                   \
        fsm_rt_t vsf_imp_i2s_disable(vsf_imp_i2s_t *i2s_ptr)                                \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            return vsf_remap_i2s_disable(i2s_ptr);                                          \
        }                                                                                   \
        vsf_i2s_status_t vsf_imp_i2s_status(vsf_imp_i2s_t *i2s_ptr)                         \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            return vsf_remap_i2s_status(i2s_ptr);                                           \
        }                                                                                   \
        vsf_i2s_capability_t vsf_imp_i2s_capability(vsf_imp_i2s_t *i2s_ptr)                 \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            return vsf_remap_i2s_capability(i2s_ptr);                                       \
        }                                                                                   \
        vsf_err_t vsf_imp_i2s_get_configuration(vsf_imp_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr) \
        {                                                                                   \
            VSF_HAL_ASSERT(i2s_ptr != NULL);                                                \
            return vsf_remap_i2s_get_configuration(i2s_ptr, cfg_ptr);                       \
        }
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _i2s
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _I2S

#if !defined(VSF_I2S_CFG_IMP_PREFIX) && !defined(VSF_I2S_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_I2S_CFG_IMP_PREFIX in i2s driver"
#endif

#if !defined(VSF_I2S_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_I2S_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_I2S_CFG_IMP_UPCASE_PREFIX in i2s driver"
#endif

#ifndef VSF_I2S_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_I2S_CFG_IMP_COUNT_MASK_PREFIX        VSF_I2S_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_I2S_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_I2S_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_I2S_CFG_IMP_PREFIX
#undef VSF_I2S_CFG_IMP_UPCASE_PREFIX
#undef VSF_I2S_CFG_IMP_DEVICE_PREFIX
#undef VSF_I2S_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_I2S_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_I2S_CFG_IMP_LV0
#undef VSF_I2S_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_I2S_CFG_IMP_HAS_OP
#undef VSF_I2S_CFG_IMP_EXTERN_OP

#undef vsf_imp_i2s_t
#undef vsf_imp_i2s_init
#undef vsf_imp_i2s_enable
#undef vsf_imp_i2s_disable
#undef vsf_imp_i2s_status
#undef vsf_imp_i2s_capability
#undef vsf_imp_i2s_tx_init
#undef vsf_imp_i2s_tx_fini
#undef vsf_imp_i2s_tx_get_configuration
#undef vsf_imp_i2s_tx_start
#undef vsf_imp_i2s_rx_init
#undef vsf_imp_i2s_rx_fini
#undef vsf_imp_i2s_rx_get_configuration
#undef vsf_imp_i2s_rx_start
#undef vsf_imp_i2s_get_configuration

#undef vsf_remap_i2s_t
#undef vsf_remap_i2s_init
#undef vsf_remap_i2s_enable
#undef vsf_remap_i2s_disable
#undef vsf_remap_i2s_status
#undef vsf_remap_i2s_capability
#undef vsf_remap_i2s_tx_init
#undef vsf_remap_i2s_tx_fini
#undef vsf_remap_i2s_tx_get_configuration
#undef vsf_remap_i2s_tx_start
#undef vsf_remap_i2s_rx_init
#undef vsf_remap_i2s_rx_fini
#undef vsf_remap_i2s_rx_get_configuration
#undef vsf_remap_i2s_rx_start
#undef vsf_remap_i2s_get_configuration

#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME

/*============================ STATIC ASSERTIONS ==============================*/

/* User-extensible macros for custom mode and IRQ mask values
 * Users can define these macros before including this template to append
 * their custom enum values to the uniqueness checks.
 *
 * Check modes for custom values:
 *   VSF_I2S_CFG_MODE_CHECK_UNIQUE - Check mode for custom mode values
 *     Default: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *   VSF_I2S_CFG_IRQ_MASK_CHECK_UNIQUE - Check mode for custom IRQ mask values
 *     Default: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *
 * Example usage in vendor driver:
 *   #define VSF_I2S_CUSTOM_MODE_VALUES           MY_CUSTOM_MODE1, MY_CUSTOM_MODE2
 *   #define VSF_I2S_CFG_MODE_CHECK_UNIQUE        VSF_HAL_CHECK_MODE_LOOSE
 *   #define VSF_I2S_CUSTOM_IRQ_MASK_VALUES       MY_CUSTOM_IRQ1, MY_CUSTOM_IRQ2
 *   #define VSF_I2S_CFG_IRQ_MASK_CHECK_UNIQUE    VSF_HAL_CHECK_MODE_STRICT
 *   #include "hal/driver/common/i2s/i2s_template.inc"
 */


#ifdef VSF_I2S_CFG_MODE_CHECK_UNIQUE
/* Default mode lists - can be redefined by users before including this file */
#ifndef VSF_I2S_CHECK_UNIQUE_MODE_MODES
#   define VSF_I2S_CHECK_UNIQUE_MODE_MODES \
        VSF_I2S_MODE_MASTER, \
        VSF_I2S_MODE_SLAVE
#endif

#ifndef VSF_I2S_CHECK_UNIQUE_DATA_BITLEN_MODES
#   define VSF_I2S_CHECK_UNIQUE_DATA_BITLEN_MODES \
        VSF_I2S_DATA_BITLEN_16, \
        VSF_I2S_DATA_BITLEN_24, \
        VSF_I2S_DATA_BITLEN_32
#endif

#ifndef VSF_I2S_CHECK_UNIQUE_FRAME_BITLEN_MODES
#   define VSF_I2S_CHECK_UNIQUE_FRAME_BITLEN_MODES \
        VSF_I2S_FRAME_BITLEN_16, \
        VSF_I2S_FRAME_BITLEN_24, \
        VSF_I2S_FRAME_BITLEN_32
#endif

#ifndef VSF_I2S_CHECK_UNIQUE_STANDARD_MODES
#   define VSF_I2S_CHECK_UNIQUE_STANDARD_MODES \
        VSF_I2S_STANDARD_PHILIPS, \
        VSF_I2S_STANDARD_MSB, \
        VSF_I2S_STANDARD_LSB
#endif

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check mode uniqueness - all mode modes should have different values
 * Mandatory: VSF_I2S_MODE_MASTER, VSF_I2S_MODE_SLAVE
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2S_CFG_MODE_CHECK_UNIQUE,
    VSF_I2S_MODE_MASTER,
    VSF_I2S_MODE_SLAVE
);

/* Check data bit length uniqueness - all data bit length modes should have different values
 * Mandatory: VSF_I2S_DATA_BITLEN_16
 * Implementation Required: VSF_I2S_DATA_BITLEN_24, VSF_I2S_DATA_BITLEN_32
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2S_CFG_MODE_CHECK_UNIQUE,
    VSF_I2S_DATA_BITLEN_16
#ifdef VSF_I2S_DATA_BITLEN_24
    , VSF_I2S_DATA_BITLEN_24
#endif
#ifdef VSF_I2S_DATA_BITLEN_32
    , VSF_I2S_DATA_BITLEN_32
#endif
);

/* Check frame bit length uniqueness - all frame bit length modes should have different values
 * Mandatory: VSF_I2S_FRAME_BITLEN_16
 * Implementation Required: VSF_I2S_FRAME_BITLEN_24, VSF_I2S_FRAME_BITLEN_32
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2S_CFG_MODE_CHECK_UNIQUE,
    VSF_I2S_FRAME_BITLEN_16
#ifdef VSF_I2S_FRAME_BITLEN_24
    , VSF_I2S_FRAME_BITLEN_24
#endif
#ifdef VSF_I2S_FRAME_BITLEN_32
    , VSF_I2S_FRAME_BITLEN_32
#endif
);

/* Check standard uniqueness - all standard modes should have different values
 * Mandatory: VSF_I2S_STANDARD_PHILIPS
 * Implementation Required: VSF_I2S_STANDARD_MSB, VSF_I2S_STANDARD_LSB
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2S_CFG_MODE_CHECK_UNIQUE,
    VSF_I2S_STANDARD_PHILIPS
#ifdef VSF_I2S_STANDARD_MSB
    , VSF_I2S_STANDARD_MSB
#endif
#ifdef VSF_I2S_STANDARD_LSB
    , VSF_I2S_STANDARD_LSB
#endif
);

#ifdef VSF_I2S_CUSTOM_MODE_VALUES
/* Check uniqueness among custom mode values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_I2S_CFG_MODE_CHECK_UNIQUE, VSF_I2S_CUSTOM_MODE_VALUES);
#endif

VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2S_CFG_MODE_CHECK_UNIQUE,
    /* Mandatory modes - always included */
    VSF_I2S_MODE_MASTER, VSF_I2S_MODE_SLAVE,
    VSF_I2S_DATA_BITLEN_16, VSF_I2S_FRAME_BITLEN_16, VSF_I2S_STANDARD_PHILIPS
    /* Implementation Required modes - conditionally included */
#ifdef VSF_I2S_DATA_BITLEN_24
    , VSF_I2S_DATA_BITLEN_24
#endif
#ifdef VSF_I2S_DATA_BITLEN_32
    , VSF_I2S_DATA_BITLEN_32
#endif
#ifdef VSF_I2S_FRAME_BITLEN_24
    , VSF_I2S_FRAME_BITLEN_24
#endif
#ifdef VSF_I2S_FRAME_BITLEN_32
    , VSF_I2S_FRAME_BITLEN_32
#endif
#ifdef VSF_I2S_STANDARD_MSB
    , VSF_I2S_STANDARD_MSB
#endif
#ifdef VSF_I2S_STANDARD_LSB
    , VSF_I2S_STANDARD_LSB
#endif
    /* User-defined modes - appended by user configuration */
#ifdef VSF_I2S_CUSTOM_MODE_VALUES
    , VSF_I2S_CUSTOM_MODE_VALUES
#endif
);

#undef VSF_I2S_CHECK_UNIQUE_MODE_MODES
#undef VSF_I2S_CHECK_UNIQUE_DATA_BITLEN_MODES
#undef VSF_I2S_CHECK_UNIQUE_FRAME_BITLEN_MODES
#undef VSF_I2S_CHECK_UNIQUE_STANDARD_MODES
#endif  /* VSF_I2S_CFG_MODE_CHECK_UNIQUE */

/* ==================== IRQ MASK UNIQUENESS CHECKS ==================== */

#ifdef VSF_I2S_CFG_IRQ_MASK_CHECK_UNIQUE

#ifdef VSF_I2S_CUSTOM_IRQ_MASK_VALUES
/* Check uniqueness among custom IRQ mask values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_I2S_CFG_IRQ_MASK_CHECK_UNIQUE, VSF_I2S_CUSTOM_IRQ_MASK_VALUES);
#endif

/* Check uniqueness within vsf_i2s_irq_mask_t enum */

/* Check IRQ mask uniqueness - all IRQ mask bits should have different values
 * Mandatory: VSF_I2S_IRQ_MASK_TX_TGL_BUFFER, VSF_I2S_IRQ_MASK_RX_TGL_BUFFER
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2S_CFG_IRQ_MASK_CHECK_UNIQUE,
    /* Mandatory IRQ masks - always included */
    VSF_I2S_IRQ_MASK_TX_TGL_BUFFER,
    VSF_I2S_IRQ_MASK_RX_TGL_BUFFER
    /* User-defined IRQ masks - appended by user configuration */
#ifdef VSF_I2S_CUSTOM_IRQ_MASK_VALUES
    , VSF_I2S_CUSTOM_IRQ_MASK_VALUES
#endif
);
#endif  /* VSF_I2S_CFG_IRQ_MASK_CHECK_UNIQUE */

#undef VSF_I2S_CFG_MODE_CHECK_UNIQUE
#undef VSF_I2S_CFG_IRQ_MASK_CHECK_UNIQUE
#undef VSF_I2S_CUSTOM_MODE_VALUES
#undef VSF_I2S_CUSTOM_IRQ_MASK_VALUES

#endif // VSF_HAL_USE_I2S == ENABLED
