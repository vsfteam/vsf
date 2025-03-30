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

#define vsf_real_spi_t          VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_t)
#define vsf_real_spi_capability VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_capability)

#ifndef VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#ifdef VSF_SPI_CFG_IMP_REMAP_PREFIX
#   undef VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY                ENABLED
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

/*============================ MACROS ========================================*/

#undef VSF_SPI_CFG_CAPABILITY_SUPPORT_AUTO_CS
#undef VSF_SPI_CFG_CAPABILITY_SUPPORT_MANUAL_CS
#undef VSF_SPI_CFG_CAPABILITY_CS_COUNT
#undef VSF_SPI_CFG_CAPABILITY_MAX_CLOCK_HZ
#undef VSF_SPI_CFG_CAPABILITY_MIN_CLOCK_HZ
#undef vsf_real_spi_t
#undef vsf_real_spi_capability

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
#   define vsf_imp_spi_status                   VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_status)
#   define vsf_imp_spi_capability               VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_capability)
#   define vsf_imp_spi_cs_active                VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_active)
#   define vsf_imp_spi_cs_inactive              VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cs_inactive)
#   define vsf_imp_spi_fifo_transfer            VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_fifo_transfer)
#   define vsf_imp_spi_request_transfer         VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_request_transfer)
#   define vsf_imp_spi_cancel_transfer          VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_cancel_transfer)
#   define vsf_imp_spi_get_transferred_count    VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_get_transferred_count)
#   define vsf_imp_spi_ctrl                     VSF_MCONNECT(VSF_SPI_CFG_IMP_PREFIX, _spi_ctrl)

#   define vsf_remap_spi_t                      VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_t)
#   define vsf_remap_spi_init                   VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_init)
#   define vsf_remap_spi_enable                 VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_enable)
#   define vsf_remap_spi_disable                VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_disable)
#   define vsf_remap_spi_irq_enable             VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_irq_enable)
#   define vsf_remap_spi_irq_disable            VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_irq_disable)
#   define vsf_remap_spi_status                 VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_status)
#   define vsf_remap_spi_capability             VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_capability)
#   define vsf_remap_spi_cs_active              VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_cs_active)
#   define vsf_remap_spi_cs_inactive            VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_cs_inactive)
#   define vsf_remap_spi_fifo_transfer          VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_fifo_transfer)
#   define vsf_remap_spi_request_transfer       VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_request_transfer)
#   define vsf_remap_spi_cancel_transfer        VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_cancel_transfer)
#   define vsf_remap_spi_get_transferred_count  VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_get_transferred_count)
#   define vsf_remap_spi_ctrl                   VSF_MCONNECT(VSF_SPI_CFG_IMP_REMAP_PREFIX, _spi_ctrl)

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
        }
#endif

/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _spi
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _SPI

#ifndef VSF_SPI_CFG_IMP_PREFIX
#   error "Please define VSF_SPI_CFG_IMP_PREFIX in spi driver"
#endif

#ifndef VSF_SPI_CFG_IMP_UPCASE_PREFIX
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
#undef vsf_imp_spi_status
#undef vsf_imp_spi_capability
#undef vsf_imp_spi_cs_active
#undef vsf_imp_spi_cs_inactive
#undef vsf_imp_spi_fifo_transfer
#undef vsf_imp_spi_request_transfer
#undef vsf_imp_spi_cancel_transfer
#undef vsf_imp_spi_get_transferred_count
#undef vsf_imp_spi_ctrl

#undef vsf_remap_spi_t
#undef vsf_remap_spi_init
#undef vsf_remap_spi_enable
#undef vsf_remap_spi_disable
#undef vsf_remap_spi_irq_enable
#undef vsf_remap_spi_irq_disable
#undef vsf_remap_spi_status
#undef vsf_remap_spi_capability
#undef vsf_remap_spi_cs_active
#undef vsf_remap_spi_cs_inactive
#undef vsf_remap_spi_fifo_transfer
#undef vsf_remap_spi_request_transfer
#undef vsf_remap_spi_cancel_transfer
#undef vsf_remap_spi_get_transferred_count
#undef vsf_remap_spi_ctrl

#endif  /* VSF_HAL_USE_SPI */

