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

#define vsf_real_dma_t                                VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t)
#define vsf_real_dma_capability                       VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_capability)

#ifndef VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY              DISALBED
#endif

#if VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY == ENABLED
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
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                     _dma
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME              _DMA

#ifndef VSF_DMA_CFG_IMP_PREFIX
#   error "Please define VSF_DMA_CFG_IMP_PREFIX in dma driver"
#endif

#ifndef VSF_DMA_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_DMA_CFG_IMP_UPCASE_PREFIX in dma driver"
#endif

#ifndef VSF_DMA_CFG_IMP_INSTANCE_PREFIX
#   define VSF_DMA_CFG_IMP_INSTANCE_PREFIX         VSF_DMA_CFG_IMP_PREFIX
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_DMA_CFG_IMP_PREFIX
#undef VSF_DMA_CFG_IMP_INSTANCE_PREFIX
#undef VSF_DMA_CFG_IMP_UPCASE_PREFIX
#undef VSF_DMA_CFG_IMP_LV0

#endif  /* VSF_HAL_USE_DMA */
