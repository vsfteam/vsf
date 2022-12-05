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

#ifndef __HAL_DRIVER_GPIO_INTERFACE_H__
#define __HAL_DRIVER_GPIO_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_GPIO_CFG_MULTI_CLASS
#   define VSF_GPIO_CFG_MULTI_CLASS                 ENABLED
#endif

/**
 \~english
 VSF_GPIO_CFG_PREFIX is used to set the actual API call when calling the vsf_gpio_*().
 For example, if you configure VSF_GPIO_CFG_PREFIX to be vsf_hw, then call
 vsf_gpio_init(), which is actually vsf_hw_gpio_init()

 If we want the call to vsf_gpio_init in xxxx.c to actually call vsf_extern_gpio_init,
 then it can be configured in front of the .c:

 \~chinese
 VSF_GPIO_CFG_PREFIX 是用来配置调用vsf_gpio_*()的时候实际调用的API。
 例如，当 VSF_GPIO_CFG_PREFIX 配置成 vsf_hw 的时候，调用 vsf_gpio_init()
 实际调用的是 vsf_hw_gpio_init()

 如果我们想要 xxxx.c 里调用 vsf_gpio_init() 实际调用的是 vsf_extern_gpio_init()，
 那么可以在 xxxx.c 源码前面配置：

 \~
 \code {.c}
    #undef  VSF_GPIO_CFG_PREFIX
    #define VSF_GPIO_CFG_PREFIX     vsf_extern
 \endcode
 */
#ifndef VSF_GPIO_CFG_PREFIX
#   if VSF_GPIO_CFG_MULTI_CLASS == ENABLED
#       define VSF_GPIO_CFG_PREFIX                  vsf
#   elif defined(VSF_HW_GPIO_COUNT) && (VSF_HW_GPIO_COUNT != 0)
#       define VSF_GPIO_CFG_PREFIX                  vsf_hw
#   else
#       define VSF_GPIO_CFG_PREFIX                  vsf
#   endif
#endif

#ifndef VSF_GPIO_CFG_FUNCTION_RENAME
#   define VSF_GPIO_CFG_FUNCTION_RENAME             ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_GPIO_APIS(__prefix_name)                                                                                                                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, gpio_capability_t, gpio, capability,       VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr)                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, config_pin,       VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask, vsf_io_feature_t feature)   \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set_direction,    VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask)    \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,          gpio, get_direction,    VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                             \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set_input,        VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                             \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set_output,       VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                             \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, switch_direction, VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                             \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,          gpio, read,             VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr)                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, write,            VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t value, uint32_t pin_mask)             \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set,              VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                             \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, clear,            VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                             \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, toggle,           VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                             \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, output_and_set,   VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                             \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, output_and_clear, VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)

/*============================ TYPES =========================================*/

typedef struct gpio_capability_t {
    inherit(vsf_peripheral_capability_t)

    //! Asynchronous GPIO
    //!  they are only guaranteed to be sequential when operating the current port consecutively.
    //!
    //!  When they are used together with other ports, they are not guaranteed to
    //!  be sequential for the operation of the pins of different ports.
    uint8_t is_async                     : 1;

    uint8_t is_support_output_and_set    : 1;
    uint8_t is_support_output_and_clear  : 1;

    //! total number of pins of hardware, even if some pins cannot be configured as GPIO.
    //! if some of the pins cannot be used as GPIO,the corresponding bit
    //! in avail_pin_mask is 0, otherwise is 1.
    uint8_t pin_count;

    //! available pin mask
    //! may be:
    //!  0x000000FF (8  pins),
    //!  0x0000FFFF (16  pins),
    //!  0xFFFFFFFF (32 pins),
    //!  0xFFFFFFFE (32 pins, but pin0 cannot be used as GPIO)
    uint32_t avail_pin_mask;
} gpio_capability_t;

typedef struct vsf_gpio_t vsf_gpio_t;

typedef struct vsf_gpio_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_GPIO_APIS(vsf)
} vsf_gpio_op_t;

#if VSF_GPIO_CFG_MULTI_CLASS == ENABLED
/**
 \~english
 When enabling multiple classes, the first member of all gpio implementations
 needs to be of type const "vsf_gpio_op_t * op"

 \~chinese
 当使能多类的时候，所有的具体gpio实现的第一个成员都需要是const vsf_gpio_op_t *类型。
 */
struct vsf_gpio_t  {
    const vsf_gpio_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief Configure one or more pins of the gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, the value of
            this bit 1 means configuration is required
 @param[in] feature: enumeration type @ref vsf_io_feature_t
 @note TODO

 \~chinese
 @brief 配置 gpio 实例的一个或者多个引脚
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要配置
 @param[in] feature: 枚举类型 @ref vsf_io_feature_t
 @note TODO
 */
extern void vsf_gpio_config_pin(vsf_gpio_t *gpio_ptr,
                                uint32_t pin_mask,
                                vsf_io_feature_t feature);

/**
 \~english
 @brief Set the orientation of one or more pins of the gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] direction_mask: direction mask, 1 for output, 0 for input
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, the value of
            this bit 1 means configuration is required

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的方向
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] direction_mask: 方向掩码，1 表示输出，0 表示输入
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要配置
 */
extern void vsf_gpio_set_direction(vsf_gpio_t *gpio_ptr,
                                   uint32_t direction_mask,
                                   uint32_t pin_mask);

/**
 \~english
 @brief Get the orientation of one or more pins of the gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, the value of
            this bit 1 means configuration is required
 @return uint32_t: The value of all pin_mask directions, 1 for output, 0 for input

 \~chinese
 @brief 获取 gpio 实例的一个或者多个引脚的方向
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要获取
 @return uint32_t: 所有pin_mask的方向的值，1 表示输出，0 表示输入
 */
extern uint32_t vsf_gpio_get_direction(vsf_gpio_t *gpio_ptr,
                                       uint32_t pin_mask);

/**
 \~english
 @brief Set the direction of one or more pins of the gpio instance to input
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] Pin mask, each pin corresponds to one bit, the value of the bit 1
            indicates the need to set to input

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的方向为输入
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要设置成输入
 */
extern void vsf_gpio_set_input(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

/**
 \~english
 @brief Set the direction of one or more pins of the gpio instance to output
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] Pin mask, each pin corresponds to one bit, the value of the bit 1
            indicates the need to set to output

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的方向为输出
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要设置成输出
 */
extern void vsf_gpio_set_output(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

/**
 \~english
 @brief Toggle the orientation of one or more pins of a gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] Pin mask, each pin corresponds to one bit, the value of the bit 1
            indicates the need to set to output

 \~chinese
 @brief 反转 gpio 实例的一个或者多个引脚的方向
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要反转
 */
extern void vsf_gpio_switch_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

/**
 \~english
 @brief Read the values of all pins of the gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @return uint32_t: Value of all pins, 1 for output, 0 for input

 \~chinese
 @brief 读取 gpio 实例的所有引脚的值
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @return uint32_t: 所有引脚的值，1 表示输出，0 表示输入
 */
extern uint32_t vsf_gpio_read(vsf_gpio_t *gpio_ptr);

/**
 \~english
 @brief set the value of one or more of the gpio instances
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] value: value of the pin, each pin corresponds to a bit, 1 means high, 0 means high
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated

 \~chinese
 @brief 设置 gpio 实例的一个或者多个的值
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] value: 引脚的值，每一个引脚对应一个位，1 表示高电平，0 表示高电平
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 */
extern void vsf_gpio_write( vsf_gpio_t *gpio_ptr,
                                uint32_t value,
                                uint32_t pin_mask);

/**
 \~english
 @brief Set the value of one or more pins of the gpio instance to high
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的值为高电平
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 */
extern void vsf_gpio_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

/**
 \~english
 @brief Set the value of one or more pins of the gpio instance to low
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的值为低电平
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 */
extern void vsf_gpio_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

/**
 \~english
 @brief Toggle the value of one or more pins of the gpio instance
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated

 \~chinese
 @brief 反转 gpio 实例的一个或者多个引脚的值
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 */
extern void vsf_gpio_toggle(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

/**
 \~english
 @brief Set the direction of one or more pins of the gpio instance to output high
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated
 @note This API can be used when modifying the IO direction to output and set to
       high in order to avoid possible pulses. Note that it is not supported by all hardware.
       You can get if this feature is supported with vsf_gpio_capability()
       (is_support_output_and_set)

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的方向为输出高电平
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 @note 修改IO方向为输出并且设置成高电平的时候，为了避免可能的脉冲，可以使用这个API。
       需要注意的是，并不是所有硬件都支持。可以通过 vsf_gpio_capability() 获取是否支持该特性
      （ is_support_output_and_set ）
 */
extern void vsf_gpio_output_and_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

/**
 \~english
 @brief Set the direction of one or more pins of the gpio instance to output low
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated
 @note This API can be used when modifying the IO direction to output and set to
       low in order to avoid possible pulses. Note that it is not supported by all hardware.
       You can get if this feature is supported with vsf_gpio_capability()
       (is_support_output_and_set)

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的方向为输出低电平
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 @note 修改IO方向为输出并且设置成低电平的时候，为了避免可能的脉冲，可以使用这个API。
       需要注意的是，并不是所有硬件都支持。可以通过 vsf_gpio_capability() 获取是否支持该特性
      （ is_support_output_and_set ）
 */
extern void vsf_gpio_output_and_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

/**
 \~english
 @brief Get the capability of gpio instance
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t

 \~chinese
 @brief 获取 gpio 实例的能力
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 */
extern gpio_capability_t vsf_gpio_capability(vsf_gpio_t *gpio_ptr);

/*============================ MACROS ========================================*/

#if VSF_GPIO_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_gpio_t                           VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t)
#   define vsf_gpio_capability(__GPIO)            VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_capability)       ((__vsf_gpio_t *)__GPIO)
#   define vsf_gpio_config_pin(__GPIO, ...)       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_config_pin)       ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_direction(__GPIO, ...)    VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_direction)    ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_get_direction(__GPIO, ...)    VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_get_direction)    ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_input(__GPIO, ...)        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_input)        ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_output(__GPIO, ...)       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_output)       ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_switch_direction(__GPIO, ...) VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_switch_direction) ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_read(__GPIO)                  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_read)             ((__vsf_gpio_t *)__GPIO)
#   define vsf_gpio_write(__GPIO, ...)            VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_write)            ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set(__GPIO, ...)              VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set)              ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_clear(__GPIO, ...)            VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_clear)            ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_output_and_set(__GPIO, ...)   VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_set)   ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_output_and_clear(__GPIO, ...) VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_clear) ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_toggle(__GPIO, ...)           VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_toggle)           ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __HAL_DRIVER_GPIO_INTERFACE_H__ */
