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

#ifndef __HAL_ADS7830_ADC_H__
#define __HAL_ADS7830_ADC_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_ADC == ENABLED && VSF_HAL_USE_ADS7830_ADC == ENABLED

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Includes CAN ONLY be put here. */
/*\note If current header is for a peripheral driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

// IPCore
#if     defined(__VSF_HAL_ADS7830_ADC_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_ADS7830_ADC_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_I2C != ENABLED
#   error ADS7830 uses I2C, please enable VSF_HAL_USE_I2C
#endif

/*\note VSF_ADS7830_ADC_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_ADC_CFG_MULTI_CLASS should be in adc.c.
 */

// IPCore
#ifndef VSF_ADS7830_ADC_CFG_MULTI_CLASS
#   define VSF_ADS7830_ADC_CFG_MULTI_CLASS  VSF_ADC_CFG_MULTI_CLASS
#endif
// IPCore end

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_ADS7830_ADC_CFG_MULTI_CLASS == ENABLED
#   define __VSF_ADS7830_ADC_HALOP          .vsf_adc.op = &vsf_ads7830_adc_op,
#else
#   define __VSF_ADS7830_ADC_HALOP
#endif

#define __VSF_ADS7830_ADC_INIT(__I2C, __I2C_ADDR)                               \
            __VSF_ADS7830_ADC_HALOP                                             \
            .i2c                = (__I2C),                                      \
            .i2c_addr           = (__I2C_ADDR),
#define VSF_ADS7830_ADC_INIT(__I2C, __I2C_ADDR)                                 \
            __VSF_ADS7830_ADC_INIT((__I2C), (__I2C_ADDR))

#define __describe_ads7830_adc(__name, __i2c, __i2c_addr)                       \
            vsf_ads7830_adc_t __name = {                                        \
                __VSF_ADS7830_ADC_INIT((__i2c), (__i2c_addr))                   \
            };

#define describe_ads7830_adc(__name, __i2c, __i2c_addr)                         \
            __describe_ads7830_adc(__name, (__i2c), (__i2c_addr))

/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_ads7830_adc_t) {
    public_member(
#if VSF_ADS7830_ADC_CFG_MULTI_CLASS == ENABLED
        vsf_adc_t               vsf_adc;
#endif
        vsf_i2c_t               *i2c;
        uint8_t                 i2c_addr;
    )

/*\note You can add more member in vsf_ads7830_adc_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_adc_isr_t           isr;
        vsf_adc_irq_mask_t      irq_mask;
        uint32_t                total_count;
        uint8_t                 *result_buffer;
        uint8_t                 is_continuous_mode : 1;
        uint8_t                 is_inited : 1;
        uint8_t                 is_busy : 1;
        uint8_t                 is_cmd : 1;
        uint8_t                 cur_channel_seq_idx;
        uint8_t                 tmp;
        uint8_t                 channel_seq_num;
        uint8_t                 channel_seq_map[8];
    )
};
// IPCore end

/*============================ INCLUDES ======================================*/

#define VSF_ADC_CFG_DEC_PREFIX              vsf_ads7830
#define VSF_ADC_CFG_DEC_UPCASE_PREFIX       VSF_ADS7830
#define VSF_ADC_CFG_DEC_EXTERN_OP           ENABLED
#include "hal/driver/common/adc/adc_template.h"

#ifdef __cplusplus
}
#endif

// IPCore
#undef __VSF_HAL_ADS7830_ADC_CLASS_IMPLEMENT
#undef __VSF_HAL_ADS7830_ADC_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_ADC
#endif      // __HAL_ADS7830_ADC_H__
/* EOF */
