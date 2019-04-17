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

//#ifndef __VSF_KERNEL_THREAD_H__           /* deliberately comment this out! */
//#define __VSF_KERNEL_THREAD_H__           /* deliberately comment this out! */

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __VSF_CLASS_USE_STRICT_TEMPLATE__
#include "utilities/ooc_class.h"

declare_class(vsf_thread_t)

typedef void vsf_thread_entry_t(void *p);

//! \name thread
//! @{
def_class(vsf_thread_t,
    which(
#if VSF_CFG_TIMER_EN
        implement(vsf_teda_t)
#else
        implement(vsf_eda_t)
#endif
    ))

    public_member(
        // you can add public member here
        vsf_thread_entry_t  *pentry;
        uint16_t            stack_size;
        uint64_t            *pstack;                //!< stack must be 8byte aligned
    )
    private_member(
        jmp_buf         *pos;
        jmp_buf         *pret;
    )
end_def_class(vsf_thread_t)
//! @}

//#endif                                /* deliberately comment this out! */
