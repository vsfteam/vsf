/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

//#ifndef __CLASS_KERNEL_POOL_H__       /* deliberately comment this out! */
//#define __CLASS_KERNEL_POOL_H__       /* deliberately comment this out! */


/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __VSF_CLASS_USE_STRICT_TEMPLATE__
#include "utilities/ooc_class.h"     

//! \name pool
//! @{
declare_class(vsf_kernel_pool_t)

def_class(vsf_kernel_pool_t, 
    which(inherit(vsf_pool_t)))
    private_member(
        vsf_sem_t  available_sem;
    )
end_def_class(vsf_kernel_pool_t)
//! @}

//#endif                                /* deliberately comment this out! */