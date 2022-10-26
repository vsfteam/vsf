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

#ifndef __HAL_DRIVER_I2S_INTERFACE_H__
#define __HAL_DRIVER_I2S_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_I2S_CFG_MULTI_CLASS
#   define VSF_I2S_CFG_MULTI_CLASS                  ENABLED
#endif

// application code can redefine it
#ifndef VSF_I2S_CFG_PREFIX
#   if VSF_I2S_CFG_MULTI_CLASS == ENABLED
#       define VSF_I2S_CFG_PREFIX                   vsf
#   elif defined(VSF_HW_I2S_COUNT) && (VSF_HW_I2S_COUNT != 0)
#       define VSF_I2S_CFG_PREFIX                   vsf_hw
#   else
#       define VSF_I2S_CFG_PREFIX                   vsf
#   endif
#endif

#ifndef VSF_I2S_CFG_FUNCTION_RENAME
#   define VSF_I2S_CFG_FUNCTION_RENAME              ENABLED
#endif

#ifndef VSF_I2S_CFG_REQUEST_TEMPLATE
#   define VSF_I2S_CFG_REQUEST_TEMPLATE             DISABLED
#endif

#ifndef VSF_I2S_CFG_REIMPLEMENT_TYPE_FEATURE
#   define VSF_I2S_CFG_REIMPLEMENT_TYPE_FEATURE     DISABLED
#endif

#ifndef VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK    DISABLED
#endif

#ifndef VSF_I2S_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_I2S_CFG_REIMPLEMENT_TYPE_STATUS      DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_I2S_APIS(__prefix_name)                                                                                                                        \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        i2s, init,           VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,         i2s, enable,         VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,         i2s, disable,        VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             i2s, irq_enable,     VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr, vsf_i2s_irq_mask_t irq_mask)      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             i2s, irq_disable,    VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr, vsf_i2s_irq_mask_t irq_mask)      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_i2s_status_t,     i2s, status,         VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_i2s_capability_t, i2s, capability,     VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        i2s, master_request, VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr, uint16_t address, vsf_i2s_cmd_t cmd, uint16_t count, uint8_t* buffer_ptr)

/*============================ TYPES =========================================*/

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_FEATURE == DISABLED
typedef enum vsf_i2s_mode_t {
    I2S_MODE_MASTER             = (0x1ul << 0),     // select master mode
    I2S_MODE_SLAVE              = (0x0ul << 0),     // select slave mode
    I2S_MODE_MASK               = (0x1ul << 0),

    I2S_FORMAT_16B              = (0x01ul << 1),
    I2S_FORMAT_16B_EX32B        = (0x02ul << 1),    // 16-bit extend 32-bit
    I2S_FORMAT_24B              = (0x03ul << 1),
    I2S_FORMAT_32B              = (0x04ul << 1),
    I2S_FORMAT_MASK             = (0x07ul << 1),

    I2S_STANDARD_PHILLIPS       = (0x01ul << 4),
    I2S_STANDARD_MSB            = (0x02ul << 4),
    I2S_STANDARD_LSB            = (0x03ul << 4),
    I2S_STANDARD_MASK           = (0x07ul << 1),

    I2S_FEATURE_MASK            =  I2S_MODE_MASK | I2S_FORMAT_MASK | I2S_STANDARD_MASK,
} vsf_i2s_mode_t;
#endif

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_i2s_irq_mask_t {
    I2S_IRQ_MASK_TX_CPL         = (0x1ul <<  0),
    I2S_IRQ_MASK_RX_CPL         = (0x1ul <<  1),

    I2S_IRQ_MASK_ALL            =  I2S_IRQ_MASK_TX_CPL | I2S_IRQ_MASK_RX_CPL;
} vsf_i2s_irq_mask_t;
#endif

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_i2s_status_t {
    union {
        inherit(peripheral_status_t)
        uint32_t value;
    };
} vsf_i2s_status_t;
#endif

typedef struct vsf_i2s_capability_t {
    inherit(vsf_peripheral_capability_t)
    struct {
        // sample rate converter
        bool is_src_supported;
    } i2s_capability;
} vsf_i2s_capability_t;

typedef struct vsf_i2s_t vsf_i2s_t;

/**
 \~english
 @brief i2s interrupt callback function prototype.
 @note It is called after interrupt occurs.

 @param target_ptr pointer of user.
 @param i2s_ptr pointer of i2s instance.
 @param irq_mask one or more value of enum vsf_i2s_irq_mask_t
 @return None.

 \~chinese
 @brief i2s 中断回调函数原型
 @note 这个回调函数在中断发生之后被调用

 @param target_ptr 用户指针
 @param i2s_ptr i2s 实例的指针
 @param irq_mask 一个或者多个枚举 vsf_i2s_irq_mask_t 的值的按位或
 @return 无。

 \~
 \code {.c}
    static void __user_i2s_irqhandler(void *target_ptr, vsf_i2s_t *i2s_ptr, enum irq_mask)
    {
        if (irq_mask & I2S_IRQ_MASK_MASTER_STARTED) {
            // do something
        }

        if (irq_mask & I2S_IRQ_MASK_MASTER_STOP) {
            // do something
        }
    }
 \endcode
 */
typedef void vsf_i2s_isr_handler_t(void *target_ptr,
                                   vsf_i2s_t *i2s_ptr,
                                   vsf_i2s_irq_mask_t irq_mask);

/**
 \~english
 @brief i2s interrut configuration

 \~chinese
 @brief i2s 中断配置
 */
typedef struct vsf_i2s_isr_t {
    vsf_i2s_isr_handler_t *handler_fn;      //!< \~english TODO
                                            //!< \~chinese 中断回调函数
    void                  *target_ptr;      //!< \~english pointer of user target
                                            //!< \~chinese 用户传入的指针
    vsf_arch_prio_t        prio;            //!< \~english interrupt priority
                                            //!< \~chinesh 中断优先级
} vsf_i2s_isr_t;

/**
 \~english
 @brief i2s configuration

 \~chinese
 @brief i2s 配置
 */
typedef struct vsf_i2s_cfg_t {
    vsf_i2s_mode_t mode;                    //!< \~english i2s mode \ref vsf_i2s_mode_t
                                            //!< \~chinese i2s 模式 \ref vsf_i2s_mode_t
    uint32_t data_clock_hz;                 //!< \~english i2s clock of data (in Hz), valid only if is_src_supported capability is enabled, else ignored
                                            //!< \~chinese i2s 数据的时钟频率 (单位：Hz)，支持 is_src_supported 特性的时候可用，否则忽略
    uint32_t output_clock_hz;               //!< \~english i2s clock of output to codec (in Hz)
                                            //!< \~chinese i2s 输出给 codec 的时钟频率 (单位：Hz)
    vsf_i2s_isr_t isr;                      //!< \~english i2s interrupt
                                            //!< \~chinese i2s 中断
} vsf_i2s_cfg_t;

typedef struct vsf_i2s_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_I2S_APIS(vsf)
} vsf_i2s_op_t;

#if VSF_I2S_CFG_MULTI_CLASS == ENABLED
struct vsf_i2s_t  {
    const vsf_i2s_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_i2s_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if i2s was initialized, or a negative error code

 \~chinese
 @brief 初始化一个 i2s 实例
 @param[in] i2s_ptr: 结构体 vsf_i2s_t 的指针，参考 @ref vsf_i2s_t
 @param[in] cfg_ptr: 结构体 vsf_i2s_cfg_t 的指针，参考 @ref vsf_i2s_cfg_t
 @return vsf_err_t: 如果 i2s 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_i2s_init(vsf_i2s_t *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return none

 \~chinese
 @brief 结束一个 i2s 实例
 @param[in] i2s_ptr: 结构体 vsf_i2s_t 的指针，参考 @ref vsf_i2s_t
 @return none
 */
extern void vsf_i2s_fini(vsf_i2s_t *i2s_ptr);

/**
 \~english
 @brief enable i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return fsm_rt_t: fsm_rt_cpl if enable complete, else return fsm_rt_onging

 \~chinese
 @brief 使能 i2s 实例
 @param[in] i2s_ptr: 结构体 vsf_i2s_t 的指针，参考 @ref vsf_i2s_t
 @return fsm_rt_t: 如果使能成功，返回 fsm_rt_cpl, 未完成初始化返回 fsm_rt_onging
 */
extern fsm_rt_t vsf_i2s_enable(vsf_i2s_t *i2s_ptr);

/**
 \~english
 @brief disable i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return fsm_rt_t: fsm_rt_cpl if disable complete, else return fsm_rt_onging

 \~chinese
 @brief禁能 i2s 实例
 @param[in] i2s_ptr: 结构体 vsf_i2s_t 的指针，参考 @ref vsf_i2s_t
 @return fsm_rt_t: 如果禁能成功，返回 fsm_rt_cpl, 未完成初始化返回 fsm_rt_onging
 */
extern fsm_rt_t vsf_i2s_disable(vsf_i2s_t *i2s_ptr);

/**
 \~english
 @brief enable interrupt masks of i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @param[in] irq_mask: one or more value of enum @ref vsf_i2s_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 i2s 实例的中断
 @param[in] i2s_ptr: 结构体 vsf_i2s_t 的指针，参考 @ref vsf_i2s_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_i2s_irq_mask_t 的值的按位或，@ref vsf_i2s_irq_mask_t
 @return 无。
 */
extern void vsf_i2s_irq_enable(vsf_i2s_t *i2s_ptr, vsf_i2s_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @param[in] irq_mask: one or more value of enum vsf_i2s_irq_mask_t, @ref vsf_i2s_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 i2s 实例的中断
 @param[in] i2s_ptr: 结构体 vsf_i2s_t 的指针，参考 @ref vsf_i2s_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_i2s_irq_mask_t 的值的按位或，@ref vsf_i2s_irq_mask_t
 @return 无。
 */
extern void vsf_i2s_irq_disable(vsf_i2s_t *i2s_ptr, vsf_i2s_irq_mask_t irq_mask);

/**
 \~english
 @brief get the status of i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return vsf_i2s_status_t: return all status of current i2s

 \~chinese
 @brief 获取 i2s 实例的状态
 @param[in] i2s_ptr: 结构体 vsf_i2s_t 的指针，参考 @ref vsf_i2s_t
 @return vsf_i2s_status_t: 返回当前i2s的所有状态
 */
extern vsf_i2s_status_t vsf_i2s_status(vsf_i2s_t *i2s_ptr);

/**
 \~english
 @brief get the capability of i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return vsf_i2s_capability_t: return all capability of current i2s @ref vsf_i2s_capability_t

 \~chinese
 @brief 获取 i2s 实例的能力
 @param[in] i2s_ptr: 结构体 vsf_i2s_t 的指针，参考 @ref vsf_i2s_t
 @return vsf_i2s_capability_t: 返回当前 i2s 的所有能力 @ref vsf_i2s_capability_t
 */
extern vsf_i2s_capability_t vsf_i2s_capability(vsf_i2s_t *i2s_ptr);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_I2S_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_i2s_t                       VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_t)
#   define vsf_i2s_init(__i2s, ...)          VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_init)          ((__vsf_i2s_t *)__i2s, ##__VA_ARGS__)
#   define vsf_i2s_enable(__i2s)             VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_enable)        ((__vsf_i2s_t *)__i2s)
#   define vsf_i2s_disable(__i2s)            VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_disable)       ((__vsf_i2s_t *)__i2s)
#   define vsf_i2s_irq_enable(__i2s, ...)    VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_irq_enable)    ((__vsf_i2s_t *)__i2s, ##__VA_ARGS__)
#   define vsf_i2s_irq_disable(__i2s, ...)   VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_irq_disable)   ((__vsf_i2s_t *)__i2s, ##__VA_ARGS__)
#   define vsf_i2s_status(__i2s)             VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_status)        ((__vsf_i2s_t *)__i2s)
#   define vsf_i2s_capability(__i2s)         VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_capability)    ((__vsf_i2s_t *)__i2s)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_I2S_INTERFACE_H__*/
