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

vsf_err_t vsf_flash_init(vsf_flash_t *flash_ptr, vsf_flash_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->init != NULL);

    return flash_ptr->op->init(flash_ptr, cfg_ptr);
}

void vsf_flash_fini(vsf_flash_t *flash_ptr)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->init != NULL);

    flash_ptr->op->fini(flash_ptr);
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

void vsf_flash_irq_enable(vsf_flash_t *flash_ptr, vsf_flash_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->irq_enable != NULL);

    flash_ptr->op->irq_enable(flash_ptr, irq_mask);
}

void vsf_flash_irq_disable(vsf_flash_t *flash_ptr, vsf_flash_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->irq_disable != NULL);

    flash_ptr->op->irq_disable(flash_ptr, irq_mask);
}

vsf_flash_status_t vsf_flash_status(vsf_flash_t *flash_ptr)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->status != NULL);

    return flash_ptr->op->status(flash_ptr);
}

vsf_flash_capability_t vsf_flash_capability(vsf_flash_t *flash_ptr)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->capability != NULL);

    return flash_ptr->op->capability(flash_ptr);
}

vsf_err_t vsf_flash_erase_one_sector(vsf_flash_t *flash_ptr, uint_fast32_t offset)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->erase_one_sector != NULL);

    return flash_ptr->op->erase_one_sector(flash_ptr, offset);
}

vsf_err_t vsf_flash_erase_multi_sector(vsf_flash_t *flash_ptr, uint_fast32_t offset, uint_fast32_t size)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->erase_multi_sector != NULL);

    return flash_ptr->op->erase_multi_sector(flash_ptr, offset, size);
}

vsf_err_t vsf_flash_erase_all(vsf_flash_t *flash_ptr)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->erase_all != NULL);

    return flash_ptr->op->erase_all(flash_ptr);
}

vsf_err_t vsf_flash_write_one_sector(vsf_flash_t *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->write_one_sector != NULL);

    return flash_ptr->op->write_one_sector(flash_ptr, offset, buffer, size);
}

vsf_err_t vsf_flash_write_multi_sector(vsf_flash_t *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->write_multi_sector != NULL);

    return flash_ptr->op->write_multi_sector(flash_ptr, offset, buffer, size);
}

vsf_err_t vsf_flash_read_one_sector(vsf_flash_t *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->read_one_sector != NULL);

    return flash_ptr->op->read_one_sector(flash_ptr, offset, buffer, size);
}

vsf_err_t vsf_flash_read_multi_sector(vsf_flash_t *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->read_multi_sector != NULL);

    return flash_ptr->op->read_multi_sector(flash_ptr, offset, buffer, size);
}

vsf_err_t vsf_flash_get_configuration(vsf_flash_t *flash_ptr, vsf_flash_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(flash_ptr->op != NULL);
    VSF_HAL_ASSERT(flash_ptr->op->get_configuration != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return flash_ptr->op->get_configuration(flash_ptr, cfg_ptr);
}

#endif /* VSF_FLASH_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_FLASH == ENABLED */
