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

#if defined(VSF_FLASH_CFG_IMP_RENAME_DEVICE_PREFIX) && (VSF_FLASH_CFG_IMP_RENAME_DEVICE_PREFIX == ENABLED)
#   define vsf_real_flash_t                      VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _t)
#   define vsf_real_flash_capability             VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _capability)
#   define vsf_real_flash_get_configuration      VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _get_configuration)
#   define vsf_real_flash_erase_all              VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _erase_all)
#   define vsf_real_flash_erase_one_sector       VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _erase_one_sector)
#   define vsf_real_flash_erase_multi_sector     VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _erase_multi_sector)
#   define vsf_real_flash_write_one_sector       VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _write_one_sector)
#   define vsf_real_flash_write_multi_sector     VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _write_multi_sector)
#   define vsf_real_flash_read_one_sector        VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _read_one_sector)
#   define vsf_real_flash_read_multi_sector      VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _read_multi_sector)
#   define vsf_real_flash_ctrl                   VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _ctrl)
#   define vsf_real_flash_irq_clear              VSF_MCONNECT(VSF_FLASH_CFG_IMP_DEVICE_PREFIX, _irq_clear)
#else
#   define vsf_real_flash_t                      VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t)
#   define vsf_real_flash_capability             VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_capability)
#   define vsf_real_flash_get_configuration      VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_get_configuration)
#   define vsf_real_flash_erase_all              VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_all)
#   define vsf_real_flash_erase_one_sector       VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_one_sector)
#   define vsf_real_flash_erase_multi_sector     VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_multi_sector)
#   define vsf_real_flash_write_one_sector       VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_one_sector)
#   define vsf_real_flash_write_multi_sector     VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_multi_sector)
#   define vsf_real_flash_read_one_sector        VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_read_one_sector)
#   define vsf_real_flash_read_multi_sector      VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_read_multi_sector)
#   define vsf_real_flash_ctrl                   VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_ctrl)
#   define vsf_real_flash_irq_clear              VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_clear)
#endif

#ifndef VSF_FLASH_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_FLASH_CFG_REIMPLEMENT_API_GET_CONFIGURATION          DISABLED
#endif

#ifndef VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY                DISABLED
#endif

#ifndef VSF_FLASH_CFG_REIMPLEMENT_API_CTRL
#   define VSF_FLASH_CFG_REIMPLEMENT_API_CTRL                       DISABLED
#endif

#ifndef VSF_FLASH_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_FLASH_CFG_REIMPLEMENT_API_IRQ_CLEAR                  DISABLED
#endif

#ifdef VSF_FLASH_CFG_IMP_REMAP_PREFIX
#   undef VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY                 ENABLED
#   undef VSF_FLASH_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_FLASH_CFG_REIMPLEMENT_API_GET_CONFIGURATION          ENABLED
#   undef VSF_FLASH_CFG_REIMPLEMENT_API_CTRL
#   define VSF_FLASH_CFG_REIMPLEMENT_API_CTRL                       ENABLED
#   undef VSF_FLASH_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_FLASH_CFG_REIMPLEMENT_API_IRQ_CLEAR                  ENABLED
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_FLASH_CFG_CAPABILITY_IRQ_MASK
#       define VSF_FLASH_CFG_CAPABILITY_IRQ_MASK                    VSF_FLASH_IRQ_ALL_BITS_MASK
#   endif
#   ifndef VSF_FLASH_CFG_CAPABILITY_BASE_ADDRESS
#       define VSF_FLASH_CFG_CAPABILITY_BASE_ADDRESS                0
#   endif
#   ifndef VSF_FLASH_CFG_CAPABILITY_MAX_SIZE
#       error "Please define flash size macro 'VSF_FLASH_CFG_CAPABILITY_MAX_SIZE' in source files"
#   endif
#   ifndef VSF_FLASH_CFG_CAPABILITY_ERASE_SECTORE_SIZE
#       define VSF_FLASH_CFG_CAPABILITY_ERASE_SECTORE_SIZE          4096
#   endif
#   ifndef VSF_FLASH_CFG_CAPABILITY_WRITE_SECTORE_SIZE
#       define VSF_FLASH_CFG_CAPABILITY_WRITE_SECTORE_SIZE          512
#   endif
#   ifndef VSF_FLASH_CFG_CAPABILITY_NONE_SECTOR_ALIGNED_WRITE
#       define VSF_FLASH_CFG_CAPABILITY_NONE_SECTOR_ALIGNED_WRITE   0
#   endif
#   ifndef VSF_FLASH_CFG_CAPABILITY_NONE_SECTOR_ALIGNED_READ
#       define VSF_FLASH_CFG_CAPABILITY_NONE_SECTOR_ALIGNED_READ    1
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_flash_capability_t vsf_real_flash_capability(vsf_real_flash_t *flash_ptr)
{
    vsf_flash_capability_t flash_capability = {
        .irq_mask                   = VSF_FLASH_CFG_CAPABILITY_IRQ_MASK,
        .base_address               = VSF_FLASH_CFG_CAPABILITY_BASE_ADDRESS,
        .max_size                   = VSF_FLASH_CFG_CAPABILITY_MAX_SIZE,
        .erase_sector_size          = VSF_FLASH_CFG_CAPABILITY_ERASE_SECTORE_SIZE,
        .write_sector_size          = VSF_FLASH_CFG_CAPABILITY_WRITE_SECTORE_SIZE,
        .none_sector_aligned_write  = VSF_FLASH_CFG_CAPABILITY_NONE_SECTOR_ALIGNED_WRITE,
        .none_sector_aligned_read   = VSF_FLASH_CFG_CAPABILITY_NONE_SECTOR_ALIGNED_READ,
    };

    VSF_ASSERT(flash_capability.erase_sector_size >= flash_capability.write_sector_size);

    return flash_capability;
}
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_flash_get_configuration(vsf_real_flash_t *flash_ptr, vsf_flash_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != flash_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_API_CTRL == DISABLED
vsf_err_t vsf_real_flash_ctrl(vsf_real_flash_t *flash_ptr, vsf_flash_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != flash_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_API_IRQ_CLEAR == DISABLED
vsf_flash_irq_mask_t vsf_real_flash_irq_clear(vsf_real_flash_t *flash_ptr, vsf_flash_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != flash_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return 0;
}
#endif

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
#   error not supported yet
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
    VSF_ASSERT(flash_capability.none_sector_aligned_write || ((offset % flash_capability.write_sector_size) == 0));

    return vsf_real_flash_write_multi_sector(real_flash_ptr, offset, buffer, size);
}
#endif

#if VSF_FLASH_CFG_WRITE_MULTI_SECTOR_TEMPLATE == ENABLED
#   error not supported yet
#endif

#if VSF_FLASH_CFG_READ_ONE_SECTOR_TEMPLATE == ENABLED
vsf_err_t vsf_real_flash_read_one_sector(vsf_real_flash_t *real_flash_ptr, vsf_flash_size_t offset, uint8_t* buffer, vsf_flash_size_t size)
{
    VSF_ASSERT(real_flash_ptr != 0);

    vsf_flash_capability_t flash_capability = vsf_real_flash_capability(real_flash_ptr);
    VSF_ASSERT(size <= flash_capability.write_sector_size);
    VSF_ASSERT(flash_capability.none_sector_aligned_read || ((offset % flash_capability.write_sector_size) == 0));

    return vsf_real_flash_read_multi_sector(real_flash_ptr, offset, buffer, size);
}
#endif

#if VSF_FLASH_CFG_READ_MULTI_SECTOR_TEMPLATE == ENABLED
#   error not supported yet
#endif

/*============================ MACROS ========================================*/

#undef VSF_FLASH_CFG_REIMPLEMENT_TYPE_CAPABILITY
#undef VSF_FLASH_CFG_REIMPLEMENT_TYPE_CFG
#undef VSF_FLASH_CFG_REIMPLEMENT_TYPE_CTRL
#undef VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#undef VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS
#undef VSF_FLASH_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_FLASH_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_FLASH_CFG_REIMPLEMENT_API_CTRL
#undef VSF_FLASH_CFG_REIMPLEMENT_API_IRQ_CLEAR
#undef VSF_FLASH_CFG_CAPABILITY_IRQ_MASK
#undef VSF_FLASH_CFG_CAPABILITY_BASE_ADDRESS
#undef VSF_FLASH_CFG_CAPABILITY_MAX_SIZE
#undef VSF_FLASH_CFG_CAPABILITY_ERASE_SECTORE_SIZE
#undef VSF_FLASH_CFG_CAPABILITY_WRITE_SECTORE_SIZE
#undef VSF_FLASH_CFG_CAPABILITY_NONE_SECTOR_ALIGNED_WRITE
#undef VSF_FLASH_CFG_CAPABILITY_NONE_SECTOR_ALIGNED_READ

#undef vsf_real_flash_t
#undef vsf_real_flash_erase_all
#undef vsf_real_flash_erase_one_sector
#undef vsf_real_flash_erase_multi_sector
#undef vsf_real_flash_write_one_sector
#undef vsf_real_flash_write_multi_sector
#undef vsf_real_flash_read_one_sector
#undef vsf_real_flash_read_multi_sector
#undef vsf_real_flash_capability
#undef vsf_real_flash_get_configuration
#undef vsf_real_flash_ctrl
#undef vsf_real_flash_irq_clear

/*============================ MACROS ========================================*/

#ifdef VSF_FLASH_CFG_IMP_REMAP_PREFIX
#   define vsf_imp_flash_t                    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_t)
#   define vsf_imp_flash_init                 VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_init)
#   define vsf_imp_flash_fini                 VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_fini)
#   define vsf_imp_flash_enable               VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_enable)
#   define vsf_imp_flash_disable              VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_disable)
#   define vsf_imp_flash_capability           VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_capability)
#   define vsf_imp_flash_status               VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_status)
#   define vsf_imp_flash_irq_enable           VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_enable)
#   define vsf_imp_flash_irq_disable          VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_disable)
#   define vsf_imp_flash_irq_clear            VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_irq_clear)
#   define vsf_imp_flash_erase_one_sector     VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_one_sector)
#   define vsf_imp_flash_erase_multi_sector   VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_multi_sector)
#   define vsf_imp_flash_erase                VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_multi_sector)
#   define vsf_imp_flash_erase_all            VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_erase_all)
#   define vsf_imp_flash_write_one_sector     VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_one_sector)
#   define vsf_imp_flash_write_multi_sector   VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_multi_sector)
#   define vsf_imp_flash_write                VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_write_multi_sector)
#   define vsf_imp_flash_read_one_sector      VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_read_one_sector)
#   define vsf_imp_flash_read_multi_sector    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_read_multi_sector)
#   define vsf_imp_flash_read                 VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_read_multi_sector)
#   define vsf_imp_flash_get_configuration    VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_get_configuration)
#   define vsf_imp_flash_ctrl                 VSF_MCONNECT(VSF_FLASH_CFG_IMP_PREFIX, _flash_ctrl)

#   define vsf_remap_flash_t                  VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_t)
#   define vsf_remap_flash_init               VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_init)
#   define vsf_remap_flash_fini               VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_fini)
#   define vsf_remap_flash_enable             VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_enable)
#   define vsf_remap_flash_disable            VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_disable)
#   define vsf_remap_flash_capability         VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_capability)
#   define vsf_remap_flash_status             VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_status)
#   define vsf_remap_flash_irq_enable         VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_irq_enable)
#   define vsf_remap_flash_irq_disable        VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_irq_disable)
#   define vsf_remap_flash_irq_clear          VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_irq_clear)
#   define vsf_remap_flash_erase_one_sector   VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_erase_one_sector)
#   define vsf_remap_flash_erase_multi_sector VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_erase_multi_sector)
#   define vsf_remap_flash_erase              VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_erase_multi_sector)
#   define vsf_remap_flash_erase_all          VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_erase_all)
#   define vsf_remap_flash_write_one_sector   VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_write_one_sector)
#   define vsf_remap_flash_write_multi_sector VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_write_multi_sector)
#   define vsf_remap_flash_write              VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_write_multi_sector)
#   define vsf_remap_flash_read_one_sector    VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_read_one_sector)
#   define vsf_remap_flash_read_multi_sector  VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_read_multi_sector)
#   define vsf_remap_flash_read               VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_read_multi_sector)
#   define vsf_remap_flash_get_configuration  VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_get_configuration)
#   define vsf_remap_flash_ctrl               VSF_MCONNECT(VSF_FLASH_CFG_IMP_REMAP_PREFIX, _flash_ctrl)

#   define VSF_FLASH_CFG_IMP_REMAP_FUNCTIONS                                                                                              \
        vsf_err_t vsf_imp_flash_init(vsf_imp_flash_t *flash_ptr, vsf_flash_cfg_t *cfg_ptr)                                                \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_init((vsf_remap_flash_t *)flash_ptr, cfg_ptr);                                                         \
        }                                                                                                                                 \
        void vsf_imp_flash_fini(vsf_imp_flash_t *flash_ptr)                                                                               \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            vsf_remap_flash_fini((vsf_remap_flash_t *)flash_ptr);                                                                         \
        }                                                                                                                                 \
        fsm_rt_t vsf_imp_flash_enable(vsf_imp_flash_t *flash_ptr)                                                                         \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_enable((vsf_remap_flash_t *)flash_ptr);                                                                \
        }                                                                                                                                 \
        fsm_rt_t vsf_imp_flash_disable(vsf_imp_flash_t *flash_ptr)                                                                        \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_disable((vsf_remap_flash_t *)flash_ptr);                                                               \
        }                                                                                                                                 \
        void vsf_imp_flash_irq_enable(vsf_imp_flash_t *flash_ptr, vsf_flash_irq_mask_t irq_mask)                                          \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            vsf_remap_flash_irq_enable((vsf_remap_flash_t *)flash_ptr, irq_mask);                                                         \
        }                                                                                                                                 \
        void vsf_imp_flash_irq_disable(vsf_imp_flash_t *flash_ptr, vsf_flash_irq_mask_t irq_mask)                                         \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            vsf_remap_flash_irq_disable((vsf_remap_flash_t *)flash_ptr, irq_mask);                                                        \
        }                                                                                                                                 \
        vsf_flash_irq_mask_t vsf_imp_flash_irq_clear(vsf_imp_flash_t *flash_ptr, vsf_flash_irq_mask_t irq_mask)                           \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_irq_clear((vsf_remap_flash_t *)flash_ptr, irq_mask);                                                   \
        }                                                                                                                                 \
        vsf_flash_status_t vsf_imp_flash_status(vsf_imp_flash_t *flash_ptr)                                                               \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_status((vsf_remap_flash_t *)flash_ptr);                                                                \
        }                                                                                                                                 \
        vsf_flash_capability_t vsf_imp_flash_capability(vsf_imp_flash_t *flash_ptr)                                                       \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_capability((vsf_remap_flash_t *)flash_ptr);                                                            \
        }                                                                                                                                 \
        vsf_err_t vsf_imp_flash_erase_one_sector(vsf_imp_flash_t *flash_ptr, uint_fast32_t offset)                                        \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_erase_one_sector((vsf_remap_flash_t *)flash_ptr, offset);                                              \
        }                                                                                                                                 \
        vsf_err_t vsf_imp_flash_erase_multi_sector(vsf_imp_flash_t *flash_ptr, uint_fast32_t offset, uint_fast32_t size)                  \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_erase_multi_sector((vsf_remap_flash_t *)flash_ptr, offset, size);                                      \
        }                                                                                                                                 \
        vsf_err_t vsf_imp_flash_erase_all(vsf_imp_flash_t *flash_ptr)                                                                     \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_erase_all((vsf_remap_flash_t *)flash_ptr);                                                             \
        }                                                                                                                                 \
        vsf_err_t vsf_imp_flash_write_one_sector(vsf_imp_flash_t *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size)   \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_write_one_sector((vsf_remap_flash_t *)flash_ptr, offset, buffer, size);                                \
        }                                                                                                                                 \
        vsf_err_t vsf_imp_flash_write_multi_sector(vsf_imp_flash_t *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size) \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_write_multi_sector((vsf_remap_flash_t *)flash_ptr, offset, buffer, size);                              \
        }                                                                                                                                 \
        vsf_err_t vsf_imp_flash_read_one_sector(vsf_imp_flash_t *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size)    \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_read_one_sector((vsf_remap_flash_t *)flash_ptr, offset, buffer, size);                                 \
        }                                                                                                                                 \
        vsf_err_t vsf_imp_flash_read_multi_sector(vsf_imp_flash_t *flash_ptr, uint_fast32_t offset, uint8_t* buffer, uint_fast32_t size)  \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_read_multi_sector((vsf_remap_flash_t *)flash_ptr, offset, buffer, size);                               \
        }                                                                                                                                 \
                                                                                                                                          \
        vsf_err_t vsf_imp_flash_get_configuration(vsf_imp_flash_t *flash_ptr, vsf_flash_cfg_t *cfg_ptr)                                   \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_get_configuration((vsf_remap_flash_t *)flash_ptr, cfg_ptr);                                            \
        }                                                                                                                                 \
        vsf_err_t vsf_imp_flash_ctrl(vsf_imp_flash_t *flash_ptr, vsf_flash_ctrl_t ctrl, void *param)                                      \
        {                                                                                                                                 \
            VSF_HAL_ASSERT(flash_ptr != NULL);                                                                                            \
            return vsf_remap_flash_ctrl((vsf_remap_flash_t *)flash_ptr, ctrl, param);                                                      \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _flash
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _FLASH

#if !defined(VSF_FLASH_CFG_IMP_PREFIX) && !defined(VSF_FLASH_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_FLASH_CFG_IMP_PREFIX in flash driver"
#endif

#if !defined(VSF_FLASH_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_FLASH_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_FLASH_CFG_IMP_UPCASE_PREFIX in flash driver"
#endif

#ifndef VSF_FLASH_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_FLASH_CFG_IMP_COUNT_MASK_PREFIX      VSF_FLASH_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_FLASH_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_FLASH_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_FLASH_CFG_IMP_PREFIX
#undef VSF_FLASH_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_FLASH_CFG_IMP_UPCASE_PREFIX
#undef VSF_FLASH_CFG_IMP_DEVICE_PREFIX
#undef VSF_FLASH_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_FLASH_CFG_IMP_LV0
#undef VSF_FLASH_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_FLASH_CFG_IMP_HAS_OP
#undef VSF_FLASH_CFG_IMP_EXTERN_OP
#undef VSF_FLASH_CFG_IMP_RENAME_DEVICE_PREFIX

#undef vsf_imp_flash_t
#undef vsf_imp_flash_init
#undef vsf_imp_flash_fini
#undef vsf_imp_flash_enable
#undef vsf_imp_flash_disable
#undef vsf_imp_flash_capability
#undef vsf_imp_flash_status
#undef vsf_imp_flash_irq_enable
#undef vsf_imp_flash_irq_disable
#undef vsf_imp_flash_irq_clear
#undef vsf_imp_flash_erase_one_sector
#undef vsf_imp_flash_erase_multi_sector
#undef vsf_imp_flash_erase
#undef vsf_imp_flash_erase_all
#undef vsf_imp_flash_write_one_sector
#undef vsf_imp_flash_write_multi_sector
#undef vsf_imp_flash_write
#undef vsf_imp_flash_read_one_sector
#undef vsf_imp_flash_read_multi_sector
#undef vsf_imp_flash_read
#undef vsf_imp_flash_get_configuration
#undef vsf_imp_flash_ctrl

#undef vsf_remap_flash_t
#undef vsf_remap_flash_init
#undef vsf_remap_flash_fini
#undef vsf_remap_flash_enable
#undef vsf_remap_flash_disable
#undef vsf_remap_flash_capability
#undef vsf_remap_flash_status
#undef vsf_remap_flash_irq_enable
#undef vsf_remap_flash_irq_disable
#undef vsf_remap_flash_irq_clear
#undef vsf_remap_flash_erase_one_sector
#undef vsf_remap_flash_erase_multi_sector
#undef vsf_remap_flash_erase
#undef vsf_remap_flash_erase_all
#undef vsf_remap_flash_write_one_sector
#undef vsf_remap_flash_write_multi_sector
#undef vsf_remap_flash_write
#undef vsf_remap_flash_read_one_sector
#undef vsf_remap_flash_read_multi_sector
#undef vsf_remap_flash_read
#undef vsf_remap_flash_get_configuration
#undef vsf_remap_flash_ctrl

#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME

/*============================ STATIC ASSERTIONS ==============================*/

/* User-extensible macros for custom mode and IRQ mask values
 * Users can define these macros before including this template to append
 * their custom enum values to the uniqueness checks.
 *
 * Check modes for custom values:
 *   VSF_FLASH_CFG_MODE_CHECK_UNIQUE - Check mode for custom mode values
 *     Default: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *   VSF_FLASH_CFG_IRQ_MASK_CHECK_UNIQUE - Check mode for custom IRQ mask values
 *     Default: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *
 * Example usage in vendor driver:
 *   #define VSF_FLASH_CUSTOM_MODE_VALUES           MY_CUSTOM_MODE1, MY_CUSTOM_MODE2
 *   #define VSF_FLASH_CFG_MODE_CHECK_UNIQUE        VSF_HAL_CHECK_MODE_LOOSE
 *   #define VSF_FLASH_CUSTOM_IRQ_MASK_VALUES       MY_CUSTOM_IRQ1, MY_CUSTOM_IRQ2
 *   #define VSF_FLASH_CFG_IRQ_MASK_CHECK_UNIQUE    VSF_HAL_CHECK_MODE_STRICT
 *   #include "hal/driver/common/flash/flash_template.inc"
 */


#ifdef VSF_FLASH_CFG_MODE_CHECK_UNIQUE
/* Default mode lists - can be redefined by users before including this file */
/* Note: Flash typically doesn't have mode enums, but custom modes can be added */

#ifdef VSF_FLASH_CUSTOM_MODE_VALUES
/* Check uniqueness among custom mode values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_FLASH_CFG_MODE_CHECK_UNIQUE, VSF_FLASH_CUSTOM_MODE_VALUES);
#endif
#endif  /* VSF_FLASH_CFG_MODE_CHECK_UNIQUE */

/* ==================== IRQ MASK UNIQUENESS CHECKS ==================== */

#ifdef VSF_FLASH_CFG_IRQ_MASK_CHECK_UNIQUE

#ifdef VSF_FLASH_CUSTOM_IRQ_MASK_VALUES
/* Check uniqueness among custom IRQ mask values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_FLASH_CFG_IRQ_MASK_CHECK_UNIQUE, VSF_FLASH_CUSTOM_IRQ_MASK_VALUES);
#endif

/* Check uniqueness within vsf_flash_irq_mask_t enum */

/* Check IRQ mask uniqueness - all IRQ mask bits should have different values
 * Mandatory: VSF_FLASH_IRQ_ERASE_MASK, VSF_FLASH_IRQ_WRITE_MASK, VSF_FLASH_IRQ_READ_MASK,
 *            VSF_FLASH_IRQ_ERASE_ERROR_MASK, VSF_FLASH_IRQ_WRITE_ERROR_MASK, VSF_FLASH_IRQ_READ_ERROR_MASK
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_FLASH_CFG_IRQ_MASK_CHECK_UNIQUE,
    /* Mandatory IRQ masks - always included */
    VSF_FLASH_IRQ_ERASE_MASK,
    VSF_FLASH_IRQ_WRITE_MASK,
    VSF_FLASH_IRQ_READ_MASK,
    VSF_FLASH_IRQ_ERASE_ERROR_MASK,
    VSF_FLASH_IRQ_WRITE_ERROR_MASK,
    VSF_FLASH_IRQ_READ_ERROR_MASK
    /* User-defined IRQ masks - appended by user configuration */
#ifdef VSF_FLASH_CUSTOM_IRQ_MASK_VALUES
    , VSF_FLASH_CUSTOM_IRQ_MASK_VALUES
#endif
);
#endif  /* VSF_FLASH_CFG_IRQ_MASK_CHECK_UNIQUE */

#undef VSF_FLASH_CFG_MODE_CHECK_UNIQUE
#undef VSF_FLASH_CFG_IRQ_MASK_CHECK_UNIQUE
#undef VSF_FLASH_CUSTOM_MODE_VALUES
#undef VSF_FLASH_CUSTOM_IRQ_MASK_VALUES

#endif  /* VSF_HAL_USE_FLASH */
