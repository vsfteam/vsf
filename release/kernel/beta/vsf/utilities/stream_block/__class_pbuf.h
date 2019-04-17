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

//#ifndef __CLASS_PUBF_H__              /* deliberately comment this out! */
//#define __CLASS_PUBF_H__

/*============================ INCLUDES ======================================*/

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __VSF_CLASS_USE_STRICT_TEMPLATE__
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/    
/*============================ TYPES =========================================*/

//! \brief fixed memory block used as stream buffer
//! @{

declare_class(vsf_pbuf_t)

def_class(vsf_pbuf_t, 
    which(
        implement(vsf_slist_t)
    ))

    private_member(
        union {
            struct {
                uint32_t    pbuf_size           : 24;
                uint32_t    is_no_write         : 1;
                uint32_t    is_no_read          : 1;
                uint32_t    is_no_direct_access : 1;
                uint32_t                        : 1;
                
                //!< should be zero, reserved for future
                uint32_t    pbuf_type           : 4;    
            };
            uint32_t        capability;
        };
        
        uint8_t  *pchBuffer;                            //!< buffer address
       
        uint32_t    size                        : 24;
        uint32_t    adapter_id                  : 8;
    )
end_def_class(vsf_pbuf_t)
//! @}


//#endif                                /* deliberately comment this out! */

