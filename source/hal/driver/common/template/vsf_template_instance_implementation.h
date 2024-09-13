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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/


/**************************** prefex *******************************************/

#ifndef VSF_HAL_TEMPLATE_IMP_PREFIX
#   define VSF_HAL_TEMPLATE_IMP_PREFIX                      VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_PREFIX)
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_UPCASE_PREFIX
#   define VSF_HAL_TEMPLATE_IMP_UPCASE_PREFIX               VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_UPCASE_PREFIX)
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX
#   define VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX           VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_COUNT_MASK_PREFIX)
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX
#   define VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX             VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_PREFIX)
#endif

/**************************** remap ********************************************/

#ifdef VSF_HAL_CFG_IMP_REMAP_FUNCTIONS
VSF_HAL_CFG_IMP_REMAP_FUNCTIONS
#endif

/**************************** instance op **************************************/

#ifndef VSF_HAL_TEMPLATE_IMP_MULTI_CLASS
#   define VSF_HAL_TEMPLATE_IMP_MULTI_CLASS                 VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_UPCASE_PREFIX, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_MULTI_CLASS)
#endif

#if VSF_HAL_TEMPLATE_IMP_MULTI_CLASS == ENABLED
#   ifndef VSF_HAL_TEMPLATE_IMP_HAS_OP
#       define VSF_HAL_TEMPLATE_IMP_HAS_OP                  VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_HAS_OP)
#   endif
#   if VSF_HAL_TEMPLATE_IMP_HAS_OP
#       define VSF_HAL_TEMPLATE_IMP_OP_VAR                  VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME, _op)
#       define VSF_HAL_TEMPLATE_IMP_OP                      .VSF_MCONNECT(vsf, VSF_HAL_TEMPLATE_IMP_NAME).op = & VSF_HAL_TEMPLATE_IMP_OP_VAR,
#   else
#       ifndef VSF_HAL_TEMPLATE_IMP_EXTERN_OP
#           define VSF_HAL_TEMPLATE_IMP_EXTERN_OP           VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_EXTERN_OP)
#       endif
#       if VSF_HAL_TEMPLATE_IMP_EXTERN_OP == DISABLED
#           define VSF_HAL_TEMPLATE_IMP_OP_VAR              VSF_MCONNECT(__, VSF_HAL_TEMPLATE_IMP_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME, _op)
#           define VSF_HAL_TEMPLATE_IMP_OP_ATR              static
#       else
#           define VSF_HAL_TEMPLATE_IMP_OP_VAR              VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME, _op)
#           define VSF_HAL_TEMPLATE_IMP_OP_ATR
#       endif
#       define VSF_HAL_TEMPLATE_IMP_OP                      .VSF_MCONNECT(vsf, VSF_HAL_TEMPLATE_IMP_NAME).op = & VSF_HAL_TEMPLATE_IMP_OP_VAR,

#       undef __VSF_HAL_TEMPLATE_API
#       define __VSF_HAL_TEMPLATE_API                       VSF_HAL_TEMPLATE_API_OP

#ifndef VSF_HAL_TEMPLATE_IMP_OP_TYPE
#   define VSF_HAL_TEMPLATE_IMP_OP_TYPE                     VSF_MCONNECT(vsf, VSF_HAL_TEMPLATE_IMP_NAME, _op_t)
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_OP_MACRO
#   define VSF_HAL_TEMPLATE_IMP_OP_MACRO                    VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _APIS)
#endif

/*
static const vsf_gpio_op_t __vsf_hw_gpio_op = {
    .capability =
        (vsf_gpio_capability_t(*)(vsf_gpio_t *)) & vsf_hw_gpio_capability,
    .port_config_pins = (vsf_err_t(*)(vsf_gpio_t *, uint32_t pin_mask,
                                      vsf_gpio_cfg_t *cfg_ptr)) &
                        vsf_hw_gpio_port_config_pins,
    ...
};
 */
VSF_HAL_TEMPLATE_IMP_OP_ATR const VSF_HAL_TEMPLATE_IMP_OP_TYPE VSF_HAL_TEMPLATE_IMP_OP_VAR = {
    VSF_HAL_TEMPLATE_IMP_OP_MACRO(VSF_HAL_TEMPLATE_IMP_PREFIX)
};
#   endif
#else
#   define VSF_HAL_TEMPLATE_IMP_OP
#endif

/**************************** count and mask ***********************************/

#ifndef VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX
#   define VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX                _COUNT
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX
#   define VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX                 _MASK
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_DEFINED_COUNT
#   define VSF_HAL_TEMPLATE_IMP_DEFINED_COUNT               VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX)
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_DEFINED_MASK
#   define VSF_HAL_TEMPLATE_IMP_DEFINED_MASK                VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX)
#endif

#if VSF_HAL_TEMPLATE_IMP_DEFINED_COUNT || VSF_HAL_TEMPLATE_IMP_DEFINED_MASK
#   if !defined(VSF_HAL_TEMPLATE_IMP_COUNT) && VSF_HAL_TEMPLATE_IMP_DEFINED_COUNT
#       define VSF_HAL_TEMPLATE_IMP_COUNT                   VSF_HAL_TEMPLATE_IMP_DEFINED_COUNT
#   endif

#   if !defined(VSF_HAL_TEMPLATE_IMP_MASK) && VSF_HAL_TEMPLATE_IMP_DEFINED_MASK
#       define VSF_HAL_TEMPLATE_IMP_MASK                    VSF_HAL_TEMPLATE_IMP_DEFINED_MASK
#   endif

#   if defined(VSF_HAL_TEMPLATE_IMP_MASK) && !defined(VSF_HAL_TEMPLATE_IMP_COUNT)
#       define VSF_HAL_TEMPLATE_IMP_COUNT                   VSF_HAL_MASK_TO_COUNT(VSF_HAL_TEMPLATE_IMP_MASK)
#   endif

#   if defined(VSF_HAL_TEMPLATE_IMP_COUNT) && !defined(VSF_HAL_TEMPLATE_IMP_MASK)
#       define VSF_HAL_TEMPLATE_IMP_MASK                    ((1ull << VSF_HAL_TEMPLATE_IMP_COUNT) - 1)
#   endif

/**************************** instance and array **************************************/

/*
vsf_hw_gpio_t vsf_hw_gpio0 = {
    .vsf_gpio.op   = &__vsf_hw_gpio_op,
    ...
};
vsf_hw_gpio_t vsf_hw_gpio1 = {
    .vsf_gpio.op   = &__vsf_hw_gpio_op,
    ...
};
 */
#   define __VSF_HAL_TEMPLATE_MASK                          VSF_HAL_TEMPLATE_IMP_MASK
#   define __VSF_HAL_TEMPLATE_MACRO                         VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_LV0)
#   define __VSF_HAL_TEMPLATE_ARG                           VSF_HAL_TEMPLATE_IMP_OP
#   include "./vsf_template_instance_mask.h"

/**************************** instance's array *********************************/

/*
vsf_hw_gpio_t *const vsf_hw_gpios[2] = {
    &vsf_hw_gpio0,
    &vsf_hw_gpio1,
};
 */
#   if !defined(VSF_HAL_TEMPLATE_IMP_REMOVE_ARRAY)
#       ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY
#           define VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY      VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME, s)
#       endif
#       ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_TYPE
#           define VSF_HAL_TEMPLATE_IMP_INSTANCE_TYPE           VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME, _t)
#       endif
VSF_HAL_TEMPLATE_IMP_INSTANCE_TYPE * const VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY[VSF_HAL_TEMPLATE_IMP_COUNT] = {
#       ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY_ITEM
#           define VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY_ITEM(__INDEX, __NULL)    \
                                                            &VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME, __INDEX),
#       endif

#       define __VSF_HAL_TEMPLATE_MASK                      VSF_HAL_TEMPLATE_IMP_MASK
#       define __VSF_HAL_TEMPLATE_MACRO                     VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY_ITEM
#       define __VSF_HAL_TEMPLATE_ARG                       VSF_HAL_TEMPLATE_IMP_OP
#       include "./vsf_template_instance_mask.h"
};

/*
const uint8_t vsf_hw_gpio_indexs[2] = {
    0,
    1,
};
 */
#ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_INDEX_ARRAY
#   define VSF_HAL_TEMPLATE_IMP_INSTANCE_INDEX_ARRAY        VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME, _indexs)
#endif
const uint8_t VSF_HAL_TEMPLATE_IMP_INSTANCE_INDEX_ARRAY[VSF_HAL_TEMPLATE_IMP_COUNT] = {
#       ifndef VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM
#           define VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(__INDEX, __NULL) __INDEX,
#       endif
#       define __VSF_HAL_TEMPLATE_MASK                      VSF_HAL_TEMPLATE_IMP_MASK
#       define __VSF_HAL_TEMPLATE_MACRO                     VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM
#       define __VSF_HAL_TEMPLATE_ARG                       VSF_HAL_TEMPLATE_IMP_OP
#       include "./vsf_template_instance_mask.h"
};

/*
const char *vsf_hw_gpio_names[2] = {
    "vsf_hw_gpio" "0",
    "vsf_hw_gpio" "1",
};
 */
#ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_NAME_ARRAY
#   define VSF_HAL_TEMPLATE_IMP_INSTANCE_NAME_ARRAY         VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME, _names)
#endif
const char* VSF_HAL_TEMPLATE_IMP_INSTANCE_NAME_ARRAY[VSF_HAL_TEMPLATE_IMP_COUNT] = {
#       ifndef VSF_HAL_TEMPLATE_IMP_NAME_ARRAY_ITEM
#           undef __VSF_HAL_TEMPLATE_IMP_NAME_ARRAY_ITEM
#           define __VSF_HAL_TEMPLATE_IMP_NAME_ARRAY_ITEM(__INDEX, __ARG)       \
                                                            # __ARG #__INDEX,
#           define VSF_HAL_TEMPLATE_IMP_NAME_ARRAY_ITEM(__INDEX, __ARG)         \
                                                            __VSF_HAL_TEMPLATE_IMP_NAME_ARRAY_ITEM(__INDEX, __ARG)
#       endif
#       define __VSF_HAL_TEMPLATE_IMP_NAME_ARRAY_ITEM(__INDEX, __ARG)           \
                                                            # __ARG #__INDEX,
#       define VSF_HAL_TEMPLATE_IMP_NAME_ARRAY_ITEM(__INDEX, __ARG)             \
                                                            __VSF_HAL_TEMPLATE_IMP_NAME_ARRAY_ITEM(__INDEX, __ARG)
#       define __VSF_HAL_TEMPLATE_MASK                      VSF_HAL_TEMPLATE_IMP_MASK
#       define __VSF_HAL_TEMPLATE_MACRO                     VSF_HAL_TEMPLATE_IMP_NAME_ARRAY_ITEM
#       define __VSF_HAL_TEMPLATE_ARG                       VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME)
#       include "./vsf_template_instance_mask.h"
};

/*
const vsf_hal_device_t vsf_hw_gpio_devices[2] = {
    {
        .pointer = &vsf_hw_gpio0,
        .name    = "vsf_hw_gpio" "0",
    },
    {
        .pointer = &vsf_hw_gpio1,
        .name    = "vsf_hw_gpio" "1",
    },
};
 */
#ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY
#   define VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY       VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME, _devices)
#endif
const vsf_hal_device_t VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY[VSF_HAL_TEMPLATE_IMP_COUNT] = {
#       define __VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY_ITEM(__INDEX, __ARG)\
                                                            { .pointer = &VSF_MCONNECT(__ARG, __INDEX), .name = # __ARG #__INDEX, },
#       define VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY_ITEM(__INDEX, __ARG)  \
                                                            __VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY_ITEM(__INDEX, __ARG)
#       define __VSF_HAL_TEMPLATE_MASK                      VSF_HAL_TEMPLATE_IMP_MASK
#       define __VSF_HAL_TEMPLATE_MACRO                     VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY_ITEM
#       define __VSF_HAL_TEMPLATE_ARG                       VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME)
#       include "./vsf_template_instance_mask.h"
};
#   endif
#endif

/**************************** undef ********************************************/

#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME
#undef VSF_HAL_TEMPLATE_IMP_PREFIX
#undef VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_PREFIX
#undef VSF_HAL_TEMPLATE_IMP_REMAP_PREFIX
#undef VSF_HAL_TEMPLATE_IMP_INSTANCE_TYPE
#undef VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY
#undef VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY_ITEM
#undef VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM
#undef VSF_HAL_TEMPLATE_IMP_INSTANCE_INDEX_ARRAY
#undef VSF_HAL_TEMPLATE_IMP_INSTANCE_NAME_ARRAY
#undef VSF_HAL_TEMPLATE_IMP_EXTERN_OP
#undef VSF_HAL_TEMPLATE_IMP_COUNT
#undef VSF_HAL_TEMPLATE_IMP_MASK
#undef VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX
#undef VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX
#undef VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX
#undef VSF_HAL_TEMPLATE_IMP_DEFINED_COUNT
#undef VSF_HAL_TEMPLATE_IMP_DEFINED_MASK
#undef VSF_HAL_TEMPLATE_IMP_LV0
#undef VSF_HAL_TEMPLATE_IMP_MULTI_CLASS
#undef VSF_HAL_TEMPLATE_IMP_HAS_OP
#undef VSF_HAL_TEMPLATE_IMP_OP_TYPE
#undef VSF_HAL_TEMPLATE_IMP_OP_MACRO
#undef VSF_HAL_TEMPLATE_IMP_OP_VAR
#undef VSF_HAL_TEMPLATE_IMP_OP
#undef VSF_HAL_TEMPLATE_IMP_REMOVE_ARRAY
