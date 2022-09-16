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

#define VSF_IO_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_IO == ENABLED && VSF_IO_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_io_config_one_pin(vsf_io_t *io_ptr, io_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(io_ptr != NULL);
    VSF_HAL_ASSERT(io_ptr->op != NULL);
    VSF_HAL_ASSERT(io_ptr->op->config_one_pin != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return io_ptr->op->config_one_pin(io_ptr, cfg_ptr);
}

vsf_err_t vsf_io_config(vsf_io_t *io_ptr, io_cfg_t *cfg_ptr, uint_fast8_t count)
{
    VSF_HAL_ASSERT(io_ptr != NULL);
    VSF_HAL_ASSERT(io_ptr->op != NULL);
    VSF_HAL_ASSERT(io_ptr->op->config != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);
    VSF_HAL_ASSERT(count != 0);

    return io_ptr->op->config(io_ptr, cfg_ptr, count);
}

#endif
