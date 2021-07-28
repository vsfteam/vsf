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

#undef VSF_AIC8800_ADC_COMMON_FALSE
#define VSF_AIC8800_ADC_COMMON_FALSE                                0

#undef VSF_AIC8800_ADC_COMMON_TRUE
#define VSF_AIC8800_ADC_COMMON_TRUE                                 1

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __adc_req_init(vsf_adc_t *adc_ptr, void *buffer_ptr, uint_fast32_t count)
{
    adc_ptr->__req_by_once.info.buffer = buffer_ptr;
    adc_ptr->__req_by_once.info.max_count = count;
    adc_ptr->__req_by_once.info.count = 0;
    adc_ptr->channel_index = 0;
}

static void __adc_request_isr_handler(void *target, vsf_adc_t *adc_ptr)
{
    int i;
    if (adc_ptr->__req_by_once.info.count < adc_ptr->__req_by_once.info.max_count) {
        vsf_adc_channel_request_once(adc_ptr,
            &adc_ptr->cfg_channel[adc_ptr->channel_index],
            (void *)((uint16_t *)adc_ptr->__req_by_once.info.buffer
                + adc_ptr->__req_by_once.info.count));
        if (adc_ptr->channel_index < adc_ptr->channel_count) {
            adc_ptr->channel_index++;
        } else {
            adc_ptr->channel_index = 0;
        }
        adc_ptr->__req_by_once.info.count++;
        return;
    }
    adc_ptr->status.is_busy = VSF_AIC8800_ADC_COMMON_FALSE;
    vsf_adc_irq_disable(adc_ptr);
    vsf_adc_isr_t *isr = &adc_ptr->__req_by_once.isr;
    adc_ptr->data = adc_ptr->__req_by_once.info.buffer;
    if (NULL != isr->handler_fn) {
        isr->handler_fn(isr->target_ptr, adc_ptr);
    }
    vsf_adc_irq_enable(adc_ptr);
}

static void __adc_req_isr_init(vsf_adc_t *adc_ptr)
{
    if (    (adc_ptr->VSF_HAL_ADC_REQUEST_CFG_ISR.handler_fn != NULL)
        &&  (adc_ptr->VSF_HAL_ADC_REQUEST_CFG_ISR.handler_fn != __adc_request_isr_handler)) {
        adc_ptr->__req_by_once.isr = adc_ptr->VSF_HAL_ADC_REQUEST_CFG_ISR;
        adc_ptr->VSF_HAL_ADC_REQUEST_CFG_ISR.handler_fn = __adc_request_isr_handler;
    }
}

vsf_err_t vsf_adc_channel_request(vsf_adc_t *adc_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != buffer_ptr) && (count > 0));
    if (VSF_AIC8800_ADC_COMMON_FALSE == adc_ptr->status.is_enable) {
        return VSF_ERR_NOT_READY;//todo
    }
    if (VSF_AIC8800_ADC_COMMON_FALSE != adc_ptr->status.is_busy) {
        return VSF_ERR_ALREADY_EXISTS;//todo
    }
    __adc_req_init(adc_ptr, buffer_ptr, count);
    __adc_req_isr_init(adc_ptr);
    vsf_protect_t orig = vsf_protect(interrupt)();
    __adc_request_isr_handler(NULL, adc_ptr);
    vsf_unprotect(interrupt)(orig);
}

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