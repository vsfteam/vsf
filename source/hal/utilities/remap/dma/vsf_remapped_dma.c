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

#include "hal/vsf_hal.h"

#if VSF_HAL_USE_DMA == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_DMA_CLASS_IMPLEMENT
#include "./vsf_remapped_dma.h"

#if VSF_DMA_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_DMA_CFG_MULTI_CLASS == ENABLED
const vsf_dma_op_t vsf_remapped_dma_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_DMA_APIS(vsf_remapped)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_dma_init(vsf_remapped_dma_t *dma)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_init(dma->target);
}

void vsf_remapped_dma_fini(vsf_remapped_dma_t *dma)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    vsf_dma_fini(dma->target);
}

vsf_dma_capability_t vsf_remapped_dma_capability(vsf_remapped_dma_t *dma)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_capability(dma->target);
}

int8_t vsf_remapped_dma_channel_request(vsf_remapped_dma_t *dma, void *filter_param)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_request(dma->target, filter_param);
}

void vsf_remapped_dma_channel_release(vsf_remapped_dma_t *dma, int8_t channel)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    vsf_dma_channel_release(dma->target, channel);
}

vsf_err_t vsf_remapped_dma_channel_config(vsf_remapped_dma_t *dma, int8_t channel,
                                 vsf_dma_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_config(dma->target, channel, cfg_ptr);
}

vsf_err_t vsf_remapped_dma_channel_cancel(vsf_remapped_dma_t *dma, int8_t channel)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_cancel(dma->target, channel);
}

vsf_dma_channel_status_t vsf_remapped_dma_channel_status(vsf_remapped_dma_t *dma,
                                                     int8_t channel)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_status(dma->target, channel);
}


#endif
#endif
