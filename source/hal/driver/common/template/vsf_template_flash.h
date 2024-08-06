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

#ifndef __VSF_TEMPLATE_FLASH_H__
#define __VSF_TEMPLATE_FLASH_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_FLASH_CFG_MULTI_CLASS
#   define VSF_FLASH_CFG_MULTI_CLASS                        ENABLED
#endif

#if defined(VSF_HW_FLASH_COUNT) && !defined(VSF_HW_FLASH_MASK)
#   define VSF_HW_FLASH_MASK                                VSF_HAL_COUNT_TO_MASK(VSF_HW_FLASH_COUNT)
#endif

#if defined(VSF_HW_FLASH_MASK) && !defined(VSF_HW_FLASH_COUNT)
#   define VSF_HW_FLASH_COUNT                               VSF_HAL_MASK_TO_COUNT(VSF_HW_FLASH_MASK)
#endif

// application code can redefine it
#ifndef VSF_FLASH_CFG_PREFIX
#   if VSF_FLASH_CFG_MULTI_CLASS == ENABLED
#       define VSF_FLASH_CFG_PREFIX                         vsf
#   elif defined(VSF_HW_FLASH_COUNT) && (VSF_HW_FLASH_COUNT != 0)
#       define VSF_FLASH_CFG_PREFIX                         vsf_hw
#   else
#       define VSF_FLASH_CFG_PREFIX                         vsf
#   endif
#endif

#ifndef VSF_FLASH_CFG_FUNCTION_RENAME
#   define VSF_FLASH_CFG_FUNCTION_RENAME                    ENABLED
#endif

#ifndef VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK          DISABLED
#endif

#ifndef VSF_FLASH_CFG_REIMPLEMENT_TYPE_FLASH_SIZE
#   define VSF_FLASH_CFG_REIMPLEMENT_TYPE_FLASH_SIZE        DISABLED
#endif

#ifndef VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS            DISABLED
#endif

#ifndef VSF_FLASH_CFG_INHERT_HAL_CAPABILITY
#   define VSF_FLASH_CFG_INHERT_HAL_CAPABILITY              ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_FLASH_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, init,                  VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   flash, fini,                  VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptrr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               flash, enable,                VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               flash, disable,               VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_flash_status_t,     flash, status,                VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_flash_capability_t, flash, capability,            VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   flash, irq_enable,            VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   flash, irq_disable,           VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, erase_one_sector,      VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, erase_multi_sector,    VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, erase_all,             VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, write_one_sector,      VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, write_multi_sector,    VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, read_one_sector,       VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, read_multi_sector,     VSF_MCONNECT(__prefix_name, _flash_t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes)

/*============================ TYPES =========================================*/

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_flash_irq_mask_t {
    VSF_FLASH_IRQ_ERASE_MASK        = (0x1ul << 0),
    VSF_FLASH_IRQ_WRITE_MASK        = (0x1ul << 1),
    VSF_FLASH_IRQ_READ_MASK         = (0x1ul << 2),

    VSF_FLASH_IRQ_ERASE_ERROR_MASK  = (0x1ul << 3),
    VSF_FLASH_IRQ_WRITE_ERROR_MASK  = (0x1ul << 4),
    VSF_FLASH_IRQ_READ_ERROR_MASK   = (0x1ul << 5),
} vsf_flash_irq_mask_t;
#endif

enum {
    VSF_FLASH_IRQ_COUNT             = 6,
    VSF_FLASH_IRQ_ALL_BITS_MASK     = VSF_FLASH_IRQ_ERASE_MASK       |
                                      VSF_FLASH_IRQ_WRITE_MASK       |
                                      VSF_FLASH_IRQ_READ_MASK        |
                                      VSF_FLASH_IRQ_ERASE_ERROR_MASK |
                                      VSF_FLASH_IRQ_WRITE_ERROR_MASK |
                                      VSF_FLASH_IRQ_READ_ERROR_MASK,
};

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_FLASH_SIZE == DISABLED
typedef uint_fast32_t vsf_flash_size_t;
#endif

typedef struct vsf_flash_t vsf_flash_t;

typedef void vsf_flash_isr_handler_t(void *target_ptr,
                                     vsf_flash_t *flash_ptr,
                                     vsf_flash_irq_mask_t mask);

typedef struct vsf_flash_isr_t {
    vsf_flash_isr_handler_t *handler_fn;
    void                    *target_ptr;
    vsf_arch_prio_t          prio;
} vsf_flash_isr_t;

typedef struct vsf_flash_cfg_t {
    vsf_flash_isr_t isr;
} vsf_flash_cfg_t;

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_flash_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_flash_status_t;
#endif

typedef struct vsf_flash_capability_t {
#if VSF_FLASH_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_flash_irq_mask_t irq_mask;

    vsf_flash_size_t base_address;
    vsf_flash_size_t max_size;
    vsf_flash_size_t erase_sector_size;
    vsf_flash_size_t write_sector_size;
    struct {
        // Some flash allow only full sectors to be write or read
        uint8_t none_sector_aligned_write : 1;
        uint8_t none_sector_aligned_read  : 1;
    };
} vsf_flash_capability_t;

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

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a flash instance.
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_flash_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if flash was initialized, or a negative error code

  @note It is not necessary to call vsf_flash_fini() to deinitialization.
       vsf_flash_init() should be called before any other flash API except vsf_flash_capability().

 \~chinese
 @brief 初始化一个 flash 实例
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] cfg_ptr: 结构体 vsf_flash_cfg_t 的指针，参考 @ref vsf_flash_cfg_t
 @return vsf_err_t: 如果 flash 初始化完成返回 VSF_ERR_NONE , 否则返回负数。

 @note 失败后不需要调用 vsf_flash_fini() 反初始化。
       vsf_flash_init() 应该在除 vsf_flash_capability() 之外的其他 flash API 之前调用。
 */
extern vsf_err_t vsf_flash_init(vsf_flash_t *flash_ptr, vsf_flash_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a flash instance.
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @return none

 \~chinese
 @brief 终止一个 flash 实例
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] cfg_ptr: 结构体 vsf_flash_cfg_t 的指针，参考 @ref vsf_flash_cfg_t
 @return 无。
 */
extern void vsf_flash_fini(vsf_flash_t *flash_ptr);

/**
 \~english
 @brief enable interrupt masks of flash instance.
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] irq_mask: one or more value of enum @ref vsf_flash_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 flash 实例的中断
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_flash_irq_mask_t 的值的按位或，@ref vsf_flash_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_flash_enable(vsf_flash_t *flash_ptr);

/**
 \~english
 @brief disable interrupt masks of flash instance.
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] irq_mask: one or more value of enum vsf_flash_irq_mask_t, @ref vsf_flash_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 flash 实例的中断
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_flash_irq_mask_t 的值的按位或，@ref vsf_flash_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_flash_disable(vsf_flash_t *flash_ptr);

/**
 \~english
 @brief get the status of flash instance.
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @return vsf_flash_status_t: return all status of current flash @ref vsf_flash_status_t

 \~chinese
 @brief 获取 flash 实例的状态
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @return vsf_flash_status_t: 返回当前 flash 的所有能力 @ref vsf_flash_status_t
 */
extern vsf_flash_status_t vsf_flash_status(vsf_flash_t *flash_ptr);

/**
 \~english
 @brief get the capability of flash instance.
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @return vsf_flash_capability_t: return all capability of current flash @ref vsf_flash_capability_t

 \~chinese
 @brief 获取 flash 实例的能力
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @return vsf_flash_capability_t: 返回当前 flash 的所有能力 @ref vsf_flash_capability_t
 */
extern vsf_flash_capability_t vsf_flash_capability(vsf_flash_t *flash_ptr);


/**
 \~english
 @brief enable interrupt masks of flash instance.
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] irq_mask: one or more value of enum @ref vsf_flash_irq_mask_t
 @return none.
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 使能 flash 实例的中断
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_flash_irq_mask_t 的值的按位或，@ref vsf_flash_irq_mask_t
 @return 无。
 @note 在中断使能之前，应该清除所有悬挂的中断。
 */
extern void vsf_flash_irq_enable(vsf_flash_t *flash_ptr, vsf_flash_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of flash instance.
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] irq_mask: one or more value of enum vsf_flash_irq_mask_t, @ref vsf_flash_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 flash 实例的中断
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_flash_irq_mask_t 的值的按位或，@ref vsf_flash_irq_mask_t
 @return 无。
 */
extern void vsf_flash_irq_disable(vsf_flash_t *flash_ptr, vsf_flash_irq_mask_t irq_mask);

/**
 \~english
 @brief flash erase one sector.
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] offset_of_bytes: The address of the sector to be erased,
            needs to be an integer multiple of the smallest erasable sector size
 @return vsf_err_t: VSF_ERR_NONE if flash starts to perform the erase, or a negative error code

 \~chinese
 @brief flash 擦除一块扇区
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] offset_of_bytes: 被除扇区的地址，需要是最小可擦除扇区的整数倍
 @return  如果 flash 开始执行擦除返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_flash_erase_one_sector(vsf_flash_t *flash_ptr,
                                            vsf_flash_size_t offset_of_bytes);

/**
 \~english
 @brief flash erase a continuous range
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] offset_of_bytes: The address of the sector to be erased,
            needs to be an integer multiple of the smallest erasable sector size
 @param[in] size_of_bytes: a pointer to structure @ref vsf_flash_t
 @return vsf_err_t: VSF_ERR_NONE if flash starts to perform the erase, or a negative error code

 \~chinese
 @brief flash 擦除连续区域
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] offset_of_bytes: 被擦除扇区的地址，需要是最小可擦除扇区大小的整数倍
 @param[in] size_of_bytes: 被擦除扇区的长度，需要是最小可擦除扇区大小的整数倍
 @return  如果 flash 开始执行擦除返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_flash_erase_multi_sector(vsf_flash_t *flash_ptr,
                                              vsf_flash_size_t offset_of_bytes,
                                              vsf_flash_size_t size_of_bytes);

/**
 \~english
 @brief flash chip erase
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @return vsf_err_t: VSF_ERR_NONE if flash starts to perform the chip erase, or a negative error code

 \~chinese
 @brief flash 整片擦除
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @return  如果 flash 开始执行整片擦除返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_flash_erase_all(vsf_flash_t *flash_ptr);


/**
 \~english
 @brief flash write one sector
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] offset_of_bytes: Address of the sector to be written,
                some flash requires an integer multiple of the smallest writable sector size
 @param[in] buffer: a pointer to data
 @param[in] size_of_bytes: a pointer to data
 @return vsf_err_t: VSF_ERR_NONE if flash starts to perform the chip erase, or a negative error code

 \~chinese
 @brief flash 写一个扇区
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] offset_of_bytes: 被写入扇区的地址, 部分 flash 需要是最小可擦除扇区大小的整数倍
 @param[in] buffer: 数据的指针
 @param[in] size_of_bytes: 被写入扇区的长度，部分 flash 需要是最小可擦除扇区大小的整数倍
 @return  如果 flash 开始执行写入返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_flash_write_one_sector(vsf_flash_t *flash_ptr,
                                            vsf_flash_size_t offset_of_bytes,
                                            uint8_t* buffer,
                                            vsf_flash_size_t size_of_bytes);

/**
 \~english
 @brief flash write multi sector
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] offset_of_bytes: Address of the sector to be written,
                some flash requires an integer multiple of the smallest writable sector size
 @param[in] buffer: a pointer to data
 @param[in] size_of_bytes: a pointer to data
 @return vsf_err_t: VSF_ERR_NONE if flash starts to perform the chip erase, or a negative error code

 \~chinese
 @brief flash 写多个扇区
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] offset_of_bytes: 被写入扇区的地址, 部分 flash 需要是最小可擦除扇区大小的整数倍
 @param[in] buffer: 数据的指针
 @param[in] size_of_bytes: 被写入扇区的长度，部分 flash 需要是最小可擦除扇区大小的整数倍
 @return  如果 flash 开始执行写入返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_flash_write_multi_sector(vsf_flash_t *flash_ptr,
                                              vsf_flash_size_t offset_of_bytes,
                                              uint8_t* buffer,
                                              vsf_flash_size_t size_of_bytes);

/**
 \~english
 @brief flash read one sector
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] offset_of_bytes: Address of the sector to be written,
                some flash requires an integer multiple of the smallest writable sector size
 @param[in] buffer: a pointer to data
 @param[in] size_of_bytes: a pointer to data
 @return vsf_err_t: VSF_ERR_NONE if flash starts to perform the chip erase, or a negative error code

 \~chinese
 @brief flash 读一个扇区
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] offset_of_bytes: 被读入扇区的地址, 部分 flash 需要是最小可擦除扇区大小的整数倍
 @param[in] buffer: 数据的指针
 @param[in] size_of_bytes: 被读入扇区的长度，部分 flash 需要是最小可擦除扇区大小的整数倍
 @return  如果 flash 开始执行读入返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_flash_read_one_sector(vsf_flash_t *flash_ptr,
                                           vsf_flash_size_t offset_of_bytes,
                                           uint8_t* buffer,
                                           vsf_flash_size_t size_of_bytes);

/**
 \~english
 @brief flash read multi sector
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] offset_of_bytes: Address of the sector to be written,
                some flash requires an integer multiple of the smallest writable sector size
 @param[in] buffer: a pointer to data
 @param[in] size_of_bytes: a pointer to data
 @return vsf_err_t: VSF_ERR_NONE if flash starts to perform the chip erase, or a negative error code

 \~chinese
 @brief flash 读多个扇区
 @param[in] flash_ptr: 结构体 vsf_flash_t 的指针，参考 @ref vsf_flash_t
 @param[in] offset_of_bytes: 被读入扇区的地址, 部分 flash 需要是最小可擦除扇区大小的整数倍
 @param[in] buffer: 数据的指针
 @param[in] size_of_bytes: 被读入扇区的长度，部分 flash 需要是最小可擦除扇区大小的整数倍
 @return  如果 flash 开始执行读入返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_flash_read_multi_sector(vsf_flash_t *flash_ptr,
                                             vsf_flash_size_t offset_of_bytes,
                                             uint8_t* buffer,
                                             vsf_flash_size_t size_of_bytes);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_FLASH_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_flash_t                              VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t)
#   define vsf_flash_init(__FLASH, ...)               VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_init)               ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_fini(__FLASH)                    VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_fini)               ((__vsf_flash_t *)__FLASH)
#   define vsf_flash_enable(__FLASH)                  VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_enable)             ((__vsf_flash_t *)__FLASH)
#   define vsf_flash_disable(__FLASH)                 VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_disable)            ((__vsf_flash_t *)__FLASH)
#   define vsf_flash_capability(__FLASH)              VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_capability)         ((__vsf_flash_t *)__FLASH)
#   define vsf_flash_status(__FLASH)                  VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_status)             ((__vsf_flash_t *)__FLASH)
#   define vsf_flash_irq_enable(__FLASH, ...)         VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_irq_enable)         ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_irq_disable(__FLASH, ...)        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_irq_disable)        ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_erase_one_sector(__FLASH, ...)   VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_one_sector)   ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_erase_multi_sector(__FLASH, ...) VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_multi_sector) ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_erase(__FLASH, ...)              VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_multi_sector) ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_erase_all(__FLASH, ...)          VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_all)          ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_write_one_sector(__FLASH, ...)   VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_write_one_sector)   ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_write_multi_sector(__FLASH, ...) VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_write_multi_sector) ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_write(__FLASH, ...)              VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_write_multi_sector) ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_read_one_sector(__FLASH, ...)    VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_read_one_sector)    ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_read_multi_sector(__FLASH, ...)  VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_read_multi_sector)  ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#   define vsf_flash_read(__FLASH, ...)               VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_read_multi_sector)  ((__vsf_flash_t *)__FLASH, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_FLASH_H__*/
