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
#   define VSF_SPI_CFG_MULTI_CLASS                      ENABLED
#endif

// application code can redefine it
#ifndef VSF_SPI_CFG_PREFIX
#   if VSF_SPI_CFG_MULTI_CLASS == ENABLED
#       define VSF_SPI_CFG_PREFIX                       vsf
#   elif defined(VSF_HW_SPI_COUNT) && (VSF_HW_SPI_COUNT != 0)
#       define VSF_SPI_CFG_PREFIX                       vsf_hw
#   else
#       define VSF_SPI_CFG_PREFIX                       vsf
#   endif
#endif

#ifndef VSF_SPI_CFG_FUNCTION_RENAME
#   define VSF_SPI_CFG_FUNCTION_RENAME                  ENABLED
#endif

#ifndef VSF_SPI_CFG_MULTIPLEX_CS
#   define VSF_SPI_CFG_MULTIPLEX_CS                     DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE            DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK        DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_SPI_CFG_REIMPLEMENT_TYPE_STATUS          DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS
#   define VSF_SPI_CFG_REIMPLEMENT_MODE_TO_DATA_BITS    DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_MODE
#   define VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_MODE    DISABLED
#endif

#ifndef VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_BYTES
#   define VSF_SPI_CFG_REIMPLEMENT_DATA_BITS_TO_BYTES   DISABLED
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
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 spi, get_transferred_count, VSF_MCONNECT(__prefix_name, _spi_t) *spi_ptr, uint_fast32_t * tx_count, uint_fast32_t *rx_count)


/*============================ TYPES =========================================*/

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
//! spi working mode
typedef enum vsf_spi_mode_t {
    VSF_SPI_MASTER                  = 0x00ul << 21,      //!< select master mode
    VSF_SPI_SLAVE                   = 0x01ul << 21,      //!< select slave mode
    VSF_SPI_DIR_MODE_MASK           = VSF_SPI_MASTER  |
                                      VSF_SPI_SLAVE,

    VSF_SPI_MSB_FIRST               = 0x00ul << 22,      //!< default enable MSB
    VSF_SPI_LSB_FIRST               = 0x01ul << 22,      //!< transfer LSB first
    VSF_SPI_BIT_ORDER_MASK          = VSF_SPI_MSB_FIRST |
                                      VSF_SPI_LSB_FIRST,

    VSF_SPI_CPOL_LOW                = 0x00ul << 23,      //!< SCK clock polarity is low
    VSF_SPI_CPOL_HIGH               = 0x01ul << 23,      //!< SCK clock polarity is high
    VSF_SPI_CPHA_LOW                = 0x00ul << 24,      //!< SCK clock phase is low
    VSF_SPI_CPHA_HIGH               = 0x01ul << 24,      //!< SCK clock phase is high
    VSF_SPI_CLOCK_MODE_0            = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_LOW,
    VSF_SPI_CLOCK_MODE_1            = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_HIGH,
    VSF_SPI_CLOCK_MODE_2            = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_LOW,
    VSF_SPI_CLOCK_MODE_3            = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_HIGH,
    VSF_SPI_CLOCK_MODE_MASK         = VSF_SPI_CLOCK_MODE_0 |
                                      VSF_SPI_CLOCK_MODE_1 |
                                      VSF_SPI_CLOCK_MODE_2 |
                                      VSF_SPI_CLOCK_MODE_3,

    //!< datasize is 8 bits
    VSF_SPI_DATASIZE_BIT_OFFSET     = 25,
    VSF_SPI_DATASIZE_DIFF           = 1,
    VSF_SPI_DATASIZE_4              = ( 4ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_5              = ( 5ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_6              = ( 6ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_7              = ( 7ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_8              = ( 8ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_9              = ( 9ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_10             = (10ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_11             = (11ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_12             = (12ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_13             = (13ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_14             = (14ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_15             = (15ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_16             = (16ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_17             = (17ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_18             = (18ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_19             = (19ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_20             = (20ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_21             = (21ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_22             = (22ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_23             = (23ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_24             = (24ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_25             = (25ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_26             = (26ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_27             = (27ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_28             = (28ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_29             = (29ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_30             = (30ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_31             = (31ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_32             = (32ul - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET,
    VSF_SPI_DATASIZE_MASK           = VSF_SPI_DATASIZE_4  | VSF_SPI_DATASIZE_5  | VSF_SPI_DATASIZE_6  | VSF_SPI_DATASIZE_7  |
                                      VSF_SPI_DATASIZE_8  | VSF_SPI_DATASIZE_9  | VSF_SPI_DATASIZE_10 | VSF_SPI_DATASIZE_11 |
                                      VSF_SPI_DATASIZE_12 | VSF_SPI_DATASIZE_13 | VSF_SPI_DATASIZE_14 | VSF_SPI_DATASIZE_15 |
                                      VSF_SPI_DATASIZE_16 | VSF_SPI_DATASIZE_17 | VSF_SPI_DATASIZE_18 | VSF_SPI_DATASIZE_19 |
                                      VSF_SPI_DATASIZE_20 | VSF_SPI_DATASIZE_21 | VSF_SPI_DATASIZE_22 | VSF_SPI_DATASIZE_23 |
                                      VSF_SPI_DATASIZE_24 | VSF_SPI_DATASIZE_25 | VSF_SPI_DATASIZE_26 | VSF_SPI_DATASIZE_27 |
                                      VSF_SPI_DATASIZE_28 | VSF_SPI_DATASIZE_29 | VSF_SPI_DATASIZE_30 | VSF_SPI_DATASIZE_31 |
                                      VSF_SPI_DATASIZE_32,


    VSF_SPI_AUTO_CS_DISABLE         = 0x00ul << 30,
    VSF_SPI_AUTO_CS_ENABLE          = 0x01ul << 30,
    VSF_SPI_AUTO_CS_MASK            = VSF_SPI_AUTO_CS_DISABLE |
                                      VSF_SPI_AUTO_CS_ENABLE,

    VSF_SPI_LOOP_BACK               = 0x01ul << 31,     //!< enable loop back

    VSF_SPI_MODE_ALL_BITS_MASK      = VSF_SPI_DIR_MODE_MASK |
                                      VSF_SPI_BIT_ORDER_MASK |
                                      VSF_SPI_CLOCK_MODE_MASK |
                                      VSF_SPI_DIR_MODE_MASK |
                                      VSF_SPI_AUTO_CS_MASK |
                                      VSF_SPI_LOOP_BACK,
} vsf_spi_mode_t;
#endif

#if VSF_SPI_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
/*! \brief vsf_spi_irq_mask_t
 *! \note vsf_spi_irq_mask_t should provide irq masks
 */
typedef enum vsf_spi_irq_mask_t {
    // TX/RX reach fifo threshold, threshold on some devices is bound to 1
    VSF_SPI_IRQ_MASK_TX             = 0x01ul << 0,
    VSF_SPI_IRQ_MASK_RX             = 0x01ul << 1,

    // request_rx/request_tx complete
    VSF_SPI_IRQ_MASK_TX_CPL         = 0x01ul << 2,
    VSF_SPI_IRQ_MASK_CPL            = 0x01ul << 3,

    VSF_SPI_IRQ_ALL_BITS_MASK       = VSF_SPI_IRQ_MASK_TX |
                                      VSF_SPI_IRQ_MASK_RX |
                                      VSF_SPI_IRQ_MASK_TX_CPL |
                                      VSF_SPI_IRQ_MASK_CPL,
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
    uint8_t is_auto_cs : 1; // some hardware support
    uint8_t cs_count   : 7;
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
    uint32_t         clock_hz;
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
extern void                 vsf_spi_get_transferred_count(vsf_spi_t *spi_ptr, uint_fast32_t * tx_count, uint_fast32_t *rx_count);


/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SPI_CFG_REIMPLEMENT_MODE_TO_BITS == ENABLD
static inline uint8_t vsf_spi_mode_to_data_bits(vsf_spi_mode_t mode)
{
    int bits = (mode & VSF_SPI_DATASIZE_MASK) >> VSF_SPI_DATASIZE_BIT_OFFSET;
    return bits + VSF_SPI_DATASIZE_DIFF;
}
#endif

#if VSF_SPI_CFG_REIMPLEMENT_BITS_TO_MODE == ENABLD
static inline vsf_spi_mode_t vsf_spi_data_bits_to_mode(uint8_t bits)
{
    return (vsf_spi_mode_t)((bits - VSF_SPI_DATASIZE_DIFF) << VSF_SPI_DATASIZE_BIT_OFFSET);
}
#endif

#if VSF_SPI_CFG_REIMPLEMENT_MODE_TO_BYTES == ENABLD
static inline uint8_t vsf_spi_mode_to_data_bytes(vsf_spi_mode_t mode)
{
    int bits = (mode & VSF_SPI_DATASIZE_MASK);
    if (bits <= VSF_SPI_DATASIZE_8) {
        return 1;
    } else if (bits <= VSF_SPI_DATASIZE_16) {
        return 2;
    } else {
        return 4;
    }
}
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SPI_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_spi_t                                  VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_t)
#   define vsf_spi_init(__SPI, ...)                     VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_init)                 ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_enable(__SPI)                        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_enable)               ((__vsf_spi_t *)__SPI)
#   define vsf_spi_disable(__SPI)                       VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_disable)              ((__vsf_spi_t *)__SPI)
#   define vsf_spi_irq_enable(__SPI, ...)               VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_irq_enable)           ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_irq_disable(__SPI, ...)              VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_irq_disable)          ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_status(__SPI)                        VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_status)               ((__vsf_spi_t *)__SPI)
#   define vsf_spi_capability(__SPI)                    VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_capability)           ((__vsf_spi_t *)__SPI)
#   define vsf_spi_cs_active(__SPI, ...)                VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cs_active)            ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_cs_inactive(__SPI, ...)              VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cs_inactive)          ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_fifo_transfer(__SPI, ...)            VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_fifo_transfer)        ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_request_transfer(__SPI, ...)         VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_request_transfer)     ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#   define vsf_spi_cancel_transfer(__SPI)               VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_cancel_transfer)      ((__vsf_spi_t *)__SPI)
#   define vsf_spi_get_transferred_count(__SPI, ...)     VSF_MCONNECT(VSF_SPI_CFG_PREFIX, _spi_get_transferred_count) ((__vsf_spi_t *)__SPI, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_SPI_INTERFACE_H__*/

