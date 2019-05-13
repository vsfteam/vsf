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

#ifndef __HAL_DRIVER_COMMON_INTERFACE_H__
#define __HAL_DRIVER_COMMON_INTERFACE_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct peripheral_status_t peripheral_status_t;
struct peripheral_status_t {
    uint32_t    bIsBusy : 1;
    uint32_t            : 31;
};


typedef struct peripheral_capability_t peripheral_capability_t;
struct peripheral_capability_t {
    union {
        struct {
            uint8_t bCanReadByte        : 1;
            uint8_t bCanReadBlock       : 1;
            uint8_t bSupportFIFO        : 1;
            uint8_t bSupportDMA         : 1;
            uint8_t bSupportISR         : 1;
            uint8_t bRandomAccess       : 1;
            uint8_t                     : 2;
        };
        uint8_t bCanRead;
    }Read;
    
    union {
        struct {
            uint8_t bCanWriteByte       : 1;
            uint8_t bCanWriteBlock      : 1;
            uint8_t bSupportFIFO        : 1;
            uint8_t bSupportDMA         : 1;
            uint8_t bSupportISR         : 1;
            uint8_t bRandomAccess       : 1;
            uint8_t                     : 2;
        };
        uint8_t bCanRead;
    }Write;
    
    union {
        struct {
            /* Data Type: 2^(u6DataTypeSize) */
            uint16_t u3DataTypeSize     : 3; 

            /* Block Size: 2^(u5BlockSize) */
            uint16_t u5BlockSize        : 5;
            uint16_t                    : 8;
        };
        uint16_t hwInfo;
    }Feature;
};

//! \name class: peripheral_t
//! @{
def_interface(peripheral_t)
    peripheral_status_t (*Status)   (void);
    fsm_rt_t            (*Unint)    (void);
    union {
        fsm_rt_t        (*Enable)   (void);
        fsm_rt_t        (*Open)     (void);
    };
    union {
        fsm_rt_t        (*Disable)  (void);
        fsm_rt_t        (*Close)    (void);
    };
end_def_interface(peripheral_t)
//! @}

def_interface(vsf_async_block_access_t)
    /*! \brief request a block read
     *! \param pchBuffer    address of target memory
     *! \param wSize        the size of the target memory
     *! \retval fsm_rt_cpl  The transaction is complete
     *! \retval fsm_rt_asyn The transaction is handled asynchronousely, i.e. by 
     *!                     DMA or by ISR or etc.
     *! \retval fsm_rt_on_going User should poll this API until fsm_rt_cpl or err
     *!                     value is returned.
     *! \retval vsf_err_t   Error value is returned.
     */
    fsm_rt_t  (*RequestRead)(uint8_t *pchBuffer, uint_fast32_t wSize);
    /*! \brief request a block write
     *! \param pchBuffer    address of target memory
     *! \param wSize        the size of the target memory
     *! \retval fsm_rt_cpl  The transaction is complete
     *! \retval fsm_rt_asyn The transaction is handled asynchronousely, i.e. by 
     *!                     DMA or by ISR or etc.
     *! \retval fsm_rt_on_going User should poll this API until fsm_rt_cpl or err
     *!                     value is returned.
     *! \retval vsf_err_t   Error value is returned.
     */
    fsm_rt_t  (*RequestWrite)(uint8_t *pchBuffer, uint_fast32_t wSize);
end_def_interface(vsf_async_block_access_t)


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/



#endif
