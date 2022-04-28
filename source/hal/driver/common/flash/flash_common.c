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

#define VSF_FLASH_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_FLASH == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#if VSF_FLASH_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_flash_init(vsf_flash_t *flash_ptr, flash_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->init != NULL);

    return flash_ptr->op->init(flash_ptr, cfg_ptr);
}

fsm_rt_t vsf_flash_enable(vsf_flash_t *flash_ptr)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->enable != NULL);

    return flash_ptr->op->enable(flash_ptr);
}

fsm_rt_t vsf_flash_disable(vsf_flash_t *flash_ptr)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->disable != NULL);

    return flash_ptr->op->disable(flash_ptr);
}

flash_capability_t vsf_flash_capability(vsf_flash_t *flash_ptr)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->capability != NULL);

    return flash_ptr->op->capability(flash_ptr);
}

vsf_err_t vsf_flash_erase(vsf_flash_t *flash_ptr, uint_fast32_t offset, uint_fast32_t size)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->erase != NULL);

    return flash_ptr->op->erase(flash_ptr, offset, size);
}


vsf_err_t vsf_flash_write(vsf_flash_t *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->write != NULL);

    return flash_ptr->op->write(flash_ptr, offset, buffer, size);
}

vsf_err_t vsf_flash_read(vsf_flash_t *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->read != NULL);

    return flash_ptr->op->read(flash_ptr, offset, buffer, size);
}


#endif /* VSF_FLASH_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_FLASH == ENABLED */
