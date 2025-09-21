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

#if VSF_HAL_USE_I2S == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_I2S_CLASS_IMPLEMENT
#include "./vsf_remapped_i2s.h"

#if VSF_I2S_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_I2S_CFG_MULTI_CLASS == ENABLED
const vsf_i2s_op_t vsf_remapped_i2s_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_I2S_APIS(vsf_remapped_i2s)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_i2s_init(vsf_remapped_i2s_t *i2s, vsf_i2s_cfg_t *i2s_cfg)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_init(i2s->target, i2s_cfg);
}

vsf_err_t vsf_remapped_i2s_get_configuration(vsf_remapped_i2s_t *i2s, vsf_i2s_cfg_t *i2s_cfg)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_get_configuration(i2s->target, i2s_cfg);
}

vsf_err_t vsf_remapped_i2s_tx_init(vsf_remapped_i2s_t *i2s, vsf_i2s_cfg_t *i2s_cfg)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_tx_init(i2s->target, i2s_cfg);
}

void vsf_remapped_i2s_tx_fini(vsf_remapped_i2s_t *i2s)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    vsf_i2s_tx_fini(i2s->target);
}

vsf_err_t vsf_remapped_i2s_tx_get_configuration(vsf_remapped_i2s_t *i2s, vsf_i2s_cfg_t *i2s_cfg)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_tx_get_configuration(i2s->target, i2s_cfg);
}

vsf_err_t vsf_remapped_i2s_tx_start(vsf_remapped_i2s_t *i2s)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_tx_start(i2s->target);
}

vsf_err_t vsf_remapped_i2s_rx_init(vsf_remapped_i2s_t *i2s, vsf_i2s_cfg_t *i2s_cfg)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_rx_init(i2s->target, i2s_cfg);
}

void vsf_remapped_i2s_rx_fini(vsf_remapped_i2s_t *i2s)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    vsf_i2s_rx_fini(i2s->target);
}

vsf_err_t vsf_remapped_i2s_rx_get_configuration(vsf_remapped_i2s_t *i2s, vsf_i2s_cfg_t *i2s_cfg)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_rx_get_configuration(i2s->target, i2s_cfg);
}

vsf_err_t vsf_remapped_i2s_rx_start(vsf_remapped_i2s_t *i2s)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_rx_start(i2s->target);
}

fsm_rt_t vsf_remapped_i2s_enable(vsf_remapped_i2s_t *i2s)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_enable(i2s->target);
}

fsm_rt_t vsf_remapped_i2s_disable(vsf_remapped_i2s_t *i2s)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_disable(i2s->target);
}

vsf_i2s_status_t vsf_remapped_i2s_status(vsf_remapped_i2s_t *i2s)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_status(i2s->target);
}

vsf_i2s_capability_t vsf_remapped_i2s_capability(vsf_remapped_i2s_t *i2s)
{
    VSF_HAL_ASSERT((i2s != NULL) && (i2s->target != NULL));
    return vsf_i2s_capability(i2s->target);
}

#endif
#endif
