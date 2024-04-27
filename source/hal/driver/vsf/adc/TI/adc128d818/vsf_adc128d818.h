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

#ifndef __HAL_ADC128D818_ADC_H__
#define __HAL_ADC128D818_ADC_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_ADC == ENABLED && VSF_HAL_USE_ADC128D818_ADC == ENABLED

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Includes CAN ONLY be put here. */
/*\note If current header is for a peripheral driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

// IPCore
#if     defined(__VSF_HAL_ADC128D818_ADC_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_ADC128D818_ADC_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_I2C != ENABLED
#   error ADC128D818 uses I2C, please enable VSF_HAL_USE_I2C
#endif

/*\note VSF_ADC128D818_ADC_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_ADC_CFG_MULTI_CLASS should be in adc.c.
 */

// IPCore
#ifndef VSF_ADC128D818_ADC_CFG_MULTI_CLASS
#   define VSF_ADC128D818_ADC_CFG_MULTI_CLASS   VSF_ADC_CFG_MULTI_CLASS
#endif
// IPCore end

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_ADC128D818_ADC_CFG_MULTI_CLASS == ENABLED
#   define __VSF_ADC128D818_ADC_HALOP       .vsf_adc.op = &vsf_adc128d818_adc_op,
#else
#   define __VSF_ADC128D818_ADC_HALOP
#endif

#define __VSF_ADC128D818_ADC_INIT(__I2C, __I2C_ADDR)                            \
            __VSF_ADC128D818_ADC_HALOP                                          \
            .i2c                = (__I2C),                                      \
            .i2c_addr           = (__I2C_ADDR),
#define VSF_ADC128D818_ADC_INIT(__I2C, __I2C_ADDR)                              \
            __VSF_ADC128D818_ADC_INIT((__I2C), (__I2C_ADDR))

#define __describe_adc128d818_adc(__name, __i2c, __i2c_addr)                    \
            vsf_adc128d818_adc_t __name = {                                     \
                __VSF_ADC128D818_ADC_INIT((__i2c), (__i2c_addr))                \
            };

#define describe_adc128d818_adc(__name, __i2c, __i2c_addr)                      \
            __describe_adc128d818_adc(__name, (__i2c), (__i2c_addr))

/*============================ TYPES =========================================*/

// IPCore
vsf_class(vsf_adc128d818_adc_t) {
    public_member(
#if VSF_ADC128D818_ADC_CFG_MULTI_CLASS == ENABLED
        vsf_adc_t               vsf_adc;
#endif
        // TODO: support INT
        vsf_i2c_t               *i2c;
        uint8_t                 i2c_addr;
    )

/*\note You can add more member in vsf_adc128d818_adc_t instance.
 *      For members accessible from child, put in protected_member.
 *      Else, put in private_member.
 */

    protected_member(
        vsf_adc_isr_t           isr;
        vsf_adc_irq_mask_t      irq_mask;
        uint16_t                *result_buffer;
        uint32_t                total_count;
        uint8_t                 is_continuous_mode : 1;
        uint8_t                 is_inited : 1;
        uint8_t                 is_busy : 1;
        uint8_t                 rx_byte_len : 2;
        uint8_t                 channels_configured;
        uint8_t                 channels_map[8];
        uint8_t                 cur_channels;
        uint8_t                 cur_idx;
        struct {
            uint8_t             cur_reg;
            union {
                uint8_t         data_buffer[2];
                uint16_t        data;
            };
        } VSF_CAL_PACKED;
    )
};
// IPCore end

/*============================ INCLUDES ======================================*/

#define VSF_ADC_CFG_DEC_PREFIX              vsf_adc128d818
#define VSF_ADC_CFG_DEC_UPCASE_PREFIX       VSF_ADC128D818
#define VSF_ADC_CFG_DEC_EXTERN_OP           ENABLED
#include "hal/driver/common/adc/adc_template.h"

#ifdef __cplusplus
}
#endif

// IPCore
#undef __VSF_HAL_ADC128D818_ADC_CLASS_IMPLEMENT
#undef __VSF_HAL_ADC128D818_ADC_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_ADC
#endif      // __HAL_ADC128D818_ADC_H__
/* EOF */
