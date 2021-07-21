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

#if VSF_HAL_ADC_IMP_REQUEST_MULTI == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#endif

#if VSF_HAL_ADC_MAX_CNT >= 0 && VSF_HAL_USE_DAC0 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 0))
__vsf_hw_adc_imp_lv0(0, NULL)
#endif
#if VSF_HAL_ADC_MAX_CNT >= 1 && VSF_HAL_USE_ADC1 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 1))
__vsf_hw_adc_imp_lv0(1, NULL)
#endif
#if VSF_HAL_ADC_MAX_CNT >= 2 && VSF_HAL_USE_ADC2 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 2))
__vsf_hw_adc_imp_lv0(2, NULL)
#endif
#if VSF_HAL_ADC_MAX_CNT >= 3 && VSF_HAL_USE_ADC3 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 3))
__vsf_hw_adc_imp_lv0(3, NULL)
#endif
#if VSF_HAL_ADC_MAX_CNT >= 4 && VSF_HAL_USE_ADC4 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 4))
__vsf_hw_adc_imp_lv0(4, NULL)
#endif
#if VSF_HAL_ADC_MAX_CNT >= 5 && VSF_HAL_USE_ADC5 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 5))
__vsf_hw_adc_imp_lv0(5, NULL)
#endif
#if VSF_HAL_ADC_MAX_CNT >= 6 && VSF_HAL_USE_ADC6 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 6))
__vsf_hw_adc_imp_lv0(6, NULL)
#endif
#if VSF_HAL_ADC_MAX_CNT >= 7 && VSF_HAL_USE_ADC7 == ENABLED && (VSF_HAL_ADC_MASK & (1 << 7))
__vsf_hw_adc_imp_lv0(7, NULL)
#endif

/*EOF*/