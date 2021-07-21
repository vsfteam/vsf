/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_adc_t vsf_adc_t;

typedef void vsf_adc_isr_handler_t(void *target_ptr,
                                   vsf_adc_t *ad_ptr);

typedef struct vsf_adc_isr_t {
    vsf_adc_isr_handler_t  *handler_fn;
    void                   *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_adc_isr_t;

/*
//! \name ad feature
//! @{
enum ad_feature_t{
    DATA_ALIGN_RIGHT        = (0 << 0),  //!< ADC data alignment to right
    DATA_ALIGN_LEFT         = (1 << 0),  //!< ADC data alignment to left

    SCAN_CONV_SINGLE_MODE   = (0 << 1),  //!< Conversion is performed in single mode
    SCAN_CONV_SEQUENCE_MODE = (1 << 1),  //!< Conversions are performed in sequence mode

    //! Selects the external event used to trigger the conversion
    EXTERN_TRIGGER_0        = (0 << 2),
    EXTERN_TRIGGER_1        = (1 << 2),
    EXTERN_TRIGGER_2        = (2 << 2),
};
//! @}
*/

//! \name ad configuration
//! @{
typedef struct adc_cfg_t adc_cfg_t;
struct adc_cfg_t {
    uint32_t      feature;
    uint32_t      clock;
    uint32_t      resolution;
    vsf_adc_isr_t isr;
};
//! @}

/*
//! \name ad channel feature
//! @{
enum ad_channel_feature_t{
    ADC_GAIN_1_6        = (0 << 0),
    ADC_GAIN_1_5        = (1 << 0),
    ADC_GAIN_1_4        = (2 << 0),
    ADC_GAIN_1_3        = (3 << 0),
    ADC_GAIN_1_2        = (4 << 0),
    ADC_GAIN_1          = (5 << 0),

    ADC_REF_VDD_1       = (0 << 4),
    ADC_REF_VDD_1_2     = (1 << 4),
    ADC_REF_VDD_1_3     = (2 << 4),
    ADC_REF_VDD_1_4     = (3 << 4),
};
//! @}
*/

//! \name ad channel configuration
//! @{
typedef struct adc_channel_cfg_t adc_channel_cfg_t;
struct adc_channel_cfg_t {
    uint32_t channel;       // Channel Index
    uint32_t feature;       // Channle Feature
    uint32_t rank;
    uint32_t sample_time;
};
//! @}

//! \name class: ad_t
//! @{
def_interface(i_adc_t)
    implement(i_peripheral_t);

    vsf_err_t (*Init)(adc_cfg_t *pCfg);

    //! Irq
    struct {
        void (*Enable)(void);
        void (*Disable)(void);
    } Irq;

    //! Channel
    struct {
        vsf_err_t (*Config)(adc_channel_cfg_t *ptChannelCfgs, uint32_t nCount);
        vsf_err_t (*Request)(void *pBuffer, uint32_t nCount);
    } Channel;

end_def_interface(i_adc_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_adc_init(vsf_adc_t *adc_ptr, adc_cfg_t *cfg_ptr);

extern vsf_err_t vsf_adc_enable(vsf_adc_t *adc_ptr);
extern vsf_err_t vsf_adc_disable(vsf_adc_t *adc_ptr);

extern void vsf_adc_irq_enable(vsf_adc_t *adc_ptr);
extern void vsf_adc_irq_disable(vsf_adc_t *adc_ptr);

extern vsf_err_t vsf_adc_channel_config(vsf_adc_t *adc_ptr,
                                        adc_channel_cfg_t *channel_cfgs_ptr,
                                        uint32_t channel_cfgs_cnt);

extern vsf_err_t vsf_adc_channel_request_one(vsf_adc_t *adc_ptr,
                                             void *buffer_ptr);

extern vsf_err_t vsf_adc_channel_request(vsf_adc_t *adc_ptr,
                                         void *buffer_ptr,
                                         uint_fast32_t count);


#ifdef __cplusplus
}
#endif

#endif
