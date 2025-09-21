/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                      *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                 *
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

#define VSF_I2S_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_I2S == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_I2S_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_i2s_init(vsf_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->init != NULL);

    return i2s_ptr->op->init(i2s_ptr, cfg_ptr);
}

vsf_err_t vsf_i2s_tx_init(vsf_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->tx_init != NULL);

    return i2s_ptr->op->tx_init(i2s_ptr, cfg_ptr);
}

void vsf_i2s_tx_fini(vsf_i2s_t *i2s_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->tx_fini != NULL);

    i2s_ptr->op->tx_fini(i2s_ptr);
}

vsf_err_t vsf_i2s_tx_get_configuration(vsf_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->tx_get_configuration != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return i2s_ptr->op->tx_get_configuration(i2s_ptr, cfg_ptr);
}

vsf_err_t vsf_i2s_tx_start(vsf_i2s_t *i2s_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->tx_start != NULL);

    return i2s_ptr->op->tx_start(i2s_ptr);
}

vsf_err_t vsf_i2s_rx_init(vsf_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->rx_init != NULL);

    return i2s_ptr->op->rx_init(i2s_ptr, cfg_ptr);
}

void vsf_i2s_rx_fini(vsf_i2s_t *i2s_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->rx_fini != NULL);

    i2s_ptr->op->rx_fini(i2s_ptr);
}

vsf_err_t vsf_i2s_rx_get_configuration(vsf_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->rx_get_configuration != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return i2s_ptr->op->rx_get_configuration(i2s_ptr, cfg_ptr);
}

vsf_err_t vsf_i2s_rx_start(vsf_i2s_t *i2s_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->rx_start != NULL);

    return i2s_ptr->op->rx_start(i2s_ptr);
}

fsm_rt_t vsf_i2s_enable(vsf_i2s_t *i2s_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->enable != NULL);

    return i2s_ptr->op->enable(i2s_ptr);
}

fsm_rt_t vsf_i2s_disable(vsf_i2s_t *i2s_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->disable != NULL);

    return i2s_ptr->op->disable(i2s_ptr);
}

vsf_i2s_status_t vsf_i2s_status(vsf_i2s_t *i2s_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->status != NULL);

    return i2s_ptr->op->status(i2s_ptr);
}

vsf_i2s_capability_t vsf_i2s_capability(vsf_i2s_t *i2s_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->capability != NULL);

    return i2s_ptr->op->capability(i2s_ptr);
}

vsf_err_t vsf_i2s_get_configuration(vsf_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(i2s_ptr != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op != NULL);
    VSF_HAL_ASSERT(i2s_ptr->op->get_configuration != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return i2s_ptr->op->get_configuration(i2s_ptr, cfg_ptr);
}

#endif /* VSF_I2S_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_I2S == ENABLED */
