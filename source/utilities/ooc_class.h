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

#if defined(__VSF_CLASS_IMPLEMENT) || defined(__VSF_CLASS_IMPLEMENT__)
#   undef __VSF_CLASS_IMPLEMENT
#   undef __VSF_CLASS_IMPLEMENT__
#   define __PLOOC_CLASS_IMPLEMENT
#endif
#if defined(__VSF_CLASS_INHERIT) || defined(__VSF_CLASS_INHERIT__)
#   undef __VSF_CLASS_INHERIT
#   undef __VSF_CLASS_INHERIT__
#   define __PLOOC_CLASS_INHERIT
#endif

#include "./3rd-party/PLOOC/raw/plooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_declare_class(__name)           dcl_simple_class(__name)
#define vsf_dcl_class                       vsf_declare_class

#define vsf_class(__name)                                                       \
                dcl_simple_class(__name)                                        \
                def_simple_class(__name)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

/*#endif */                                /* deliberately comment this out! */
