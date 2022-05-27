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

#ifndef __HAL_DRIVER_SPI_INTERFACE_H__
#define __HAL_DRIVER_SPI_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_SPI_CFG_MULTI_CLASS
#   define VSF_SPI_CFG_MULTI_CLASS              DISABLED
#endif

// Turn off multi class support for the current implementation
// when the VSF_SPI_CFG_MULTI_CLASS is enabled
#ifndef VSF_SPI_CFG_IMPLEMENT_OP
#   if VSF_SPI_CFG_MULTI_CLASS == ENABLED
#       define VSF_SPI_CFG_IMPLEMENT_OP         ENABLED
#   else
#       define VSF_SPI_CFG_IMPLEMENT_OP         DISABLED
#   endif
#endif

// VSF_SPI_CFG_PREFIX: use for macro vsf_spi_{init, enable, ...}
#ifndef VSF_SPI_CFG_PREFIX
#   if VSF_SPI_CFG_MULTI_CLASS == ENABLED
#       define VSF_SPI_CFG_PREFIX           vsf
#   elif defined(VSF_HW_SPI_COUNT) && (VSF_HW_SPI_COUNT != 0)
#       define VSF_SPI_CFG_PREFIX           vsf_hw
#   endif
#endif

#ifndef VSF_SPI_CFG_FUNCTION_RENAME
#   define VSF_SPI_CFG_FUNCTION_RENAME          ENABLED
#endif

#ifndef VSF_SPI_CFG_MULTIPLEX_CS
#   define VSF_SPI_CFG_MULTIPLEX_CS             DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_MODE
#   define VSF_SPI_CFG_REIMPLEMENT_MODE         DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_IRQ_MASK
#   define VSF_SPI_CFG_REIMPLEMENT_IRQ_MASK     DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_STATUS
#   define VSF_SPI_CFG_REIMPLEMENT_STATUS       DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_SPI_CFG_REIMPLEMENT_CAPABILITY   DISABLED
#endif

#ifndef VSF_SPI_DATASIZE_TO_BYTE
#   define VSF_SPI_DATASIZE_TO_BYTE         (((((__S) & SPI_DATASIZE_MASK) >> __SPI_DATASIZE_OFFSET) + 8) / 8)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_SPI_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        spi, init,                 VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, spi_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,         spi, enable,               VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,         spi, disable,              VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             spi, irq_enable,           VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, em_spi_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             spi, irq_disable,          VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, em_spi_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, spi_status_t,     spi, status,               VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, spi_capability_t, spi, capability,           VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             spi, cs_active,            VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, uint_fast8_t index) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             spi, cs_inactive,          VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, uint_fast8_t index) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             spi, fifo_transfer,        VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, \
                                                                                       void *out_buffer_ptr, uint_fast32_t out_cnt, uint_fast32_t *out_offset_ptr, \
                                                                                       void *in_buffer_ptr, uint_fast32_t in_cnt, uint_fast32_t *in_offset_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        spi, request_transfer,     VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, void *out_buffer_ptr, \
                                                                                       void *in_buffer_ptr, uint_fast32_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        spi, cancel_transfer,      VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, int_fast32_t,     spi, get_transfered_count, VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr)

/*============================ TYPES =========================================*/

#if VSF_SPI_CFG_REIMPLEMENT_MODE == DISABLED
//! spi working mode
typedef enum em_spi_mode_t {
    SPI_MASTER                  = 0x00ul << 0,      //!< select master mode
    SPI_SLAVE                   = 0x01ul << 0,      //!< select slave mode
    SPI_DIR_MODE_MASK           = 0x01ul << 0,

    SPI_MSB_FIRST               = 0x00ul << 1,      //!< default enable MSB
    SPI_LSB_FIRST               = 0x01ul << 1,      //!< transfer LSB first
    SPI_BIT_ORDER_MASK          = 0x01ul << 1,

    SPI_CPOL_LOW                = 0x00ul << 2,      //!< SCK clock polarity is low
    SPI_CPOL_HIGH               = 0x01ul << 2,      //!< SCK clock polarity is high
    SPI_CPHA_LOW                = 0x00ul << 3,      //!< SCK clock phase is low
    SPI_CPHA_HIGH               = 0x01ul << 3,      //!< SCK clock phase is high
    SPI_CLOCK_MODE_0            = SPI_CPOL_LOW  | SPI_CPHA_LOW,
    SPI_CLOCK_MODE_1            = SPI_CPOL_LOW  | SPI_CPHA_HIGH,
    SPI_CLOCK_MODE_2            = SPI_CPOL_HIGH | SPI_CPHA_LOW,
    SPI_CLOCK_MODE_3            = SPI_CPOL_HIGH | SPI_CPHA_HIGH,
    SPI_CLOCK_MODE_MASK         = SPI_CLOCK_MODE_3,

    __SPI_DATASIZE_OFFSET       = 0x04,
    SPI_DATASIZE_8              = 0x07ul << __SPI_DATASIZE_OFFSET,      //!< datasize is 8 bits
    SPI_DATASIZE_9              = 0x08ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_10             = 0x09ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_11             = 0x0Aul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_12             = 0x0Bul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_13             = 0x0Cul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_14             = 0x0Dul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_15             = 0x0Eul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_16             = 0x0Ful << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_17             = 0x10ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_18             = 0x11ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_19             = 0x12ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_20             = 0x13ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_21             = 0x14ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_22             = 0x15ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_23             = 0x16ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_24             = 0x17ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_25             = 0x18ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_26             = 0x19ul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_27             = 0x1Aul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_28             = 0x1Bul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_29             = 0x1Cul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_30             = 0x1Dul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_31             = 0x1Eul << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_32             = 0x1Ful << __SPI_DATASIZE_OFFSET,
    SPI_DATASIZE_MASK           = 0x1Ful << __SPI_DATASIZE_OFFSET,


    SPI_AUTO_CS_DISABLE         = 0x00ul << 9,
    SPI_AUTO_CS_ENABLE          = 0x01ul << 9,
    SPI_AUTO_CS_MASK            = 0x01ul << 9,

    SPI_LOOP_BACK               = 0x01ul << 10,     //!< enable loop back
} em_spi_mode_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_IRQ_MASK == DISABLED
/*! \brief em_spi_irq_mask_t
 *! \note em_spi_irq_mask_t should provide irq masks
 */
typedef enum em_spi_irq_mask_t {
    // TX/RX reach fifo threshold, threshold on some devices is bound to 1
    SPI_IRQ_MASK_TX             = 0x01ul << 0,
    SPI_IRQ_MASK_RX             = 0x01ul << 1,

    // request_rx/request_tx complete
    SPI_IRQ_MASK_TX_CPL         = 0x01ul << 2,
    SPI_IRQ_MASK_CPL            = 0x01ul << 3,

    // optional
    // FIFO
    SPI_IRQ_MASK_RX_FIFO_FULL   = 0x01ul << 4,
    SPI_IRQ_MASK_TX_FIFO_EMPTY  = 0x01ul << 5,

    SPI_IRQ_MASK                =  SPI_IRQ_MASK_TX
                                 | SPI_IRQ_MASK_RX
                                 | SPI_IRQ_MASK_TX_CPL
                                 | SPI_IRQ_MASK_CPL
                                 | SPI_IRQ_MASK_RX_FIFO_FULL
                                 | SPI_IRQ_MASK_TX_FIFO_EMPTY,
} em_spi_irq_mask_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_STATUS == DISABLED
typedef struct spi_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
        };
    };
} spi_status_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct spi_capability_t {
    inherit(peripheral_capability_t)
    uint8_t cs_count;
} spi_capability_t;
#endif

typedef struct vsf_spi_t vsf_spi_t;

typedef void vsf_spi_isr_handler_t(void *target_ptr,
                                   vsf_spi_t *spi_ptr,
                                   em_spi_irq_mask_t irq_mask);

//! spi isr for api
typedef struct vsf_spi_isr_t {
    vsf_spi_isr_handler_t *handler_fn;
    void                  *target_ptr;
    vsf_arch_prio_t        prio;
} vsf_spi_isr_t;

//! spi configuration for api
typedef struct spi_cfg_t {
    em_spi_mode_t   mode;               //!< spi working mode
    uint32_t        clock_hz;
    vsf_spi_isr_t   isr;
} spi_cfg_t;

typedef struct vsf_spi_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_SPI_APIS(vsf)
} vsf_spi_op_t;

#if VSF_SPI_CFG_MULTI_CLASS == ENABLED
struct vsf_spi_t  {
    const vsf_spi_op_t * op;
};
#endif

dcl_interface(i_spi_t)

typedef void vsf_i_spi_isrhandler_t(void *target_ptr,
                                    const i_spi_t *i_spi_ptr,
                                    em_spi_irq_mask_t irq_mask);

//! spi isr for interface
typedef struct vsf_i_spi_isr_t {
    vsf_i_spi_isrhandler_t      *handler_fn;
    void                        *target_ptr;
    vsf_arch_prio_t             prio;
} vsf_i_spi_isr_t;


//! spi configuration for interface
typedef struct i_spi_cfg_t {
    uint32_t                    mode;               //!< spi working mode
    uint32_t                    clock_hz;
    vsf_i_spi_isr_t             isr;
} i_spi_cfg_t;

//! class: spi_t
def_interface(i_spi_t)
    union {
        implement(i_peripheral_t);
        struct {
            spi_status_t        (*Status)(void);
            spi_capability_t    (*Capability)(void);
        } SPI;
    };
    vsf_err_t                   (*Init)(i_spi_cfg_t *cfg_ptr);

    struct {
        void                    (*Set)(uintalu_t tIndex);
        void                    (*Clear)(uintalu_t tIndex);
    } CS;

    struct {
        //!< read/write of fifo
        void                    (*Transfer)(void *pOutput,
                                            uint_fast32_t* pOutCount,
                                            void *pInput,
                                            uint_fast32_t* pInCount);
        //!< flush fifo
        bool                    (*Flush)(void);
    } FIFO;

    struct {
        /*! \brief request a block exchaging access
         *! \param pOutput      address of output buffer
         *! \param pInput       address of input buffer
         *! \param nCount       the count of data in the two buffers
         *! \retval fsm_rt_cpl  The transaction is complete
         *! \retval fsm_rt_asyn The transaction is handled asynchronousely, i.e. by
         *!                     DMA or by ISR or etc.
         *! \retval fsm_rt_on_going User should poll this API until fsm_rt_cpl or err
         *!                     value is returned.
         *! \retval vsf_err_t   Error value is returned.
         */
        vsf_err_t               (*RequestTransfer)(void *pOutput, void *pInput, uint_fast32_t nCount);

        /*! \brief cancel on going communication */
        vsf_err_t               (*Cancel)(void);

        /*! \brief get transfered count */
        int_fast32_t            (*GetTransferedCount)(void);
    } Block;

    struct {
        void                    (*Enable)(em_spi_irq_mask_t mask);
        void                    (*Disable)(em_spi_irq_mask_t mask);
    } IRQ;
end_def_interface(i_spi_t)

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t        vsf_spi_init(               vsf_spi_t *spi_ptr,
                                                    spi_cfg_t *cfg_ptr);

extern fsm_rt_t         vsf_spi_enable(             vsf_spi_t *spi_ptr);
extern fsm_rt_t         vsf_spi_disable(            vsf_spi_t *spi_ptr);

extern void             vsf_spi_irq_enable(         vsf_spi_t *spi_ptr,
                                                    em_spi_irq_mask_t irq_mask);
extern void             vsf_spi_irq_disable(        vsf_spi_t *spi_ptr,
                                                    em_spi_irq_mask_t irq_mask);

extern void             vsf_spi_cs_active(          vsf_spi_t *spi_ptr,
                                                    uint_fast8_t index);
extern void             vsf_spi_cs_inactive(        vsf_spi_t *spi_ptr,
                                                    uint_fast8_t index);

extern spi_status_t     vsf_spi_status(             vsf_spi_t *spi_ptr);

extern spi_capability_t vsf_spi_capability(         vsf_spi_t *spi_ptr);

extern void             vsf_spi_fifo_transfer(      vsf_spi_t *spi_ptr,
                                                    void *out_buffer_ptr,
                                                    uint_fast32_t  out_cnt,
                                                    uint_fast32_t* out_offset_ptr,
                                                    void *in_buffer_ptr,
                                                    uint_fast32_t  in_cnt,
                                                    uint_fast32_t* in_offset_ptr);

extern vsf_err_t        vsf_spi_request_transfer(   vsf_spi_t *spi_ptr,
                                                    void *out_buffer_ptr,
                                                    void *in_buffer_ptr,
                                                    uint_fast32_t count);
extern vsf_err_t        vsf_spi_cancel_transfer(    vsf_spi_t *spi_ptr);
extern int_fast32_t     vsf_spi_get_transfered_count(vsf_spi_t *spi_ptr);



/*============================ INCLUDES ======================================*/

#if VSF_SPI_CFG_MULTIPLEX_CS == ENABLED
#   include "hal/driver/common/template/vsf_template_io.h"
#   include "hal/driver/common/spi/multiplex_spi.h"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SPI_CFG_FUNCTION_RENAME == ENABLED
#   define vsf_spi_init(__SPI, ...)                                             \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_init)                 ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_enable(__SPI)                                                \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_enable)               ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI)
#   define vsf_spi_disable(__SPI)                                               \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_disable)              ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI)
#   define vsf_spi_irq_enable(__SPI, ...)                                       \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_irq_enable)           ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_irq_disable(__SPI, ...)                                      \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_irq_disable)          ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_status(__SPI)                                                \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_status)               ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI)
#   define vsf_spi_capability(__SPI)                                            \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_capability)           ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI)
#   define vsf_spi_cs_active(__SPI, ...)                                        \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cs_active)            ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_cs_inactive(__SPI, ...)                                      \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cs_inactive)          ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_fifo_transfer(__SPI, ...)                                    \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_fifo_transfer)        ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_request_transfer(__SPI, ...)                                 \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_request_transfer)     ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_cancel_transfer(__SPI)                                       \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cancel_transfer)      ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI)
#   define vsf_spi_get_transfered_count(__SPI)                                  \
        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_get_transfered_count) ((VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t) *)__SPI)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_SPI_INTERFACE_H__*/

