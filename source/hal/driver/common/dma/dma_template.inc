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

#if VSF_HAL_USE_DMA == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define vsf_real_dma_t                                      VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t)
#define vsf_real_dma_capability                             VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_capability)

#ifndef VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY          DISABLED
#endif

#ifndef VSF_DMA_CFG_REIMPLEMENT_API_SCATTER_GATHER_CONFIG
#   define VSF_DMA_CFG_REIMPLEMENT_API_SCATTER_GATHER_CONFIG DISABLED
#endif

#ifndef VSF_DMA_CFG_REIMPLEMENT_API_SCATTER_GATHER_START
#   define VSF_DMA_CFG_REIMPLEMENT_API_SCATTER_GATHER_START DISABLED
#endif

#ifdef VSF_DMA_CFG_IMP_REMAP_PREFIX
#   undef VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY           ENABLED
#endif

#if VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_DMA_CFG_CAPABILITY_IRQ_MASK
#       define VSF_DMA_CFG_CAPABILITY_IRQ_MASK              VSF_DMA_IRQ_ALL_BITS_MASK
#   endif
#   ifndef VSF_DMA_CFG_CAPABILITY_MAX_REQUEST_COUNT
#       define VSF_DMA_CFG_CAPABILITY_MAX_REQUEST_COUNT     0xFFFFFFFF
#   endif
#   ifndef VSF_DMA_CFG_CAPABILITY_CHANNEL_COUNT
#       define VSF_DMA_CFG_CAPABILITY_CHANNEL_COUNT         8
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_dma_capability_t vsf_real_dma_capability(vsf_real_dma_t *dma_ptr)
{
    vsf_dma_capability_t dma_capability = {
        .irq_mask           = VSF_DMA_CFG_CAPABILITY_IRQ_MASK,
        .max_request_count  = VSF_DMA_CFG_CAPABILITY_MAX_REQUEST_COUNT,
        .channel_count      = VSF_DMA_CFG_CAPABILITY_CHANNEL_COUNT,
    };

    return dma_capability;
}
#endif

#if VSF_DMA_CFG_REIMPLEMENT_API_SCATTER_GATHER_CONFIG == DISABLED
vsf_err_t vsf_dma_channel_scatter_gather_config(vsf_dma_t *dma_ptr, uint8_t channel, vsf_dma_isr_t isr,
            vsf_dma_channel_scatter_gather_desc_t *scatter_gather_desc_ptr, uint32_t scatter_gather_count)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(scatter_gather_desc_ptr != NULL);
    VSF_HAL_ASSERT(scatter_gather_count > 0);

    if ((NULL == dma_ptr) || (NULL == scatter_gather_desc_ptr) || (scatter_gather_count == 0)) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    // TODO
    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_DMA_CFG_REIMPLEMENT_API_SCATTER_GATHER_START == DISABLED
vsf_err_t vsf_dma_channel_scatter_gather_start(vsf_dma_t *dma_ptr, uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);

    if (NULL == dma_ptr) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    // TODO
    return VSF_ERR_NOT_SUPPORT;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_DMA_CFG_CAPABILITY_IRQ_MASK
#undef VSF_DMA_CFG_CAPABILITY_MAX_REQUEST_COUNT
#undef VSF_DMA_CFG_CAPABILITY_CHANNEL_COUNT
#undef vsf_real_dma_t
#undef vsf_real_dma_capability

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ MACROS ========================================*/


#ifdef VSF_DMA_CFG_IMP_REMAP_PREFIX
#   define vsf_imp_dma_t                                VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t)
#   define vsf_imp_dma_init                             VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_init)
#   define vsf_imp_dma_fini                             VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_fini)
#   define vsf_imp_dma_capability                       VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_capability)
#   define vsf_imp_dma_channel_request                  VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_request)
#   define vsf_imp_dma_channel_release                  VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_release)
#   define vsf_imp_dma_channel_config                   VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_config)
#   define vsf_imp_dma_channel_start                    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_start)
#   define vsf_imp_dma_channel_cancel                   VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_cancel)
#   define vsf_imp_dma_channel_get_transferred_count    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_get_transferred_count)
#   define vsf_imp_dma_channel_status                   VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_status)

#   define vsf_remap_dma_t                              VSF_MCONNECT(VSF_DMA_CFG_IMP_REMAP_PREFIX, _dma_t)
#   define vsf_remap_dma_init                           VSF_MCONNECT(VSF_DMA_CFG_IMP_REMAP_PREFIX, _dma_init)
#   define vsf_remap_dma_fini                           VSF_MCONNECT(VSF_DMA_CFG_IMP_REMAP_PREFIX, _dma_fini)
#   define vsf_remap_dma_capability                     VSF_MCONNECT(VSF_DMA_CFG_IMP_REMAP_PREFIX, _dma_capability)
#   define vsf_remap_dma_channel_request                VSF_MCONNECT(VSF_DMA_CFG_IMP_REMAP_PREFIX, _dma_channel_request)
#   define vsf_remap_dma_channel_release                VSF_MCONNECT(VSF_DMA_CFG_IMP_REMAP_PREFIX, _dma_channel_release)
#   define vsf_remap_dma_channel_config                 VSF_MCONNECT(VSF_DMA_CFG_IMP_REMAP_PREFIX, _dma_channel_config)
#   define vsf_remap_dma_channel_start                  VSF_MCONNECT(VSF_DMA_CFG_IMP_REMAP_PREFIX, _dma_channel_start)
#   define vsf_remap_dma_channel_cancel                 VSF_MCONNECT(VSF_DMA_CFG_IMP_REMAP_PREFIX, _dma_channel_cancel)
#   define vsf_remap_dma_channel_get_transferred_count  VSF_MCONNECT(VSF_DMA_CFG_IMP_REMAP_PREFIX, _dma_channel_get_transferred_count)
#   define vsf_remap_dma_channel_status                 VSF_MCONNECT(VSF_DMA_CFG_IMP_REMAP_PREFIX, _dma_channel_status)

#   define VSF_DMA_CFG_IMP_REMAP_FUNCTIONS                                                            \
        vsf_err_t vsf_imp_dma_init(vsf_imp_dma_t *dma_ptr)                              \
        {                                                                               \
            VSF_HAL_ASSERT(dma_ptr != NULL);                                            \
            return vsf_remap_dma_init(dma_ptr);                                         \
        }                                                                               \
        void vsf_imp_dma_fini(vsf_imp_dma_t *dma_ptr)                                   \
        {                                                                               \
            VSF_HAL_ASSERT(dma_ptr != NULL);                                            \
            vsf_remap_dma_fini(dma_ptr);                                                \
        }                                                                               \
        vsf_dma_capability_t vsf_imp_dma_capability(vsf_imp_dma_t *dma_ptr)             \
        {                                                                               \
            VSF_HAL_ASSERT(dma_ptr != NULL);                                            \
            return vsf_remap_dma_capability(dma_ptr);                                   \
        }                                                                               \
        int8_t vsf_imp_dma_channel_request(vsf_imp_dma_t *dma_ptr, void *filter_param)  \
        {                                                                               \
            VSF_HAL_ASSERT(dma_ptr != NULL);                                            \
            return vsf_remap_dma_channel_request(dma_ptr, filter_param);                \
        }                                                                               \
        void vsf_imp_dma_channel_release(vsf_imp_dma_t *dma_ptr, int8_t channel)        \
        {                                                                               \
            VSF_HAL_ASSERT(dma_ptr != NULL);                                            \
            vsf_remap_dma_channel_release(dma_ptr, channel);                            \
        }                                                                               \
        vsf_err_t vsf_imp_dma_channel_config(vsf_imp_dma_t *dma_ptr, int8_t channel,    \
                                         vsf_dma_channel_cfg_t *cfg_ptr)                \
        {                                                                               \
            VSF_HAL_ASSERT(dma_ptr != NULL);                                            \
            return vsf_remap_dma_channel_config(dma_ptr, channel, cfg_ptr);             \
        }                                                                               \
        vsf_err_t vsf_imp_dma_channel_start(vsf_imp_dma_t *dma_ptr, int8_t channel,     \
                                            uint32_t src_address, uint32_t dst_address, \
                                            uint32_t count)                             \
        {                                                                               \
            VSF_HAL_ASSERT(dma_ptr != NULL);                                            \
            return vsf_remap_dma_channel_start(dma_ptr, channel, src_address,           \
                                               dst_address, count);                     \
        }                                                                               \
        vsf_err_t vsf_imp_dma_channel_cancel(vsf_imp_dma_t *dma_ptr, int8_t channel)    \
        {                                                                               \
            VSF_HAL_ASSERT(dma_ptr != NULL);                                            \
            return vsf_remap_dma_channel_cancel(dma_ptr, channel);                      \
        }                                                                               \
        vsf_err_t vsf_imp_dma_channel_get_transferred_count(vsf_imp_dma_t *dma_ptr,     \
                                                            int8_t channel)             \
        {                                                                               \
            VSF_HAL_ASSERT(dma_ptr != NULL);                                            \
            return vsf_remap_dma_channel_get_transferred_count(dma_ptr, channel);       \
        }                                                                               \
        vsf_dma_channel_status_t vsf_imp_dma_channel_status(vsf_imp_dma_t *dma_ptr,     \
                                                             int8_t channel)            \
        {                                                                               \
            VSF_HAL_ASSERT(dma_ptr != NULL);                                            \
            return vsf_remap_dma_channel_status(dma_ptr, channel);                      \
        }
#endif


/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _dma
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _DMA

#ifndef VSF_DMA_CFG_IMP_PREFIX
#   error "Please define VSF_DMA_CFG_IMP_PREFIX in dma driver"
#endif

#ifndef VSF_DMA_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_DMA_CFG_IMP_UPCASE_PREFIX in dma driver"
#endif

#ifndef VSF_DMA_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_DMA_CFG_IMP_COUNT_MASK_PREFIX        VSF_DMA_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_DMA_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_DMA_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_DMA_CFG_IMP_PREFIX
#undef VSF_DMA_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_DMA_CFG_IMP_UPCASE_PREFIX
#undef VSF_DMA_CFG_IMP_LV0
#undef VSF_DMA_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_DMA_CFG_IMP_HAS_OP
#undef VSF_DMA_CFG_IMP_EXTERN_OP

#undef vsf_imp_dma_t
#undef vsf_imp_dma_init
#undef vsf_imp_dma_fini
#undef vsf_imp_dma_capability
#undef vsf_imp_dma_channel_request
#undef vsf_imp_dma_channel_release
#undef vsf_imp_dma_channel_config
#undef vsf_imp_dma_channel_start
#undef vsf_imp_dma_channel_cancel
#undef vsf_imp_dma_channel_get_transferred_count
#undef vsf_imp_dma_channel_status

#undef vsf_remap_dma_t
#undef vsf_remap_dma_init
#undef vsf_remap_dma_fini
#undef vsf_remap_dma_capability
#undef vsf_remap_dma_channel_request
#undef vsf_remap_dma_channel_release
#undef vsf_remap_dma_channel_config
#undef vsf_remap_dma_channel_start
#undef vsf_remap_dma_channel_cancel
#undef vsf_remap_dma_channel_get_transferred_count
#undef vsf_remap_dma_channel_status

#endif  /* VSF_HAL_USE_DMA */
