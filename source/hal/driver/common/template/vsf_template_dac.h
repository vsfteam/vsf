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

#ifndef __VSF_TEMPLATE_DAC_H__
#define __VSF_TEMPLATE_DAC_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_DAC_CFG_MULTI_CLASS
#   define VSF_DAC_CFG_MULTI_CLASS                      ENABLED
#endif

#if defined(VSF_HW_DAC_COUNT) && !defined(VSF_HW_DAC_MASK)
#   define VSF_HW_DAC_MASK                              VSF_HAL_COUNT_TO_MASK(VSF_HW_DAC_COUNT)
#endif

#if defined(VSF_HW_DAC_MASK) && !defined(VSF_HW_DAC_COUNT)
#   define VSF_HW_DAC_COUNT                             VSF_HAL_MASK_TO_COUNT(VSF_HW_DAC_MASK)
#endif

// application code can redefine it
#ifndef VSF_DAC_CFG_PREFIX
#   if VSF_DAC_CFG_MULTI_CLASS == ENABLED
#       define VSF_DAC_CFG_PREFIX                       vsf
#   elif defined(VSF_HW_DAC_COUNT) && (VSF_HW_DAC_COUNT != 0)
#       define VSF_DAC_CFG_PREFIX                       vsf_hw
#   else
#       define VSF_DAC_CFG_PREFIX                       vsf
#   endif
#endif

#ifndef VSF_DAC_CFG_FUNCTION_RENAME
#   define VSF_DAC_CFG_FUNCTION_RENAME                  ENABLED
#endif

#ifndef VSF_DAC_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_DAC_CFG_REIMPLEMENT_TYPE_IRQ_MASK        DISABLED
#endif

#ifndef VSF_DAC_CFG_INHERT_HAL_CAPABILITY
#   define VSF_DAC_CFG_INHERT_HAL_CAPABILITY            ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_DAC_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            dac, init,                 VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, vsf_dac_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 dac, fini,                 VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             dac, enable,               VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             dac, disable,              VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_dac_status_t,     dac, status,               VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_dac_capability_t, dac, capability,           VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 dac, irq_enable,           VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, vsf_dac_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 dac, irq_disable,          VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, vsf_dac_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            dac, channel_request_once, VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, vsf_dac_channel_cfg_t *cfg, uint_fast16_t value) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            dac, channel_config,       VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, vsf_dac_channel_cfg_t *cfgs_ptr, uint_fast8_t cnt) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            dac, channel_request,      VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, void *values_ptr, uint_fast32_t cnt)

/*============================ TYPES =========================================*/

#if VSF_DAC_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_dac_irq_mask_t {
    VSF_DAC_IRQ_MASK_IDLE = (0x01ul << 0),
    VSF_DAC_IRQ_MASK_CPL  = (0x1ul << 0),
} vsf_dac_irq_mask_t;
#endif

enum {
    VSF_DAC_IRQ_COUNT               = 2,
    VSF_DAC_IRQ_ALL_BITS_MASK       = VSF_DAC_IRQ_MASK_IDLE | VSF_DAC_IRQ_MASK_CPL,
};

//! dac channel configuration
typedef struct vsf_dac_channel_cfg_t {
    uint8_t channel;
    uint8_t resolution;
} vsf_dac_channel_cfg_t;

typedef struct vsf_dac_t vsf_dac_t;

typedef void vsf_dac_isr_handler_t(void *target_ptr, vsf_dac_t *dac_ptr, vsf_dac_irq_mask_t irq_mask);

typedef struct vsf_dac_isr_t {
    vsf_dac_isr_handler_t  *handler_fn;
    void                   *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_dac_isr_t;

//! dac configuration
typedef struct vsf_dac_cfg_t {
    vsf_dac_isr_t   isr;
} vsf_dac_cfg_t;

#if VSF_DAC_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_dac_status_t {
    union {
        inherit(vsf_peripheral_status_t)
            struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_dac_status_t;
#endif

typedef struct vsf_dac_capability_t {
#if VSF_DAC_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_dac_irq_mask_t irq_mask;

    uint8_t max_resolution_bits;
    uint8_t min_resolution_bits;
    uint8_t channel_count;
} vsf_dac_capability_t;

typedef struct vsf_dac_op_t {
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_DAC_APIS(vsf)
} vsf_dac_op_t;

#if VSF_DAC_CFG_MULTI_CLASS == ENABLED
struct vsf_dac_t  {
    const vsf_dac_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a dac instance.
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_dac_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if dac was initialized, or a negative error code

 @note It is not necessary to call vsf_dac_fini() to deinitialization.
       vsf_dac_init() should be called before any other DAC API except vsf_dac_capability().

 \~chinese
 @brief 初始化一个 dac 实例
 @param[in] dac_ptr: 结构体 vsf_dac_t 的指针，参考 @ref vsf_dac_t
 @param[in] cfg_ptr: 结构体 vsf_dac_cfg_t 的指针，参考 @ref vsf_dac_cfg_t
 @return vsf_err_t: 如果 dac 初始化成功返回 VSF_ERR_NONE , 失败返回负数。

 @note 失败后不需要调用 vsf_dac_fini() 反初始化。
       vsf_dac_init() 应该在除 vsf_dac_capability() 之外的其他 DAC API 之前调用。
 */
extern vsf_err_t vsf_dac_init(vsf_dac_t *dac_ptr, vsf_dac_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a dac instance.
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @return none

 \~chinese
 @brief 终止一个 dac 实例
 @param[in] dac_ptr: 结构体 vsf_dac_t 的指针，参考 @ref vsf_dac_t
 @param[in] cfg_ptr: 结构体 vsf_dac_cfg_t 的指针，参考 @ref vsf_dac_cfg_t
 @return 无。
 */
extern void vsf_dac_fini(vsf_dac_t *dac_ptr);

/**
 \~english
 @brief enable interrupt masks of dac instance.
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] irq_mask: one or more value of enum @ref vsf_dac_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 dac 实例的中断
 @param[in] dac_ptr: 结构体 vsf_dac_t 的指针，参考 @ref vsf_dac_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_dac_irq_mask_t 的值的按位或，@ref vsf_dac_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_dac_enable(vsf_dac_t *dac_ptr);

/**
 \~english
 @brief disable interrupt masks of dac instance.
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] irq_mask: one or more value of enum vsf_dac_irq_mask_t, @ref vsf_dac_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 dac 实例的中断
 @param[in] dac_ptr: 结构体 vsf_dac_t 的指针，参考 @ref vsf_dac_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_dac_irq_mask_t 的值的按位或，@ref vsf_dac_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_dac_disable(vsf_dac_t *dac_ptr);

/**
 \~english
 @brief enable interrupt masks of dac instance.
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] irq_mask: one or more value of enum @ref vsf_dac_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 dac 实例的中断
 @param[in] dac_ptr: 结构体 vsf_dac_t 的指针，参考 @ref vsf_dac_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_dac_irq_mask_t 的值的按位或，@ref vsf_dac_irq_mask_t
 @return 无。
 */
extern void vsf_dac_irq_enable(vsf_dac_t *dac_ptr, vsf_dac_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of dac instance.
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] irq_mask: one or more value of enum vsf_dac_irq_mask_t, @ref vsf_dac_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 dac 实例的中断
 @param[in] dac_ptr: 结构体 vsf_dac_t 的指针，参考 @ref vsf_dac_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_dac_irq_mask_t 的值的按位或，@ref vsf_dac_irq_mask_t
 @return 无。
 */
extern void vsf_dac_irq_disable(vsf_dac_t *dac_ptr, vsf_dac_irq_mask_t irq_mask);

/**
 \~english
 @brief get the status of dac instance.
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @return vsf_dac_status_t: return all status of current dac

 \~chinese
 @brief 获取 dac 实例的状态
 @param[in] dac_ptr: 结构体 vsf_dac_t 的指针，参考 @ref vsf_dac_t
 @return vsf_dac_status_t: 返回当前 dac 的所有状态
 */
extern vsf_dac_status_t vsf_dac_status(vsf_dac_t *dac_ptr);

/**
 \~english
 @brief get the capability of dac instance.
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @return vsf_dac_capability_t: return all capability of current dac @ref vsf_dac_capability_t

 \~chinese
 @brief 获取 dac 实例的能力
 @param[in] dac_ptr: 结构体 vsf_dac_t 的指针，参考 @ref vsf_dac_t
 @return vsf_dac_capability_t: 返回当前 dac 的所有能力 @ref vsf_dac_capability_t
 */
extern vsf_dac_capability_t vsf_dac_capability(vsf_dac_t *dac_ptr);


/**
 \~english
 @brief dac request convert once
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] channel_cfg: a pointer to convert channel configuration
 @param[in] buffer_ptr: value to be converted
 @return vsf_err_t: VSF_ERR_NONE if the dac request was successfully, or a negative error code

 \~chinese
 @brief dac 请求转换一次
 @param[in] dac_ptr: 结构体 vsf_dac_t 的指针，参考 @ref vsf_dac_t
 @param[in] channel_cfg: 转换通道配置的指针
 @param[in] value: 待转换的值
 @return vsf_err_t: 如果 dac 请求成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_dac_channel_request_once(vsf_dac_t *dac_ptr,
                                              vsf_dac_channel_cfg_t *channel_cfg,
                                              uint_fast16_t value);

/**
 \~english
 @brief dac channel configuration
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] cfgs_ptr: convert channel configuration array
 @param[in] cnt: the length of convert channel configuration array
 @param[in] buffer_ptr: data buffer
 @return vsf_err_t: VSF_ERR_NONE if the dac request was successfully, or a negative error code

 \~chinese
 @brief dac 通道配置
 @param[in] dac_ptr: 结构体 vsf_dac_t 的指针，参考 @ref vsf_dac_t
 @param[in] cfgs_ptr: 转换通道配置数组
 @param[in] cnt: 转换通道配置数组的长度
 @param[in] buffer_ptr: 数据缓冲区
 @return vsf_err_t: 如果 dac 请求成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_dac_channel_config(vsf_dac_t *dac_ptr,
                                        vsf_dac_channel_cfg_t *cfgs_ptr,
                                        uint_fast8_t cnt);

/**
 \~english
 @brief dac channel request
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[out] buffer_ptr: convert channel data array
 @param[in] count: the length of convert channel configuration data array
 @return vsf_err_t: VSF_ERR_NONE if the dac request was successfully, or a negative error code

 \~chinese
 @brief dac 通道配置
 @param[in] dac_ptr: 结构体 vsf_dac_t 的指针，参考 @ref vsf_dac_t
 @param[out] buffer_ptr: 转换通道配置数据数组
 @param[in] count: 转换通道配置数据数组的长度
 @return vsf_err_t: 如果 dac 请求成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_dac_channel_request(vsf_dac_t *dac_ptr,
                                         void *buffer_ptr,
                                         uint_fast32_t count);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_DAC_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_dac_t                              VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_t)
#   define vsf_dac_init(__DAC, ...)                 VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_init)                 ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#   define vsf_dac_fini(__DAC)                      VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_fini)                 ((__vsf_dac_t *)(__DAC))
#   define vsf_dac_enable(__DAC)                    VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_enable)               ((__vsf_dac_t *)(__DAC))
#   define vsf_dac_disable(__DAC)                   VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_disable)              ((__vsf_dac_t *)(__DAC))
#   define vsf_dac_status(__DAC)                    VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_status)               ((__vsf_dac_t *)(__DAC))
#   define vsf_dac_capability(__DAC)                VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_capability)           ((__vsf_dac_t *)(__DAC))
#   define vsf_dac_irq_enable(__DAC, ...)           VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_irq_enable)           ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#   define vsf_dac_irq_disable(__DAC, ...)          VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_irq_disable)          ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#   define vsf_dac_channel_request_once(__DAC, ...) VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_channel_request_once) ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#   define vsf_dac_channel_config(__DAC, ...)       VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_channel_config)       ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#   define vsf_dac_channel_request(__DAC, ...)      VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_channel_request)      ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_DAC_H__*/
