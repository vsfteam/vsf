/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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
 *****************************************************************************/

#ifndef __VSF_TEST_ADC_H__
#define __VSF_TEST_ADC_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_ADC_ONESHOT_ENABLE
#   define VSF_TEST_ADC_ONESHOT_ENABLE         ENABLED
#endif
#ifndef VSF_TEST_ADC_TEMPERATURE_ENABLE
#   define VSF_TEST_ADC_TEMPERATURE_ENABLE     DISABLED
#endif
#ifndef VSF_TEST_ADC_STREAM_ENABLE
#   define VSF_TEST_ADC_STREAM_ENABLE          ENABLED
#endif

#ifndef VSF_TEST_ADC_ENABLE
#   define VSF_TEST_ADC_ENABLE    ENABLED
#endif

#include "suite/vsf_test_adc_stream.h"

#include "suite/vsf_test_adc_oneshot.h"
#include "suite/vsf_test_adc_temperature.h"
typedef union {
#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
    vsf_test_adc_stream_data_t adc_stream;
#endif
} vsf_test_adc_data_t;
#ifdef __cplusplus
}
#endif
#endif /* __VSF_TEST_ADC_H__ */
/* EOF */