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

#include "./flash.h"

#if VSF_HAL_USE_FLASH == ENABLED

#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"
#include <string.h>

/*============================ MACROS ========================================*/

#ifndef VSF_HW_FLASH_CFG_MULTI_CLASS
#   define VSF_HW_FLASH_CFG_MULTI_CLASS         VSF_FLASH_CFG_MULTI_CLASS
#endif

#define VSF_FLASH_CFG_IMP_PREFIX                vsf_hw
#define VSF_FLASH_CFG_IMP_UPCASE_PREFIX         VSF_HW

/* ROM function codes */
#define ROM_TABLE_CODE(c1, c2)                  ((c1) | ((c2) << 8))
#define ROM_FUNC_CONNECT_INTERNAL_FLASH         ROM_TABLE_CODE('I', 'F')
#define ROM_FUNC_FLASH_EXIT_XIP                 ROM_TABLE_CODE('E', 'X')
#define ROM_FUNC_FLASH_RANGE_ERASE              ROM_TABLE_CODE('R', 'E')
#define ROM_FUNC_FLASH_RANGE_PROGRAM            ROM_TABLE_CODE('R', 'P')
#define ROM_FUNC_FLASH_FLUSH_CACHE              ROM_TABLE_CODE('F', 'C')
#define ROM_FUNC_FLASH_ENTER_CMD_XIP            ROM_TABLE_CODE('C', 'X')

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) {
#if VSF_HW_FLASH_CFG_MULTI_CLASS == ENABLED
    vsf_flash_t             vsf_flash;
#endif
} VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t);

typedef void *(*rom_table_lookup_fn)(uint16_t *table, uint32_t code);

typedef void (*rom_connect_internal_flash_fn)(void);
typedef void (*rom_flash_exit_xip_fn)(void);
typedef void (*rom_flash_range_erase_fn)(uint32_t addr, size_t count, uint32_t block_size, uint8_t block_cmd);
typedef void (*rom_flash_range_program_fn)(uint32_t addr, const uint8_t *data, size_t count);
typedef void (*rom_flash_flush_cache_fn)(void);
typedef void (*rom_flash_enter_cmd_xip_fn)(void);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static inline void *__vsf_rom_hword_as_ptr(uint16_t rom_address)
{
    return (void *)(uintptr_t)(*(uint16_t *)(uintptr_t)(rom_address));
}

static inline void *__vsf_rom_func_lookup(uint32_t code)
{
    rom_table_lookup_fn rom_table_lookup =
        (rom_table_lookup_fn) __vsf_rom_hword_as_ptr(0x18);
    uint16_t *func_table = (uint16_t *) __vsf_rom_hword_as_ptr(0x14);
    return rom_table_lookup(func_table, code);
}

/* Must run from RAM: during flash erase/program the XIP interface is busy
 * and code cannot execute from flash.  The .time_critical section is placed
 * in RAM by the pico-sdk linker script (memmap_default.ld).
 * NOTE: section attribute verified with GCC only; IAR / ARMCC / MSVC not
 * yet tested on this port. */
static void VSF_CAL_SECTION(".time_critical") __vsf_rp2040_flash_do_erase(uint32_t offset, size_t size)
{
    rom_connect_internal_flash_fn connect =
        (rom_connect_internal_flash_fn) __vsf_rom_func_lookup(ROM_FUNC_CONNECT_INTERNAL_FLASH);
    rom_flash_exit_xip_fn exit_xip =
        (rom_flash_exit_xip_fn) __vsf_rom_func_lookup(ROM_FUNC_FLASH_EXIT_XIP);
    rom_flash_range_erase_fn erase =
        (rom_flash_range_erase_fn) __vsf_rom_func_lookup(ROM_FUNC_FLASH_RANGE_ERASE);
    rom_flash_flush_cache_fn flush =
        (rom_flash_flush_cache_fn) __vsf_rom_func_lookup(ROM_FUNC_FLASH_FLUSH_CACHE);
    rom_flash_enter_cmd_xip_fn enter_xip =
        (rom_flash_enter_cmd_xip_fn) __vsf_rom_func_lookup(ROM_FUNC_FLASH_ENTER_CMD_XIP);

    if (connect && exit_xip && erase && flush && enter_xip) {
        connect();
        exit_xip();
        erase(offset, size, VSF_HW_FLASH0_BLOCK_SIZE, 0xd8);
        flush();
        enter_xip();
    }
}

static void VSF_CAL_SECTION(".time_critical") __vsf_rp2040_flash_do_program(uint32_t offset, const uint8_t *data, size_t size)
{
    rom_connect_internal_flash_fn connect =
        (rom_connect_internal_flash_fn) __vsf_rom_func_lookup(ROM_FUNC_CONNECT_INTERNAL_FLASH);
    rom_flash_exit_xip_fn exit_xip =
        (rom_flash_exit_xip_fn) __vsf_rom_func_lookup(ROM_FUNC_FLASH_EXIT_XIP);
    rom_flash_range_program_fn program =
        (rom_flash_range_program_fn) __vsf_rom_func_lookup(ROM_FUNC_FLASH_RANGE_PROGRAM);
    rom_flash_flush_cache_fn flush =
        (rom_flash_flush_cache_fn) __vsf_rom_func_lookup(ROM_FUNC_FLASH_FLUSH_CACHE);
    rom_flash_enter_cmd_xip_fn enter_xip =
        (rom_flash_enter_cmd_xip_fn) __vsf_rom_func_lookup(ROM_FUNC_FLASH_ENTER_CMD_XIP);

    if (connect && exit_xip && program && flush && enter_xip) {
        connect();
        exit_xip();
        program(offset, data, size);
        flush();
        enter_xip();
    }
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_init)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    /* RP2040 flash has no hardware interrupt.  erase/program are synchronous
     * ROM function calls; there is no async completion signal. */
    if (cfg_ptr->isr.handler_fn != NULL) {
        return VSF_ERR_NOT_SUPPORT;
    }

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
    /* RP2040 flash has no hardware interrupt line.  No-op. */
    VSF_UNUSED_PARAM(irq_mask);
}

void VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_disable)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    /* RP2040 flash has no hardware interrupt line.  No-op. */
    VSF_UNUSED_PARAM(irq_mask);
}

vsf_flash_irq_mask_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_clear)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    /* RP2040 flash has no hardware interrupt line.  Nothing to clear. */
    VSF_UNUSED_PARAM(irq_mask);
    return 0;
}

__attribute__((section(".time_critical"))) vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT((offset % VSF_HW_FLASH0_SECTOR_SIZE) == 0);
    VSF_HAL_ASSERT((size % VSF_HW_FLASH0_SECTOR_SIZE) == 0);
    VSF_HAL_ASSERT((offset + size) <= VSF_HW_FLASH0_SIZE);

    vsf_protect_t state = vsf_protect_interrupt();
    __vsf_rp2040_flash_do_erase(offset, size);
    vsf_unprotect_interrupt(state);

    return VSF_ERR_NONE;
}

__attribute__((section(".time_critical"))) vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    uint8_t *buffer,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(buffer != NULL);
    VSF_HAL_ASSERT((offset % VSF_HW_FLASH0_PAGE_SIZE) == 0);
    VSF_HAL_ASSERT((size % VSF_HW_FLASH0_PAGE_SIZE) == 0);
    VSF_HAL_ASSERT((offset + size) <= VSF_HW_FLASH0_SIZE);

    vsf_protect_t state = vsf_protect_interrupt();
    __vsf_rp2040_flash_do_program(offset, buffer, size);
    vsf_unprotect_interrupt(state);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_read_multi_sector)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_size_t offset,
    uint8_t *buffer,
    vsf_flash_size_t size
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(buffer != NULL);
    VSF_HAL_ASSERT((offset + size) <= VSF_HW_FLASH0_SIZE);

    const uint8_t *flash_src = (const uint8_t *)(VSF_HW_FLASH0_XIP_BASE + offset);
    memcpy(buffer, flash_src, size);

    return VSF_ERR_NONE;
}

vsf_flash_status_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_status)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    return (vsf_flash_status_t) {
        .is_busy = 0,
    };
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_get_configuration)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    /* RP2040 flash has no hardware interrupt; ISR is not applicable. */
    return VSF_ERR_NONE;
}

vsf_flash_capability_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_capability)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    return (vsf_flash_capability_t) {
        /* RP2040 flash erase/program are synchronous ROM calls;
         * there is no hardware interrupt line. */
        .irq_mask                   = 0,
        .base_address               = VSF_HW_FLASH0_XIP_BASE,
        .max_size                   = VSF_HW_FLASH0_SIZE,
        .erase_sector_size          = VSF_HW_FLASH0_SECTOR_SIZE,
        .write_sector_size          = VSF_HW_FLASH0_PAGE_SIZE,
        .none_sector_aligned_write  = 0,
        .none_sector_aligned_read   = 1,
    };
}

vsf_err_t VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_ctrl)(
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t) *flash_ptr,
    vsf_flash_ctrl_t ctrl,
    void *param
) {
    VSF_HAL_ASSERT(flash_ptr != NULL);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

/*============================ INCLUDES ======================================*/

#define VSF_FLASH_CFG_MODE_CHECK_UNIQUE                 VSF_HAL_CHECK_MODE_LOOSE
#define VSF_FLASH_CFG_IRQ_MASK_CHECK_UNIQUE             VSF_HAL_CHECK_MODE_STRICT
#define VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY        ENABLED
#define VSF_FLASH_CFG_REIMPLEMENT_API_GET_CONFIGURATION ENABLED
#define VSF_FLASH_CFG_REIMPLEMENT_API_IRQ_CLEAR         ENABLED
#define VSF_FLASH_CFG_ERASE_ALL_TEMPLATE                ENABLED
#define VSF_FLASH_CFG_ERASE_ONE_SECTOR_TEMPLATE         ENABLED
#define VSF_FLASH_CFG_WRITE_ONE_SECTOR_TEMPLATE         ENABLED
#define VSF_FLASH_CFG_READ_ONE_SECTOR_TEMPLATE          ENABLED
#define VSF_FLASH_CFG_REIMPLEMENT_API_CTRL              ENABLED

#define VSF_FLASH_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t)                            \
    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash##__IDX) = {__HAL_OP};

#include "hal/driver/common/flash/flash_template.inc"

#endif      // VSF_HAL_USE_FLASH
/* EOF */
