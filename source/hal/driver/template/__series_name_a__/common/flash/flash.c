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

#if VSF_HAL_USE_FLASH == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${FLASH_IP}_FLASH_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${FLASH_IP}_FLASH_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${FLASH_IP}_FLASH_CLASS_IMPLEMENT
// IPCore end
// HW using ${FLASH_IP} IPCore driver
#define __VSF_HAL_${FLASH_IP}_FLASH_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_FLASH_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${FLASH_IP}_FLASH_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_FLASH_CFG_MULTI_CLASS
#   define VSF_HW_FLASH_CFG_MULTI_CLASS         VSF_FLASH_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_FLASH_CFG_IMP_PREFIX                vsf_hw
#define VSF_FLASH_CFG_IMP_UPCASE_PREFIX         VSF_HW
// HW end
// IPCore
#define VSF_FLASH_CFG_IMP_PREFIX                vsf_${flash_ip}
#define VSF_FLASH_CFG_IMP_UPCASE_PREFIX         VSF_${FLASH_IP}
// IPCore end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) {
#if VSF_HW_FLASH_CFG_MULTI_CLASS == ENABLED
    vsf_flash_t vsf_flash;
#endif
    // TODO: fix reg to the real type
    void *reg;
    vsf_flash_isr_t isr;
} VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t);
// HW end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw flash only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_init)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    // configure according to cfg_ptr
    flash_ptr->isr = cfg_ptr->isr;
    // configure interrupt according to cfg_ptr->isr
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_fini)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
}

fsm_rt_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_enable)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_disable)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_enable)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
}

void VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_disable)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    uint8_t *buffer,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(NULL != buffer);
    return VSF_ERR_FAIL;
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_read_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    uint8_t *buffer,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(NULL != buffer);
    return VSF_ERR_FAIL;
}

vsf_flash_status_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_status)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    return (vsf_flash_status_t) {
        .is_busy          = 0,
    };
}

vsf_flash_capability_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_capability)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    return (vsf_flash_capability_t) {
        .irq_mask              = VSF_FLASH_IRQ_ALL_BITS_MASK,
        .base_address          = 0,
        .max_size              = 0,
        .erase_sector_size     = 4096,
        .write_sector_size     = 256,
        .none_sector_aligned_write = 0,
        .none_sector_aligned_read  = 0,
    };
}

static vsf_flash_irq_mask_t VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_get_irq_mask)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    // implement this function in the device file
    VSF_HAL_ASSERT(0);
    return 0;
}

static void VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_irqhandler)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    VSF_HAL_ASSERT(NULL != flash_ptr);

    vsf_flash_irq_mask_t irq_mask = VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_get_irq_mask)(flash_ptr);
    vsf_flash_isr_t *isr_ptr = &flash_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_flash_t *)flash_ptr, irq_mask);
    }
}
// HW end

/*============================ INCLUDES ======================================*/

/*\note TODO: add comments about template configurations below:
 *  VSF_FLASH_CFG_ERASE_ALL_TEMPLATE                implement erase_all by erase_multi_sector in template
 *  VSF_FLASH_CFG_ERASE_ONE_SECTOR_TEMPLATE         implement erase_one_sector by erase_multi_sector in template
 *  VSF_FLASH_CFG_WRITE_ONE_SECTOR_TEMPLATE         implement write_one_sector by write_multi_sector in template
 *  VSF_FLASH_CFG_READ_ONE_SECTOR_TEMPLATE          implement read_one_sector by read_multi_sector in template
 */

// only define in source file
#define VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#define VSF_FLASH_CFG_ERASE_ALL_TEMPLATE            ENABLED
#define VSF_FLASH_CFG_ERASE_ONE_SECTOR_TEMPLATE     ENABLED
#define VSF_FLASH_CFG_WRITE_ONE_SECTOR_TEMPLATE     ENABLED
#define VSF_FLASH_CFG_READ_ONE_SECTOR_TEMPLATE      ENABLED

#define VSF_FLASH_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t)                            \
        VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash ## __IDX) = {             \
        .reg                = VSF_MCONNECT(VSF_FLASH_CFG_IMP_UPCASE_PREFIX, _FLASH, __IDX, _REG_BASE),\
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_FLASH_CFG_IMP_UPCASE_PREFIX, _FLASH, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_FLASH_CFG_IMP_PREFIX, _flash_irqhandler)(          \
            &VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash, __IDX)              \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/flash/flash_template.inc"

#endif      // VSF_HAL_USE_FLASH
