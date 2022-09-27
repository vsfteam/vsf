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

// application code can redefine it
#ifndef VSF_FLASH_CFG_PREFIX
#   if defined(VSF_HW_FLASH_COUNT) && (VSF_HW_FLASH_COUNT != 0)
#       define VSF_FLASH_CFG_PREFIX                     vsf_hw
#   else
#       define VSF_FLASH_CFG_PREFIX                     vsf
#   endif
#endif

// multi-class support enabled by default for maximum availability.
#ifndef VSF_FLASH_CFG_MULTI_CLASS
#   define VSF_FLASH_CFG_MULTI_CLASS                    ENABLED
#endif

#ifndef VSF_FLASH_CFG_FUNCTION_RENAME
#   define VSF_FLASH_CFG_FUNCTION_RENAME                ENABLED
#endif

#ifndef VSF_FLASH_CFG_REIMPLEMENT_IRQ_TYPE
#   define VSF_FLASH_CFG_REIMPLEMENT_IRQ_TYPE           DISABLED
#endif

#ifndef VSF_FLASH_CFG_REIMPLEMENT_FLASH_SIZE_TYPE
#   define VSF_FLASH_CFG_REIMPLEMENT_FLASH_SIZE_TYPE    DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_FLASH_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, init,                  VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               flash, enable,                VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               flash, disable,               VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_flash_capability_t, flash, capability,            VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, erase_one_sector,      VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, erase_multi_sector,    VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, erase_all,             VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, write_one_sector,      VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, write_multi_sector,    VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, read_one_sector,       VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, read_multi_sector,     VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes)

/*============================ TYPES =========================================*/

#if VSF_FLASH_CFG_REIMPLEMENT_IRQ_TYPE == DISABLED
typedef enum vsf_flash_irq_mask_t {
    VSF_FLASH_IRQ_ERASE_MASK        = (1 << 0),
    VSF_FLASH_IRQ_WRITE_MASK        = (1 << 1),
    VSF_FLASH_IRQ_READ_MASK         = (1 << 2),

    VSF_FLASH_IRQ_ERASE_ERROR_MASK  = (1 << 3),
    VSF_FLASH_IRQ_WRITE_ERROR_MASK  = (1 << 4),
    VSF_FLASH_IRQ_READ_ERROR_MASK   = (1 << 5),
} vsf_flash_irq_mask_t;
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_FLASH_SIZE_TYPE == DISABLED
typedef uint_fast32_t vsf_flash_size_t;
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

typedef struct vsf_flash_cfg_t {
    vsf_flash_isr_t isr;
} vsf_flash_cfg_t;

#if VSF_FLASH_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct vsf_flash_capability_t {
    inherit(vsf_peripheral_capability_t)
    vsf_flash_size_t base_address;
    vsf_flash_size_t max_size;
    vsf_flash_size_t erase_sector_size;
    vsf_flash_size_t write_sector_size;
    struct {
        // there are some flash only supports writing from the first address of the sector
        uint8_t can_write_any_address : 1;
        uint8_t can_read_any_address  : 1;
    };
} vsf_flash_capability_t;
#endif

typedef struct vsf_flash_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_FLASH_APIS(vsf)
} vsf_flash_op_t;

#if VSF_FLASH_CFG_MULTI_CLASS == ENABLED
struct vsf_flash_t {
    const vsf_flash_op_t * op;
};
#endif


//! \name class: flash_t
//! @{
def_interface(i_flash_t)
    implement(i_peripheral_t);

    vsf_err_t (*Init)(vsf_flash_cfg_t *pCfg);

    // Provides byte-level random reads and writes
    vsf_err_t (*Erase)(vsf_flash_size_t address, vsf_flash_size_t size);
    vsf_err_t (*Write)(vsf_flash_size_t address, uint8_t* buffer, vsf_flash_size_t size);
    vsf_err_t (*Read)(vsf_flash_size_t address, uint8_t* buffer, vsf_flash_size_t size);

end_def_interface(i_flash_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_flash_init(vsf_flash_t *flash_ptr, vsf_flash_cfg_t *cfg_ptr);

extern fsm_rt_t vsf_flash_enable(vsf_flash_t *flash_ptr);

extern fsm_rt_t vsf_flash_disable(vsf_flash_t *flash_ptr);

extern vsf_err_t vsf_flash_erase_one_sector(vsf_flash_t *flash_ptr,
                                            vsf_flash_size_t offset_of_bytes);

/**
 * flash erase a continuous range
 * @note offset_of_bytes must be aligend to the start address
 *       of the erase sector. size_of_bytes must be an integer
 *       multiple of the minimum erase size_of_bytes
 *
 * @param[in] flash_ptr flash instance
 * @param[in] offset_of_bytes flash address offset_of_bytes
 * @param[in] size_of_bytes flash erase size_of_bytes(bytes)
 */
extern vsf_err_t vsf_flash_erase_multi_sector(vsf_flash_t *flash_ptr,
                                              vsf_flash_size_t offset_of_bytes,
                                              vsf_flash_size_t size_of_bytes);

/**
 * flash erase all
 *
 * @param[in] flash_ptr flash instance
 */
extern vsf_err_t vsf_flash_erase_all(vsf_flash_t *flash_ptr);


extern vsf_err_t vsf_flash_write_one_sector(vsf_flash_t *flash_ptr,
                                            vsf_flash_size_t offset_of_bytes,
                                            uint8_t* buffer,
                                            vsf_flash_size_t size_of_bytes);

/**
 * flash write a continuous range
 * @note offset_of_bytes must be aligend to the start address
 *       of the write sector. size_of_bytes must be an integer
 *       multiple of the minimum write size_of_bytes
 *
 * @param[in] flash_ptr flash instance
 * @param[in] offset_of_bytes flash address offset_of_bytes
 * @param[out] buffer flash data to write
 * @param[in] size_of_bytes flash write size_of_bytes(bytes)
 */
extern vsf_err_t vsf_flash_write_multi_sector(vsf_flash_t *flash_ptr,
                                              vsf_flash_size_t offset_of_bytes,
                                              uint8_t* buffer,
                                              vsf_flash_size_t size_of_bytes);

extern vsf_err_t vsf_flash_read_one_sector(vsf_flash_t *flash_ptr,
                                           vsf_flash_size_t offset_of_bytes,
                                           uint8_t* buffer,
                                           vsf_flash_size_t size_of_bytes);

/**
 * flash read a continuous range
 * @note if the flash hardware does not support random read,
 *       the unaligned range should be returned as a failure.
 *
 * @param[in] flash_ptr flash instance
 * @param[in] offset_of_bytes flash address offset_of_bytes
 * @param[in] buffer flash data to read
 * @param[in] size_of_bytes flash write size_of_bytes(bytes)
 */
extern vsf_err_t vsf_flash_read_multi_sector(vsf_flash_t *flash_ptr,
                                             vsf_flash_size_t offset_of_bytes,
                                             uint8_t* buffer,
                                             vsf_flash_size_t size_of_bytes);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_FLASH_CFG_FUNCTION_RENAME == ENABLED
#   define vsf_flash_init(__FLASH, ...)                                         \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_init)                 ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_enable(__FLASH)                                            \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_enable)               ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH)
#   define vsf_flash_disable(__FLASH)                                           \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_disable)              ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH)
#   define vsf_flash_erase_one_sector(__FLASH, ...)                             \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_one_sector)     ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_erase_multi_sector(__FLASH, ...)                           \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_multi_sector)   ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_erase(__FLASH, ...)                                        \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_multi_sector)   ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_erase_all(__FLASH, ...)                                    \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_all)            ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_write_one_sector(__FLASH, ...)                             \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_write_one_sector)     ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_write_multi_sector(__FLASH, ...)                           \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_write_multi_sector)   ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_write(__FLASH, ...)                                        \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_write_multi_sector)   ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_read_one_sector(__FLASH, ...)                              \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_read_one_sector)      ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_read_multi_sector(__FLASH, ...)                            \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_read_multi_sector)    ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_read(__FLASH, ...)                                         \
        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_read_multi_sector)    ((VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t) *)__FLASH, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_FLASH_INTERFACE_H__*/
