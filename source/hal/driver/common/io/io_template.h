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

/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_io.h"

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_HAL_TEMPLATE_NAME                     _io
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _IO

#ifndef VSF_IO_CFG_DEC_PREFIX
#   error "Please define VSF_GPIO_CFG_DEC_PREFIX when using vsf_template_io.h"
#endif

#ifdef VSF_IO_CFG_DEC_UPCASE_PREFIX
#   define __VSF_HAL_TEMPLATE_UPCASE_PREFIX         VSF_IO_CFG_DEC_UPCASE_PREFIX
#endif

#define __VSF_HAL_TEMPLATE_DEC_COUNT                1
#define __VSF_HAL_TEMPLATE_DEC_LV0(__COUNT, __DONT_CARE)                        \
    extern VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t) \
        VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME);

#include "hal/driver/common/template/vsf_template_declaration.h"

#undef VSF_IO_CFG_DEC_PREFIX
#undef VSF_IO_CFG_DEC_UPCASE_PREFIX

