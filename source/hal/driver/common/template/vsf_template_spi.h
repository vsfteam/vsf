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

// multi-class support enabled by default for maximum availability.
#ifndef VSF_SPI_CFG_MULTI_CLASS
#   define VSF_SPI_CFG_MULTI_CLASS                  ENABLED
#endif

// application code can redefine it
#ifndef VSF_SPI_CFG_PREFIX
#   if VSF_SPI_CFG_MULTI_CLASS == ENABLED
#       define VSF_SPI_CFG_PREFIX                   vsf
#   elif defined(VSF_HW_SPI_COUNT) && (VSF_HW_SPI_COUNT != 0)
#       define VSF_SPI_CFG_PREFIX                   vsf_hw
#   else
#       define VSF_SPI_CFG_PREFIX                   vsf
#   endif
#endif

#ifndef VSF_SPI_CFG_FUNCTION_RENAME
#   define VSF_SPI_CFG_FUNCTION_RENAME              ENABLED
#endif

#ifndef VSF_SPI_CFG_MULTIPLEX_CS
#   define VSF_SPI_CFG_MULTIPLEX_CS                 DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE        DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK    DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS      DISABLED
#endif

#ifndef VSF_SPI_DATASIZE_TO_BYTE
#   define VSF_SPI_DATASIZE_TO_BYTE         (((((__S) & SPI_DATASIZE_MASK) >> __SPI_DATASIZE_OFFSET) + 8) / 8)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_SPI_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, init,                 VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, vsf_spi_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             spi, enable,               VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             spi, disable,              VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, irq_enable,           VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, vsf_spi_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, irq_disable,          VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, vsf_spi_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_spi_status_t,     spi, status,               VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_spi_capability_t, spi, capability,           VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, cs_active,            VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, uint_fast8_t index) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, cs_inactive,          VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, uint_fast8_t index) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, fifo_transfer,        VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, \
                                                                                           void *out_buffer_ptr, uint_fast32_t out_cnt, uint_fast32_t *out_offset_ptr, \
                                                                                           void *in_buffer_ptr, uint_fast32_t in_cnt, uint_fast32_t *in_offset_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, request_transfer,     VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, void *out_buffer_ptr, \
                                                                                           void *in_buffer_ptr, uint_fast32_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            spi, cancel_transfer,      VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, int_fast32_t,         spi, get_transfered_count, VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr)

/*============================ TYPES =========================================*/

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
//! spi working mode
typedef enum vsf_spi_mode_t {
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
} vsf_spi_mode_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
/*! \brief vsf_spi_irq_mask_t
 *! \note vsf_spi_irq_mask_t should provide irq masks
 */
typedef enum vsf_spi_irq_mask_t {
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
} vsf_spi_irq_mask_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_spi_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_spi_status_t;
#endif

typedef struct vsf_spi_capability_t {
    inherit(vsf_peripheral_capability_t)
    uint8_t cs_count;
} vsf_spi_capability_t;

typedef struct vsf_spi_t vsf_spi_t;

typedef void vsf_spi_isr_handler_t(void *target_ptr,
                                   vsf_spi_t *spi_ptr,
                                   vsf_spi_irq_mask_t irq_mask);

//! spi isr for api
typedef struct vsf_spi_isr_t {
    vsf_spi_isr_handler_t *handler_fn;
    void                  *target_ptr;
    vsf_arch_prio_t        prio;
} vsf_spi_isr_t;

//! spi configuration for api
typedef struct vsf_spi_cfg_t {
    vsf_spi_mode_t   mode;               //!< spi working mode
    uint32_t        clock_hz;
    vsf_spi_isr_t   isr;
} vsf_spi_cfg_t;

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
                                    vsf_spi_irq_mask_t irq_mask);

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
            vsf_spi_status_t        (*Status)(void);
            vsf_spi_capability_t    (*Capability)(void);
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
        void                    (*Enable)(vsf_spi_irq_mask_t mask);
        void                    (*Disable)(vsf_spi_irq_mask_t mask);
    } IRQ;
end_def_interface(i_spi_t)

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t            vsf_spi_init(               vsf_spi_t *spi_ptr,
                                                        vsf_spi_cfg_t *cfg_ptr);

extern fsm_rt_t             vsf_spi_enable(             vsf_spi_t *spi_ptr);
extern fsm_rt_t             vsf_spi_disable(            vsf_spi_t *spi_ptr);

extern void                 vsf_spi_irq_enable(         vsf_spi_t *spi_ptr,
                                                        vsf_spi_irq_mask_t irq_mask);
extern void                 vsf_spi_irq_disable(        vsf_spi_t *spi_ptr,
                                                        vsf_spi_irq_mask_t irq_mask);

extern void                 vsf_spi_cs_active(          vsf_spi_t *spi_ptr,
                                                        uint_fast8_t index);
extern void                 vsf_spi_cs_inactive(        vsf_spi_t *spi_ptr,
                                                        uint_fast8_t index);

extern vsf_spi_status_t     vsf_spi_status(             vsf_spi_t *spi_ptr);

extern vsf_spi_capability_t vsf_spi_capability(         vsf_spi_t *spi_ptr);

extern void                 vsf_spi_fifo_transfer(      vsf_spi_t *spi_ptr,
                                                        void *out_buffer_ptr,
                                                        uint_fast32_t  out_cnt,
                                                        uint_fast32_t* out_offset_ptr,
                                                        void *in_buffer_ptr,
                                                        uint_fast32_t  in_cnt,
                                                        uint_fast32_t* in_offset_ptr);

/**
 \~english
 @brief start spi transfer. The best way to do this is to use DMA.
        Software implementation is the solution when DMA cannot be used
 @note When using DMA, you need to use DMA to receive data first,
       and then process DMA to send data.
 @param[in] spi_ptr: a pointer to structure @ref vsf_spi_t
 @param[in] out_buffer_ptr: a pointer to spi send data buffer, it can be a null pointer
 @param[in] in_buffer_ptr: a pointer to spi receive data buffer, it can be a null pointer
 @param[in] count: The number of data received. It is also the number of data sent
 @return vsf_err_t: VSF_ERR_NONE if spi transfer successfully, or a negative error code

 \~chinese
 @brief 启动 spi 传输
 @brief 启动 spi 传输. 最好的方式是使用 DMA 来完成，
        无法使用 DMA 完成的时候可以使用软件模拟完成
 @note 当使用 DMA 的时候，需要先使用 DMA 接收数据，然后再使用 DMA 发送数据
 @param[in] spi_ptr: @ref vsf_spi_t 传输结构指针
 @param[in] out_buffer_ptr: spi 发送数据的缓冲区, 它可以是空指针
 @param[in] in_buffer_ptr: spi 接收数据的缓冲区, 它可以是空指针
 @param[in] count: 接收数据的个数， 同时也是发送数据的个数
 @return vsf_err_t: 如果传输成功返回 VSF_ERR_NONE, 否则返回负数
 */
extern vsf_err_t vsf_spi_request_transfer(vsf_spi_t *spi_ptr, void *out_buffer_ptr,
                                          void *in_buffer_ptr, uint_fast32_t count);

extern vsf_err_t            vsf_spi_cancel_transfer(    vsf_spi_t *spi_ptr);
extern int_fast32_t         vsf_spi_get_transfered_count(vsf_spi_t *spi_ptr);



/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SPI_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_spi_t                          VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t)
#   define vsf_spi_init(__SPI, ...)             VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_init)                 ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_enable(__SPI)                VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_enable)               ((__vsf_spi_t *)__SPI)
#   define vsf_spi_disable(__SPI)               VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_disable)              ((__vsf_spi_t *)__SPI)
#   define vsf_spi_irq_enable(__SPI, ...)       VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_irq_enable)           ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_irq_disable(__SPI, ...)      VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_irq_disable)          ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_status(__SPI)                VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_status)               ((__vsf_spi_t *)__SPI)
#   define vsf_spi_capability(__SPI)            VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_capability)           ((__vsf_spi_t *)__SPI)
#   define vsf_spi_cs_active(__SPI, ...)        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cs_active)            ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_cs_inactive(__SPI, ...)      VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cs_inactive)          ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_fifo_transfer(__SPI, ...)    VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_fifo_transfer)        ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_request_transfer(__SPI, ...) VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_request_transfer)     ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_cancel_transfer(__SPI)       VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cancel_transfer)      ((__vsf_spi_t *)__SPI)
#   define vsf_spi_get_transfered_count(__SPI)  VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_get_transfered_count) ((__vsf_spi_t *)__SPI)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_SPI_INTERFACE_H__*/

