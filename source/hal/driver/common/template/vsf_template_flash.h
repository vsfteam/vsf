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

/**
 * \~english
 * @brief Enable multi-class support by default for maximum availability.
 *
 * \~chinese
 * @brief 默认启用多类支持，以获得最大可用性。
 */
#ifndef VSF_FLASH_CFG_MULTI_CLASS
#   define VSF_FLASH_CFG_MULTI_CLASS                        ENABLED
#endif

/**
 * \~english
 * @brief Define flash hardware mask if count is defined
 *
 * \~chinese
 * @brief 如果定义了flash硬件数量，则定义对应的掩码
 */
#if defined(VSF_HW_FLASH_COUNT) && !defined(VSF_HW_FLASH_MASK)
#   define VSF_HW_FLASH_MASK                                VSF_HAL_COUNT_TO_MASK(VSF_HW_FLASH_COUNT)
#endif

/**
 * \~english
 * @brief Define flash hardware count if mask is defined
 *
 * \~chinese
 * @brief 如果定义了flash硬件掩码，则定义对应的数量
 */
#if defined(VSF_HW_FLASH_MASK) && !defined(VSF_HW_FLASH_COUNT)
#   define VSF_HW_FLASH_COUNT                               VSF_HAL_MASK_TO_COUNT(VSF_HW_FLASH_MASK)
#endif

/**
 * \~english
 * @brief We can redefine VSF_FLASH_CFG_PREFIX to specify a prefix to call a
 *        specific driver directly in the application code.
 *
 * \~chinese
 * @brief 可以重定义 VSF_FLASH_CFG_PREFIX 来指定一个前缀，用于在应用代码中
 *        直接调用特定驱动。
 */
#ifndef VSF_FLASH_CFG_PREFIX
#   if VSF_FLASH_CFG_MULTI_CLASS == ENABLED
#       define VSF_FLASH_CFG_PREFIX                         vsf
#   elif defined(VSF_HW_FLASH_COUNT) && (VSF_HW_FLASH_COUNT != 0)
#       define VSF_FLASH_CFG_PREFIX                         vsf_hw
#   else
#       define VSF_FLASH_CFG_PREFIX                         vsf
#   endif
#endif

/**
 * \~english
 * @brief Disable VSF_FLASH_CFG_FUNCTION_RENAME to use the original function names
 *        (e.g., vsf_flash_init()).
 *
 * \~chinese
 * @brief 禁用 VSF_FLASH_CFG_FUNCTION_RENAME 以使用原始函数名
 *        (例如 vsf_flash_init())。
 */
#ifndef VSF_FLASH_CFG_FUNCTION_RENAME
#   define VSF_FLASH_CFG_FUNCTION_RENAME                    ENABLED
#endif

/**
 * \~english
 * @brief Enable option to reimplement interrupt mask type in specific hardware drivers.
 *
 * \~chinese
 * @brief 启用在特定硬件驱动中重新实现中断掩码类型的选项。
 */
#ifndef VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK          DISABLED
#endif

/**
 * \~english
 * @brief Enable option to reimplement flash size type in specific hardware drivers.
 *
 * \~chinese
 * @brief 在具体硬件驱动中启用重新实现flash大小类型的选项。
 */
#ifndef VSF_FLASH_CFG_REIMPLEMENT_TYPE_FLASH_SIZE
#   define VSF_FLASH_CFG_REIMPLEMENT_TYPE_FLASH_SIZE        DISABLED
#endif

/**
 * \~english
 * @brief Enable option to reimplement status type in specific hardware drivers.
 *
 * \~chinese
 * @brief 在具体硬件驱动中启用重新实现状态类型的选项。
 */
#ifndef VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS            DISABLED
#endif

/**
 * \~english
 * @brief Enable option to reimplement configuration type.
 * For compatibility, members should not be deleted when redefining.
 *
 * \~chinese
 * @brief 启用重新实现配置类型的选项。
 * 为保证兼容性，重新定义时不应删除成员。
 */
#ifndef VSF_FLASH_CFG_REIMPLEMENT_TYPE_CFG
#   define VSF_FLASH_CFG_REIMPLEMENT_TYPE_CFG               DISABLED
#endif

/**
 * \~english
 * @brief Enable option to reimplement capability type.
 * For compatibility, members should not be deleted when redefining.
 *
 * \~chinese
 * @brief 启用重新实现能力类型的选项。
 * 为保证兼容性，重新定义时不应删除成员。
 */
#ifndef VSF_FLASH_CFG_REIMPLEMENT_TYPE_CAPABILITY
#   define VSF_FLASH_CFG_REIMPLEMENT_TYPE_CAPABILITY        DISABLED
#endif

/**
 * \~english
 * @brief Enable inheriting HAL capability to reuse common capability definitions.
 *
 * \~chinese
 * @brief 启用继承HAL能力以重用通用能力定义。
 */
#ifndef VSF_FLASH_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_FLASH_CFG_INHERIT_HAL_CAPABILITY              ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_FLASH_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, init,                  VSF_MCONNECT(__prefix_name, _t) *flash_ptr, vsf_flash_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   flash, fini,                  VSF_MCONNECT(__prefix_name, _t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, get_configuration,     VSF_MCONNECT(__prefix_name, _t) *flash_ptr, vsf_flash_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               flash, enable,                VSF_MCONNECT(__prefix_name, _t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               flash, disable,               VSF_MCONNECT(__prefix_name, _t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_flash_status_t,     flash, status,                VSF_MCONNECT(__prefix_name, _t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_flash_capability_t, flash, capability,            VSF_MCONNECT(__prefix_name, _t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   flash, irq_enable,            VSF_MCONNECT(__prefix_name, _t) *flash_ptr, vsf_flash_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   flash, irq_disable,           VSF_MCONNECT(__prefix_name, _t) *flash_ptr, vsf_flash_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, erase_one_sector,      VSF_MCONNECT(__prefix_name, _t) *flash_ptr, vsf_flash_size_t offset_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, erase_multi_sector,    VSF_MCONNECT(__prefix_name, _t) *flash_ptr, vsf_flash_size_t offset_of_bytes, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, erase_all,             VSF_MCONNECT(__prefix_name, _t) *flash_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, write_one_sector,      VSF_MCONNECT(__prefix_name, _t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, write_multi_sector,    VSF_MCONNECT(__prefix_name, _t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, read_one_sector,       VSF_MCONNECT(__prefix_name, _t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              flash, read_multi_sector,     VSF_MCONNECT(__prefix_name, _t) *flash_ptr, vsf_flash_size_t offset_of_bytes, uint8_t* buffer, vsf_flash_size_t size_of_bytes)

/*============================ TYPES =========================================*/

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
/**
 * \~english
 * @brief Predefined VSF Flash interrupts that can be reimplemented in specific HAL drivers
 *
 * \~chinese
 * @brief 预定义的 VSF Flash 中断，可以在具体的 HAL 驱动重新实现
 */
typedef enum vsf_flash_irq_mask_t {
    VSF_FLASH_IRQ_ERASE_MASK        = (0x1ul << 0),  //!< \~english Erase operation complete \~chinese 擦除操作完成
    VSF_FLASH_IRQ_WRITE_MASK        = (0x1ul << 1),  //!< \~english Write operation complete \~chinese 写入操作完成
    VSF_FLASH_IRQ_READ_MASK         = (0x1ul << 2),  //!< \~english Read operation complete \~chinese 读取操作完成

    VSF_FLASH_IRQ_ERASE_ERROR_MASK  = (0x1ul << 3),  //!< \~english Erase operation error \~chinese 擦除操作错误
    VSF_FLASH_IRQ_WRITE_ERROR_MASK  = (0x1ul << 4),  //!< \~english Write operation error \~chinese 写入操作错误
    VSF_FLASH_IRQ_READ_ERROR_MASK   = (0x1ul << 5),  //!< \~english Read operation error \~chinese 读取操作错误
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

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
/**
 * \~english
 * @brief Flash structure and related types for configuration
 *
 * \~chinese
 * @brief Flash 结构体和配置相关类型
 */
typedef struct vsf_flash_t vsf_flash_t;
typedef void vsf_flash_isr_handler_t(void *target_ptr, vsf_flash_t *flash_ptr,
                                     vsf_flash_irq_mask_t mask);
typedef struct vsf_flash_isr_t {
    vsf_flash_isr_handler_t *handler_fn;  //!< \~english Interrupt handler function \~chinese 中断处理函数
    void                    *target_ptr;  //!< \~english User target pointer \~chinese 用户目标指针
    vsf_arch_prio_t          prio;        //!< \~english Interrupt priority \~chinese 中断优先级
} vsf_flash_isr_t;
typedef struct vsf_flash_cfg_t {
    vsf_flash_isr_t isr;                  //!< \~english Interrupt configuration \~chinese 中断配置
} vsf_flash_cfg_t;
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
/**
 * \~english
 * @brief Flash status structure that can be reimplemented in specific HAL drivers
 *
 * \~chinese
 * @brief 可在具体 HAL 驱动中重新实现的 Flash 状态结构体
 */
typedef struct vsf_flash_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy : 1;         //!< \~english Flash is busy \~chinese Flash 处于忙状态
        };
    };
} vsf_flash_status_t;
#endif

#if VSF_FLASH_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
typedef struct vsf_flash_capability_t {
#if VSF_FLASH_CFG_INHERIT_HAL_CAPABILITY == ENABLED
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
#endif

typedef struct vsf_flash_op_t {
/// @cond
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_FLASH_APIS(vsf_flash)
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
 @brief Initialize a FLASH instance
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] cfg_ptr: a pointer to configuration structure @ref vsf_flash_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if initialization successful, otherwise returns error code

 @note It is not necessary to call vsf_flash_fini() to deinitialization.
 @note vsf_flash_init() should be called before any other FLASH API except vsf_flash_capability().

 \~chinese
 @brief 初始化一个 FLASH 实例
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @param[in] cfg_ptr: 指向配置结构体 @ref vsf_flash_cfg_t 的指针
 @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码

 @note 初始化失败后不需要调用 vsf_flash_fini() 进行反初始化。
 @note vsf_flash_init() 应该在除 vsf_flash_capability() 之外的其他 FLASH API 之前调用。
 */
extern vsf_err_t vsf_flash_init(vsf_flash_t *flash_ptr, vsf_flash_cfg_t *cfg_ptr);

/**
 \~english
 @brief Finalize a FLASH instance
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @return none

 \~chinese
 @brief 终止一个 FLASH 实例
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @return 无
 */
extern void vsf_flash_fini(vsf_flash_t *flash_ptr);

/**
 * \~english
 * @brief Get current configuration of a FLASH instance
 * @param[in] flash_ptr Pointer to FLASH instance
 * @param[out] cfg_ptr Pointer to FLASH configuration structure to store current settings
 * @return vsf_err_t VSF_ERR_NONE if successful, otherwise an error code
 *
 * @note This function retrieves the current configuration of the FLASH instance
 *
 * \~chinese
 * @brief 获取 FLASH 实例的当前配置
 * @param[in] flash_ptr FLASH 实例指针
 * @param[out] cfg_ptr 用于存储当前设置的 FLASH 配置结构体指针
 * @return vsf_err_t 成功返回 VSF_ERR_NONE，否则返回错误码
 *
 * @note 此函数获取 FLASH 实例的当前配置
 */
extern vsf_err_t vsf_flash_get_configuration(vsf_flash_t *flash_ptr, vsf_flash_cfg_t *cfg_ptr);

/**
 \~english
 @brief Enable a FLASH instance
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @return fsm_rt_t: fsm_rt_cpl if FLASH was enabled, fsm_rt_on_going if FLASH is still enabling

 \~chinese
 @brief 启用一个 FLASH 实例
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @return fsm_rt_t: 如果 FLASH 实例已启用返回 fsm_rt_cpl，如果 FLASH 实例正在启用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_flash_enable(vsf_flash_t *flash_ptr);

/**
 \~english
 @brief Disable a FLASH instance
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @return fsm_rt_t: fsm_rt_cpl if FLASH was disabled, fsm_rt_on_going if FLASH is still disabling

 \~chinese
 @brief 禁用一个 FLASH 实例
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @return fsm_rt_t: 如果 FLASH 实例已禁用返回 fsm_rt_cpl，如果 FLASH 实例正在禁用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_flash_disable(vsf_flash_t *flash_ptr);

/**
 \~english
 @brief Get the status of FLASH instance
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @return vsf_flash_status_t: All status of current FLASH @ref vsf_flash_status_t

 \~chinese
 @brief 获取 FLASH 实例的状态
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @return vsf_flash_status_t: 返回当前 FLASH 的所有状态 @ref vsf_flash_status_t
 */
extern vsf_flash_status_t vsf_flash_status(vsf_flash_t *flash_ptr);

/**
 \~english
 @brief Get the capabilities of FLASH instance
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @return vsf_flash_capability_t: All capabilities of current FLASH @ref vsf_flash_capability_t

 \~chinese
 @brief 获取 FLASH 实例的能力
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @return vsf_flash_capability_t: 返回当前 FLASH 的所有能力 @ref vsf_flash_capability_t
 */
extern vsf_flash_capability_t vsf_flash_capability(vsf_flash_t *flash_ptr);


/**
 \~english
 @brief Enable interrupt masks of FLASH instance
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] irq_mask: one or more value of enum @ref vsf_flash_irq_mask_t
 @return none
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 启用 FLASH 实例的中断
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @param[in] irq_mask: 一个或多个枚举 vsf_flash_irq_mask_t 的值的按位或，@ref vsf_flash_irq_mask_t
 @return 无
 @note 在中断启用之前，应该清除所有挂起的中断。
 */
extern void vsf_flash_irq_enable(vsf_flash_t *flash_ptr, vsf_flash_irq_mask_t irq_mask);

/**
 \~english
 @brief Disable interrupt masks of FLASH instance
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] irq_mask: one or more value of enum vsf_flash_irq_mask_t, @ref vsf_flash_irq_mask_t
 @return none

 \~chinese
 @brief 禁用 FLASH 实例的中断
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @param[in] irq_mask: 一个或多个枚举 vsf_flash_irq_mask_t 的值的按位或，@ref vsf_flash_irq_mask_t
 @return 无
 */
extern void vsf_flash_irq_disable(vsf_flash_t *flash_ptr, vsf_flash_irq_mask_t irq_mask);

/**
 \~english
 @brief FLASH erase one sector
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] offset_of_bytes: The address of the sector to be erased,
            needs to be an integer multiple of the smallest erasable sector size
 @return vsf_err_t: VSF_ERR_NONE if FLASH starts to perform the erase, otherwise returns error code

 \~chinese
 @brief FLASH 擦除一块扇区
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @param[in] offset_of_bytes: 被擦除扇区的地址，需要是最小可擦除扇区的整数倍
 @return  如果 FLASH 开始执行擦除返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_flash_erase_one_sector(vsf_flash_t *flash_ptr,
                                            vsf_flash_size_t offset_of_bytes);

/**
 \~english
 @brief Flash erase a continuous range
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] offset_of_bytes: The address of the sector to be erased,
            needs to be an integer multiple of the smallest erasable sector size
 @param[in] size_of_bytes: Size in bytes to erase, needs to be an integer multiple of the
            smallest erasable sector size
 @return vsf_err_t: VSF_ERR_NONE if flash starts to perform the erase, or a negative error code

 \~chinese
 @brief flash 擦除连续区域
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @param[in] offset_of_bytes: 被擦除扇区的地址，需要是最小可擦除扇区大小的整数倍
 @param[in] size_of_bytes: 被擦除区域的大小，需要是最小可擦除扇区大小的整数倍
 @return  如果 flash 开始执行擦除返回 VSF_ERR_NONE，否则返回负数
 */
extern vsf_err_t vsf_flash_erase_multi_sector(vsf_flash_t *flash_ptr,
                                              vsf_flash_size_t offset_of_bytes,
                                              vsf_flash_size_t size_of_bytes);

/**
 \~english
 @brief Flash chip erase
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @return vsf_err_t: VSF_ERR_NONE if flash starts to perform the chip erase, or a negative error code

 \~chinese
 @brief flash 整片擦除
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @return  如果 flash 开始执行整片擦除返回 VSF_ERR_NONE，否则返回负数
 */
extern vsf_err_t vsf_flash_erase_all(vsf_flash_t *flash_ptr);


/**
 \~english
 @brief Flash write one sector
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] offset_of_bytes: Address of the sector to be written,
                some flash requires an integer multiple of the smallest writable sector size
 @param[in] buffer: a pointer to data
 @param[in] size_of_bytes: a pointer to data
 @return vsf_err_t: VSF_ERR_NONE if flash starts to perform the write, or a negative error code

 \~chinese
 @brief flash 写一个扇区
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @param[in] offset_of_bytes: 被写入扇区的地址, 部分 flash 需要是最小可写入扇区大小的整数倍
 @param[in] buffer: 数据的指针
 @param[in] size_of_bytes: 被写入扇区的长度，部分 flash 需要是最小可写入扇区大小的整数倍
 @return  如果 flash 开始执行写入返回 VSF_ERR_NONE，否则返回负数
 */
extern vsf_err_t vsf_flash_write_one_sector(vsf_flash_t *flash_ptr,
                                            vsf_flash_size_t offset_of_bytes,
                                            uint8_t* buffer,
                                            vsf_flash_size_t size_of_bytes);

/**
 \~english
 @brief Flash write multi sector
 @param[in] flash_ptr: a pointer to structure @ref vsf_flash_t
 @param[in] offset_of_bytes: Address of the sector to be written,
                some flash requires an integer multiple of the smallest writable sector size
 @param[in] buffer: a pointer to data
 @param[in] size_of_bytes: size of data to write
 @return vsf_err_t: VSF_ERR_NONE if flash starts to perform the write, or a negative error code

 \~chinese
 @brief flash 写多个扇区
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @param[in] offset_of_bytes: 被写入扇区的地址, 部分 flash 需要是最小可擦除扇区大小的整数倍
 @param[in] buffer: 数据的指针
 @param[in] size_of_bytes: 被写入扇区的长度，部分 flash 需要是最小可擦除扇区大小的整数倍
 @return  如果 flash 开始执行写入返回 VSF_ERR_NONE，否则返回负数
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
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
 @param[in] offset_of_bytes: 被读入扇区的地址, 部分 flash 需要是最小可读入扇区大小的整数倍
 @param[in] buffer: 数据的指针
 @param[in] size_of_bytes: 被读入扇区的长度，部分 flash 需要是最小可读入扇区大小的整数倍
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
 @param[in] flash_ptr: 指向结构体 @ref vsf_flash_t 的指针
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

/// @cond
#if VSF_FLASH_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_flash_t                              VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_t)
#   define vsf_flash_init(__FLASH, ...)               VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_init)               ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_fini(__FLASH)                    VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_fini)               ((__vsf_flash_t *)(__FLASH))
#   define vsf_flash_get_configuration(__FLASH, ...)  VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_get_configuration)  ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_enable(__FLASH)                  VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_enable)             ((__vsf_flash_t *)(__FLASH))
#   define vsf_flash_disable(__FLASH)                 VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_disable)            ((__vsf_flash_t *)(__FLASH))
#   define vsf_flash_capability(__FLASH)              VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_capability)         ((__vsf_flash_t *)(__FLASH))
#   define vsf_flash_status(__FLASH)                  VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_status)             ((__vsf_flash_t *)(__FLASH))
#   define vsf_flash_irq_enable(__FLASH, ...)         VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_irq_enable)         ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_irq_disable(__FLASH, ...)        VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_irq_disable)        ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_erase_one_sector(__FLASH, ...)   VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_one_sector)   ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_erase_multi_sector(__FLASH, ...) VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_multi_sector) ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_erase(__FLASH, ...)              VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_multi_sector) ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_erase_all(__FLASH, ...)          VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_erase_all)          ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_write_one_sector(__FLASH, ...)   VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_write_one_sector)   ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_write_multi_sector(__FLASH, ...) VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_write_multi_sector) ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_write(__FLASH, ...)              VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_write_multi_sector) ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_read_one_sector(__FLASH, ...)    VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_read_one_sector)    ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_read_multi_sector(__FLASH, ...)  VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_read_multi_sector)  ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#   define vsf_flash_read(__FLASH, ...)               VSF_MCONNECT(VSF_FLASH_CFG_PREFIX, _flash_read_multi_sector)  ((__vsf_flash_t *)(__FLASH), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_FLASH_H__*/
