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

#if VSF_HAL_USE_SDIO == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define vsf_real_sdio_t                                     VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t)
#define vsf_real_sdio_capability                            VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_capability)
#define vsf_real_sdio_get_configuration                     VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_get_configuration)
#define vsf_real_sdio_irq_clear                             VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_irq_clear)

#ifndef VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY

#ifndef VSF_SDIO_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_SDIO_CFG_REIMPLEMENT_API_GET_CONFIGURATION   DISABLED
#endif

#ifndef VSF_SDIO_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_SDIO_CFG_REIMPLEMENT_API_IRQ_CLEAR          DISABLED
#endif
#   define  VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY         DISABLED
#endif

#ifdef VSF_SDIO_CFG_IMP_REMAP_PREFIX
#   undef VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY          ENABLED
#   undef VSF_SDIO_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_SDIO_CFG_REIMPLEMENT_API_GET_CONFIGURATION   ENABLED
#   undef VSF_SDIO_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_SDIO_CFG_REIMPLEMENT_API_IRQ_CLEAR           ENABLED
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_SDIO_CFG_CAPCBILITY_MAX_FREQ_HZ
#       define VSF_SDIO_CFG_CAPCBILITY_MAX_FREQ_HZ          (50 * 1000 * 1000)
#   endif
#   ifndef VSF_SDIO_CFG_CAPCBILITY_BUS_WIDTH
#       define VSF_SDIO_CFG_CAPCBILITY_BUS_WIDTH            SDIO_CAP_BUS_WIDTH_8
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_sdio_capability_t vsf_real_sdio_capability(vsf_real_sdio_t *sdio_ptr)
{
    return (vsf_sdio_capability_t){
        .max_freq_hz = VSF_SDIO_CFG_CAPCBILITY_MAX_FREQ_HZ,
        .bus_width   = VSF_SDIO_CFG_CAPCBILITY_BUS_WIDTH,
    };
}
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_sdio_get_configuration(vsf_real_sdio_t *sdio_ptr, vsf_sdio_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != sdio_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_API_IRQ_CLEAR == DISABLED
vsf_sdio_irq_mask_t vsf_real_sdio_irq_clear(vsf_real_sdio_t *sdio_ptr, vsf_sdio_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != sdio_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return 0;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_SDIO_CFG_REIMPLEMENT_TYPE_CAPABILITY
#undef VSF_SDIO_CFG_REIMPLEMENT_TYPE_CFG
#undef VSF_SDIO_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#undef VSF_SDIO_CFG_REIMPLEMENT_TYPE_MODE
#undef VSF_SDIO_CFG_REIMPLEMENT_TYPE_REQOP
#undef VSF_SDIO_CFG_REIMPLEMENT_TYPE_REQSTS
#undef VSF_SDIO_CFG_REIMPLEMENT_TYPE_STATUS
#undef VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_SDIO_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_SDIO_CFG_REIMPLEMENT_API_IRQ_CLEAR
#undef VSF_SDIO_CFG_CAPCBILITY_MAX_FREQ_HZ
#undef VSF_SDIO_CFG_CAPCBILITY_BUS_WIDTH
#undef vsf_real_sdio_t
#undef vsf_real_sdio_capability
#undef vsf_real_sdio_get_configuration
#undef vsf_real_sdio_irq_clear

/*============================ MACROS ========================================*/

#ifdef VSF_SDIO_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_sdio_t                       VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t)
#   define vsf_imp_sdio_init                    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_init)
#   define vsf_imp_sdio_enable                  VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_enable)
#   define vsf_imp_sdio_disable                 VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_disable)
#   define vsf_imp_sdio_irq_enable              VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_irq_enable)
#   define vsf_imp_sdio_irq_disable             VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_irq_disable)
#   define vsf_imp_sdio_irq_clear               VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_irq_clear)
#   define vsf_imp_sdio_status                  VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_status)
#   define vsf_imp_sdio_capability              VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_capability)
#   define vsf_imp_sdio_set_clock               VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_set_clock)
#   define vsf_imp_sdio_set_bus_width           VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_set_bus_width)
#   define vsf_imp_sdio_host_request            VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_host_request)
#   define vsf_imp_sdio_get_configuration  VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_get_configuration)

#   define vsf_remap_sdio_t                     VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_t)
#   define vsf_remap_sdio_init                  VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_init)
#   define vsf_remap_sdio_enable                VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_enable)
#   define vsf_remap_sdio_disable               VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_disable)
#   define vsf_remap_sdio_irq_enable            VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_irq_enable)
#   define vsf_remap_sdio_irq_disable           VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_irq_disable)
#   define vsf_remap_sdio_irq_clear             VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_irq_clear)
#   define vsf_remap_sdio_status                VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_status)
#   define vsf_remap_sdio_capability            VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_capability)
#   define vsf_remap_sdio_set_clock             VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_set_clock)
#   define vsf_remap_sdio_set_bus_width         VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_set_bus_width)
#   define vsf_remap_sdio_host_request          VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_host_request)
#   define vsf_remap_sdio_get_configuration VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_get_configuration)

#   define VSF_SDIO_CFG_IMP_REMAP_FUNCTIONS                                                     \
        vsf_err_t vsf_imp_sdio_init(vsf_imp_sdio_t *sdio_ptr, vsf_sdio_cfg_t *cfg_ptr)          \
        {                                                                                       \
            VSF_HAL_ASSERT((sdio_ptr != NULL) && (cfg_ptr != NULL));                            \
            return vsf_remap_sdio_init(sdio_ptr, cfg_ptr);                                      \
        }                                                                                       \
        void vsf_imp_sdio_fini(vsf_imp_sdio_t *sdio_ptr)                                        \
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            vsf_remap_sdio_fini(sdio_ptr);                                                      \
        }                                                                                       \
        void vsf_imp_sdio_irq_enable(vsf_imp_sdio_t *sdio_ptr, vsf_sdio_irq_mask_t irq_mask)    \
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            vsf_remap_sdio_irq_enable(sdio_ptr, irq_mask);                                      \
        }                                                                                       \
        void vsf_imp_sdio_irq_disable(vsf_imp_sdio_t *sdio_ptr, vsf_sdio_irq_mask_t irq_mask)   \
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            vsf_remap_sdio_irq_disable(sdio_ptr, irq_mask);                                     \
        }                                                                                       \
        vsf_sdio_irq_mask_t vsf_imp_sdio_irq_clear(vsf_imp_sdio_t *sdio_ptr, vsf_sdio_irq_mask_t irq_mask) \
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            return vsf_remap_sdio_irq_clear(sdio_ptr, irq_mask);                                \
        }                                                                                       \
        vsf_sdio_status_t vsf_imp_sdio_status(vsf_imp_sdio_t *sdio_ptr)                         \
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            return vsf_remap_sdio_status(sdio_ptr);                                             \
        }                                                                                       \
        vsf_sdio_capability_t vsf_imp_sdio_capability(vsf_imp_sdio_t *sdio_ptr)                 \
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            return vsf_remap_sdio_capability(sdio_ptr);                                         \
        }                                                                                       \
        vsf_err_t vsf_imp_sdio_set_clock(vsf_imp_sdio_t *sdio_ptr, uint32_t clock_hz)           \
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            return vsf_remap_sdio_set_clock(sdio_ptr, clock_hz);                                \
        }                                                                                       \
        vsf_err_t vsf_imp_sdio_set_bus_width(vsf_imp_sdio_t *sdio_ptr, uint8_t bus_width)       \
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            return vsf_remap_sdio_set_bus_width(sdio_ptr, bus_width);                           \
        }                                                                                       \
        vsf_err_t vsf_imp_sdio_host_request(vsf_imp_sdio_t *sdio_ptr, vsf_sdio_trans_t *trans)  \
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            return vsf_remap_sdio_host_request(sdio_ptr, trans);                                \
        }                                                                                       \
        vsf_err_t vsf_imp_sdio_get_configuration(vsf_imp_sdio_t *sdio_ptr, vsf_sdio_cfg_t *cfg_ptr) \
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            return vsf_remap_sdio_get_configuration(sdio_ptr, cfg_ptr);                         \
        }
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _sdio
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _SDIO

#if !defined(VSF_SDIO_CFG_IMP_PREFIX) && !defined(VSF_SDIO_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_SDIO_CFG_IMP_PREFIX in sdio driver"
#endif

#if !defined(VSF_SDIO_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_SDIO_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_SDIO_CFG_IMP_UPCASE_PREFIX in sdio driver"
#endif

#ifndef VSF_SDIO_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_SDIO_CFG_IMP_COUNT_MASK_PREFIX        VSF_SDIO_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_SDIO_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_SDIO_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_SDIO_CFG_IMP_PREFIX
#undef VSF_SDIO_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_SDIO_CFG_IMP_UPCASE_PREFIX
#undef VSF_SDIO_CFG_IMP_DEVICE_PREFIX
#undef VSF_SDIO_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_SDIO_CFG_IMP_LV0
#undef VSF_SDIO_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_SDIO_CFG_IMP_HAS_OP
#undef VSF_SDIO_CFG_IMP_EXTERN_OP

#undef vsf_imp_sdio_t
#undef vsf_imp_sdio_init
#undef vsf_imp_sdio_enable
#undef vsf_imp_sdio_disable
#undef vsf_imp_sdio_irq_enable
#undef vsf_imp_sdio_irq_disable
#undef vsf_imp_sdio_irq_clear
#undef vsf_imp_sdio_status
#undef vsf_imp_sdio_capability
#undef vsf_imp_sdio_set_clock
#undef vsf_imp_sdio_set_bus_width
#undef vsf_imp_sdio_host_request
#undef vsf_imp_sdio_get_configuration

#undef vsf_remap_sdio_t
#undef vsf_remap_sdio_init
#undef vsf_remap_sdio_enable
#undef vsf_remap_sdio_disable
#undef vsf_remap_sdio_irq_enable
#undef vsf_remap_sdio_irq_disable
#undef vsf_remap_sdio_irq_clear
#undef vsf_remap_sdio_status
#undef vsf_remap_sdio_capability
#undef vsf_remap_sdio_set_clock
#undef vsf_remap_sdio_set_bus_width
#undef vsf_remap_sdio_host_request
#undef vsf_remap_sdio_get_configuration

#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME

/*============================ STATIC ASSERTIONS ==============================*/

/* User-extensible macros for custom mode and IRQ mask values
 * Users can define these macros before including this template to append
 * their custom enum values to the uniqueness checks.
 *
 * Check modes for custom values:
 *   VSF_SDIO_CFG_MODE_CHECK_UNIQUE - Check mode for custom mode values
 *     Default: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *   VSF_SDIO_CFG_IRQ_MASK_CHECK_UNIQUE - Check mode for custom IRQ mask values
 *     Default: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *
 * Example usage in vendor driver:
 *   #define VSF_SDIO_CUSTOM_MODE_VALUES           MY_CUSTOM_MODE1, MY_CUSTOM_MODE2
 *   #define VSF_SDIO_CFG_MODE_CHECK_UNIQUE        VSF_HAL_CHECK_MODE_LOOSE
 *   #define VSF_SDIO_CUSTOM_IRQ_MASK_VALUES       MY_CUSTOM_IRQ1, MY_CUSTOM_IRQ2
 *   #define VSF_SDIO_CFG_IRQ_MASK_CHECK_UNIQUE    VSF_HAL_CHECK_MODE_STRICT
 *   #include "hal/driver/common/sdio/sdio_template.inc"
 */


#ifdef VSF_SDIO_CFG_MODE_CHECK_UNIQUE
/* Default mode lists - can be redefined by users before including this file */
#ifndef VSF_SDIO_CHECK_UNIQUE_MODE_MODES
#   define VSF_SDIO_CHECK_UNIQUE_MODE_MODES \
        SDIO_MODE_HOST, \
        SDIO_MODE_SLAVE
#endif

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check mode uniqueness - all mode modes should have different values
 * Mandatory: SDIO_MODE_HOST, SDIO_MODE_SLAVE
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_SDIO_CFG_MODE_CHECK_UNIQUE,
    SDIO_MODE_HOST,
    SDIO_MODE_SLAVE
);

#ifdef VSF_SDIO_CUSTOM_MODE_VALUES
/* Check uniqueness among custom mode values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SDIO_CFG_MODE_CHECK_UNIQUE, VSF_SDIO_CUSTOM_MODE_VALUES);
#endif

VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_SDIO_CFG_MODE_CHECK_UNIQUE,
    /* Mandatory modes - always included */
    SDIO_MODE_HOST, SDIO_MODE_SLAVE
    /* User-defined modes - appended by user configuration */
#ifdef VSF_SDIO_CUSTOM_MODE_VALUES
    , VSF_SDIO_CUSTOM_MODE_VALUES
#endif
);

#undef VSF_SDIO_CHECK_UNIQUE_MODE_MODES
#endif  /* VSF_SDIO_CFG_MODE_CHECK_UNIQUE */

/* ==================== IRQ MASK UNIQUENESS CHECKS ==================== */

#ifdef VSF_SDIO_CFG_IRQ_MASK_CHECK_UNIQUE

#ifdef VSF_SDIO_CUSTOM_IRQ_MASK_VALUES
/* Check uniqueness among custom IRQ mask values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SDIO_CFG_IRQ_MASK_CHECK_UNIQUE, VSF_SDIO_CUSTOM_IRQ_MASK_VALUES);
#endif

/* Check uniqueness within vsf_sdio_irq_mask_t enum */

/* Check IRQ mask uniqueness - all IRQ mask bits should have different values
 * Mandatory: SDIO_IRQ_MASK_HOST_RESP_DONE, SDIO_IRQ_MASK_HOST_DATA_DONE, SDIO_IRQ_MASK_HOST_DATA_ABORT
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_SDIO_CFG_IRQ_MASK_CHECK_UNIQUE,
    /* Mandatory IRQ masks - always included */
    SDIO_IRQ_MASK_HOST_RESP_DONE,
    SDIO_IRQ_MASK_HOST_DATA_DONE,
    SDIO_IRQ_MASK_HOST_DATA_ABORT
    /* User-defined IRQ masks - appended by user configuration */
#ifdef VSF_SDIO_CUSTOM_IRQ_MASK_VALUES
    , VSF_SDIO_CUSTOM_IRQ_MASK_VALUES
#endif
);
#endif  /* VSF_SDIO_CFG_IRQ_MASK_CHECK_UNIQUE */

/* ==================== REQOP UNIQUENESS CHECKS ==================== */

#ifdef VSF_SDIO_CFG_REQOP_CHECK_UNIQUE
/* Default reqop lists - can be redefined by users before including this file */
#ifndef VSF_SDIO_CHECK_UNIQUE_REQOP_CMDOP_MODES
#   define VSF_SDIO_CHECK_UNIQUE_REQOP_CMDOP_MODES \
        SDIO_CMDOP_BYTE, \
        SDIO_CMDOP_STREAM, \
        SDIO_CMDOP_SINGLE_BLOCK, \
        SDIO_CMDOP_MULTI_BLOCK
#endif

#ifndef VSF_SDIO_CHECK_UNIQUE_REQOP_RW_MODES
#   define VSF_SDIO_CHECK_UNIQUE_REQOP_RW_MODES \
        SDIO_CMDOP_WRITE, \
        SDIO_CMDOP_READ
#endif

#ifndef VSF_SDIO_CHECK_UNIQUE_REQOP_RESP_MODES
#   define VSF_SDIO_CHECK_UNIQUE_REQOP_RESP_MODES \
        SDIO_CMDOP_RESP_BUSY, \
        __SDIO_CMDOP_RESP, \
        __SDIO_CMDOP_RESP_SHORT, \
        __SDIO_CMDOP_RESP_SHORT_CRC, \
        __SDIO_CMDOP_RESP_LONG_CRC
#endif

#ifndef VSF_SDIO_CHECK_UNIQUE_REQOP_SPECIAL_MODES
#   define VSF_SDIO_CHECK_UNIQUE_REQOP_SPECIAL_MODES \
        SDIO_CMDOP_CLKHOLD, \
        SDIO_CMDOP_TRANS_STOP
#endif

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check command operation mode uniqueness - all command operation modes should have different values
 * Mandatory: SDIO_CMDOP_BYTE, SDIO_CMDOP_STREAM, SDIO_CMDOP_SINGLE_BLOCK, SDIO_CMDOP_MULTI_BLOCK
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SDIO_CFG_REQOP_CHECK_UNIQUE, VSF_SDIO_CHECK_UNIQUE_REQOP_CMDOP_MODES);

/* Check read/write mode uniqueness - all read/write modes should have different values
 * Mandatory: SDIO_CMDOP_WRITE, SDIO_CMDOP_READ
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SDIO_CFG_REQOP_CHECK_UNIQUE, VSF_SDIO_CHECK_UNIQUE_REQOP_RW_MODES);

/* Check response mode uniqueness - all response modes should have different values
 * Mandatory: SDIO_CMDOP_RESP_BUSY, __SDIO_CMDOP_RESP, __SDIO_CMDOP_RESP_SHORT,
 *            __SDIO_CMDOP_RESP_SHORT_CRC, __SDIO_CMDOP_RESP_LONG_CRC
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SDIO_CFG_REQOP_CHECK_UNIQUE, VSF_SDIO_CHECK_UNIQUE_REQOP_RESP_MODES);

/* Check special operation mode uniqueness - all special operation modes should have different values
 * Mandatory: SDIO_CMDOP_CLKHOLD, SDIO_CMDOP_TRANS_STOP
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SDIO_CFG_REQOP_CHECK_UNIQUE, VSF_SDIO_CHECK_UNIQUE_REQOP_SPECIAL_MODES);

#ifdef VSF_SDIO_CUSTOM_REQOP_VALUES
/* Check uniqueness among custom reqop values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SDIO_CFG_REQOP_CHECK_UNIQUE, VSF_SDIO_CUSTOM_REQOP_VALUES);
#endif

/* Check reqop uniqueness - all reqop bits should have different values
 * Mandatory: SDIO_CMDOP_BYTE, SDIO_CMDOP_STREAM, SDIO_CMDOP_SINGLE_BLOCK, SDIO_CMDOP_MULTI_BLOCK,
 *            SDIO_CMDOP_WRITE, SDIO_CMDOP_READ,
 *            SDIO_CMDOP_RESP_BUSY, __SDIO_CMDOP_RESP, __SDIO_CMDOP_RESP_SHORT,
 *            __SDIO_CMDOP_RESP_SHORT_CRC, __SDIO_CMDOP_RESP_LONG_CRC,
 *            SDIO_CMDOP_CLKHOLD, SDIO_CMDOP_TRANS_STOP
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_SDIO_CFG_REQOP_CHECK_UNIQUE,
    /* Mandatory reqops - always included */
    SDIO_CMDOP_BYTE, SDIO_CMDOP_STREAM, SDIO_CMDOP_SINGLE_BLOCK, SDIO_CMDOP_MULTI_BLOCK,
    SDIO_CMDOP_WRITE, SDIO_CMDOP_READ,
    SDIO_CMDOP_RESP_BUSY, __SDIO_CMDOP_RESP, __SDIO_CMDOP_RESP_SHORT,
    __SDIO_CMDOP_RESP_SHORT_CRC, __SDIO_CMDOP_RESP_LONG_CRC,
    SDIO_CMDOP_CLKHOLD, SDIO_CMDOP_TRANS_STOP
    /* User-defined reqops - appended by user configuration */
#ifdef VSF_SDIO_CUSTOM_REQOP_VALUES
    , VSF_SDIO_CUSTOM_REQOP_VALUES
#endif
);

#undef VSF_SDIO_CHECK_UNIQUE_REQOP_CMDOP_MODES
#undef VSF_SDIO_CHECK_UNIQUE_REQOP_RW_MODES
#undef VSF_SDIO_CHECK_UNIQUE_REQOP_RESP_MODES
#undef VSF_SDIO_CHECK_UNIQUE_REQOP_SPECIAL_MODES
#endif  /* VSF_SDIO_CFG_REQOP_CHECK_UNIQUE */

/* ==================== REQSTS UNIQUENESS CHECKS ==================== */

#ifdef VSF_SDIO_CFG_REQSTS_CHECK_UNIQUE

#ifdef VSF_SDIO_CUSTOM_REQSTS_VALUES
/* Check uniqueness among custom reqsts values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SDIO_CFG_REQSTS_CHECK_UNIQUE, VSF_SDIO_CUSTOM_REQSTS_VALUES);
#endif

/* Check reqsts uniqueness - all reqsts bits should have different values
 * Mandatory: SDIO_REQSTS_DONE, SDIO_REQSTS_ERR_RESP_NONE, SDIO_REQSTS_ERR_RESP_CRC,
 *            SDIO_REQSTS_ERR_DATA_CRC, SDIO_REQSTS_DATA_BUSY, SDIO_REQSTS_BUSY
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_SDIO_CFG_REQSTS_CHECK_UNIQUE,
    /* Mandatory reqstss - always included */
    SDIO_REQSTS_DONE,
    SDIO_REQSTS_ERR_RESP_NONE,
    SDIO_REQSTS_ERR_RESP_CRC,
    SDIO_REQSTS_ERR_DATA_CRC,
    SDIO_REQSTS_DATA_BUSY,
    SDIO_REQSTS_BUSY
    /* User-defined reqstss - appended by user configuration */
#ifdef VSF_SDIO_CUSTOM_REQSTS_VALUES
    , VSF_SDIO_CUSTOM_REQSTS_VALUES
#endif
);
#endif  /* VSF_SDIO_CFG_REQSTS_CHECK_UNIQUE */

#undef VSF_SDIO_CFG_MODE_CHECK_UNIQUE
#undef VSF_SDIO_CFG_IRQ_MASK_CHECK_UNIQUE
#undef VSF_SDIO_CFG_REQOP_CHECK_UNIQUE
#undef VSF_SDIO_CFG_REQSTS_CHECK_UNIQUE
#undef VSF_SDIO_CUSTOM_MODE_VALUES
#undef VSF_SDIO_CUSTOM_IRQ_MASK_VALUES
#undef VSF_SDIO_CUSTOM_REQOP_VALUES
#undef VSF_SDIO_CUSTOM_REQSTS_VALUES

#endif  /* VSF_HAL_USE_SDIO */
