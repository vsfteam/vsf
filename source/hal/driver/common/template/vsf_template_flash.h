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

typedef enum vsf_flash_irq_type_t{
    VSF_FLASH_IRQ_ERASE_MASK        = (1 << 0),
    VSF_FLASH_IRQ_WRITE_MASK        = (1 << 1),
    VSF_FLASH_IRQ_READ_MASK         = (1 << 2),

    VSF_FLASH_IRQ_ERASE_ERROR_MASK  = (1 << 3),
    VSF_FLASH_IRQ_WRITE_ERROR_MASK  = (1 << 4),
    VSF_FLASH_IRQ_READ_ERROR_MASK   = (1 << 5),
} vsf_flash_irq_type_t;

typedef struct vsf_flash_t vsf_flash_t;

typedef void vsf_flash_isrhandler_t(void *target_ptr,
                                    vsf_flash_irq_type_t type,
                                    vsf_flash_t *flash_ptr);


typedef struct vsf_flash_isr_t {
    vsf_flash_isrhandler_t *handler_fn;
    void                   *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_flash_isr_t;

//! \name flash channel configuration
//! @{
typedef struct flash_cfg_t flash_cfg_t;
struct flash_cfg_t {
    vsf_flash_isr_t isr;
};
//! @}

//! \name class: flash_t
//! @{
def_interface(i_flash_t)
    implement(i_peripheral_t);

    vsf_err_t (*Init)(flash_cfg_t *pCfg);

    vsf_err_t (*Erase)(uint_fast32_t address, uint_fast32_t size);
    vsf_err_t (*Write)(uint_fast32_t address, uint8_t* buffer, uint_fast32_t size);
    vsf_err_t (*Read)(uint_fast32_t address, uint8_t* buffer, uint_fast32_t size);

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
                                 uint_fast32_t offset,
                                 uint_fast32_t size);

/**
 * flash write a continuous range
 * @note offset must be aligend to the start address
 *       of the write sector. size must be an integer
 *       multiple of the minimum write size
 *
 * @param[in] flash_ptr flash instance
 * @param[in] offset flash address offset
 * @param[out] buffer flash data to write
 * @param[in] size flash write size(bytes)
 */
extern vsf_err_t vsf_flash_write(vsf_flash_t *flash_ptr,
                                 uint_fast32_t offset,
                                 uint8_t* buffer,
                                 uint_fast32_t size);

/**
 * flash read a continuous range
 * @note if the flash hardware does not support random read,
 *       the unaligned range should be returned as a failure.
 *
 * @param[in] flash_ptr flash instance
 * @param[in] offset flash address offset
 * @param[in] buffer flash data to read
 * @param[in] size flash write size(bytes)
 */
extern vsf_err_t vsf_flash_read(vsf_flash_t *flash_ptr,
                                uint_fast32_t offset,
                                uint8_t* buffer,
                                uint_fast32_t size);

/** TODO:
 * information query API, include:
 * - minimum erase size
 * - minimum write size
 * - whether to allow random read
 */

#ifdef __cplusplus
}
#endif

#endif
