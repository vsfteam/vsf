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

#ifndef VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY         DISABLED
#endif

#ifdef VSF_SDIO_CFG_IMP_REMAP_PREFIX
#   undef VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY          ENABLED
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

/*============================ MACROS ========================================*/

#undef VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_SDIO_CFG_CAPCBILITY_MAX_FREQ_HZ
#undef VSF_SDIO_CFG_CAPCBILITY_BUS_WIDTH
#undef vsf_real_sdio_t
#undef vsf_real_sdio_capability

/*============================ MACROS ========================================*/

#ifdef VSF_SDIO_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_sdio_t                       VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t)
#   define vsf_imp_sdio_init                    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_init)
#   define vsf_imp_sdio_enable                  VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_enable)
#   define vsf_imp_sdio_disable                 VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_disable)
#   define vsf_imp_sdio_irq_enable              VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_irq_enable)
#   define vsf_imp_sdio_irq_disable             VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_irq_disable)
#   define vsf_imp_sdio_status                  VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_status)
#   define vsf_imp_sdio_capability              VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_capability)
#   define vsf_imp_sdio_set_clock               VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_set_clock)
#   define vsf_imp_sdio_set_bus_width           VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_set_bus_width)
#   define vsf_imp_sdio_host_transact_start     VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_host_transact_start)
#   define vsf_imp_sdio_host_transact_stop      VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_host_transact_stop)

#   define vsf_remap_sdio_t                     VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_t)
#   define vsf_remap_sdio_init                  VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_init)
#   define vsf_remap_sdio_enable                VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_enable)
#   define vsf_remap_sdio_disable               VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_disable)
#   define vsf_remap_sdio_irq_enable            VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_irq_enable)
#   define vsf_remap_sdio_irq_disable           VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_irq_disable)
#   define vsf_remap_sdio_status                VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_status)
#   define vsf_remap_sdio_capability            VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_capability)
#   define vsf_remap_sdio_set_clock             VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_set_clock)
#   define vsf_remap_sdio_set_bus_width         VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_set_bus_width)
#   define vsf_remap_sdio_host_transact_start   VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_host_transact_start)
#   define vsf_remap_sdio_host_transact_stop    VSF_MCONNECT(VSF_SDIO_CFG_IMP_REMAP_PREFIX, _sdio_host_transact_stop)

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
        vsf_err_t vsf_imp_sdio_host_transact_start(vsf_imp_sdio_t *sdio_ptr, vsf_sdio_trans_t *trans)\
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            return vsf_remap_sdio_host_transact_start(sdio_ptr, trans);                         \
        }                                                                                       \
        void vsf_imp_sdio_host_transact_stop(vsf_imp_sdio_t *sdio_ptr)                          \
        {                                                                                       \
            VSF_HAL_ASSERT(sdio_ptr != NULL);                                                   \
            vsf_remap_sdio_host_transact_stop(sdio_ptr);                                        \
        }
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _sdio
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _SDIO

#ifndef VSF_SDIO_CFG_IMP_PREFIX
#   error "Please define VSF_SDIO_CFG_IMP_PREFIX in sdio driver"
#endif

#ifndef VSF_SDIO_CFG_IMP_UPCASE_PREFIX
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
#undef vsf_imp_sdio_status
#undef vsf_imp_sdio_capability
#undef vsf_imp_sdio_set_clock
#undef vsf_imp_sdio_set_bus_width
#undef vsf_imp_sdio_host_transact_start
#undef vsf_imp_sdio_host_transact_stop

#undef vsf_remap_sdio_t
#undef vsf_remap_sdio_init
#undef vsf_remap_sdio_enable
#undef vsf_remap_sdio_disable
#undef vsf_remap_sdio_irq_enable
#undef vsf_remap_sdio_irq_disable
#undef vsf_remap_sdio_status
#undef vsf_remap_sdio_capability
#undef vsf_remap_sdio_set_clock
#undef vsf_remap_sdio_set_bus_width
#undef vsf_remap_sdio_host_transact_start
#undef vsf_remap_sdio_host_transact_stop

#endif  /* VSF_HAL_USE_SDIO */
