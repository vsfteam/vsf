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

#include "hal/driver/common/template/vsf_template_hal_driver.h"

/*============================ MACROS ========================================*/
#if VSF_HAL_USE_I2C == ENABLED

#ifndef VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY               DISABLED
#endif

#ifndef VSF_I2C_CFG_REIMPLEMENT_API_CTRL
#   define VSF_I2C_CFG_REIMPLEMENT_API_CTRL                     DISABLED
#endif

#ifndef VSF_I2C_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_I2C_CFG_REIMPLEMENT_API_GET_CONFIGURATION        DISABLED
#endif

#ifndef VSF_I2C_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_I2C_CFG_REIMPLEMENT_API_IRQ_CLEAR                DISABLED
#endif

#ifdef VSF_I2C_CFG_IMP_REMAP_PREFIX
#   undef VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY
#   undef VSF_I2C_CFG_REIMPLEMENT_API_CTRL
#   undef VSF_I2C_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   undef VSF_I2C_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY               ENABLED
#   define VSF_I2C_CFG_REIMPLEMENT_API_GET_CONFIGURATION        ENABLED
#   define VSF_I2C_CFG_REIMPLEMENT_API_CTRL                     ENABLED
#   define VSF_I2C_CFG_REIMPLEMENT_API_IRQ_CLEAR                ENABLED
#endif

#if VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_I2C_CFG_CAPABILITY_IRQ_MASK
#       define VSF_I2C_CFG_CAPABILITY_IRQ_MASK                  VSF_I2C_IRQ_ALL_BITS_MASK
#   endif
#   ifndef VSF_I2C_CFG_CAPABILITY_SUPPORT_NO_START
#       define VSF_I2C_CFG_CAPABILITY_SUPPORT_NO_START          1
#   endif
#   ifndef VSF_I2C_CFG_CAPABILITY_SUPPORT_NO_STOP_RESTART
#       define VSF_I2C_CFG_CAPABILITY_SUPPORT_NO_STOP_RESTART   1
#   endif
#   ifndef VSF_I2C_CFG_CAPABILITY_SUPPORT_RESTART
#       define VSF_I2C_CFG_CAPABILITY_SUPPORT_RESTART           1
#   endif
#   ifndef VSF_I2C_CFG_CAPABILITY_MAX_TRANSFER_SIZE
#       define VSF_I2C_CFG_CAPABILITY_MAX_TRANSFER_SIZE         0xFFFF
#   endif
#   ifndef VSF_I2C_CFG_CAPABILITY_MIN_TRANSFER_SIZE
#       define VSF_I2C_CFG_CAPABILITY_MIN_TRANSFER_SIZE         0
#   endif
#endif

#define vsf_real_i2c_t                  VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t)
#define vsf_real_master_request         VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_request)
#define vsf_real_i2c_capability         VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_capability)
#define vsf_real_i2c_ctrl               VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_ctrl)
#define vsf_real_i2c_get_configuration  VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_get_configuration)
#define vsf_real_i2c_irq_clear          VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_clear)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_I2C_CFG_REQUEST_TEMPLATE == ENABLED
vsf_err_t vsf_real_i2c_master_request(vsf_real_i2c_t *i2c_ptr, uint16_t address, vsf_i2c_cmd_t cmd, uint16_t count, uint8_t *buffer_ptr)
{
    vsf_real_i2c_t * i2c_type_ptr = (vsf_real_i2c_t *)i2c_ptr;
    return vsf_i2c_request_master_request(i2c_ptr, &i2c_type_ptr->request, address, cmd, count, buffer_ptr);
}
#endif

#if VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_i2c_capability_t vsf_real_i2c_capability(vsf_real_i2c_t *i2c_ptr)
{
    vsf_i2c_capability_t i2c_capability = {
        .irq_mask                   = VSF_I2C_CFG_CAPABILITY_IRQ_MASK,
        .support_no_start           = VSF_I2C_CFG_CAPABILITY_SUPPORT_NO_START,
        .support_no_stop_restart    = VSF_I2C_CFG_CAPABILITY_SUPPORT_NO_STOP_RESTART,
        .support_restart            = VSF_I2C_CFG_CAPABILITY_SUPPORT_RESTART,
        .max_transfer_size          = VSF_I2C_CFG_CAPABILITY_MAX_TRANSFER_SIZE,
        .min_transfer_size          = VSF_I2C_CFG_CAPABILITY_MIN_TRANSFER_SIZE,
    };

    return i2c_capability;
}
#endif

#if VSF_I2C_CFG_REIMPLEMENT_API_CTRL == DISABLED
vsf_err_t vsf_real_i2c_ctrl(vsf_real_i2c_t *i2c_ptr, vsf_i2c_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_I2C_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_i2c_get_configuration(vsf_real_i2c_t *i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_I2C_CFG_REIMPLEMENT_API_IRQ_CLEAR == DISABLED
vsf_i2c_irq_mask_t vsf_real_i2c_irq_clear(vsf_real_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != i2c_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return 0;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_I2C_CFG_REIMPLEMENT_TYPE_CAPABILITY
#undef VSF_I2C_CFG_REIMPLEMENT_TYPE_CFG
#undef VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD
#undef VSF_I2C_CFG_REIMPLEMENT_TYPE_CTRL
#undef VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#undef VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE
#undef VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS
#undef VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_I2C_CFG_REIMPLEMENT_API_CTRL
#undef VSF_I2C_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_I2C_CFG_REIMPLEMENT_API_IRQ_CLEAR
#undef VSF_I2C_CFG_CAPABILITY_IRQ_MASK
#undef VSF_I2C_CFG_CAPABILITY_SUPPORT_NO_START
#undef VSF_I2C_CFG_CAPABILITY_SUPPORT_NO_STOP_RESTART
#undef VSF_I2C_CFG_CAPABILITY_SUPPORT_RESTART
#undef VSF_I2C_CFG_CAPABILITY_MAX_TRANSFER_SIZE
#undef VSF_I2C_CFG_CAPABILITY_MIN_TRANSFER_SIZE

#undef vsf_real_i2c_t
#undef vsf_real_master_request
#undef vsf_real_i2c_capability
#undef vsf_real_i2c_ctrl
#undef vsf_real_i2c_get_configuration
#undef vsf_real_i2c_irq_clear

/*============================ MACROS ========================================*/

#ifdef VSF_I2C_CFG_IMP_REMAP_PREFIX
#   define vsf_imp_i2c_t                        VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t)
#   define vsf_imp_i2c_init                     VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_init)
#   define vsf_imp_i2c_fini                     VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_fini)
#   define vsf_imp_i2c_enable                   VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_enable)
#   define vsf_imp_i2c_disable                  VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_disable)
#   define vsf_imp_i2c_irq_enable               VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_enable)
#   define vsf_imp_i2c_irq_disable              VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_disable)
#   define vsf_imp_i2c_status                   VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_status)
#   define vsf_imp_i2c_capability               VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_capability)
#   define vsf_imp_i2c_master_fifo_transfer     VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_fifo_transfer)
#   define vsf_imp_i2c_master_request           VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_request)
#   define vsf_imp_i2c_get_transferred_count    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_get_transferred_count)
#   define vsf_imp_i2c_ctrl                     VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_ctrl)
#   define vsf_imp_i2c_get_configuration        VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_get_configuration)
#   define vsf_imp_i2c_irq_clear                VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_irq_clear)

#   define vsf_remap_i2c_t                      VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_t)
#   define vsf_remap_i2c_init                   VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_init)
#   define vsf_remap_i2c_fini                   VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_fini)
#   define vsf_remap_i2c_enable                 VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_enable)
#   define vsf_remap_i2c_disable                VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_disable)
#   define vsf_remap_i2c_irq_enable             VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_irq_enable)
#   define vsf_remap_i2c_irq_disable            VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_irq_disable)
#   define vsf_remap_i2c_status                 VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_status)
#   define vsf_remap_i2c_capability             VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_capability)
#   define vsf_remap_i2c_master_fifo_transfer  VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_master_fifo_transfer)
#   define vsf_remap_i2c_master_request         VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_master_request)
#   define vsf_remap_i2c_get_transferred_count VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_get_transferred_count)
#   define vsf_remap_i2c_ctrl                   VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_ctrl)
#   define vsf_remap_i2c_get_configuration      VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_get_configuration)
#   define vsf_remap_i2c_irq_clear              VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_irq_clear)

#   define VSF_I2C_CFG_IMP_REMAP_FUNCTIONS                                                         \
        vsf_err_t vsf_imp_i2c_init(vsf_imp_i2c_t *i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)                 \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            return vsf_remap_i2c_init(i2c_ptr, cfg_ptr);                                           \
        }                                                                                          \
        void vsf_imp_i2c_fini(vsf_imp_i2c_t *i2c_ptr)                                              \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            vsf_remap_i2c_fini(i2c_ptr);                                                           \
        }                                                                                          \
        fsm_rt_t vsf_imp_i2c_enable(vsf_imp_i2c_t *i2c_ptr)                                        \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            return vsf_remap_i2c_enable(i2c_ptr);                                                  \
        }                                                                                          \
        fsm_rt_t vsf_imp_i2c_disable(vsf_imp_i2c_t *i2c_ptr)                                       \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            return vsf_remap_i2c_disable(i2c_ptr);                                                 \
        }                                                                                          \
        void vsf_imp_i2c_irq_enable(vsf_imp_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)           \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            vsf_remap_i2c_irq_enable(i2c_ptr, irq_mask);                                           \
        }                                                                                          \
        void vsf_imp_i2c_irq_disable(vsf_imp_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)          \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            vsf_remap_i2c_irq_disable(i2c_ptr, irq_mask);                                          \
        }                                                                                          \
        vsf_i2c_status_t vsf_imp_i2c_status(vsf_imp_i2c_t *i2c_ptr)                                \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            return vsf_remap_i2c_status(i2c_ptr);                                                  \
        }                                                                                          \
        vsf_i2c_capability_t vsf_imp_i2c_capability(vsf_imp_i2c_t *i2c_ptr)                        \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            return vsf_remap_i2c_capability(i2c_ptr);                                              \
        }                                                                                          \
        fsm_rt_t vsf_imp_i2c_master_fifo_transfer(vsf_imp_i2c_t *i2c_ptr,                          \
                                                   uint16_t address,                               \
                                                   vsf_i2c_cmd_t cmd,                              \
                                                   uint_fast16_t count,                            \
                                                   uint8_t *buffer_ptr,                            \
                                                   vsf_i2c_cmd_t *cur_cmd_ptr,                     \
                                                   uint_fast16_t *offset_ptr)                      \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            return vsf_remap_i2c_master_fifo_transfer(i2c_ptr, address, cmd, count, buffer_ptr,    \
                                                      cur_cmd_ptr, offset_ptr);                    \
        }                                                                                          \
        vsf_err_t vsf_imp_i2c_master_request(vsf_imp_i2c_t *i2c_ptr,                               \
                                         uint16_t address,                                         \
                                         vsf_imp_i2c_cmd_t cmd,                                    \
                                         uint16_t count,                                           \
                                         uint8_t *buffer_ptr)                                      \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            return vsf_remap_i2c_master_request(i2c_ptr, address, cmd, count, buffer_ptr);         \
        }                                                                                          \
        uint_fast32_t vsf_imp_i2c_get_transferred_count(vsf_imp_i2c_t *i2c_ptr)                    \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            VSF_HAL_ASSERT(vsf_remap_i2c_get_transferred_count!= NULL);                            \
            return vsf_remap_i2c_get_transferred_count(i2c_ptr);                                   \
        }                                                                                          \
        vsf_err_t vsf_imp_i2c_ctrl(vsf_imp_i2c_t *i2c_ptr, vsf_i2c_ctrl_t ctrl, void *param)       \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            return vsf_remap_i2c_ctrl(i2c_ptr, ctrl, param);                                       \
        }                                                                                          \
        vsf_err_t vsf_imp_i2c_get_configuration(vsf_imp_i2c_t *i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)    \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            return vsf_remap_i2c_get_configuration(i2c_ptr, cfg_ptr);                              \
        }                                                                                          \
        vsf_i2c_irq_mask_t vsf_imp_i2c_irq_clear(vsf_imp_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask) \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            return vsf_remap_i2c_irq_clear(i2c_ptr, irq_mask);                                     \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _i2c
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _I2C

#if !defined(VSF_I2C_CFG_IMP_PREFIX) && !defined(VSF_I2C_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_I2C_CFG_IMP_PREFIX in i2c driver"
#endif

#if !defined(VSF_I2C_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_I2C_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_I2C_CFG_IMP_UPCASE_PREFIX in i2c driver"
#endif

#ifndef VSF_I2C_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_I2C_CFG_IMP_COUNT_MASK_PREFIX        VSF_I2C_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_I2C_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_I2C_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_I2C_CFG_IMP_PREFIX
#undef VSF_I2C_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_I2C_CFG_IMP_UPCASE_PREFIX
#undef VSF_I2C_CFG_IMP_DEVICE_PREFIX
#undef VSF_I2C_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_I2C_CFG_IMP_LV0
#undef VSF_I2C_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_I2C_CFG_IMP_HAS_OP
#undef VSF_I2C_CFG_IMP_EXTERN_OP

#undef vsf_imp_i2c_t
#undef vsf_imp_i2c_init
#undef vsf_imp_i2c_fini
#undef vsf_imp_i2c_enable
#undef vsf_imp_i2c_disable
#undef vsf_imp_i2c_irq_enable
#undef vsf_imp_i2c_irq_disable
#undef vsf_imp_i2c_status
#undef vsf_imp_i2c_capability
#undef vsf_imp_i2c_master_request
#undef vsf_imp_i2c_get_transferred_count
#undef vsf_imp_i2c_ctrl
#undef vsf_imp_i2c_get_configuration
#undef vsf_imp_i2c_irq_clear

#undef vsf_remap_i2c_t
#undef vsf_remap_i2c_init
#undef vsf_remap_i2c_fini
#undef vsf_remap_i2c_enable
#undef vsf_remap_i2c_disable
#undef vsf_remap_i2c_irq_enable
#undef vsf_remap_i2c_irq_disable
#undef vsf_remap_i2c_status
#undef vsf_remap_i2c_capability
#undef vsf_remap_i2c_master_request
#undef vsf_remap_i2c_get_transferred_count
#undef vsf_remap_i2c_ctrl
#undef vsf_remap_i2c_get_configuration
#undef vsf_remap_i2c_irq_clear

#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME

/*============================ STATIC ASSERTIONS ==============================*/

/* User-extensible macros for custom mode and IRQ mask values
 * Users can define these macros before including this template to append
 * their custom enum values to the uniqueness checks.
 *
 * Check modes for custom values:
 *   VSF_I2C_CFG_MODE_CHECK_UNIQUE - Check mode for custom mode values
 *     Default: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *   VSF_I2C_CFG_IRQ_MASK_CHECK_UNIQUE - Check mode for custom IRQ mask values
 *     Default: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *
 * Example usage in vendor driver:
 *   #define VSF_I2C_CUSTOM_MODE_VALUES           MY_CUSTOM_MODE1, MY_CUSTOM_MODE2
 *   #define VSF_I2C_CFG_MODE_CHECK_UNIQUE        VSF_HAL_CHECK_MODE_LOOSE
 *   #define VSF_I2C_CUSTOM_IRQ_MASK_VALUES       MY_CUSTOM_IRQ1, MY_CUSTOM_IRQ2
 *   #define VSF_I2C_CFG_IRQ_MASK_CHECK_UNIQUE    VSF_HAL_CHECK_MODE_STRICT
 *   #include "hal/driver/common/i2c/i2c_template.inc"
 */


#ifdef VSF_I2C_CFG_MODE_CHECK_UNIQUE
/* Default mode lists - can be redefined by users before including this file */
#ifndef VSF_I2C_CHECK_UNIQUE_MODE_MODES
#   define VSF_I2C_CHECK_UNIQUE_MODE_MODES \
        VSF_I2C_MODE_MASTER, \
        VSF_I2C_MODE_SLAVE
#endif

#ifndef VSF_I2C_CHECK_UNIQUE_SPEED_MODES
#   define VSF_I2C_CHECK_UNIQUE_SPEED_MODES \
        VSF_I2C_SPEED_STANDARD_MODE, \
        VSF_I2C_SPEED_FAST_MODE, \
        VSF_I2C_SPEED_FAST_MODE_PLUS, \
        VSF_I2C_SPEED_HIGH_SPEED_MODE
#endif

#ifndef VSF_I2C_CHECK_UNIQUE_ADDR_MODES
#   define VSF_I2C_CHECK_UNIQUE_ADDR_MODES \
        VSF_I2C_ADDR_7_BITS, \
        VSF_I2C_ADDR_10_BITS
#endif

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check mode uniqueness - all mode modes should have different values
 * Mandatory: VSF_I2C_MODE_MASTER, VSF_I2C_MODE_SLAVE
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2C_CFG_MODE_CHECK_UNIQUE,
    VSF_I2C_MODE_MASTER,
    VSF_I2C_MODE_SLAVE
);

/* Check speed mode uniqueness - all speed modes should have different values
 * Mandatory: VSF_I2C_SPEED_STANDARD_MODE, VSF_I2C_SPEED_FAST_MODE,
 *            VSF_I2C_SPEED_FAST_MODE_PLUS, VSF_I2C_SPEED_HIGH_SPEED_MODE
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2C_CFG_MODE_CHECK_UNIQUE,
    VSF_I2C_SPEED_STANDARD_MODE,
    VSF_I2C_SPEED_FAST_MODE,
    VSF_I2C_SPEED_FAST_MODE_PLUS,
    VSF_I2C_SPEED_HIGH_SPEED_MODE
);

/* Check address mode uniqueness - all address modes should have different values
 * Mandatory: VSF_I2C_ADDR_7_BITS, VSF_I2C_ADDR_10_BITS
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2C_CFG_MODE_CHECK_UNIQUE,
    VSF_I2C_ADDR_7_BITS,
    VSF_I2C_ADDR_10_BITS
);

#ifdef VSF_I2C_CUSTOM_MODE_VALUES
/* Check uniqueness among custom mode values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_I2C_CFG_MODE_CHECK_UNIQUE, VSF_I2C_CUSTOM_MODE_VALUES);
#endif

VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2C_CFG_MODE_CHECK_UNIQUE,
    /* Mandatory modes - always included */
    VSF_I2C_MODE_MASTER, VSF_I2C_MODE_SLAVE,
    VSF_I2C_SPEED_STANDARD_MODE, VSF_I2C_SPEED_FAST_MODE,
    VSF_I2C_SPEED_FAST_MODE_PLUS, VSF_I2C_SPEED_HIGH_SPEED_MODE,
    VSF_I2C_ADDR_7_BITS, VSF_I2C_ADDR_10_BITS
    /* User-defined modes - appended by user configuration */
#ifdef VSF_I2C_CUSTOM_MODE_VALUES
    , VSF_I2C_CUSTOM_MODE_VALUES
#endif
);

#undef VSF_I2C_CHECK_UNIQUE_MODE_MODES
#undef VSF_I2C_CHECK_UNIQUE_SPEED_MODES
#undef VSF_I2C_CHECK_UNIQUE_ADDR_MODES
#endif  /* VSF_I2C_CFG_MODE_CHECK_UNIQUE */

/* ==================== CMD UNIQUENESS CHECKS ==================== */

#ifdef VSF_I2C_CFG_CMD_CHECK_UNIQUE
/* Default cmd lists - can be redefined by users before including this file */
#ifndef VSF_I2C_CHECK_UNIQUE_CMD_RW_MODES
#   define VSF_I2C_CHECK_UNIQUE_CMD_RW_MODES \
        VSF_I2C_CMD_WRITE, \
        VSF_I2C_CMD_READ
#endif

#ifndef VSF_I2C_CHECK_UNIQUE_CMD_START_STOP_RESTART_MODES
#   define VSF_I2C_CHECK_UNIQUE_CMD_START_STOP_RESTART_MODES \
        VSF_I2C_CMD_START, \
        VSF_I2C_CMD_STOP, \
        VSF_I2C_CMD_RESTART
#endif

#ifndef VSF_I2C_CHECK_UNIQUE_CMD_ADDR_MODES
#   define VSF_I2C_CHECK_UNIQUE_CMD_ADDR_MODES \
        VSF_I2C_CMD_7_BITS, \
        VSF_I2C_CMD_10_BITS
#endif

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check read/write command uniqueness - all read/write commands should have different values
 * Mandatory: VSF_I2C_CMD_WRITE, VSF_I2C_CMD_READ
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_I2C_CFG_CMD_CHECK_UNIQUE, VSF_I2C_CHECK_UNIQUE_CMD_RW_MODES);

/* Check start/stop/restart command uniqueness - all start/stop/restart commands should have different values
 * Mandatory: VSF_I2C_CMD_START, VSF_I2C_CMD_STOP, VSF_I2C_CMD_RESTART
 * Optional: VSF_I2C_CMD_NO_START, VSF_I2C_CMD_NO_STOP, VSF_I2C_CMD_NO_RESTART
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2C_CFG_CMD_CHECK_UNIQUE,
    VSF_I2C_CHECK_UNIQUE_CMD_START_STOP_RESTART_MODES
#ifdef VSF_I2C_CMD_NO_START
    , VSF_I2C_CMD_NO_START
#endif
#ifdef VSF_I2C_CMD_NO_STOP
    , VSF_I2C_CMD_NO_STOP
#endif
#ifdef VSF_I2C_CMD_NO_RESTART
    , VSF_I2C_CMD_NO_RESTART
#endif
);

/* Check address mode command uniqueness - all address mode commands should have different values
 * Mandatory: VSF_I2C_CMD_7_BITS, VSF_I2C_CMD_10_BITS
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_I2C_CFG_CMD_CHECK_UNIQUE, VSF_I2C_CHECK_UNIQUE_CMD_ADDR_MODES);

#ifdef VSF_I2C_CUSTOM_CMD_VALUES
/* Check uniqueness among custom cmd values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_I2C_CFG_CMD_CHECK_UNIQUE, VSF_I2C_CUSTOM_CMD_VALUES);
#endif

VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2C_CFG_CMD_CHECK_UNIQUE,
    /* Mandatory commands - always included */
    VSF_I2C_CMD_WRITE, VSF_I2C_CMD_READ,
    VSF_I2C_CMD_START, VSF_I2C_CMD_STOP, VSF_I2C_CMD_RESTART,
    VSF_I2C_CMD_7_BITS, VSF_I2C_CMD_10_BITS
    /* Optional commands - conditionally included */
#ifdef VSF_I2C_CMD_NO_START
    , VSF_I2C_CMD_NO_START
#endif
#ifdef VSF_I2C_CMD_NO_STOP
    , VSF_I2C_CMD_NO_STOP
#endif
#ifdef VSF_I2C_CMD_NO_RESTART
    , VSF_I2C_CMD_NO_RESTART
#endif
    /* User-defined commands - appended by user configuration */
#ifdef VSF_I2C_CUSTOM_CMD_VALUES
    , VSF_I2C_CUSTOM_CMD_VALUES
#endif
);

#undef VSF_I2C_CHECK_UNIQUE_CMD_RW_MODES
#undef VSF_I2C_CHECK_UNIQUE_CMD_START_STOP_RESTART_MODES
#undef VSF_I2C_CHECK_UNIQUE_CMD_ADDR_MODES
#endif  /* VSF_I2C_CFG_CMD_CHECK_UNIQUE */

/* ==================== IRQ MASK UNIQUENESS CHECKS ==================== */

#ifdef VSF_I2C_CFG_IRQ_MASK_CHECK_UNIQUE

#ifdef VSF_I2C_CUSTOM_IRQ_MASK_VALUES
/* Check uniqueness among custom IRQ mask values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_I2C_CFG_IRQ_MASK_CHECK_UNIQUE, VSF_I2C_CUSTOM_IRQ_MASK_VALUES);
#endif

/* Check uniqueness within vsf_i2c_irq_mask_t enum */

/* Check IRQ mask uniqueness - all IRQ mask bits should have different values
 * Mandatory: VSF_I2C_IRQ_MASK_MASTER_TX, VSF_I2C_IRQ_MASK_MASTER_RX,
 *            VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE, VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST,
 *            VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK, VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT,
 *            VSF_I2C_IRQ_MASK_MASTER_START_OR_RESTART_DETECT,
 *            VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT, VSF_I2C_IRQ_MASK_SLAVE_START_OR_RESTART_DETECT,
 *            VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT, VSF_I2C_IRQ_MASK_SLAVE_TX,
 *            VSF_I2C_IRQ_MASK_SLAVE_RX, VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE,
 *            VSF_I2C_IRQ_MASK_SLAVE_RX_OVERFLOW_ERR
 * Note: VSF_I2C_IRQ_MASK_MASTER_ERR and VSF_I2C_IRQ_MASK_SLAVE_ERR are combined error masks
 *       (OR of other error masks) and should not be included in uniqueness checks
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2C_CFG_IRQ_MASK_CHECK_UNIQUE,
    /* Mandatory IRQ masks - always included */
    VSF_I2C_IRQ_MASK_MASTER_TX,
    VSF_I2C_IRQ_MASK_MASTER_RX,
    VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE,
    VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST,
    VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK,
    VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT,
    /* Note: VSF_I2C_IRQ_MASK_MASTER_ERR is excluded - it's a combined mask (OR of above error masks) */
    VSF_I2C_IRQ_MASK_MASTER_START_OR_RESTART_DETECT,
    VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT,
    VSF_I2C_IRQ_MASK_SLAVE_START_OR_RESTART_DETECT,
    VSF_I2C_IRQ_MASK_SLAVE_STOP_DETECT,
    VSF_I2C_IRQ_MASK_SLAVE_TX,
    VSF_I2C_IRQ_MASK_SLAVE_RX,
    VSF_I2C_IRQ_MASK_SLAVE_TRANSFER_COMPLETE
    /* Note: VSF_I2C_IRQ_MASK_SLAVE_ERR is excluded - it's a combined mask (OR of above error mask) */
    /* Implementation Required IRQ masks - conditionally included */
    /* User-defined IRQ masks - appended by user configuration */
#ifdef VSF_I2C_CUSTOM_IRQ_MASK_VALUES
    , VSF_I2C_CUSTOM_IRQ_MASK_VALUES
#endif
);
#endif  /* VSF_I2C_CFG_IRQ_MASK_CHECK_UNIQUE */

/* ==================== CTRL UNIQUENESS CHECKS ==================== */

#ifdef VSF_I2C_CFG_CTRL_CHECK_UNIQUE

#ifdef VSF_I2C_CUSTOM_CTRL_VALUES
/* Check uniqueness among custom ctrl values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_I2C_CFG_CTRL_CHECK_UNIQUE, VSF_I2C_CUSTOM_CTRL_VALUES);
#endif

/* Check ctrl uniqueness - all ctrl values should have different values
 * Placeholder: __VSF_I2C_CTRL_DUMMP
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_I2C_CFG_CTRL_CHECK_UNIQUE,
    /* Placeholder ctrl - always included */
    __VSF_I2C_CTRL_DUMMP
    /* User-defined ctrls - appended by user configuration */
#ifdef VSF_I2C_CUSTOM_CTRL_VALUES
    , VSF_I2C_CUSTOM_CTRL_VALUES
#endif
);
#endif  /* VSF_I2C_CFG_CTRL_CHECK_UNIQUE */

#undef VSF_I2C_CFG_MODE_CHECK_UNIQUE
#undef VSF_I2C_CFG_CMD_CHECK_UNIQUE
#undef VSF_I2C_CFG_IRQ_MASK_CHECK_UNIQUE
#undef VSF_I2C_CFG_CTRL_CHECK_UNIQUE
#undef VSF_I2C_CUSTOM_MODE_VALUES
#undef VSF_I2C_CUSTOM_CMD_VALUES
#undef VSF_I2C_CUSTOM_IRQ_MASK_VALUES
#undef VSF_I2C_CUSTOM_CTRL_VALUES

#endif  /* VSF_HAL_USE_I2C */
