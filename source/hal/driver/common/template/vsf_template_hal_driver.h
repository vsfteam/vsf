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

#ifndef __VSF_TEMPLATE_COMMON_H__
#define __VSF_TEMPLATE_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"
#include "vsf_template_static_assert_unique.h"

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_HAL_TEMPLATE_API_OP(__prefix_name, __return, __module__, __name, __first_args, ...)           \
    .__name = (__return (*)( VSF_MCONNECT(vsf, _, __module__, _t) *, ##__VA_ARGS__ ))& VSF_MCONNECT(__prefix_name, _, __name),

#define VSF_HAL_TEMPLATE_API_FP(__prefix_name, __return, __module__, __name, __first_args, ...)           \
    __return (* __name )( __first_args, ##__VA_ARGS__ );

#define VSF_HAL_TEMPLATE_API_EXTERN(__prefix_name, __return, __module__, __name, __first_args, ...)       \
    extern __return VSF_MCONNECT(__prefix_name, _, __name)( __first_args, ##__VA_ARGS__ );

#define VSF_HAL_TEMPLATE_API_DEFINE(__prefix_name, __return, __module__, __name, __first_args, ...)       \
    __return VSF_MCONNECT(__prefix_name, _, __name)( __first_args, ##__VA_ARGS__ )

#define __VSF_HAL_COUNT_TO_MASK(__CNT)                                          \
    ((0x1ull << (__CNT > 0x00 ? 0x00 : 0)) |                                    \
     (0x1ull << (__CNT > 0x01 ? 0x01 : 0)) |                                    \
     (0x1ull << (__CNT > 0x02 ? 0x02 : 0)) |                                    \
     (0x1ull << (__CNT > 0x03 ? 0x03 : 0)) |                                    \
     (0x1ull << (__CNT > 0x04 ? 0x04 : 0)) |                                    \
     (0x1ull << (__CNT > 0x05 ? 0x05 : 0)) |                                    \
     (0x1ull << (__CNT > 0x06 ? 0x06 : 0)) |                                    \
     (0x1ull << (__CNT > 0x07 ? 0x07 : 0)) |                                    \
     (0x1ull << (__CNT > 0x08 ? 0x08 : 0)) |                                    \
     (0x1ull << (__CNT > 0x09 ? 0x09 : 0)) |                                    \
     (0x1ull << (__CNT > 0x0A ? 0x0A : 0)) |                                    \
     (0x1ull << (__CNT > 0x0B ? 0x0B : 0)) |                                    \
     (0x1ull << (__CNT > 0x0C ? 0x0C : 0)) |                                    \
     (0x1ull << (__CNT > 0x0D ? 0x0D : 0)) |                                    \
     (0x1ull << (__CNT > 0x0E ? 0x0E : 0)) |                                    \
     (0x1ull << (__CNT > 0x0F ? 0x0F : 0)) |                                    \
     (0x1ull << (__CNT > 0x10 ? 0x10 : 0)) |                                    \
     (0x1ull << (__CNT > 0x11 ? 0x11 : 0)) |                                    \
     (0x1ull << (__CNT > 0x12 ? 0x12 : 0)) |                                    \
     (0x1ull << (__CNT > 0x13 ? 0x13 : 0)) |                                    \
     (0x1ull << (__CNT > 0x14 ? 0x14 : 0)) |                                    \
     (0x1ull << (__CNT > 0x15 ? 0x15 : 0)) |                                    \
     (0x1ull << (__CNT > 0x16 ? 0x16 : 0)) |                                    \
     (0x1ull << (__CNT > 0x17 ? 0x17 : 0)) |                                    \
     (0x1ull << (__CNT > 0x18 ? 0x18 : 0)) |                                    \
     (0x1ull << (__CNT > 0x19 ? 0x19 : 0)) |                                    \
     (0x1ull << (__CNT > 0x1A ? 0x1A : 0)) |                                    \
     (0x1ull << (__CNT > 0x1B ? 0x1B : 0)) |                                    \
     (0x1ull << (__CNT > 0x1C ? 0x1C : 0)) |                                    \
     (0x1ull << (__CNT > 0x1D ? 0x1D : 0)) |                                    \
     (0x1ull << (__CNT > 0x1E ? 0x1E : 0)) |                                    \
     (0x1ull << (__CNT > 0x1F ? 0x1F : 0)) |                                    \
     (0x1ull << (__CNT > 0x20 ? 0x20 : 0)) |                                    \
     (0x1ull << (__CNT > 0x21 ? 0x21 : 0)) |                                    \
     (0x1ull << (__CNT > 0x22 ? 0x22 : 0)) |                                    \
     (0x1ull << (__CNT > 0x23 ? 0x23 : 0)) |                                    \
     (0x1ull << (__CNT > 0x24 ? 0x24 : 0)) |                                    \
     (0x1ull << (__CNT > 0x25 ? 0x25 : 0)) |                                    \
     (0x1ull << (__CNT > 0x26 ? 0x26 : 0)) |                                    \
     (0x1ull << (__CNT > 0x27 ? 0x27 : 0)) |                                    \
     (0x1ull << (__CNT > 0x28 ? 0x28 : 0)) |                                    \
     (0x1ull << (__CNT > 0x29 ? 0x29 : 0)) |                                    \
     (0x1ull << (__CNT > 0x2A ? 0x2A : 0)) |                                    \
     (0x1ull << (__CNT > 0x2B ? 0x2B : 0)) |                                    \
     (0x1ull << (__CNT > 0x2C ? 0x2C : 0)) |                                    \
     (0x1ull << (__CNT > 0x2D ? 0x2D : 0)) |                                    \
     (0x1ull << (__CNT > 0x2E ? 0x2E : 0)) |                                    \
     (0x1ull << (__CNT > 0x2F ? 0x2F : 0)) |                                    \
     (0x1ull << (__CNT > 0x30 ? 0x30 : 0)) |                                    \
     (0x1ull << (__CNT > 0x31 ? 0x31 : 0)) |                                    \
     (0x1ull << (__CNT > 0x32 ? 0x32 : 0)) |                                    \
     (0x1ull << (__CNT > 0x33 ? 0x33 : 0)) |                                    \
     (0x1ull << (__CNT > 0x34 ? 0x34 : 0)) |                                    \
     (0x1ull << (__CNT > 0x35 ? 0x35 : 0)) |                                    \
     (0x1ull << (__CNT > 0x36 ? 0x36 : 0)) |                                    \
     (0x1ull << (__CNT > 0x37 ? 0x37 : 0)) |                                    \
     (0x1ull << (__CNT > 0x38 ? 0x38 : 0)) |                                    \
     (0x1ull << (__CNT > 0x39 ? 0x39 : 0)) |                                    \
     (0x1ull << (__CNT > 0x3A ? 0x3A : 0)) |                                    \
     (0x1ull << (__CNT > 0x3B ? 0x3B : 0)) |                                    \
     (0x1ull << (__CNT > 0x3C ? 0x3C : 0)) |                                    \
     (0x1ull << (__CNT > 0x3D ? 0x3D : 0)) |                                    \
     (0x1ull << (__CNT > 0x3E ? 0x3E : 0)) |                                    \
     (0x1ull << (__CNT > 0x3F ? 0x3F : 0)))

#define VSF_HAL_COUNT_TO_MASK(__CNT)                                            \
    __VSF_HAL_COUNT_TO_MASK(__CNT)

#define __VSF_HAL_MASK_TO_COUNT(__MASK)                                         \
    ((__MASK & (0x01ull << 0x00) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x01) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x02) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x03) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x04) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x05) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x06) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x07) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x08) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x09) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x0A) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x0B) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x0C) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x0D) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x0E) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x0F) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x10) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x11) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x12) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x13) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x14) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x15) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x16) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x17) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x18) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x19) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x1A) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x1B) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x1C) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x1D) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x1E) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x1F) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x20) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x21) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x22) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x23) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x24) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x25) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x26) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x27) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x28) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x29) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x2A) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x2B) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x2C) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x2D) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x2E) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x2F) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x30) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x31) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x32) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x33) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x34) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x35) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x36) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x37) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x38) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x39) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x3A) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x3B) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x3C) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x3D) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x3E) ? 1 : 0) +                                     \
     (__MASK & (0x01ull << 0x3F) ? 1 : 0))

#define VSF_HAL_MASK_TO_COUNT(__MASK)                                           \
    __VSF_HAL_MASK_TO_COUNT(__MASK)

/*============================ TYPES =========================================*/

typedef struct vsf_hal_device_t {
    void *pointer;
    char *name;
} vsf_hal_device_t;

/*
 *  if __VSF_HAL_XXXX_IS_REG_CLK_ASYNC__ is defined for some module, means
 *    vsf_xxx_enable/vsf_xxx_disable will not succeed in one run. And user can
 *    not call vsf_xxx_init before disabled. Or, user don't need to call
 *    vsf_xxx_disable before re-initializing(by calling vsf_xxx_init).
 */

typedef struct vsf_peripheral_status_t vsf_peripheral_status_t;
struct vsf_peripheral_status_t {
    uint32_t    is_busy                 : 1;
    uint32_t                            : 31;
};


typedef struct vsf_peripheral_capability_t vsf_peripheral_capability_t;
struct vsf_peripheral_capability_t {
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
        uint8_t can_read;
    } Read;

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
        uint8_t can_write;
    } Write;

    union {
        struct {
            /* Data Type: 2^(u3DataTypeSize) */
            uint16_t u3_data_type_size  : 3;

            /* Block Size: 2^(u5BlockSize) */
            uint16_t u5_block_size      : 5;
            uint16_t                    : 8;
        };
        uint16_t info;
    } Feature;
};

//! \name interface: i_peripheral_t
//! @{
def_interface(i_peripheral_t)
    vsf_peripheral_status_t     (*fn_Status)        (void);
    vsf_peripheral_capability_t (*fn_Capability)    (void);
    fsm_rt_t                    (*fn_Uninit)        (void);
    union {
        fsm_rt_t                (*fn_Enable)        (void);
        fsm_rt_t                (*fn_Open)          (void);
    };
    union {
        fsm_rt_t                (*fn_Disable)       (void);
        fsm_rt_t                (*fn_Close)         (void);
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
    vsf_err_t               (*fn_Request)(uint8_t *buffer_ptr, uint_fast32_t size);

    /*! \brief cancel on going communication */
    vsf_err_t               (*fn_Cancel) (void);

    /*! \brief get transferred count */
    int_fast32_t            (*fn_GetTransferredCount)(void);
end_def_interface(vsf_async_block_access_t)


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
