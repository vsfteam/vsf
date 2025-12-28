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

// Here's an example showing the complete configuration pattern:

// 1. In your device.h file, define hardware-specific parameters:
// device.h
// #define VSF_HW_SPI_COUNT                            4       // Total number of SPI instances available
// #define VSF_HW_SPI0_IRQN                            SPI0_IRQn   // Interrupt number for SPI0
// #define VSF_HW_SPI0_IRQHandler                      SPI0_Handler // Handler function for SPI0 interrupts
// ...

// 2. In your implementation file (e.g., spi.c), configure the implementation prefixes:
// spi.c
// #   define VSF_SPI_CFG_IMP_PREFIX                    vsf_hw      // Lowercase prefix for functions and variables
// #   define VSF_SPI_CFG_IMP_UPCASE_PREFIX             VSF_HW      // Uppercase prefix for macros
//
// The following definitions are optional and only needed when dealing with special peripherals
// (e.g., when you need QSPI instead of standard SPI):
// #   define VSF_SPI_CFG_IMP_RENAME_DEVICE_PREFIX      ENABLED     // Enable renaming device prefix
// #   define VSF_SPI_CFG_IMP_DEVICE_PREFIX             vsf_hw_qspi // Custom device prefix when renaming
// #   define VSF_SPI_CFG_IMP_DEVICE_UPCASE_PREFIX      VSF_HW_QSPI // Uppercase version of custom device prefix

// 3. In the template include file (e.g., spi_template.inc), define the component name:
// spi_template.inc
// #define VSF_HAL_TEMPLATE_IMP_NAME                    _spi       // Component name in lowercase with leading underscore
// #define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME             _SPI       // Component name in uppercase with leading underscore

// These configurations work together to create appropriately named instances and functions.
// For example, with the above configuration, you'll get:
// - Instance names like vsf_hw_qspi0, vsf_hw_qspi1, etc.
// - Arrays like vsf_hw_qspi_devices[], vsf_hw_qspi_names[], etc.
// - The template handles all the repetitive implementation details automatically.

/**************************** prefex *******************************************/

// VSF_HAL_TEMPLATE_IMP_RENAME_DEVICE_PREFIX -> VSF_SPI_CFG_IMP_RENAME_DEVICE_PREFIX -> ENABLED or DISABLED
#ifndef VSF_HAL_TEMPLATE_IMP_RENAME_DEVICE_PREFIX
#   define VSF_HAL_TEMPLATE_IMP_RENAME_DEVICE_PREFIX        VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_RENAME_DEVICE_PREFIX)
#endif

// VSF_HAL_TEMPLATE_IMP_UPCASE_PREFIX -> VSF_SPI_CFG_IMP_UPCASE_PREFIX -> VSF_HW
#ifndef VSF_HAL_TEMPLATE_IMP_UPCASE_PREFIX
#   define VSF_HAL_TEMPLATE_IMP_UPCASE_PREFIX               VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_UPCASE_PREFIX)
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX
#   if VSF_HAL_TEMPLATE_IMP_RENAME_DEVICE_PREFIX == ENABLED
#       define VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX           VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_DEVICE_PREFIX)
#   else
#       define __VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX         VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_PREFIX)
#       define VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX           VSF_MCONNECT(__VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX, VSF_HAL_TEMPLATE_IMP_NAME)
#   endif
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_DEVICE_UPCASE_PREFIX
#   if VSF_HAL_TEMPLATE_IMP_RENAME_DEVICE_PREFIX == ENABLED
#       define VSF_HAL_TEMPLATE_IMP_DEVICE_UPCASE_PREFIX    VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_DEVICE_UPCASE_PREFIX)
#   else
#       define __VSF_HAL_TEMPLATE_IMP_DEVICE_UPCASE_PREFIX  VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_UPCASE_PREFIX)
#       define VSF_HAL_TEMPLATE_IMP_DEVICE_UPCASE_PREFIX    VSF_MCONNECT(__VSF_HAL_TEMPLATE_IMP_DEVICE_UPCASE_PREFIX, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME)
#   endif
#endif

// VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX -> VSF_SPI_CFG_IMP_COUNT_MASK_PREFIX -> VSF_HW
#ifndef VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX
#   if VSF_HAL_TEMPLATE_IMP_RENAME_DEVICE_PREFIX == ENABLED
#       define VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX       VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_DEVICE_UPCASE_PREFIX)
#   else
#       define __VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX     VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_COUNT_MASK_PREFIX)
#       define VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX       VSF_MCONNECT(__VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME)
#   endif
#endif

// VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX -> VSF_SPI_CFG_IMP_PREFIX -> vsf_hw
#ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX
#   define VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX             VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX
#endif

/**************************** remap ********************************************/

#ifdef VSF_HAL_CFG_IMP_REMAP_FUNCTIONS
VSF_HAL_CFG_IMP_REMAP_FUNCTIONS
#endif

/**************************** instance op **************************************/

#ifndef VSF_HAL_TEMPLATE_IMP_MULTI_CLASS
// VSF_HAL_TEMPLATE_IMP_MULTI_CLASS -> VSF_HW_SPI_CFG_MULTI_CLASS
#   if VSF_HAL_TEMPLATE_IMP_RENAME_DEVICE_PREFIX == ENABLED
// When using RENAME_DEVICE_PREFIX, use DEVICE_UPCASE_PREFIX directly (e.g., VSF_HW_WWDT_CFG_MULTI_CLASS)
#       define VSF_HAL_TEMPLATE_IMP_MULTI_CLASS             VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_DEVICE_UPCASE_PREFIX, _CFG_MULTI_CLASS)
#   else
// Normal case: VSF_HAL_TEMPLATE_IMP_UPCASE_PREFIX + _WDT + _CFG_MULTI_CLASS (e.g., VSF_HW_WDT_CFG_MULTI_CLASS)
#       define VSF_HAL_TEMPLATE_IMP_MULTI_CLASS             VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_UPCASE_PREFIX, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_MULTI_CLASS)
#   endif
#endif

#if VSF_HAL_TEMPLATE_IMP_MULTI_CLASS == ENABLED
// VSF_HAL_TEMPLATE_IMP_OP -> VSF_HW_SPI_CFG_IMP_HAS_OP
#   ifndef VSF_HAL_TEMPLATE_IMP_HAS_OP
#       define VSF_HAL_TEMPLATE_IMP_HAS_OP                  VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_HAS_OP)
#   endif
#   if VSF_HAL_TEMPLATE_IMP_HAS_OP
// VSF_HAL_TEMPLATE_IMP_OP_VAR -> vsf_hw_spi_op
#       define VSF_HAL_TEMPLATE_IMP_OP_VAR                  VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX, _op)
// VSF_HAL_TEMPLATE_IMP_OP -> .vsf_hw_spi.op = & vsf_hw_spi_op
#       define VSF_HAL_TEMPLATE_IMP_OP                      .VSF_MCONNECT(vsf, VSF_HAL_TEMPLATE_IMP_NAME).op = &VSF_HAL_TEMPLATE_IMP_OP_VAR,
#   else
#       ifndef VSF_HAL_TEMPLATE_IMP_EXTERN_OP
// VSF_HAL_TEMPLATE_IMP_EXTERN_OP -> VSF_SPI_CFG_IMP_EXTERN_OP ->
#           define VSF_HAL_TEMPLATE_IMP_EXTERN_OP           VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_EXTERN_OP)
#       endif
#       if VSF_HAL_TEMPLATE_IMP_EXTERN_OP == DISABLED
// VSF_HAL_TEMPLATE_IMP_OP_VAR -> __vsf_hw_spi_op
#           define VSF_HAL_TEMPLATE_IMP_OP_VAR              VSF_MCONNECT(__, VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX, _op)
#           define VSF_HAL_TEMPLATE_IMP_OP_ATR              static
#       else
// VSF_HAL_TEMPLATE_IMP_OP_VAR -> vsf_hw_spi_op
#           define VSF_HAL_TEMPLATE_IMP_OP_VAR              VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX, _op)
#           define VSF_HAL_TEMPLATE_IMP_OP_ATR
#       endif
// VSF_HAL_TEMPLATE_IMP_OP -> .vsf_hw_spi.op = & VSF_HAL_TEMPLATE_IMP_OP_VAR
#       define VSF_HAL_TEMPLATE_IMP_OP                      .VSF_MCONNECT(vsf, VSF_HAL_TEMPLATE_IMP_NAME).op = &VSF_HAL_TEMPLATE_IMP_OP_VAR,

#       undef __VSF_HAL_TEMPLATE_API
#       define __VSF_HAL_TEMPLATE_API                       VSF_HAL_TEMPLATE_API_OP

#ifndef VSF_HAL_TEMPLATE_IMP_OP_TYPE
// VSF_HAL_TEMPLATE_IMP_OP_TYPE -> vsf_spi_op_t
#   define VSF_HAL_TEMPLATE_IMP_OP_TYPE                     VSF_MCONNECT(vsf, VSF_HAL_TEMPLATE_IMP_NAME, _op_t)
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_OP_MACRO
// VSF_HAL_TEMPLATE_IMP_OP_MACRO -> VSF_SPI_APIS
#   define VSF_HAL_TEMPLATE_IMP_OP_MACRO                    VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _APIS)
#endif

/*
static const vsf_spi_op_t __vsf_hw_spi_op = {
    .capability =
        (vsf_spi_capability_t(*)(vsf_spi_t *)) & vsf_hw_spi_capability,
    .port_config_pins = (vsf_err_t(*)(vsf_spi_t *, uint32_t pin_mask,
                                      vsf_spi_cfg_t *cfg_ptr)) &
                        vsf_hw_spi_port_config_pins,
    ...
};
 */
VSF_HAL_TEMPLATE_IMP_OP_ATR const VSF_HAL_TEMPLATE_IMP_OP_TYPE VSF_HAL_TEMPLATE_IMP_OP_VAR = {
    VSF_HAL_TEMPLATE_IMP_OP_MACRO(VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX)
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
// VSF_HAL_TEMPLATE_IMP_DEFINED_COUNT -> VSF_HW_SPI_COUNT -> Number(for example, 2, vsf_hw_spi0 and vsf_hw_spi1)
#   define VSF_HAL_TEMPLATE_IMP_DEFINED_COUNT               VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX, VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX)
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_DEFINED_MASK
// VSF_HAL_TEMPLATE_IMP_DEFINED_MASK -> VSF_HW_SPI_MASK -> Number (for example, 0x05, vsf_hw_spi0 and vsf_hw_spi2)
#   define VSF_HAL_TEMPLATE_IMP_DEFINED_MASK                VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX, VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX)
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
#       define VSF_HAL_TEMPLATE_IMP_MASK                    VSF_HAL_COUNT_TO_MASK(VSF_HAL_TEMPLATE_IMP_COUNT)
#   endif

/**************************** instance and array **************************************/

/*
vsf_hw_spi_t vsf_hw_spi0 = {
    .vsf_spi.op   = &__vsf_hw_spi_op,
    ...
};
vsf_hw_spi_t vsf_hw_spi1 = {
    .vsf_spi.op   = &__vsf_hw_spi_op,
    ...
};
 */
#   define __VSF_HAL_TEMPLATE_MASK                          VSF_HAL_TEMPLATE_IMP_MASK
#   define __VSF_HAL_TEMPLATE_MACRO                         VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_IMP_UPCASE_NAME, _CFG_IMP_LV0)
#   define __VSF_HAL_TEMPLATE_ARG                           VSF_HAL_TEMPLATE_IMP_OP
#   include "./vsf_template_instance_mask.h"

/**************************** instance's array *********************************/

/*
vsf_hw_spi_t *const vsf_hw_spis[2] = {
    &vsf_hw_spi0,
    &vsf_hw_spi1,
};
 */
#   if !defined(VSF_HAL_TEMPLATE_IMP_REMOVE_ARRAY)
#       ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY
#           define VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY      VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX, s)
#       endif
#       ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_TYPE
#           define VSF_HAL_TEMPLATE_IMP_INSTANCE_TYPE       VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX, _t)
#       endif
VSF_HAL_TEMPLATE_IMP_INSTANCE_TYPE * const VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY[VSF_HAL_TEMPLATE_IMP_COUNT] = {
#       ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY_ITEM
#           define VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY_ITEM(__INDEX, __NULL)    \
                                                            &VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX, __INDEX),
#       endif

#       define __VSF_HAL_TEMPLATE_MASK                      VSF_HAL_TEMPLATE_IMP_MASK
#       define __VSF_HAL_TEMPLATE_MACRO                     VSF_HAL_TEMPLATE_IMP_INSTANCE_ARRAY_ITEM
#       define __VSF_HAL_TEMPLATE_ARG                       VSF_HAL_TEMPLATE_IMP_OP
#       include "./vsf_template_instance_mask.h"
};

/*
const uint8_t vsf_hw_spi_indexs[2] = {
    0,
    1,
};
 */
#ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_INDEX_ARRAY
#   define VSF_HAL_TEMPLATE_IMP_INSTANCE_INDEX_ARRAY        VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX, _indexs)
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
const char *vsf_hw_spi_names[2] = {
    "vsf_hw_spi" "0",
    "vsf_hw_spi" "1",
};
 */
#ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_NAME_ARRAY
#   define VSF_HAL_TEMPLATE_IMP_INSTANCE_NAME_ARRAY         VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX, _names)
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
#       define __VSF_HAL_TEMPLATE_ARG                       VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX
#       include "./vsf_template_instance_mask.h"
};

/*
const vsf_hal_device_t vsf_hw_spi_devices[2] = {
    {
        .pointer = &vsf_hw_spi0,
        .name    = "vsf_hw_spi" "0",
    },
    {
        .pointer = &vsf_hw_spi1,
        .name    = "vsf_hw_spi" "1",
    },
};
 */
#ifndef VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY
#   define VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY       VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX, _devices)
#endif
const vsf_hal_device_t VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY[VSF_HAL_TEMPLATE_IMP_COUNT] = {
#       define __VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY_ITEM(__INDEX, __ARG)\
                                                            { .pointer = &VSF_MCONNECT(__ARG, __INDEX), .name = # __ARG #__INDEX, },
#       define VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY_ITEM(__INDEX, __ARG)  \
                                                            __VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY_ITEM(__INDEX, __ARG)
#       define __VSF_HAL_TEMPLATE_MASK                      VSF_HAL_TEMPLATE_IMP_MASK
#       define __VSF_HAL_TEMPLATE_MACRO                     VSF_HAL_TEMPLATE_IMP_INSTANCE_DEVICE_ARRAY_ITEM
#       define __VSF_HAL_TEMPLATE_ARG                       VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX)
#       include "./vsf_template_instance_mask.h"
};
#   endif
#endif

/**************************** undef ********************************************/

#undef __VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX
#undef VSF_HAL_TEMPLATE_IMP_DEVICE_PREFIX
#undef __VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX
#undef VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX

#undef VSF_HAL_TEMPLATE_IMP_RENAME_LV0_PREFIX
#undef VSF_HAL_TEMPLATE_IMP_RENAME_DEVICE_PREFIX
#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME
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
#undef VSF_HAL_TEMPLATE_IMP_OP_ATR
