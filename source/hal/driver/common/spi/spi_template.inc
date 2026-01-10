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
/*============================ MACROS ========================================*/

#if VSF_HAL_USE_SPI == ENABLED

#if defined(VSF_SPI_CFG_IMP_RENAME_DEVICE_PREFIX) && (VSF_SPI_CFG_IMP_RENAME_DEVICE_PREFIX == ENABLED)
#   define vsf_real_spi_t                      VSF_MCONNECT(VSF_SPI_CFG_IMP_DEVICE_PREFIX, _t)
#   define vsf_real_spi_capability             VSF_MCONNECT(VSF_SPI_CFG_IMP_DEVICE_PREFIX, _capability)
#   define vsf_real_spi_ctrl                   VSF_MCONNECT(VSF_SPI_CFG_IMP_DEVICE_PREFIX, _ctrl)
#   define vsf_real_spi_get_configuration      VSF_MCONNECT(VSF_SPI_CFG_IMP_DEVICE_PREFIX, _get_configuration)
#   define vsf_real_spi_irq_clear              VSF_MCONNECT(VSF_SPI_CFG_IMP_DEVICE_PREFIX, _irq_clear)
#else
#   define vsf_real_spi_t                      VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t)
#   define vsf_real_spi_capability             VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_capability)
#   define vsf_real_spi_ctrl                   VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_ctrl)
#   define vsf_real_spi_get_configuration      VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_get_configuration)
#   define vsf_real_spi_irq_clear              VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_clear)
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_API_CTRL
#   define  VSF_SPI_CFG_REIMPLEMENT_API_CTRL                    DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define  VSF_SPI_CFG_REIMPLEMENT_API_GET_CONFIGURATION       DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_SPI_CFG_REIMPLEMENT_API_IRQ_CLEAR                DISABLED
#endif

#ifdef VSF_SPI_CFG_IMP_REMAP_PREFIX
#   undef VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY
#   undef VSF_SPI_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   undef VSF_SPI_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY               ENABLED
#   define VSF_SPI_CFG_REIMPLEMENT_API_GET_CONFIGURATION        ENABLED
#   define VSF_SPI_CFG_REIMPLEMENT_API_IRQ_CLEAR                ENABLED
#endif

#if VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_SPI_CFG_CAPABILITY_SUPPORT_AUTO_CS
#       define VSF_SPI_CFG_CAPABILITY_SUPPORT_AUTO_CS                   1
#   endif
#   ifndef VSF_SPI_CFG_CAPABILITY_SUPPORT_MANUAL_CS
#       define VSF_SPI_CFG_CAPABILITY_SUPPORT_MANUAL_CS                 1
#   endif
#   ifndef VSF_SPI_CFG_CAPABILITY_CS_COUNT
#       define VSF_SPI_CFG_CAPABILITY_CS_COUNT                          1
#   endif
#   ifndef VSF_SPI_CFG_CAPABILITY_MAX_CLOCK_HZ
#       define VSF_SPI_CFG_CAPABILITY_MAX_CLOCK_HZ                      0xFFFFFFFF
#   endif
#   ifndef VSF_SPI_CFG_CAPABILITY_MIN_CLOCK_HZ
#       define VSF_SPI_CFG_CAPABILITY_MIN_CLOCK_HZ                      1
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_spi_capability_t vsf_real_spi_capability(vsf_real_spi_t *spi_ptr)
{
    vsf_spi_capability_t spi_capability = {
        .support_hardware_cs  = VSF_SPI_CFG_CAPABILITY_SUPPORT_AUTO_CS,
        .support_software_cs  = VSF_SPI_CFG_CAPABILITY_SUPPORT_MANUAL_CS,
        .cs_count             = VSF_SPI_CFG_CAPABILITY_CS_COUNT,
        .max_clock_hz         = VSF_SPI_CFG_CAPABILITY_MAX_CLOCK_HZ,
        .min_clock_hz         = VSF_SPI_CFG_CAPABILITY_MIN_CLOCK_HZ,
    };

    return spi_capability;
}
#endif

#if VSF_SPI_CFG_REIMPLEMENT_API_CTRL == DISABLED
vsf_err_t vsf_real_spi_ctrl(vsf_real_spi_t *spi_ptr, vsf_spi_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != spi_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_SPI_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_spi_get_configuration(vsf_real_spi_t *spi_ptr, vsf_spi_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != spi_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_SPI_CFG_REIMPLEMENT_API_IRQ_CLEAR == DISABLED
vsf_spi_irq_mask_t vsf_real_spi_irq_clear(vsf_real_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != spi_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return 0;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_SPI_CFG_REIMPLEMENT_TYPE_CAPABILITY
#undef VSF_SPI_CFG_REIMPLEMENT_TYPE_CFG
#undef VSF_SPI_CFG_REIMPLEMENT_TYPE_CTRL
#undef VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#undef VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE
#undef VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS
#undef VSF_SPI_CFG_CAPABILITY_SUPPORT_AUTO_CS
#undef VSF_SPI_CFG_CAPABILITY_SUPPORT_MANUAL_CS
#undef VSF_SPI_CFG_CAPABILITY_CS_COUNT
#undef VSF_SPI_CFG_CAPABILITY_MAX_CLOCK_HZ
#undef VSF_SPI_CFG_CAPABILITY_MIN_CLOCK_HZ
#undef vsf_real_spi_t
#undef vsf_real_spi_capability
#undef vsf_real_spi_ctrl
#undef vsf_real_spi_get_configuration
#undef vsf_real_spi_irq_clear

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROS ========================================*/
/*============================ MACROS ========================================*/

#ifdef VSF_SPI_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_spi_t                        VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t)
#   define vsf_imp_spi_init                     VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_init)
#   define vsf_imp_spi_enable                   VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_enable)
#   define vsf_imp_spi_disable                  VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_disable)
#   define vsf_imp_spi_irq_enable               VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_enable)
#   define vsf_imp_spi_irq_disable              VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_disable)
#   define vsf_imp_spi_irq_clear                VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_irq_clear)
#   define vsf_imp_spi_status                   VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_status)
#   define vsf_imp_spi_capability               VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_capability)
#   define vsf_imp_spi_cs_active                VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_active)
#   define vsf_imp_spi_cs_inactive              VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_inactive)
#   define vsf_imp_spi_fifo_transfer            VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_fifo_transfer)
#   define vsf_imp_spi_request_transfer         VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_request_transfer)
#   define vsf_imp_spi_cancel_transfer          VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cancel_transfer)
#   define vsf_imp_spi_get_transferred_count    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_get_transferred_count)
#   define vsf_imp_spi_ctrl                     VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_ctrl)
#   define vsf_imp_spi_get_configuration        VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_get_configuration)

#   define vsf_remap_spi_t                      VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_t)
#   define vsf_remap_spi_init                   VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_init)
#   define vsf_remap_spi_enable                 VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_enable)
#   define vsf_remap_spi_disable                VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_disable)
#   define vsf_remap_spi_irq_enable             VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_irq_enable)
#   define vsf_remap_spi_irq_disable            VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_irq_disable)
#   define vsf_remap_spi_irq_clear              VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_irq_clear)
#   define vsf_remap_spi_status                 VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_status)
#   define vsf_remap_spi_capability             VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_capability)
#   define vsf_remap_spi_cs_active              VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_cs_active)
#   define vsf_remap_spi_cs_inactive            VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_cs_inactive)
#   define vsf_remap_spi_fifo_transfer          VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_fifo_transfer)
#   define vsf_remap_spi_request_transfer       VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_request_transfer)
#   define vsf_remap_spi_cancel_transfer        VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_cancel_transfer)
#   define vsf_remap_spi_get_transferred_count  VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_get_transferred_count)
#   define vsf_remap_spi_ctrl                   VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_ctrl)
#   define vsf_remap_spi_get_configuration      VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_get_configuration)

#   define VSF_SPI_CFG_IMP_REMAP_FUNCTIONS                                                        \
        vsf_err_t vsf_imp_spi_init(vsf_imp_spi_t *spi_ptr, vsf_spi_cfg_t *cfg_ptr)                \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            return vsf_remap_spi_init(spi_ptr, cfg_ptr);                                          \
        }                                                                                         \
        void vsf_imp_spi_fini(vsf_imp_spi_t *spi_ptr)                                             \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            vsf_remap_spi_fini(spi_ptr);                                                          \
        }                                                                                         \
        fsm_rt_t vsf_imp_spi_enable(vsf_imp_spi_t *spi_ptr)                                       \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            return vsf_remap_spi_enable(spi_ptr);                                                 \
        }                                                                                         \
        fsm_rt_t vsf_imp_spi_disable(vsf_imp_spi_t *spi_ptr)                                      \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            return vsf_remap_spi_disable(spi_ptr);                                                \
        }                                                                                         \
        void vsf_imp_spi_irq_enable(vsf_imp_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask)          \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            vsf_remap_spi_irq_enable(spi_ptr, irq_mask);                                          \
        }                                                                                         \
        void vsf_imp_spi_irq_disable(vsf_imp_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask)         \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            vsf_remap_spi_irq_disable(spi_ptr, irq_mask);                                         \
        }                                                                                         \
        vsf_spi_irq_mask_t vsf_imp_spi_irq_clear(vsf_imp_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask) \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            return vsf_remap_spi_irq_clear(spi_ptr, irq_mask);                                    \
        }                                                                                         \
        vsf_spi_status_t vsf_imp_spi_status(vsf_imp_spi_t *spi_ptr)                               \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            return vsf_remap_spi_status(spi_ptr);                                                 \
        }                                                                                         \
        vsf_err_t vsf_imp_spi_cs_active(vsf_imp_spi_t *spi_ptr, uint_fast8_t index)               \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            return vsf_remap_spi_cs_active(spi_ptr, index);                                       \
        }                                                                                         \
        vsf_err_t vsf_imp_spi_cs_inactive(vsf_imp_spi_t *spi_ptr, uint_fast8_t index)             \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            return vsf_remap_spi_cs_inactive(spi_ptr, index);                                     \
        }                                                                                         \
        void vsf_imp_spi_fifo_transfer(vsf_imp_spi_t *spi_ptr,                                    \
                                   void *out_buffer_ptr,                                          \
                                   uint_fast32_t* out_offset_ptr,                                 \
                                   void *in_buffer_ptr,                                           \
                                   uint_fast32_t* in_offset_ptr,                                  \
                                   uint_fast32_t cnt)                                             \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            vsf_remap_spi_fifo_transfer(spi_ptr,                                                  \
                                       out_buffer_ptr, out_offset_ptr,                            \
                                       in_buffer_ptr, in_offset_ptr,                              \
                                       cnt);                                                      \
        }                                                                                         \
        vsf_err_t vsf_imp_spi_request_transfer(vsf_imp_spi_t *spi_ptr, void *out_buffer_ptr,      \
                                           void *in_buffer_ptr, uint_fast32_t count)              \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            return vsf_remap_spi_request_transfer(spi_ptr, out_buffer_ptr,                        \
                                                 in_buffer_ptr, count);                           \
        }                                                                                         \
        vsf_err_t vsf_imp_spi_cancel_transfer(vsf_imp_spi_t *spi_ptr)                             \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            return vsf_remap_spi_cancel_transfer(spi_ptr);                                        \
        }                                                                                         \
        void vsf_imp_spi_get_transferred_count(vsf_imp_spi_t *spi_ptr,                            \
                                               uint_fast32_t * tx_count, uint_fast32_t *rx_count) \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            vsf_remap_spi_get_transferred_count(spi_ptr, tx_count, rx_count);                     \
        }                                                                                         \
        vsf_spi_capability_t vsf_imp_spi_capability(vsf_imp_spi_t *spi_ptr)                       \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            return vsf_remap_spi_capability(spi_ptr);                                             \
        }                                                                                         \
        vsf_err_t vsf_imp_spi_ctrl(vsf_imp_spi_t *spi_ptr, vsf_spi_ctrl_t ctrl, void *param)      \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            return vsf_remap_spi_ctrl(spi_ptr, ctrl, param);                                      \
        }                                                                                         \
        vsf_err_t vsf_imp_spi_get_configuration(vsf_imp_spi_t *spi_ptr, vsf_spi_cfg_t *cfg_ptr) \
        {                                                                                         \
            VSF_HAL_ASSERT(spi_ptr != NULL);                                                      \
            VSF_HAL_ASSERT(cfg_ptr != NULL);                                                      \
            return vsf_remap_spi_get_configuration(spi_ptr, cfg_ptr);                             \
        }
#endif

/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _spi
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _SPI

#if !defined(VSF_SPI_CFG_IMP_PREFIX) && !defined(VSF_SPI_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_SPI_CFG_IMP_PREFIX in spi driver"
#endif

#if !defined(VSF_SPI_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_SPI_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_SPI_CFG_IMP_UPCASE_PREFIX in spi driver"
#endif

#ifndef VSF_SPI_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_SPI_CFG_IMP_COUNT_MASK_PREFIX        VSF_SPI_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_SPI_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_SPI_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_SPI_CFG_IMP_PREFIX
#undef VSF_SPI_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_SPI_CFG_IMP_UPCASE_PREFIX
#undef VSF_SPI_CFG_IMP_DEVICE_PREFIX
#undef VSF_SPI_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_SPI_CFG_IMP_LV0
#undef VSF_SPI_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_SPI_CFG_IMP_HAS_OP
#undef VSF_SPI_CFG_IMP_EXTERN_OP

#undef vsf_imp_spi_t
#undef vsf_imp_spi_init
#undef vsf_imp_spi_enable
#undef vsf_imp_spi_disable
#undef vsf_imp_spi_irq_enable
#undef vsf_imp_spi_irq_disable
#undef vsf_imp_spi_irq_clear
#undef vsf_imp_spi_status
#undef vsf_imp_spi_capability
#undef vsf_imp_spi_cs_active
#undef vsf_imp_spi_cs_inactive
#undef vsf_imp_spi_fifo_transfer
#undef vsf_imp_spi_request_transfer
#undef vsf_imp_spi_cancel_transfer
#undef vsf_imp_spi_get_transferred_count
#undef vsf_imp_spi_ctrl
#undef vsf_imp_spi_get_configuration

#undef vsf_remap_spi_t
#undef vsf_remap_spi_init
#undef vsf_remap_spi_enable
#undef vsf_remap_spi_disable
#undef vsf_remap_spi_irq_enable
#undef vsf_remap_spi_irq_disable
#undef vsf_remap_spi_irq_clear
#undef vsf_remap_spi_status
#undef vsf_remap_spi_capability
#undef vsf_remap_spi_cs_active
#undef vsf_remap_spi_cs_inactive
#undef vsf_remap_spi_fifo_transfer
#undef vsf_remap_spi_request_transfer
#undef vsf_remap_spi_cancel_transfer
#undef vsf_remap_spi_get_transferred_count
#undef vsf_remap_spi_ctrl
#undef vsf_remap_spi_get_configuration

#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME

/*============================ STATIC ASSERTIONS ==============================*/

/* User-extensible macros for custom mode and IRQ mask values
 * Users can define these macros before including this template to append
 * their custom enum values to the uniqueness checks.
 *
 * Check modes for custom values:
 *   VSF_SPI_CFG_MODE_CHECK_UNIQUE - Check mode for custom mode values
 *     Default: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *   VSF_SPI_CFG_IRQ_MASK_CHECK_UNIQUE - Check mode for custom IRQ mask values
 *     Default: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *
 * Example usage in vendor driver:
 *   #define VSF_SPI_CUSTOM_MODE_VALUES           MY_CUSTOM_MODE1, MY_CUSTOM_MODE2
 *   #define VSF_SPI_CFG_MODE_CHECK_UNIQUE        VSF_HAL_CHECK_MODE_LOOSE
 *   #define VSF_SPI_CUSTOM_IRQ_MASK_VALUES       MY_CUSTOM_IRQ1, MY_CUSTOM_IRQ2
 *   #define VSF_SPI_CFG_IRQ_MASK_CHECK_UNIQUE    VSF_HAL_CHECK_MODE_STRICT
 *   #include "hal/driver/common/spi/spi_template.inc"
 */


#ifdef VSF_SPI_CFG_MODE_CHECK_UNIQUE
/* Default mode lists - can be redefined by users before including this file */
#ifndef VSF_SPI_CHECK_UNIQUE_DIR_MODES
#   define VSF_SPI_CHECK_UNIQUE_DIR_MODES \
        VSF_SPI_MASTER, \
        VSF_SPI_SLAVE
#endif

#ifndef VSF_SPI_CHECK_UNIQUE_BIT_ORDER_MODES
#   define VSF_SPI_CHECK_UNIQUE_BIT_ORDER_MODES \
        VSF_SPI_MSB_FIRST, \
        VSF_SPI_LSB_FIRST
#endif

#ifndef VSF_SPI_CHECK_UNIQUE_MODE_MODES
#   define VSF_SPI_CHECK_UNIQUE_MODE_MODES \
        VSF_SPI_MODE_0, \
        VSF_SPI_MODE_1, \
        VSF_SPI_MODE_2, \
        VSF_SPI_MODE_3
#endif

#ifndef VSF_SPI_CHECK_UNIQUE_CS_MODES
#   define VSF_SPI_CHECK_UNIQUE_CS_MODES \
        VSF_SPI_CS_SOFTWARE_MODE, \
        VSF_SPI_CS_HARDWARE_MODE
#endif

#ifndef VSF_SPI_CHECK_UNIQUE_DATASIZE_MODES
#   define VSF_SPI_CHECK_UNIQUE_DATASIZE_MODES \
        VSF_SPI_DATASIZE_8, \
        VSF_SPI_DATASIZE_16, \
        VSF_SPI_DATASIZE_32
#endif

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check direction mode uniqueness - all direction modes should have different values
 * Mandatory: VSF_SPI_MASTER, VSF_SPI_SLAVE
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SPI_CFG_MODE_CHECK_UNIQUE, VSF_SPI_CHECK_UNIQUE_DIR_MODES);

/* Check bit order uniqueness - all bit order modes should have different values
 * Mandatory: VSF_SPI_MSB_FIRST, VSF_SPI_LSB_FIRST
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SPI_CFG_MODE_CHECK_UNIQUE, VSF_SPI_CHECK_UNIQUE_BIT_ORDER_MODES);

/* Check SPI mode uniqueness - all SPI modes should have different values
 * Mandatory: VSF_SPI_MODE_0, VSF_SPI_MODE_1, VSF_SPI_MODE_2, VSF_SPI_MODE_3
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SPI_CFG_MODE_CHECK_UNIQUE, VSF_SPI_CHECK_UNIQUE_MODE_MODES);

/* Check chip select mode uniqueness - all CS modes should have different values
 * Mandatory: VSF_SPI_CS_SOFTWARE_MODE, VSF_SPI_CS_HARDWARE_MODE
 * Implementation Required: VSF_SPI_CS_HARDWARE_INPUT_MODE
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_SPI_CFG_MODE_CHECK_UNIQUE,
    VSF_SPI_CHECK_UNIQUE_CS_MODES
#ifdef VSF_SPI_CS_HARDWARE_INPUT_MODE
    , VSF_SPI_CS_HARDWARE_INPUT_MODE
#endif
);

/* Check data size uniqueness - all data size modes should have different values
 * Mandatory: VSF_SPI_DATASIZE_8, VSF_SPI_DATASIZE_16, VSF_SPI_DATASIZE_32
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SPI_CFG_MODE_CHECK_UNIQUE, VSF_SPI_CHECK_UNIQUE_DATASIZE_MODES);

#ifdef VSF_SPI_CUSTOM_MODE_VALUES
/* Check uniqueness among custom mode values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SPI_CFG_MODE_CHECK_UNIQUE, VSF_SPI_CUSTOM_MODE_VALUES);
#endif

VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_SPI_CFG_MODE_CHECK_UNIQUE,
    /* Mandatory modes - always included */
    VSF_SPI_MASTER, VSF_SPI_SLAVE,
    VSF_SPI_MSB_FIRST, VSF_SPI_LSB_FIRST,
    VSF_SPI_MODE_0, VSF_SPI_MODE_1, VSF_SPI_MODE_2, VSF_SPI_MODE_3,
    VSF_SPI_CS_SOFTWARE_MODE, VSF_SPI_CS_HARDWARE_MODE,
    VSF_SPI_DATASIZE_8, VSF_SPI_DATASIZE_16, VSF_SPI_DATASIZE_32
    /* Implementation Required modes - conditionally included */
#ifdef VSF_SPI_CS_HARDWARE_INPUT_MODE
    , VSF_SPI_CS_HARDWARE_INPUT_MODE
#endif
    /* Standard Optional modes - only included when __VSF_SPI_CFG_SUPPORT_STANDARD_OPTIONAL is enabled */
#if __VSF_SPI_CFG_SUPPORT_STANDARD_OPTIONAL
#ifdef VSF_SPI_CPOL_LOW
    , VSF_SPI_CPOL_LOW
#endif
#ifdef VSF_SPI_CPOL_HIGH
    , VSF_SPI_CPOL_HIGH
#endif
#ifdef VSF_SPI_CPHA_LOW
    , VSF_SPI_CPHA_LOW
#endif
#ifdef VSF_SPI_CPHA_HIGH
    , VSF_SPI_CPHA_HIGH
#endif
#ifdef VSF_SPI_MOTOROLA_MODE
    , VSF_SPI_MOTOROLA_MODE
#endif
#ifdef VSF_SPI_TI_MODE
    , VSF_SPI_TI_MODE
#endif
#ifdef VSF_SPI_CRC_DISABLED
    , VSF_SPI_CRC_DISABLED
#endif
#ifdef VSF_SPI_CRC_ENABLED
    , VSF_SPI_CRC_ENABLED
#endif
#ifdef VSF_SPI_CLOCK_PRESCALER_2
    , VSF_SPI_CLOCK_PRESCALER_2
#endif
#ifdef VSF_SPI_CLOCK_PRESCALER_4
    , VSF_SPI_CLOCK_PRESCALER_4
#endif
#ifdef VSF_SPI_CLOCK_PRESCALER_8
    , VSF_SPI_CLOCK_PRESCALER_8
#endif
#ifdef VSF_SPI_CLOCK_PRESCALER_16
    , VSF_SPI_CLOCK_PRESCALER_16
#endif
#ifdef VSF_SPI_CLOCK_PRESCALER_32
    , VSF_SPI_CLOCK_PRESCALER_32
#endif
#ifdef VSF_SPI_CLOCK_PRESCALER_64
    , VSF_SPI_CLOCK_PRESCALER_64
#endif
#ifdef VSF_SPI_CLOCK_PRESCALER_128
    , VSF_SPI_CLOCK_PRESCALER_128
#endif
#ifdef VSF_SPI_CLOCK_PRESCALER_256
    , VSF_SPI_CLOCK_PRESCALER_256
#endif
#ifdef VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX
    , VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX
#endif
#ifdef VSF_SPI_DATALINE_2_LINE_RX_ONLY
    , VSF_SPI_DATALINE_2_LINE_RX_ONLY
#endif
#ifdef VSF_SPI_DATALINE_2_LINE_TX_ONLY
    , VSF_SPI_DATALINE_2_LINE_TX_ONLY
#endif
#ifdef VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX
    , VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX
#endif
#ifdef VSF_SPI_DATALINE_2_LINE_HALF_DUPLEX
    , VSF_SPI_DATALINE_2_LINE_HALF_DUPLEX
#endif
#ifdef VSF_SPI_DATALINE_4_LINE_HALF_DUPLEX
    , VSF_SPI_DATALINE_4_LINE_HALF_DUPLEX
#endif
#ifdef VSF_SPI_DATALINE_8_LINE_HALF_DUPLEX
    , VSF_SPI_DATALINE_8_LINE_HALF_DUPLEX
#endif
#ifdef VSF_SPI_DATALINE_16_LINE_HALF_DUPLEX
    , VSF_SPI_DATALINE_16_LINE_HALF_DUPLEX
#endif
#endif
    /* User-defined modes - appended by user configuration */
#ifdef VSF_SPI_CUSTOM_MODE_VALUES
    , VSF_SPI_CUSTOM_MODE_VALUES
#endif
);

#undef VSF_SPI_CHECK_UNIQUE_DIR_MODES
#undef VSF_SPI_CHECK_UNIQUE_BIT_ORDER_MODES
#undef VSF_SPI_CHECK_UNIQUE_MODE_MODES
#undef VSF_SPI_CHECK_UNIQUE_CS_MODES
#undef VSF_SPI_CHECK_UNIQUE_DATASIZE_MODES
#endif  /* VSF_SPI_CFG_MODE_CHECK_UNIQUE */

/* ==================== IRQ MASK UNIQUENESS CHECKS ==================== */

#ifdef VSF_SPI_CFG_IRQ_MASK_CHECK_UNIQUE

#ifdef VSF_SPI_CUSTOM_IRQ_MASK_VALUES
/* Check uniqueness among custom IRQ mask values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SPI_CFG_IRQ_MASK_CHECK_UNIQUE, VSF_SPI_CUSTOM_IRQ_MASK_VALUES);
#endif

/* Check uniqueness within vsf_spi_irq_mask_t enum */

/* Check IRQ mask uniqueness - all IRQ mask bits should have different values
 * Mandatory: VSF_SPI_IRQ_MASK_TX, VSF_SPI_IRQ_MASK_RX,
 *            VSF_SPI_IRQ_MASK_TX_CPL, VSF_SPI_IRQ_MASK_RX_CPL,
 *            VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR
 * Standard Optional: VSF_SPI_IRQ_MASK_CRC_ERR, VSF_SPI_IRQ_MASK_FRAME_ERR
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_SPI_CFG_IRQ_MASK_CHECK_UNIQUE,
    /* Mandatory IRQ masks - always included */
    VSF_SPI_IRQ_MASK_TX,
    VSF_SPI_IRQ_MASK_RX,
    VSF_SPI_IRQ_MASK_TX_CPL,
    VSF_SPI_IRQ_MASK_RX_CPL,
    VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR
    /* Standard Optional IRQ masks - only included when __VSF_SPI_CFG_SUPPORT_STANDARD_OPTIONAL is enabled */
#if __VSF_SPI_CFG_SUPPORT_STANDARD_OPTIONAL
#ifdef VSF_SPI_IRQ_MASK_CRC_ERR
    , VSF_SPI_IRQ_MASK_CRC_ERR
#endif
#ifdef VSF_SPI_IRQ_MASK_FRAME_ERR
    , VSF_SPI_IRQ_MASK_FRAME_ERR
#endif
#endif
    /* User-defined IRQ masks - appended by user configuration */
#ifdef VSF_SPI_CUSTOM_IRQ_MASK_VALUES
    , VSF_SPI_CUSTOM_IRQ_MASK_VALUES
#endif
);
#endif  /* VSF_SPI_CFG_IRQ_MASK_CHECK_UNIQUE */

/* ==================== CTRL UNIQUENESS CHECKS ==================== */

#ifdef VSF_SPI_CFG_CTRL_CHECK_UNIQUE

#ifdef VSF_SPI_CUSTOM_CTRL_VALUES
/* Check uniqueness among custom ctrl values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_SPI_CFG_CTRL_CHECK_UNIQUE, VSF_SPI_CUSTOM_CTRL_VALUES);
#endif

/* Check ctrl uniqueness - all ctrl values should have different values
 * Placeholder: __VSF_SPI_CTRL_DUMMY
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_SPI_CFG_CTRL_CHECK_UNIQUE,
    /* Placeholder ctrl - always included */
    __VSF_SPI_CTRL_DUMMY
    /* User-defined ctrls - appended by user configuration */
#ifdef VSF_SPI_CUSTOM_CTRL_VALUES
    , VSF_SPI_CUSTOM_CTRL_VALUES
#endif
);
#endif  /* VSF_SPI_CFG_CTRL_CHECK_UNIQUE */

#undef VSF_SPI_CFG_MODE_CHECK_UNIQUE
#undef VSF_SPI_CFG_IRQ_MASK_CHECK_UNIQUE
#undef VSF_SPI_CFG_CTRL_CHECK_UNIQUE
#undef VSF_SPI_CUSTOM_MODE_VALUES
#undef VSF_SPI_CUSTOM_IRQ_MASK_VALUES
#undef VSF_SPI_CUSTOM_CTRL_VALUES

#endif  /* VSF_HAL_USE_SPI */

