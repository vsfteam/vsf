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

    VSF_DMA_APIS(vsf_remapped_dma)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_dma_init(vsf_remapped_dma_t *dma, vsf_dma_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_init(dma->target, cfg_ptr);
}

void vsf_remapped_dma_fini(vsf_remapped_dma_t *dma)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    vsf_dma_fini(dma->target);
}

vsf_err_t vsf_remapped_dma_get_configuration(vsf_remapped_dma_t *dma, vsf_dma_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_get_configuration(dma->target, cfg_ptr);
}

vsf_dma_capability_t vsf_remapped_dma_capability(vsf_remapped_dma_t *dma)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_capability(dma->target);
}

int8_t vsf_remapped_dma_channel_request(vsf_remapped_dma_t *dma, vsf_dma_channel_hint_t *channel_hint_ptr)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_request(dma->target, channel_hint_ptr);
}

void vsf_remapped_dma_channel_release(vsf_remapped_dma_t *dma, uint8_t channel)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    vsf_dma_channel_release(dma->target, channel);
}

vsf_err_t vsf_remapped_dma_channel_config(vsf_remapped_dma_t *dma, uint8_t channel,
                                 vsf_dma_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_config(dma->target, channel, cfg_ptr);
}

vsf_err_t vsf_remapped_dma_channel_get_configuration(vsf_remapped_dma_t *dma, uint8_t channel,
                                                    vsf_dma_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_get_configuration(dma->target, channel, cfg_ptr);
}

vsf_err_t vsf_remapped_dma_channel_start(vsf_remapped_dma_t *dma, uint8_t channel,
                                         uint32_t src_address, uint32_t dst_address,
                                         uint32_t count)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_start(dma->target, channel, src_address, dst_address, count);
}

vsf_err_t vsf_remapped_dma_channel_sg_config_desc(vsf_remapped_dma_t *dma, uint8_t channel, vsf_dma_isr_t isr,
                                         vsf_dma_channel_sg_desc_t *desc_ptr, uint32_t count)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_sg_config_desc(dma->target, channel, isr, desc_ptr, count);
}

vsf_err_t vsf_remapped_dma_channel_sg_start(vsf_remapped_dma_t *dma, uint8_t channel)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_sg_start(dma->target, channel);
}

vsf_err_t vsf_remapped_dma_channel_cancel(vsf_remapped_dma_t *dma, uint8_t channel)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_cancel(dma->target, channel);
}

uint32_t vsf_remapped_dma_channel_get_transferred_count(vsf_remapped_dma_t *dma, uint8_t channel)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_get_transferred_count(dma->target, channel);
}

vsf_dma_channel_status_t vsf_remapped_dma_channel_status(vsf_remapped_dma_t *dma,
                                                     uint8_t channel)
{
    VSF_HAL_ASSERT((dma != NULL) && (dma->target != NULL));
    return vsf_dma_channel_status(dma->target, channel);
}


#endif
#endif
