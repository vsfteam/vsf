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
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or DEClied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

// for example:
//  #define VSF_HAL_TEMPLATE_DEC_PREFIX                     vsf_hw
//  #define VSF_HAL_TEMPLATE_DEC_UPCASE_PREFIX              VSF_HW
//  #define VSF_HAL_TEMPLATE_DEC_NAME                       adc
//  #define VSF_HAL_TEMPLATE_DEC_UPCASE_NAME                ADC

#ifndef VSF_HAL_TEMPLATE_DEC_PREFIX
#   define VSF_HAL_TEMPLATE_DEC_PREFIX                      VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_PREFIX)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_OP_PREFIX
#   define VSF_HAL_TEMPLATE_DEC_OP_PREFIX                   VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_OP_PREFIX)
#endif

#ifndef VSF_HAL_CFG_DEC_COUNT_MASK_PREFIX
#   define VSF_HAL_CFG_DEC_COUNT_MASK_PREFIX                VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_COUNT_MASK_PREFIX)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX
#   define VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX                _COUNT
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX
#   define VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX                 _MASK
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_MULTI_CLASS
#   define VSF_HAL_TEMPLATE_DEC_MULTI_CLASS                 VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_EXTERN_OP)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_DEFINED_COUNT
#   define VSF_HAL_TEMPLATE_DEC_DEFINED_COUNT               VSF_MCONNECT(VSF_HAL_CFG_DEC_COUNT_MASK_PREFIX, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_DEFINED_MASK
#   define VSF_HAL_TEMPLATE_DEC_DEFINED_MASK                VSF_MCONNECT(VSF_HAL_CFG_DEC_COUNT_MASK_PREFIX, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_TYPE
#   define VSF_HAL_TEMPLATE_DEC_TYPE                        VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, _t)
#endif

#ifndef VSF_HAL_TEMPLATE_OP_TYPE
#   define VSF_HAL_TEMPLATE_OP_TYPE                         VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_OP_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, _t)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_TYPE
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE_TYPE               VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, _t)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_API
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE_API                VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _APIS)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_INDEX_ARRAY
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE_INDEX_ARRAY        VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, _indexs)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_NAME_ARRAY
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE_NAME_ARRAY         VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, _names)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_DEVICE_ARRAY
#   define VSF_HAL_TEMPLATE_DEC_DEVICE_ARRAY                VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, _devices)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_TYPE
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_TYPE            VSF_MCONNECT(vsf, VSF_HAL_TEMPLATE_DEC_NAME, _op_t)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_INSTANCE
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_INSTANCE        VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, _op)
#endif

#define VSF_HAL_TEMPLATE_DEC_INSTACE(__IDX, __DONT_CARE)    \
    extern VSF_HAL_TEMPLATE_DEC_INSTANCE_TYPE VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, __IDX);

#ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_ARRAY
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE_ARRAY              VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, s)
#endif


#ifndef VSF_HAL_TEMPLATE_DEC_LV0
#   define VSF_HAL_TEMPLATE_DEC_LV0                         VSF_HAL_TEMPLATE_DEC_INSTACE
#endif

// expand to:
//  typedef vsf_xxx_adc_t vsf_xxx_adc_t;
typedef struct VSF_HAL_TEMPLATE_OP_TYPE VSF_HAL_TEMPLATE_DEC_TYPE;

// expand to:
//  extern vsf_err_t vsf_hw_adc_init(vsf_hw_adc_t *adc_ptr, vsf_adc_cfg_t *cfg_ptr);
//  ...
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API                              VSF_HAL_TEMPLATE_API_EXTERN
VSF_HAL_TEMPLATE_DEC_INSTANCE_API(VSF_HAL_TEMPLATE_DEC_PREFIX)

#if VSF_HAL_TEMPLATE_DEC_MULTI_CLASS == ENABLED
extern const VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_TYPE VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_INSTANCE;
#endif

#if !defined(VSF_HAL_TEMPLATE_DEC_COUNT) && VSF_HAL_TEMPLATE_DEC_DEFINED_COUNT
#   define VSF_HAL_TEMPLATE_DEC_COUNT                       VSF_HAL_TEMPLATE_DEC_DEFINED_COUNT
#endif

#if !defined(VSF_HAL_TEMPLATE_DEC_MASK) && VSF_HAL_TEMPLATE_DEC_DEFINED_MASK
#   define VSF_HAL_TEMPLATE_DEC_MASK                        VSF_HAL_TEMPLATE_DEC_DEFINED_MASK
#endif

#if defined(VSF_HAL_TEMPLATE_DEC_MASK) && !defined(VSF_HAL_TEMPLATE_DEC_COUNT)
#   define VSF_HAL_TEMPLATE_DEC_COUNT                       VSF_HAL_MASK_TO_COUNT(VSF_HAL_TEMPLATE_DEC_MASK)
#endif

#if defined(VSF_HAL_TEMPLATE_DEC_COUNT) && !defined(VSF_HAL_TEMPLATE_DEC_MASK)
#   define VSF_HAL_TEMPLATE_DEC_MASK                        VSF_HAL_COUNT_TO_MASK(VSF_HAL_TEMPLATE_DEC_COUNT)
#endif

#define __VSF_HAL_TEMPLATE_MASK                             VSF_HAL_TEMPLATE_DEC_MASK
#define __VSF_HAL_TEMPLATE_MACRO                            VSF_HAL_TEMPLATE_DEC_LV0
#define __VSF_HAL_TEMPLATE_ARG                              VSF_HAL_TEMPLATE_DEC_OP
#include "./vsf_template_instance_mask.h"

#if !defined(VSF_HAL_TEMPLATE_DEC_REMOVE_ARRAY) && (VSF_HAL_TEMPLATE_DEC_MASK != 0)
extern VSF_HAL_TEMPLATE_DEC_INSTANCE_TYPE *const VSF_HAL_TEMPLATE_DEC_INSTANCE_ARRAY[VSF_HAL_TEMPLATE_DEC_COUNT];
extern const uint8_t VSF_HAL_TEMPLATE_DEC_INSTANCE_INDEX_ARRAY[VSF_HAL_TEMPLATE_DEC_COUNT];
extern const char *VSF_HAL_TEMPLATE_DEC_INSTANCE_NAME_ARRAY[VSF_HAL_TEMPLATE_DEC_COUNT];
extern const vsf_hal_device_t VSF_HAL_TEMPLATE_DEC_DEVICE_ARRAY[VSF_HAL_TEMPLATE_DEC_COUNT];
#endif

#undef VSF_HAL_TEMPLATE_DEC_NAME
#undef VSF_HAL_TEMPLATE_DEC_UPCASE_NAME
#undef VSF_HAL_TEMPLATE_DEC_PREFIX
#undef VSF_HAL_TEMPLATE_DEC_UPCASE_PREFIX
#undef VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX
#undef VSF_HAL_TEMPLATE_DEC_MULTI_CLASS
#undef VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX
#undef VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX
#undef VSF_HAL_TEMPLATE_DEC_DEFINED_COUNT
#undef VSF_HAL_TEMPLATE_DEC_DEFINED_MASK
#undef VSF_HAL_TEMPLATE_DEC_COUNT
#undef VSF_HAL_TEMPLATE_DEC_MASK
#undef VSF_HAL_TEMPLATE_DEC_TYPE
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_TYPE
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_API
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_TYPE
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_INSTANCE
#undef VSF_HAL_TEMPLATE_DEC_INSTACE
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_ARRAY
#undef VSF_HAL_TEMPLATE_DEC_LV0
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_INDEX_ARRAY
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_NAME_ARRAY
#undef VSF_HAL_TEMPLATE_DEC_REMOVE_ARRAY

