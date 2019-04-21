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

#ifndef __UTILITIES_COMMUNICATE_H__
#define __UTILITIES_COMMUNICATE_H__

/*============================ INCLUDES ======================================*/
#include "./3rd-party/PLOOC/plooc.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name stream
//! @{
typedef struct vsf_mem_t vsf_mem_t;
struct vsf_mem_t {
    union {
        uint8_t *pchBuffer;         //!< stream buffer
        uint8_t *pchSrc;
        void *pObj;
    };
    int_fast32_t nSize;            //!< stream size
};
//! @}


//! \name interface: byte pipe
//! @{
def_interface(i_byte_pipe_t)
    //!< read a byte
    bool (*ReadByte)(uint8_t *pchByte);
    //!< write a byte
    bool (*WriteByte)(uint_fast8_t chByte);
    
    bool (*Flush)(void);
end_def_interface(i_byte_pipe_t)
//! @}

//! \name interface: pipe
//! @{
def_interface(i_pipe_t) which ( implement(i_byte_pipe_t) )
    
    struct {
        //! read a block
        uint_fast32_t  (*Read)(uint8_t *pchStream, uint_fast32_t wSize);
        //! write a block
        uint_fast32_t  (*Write)(uint8_t *pchStream, uint_fast32_t wSize);
    } Stream;

end_def_interface(i_pipe_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
