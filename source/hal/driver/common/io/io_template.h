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

#define VSF_HAL_TEMPLATE_DEC_NAME                     _io
#define VSF_HAL_TEMPLATE_DEC_UPCASE_NAME              _IO

#ifndef VSF_IO_CFG_DEC_PREFIX
#   error "Please define VSF_IO_CFG_DEC_PREFIX when using vsf_template_io.h"
#endif

// IO always is singl instance
#define VSF_HAL_TEMPLATE_DEC_REMOVE_ARRAY
#define VSF_HAL_TEMPLATE_DEC_COUNT                1
#define VSF_HAL_TEMPLATE_DEC_MASK                 1
#define VSF_HAL_TEMPLATE_DEC_LV0(__COUNT, __DONT_CARE)                            \
    extern VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, _t) \
        VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME);

#ifndef VSF_IO_CFG_DEC_INSTANCE_PREFIX
#   define VSF_IO_CFG_DEC_INSTANCE_PREFIX         VSF_IO_CFG_DEC_PREFIX
#endif

#ifdef VSF_IO_CFG_DEC_UPCASE_PREFIX
#   define VSF_HAL_TEMPLATE_DEC_UPCASE_PREFIX       VSF_IO_CFG_DEC_UPCASE_PREFIX
#endif

#include "hal/driver/common/template/vsf_template_declaration.h"

#undef VSF_IO_CFG_DEC_PREFIX
#undef VSF_IO_CFG_DEC_INSTANCE_PREFIX
#undef VSF_IO_CFG_DEC_UPCASE_PREFIX

