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
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_HAL_TEMPLATE_DEC_NAME                   _io
#define VSF_HAL_TEMPLATE_DEC_UPCASE_NAME            _IO

#ifndef __VSF_TEMPLATE_IO_H__
#   error "Please include \"vsf_template_io.h\" before include io_template.h"
#endif

#ifndef VSF_IO_CFG_DEC_PREFIX
#   error "Please define VSF_IO_CFG_DEC_PREFIX before include io_template.h"
#endif

#ifndef VSF_IO_CFG_DEC_UPCASE_PREFIX
#   error "Please define VSF_IO_CFG_DEC_UPCASE_PREFIX before include io_template.h"
#endif

#ifndef VSF_IO_CFG_DEC_REMAP_PREFIX
#   define VSF_IO_CFG_DEC_REMAP_PREFIX              VSF_IO_CFG_DEC_PREFIX
#endif

#ifndef VSF_IO_CFG_DEC_COUNT_MASK_PREFIX
#   define VSF_IO_CFG_DEC_COUNT_MASK_PREFIX         VSF_IO_CFG_DEC_UPCASE_PREFIX
#endif

// IO always is singleton mode
#define VSF_HAL_TEMPLATE_DEC_REMOVE_ARRAY
#define VSF_HAL_TEMPLATE_DEC_COUNT                1
#define VSF_HAL_TEMPLATE_DEC_MASK                 1
#define VSF_HAL_TEMPLATE_DEC_LV0(__IDX, __DONT_CARE)                            \
    extern VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME, _t) \
        VSF_MCONNECT(VSF_HAL_TEMPLATE_DEC_PREFIX, VSF_HAL_TEMPLATE_DEC_NAME);

#include "hal/driver/common/template/vsf_template_instance_declaration.h"

#undef VSF_IO_CFG_DEC_PREFIX
#undef VSF_IO_CFG_DEC_UPCASE_PREFIX
#undef VSF_IO_CFG_DEC_COUNT_MASK_PREFIX
#undef VSF_IO_CFG_DEC_REMAP_PREFIX
#undef VSF_IO_CFG_DEC_EXTERN_OP
