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

/// The following comments are examples showing how to use this template:
//
// Example - In your custom driver declaration file (e.g. driver.h that you need to provide):
// device.h
//      #define VSF_HW_SPI_COUNT                            4
//      #define VSF_HW_SPI0_IRQN                            SPI0_IRQn
//      #define VSF_HW_SPI0_IRQHandler                      SPI0_Handler
// ...
//
// driver.h:
//      #include "hal/driver/common/template/vsf_template_spi.h"  // Include VSF HAL provided template
//      #define VSF_SPI_CFG_DEC_PREFIX                            vsf_hw
//      #define VSF_SPI_CFG_DEC_UPCASE_PREFIX                     VSF_HW
//      #include "hal/driver/common/spi/spi_template.h"           // Include VSF HAL provided template
//
// Note: VSF HAL already provides templates like spi_template.h that define:
//      #define VSF_HAL_TEMPLATE_DEC_NAME                          _spi
//      #define VSF_HAL_TEMPLATE_DEC_UPCASE_NAME                   _SPI
//
// These examples demonstrate the configuration pattern where you provide the driver-specific
// configuration in your declaration file, and then include the VSF HAL provided templates.

/**************************** prefix *******************************************/

// VSF_SPI_CFG_DEC_UPCASE_PREFIX -> VSF_SPI_CFG_DEC_RENAME_DEVICE_PREFIX -> ENABLED or DISABLED
#ifndef VSF_HAL_TEMPLATE_DEC_RENAME_DEVICE_PREFIX
#   define VSF_HAL_TEMPLATE_DEC_RENAME_DEVICE_PREFIX        VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_RENAME_DEVICE_PREFIX)
#endif

// VSF_HAL_TEMPLATE_DEC_PREFIX -> VSF_SPI_CFG_DEC_PREFIX -> vsf_hw (default) or other(for example, vsf_example)
#ifndef VSF_HAL_TEMPLATE_DEC_PREFIX
#   define VSF_HAL_TEMPLATE_DEC_PREFIX                      VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_PREFIX)
#endif

// VSF_HAL_TEMPLATE_DEC_UPCASE_PREFIX -> VSF_SPI_CFG_DEC_UPCASE_PREFIX -> VSF_HW (default) or other(for example, VSF_EXAMPLE)
#ifndef VSF_HAL_TEMPLATE_DEC_UPCASE_PREFIX
#   define VSF_HAL_TEMPLATE_DEC_UPCASE_PREFIX               VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_UPCASE_PREFIX)
#endif


#ifndef VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX
#   if VSF_HAL_TEMPLATE_DEC_RENAME_DEVICE_PREFIX == ENABLED
// VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX -> VSF_SPI_CFG_DEC_DEVICE_PREFIX -> vsf_hw_qspi (for example)
#       define VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX           VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_DEVICE_PREFIX)
#   else
// VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX -> VSF_SPI_CFG_DEC_DEVICE_PREFIX -> vsf_hw_spi (default)
#       define VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX           VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME)
#   endif
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_REMAP
#   define VSF_HAL_TEMPLATE_DEC_REMAP                       VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_REMAP)
#endif

// VSF_HAL_TEMPLATE_DEC_PREFIX -> VSF_SPI_CFG_DEC_REMAP_PREFIX -> vsf_hw(default) or other(for example, vsf_example)
#ifndef VSF_HAL_TEMPLATE_DEC_REMAP_PREFIX
#   define VSF_HAL_TEMPLATE_DEC_REMAP_PREFIX                VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_REMAP_PREFIX)
#endif

// VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX -> VSF_SPI_CFG_DEC_COUNT_MASK_PREFIX -> VSF_HW (default) or other(for example, VSF_EXAMPLE)
#ifndef VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX
#   if VSF_HAL_TEMPLATE_DEC_RENAME_DEVICE_PREFIX == ENABLED
#       define VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX       VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_DEVICE_UPCASE_PREFIX)
#   else
#       define __VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX     VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_COUNT_MASK_PREFIX)
#       define VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX       VSF_MCONNECT(__VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME)
#   endif
#endif

/**************************** remap declaration ********************************/

// VSF_HAL_TEMPLATE_DEC_REMAP_TYPE -> vsf_hw_spi_t or vsf_example_spi_t
#ifndef VSF_HAL_TEMPLATE_DEC_REMAP_TYPE
#   define VSF_HAL_TEMPLATE_DEC_REMAP_TYPE                  VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_REMAP_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, _t)
#endif
// VSF_HAL_TEMPLATE_DEC_TYPE -> vsf_hw_spi_t
#ifndef VSF_HAL_TEMPLATE_DEC_TYPE
#   define VSF_HAL_TEMPLATE_DEC_TYPE                        VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX, _t)
#endif

#if VSF_HAL_TEMPLATE_DEC_REMAP == ENABLED
// expand to one of the following:
//  typedef vsf_remap_example_spi_t vsf_hw_spi_t;
typedef struct VSF_HAL_TEMPLATE_DEC_REMAP_TYPE VSF_HAL_TEMPLATE_DEC_TYPE;
#else
// expand to one of the following:
//  typedef vsf_hw_spi_t vsf_hw_spi_t;
typedef struct VSF_HAL_TEMPLATE_DEC_TYPE VSF_HAL_TEMPLATE_DEC_TYPE;
#endif

/**************************** instance declaration ********************************/

// VSF_HAL_TEMPLATE_DEC_INSTANCE_APIS -> VSF_SPI_APIS (vsf_template_spi.h)
#ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_APIS
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE_APIS               VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _APIS)
#endif

// VSF_SPI_APIS uses __VSF_HAL_TEMPLATE_API
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API                              VSF_HAL_TEMPLATE_API_EXTERN

/*
extern vsf_spi_capability_t vsf_hw_spi_capability(vsf_hw_spi_t *spi_ptr);
extern vsf_err_t vsf_hw_spi_port_config_pins(vsf_hw_spi_t  *spi_ptr,
                                              uint32_t        pin_mask,
                                              vsf_spi_cfg_t *cfg_ptr);
extern void      vsf_hw_spi_set_direction(vsf_hw_spi_t *spi_ptr,
                                           uint32_t       pin_mask,
                                           uint32_t       direction_mask);
...
 */
VSF_HAL_TEMPLATE_DEC_INSTANCE_APIS(VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX)


#ifdef VSF_HAL_TEMPLATE_DEC_ADDITIONAL_API
#   ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_ADDITIONAL_API
#       define VSF_HAL_TEMPLATE_DEC_INSTANCE_ADDITIONAL_API VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _ADDITIONAL_APIS)
#   endif
VSF_HAL_TEMPLATE_DEC_INSTANCE_ADDITIONAL_API(VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX)
#endif

/**************************** multi class op ***********************************/

#ifndef VSF_HAL_TEMPLATE_DEC_EXTERN_OP
// VSF_HAL_TEMPLATE_DEC_EXTERN_OP -> VSF_SPI_CFG_DEC_EXTERN_OP
#   define VSF_HAL_TEMPLATE_DEC_EXTERN_OP                 VSF_MCONNECT(VSF, VSF_HAL_TEMPLATE_DEC_UPCASE_NAME, _CFG_DEC_EXTERN_OP)
#endif

#if VSF_HAL_TEMPLATE_DEC_EXTERN_OP == ENABLED
#   ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_TYPE
// VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_TYPE -> vsf_hw_spi_op_t
#       define VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_TYPE        VSF_MCONNECT(vsf, VSF_HAL_TEMPLATE_DEC_NAME, _op_t)
#   endif
// VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_INSTANCE -> vsf_hw_spi_op
#   ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_INSTANCE
#       define VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_INSTANCE    VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, _op)
#   endif
// extern const vsf_hw_spi_op_t vsf_hw_spi_op;
extern const VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_TYPE VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_INSTANCE;
#endif

/**************************** instance declaration *****************************/

#ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_TYPE
// VSF_HAL_TEMPLATE_DEC_INSTANCE_TYPE -> vsf_hw_spi_t
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE_TYPE               VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX, _t)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_LV0
// VSF_HAL_TEMPLATE_DEC_INSTANCE -> extern vsf_hw_spi_t vsf_hw_spi0;
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE(__IDX, __DONT_CARE)                     \
        extern VSF_HAL_TEMPLATE_DEC_INSTANCE_TYPE VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX, __IDX);
#   define VSF_HAL_TEMPLATE_DEC_LV0                         VSF_HAL_TEMPLATE_DEC_INSTANCE
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX
#   define VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX                _COUNT
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX
#   define VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX                 _MASK
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_DEFINED_COUNT
// VSF_HAL_TEMPLATE_DEC_DEFINED_COUNT -> VSF_HW_SPI_COUNT -> Number (for example, 2, vsf_hw_spi0 and vsf_hw_spi1)
#   define VSF_HAL_TEMPLATE_DEC_DEFINED_COUNT               VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX, VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX)
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_DEFINED_MASK
// VSF_HAL_TEMPLATE_DEC_DEFINED_MASK -> VSF_HW_SPI_MASK -> Number (for example, 0x05, vsf_hw_spi0 and vsf_hw_spi2)
#   define VSF_HAL_TEMPLATE_DEC_DEFINED_MASK                VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX, VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX)
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

/*
extern vsf_hw_spi_t vsf_hw_spi0;
extern vsf_hw_spi_t vsf_hw_spi1;
....
 */
#define __VSF_HAL_TEMPLATE_MASK                             VSF_HAL_TEMPLATE_DEC_MASK
#define __VSF_HAL_TEMPLATE_MACRO                            VSF_HAL_TEMPLATE_DEC_LV0
#define __VSF_HAL_TEMPLATE_ARG                              VSF_HAL_TEMPLATE_DEC_OP
#include "./vsf_template_instance_mask.h"

/**************************** instance's array declaration *********************/

#if !defined(VSF_HAL_TEMPLATE_DEC_REMOVE_ARRAY) && (VSF_HAL_TEMPLATE_DEC_MASK != 0)
#   ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_ARRAY
#       define VSF_HAL_TEMPLATE_DEC_INSTANCE_ARRAY          VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX, s)
#   endif
/*
extern vsf_hw_spi_t *const vsf_hw_spis[2];
 */
extern VSF_HAL_TEMPLATE_DEC_INSTANCE_TYPE *const VSF_HAL_TEMPLATE_DEC_INSTANCE_ARRAY[VSF_HAL_TEMPLATE_DEC_COUNT];

#   ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_INDEX_ARRAY
#       define VSF_HAL_TEMPLATE_DEC_INSTANCE_INDEX_ARRAY    VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX, _indexs)
#   endif
/*
extern const uint8_t vsf_hw_spi_indexs[2];
 */
extern const uint8_t VSF_HAL_TEMPLATE_DEC_INSTANCE_INDEX_ARRAY[VSF_HAL_TEMPLATE_DEC_COUNT];

#ifndef VSF_HAL_TEMPLATE_DEC_INSTANCE_NAME_ARRAY
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE_NAME_ARRAY         VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX, _names)
#endif
/*
extern const uint8_t vsf_hw_spi_names[2];
 */
extern const char *VSF_HAL_TEMPLATE_DEC_INSTANCE_NAME_ARRAY[VSF_HAL_TEMPLATE_DEC_COUNT];

#   ifndef VSF_HAL_TEMPLATE_DEC_DEVICE_ARRAY
#       define VSF_HAL_TEMPLATE_DEC_DEVICE_ARRAY            VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX, _devices)
#   endif

/*
extern const uint8_t vsf_hw_spi_devices[2];
 */
extern const vsf_hal_device_t VSF_HAL_TEMPLATE_DEC_DEVICE_ARRAY[VSF_HAL_TEMPLATE_DEC_COUNT];
#endif

/**************************** undef ********************************************/

#undef VSF_HAL_TEMPLATE_DEC_NAME
#undef VSF_HAL_TEMPLATE_DEC_UPCASE_NAME
#undef VSF_HAL_TEMPLATE_DEC_DEVICE_PREFIX
#undef VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX
#undef VSF_HAL_TEMPLATE_DEC_PREFIX
#undef VSF_HAL_TEMPLATE_DEC_REMAP
#undef VSF_HAL_TEMPLATE_DEC_REMAP_PREFIX
#undef VSF_HAL_TEMPLATE_DEC_UPCASE_PREFIX
#undef VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX
#undef VSF_HAL_TEMPLATE_DEC_EXTERN_OP
#undef VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX
#undef VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX
#undef VSF_HAL_TEMPLATE_DEC_DEFINED_COUNT
#undef VSF_HAL_TEMPLATE_DEC_DEFINED_MASK
#undef VSF_HAL_TEMPLATE_DEC_COUNT
#undef VSF_HAL_TEMPLATE_DEC_MASK
#undef VSF_HAL_TEMPLATE_DEC_TYPE
#undef VSF_HAL_TEMPLATE_DEC_REMAP_TYPE
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_TYPE
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_APIS
#undef VSF_HAL_TEMPLATE_DEC_ADDITIONAL_API
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_TYPE
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_OP_INSTANCE
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_ARRAY
#undef VSF_HAL_TEMPLATE_DEC_LV0
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_INDEX_ARRAY
#undef VSF_HAL_TEMPLATE_DEC_INSTANCE_NAME_ARRAY
#undef VSF_HAL_TEMPLATE_DEC_REMOVE_ARRAY
#undef VSF_HAL_TEMPLATE_DEC_DEVICE_ARRAY
