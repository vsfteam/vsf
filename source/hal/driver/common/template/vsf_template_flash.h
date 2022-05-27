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

#ifndef VSF_FLASH_CFG_MULTI_CLASS
#   define VSF_FLASH_CFG_MULTI_CLASS              DISABLED
#endif

// Turn off multi class support for the current implementation
// when the VSF_FLASH_CFG_MULTI_CLASS is enabled
#ifndef VSF_FLASH_CFG_IMPLEMENT_OP
#   if VSF_FLASH_CFG_MULTI_CLASS == ENABLED
#       define VSF_FLASH_CFG_IMPLEMENT_OP         ENABLED
#   else
#       define VSF_FLASH_CFG_IMPLEMENT_OP         DISABLED
#   endif
#endif

// VSF_FLASH_CFG_PREFIX: use for macro vsf_flash_{init, enable, ...}
#ifndef VSF_FLASH_CFG_PREFIX
#   if VSF_FLASH_CFG_MULTI_CLASS == ENABLED
#       define VSF_FLASH_CFG_PREFIX               vsf
#   elif defined(VSF_HW_FLASH_COUNT) && (VSF_HW_FLASH_COUNT != 0)
#       define VSF_FLASH_CFG_PREFIX               vsf_hw
#   endif
#endif

#ifndef VSF_FLASH_CFG_FUNCTION_RENAME
#   define VSF_FLASH_CFG_FUNCTION_RENAME            ENABLED
#endif

#ifndef VSF_FLASH_CFG_REIMPLEMENT_IRQ_TYPE
#   define VSF_FLASH_CFG_REIMPLEMENT_IRQ_TYPE       DISABLED
#endif

#ifndef VSF_FLASH_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_FLASH_CFG_REIMPLEMENT_CAPABILITY   DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_FLASH_APIS(__prefix_name)                                                                                                                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          flash, init,      VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, flash_cfg_t *cfg_ptr)                                      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,           flash, enable,    VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr)                                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,           flash, disable,   VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr)                                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, flash_capability_t, flash, capability,VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr)                                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          flash, erase,     VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, uint_fast32_t offset, uint_fast32_t size)                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          flash, write,     VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          flash, read,      VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size)

/*============================ TYPES =========================================*/

#if VSF_FLASH_CFG_REIMPLEMENT_IRQ_TYPE == DISABLED
typedef enum vsf_flash_irq_mask_t{
    VSF_FLASH_IRQ_ERASE_MASK        = (1 << 0),
    VSF_FLASH_IRQ_WRITE_MASK        = (1 << 1),
    VSF_FLASH_IRQ_READ_MASK         = (1 << 2),

    VSF_FLASH_IRQ_ERASE_ERROR_MASK  = (1 << 3),
    VSF_FLASH_IRQ_WRITE_ERROR_MASK  = (1 << 4),
    VSF_FLASH_IRQ_READ_ERROR_MASK   = (1 << 5),
} vsf_flash_irq_mask_t;
#endif

typedef struct vsf_flash_t vsf_flash_t;

typedef void vsf_flash_isr_handler_t(void *target_ptr,
                                     vsf_flash_irq_mask_t type,
                                     vsf_flash_t *flash_ptr);

typedef struct vsf_flash_isr_t {
    vsf_flash_isr_handler_t *handler_fn;
    void                    *target_ptr;
    vsf_arch_prio_t          prio;
} vsf_flash_isr_t;

typedef struct flash_cfg_t {
    vsf_flash_isr_t isr;
} flash_cfg_t;

#if VSF_FLASH_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct flash_capability_t {
    inherit(peripheral_capability_t)
} flash_capability_t;
#endif

typedef struct vsf_flash_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_FLASH_APIS(vsf)
} vsf_flash_op_t;

#if VSF_FLASH_CFG_MULTI_CLASS == ENABLED
struct vsf_flash_t  {
    const vsf_flash_op_t * op;
};
#endif


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

extern fsm_rt_t vsf_flash_enable(vsf_flash_t *flash_ptr);

extern fsm_rt_t vsf_flash_disable(vsf_flash_t *flash_ptr);

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

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_FLASH_CFG_FUNCTION_RENAME == ENABLED
#   define vsf_flash_init(__FLASH, ...)                                         \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_init)     ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_enable(__FLASH)                                            \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_enable)   ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH)
#   define vsf_flash_disable(__FLASH)                                           \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_disable)  ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH)
#   define vsf_flash_erase(__FLASH, ...)                                        \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase)    ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_write(__FLASH, ...)                                        \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_write)    ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_read(__FLASH, ...)                                         \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_read)     ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_FLASH_INTERFACE_H__*/
