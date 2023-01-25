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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_IO == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_IO_CLASS_IMPLEMENT
#include "./vsf_remapped_io.h"

#if VSF_IO_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_IO_CFG_MULTI_CLASS == ENABLED
const vsf_io_op_t vsf_remapped_io_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_IO_APIS(vsf_remapped)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_io_config(vsf_remapped_io_t *io, vsf_io_cfg_t *cfg, uint_fast8_t count)
{
    VSF_HAL_ASSERT((io != NULL) && (io->target != NULL));
    return vsf_io_config(io->target, cfg, count);
}

vsf_err_t vsf_remapped_io_config_one_pin(vsf_remapped_io_t *io, vsf_io_cfg_t *cfg)
{
    VSF_HAL_ASSERT((io != NULL) && (io->target != NULL));
    return vsf_io_config_one_pin(io->target, cfg);
}

#endif
#endif
