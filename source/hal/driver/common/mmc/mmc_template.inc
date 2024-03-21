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

#if VSF_HAL_USE_MMC == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define vsf_real_mmc_t                                VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t)
#define vsf_real_mmc_capability                       VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_capability)

#ifndef VSF_MMC_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_MMC_CFG_REIMPLEMENT_API_CAPABILITY              DISALBED
#endif

#if VSF_MMC_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_MMC_CFG_CAPCBILITY_MAX_FREQ_HZ
#       define VSF_MMC_CFG_CAPCBILITY_MAX_FREQ_HZ           (50 * 1000 * 1000)
#   endif
#   ifndef VSF_MMC_CFG_CAPCBILITY_BUS_WIDTH
#       define VSF_MMC_CFG_CAPCBILITY_BUS_WIDTH             MMC_CAP_BUS_WIDTH_8
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_MMC_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_mmc_capability_t vsf_real_mmc_capability(vsf_real_mmc_t *mmc_ptr)
{
    vsf_mmc_capability_t mmc_capability = {
        .max_freq_hz = VSF_MMC_CFG_CAPCBILITY_MAX_FREQ_HZ,
        .bus_width   = VSF_MMC_CFG_CAPCBILITY_BUS_WIDTH,
    };

    return mmc_capability;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_MMC_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_MMC_CFG_CAPCBILITY_MAX_FREQ_HZ
#undef VSF_MMC_CFG_CAPCBILITY_BUS_WIDTH
#undef vsf_real_mmc_t
#undef vsf_real_mmc_capability

/*============================ MACROS ========================================*/

#ifdef VSF_MMC_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_mmc_t                        VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_t)
#   define vsf_imp_mmc_init                     VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_init)
#   define vsf_imp_mmc_enable                   VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_enable)
#   define vsf_imp_mmc_disable                  VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_disable)
#   define vsf_imp_mmc_irq_enable               VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_irq_enable)
#   define vsf_imp_mmc_irq_disable              VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_irq_disable)
#   define vsf_imp_mmc_status                   VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_status)
#   define vsf_imp_mmc_capability               VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_capability)
#   define vsf_imp_mmc_set_clock                VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_set_clock)
#   define vsf_imp_mmc_set_bus_width            VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_set_bus_width)
#   define vsf_imp_mmc_host_transact_start      VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_host_transact_start)
#   define vsf_imp_mmc_host_transact_stop       VSF_MCONNECT(VSF_MMC_CFG_IMP_PREFIX, _mmc_host_transact_stop)

#   define vsf_remap_mmc_t                      VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_t)
#   define vsf_remap_mmc_init                   VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_init)
#   define vsf_remap_mmc_enable                 VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_enable)
#   define vsf_remap_mmc_disable                VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_disable)
#   define vsf_remap_mmc_irq_enable             VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_irq_enable)
#   define vsf_remap_mmc_irq_disable            VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_irq_disable)
#   define vsf_remap_mmc_status                 VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_status)
#   define vsf_remap_mmc_capability             VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_capability)
#   define vsf_remap_mmc_set_clock              VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_set_clock)
#   define vsf_remap_mmc_set_bus_width          VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_set_bus_width)
#   define vsf_remap_mmc_host_transact_start    VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_host_transact_start)
#   define vsf_remap_mmc_host_transact_stop     VSF_MCONNECT(VSF_MMC_CFG_IMP_REMAP_PREFIX, _mmc_host_transact_stop)

#   define VSF_MMC_CFG_IMP_REMAP_FUNCTIONS                                                        \
        vsf_err_t vsf_imp_mmc_init(vsf_imp_mmc_t *mmc_ptr, vsf_mmc_cfg_t *cfg_ptr)                \
        {                                                                                         \
            VSF_HAL_ASSERT((mmc_ptr != NULL) && (cfg_ptr != NULL));                               \
            return vsf_remap_mmc_init(mmc_ptr, cfg_ptr);                                          \
        }                                                                                         \
        void vsf_imp_mmc_fini(vsf_imp_mmc_t *mmc_ptr)                                             \
        {                                                                                         \
            VSF_HAL_ASSERT(mmc_ptr != NULL);                                                      \
            vsf_remap_mmc_fini(mmc_ptr);                                                          \
        }                                                                                         \
        void vsf_imp_mmc_irq_enable(vsf_imp_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)          \
        {                                                                                         \
            VSF_HAL_ASSERT(mmc_ptr != NULL);                                                      \
            vsf_remap_mmc_irq_enable(mmc_ptr, irq_mask);                                          \
        }                                                                                         \
        void vsf_imp_mmc_irq_disable(vsf_imp_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)         \
        {                                                                                         \
            VSF_HAL_ASSERT(mmc_ptr != NULL);                                                      \
            vsf_remap_mmc_irq_disable(mmc_ptr, irq_mask);                                         \
        }                                                                                         \
        vsf_mmc_status_t vsf_imp_mmc_status(vsf_imp_mmc_t *mmc_ptr)                               \
        {                                                                                         \
            VSF_HAL_ASSERT(mmc_ptr != NULL);                                                      \
            return vsf_remap_mmc_status(mmc_ptr);                                                 \
        }                                                                                         \
        vsf_mmc_capability_t vsf_imp_mmc_capability(vsf_imp_mmc_t *mmc_ptr)                       \
        {                                                                                         \
            VSF_HAL_ASSERT(mmc_ptr != NULL);                                                      \
            return vsf_remap_mmc_capability(mmc_ptr);                                             \
        }                                                                                         \
        vsf_err_t vsf_imp_mmc_set_clock(vsf_imp_mmc_t *mmc_ptr, uint32_t clock_hz)                \
        {                                                                                         \
            VSF_HAL_ASSERT(mmc_ptr != NULL);                                                      \
            return vsf_remap_mmc_set_clock(mmc_ptr, clock_hz);                                    \
        }                                                                                         \
        vsf_err_t vsf_imp_mmc_set_bus_width(vsf_imp_mmc_t *mmc_ptr, uint8_t bus_width)            \
        {                                                                                         \
            VSF_HAL_ASSERT(mmc_ptr != NULL);                                                      \
            return vsf_remap_mmc_set_bus_width(mmc_ptr, bus_width);                               \
        }                                                                                         \
        vsf_err_t vsf_imp_mmc_host_transact_start(vsf_imp_mmc_t *mmc_ptr, vsf_mmc_trans_t *trans) \
        {                                                                                         \
            VSF_HAL_ASSERT(mmc_ptr != NULL);                                                      \
            return vsf_remap_mmc_host_transact_start(mmc_ptr, trans);                             \
        }                                                                                         \
        void vsf_imp_mmc_host_transact_stop(vsf_imp_mmc_t *mmc_ptr)                               \
        {                                                                                         \
            VSF_HAL_ASSERT(mmc_ptr != NULL);                                                      \
            vsf_remap_mmc_host_transact_stop(mmc_ptr);                                            \
        }
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _mmc
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _MMC

#ifndef VSF_MMC_CFG_IMP_PREFIX
#   error "Please define VSF_MMC_CFG_IMP_PREFIX in mmc driver"
#endif

#ifndef VSF_MMC_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_MMC_CFG_IMP_UPCASE_PREFIX in mmc driver"
#endif

#ifndef VSF_MMC_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_MMC_CFG_IMP_COUNT_MASK_PREFIX        VSF_MMC_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_MMC_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_MMC_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_MMC_CFG_IMP_PREFIX
#undef VSF_MMC_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_MMC_CFG_IMP_UPCASE_PREFIX
#undef VSF_MMC_CFG_IMP_LV0
#undef VSF_MMC_CFG_IMP_REMAP_FUNCTIONS

#undef vsf_imp_mmc_t
#undef vsf_imp_mmc_init
#undef vsf_imp_mmc_enable
#undef vsf_imp_mmc_disable
#undef vsf_imp_mmc_irq_enable
#undef vsf_imp_mmc_irq_disable
#undef vsf_imp_mmc_status
#undef vsf_imp_mmc_capability
#undef vsf_imp_mmc_set_clock
#undef vsf_imp_mmc_set_bus_width
#undef vsf_imp_mmc_host_transact_start
#undef vsf_imp_mmc_host_transact_stop

#undef vsf_remap_mmc_t
#undef vsf_remap_mmc_init
#undef vsf_remap_mmc_enable
#undef vsf_remap_mmc_disable
#undef vsf_remap_mmc_irq_enable
#undef vsf_remap_mmc_irq_disable
#undef vsf_remap_mmc_status
#undef vsf_remap_mmc_capability
#undef vsf_remap_mmc_set_clock
#undef vsf_remap_mmc_set_bus_width
#undef vsf_remap_mmc_host_transact_start
#undef vsf_remap_mmc_host_transact_stop

#endif  /* VSF_HAL_USE_MMC */
