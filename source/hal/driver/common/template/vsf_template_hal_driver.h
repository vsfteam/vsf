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

#ifndef __HAL_DRIVER_COMMON_INTERFACE_H__
#define __HAL_DRIVER_COMMON_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_TEMPLATE_HAL_API_OP(__prefix_name, __module__, __return, __name, ...)           \
    .__name = & __prefix_name ## __module__ ## __name,

#define VSF_TEMPLATE_HAL_API_EXTERN(__prefix_name, __module__, __return, __name, ...)       \
    extern __return __prefix_name ## __module__ ## __name ( __VA_ARGS__ );

#define VSF_TEMPLATE_HAL_API_DEFINE(__prefix_name, __module__, __return, __name, ...)       \
    __return __prefix_name ## __module__ ## __name ( __VA_ARGS__ )

/*============================ TYPES =========================================*/

/*
 *  if __VSF_HAL_XXXX_IS_REG_CLK_ASYNC__ is defined for some module, means
 *    vsf_xxx_enable/vsf_xxx_disable will not succeed in one run. And user can
 *    not call vsf_xxx_init before disabled. Or, user don't need to call
 *    vsf_xxx_disable before re-initializing(by calling vsf_xxx_init).
 */

typedef struct peripheral_status_t peripheral_status_t;
struct peripheral_status_t {
    uint32_t    is_busy                 : 1;
    uint32_t                            : 31;
};


typedef struct peripheral_capability_t peripheral_capability_t;
struct peripheral_capability_t {
    union {
        struct {
            uint8_t can_read_byte       : 1;
            uint8_t can_read_block      : 1;
            uint8_t support_fifo        : 1;
            uint8_t support_dma         : 1;
            uint8_t support_isr         : 1;
            uint8_t random_access       : 1;
            uint8_t                     : 2;
        };
        uint8_t bCanRead;
    }Read;

    union {
        struct {
            uint8_t can_write_byte      : 1;
            uint8_t can_write_block     : 1;
            uint8_t support_fifo        : 1;
            uint8_t support_dma         : 1;
            uint8_t support_isr         : 1;
            uint8_t random_access       : 1;
            uint8_t                     : 2;
        };
        uint8_t can_read;
    }Write;

    union {
        struct {
            /* Data Type: 2^(u3DataTypeSize) */
            uint16_t u3_data_type_size  : 3;

            /* Block Size: 2^(u5BlockSize) */
            uint16_t u5_block_size      : 5;
            uint16_t                    : 8;
        };
        uint16_t info;
    }Feature;
};

//! \name interface: i_peripheral_t
//! @{
def_interface(i_peripheral_t)
    peripheral_status_t     (*Status)       (void);
    peripheral_capability_t (*Capability)   (void);
    fsm_rt_t                (*Uninit)       (void);
    union {
        fsm_rt_t            (*Enable)       (void);
        fsm_rt_t            (*Open)         (void);
    };
    union {
        fsm_rt_t            (*Disable)      (void);
        fsm_rt_t            (*Close)        (void);
    };
end_def_interface(i_peripheral_t)
//! @}

def_interface(vsf_async_block_access_t)
    /*! \brief request a block access
     *! \param buffer_ptr    address of target memory
     *! \param u32_size        the size of the target memory
     *! \retval fsm_rt_cpl  The transaction is complete
     *! \retval fsm_rt_asyn The transaction is handled asynchronousely, i.e. by
     *!                     DMA or by ISR or etc.
     *! \retval fsm_rt_on_going User should poll this API until fsm_rt_cpl or err
     *!                     value is returned.
     *! \retval vsf_err_t   Error value is returned.
     */
    vsf_err_t               (*Request)(uint8_t *buffer_ptr, uint_fast32_t size);

    /*! \brief cancel on going communication */
    vsf_err_t               (*Cancel) (void);

    /*! \brief get transfered count */
    int_fast32_t            (*GetTransferredCount)(void);
end_def_interface(vsf_async_block_access_t)


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
