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

#ifndef __VSF_TEMPLATE_ADC_H__
#define __VSF_TEMPLATE_ADC_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_ADC_CFG_MULTI_CLASS
#   define VSF_ADC_CFG_MULTI_CLASS                      ENABLED
#endif

#if defined(VSF_HW_ADC_COUNT) && !defined(VSF_HW_ADC_MASK)
#   define VSF_HW_ADC_MASK                              VSF_HAL_COUNT_TO_MASK(VSF_HW_ADC_COUNT)
#endif

#if defined(VSF_HW_ADC_MASK) && !defined(VSF_HW_ADC_COUNT)
#   define VSF_HW_ADC_COUNT                             VSF_HAL_MASK_TO_COUNT(VSF_HW_ADC_MASK)
#endif

// application code can redefine it
#ifndef VSF_ADC_CFG_PREFIX
#   if VSF_ADC_CFG_MULTI_CLASS == ENABLED
#       define VSF_ADC_CFG_PREFIX                       vsf
#   elif defined(VSF_HW_ADC_COUNT) && (VSF_HW_ADC_COUNT != 0)
#       define VSF_ADC_CFG_PREFIX                       vsf_hw
#   else
#       define VSF_ADC_CFG_PREFIX                       vsf
#   endif
#endif

#ifndef VSF_ADC_CFG_FUNCTION_RENAME
#   define VSF_ADC_CFG_FUNCTION_RENAME                  ENABLED
#endif

#ifndef VSF_ADC_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_ADC_CFG_REIMPLEMENT_TYPE_MODE            DISABLED
#endif

#ifndef VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE
#   define VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE    DISABLED
#endif

#ifndef VSF_ADC_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_ADC_CFG_REIMPLEMENT_TYPE_IRQ_MASK        DISABLED
#endif

#ifndef VSF_ADC_CFG_INHERT_HAL_CAPABILITY
#   define VSF_ADC_CFG_INHERT_HAL_CAPABILITY            ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_ADC_APIS(__PREFIX_NAME) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            adc, init,                 VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, vsf_adc_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 adc, fini,                 VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, fsm_rt_t,             adc, enable,               VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, fsm_rt_t,             adc, disable,              VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_adc_status_t,     adc, status,               VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_adc_capability_t, adc, capability,           VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 adc, irq_enable,           VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, vsf_adc_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 adc, irq_disable,          VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, vsf_adc_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            adc, channel_request_once, VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, vsf_adc_channel_cfg_t *channel_cfg, void *buffer_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            adc, channel_config,       VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, vsf_adc_channel_cfg_t *channel_cfgs_ptr, uint32_t channel_cfgs_cnt) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            adc, channel_request,      VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, void *buffer_ptr, uint_fast32_t count)

/*============================ TYPES =========================================*/

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
typedef enum vsf_adc_mode_t {
    VSF_ADC_REF_VDD_1               = (0 << 0),
    VSF_ADC_REF_VDD_1_2             = (1 << 0),
    VSF_ADC_REF_VDD_1_3             = (2 << 0),
    VSF_ADC_REF_VDD_1_4             = (3 << 0),

    VSF_ADC_DATA_ALIGN_RIGHT        = (0 << 2),  //!< ADC data alignment to right
    VSF_ADC_DATA_ALIGN_LEFT         = (1 << 2),  //!< ADC data alignment to left

    VSF_ADC_SCAN_CONV_SINGLE_MODE   = (0 << 3),  //!< Conversion is performed in single mode
    VSF_ADC_SCAN_CONV_SEQUENCE_MODE = (1 << 3),  //!< Conversions are performed in sequence mode

    //! Selects the external event used to trigger the conversion
    VSF_ADC_EXTERN_TRIGGER_0        = (0 << 4),
    VSF_ADC_EXTERN_TRIGGER_1        = (1 << 4),
    VSF_ADC_EXTERN_TRIGGER_2        = (2 << 4),
} vsf_adc_mode_t;
#endif

enum {
    VSF_ADC_REF_VDD_COUNT           = 4,
    VSF_ADC_REF_VDD_MASK            = VSF_ADC_REF_VDD_1 |
                                      VSF_ADC_REF_VDD_1_2 |
                                      VSF_ADC_REF_VDD_1_3 |
                                      VSF_ADC_REF_VDD_1_4,

    VSF_ADC_DATA_ALIGN_COUNT        = 2,
    VSF_ADC_DATA_ALIGN_MASK         = VSF_ADC_DATA_ALIGN_RIGHT |
                                      VSF_ADC_DATA_ALIGN_LEFT,

    VSF_ADC_SCAN_CONV_COUNT         = 2,
    VSF_ADC_SCAN_CONV_MASK          = VSF_ADC_SCAN_CONV_SINGLE_MODE |
                                      VSF_ADC_SCAN_CONV_SEQUENCE_MODE,

    VSF_ADC_EXTERN_TRIGGER_COUNT    = 3,
    VSF_ADC_EXTERN_TRIGGER_MASK     = VSF_ADC_EXTERN_TRIGGER_0 |
                                      VSF_ADC_EXTERN_TRIGGER_1 |
                                      VSF_ADC_EXTERN_TRIGGER_2,

    VSF_ADC_MODE_MASK_COUNT         = 4,
    VSF_ADC_MODE_ALL_BITS_MASK      = VSF_ADC_REF_VDD_MASK |
                                      VSF_ADC_DATA_ALIGN_MASK |
                                      VSF_ADC_SCAN_CONV_MASK |
                                      VSF_ADC_EXTERN_TRIGGER_MASK,
};

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE == DISABLED
typedef enum vsf_adc_channel_mode_t {
    VSF_ADC_CHANNEL_GAIN_1_6        = (0 << 0),
    VSF_ADC_CHANNEL_GAIN_1_5        = (1 << 0),
    VSF_ADC_CHANNEL_GAIN_1_4        = (2 << 0),
    VSF_ADC_CHANNEL_GAIN_1_3        = (3 << 0),
    VSF_ADC_CHANNEL_GAIN_1_2        = (4 << 0),
    VSF_ADC_CHANNEL_GAIN_1          = (5 << 0),

    VSF_ADC_CHANNEL_REF_VDD_1       = (0 << 4),
    VSF_ADC_CHANNEL_REF_VDD_1_2     = (1 << 4),
    VSF_ADC_CHANNEL_REF_VDD_1_3     = (2 << 4),
    VSF_ADC_CHANNEL_REF_VDD_1_4     = (3 << 4),
} vsf_adc_channel_mode_t;
#endif

enum {
    VSF_ADC_CHANNEL_GAIN_COUNT      = 6,
    VSF_ADC_CHANNEL_GAIN_MASK       = VSF_ADC_CHANNEL_GAIN_1_6 |
                                      VSF_ADC_CHANNEL_GAIN_1_5 |
                                      VSF_ADC_CHANNEL_GAIN_1_4 |
                                      VSF_ADC_CHANNEL_GAIN_1_3 |
                                      VSF_ADC_CHANNEL_GAIN_1_2 |
                                      VSF_ADC_CHANNEL_GAIN_1,

    VSF_ADC_CHANNEL_REF_VDD_COUNT   = 4,
    VSF_ADC_CHANNEL_REF_VDD_MASK    = VSF_ADC_CHANNEL_REF_VDD_1 |
                                      VSF_ADC_CHANNEL_REF_VDD_1_2 |
                                      VSF_ADC_CHANNEL_REF_VDD_1_3 |
                                      VSF_ADC_CHANNEL_REF_VDD_1_4,

    VSF_ADC_CHANNEL_MODE_MASK_COUNT = 2,
    VSF_ADC_CHANNEL_MODE_ALL_BITS_MASK
                                    = VSF_ADC_CHANNEL_GAIN_MASK |
                                      VSF_ADC_CHANNEL_REF_VDD_MASK,
};

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_adc_irq_mask_t {
    VSF_ADC_IRQ_MASK_CPL = (0x1ul << 0),
} vsf_adc_irq_mask_t;
#endif

enum {
    VSF_ADC_IRQ_COUNT               = 1,
    VSF_ADC_IRQ_ALL_BITS_MASK       = VSF_ADC_IRQ_MASK_CPL,
};

//! adc channel configuration
typedef struct vsf_adc_channel_cfg_t {
    vsf_adc_channel_mode_t mode;    // Channel mode
    uint16_t sample_cycles;
    uint8_t  channel;       // Channel Index
} vsf_adc_channel_cfg_t;

typedef struct vsf_adc_t vsf_adc_t;

typedef void vsf_adc_isr_handler_t(void *target_ptr,
                                   vsf_adc_t *adc_ptr,
                                   vsf_adc_irq_mask_t irq_mask);

typedef struct vsf_adc_isr_t {
    vsf_adc_isr_handler_t  *handler_fn;
    void                   *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_adc_isr_t;

//! adc configuration
typedef struct vsf_adc_cfg_t {
    vsf_adc_mode_t  mode;
    vsf_adc_isr_t   isr;
    uint32_t        clock_hz;
} vsf_adc_cfg_t;

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_adc_status_t {
    union {
        inherit(vsf_peripheral_status_t)
            struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_adc_status_t;
#endif

typedef struct vsf_adc_capability_t {
#if VSF_ADC_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_adc_irq_mask_t irq_mask;

    uint8_t max_data_bits;
    uint8_t channel_count;
} vsf_adc_capability_t;

typedef struct vsf_adc_op_t {
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_ADC_APIS(vsf)
} vsf_adc_op_t;

#if VSF_ADC_CFG_MULTI_CLASS == ENABLED
struct vsf_adc_t  {
    const vsf_adc_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a adc instance.
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_adc_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if adc was initialized, or a negative error code

 @note It is not necessary to call vsf_adc_fini() to deinitialization.
       vsf_adc_init() should be called before any other ADC API except vsf_adc_capability().

 \~chinese
 @brief 初始化一个 adc 实例
 @param[in] adc_ptr: 结构体 vsf_adc_t 的指针，参考 @ref vsf_adc_t
 @param[in] cfg_ptr: 结构体 vsf_adc_cfg_t 的指针，参考 @ref vsf_adc_cfg_t
 @return vsf_err_t: 如果 adc 初始化成功返回 VSF_ERR_NONE , 失败返回负数。

 @note 失败后不需要调用 vsf_adc_fini() 反初始化。
       vsf_adc_init() 应该在除 vsf_adc_capability() 之外的其他 ADC API 之前调用。
 */
extern vsf_err_t vsf_adc_init(vsf_adc_t *adc_ptr, vsf_adc_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a adc instance.
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @return none

 \~chinese
 @brief 终止一个 adc 实例
 @param[in] adc_ptr: 结构体 vsf_adc_t 的指针，参考 @ref vsf_adc_t
 @param[in] cfg_ptr: 结构体 vsf_adc_cfg_t 的指针，参考 @ref vsf_adc_cfg_t
 @return 无。
 */
extern void vsf_adc_fini(vsf_adc_t *adc_ptr);

/**
 \~english
 @brief enable interrupt masks of adc instance.
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] irq_mask: one or more value of enum @ref vsf_adc_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 adc 实例的中断
 @param[in] adc_ptr: 结构体 vsf_adc_t 的指针，参考 @ref vsf_adc_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_adc_irq_mask_t 的值的按位或，@ref vsf_adc_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_adc_enable(vsf_adc_t *adc_ptr);

/**
 \~english
 @brief disable interrupt masks of adc instance.
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] irq_mask: one or more value of enum vsf_adc_irq_mask_t, @ref vsf_adc_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 adc 实例的中断
 @param[in] adc_ptr: 结构体 vsf_adc_t 的指针，参考 @ref vsf_adc_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_adc_irq_mask_t 的值的按位或，@ref vsf_adc_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_adc_disable(vsf_adc_t *adc_ptr);

/**
 \~english
 @brief enable interrupt masks of adc instance.
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] irq_mask: one or more value of enum @ref vsf_adc_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 adc 实例的中断
 @param[in] adc_ptr: 结构体 vsf_adc_t 的指针，参考 @ref vsf_adc_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_adc_irq_mask_t 的值的按位或，@ref vsf_adc_irq_mask_t
 @return 无。
 */
extern void vsf_adc_irq_enable(vsf_adc_t *adc_ptr, vsf_adc_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of adc instance.
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] irq_mask: one or more value of enum vsf_adc_irq_mask_t, @ref vsf_adc_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 adc 实例的中断
 @param[in] adc_ptr: 结构体 vsf_adc_t 的指针，参考 @ref vsf_adc_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_adc_irq_mask_t 的值的按位或，@ref vsf_adc_irq_mask_t
 @return 无。
 */
extern void vsf_adc_irq_disable(vsf_adc_t *adc_ptr, vsf_adc_irq_mask_t irq_mask);

/**
 \~english
 @brief get the status of adc instance.
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @return vsf_adc_status_t: return all status of current adc

 \~chinese
 @brief 获取 adc 实例的状态
 @param[in] adc_ptr: 结构体 vsf_adc_t 的指针，参考 @ref vsf_adc_t
 @return vsf_adc_status_t: 返回当前 adc 的所有状态
 */
extern vsf_adc_status_t vsf_adc_status(vsf_adc_t *adc_ptr);

/**
 \~english
 @brief get the capability of adc instance.
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @return vsf_adc_capability_t: return all capability of current adc @ref vsf_adc_capability_t

 \~chinese
 @brief 获取 adc 实例的能力
 @param[in] adc_ptr: 结构体 vsf_adc_t 的指针，参考 @ref vsf_adc_t
 @return vsf_adc_capability_t: 返回当前 adc 的所有能力 @ref vsf_adc_capability_t
 */
extern vsf_adc_capability_t vsf_adc_capability(vsf_adc_t *adc_ptr);

/**
 \~english
 @brief adc request sampling once
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] channel_cfg: a pointer to sampling channel configuration
 @param[in] buffer_ptr: data buffer
 @return vsf_err_t: VSF_ERR_NONE if the adc request was successfully, or a negative error code

 \~chinese
 @brief adc 请求采样一次
 @param[in] adc_ptr: 结构体 vsf_adc_t 的指针，参考 @ref vsf_adc_t
 @param[in] channel_cfg: 采样通道配置的指针
 @param[in] buffer_ptr: 数据缓冲区
 @return vsf_err_t: 如果 adc 请求成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_adc_channel_request_once(vsf_adc_t *adc_ptr,
                                              vsf_adc_channel_cfg_t *channel_cfg,
                                              void *buffer_ptr);

/**
 \~english
 @brief adc channel configuration
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] channel_cfgs_ptr: sampling channel configuration array
 @param[in] channel_cfgs_ptr: the length of sampling channel configuration array
 @param[in] buffer_ptr: data buffer
 @return vsf_err_t: VSF_ERR_NONE if the adc request was successfully, or a negative error code

 \~chinese
 @brief adc通道配置
 @param[in] adc_ptr: 结构体 vsf_adc_t 的指针，参考 @ref vsf_adc_t
 @param[in] channel_cfgs_ptr: 采样通道配置数组
 @param[in] channel_cfgs_cnt: 采样通道配置数组的长度
 @param[in] buffer_ptr: 数据缓冲区
 @return vsf_err_t: 如果 adc 请求成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_adc_channel_config(vsf_adc_t *adc_ptr,
                                        vsf_adc_channel_cfg_t *channel_cfgs_ptr,
                                        uint32_t channel_cfgs_cnt);

/**
 \~english
 @brief adc channel request
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[out] buffer_ptr: sampling channel data array
 @param[in] count: the length of sampling channel configuration data array
 @return vsf_err_t: VSF_ERR_NONE if the adc request was successfully, or a negative error code

 \~chinese
 @brief adc通道配置
 @param[in] adc_ptr: 结构体 vsf_adc_t 的指针，参考 @ref vsf_adc_t
 @param[out] buffer_ptr: 采样通道配置数据数组
 @param[in] count: 采样通道配置数据数组的长度
 @return vsf_err_t: 如果 adc 请求成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_adc_channel_request(vsf_adc_t *adc_ptr,
                                         void *buffer_ptr,
                                         uint_fast32_t count);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_ADC_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_adc_t                              VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_t)
#   define vsf_adc_init(__ADC, ...)                 VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_init)                 ((__vsf_adc_t *)__ADC, ##__VA_ARGS__)
#   define vsf_adc_fini(__ADC)                      VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_fini)                 ((__vsf_adc_t *)__ADC)
#   define vsf_adc_enable(__ADC)                    VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_enable)               ((__vsf_adc_t *)__ADC)
#   define vsf_adc_disable(__ADC)                   VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_disable)              ((__vsf_adc_t *)__ADC)
#   define vsf_adc_status(__ADC)                    VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_status)               ((__vsf_adc_t *)__ADC)
#   define vsf_adc_capability(__ADC)                VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_capability)           ((__vsf_adc_t *)__ADC)
#   define vsf_adc_irq_enable(__ADC, ...)           VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_irq_enable)           ((__vsf_adc_t *)__ADC, ##__VA_ARGS__)
#   define vsf_adc_irq_disable(__ADC, ...)          VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_irq_disable)          ((__vsf_adc_t *)__ADC, ##__VA_ARGS__)
#   define vsf_adc_channel_request_once(__ADC, ...) VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_channel_request_once) ((__vsf_adc_t *)__ADC, ##__VA_ARGS__)
#   define vsf_adc_channel_config(__ADC, ...)       VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_channel_config)       ((__vsf_adc_t *)__ADC, ##__VA_ARGS__)
#   define vsf_adc_channel_request(__ADC, ...)      VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_channel_request)      ((__vsf_adc_t *)__ADC, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_ADC_H__*/
