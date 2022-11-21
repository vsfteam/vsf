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

#ifndef __HAL_DRIVER_AD_INTERFACE_H__
#define __HAL_DRIVER_AD_INTERFACE_H__

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

#ifndef VSF_ADC_CFG_REIMPLEMENT_TYPE_FEATURE
#   define VSF_ADC_CFG_REIMPLEMENT_TYPE_FEATURE         DISABLED
#endif

#ifndef VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_FEATURE
#   define VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_FEATURE DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_ADC_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        adc, init,                 VSF_MCONNECT(__prefix_name, _adc_t) *adc_ptr, vsf_adc_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,         adc, enable,               VSF_MCONNECT(__prefix_name, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,         adc, disable,              VSF_MCONNECT(__prefix_name, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_adc_capability_t, adc, capability,       VSF_MCONNECT(__prefix_name, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             adc, irq_enable,           VSF_MCONNECT(__prefix_name, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             adc, irq_disable,          VSF_MCONNECT(__prefix_name, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        adc, channel_request_once, VSF_MCONNECT(__prefix_name, _adc_t) *adc_ptr, vsf_adc_channel_cfg_t *channel_cfg, void *buffer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        adc, channel_config,       VSF_MCONNECT(__prefix_name, _adc_t) *adc_ptr, vsf_adc_channel_cfg_t *channel_cfgs_ptr, uint32_t channel_cfgs_cnt) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        adc, channel_request,      VSF_MCONNECT(__prefix_name, _adc_t) *adc_ptr, void *buffer_ptr, uint_fast32_t count)

/*============================ TYPES =========================================*/

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_FEATURE == DISABLED
typedef enum vsf_adc_feature_t {
    DATA_ALIGN_RIGHT        = (0 << 0),  //!< ADC data alignment to right
    DATA_ALIGN_LEFT         = (1 << 0),  //!< ADC data alignment to left

    SCAN_CONV_SINGLE_MODE   = (0 << 1),  //!< Conversion is performed in single mode
    SCAN_CONV_SEQUENCE_MODE = (1 << 1),  //!< Conversions are performed in sequence mode

    //! Selects the external event used to trigger the conversion
    EXTERN_TRIGGER_0        = (0 << 2),
    EXTERN_TRIGGER_1        = (1 << 2),
    EXTERN_TRIGGER_2        = (2 << 2),

    ADC_REF_VDD_1           = (0 << 4),
    ADC_REF_VDD_1_2         = (1 << 4),
    ADC_REF_VDD_1_3         = (2 << 4),
    ADC_REF_VDD_1_4         = (3 << 4),
} vsf_adc_feature_t;
#endif

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_FEATURE == DISABLED
typedef enum vsf_adc_channel_feature_t{
    ADC_CHANNEL_GAIN_1_6    = (0 << 0),
    ADC_CHANNEL_GAIN_1_5    = (1 << 0),
    ADC_CHANNEL_GAIN_1_4    = (2 << 0),
    ADC_CHANNEL_GAIN_1_3    = (3 << 0),
    ADC_CHANNEL_GAIN_1_2    = (4 << 0),
    ADC_CHANNEL_GAIN_1      = (5 << 0),

    ADC_CHANNEL_REF_VDD_1   = (0 << 4),
    ADC_CHANNEL_REF_VDD_1_2 = (1 << 4),
    ADC_CHANNEL_REF_VDD_1_3 = (2 << 4),
    ADC_CHANNEL_REF_VDD_1_4 = (3 << 4),
} vsf_adc_channel_feature_t;
#endif

//! adc channel configuration
typedef struct vsf_adc_channel_cfg_t {
    vsf_adc_feature_t feature;  // Channle Feature
    uint16_t sample_cycles;
    uint8_t  channel;       // Channel Index
} vsf_adc_channel_cfg_t;

typedef struct vsf_adc_t vsf_adc_t;

typedef void vsf_adc_isr_handler_t(void *target_ptr, vsf_adc_t *adc_ptr);

typedef struct vsf_adc_isr_t {
    vsf_adc_isr_handler_t  *handler_fn;
    void                   *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_adc_isr_t;

//! adc configuration
typedef struct vsf_adc_cfg_t {
    vsf_adc_feature_t feature;
    uint32_t clock_freq;
    vsf_adc_isr_t isr;
} vsf_adc_cfg_t;

typedef struct vsf_adc_capability_t {
    inherit(vsf_peripheral_capability_t)
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

extern vsf_err_t vsf_adc_init(vsf_adc_t *adc_ptr, vsf_adc_cfg_t *cfg_ptr);

extern fsm_rt_t vsf_adc_enable(vsf_adc_t *adc_ptr);
extern fsm_rt_t vsf_adc_disable(vsf_adc_t *adc_ptr);

extern void vsf_adc_irq_enable(vsf_adc_t *adc_ptr);
extern void vsf_adc_irq_disable(vsf_adc_t *adc_ptr);

extern vsf_err_t vsf_adc_channel_request_once(vsf_adc_t *adc_ptr,
                                              vsf_adc_channel_cfg_t *channel_cfg,
                                              void *buffer_ptr);

extern vsf_err_t vsf_adc_channel_config(vsf_adc_t *adc_ptr,
                                        vsf_adc_channel_cfg_t *channel_cfgs_ptr,
                                        uint32_t channel_cfgs_cnt);

extern vsf_err_t vsf_adc_channel_request(vsf_adc_t *adc_ptr,
                                         void *buffer_ptr,
                                         uint_fast32_t count);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_ADC_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_adc_t                              VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_t)
#   define vsf_adc_init(__ADC, ...)                 VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_init)                 ((__vsf_adc_t *)__ADC, ##__VA_ARGS__)
#   define vsf_adc_enable(__ADC)                    VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_enable)               ((__vsf_adc_t *)__ADC)
#   define vsf_adc_disable(__ADC)                   VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_disable)              ((__vsf_adc_t *)__ADC)
#   define vsf_adc_capability(__ADC)                VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_capability)           ((__vsf_adc_t *)__ADC)
#   define vsf_adc_irq_enable(__ADC)                VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_irq_enable)           ((__vsf_adc_t *)__ADC)
#   define vsf_adc_irq_disable(__ADC)               VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_irq_disable)          ((__vsf_adc_t *)__ADC)
#   define vsf_adc_channel_request_once(__ADC, ...) VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_channel_request_once) ((__vsf_adc_t *)__ADC, ##__VA_ARGS__)
#   define vsf_adc_channel_config(__ADC, ...)       VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_channel_config)       ((__vsf_adc_t *)__ADC, ##__VA_ARGS__)
#   define vsf_adc_channel_request(__ADC, ...)      VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_channel_request)      ((__vsf_adc_t *)__ADC, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_ADC_INTERFACE_H__*/
