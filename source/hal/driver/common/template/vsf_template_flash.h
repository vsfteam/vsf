/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#ifndef __HAL_DRIVER_FLASH_INTERFACE_H__
#define __HAL_DRIVER_FLASH_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
  
//! \name ad configuration
//! @{
typedef struct flash_cfg_t flash_cfg_t;
struct flash_cfg_t {
    uint32_t      start_address;
    uint32_t      end_address;
};
//! @}

//! \name ad channel configuration
//! @{
typedef struct flash_channel_cfg_t flash_channel_cfg_t;
struct flash_channel_cfg_t {
    uint8_t  channel;       // Channel Index
    uint16_t sample_time;
    uint32_t feature;       // Channle Feature
};
//! @}

//! \name class: ad_t
//! @{
def_interface(i_flash_t)
    implement(i_peripheral_t);

    vsf_err_t (*Init)(flash_cfg_t *pCfg);

    vsf_err_t (*Erase)(uint32_fast_taddress_ptr, uint32_fast_t size);
    vsf_err_t (*Write)(uint32_fast_taddress_ptr, uint32_fast_t size);
    vsf_err_t (*Read)(uint32_fast_taddress_ptr, uint32_fast_t size);

end_def_interface(i_flash_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_flash_init(vsf_flash_t *flash_ptr, flash_cfg_t *cfg_ptr);

extern vsf_err_t vsf_flash_enable(vsf_flash_t *flash_ptr);
extern vsf_err_t vsf_flash_disable(vsf_flash_t *flash_ptr);

/**
 * flash erase a continuous range
 * @note offset must be aligend to the start address 
 *       of the erase sector. size must be an integer 
 *       multiple of the minimum erase size
 *
 * @param[in] flash_ptr flash instance
 * @param[in] offset flash address offset
 * @param[in] size flash erase size(bytes)
 */
extern vsf_err_t vsf_flash_erase(vsf_flash_t *flash_ptr,
                                 uint32_fast_t offset,
                                 uint32_fast_t size);

/**
 * flash write a continuous range
 * @note offset must be aligend to the start address 
 *       of the write sector. size must be an integer 
 *       multiple of the minimum write size
 *
 * @param[in] flash_ptr flash instance
 * @param[in] offset flash address offset
 * @param[in] size flash write size(bytes)
 */
extern vsf_err_t vsf_flash_write(vsf_flash_t *flash_ptr,
                                 uint32_fast_t offset,
                                 uint32_fast_t size);

/**
 * flash read a continuous range
 * @note if the flash hardware does not support random read, 
 *       the unaligned range should be returned as a failure.
 *
 * @param[in] flash_ptr flash instance
 * @param[in] offset flash address offset
 * @param[in] size flash write size(bytes)
 */
extern vsf_err_t vsf_flash_read(vsf_flash_t *flash_ptr,
                                uint32_fast_t offset,
                                uint32_fast_t size);

/** TODO: information query API, include:
 * - minimum erase size
 * - minimum write size
 * - whether to allow random read
 */

#ifdef __cplusplus
}
#endif

#endif
