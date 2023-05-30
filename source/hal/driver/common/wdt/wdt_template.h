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

#define VSF_HAL_TEMPLATE_DEC_NAME                     _wdt
#define VSF_HAL_TEMPLATE_DEC_UPCASE_NAME              _WDT

#ifndef __VSF_TEMPLATE_WDT_H__
#   error "Please include \"vsf_template_wdt.h\" before include wdt_template.h"
#endif

#ifndef VSF_WDT_CFG_DEC_PREFIX
#   error "Please define VSF_WDT_CFG_DEC_PREFIX before include wdt_template.h"
#endif

#ifndef VSF_WDT_CFG_DEC_INSTANCE_PREFIX
#   define VSF_WDT_CFG_DEC_INSTANCE_PREFIX         VSF_WDT_CFG_DEC_PREFIX
#endif

#ifdef VSF_WDT_CFG_DEC_UPCASE_PREFIX
#   define VSF_HAL_TEMPLATE_DEC_UPCASE_PREFIX       VSF_WDT_CFG_DEC_UPCASE_PREFIX
#endif

#include "hal/driver/common/template/vsf_template_declaration.h"

#undef VSF_WDT_CFG_DEC_PREFIX
#undef VSF_WDT_CFG_DEC_INSTANCE_PREFIX
#undef VSF_WDT_CFG_DEC_UPCASE_PREFIX

