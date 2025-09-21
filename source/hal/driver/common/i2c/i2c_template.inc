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

#ifdef VSF_I2C_CFG_IMP_REMAP_PREFIX
#   undef VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY
#   undef VSF_I2C_CFG_REIMPLEMENT_API_CTRL
#   undef VSF_I2C_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY               ENABLED
#   define VSF_I2C_CFG_REIMPLEMENT_API_GET_CONFIGURATION        ENABLED
#   define VSF_I2C_CFG_REIMPLEMENT_API_CTRL                     ENABLED
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

/*============================ MACROS ========================================*/

#undef VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_I2C_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_I2C_CFG_CAPABILITY_IRQ_MASK
#undef VSF_I2C_CFG_CAPABILITY_SUPPORT_NO_START
#undef VSF_I2C_CFG_CAPABILITY_SUPPORT_NO_STOP_RESTART
#undef VSF_I2C_CFG_CAPABILITY_SUPPORT_RESTART
#undef VSF_I2C_CFG_CAPABILITY_MAX_TRANSFER_SIZE
#undef VSF_I2C_CFG_CAPABILITY_MIN_TRANSFER_SIZE

#undef vsf_real_i2c_t
#undef vsf_real_i2c_capability
#undef vsf_real_i2c_ctrl
#undef vsf_real_i2c_get_configuration

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
#   define vsf_imp_i2c_master_request           VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_master_request)
#   define vsf_imp_i2c_get_transferred_count    VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_get_transferred_count)
#   define vsf_imp_i2c_ctrl                     VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_ctrl)
#   define vsf_imp_i2c_get_configuration        VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_get_configuration)

#   define vsf_remap_i2c_t                      VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_t)
#   define vsf_remap_i2c_init                   VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_init)
#   define vsf_remap_i2c_fini                   VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_fini)
#   define vsf_remap_i2c_enable                 VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_enable)
#   define vsf_remap_i2c_disable                VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_disable)
#   define vsf_remap_i2c_irq_enable             VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_irq_enable)
#   define vsf_remap_i2c_irq_disable            VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_irq_disable)
#   define vsf_remap_i2c_status                 VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_status)
#   define vsf_remap_i2c_capability             VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_capability)
#   define vsf_remap_i2c_master_request         VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_master_request)
#   define vsf_remap_i2c_get_transferred_count  VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_get_transferred_count)
#   define vsf_remap_i2c_ctrl                   VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_ctrl)
#   define vsf_remap_i2c_get_configuration      VSF_MCONNECT(VSF_I2C_CFG_IMP_REMAP_PREFIX, _i2c_get_configuration)

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
        vsf_err_t vsf_imp_i2c_get_configuration(vsf_imp_i2c_t *i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)  \
        {                                                                                          \
            VSF_HAL_ASSERT(i2c_ptr != NULL);                                                       \
            return vsf_remap_i2c_get_configuration(i2c_ptr, cfg_ptr);                              \
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

#endif  /* VSF_HAL_USE_I2C */
