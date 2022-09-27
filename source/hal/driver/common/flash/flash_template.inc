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

#if VSF_HAL_USE_FLASH == ENABLED

/*============================ INCLUDES ======================================*/

/*============================ MACROS ========================================*/

#define vsf_real_flash_t                    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t)
#define vsf_real_flash_capability           VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_capability)
#define vsf_real_flash_erase_all            VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_all)
#define vsf_real_flash_erase_one_sector     VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_one_sector)
#define vsf_real_flash_erase_multi_sector   VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_multi_sector)
#define vsf_real_flash_write_one_sector     VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_one_sector)
#define vsf_real_flash_write_multi_sector   VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_multi_sector)
#define vsf_real_flash_read_one_sector      VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_read_one_sector)
#define vsf_real_flash_read_multi_sector    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_read_multi_sector)

#ifndef VSF_FLASH_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_FLASH_CFG_REIMPLEMENT_CAPABILITY         DISABLED
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_CAPABILITY == DISABLED
#   ifndef VSF_FLASH_CFG_BASE_ADDRESS
#       define VSF_FLASH_CFG_BASE_ADDRESS               0
#   endif

#   ifndef VSF_FLASH_CFG_MAX_SIZE
#       //error "Please define flash size macro 'VSF_FLASH_CFG_MAX_SIZE' in source files"
#   endif

#   ifndef VSF_FLASH_CFG_SECTORE_WRITE_ANY_ADDRESS
#       define VSF_FLASH_CFG_SECTORE_WRITE_ANY_ADDRESS  0
#   endif

#   ifndef VSF_FLASH_CFG_SECTORE_READ_ANY_ADDRESS
#       define VSF_FLASH_CFG_SECTORE_READ_ANY_ADDRESS   1
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_FLASH_CFG_ERASE_ONE_SECTOR_TEMPLATE == ENABLED
vsf_err_t vsf_real_flash_erase_one_sector(vsf_real_flash_t *real_flash_ptr, vsf_flash_size_t offset)
{
    VSF_ASSERT(real_flash_ptr != 0);

    vsf_flash_capability_t flash_capability = vsf_real_flash_capability(real_flash_ptr);

    VSF_ASSERT((offset % flash_capability.erase_sector_size) == 0);
    VSF_ASSERT(flash_capability.erase_sector_size != 0);

    return vsf_real_flash_erase_multi_sector(real_flash_ptr, offset, flash_capability.erase_sector_size);
}
#endif

#if VSF_FLASH_CFG_ERASE_MULTI_SECTOR_TEMPLATE == ENABLED
vsf_err_t vsf_real_flash_erase_multi_sector(vsf_real_flash_t *real_flash_ptr, vsf_flash_size_t offset, vsf_flash_size_t size)
{
    VSF_ASSERT(real_flash_ptr != 0);

    vsf_flash_capability_t flash_capability = vsf_real_flash_capability(real_flash_ptr);

    VSF_ASSERT((offset % flash_capability.erase_sector_size) == 0);
    VSF_ASSERT(flash_capability.erase_sector_size != 0);
    VSF_ASSERT((size % flash_capability.erase_sector_size) == 0);

    for (vsf_flash_size_t i = 0; i < size; i += flash_capability.erase_sector_size) {
        vsf_err_t err = vsf_real_flash_erase_one_sector(real_flash_ptr, offset + i);
        if (err != VSF_ERR_NONE) {
            return err;
        }
    }

    return VSF_ERR_NONE;
}
#endif

#if VSF_FLASH_CFG_ERASE_ALL_TEMPLATE == ENABLED
vsf_err_t vsf_real_flash_erase_all(vsf_real_flash_t *real_flash_ptr)
{
    VSF_ASSERT(real_flash_ptr != 0);

    vsf_flash_capability_t flash_capability = vsf_real_flash_capability(real_flash_ptr);
    VSF_ASSERT(flash_capability.erase_sector_size > 0);
    VSF_ASSERT(flash_capability.erase_sector_size >= flash_capability.write_sector_size);
    VSF_ASSERT(flash_capability.max_size >= flash_capability.erase_sector_size);

    return vsf_real_flash_erase_multi_sector(real_flash_ptr, 0, flash_capability.max_size);
}
#endif

#if VSF_FLASH_CFG_WRITE_ONE_SECTOR_TEMPLATE == ENABLED
vsf_err_t vsf_real_flash_write_one_sector(vsf_real_flash_t *real_flash_ptr, vsf_flash_size_t offset, uint8_t* buffer, vsf_flash_size_t size)
{
    VSF_ASSERT(real_flash_ptr != 0);

    vsf_flash_capability_t flash_capability = vsf_real_flash_capability(real_flash_ptr);
    VSF_ASSERT(size <= flash_capability.write_sector_size);
    VSF_ASSERT(flash_capability.can_write_any_address || ((offset % flash_capability.write_sector_size) == 0));

    return vsf_real_flash_write_multi_sector(real_flash_ptr, offset, buffer, size);
}
#endif

#if VSF_FLASH_CFG_WRITE_MULTI_SECTOR_TEMPLATE == ENABLED
vsf_err_t vsf_real_flash_write_multi_sector(vsf_real_flash_t *real_flash_ptr, vsf_flash_size_t offset, uint8_t* buffer, vsf_flash_size_t size)
{
    VSF_ASSERT(real_flash_ptr != 0);

    vsf_flash_capability_t flash_capability = vsf_real_flash_capability(real_flash_ptr);
    VSF_ASSERT((size % flash_capability.write_sector_size) == 0);
    VSF_ASSERT(flash_capability.can_write_any_address || ((offset % flash_capability.write_sector_size) == 0));

    for (vsf_flash_size_t i = 0; i < size; i += flash_capability.write_sector_size) {
        vsf_flash_size_t current_size = vsf_min(flash_capability.write_sector_size, size - i);
        vsf_err_t err = vsf_real_flash_write_one_sector(real_flash_ptr, offset + i, buffer + i, current_size);
        if (err != VSF_ERR_NONE) {
            return err;
        }
    }

    return VSF_ERR_NONE;
}
#endif

#if VSF_FLASH_CFG_READ_ONE_SECTOR_TEMPLATE == ENABLED
vsf_err_t vsf_real_flash_read_one_sector(vsf_real_flash_t *real_flash_ptr, vsf_flash_size_t offset, uint8_t* buffer, vsf_flash_size_t size)
{
    VSF_ASSERT(real_flash_ptr != 0);

    vsf_flash_capability_t flash_capability = vsf_real_flash_capability(real_flash_ptr);
    VSF_ASSERT(size <= flash_capability.write_sector_size);
    VSF_ASSERT(flash_capability.can_read_any_address || ((offset % flash_capability.write_sector_size) == 0));

    return vsf_real_flash_read_multi_sector(real_flash_ptr, offset, buffer, size);
}
#endif

#if VSF_FLASH_CFG_READ_MULTI_SECTOR_TEMPLATE == ENABLED
vsf_err_t vsf_real_flash_read_multi_sector(vsf_real_flash_t *real_flash_ptr, vsf_flash_size_t offset, uint8_t* buffer, vsf_flash_size_t size)
{
    VSF_ASSERT(real_flash_ptr != 0);

    vsf_flash_capability_t flash_capability = vsf_real_flash_capability(real_flash_ptr);

    VSF_ASSERT((size % flash_capability.write_sector_size) == 0);
    VSF_ASSERT(flash_capability.can_read_any_address || ((offset % flash_capability.write_sector_size) == 0));

    for (vsf_flash_size_t i = 0; i < size; i += flash_capability.write_sector_size) {
        vsf_flash_size_t current_size = vsf_min(flash_capability.write_sector_size, size - i);
        vsf_err_t err = vsf_real_flash_read_one_sector(real_flash_ptr, offset + i, buffer + i, current_size);
        if (err != VSF_ERR_NONE) {
            return err;
        }
    }

    return VSF_ERR_NONE;
}
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_CAPABILITY == DISABLED
vsf_flash_capability_t vsf_real_flash_capability(vsf_real_flash_t *flash_ptr)
{
    vsf_flash_capability_t flash_capability = {
        .base_address               = VSF_FLASH_CFG_BASE_ADDRESS,
        .max_size                   = VSF_FLASH_CFG_MAX_SIZE,
        .erase_sector_size          = VSF_FLASH_CFG_ERASE_SECTORE_SIZE,
        .write_sector_size          = VSF_FLASH_CFG_WRITE_SECTORE_SIZE,
        .can_write_any_address      = VSF_FLASH_CFG_SECTORE_WRITE_ANY_ADDRESS,
        .can_read_any_address       = VSF_FLASH_CFG_SECTORE_READ_ANY_ADDRESS,
    };

    VSF_ASSERT(flash_capability.erase_sector_size >= flash_capability.write_sector_size);

    return flash_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     _flash
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _FLASH

#ifndef VSF_FLASH_CFG_IMP_PREFIX
#   error "Please define VSF_FLASH_CFG_IMP_PREFIX in flash driver"
#endif

#ifndef VSF_FLASH_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_ADC_CFG_IMP_UPCASE_PREFIX    in flash driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif  /* VSF_HAL_USE_FLASH */
