/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                  *
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

#define VSF_DMA_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_DMA == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#if VSF_DMA_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_dma_init(vsf_dma_t *dma_ptr, vsf_dma_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->init != NULL);

    return dma_ptr->op->init(dma_ptr, cfg_ptr);
}

void vsf_dma_fini(vsf_dma_t *dma_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->init != NULL);

    dma_ptr->op->fini(dma_ptr);
}

vsf_err_t vsf_dma_get_configuration(vsf_dma_t *dma_ptr, vsf_dma_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->get_configuration != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return dma_ptr->op->get_configuration(dma_ptr, cfg_ptr);
}

vsf_dma_capability_t vsf_dma_capability(vsf_dma_t *dma_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->capability != NULL);

    return dma_ptr->op->capability(dma_ptr);
}

int8_t vsf_dma_channel_request(vsf_dma_t *dma_ptr, vsf_dma_channel_hint_t *channel_hint_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->channel_request != NULL);

    return dma_ptr->op->channel_request(dma_ptr, channel_hint_ptr);
}

void vsf_dma_channel_release(vsf_dma_t *dma_ptr, uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->channel_release != NULL);

    dma_ptr->op->channel_release(dma_ptr, channel);
}

vsf_err_t vsf_dma_channel_config(vsf_dma_t *dma_ptr, uint8_t channel,
                                 vsf_dma_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->channel_config != NULL);

    return dma_ptr->op->channel_config(dma_ptr, channel, cfg_ptr);
}

vsf_err_t vsf_dma_channel_get_configuration(vsf_dma_t *dma_ptr, uint8_t channel,
                                            vsf_dma_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->channel_get_configuration != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return dma_ptr->op->channel_get_configuration(dma_ptr, channel, cfg_ptr);
}

vsf_err_t vsf_dma_channel_start(vsf_dma_t *dma_ptr, uint8_t channel,
                                uint32_t src_address, uint32_t dst_address,
                                uint32_t count)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->channel_start != NULL);

    return dma_ptr->op->channel_start(dma_ptr, channel, src_address,
                                      dst_address, count);
}

vsf_err_t vsf_dma_channel_sg_config_desc(vsf_dma_t *dma_ptr, uint8_t channel, vsf_dma_isr_t isr,
    vsf_dma_channel_sg_desc_t *sg_desc_ptr, uint32_t sg_count)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->channel_sg_config_desc != NULL);

    return dma_ptr->op->channel_sg_config_desc(dma_ptr, channel, isr,
        sg_desc_ptr, sg_count);
}

vsf_err_t vsf_dma_channel_sg_start(vsf_dma_t *dma_ptr, uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->channel_sg_start != NULL);

    return dma_ptr->op->channel_sg_start(dma_ptr, channel);
}

vsf_err_t vsf_dma_channel_cancel(vsf_dma_t *dma_ptr, uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->channel_cancel != NULL);

    return dma_ptr->op->channel_cancel(dma_ptr, channel);
}

uint32_t vsf_dma_channel_get_transferred_count(vsf_dma_t *dma_ptr, uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->channel_get_transferred_count != NULL);

    return dma_ptr->op->channel_get_transferred_count(dma_ptr, channel);
}

vsf_dma_channel_status_t vsf_dma_channel_status(vsf_dma_t *dma_ptr, uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(dma_ptr->op != NULL);
    VSF_HAL_ASSERT(dma_ptr->op->channel_status != NULL);

    return dma_ptr->op->channel_status(dma_ptr, channel);
}

#endif /* VSF_DMA_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_DMA == ENABLED */
