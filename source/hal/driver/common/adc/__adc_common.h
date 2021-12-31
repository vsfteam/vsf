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

#ifndef __HAL_DRIVER_COMMON_ADC_H__
#define __HAL_DRIVER_COMMON_ADC_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#ifndef VSF_HAL_ADC_REQUEST_CFG_ISR
#   define VSF_HAL_ADC_REQUEST_CFG_ISR                      cfg.isr
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_HAL_ADC_IMP_REQUEST_MULTI == ENABLED
#   define vsf_hal_adc_def_req_by_once()                                        \
        __vsf_adc_req_by_once_t     __req_by_once
#else
#   define vsf_hal_adc_def_req_by_once()
#endif

/*============================ TYPES =========================================*/

#if VSF_HAL_ADC_IMP_REQUEST_MULTI == ENABLED
typedef struct __vsf_adc_req_by_once_t {
    struct {
        uint_fast32_t   max_count;
        uint_fast32_t   count;
        void            *buffer;
    } info;
    vsf_adc_isr_t       isr;
} __vsf_adc_req_by_once_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_HAL_ADC_MAX_CNT >= 0 && VSF_HAL_USE_DAC0 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 0))
extern const i_adc_t VSF_ADC0;
extern vsf_adc_t vsf_adc0;
#endif
#if VSF_HAL_ADC_MAX_CNT >= 1 && VSF_HAL_USE_ADC1 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 1))
extern const i_adc_t VSF_ADC1;
extern vsf_adc_t vsf_adc1;
#endif
#if VSF_HAL_ADC_MAX_CNT >= 2 && VSF_HAL_USE_ADC2 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 2))
extern const i_adc_t VSF_ADC2;
extern vsf_adc_t vsf_adc2;
#endif
#if VSF_HAL_ADC_MAX_CNT >= 3 && VSF_HAL_USE_ADC3 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 3))
extern const i_adc_t VSF_ADC3;
extern vsf_adc_t vsf_adc3;
#endif
#if VSF_HAL_ADC_MAX_CNT >= 4 && VSF_HAL_USE_ADC4 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 4))
extern const i_adc_t VSF_ADC4;
extern vsf_adc_t vsf_adc4;
#endif
#if VSF_HAL_ADC_MAX_CNT >= 5 && VSF_HAL_USE_ADC5 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 5))
extern const i_adc_t VSF_ADC5;
extern vsf_adc_t vsf_adc5;
#endif
#if VSF_HAL_ADC_MAX_CNT >= 6 && VSF_HAL_USE_ADC6 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 6))
extern const i_adc_t VSF_ADC6;
extern vsf_adc_t vsf_adc6;
#endif
#if VSF_HAL_ADC_MAX_CNT >= 7 && VSF_HAL_USE_ADC7 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 7))
extern const i_adc_t VSF_ADC7;
extern vsf_adc_t vsf_adc7;
#endif

#endif
