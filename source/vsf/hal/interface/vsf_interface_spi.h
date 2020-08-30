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

#ifndef __HAL_DRIVER_SPI_INTERFACE_H__
#define __HAL_DRIVER_SPI_INTERFACE_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "vsf_interface_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum em_spi_mode_t em_spi_mode_t;
/*
//! \name spi working mode 
//! @{
enum em_spi_mode_t {
    SPI_MODE_MASTER             = 0x00,             //!< select master mode
    SPI_MODE_SLAVE              = _BV(0),           //!< select slave mode
          
    SPI_MODE_FORMAT_SPI         = 0x00,             //!< the driver should at least support standard spi frame
    SPI_MODE_FORMAT_TI          = _BV(1),
    SPI_MODE_FORMAT_MICROWIRE   = _BV(2),
    
    //! polarity configure begin 
    SPI_MODE_CLK_IDLE_HIGH      = _BV(3),           //!< SCK is high in idle 
    SPI_MODE_CLK_IDLE_LOW       = 0x00,             //!< SCK is low in idle
    
    SPI_MODE_SAMP_FIRST_EDGE    = 0x00,             //!< sample at first edge of sck
    SPI_MODE_SAMP_SECOND_EDGE   = _BV(4),           //!< sample at second edge of sck
    //! polarity configure end     

    SPI_MODE_LOOP_BACK          = _BV(5),           //!< enable loop back
    SPI_MODE_NOT_LOOP_BACK      = 0x00,             //!< default disable loop back
    
    SPI_MODE_SLAVE_OUT_ENABLE   = 0x00,             //!< default enable slave output
    SPI_MODE_SLAVE_OUT_DISABLE  = _BV(6)            //!< disable slave output

    SPI_MODE_MSB                = 0x00,             //!< default enable MSB
    SPI_MODE_LSB                = _BV(7)            //!< transfer LSB first
};
//! @}
*/

/*
#define __SPI_CLK_DIV(__N,__Value)                          \
            SPI_PCLK_DIV_##__N = (2*(__N)),

//! \name enum spi clk div
//! @{
typedef enum{
   REPEAT_MACRO(127,__SPI_CLK_DIV ,NULL)
} em_spiclk_div_t;
//! @}
*/


/*
//! \name spi datasize 
//! @{
typedef enum {
    SPI_MODE_DATASIZE_4         = 0x3,              //!< datasize is 4 bits
    SPI_MODE_DATASIZE_5         = 0x4,              //!< datasize is 5 bits
    SPI_MODE_DATASIZE_6         = 0x5,              //!< datasize is 6 bits
    SPI_MODE_DATASIZE_7         = 0x6,              //!< datasize is 7 bits
    SPI_MODE_DATASIZE_8         = 0x7,              //!< datasize is 8 bits
    SPI_MODE_DATASIZE_9         = 0x8,              //!< datasize is 9 bits
    SPI_MODE_DATASIZE_10        = 0x9,              //!< datasize is 10 bits
    SPI_MODE_DATASIZE_11        = 0xA,              //!< datasize is 11 bits
    SPI_MODE_DATASIZE_12        = 0xB,              //!< datasize is 12 bits
    SPI_MODE_DATASIZE_13        = 0xC,              //!< datasize is 13 bits
    SPI_MODE_DATASIZE_14        = 0xD,              //!< datasize is 14 bits
    SPI_MODE_DATASIZE_15        = 0xE,              //!< datasize is 15 bits
    SPI_MODE_DATASIZE_16        = 0xF               //!< datasize is 16 bits
}em_spi_datasize_t;
//! @}
*/

//! \name spi configuration
//! @{
typedef struct spi_cfg_t spi_cfg_t;
struct spi_cfg_t {
    uint32_t            wMode;                      //!< spi working mode
    uint8_t             chDataSize;                 //!< frame size
    uint16_t            hwClockDiv;                 //!< P_CLK prescaler

#if VSF_HAL_CFG_USE_STREAM == ENABLED
    implement(vsf_stream_src_cfg_t)
    vsf_stream_pbuf_fifo_t *ptRXFIFO;
    vsf_stream_pbuf_fifo_t *ptTXFIFO;
#endif
};

//! @}

/* spi_status_t should implement peripheral_status_t */
typedef struct spi_status_t spi_status_t;

/* spi_capability_t should implement peripheral_capability_t */
typedef struct spi_capability_t spi_capability_t;

typedef struct vsf_spi_t vsf_spi_t;

typedef void vsf_spi_evt_handler_t(   void *target_ptr, 
                                        vsf_spi_t *,
                                        spi_status_t Status);

typedef struct vsf_spi_evt_t vsf_spi_evt_t;
struct vsf_spi_evt_t
{
    vsf_spi_evt_handler_t *handler_fn;
    void *target_ptr;
};

typedef enum {
    VSF_SPI_EVT_EXC               = _BV(0),
    VSF_SPI_EVT_EXC_BLK_CPL       = _BV(2),
} vsf_spi_evt_type_t;

//! the the target machine int
typedef uint_fast8_t spi_evt_status_t;

//! \name class: spi_t
//! @{
def_interface(i_spi_t)
    union {
        implement(peripheral_t);
        struct {
            spi_status_t     (*Status)(void);
            spi_capability_t (*Capability)(void);
        } SPI;
    };
    vsf_err_t       (*Init)   (spi_cfg_t *cfg_ptr);
    
    struct {
        /*! set the new polarity and return the old polarity */
        uintalu_t   (*Set)(uintalu_t tPolarity);
        /*! get current polarity */
        uintalu_t   (*Get)(void); 
    } Polarity;

    struct {
        uintalu_t (*Set)    (uintalu_t tMask);      /*!< set specified CS pin with bit mask*/
        uintalu_t (*Clear)  (uintalu_t tMask);      /*!< clear specified CS pin with bit mask*/
        uintalu_t (*Resume) (uintalu_t tMask);      /*!< resume previous status of cs pins*/
    } CS;

    
    //! data access
    fsm_rt_t (*SingleExchange)(uintalu_t tOutput, void *pInput);
    

    struct {

#if VSF_HAL_SPI_CFG_SUPPORT_UNEVEN_TRANSFER_SIZE == ENABLED
        /*! \brief request a block exchaging access
         *! \param pOutput      address of output buffer
         *! \param wOutputSize  the size of the output buffer
         *! \param pInput       address of input buffer
         *! \param wInputSize  the size of the input buffer
         *! \retval fsm_rt_cpl  The transaction is complete
         *! \retval fsm_rt_asyn The transaction is handled asynchronousely, i.e. by 
         *!                     DMA or by ISR or etc.
         *! \retval fsm_rt_on_going User should poll this API until fsm_rt_cpl or err
         *!                     value is returned.
         *! \retval vsf_err_t   Error value is returned.
         */

        fsm_rt_t  (*RequestExchangeEx)( void *pOutput, uint_fast32_t wOutputSize, 
                                        void *pInput, uint_fast32_t wInputSize);
#else
        /*! \brief request a block exchaging access
         *! \param pOutput      address of output buffer
         *! \param pInput       address of input buffer
         *! \param u32_size        the size of the two buffers
         *! \retval fsm_rt_cpl  The transaction is complete
         *! \retval fsm_rt_asyn The transaction is handled asynchronousely, i.e. by 
         *!                     DMA or by ISR or etc.
         *! \retval fsm_rt_on_going User should poll this API until fsm_rt_cpl or err
         *!                     value is returned.
         *! \retval vsf_err_t   Error value is returned.
         */

        fsm_rt_t  (*RequestExchange)( void *pOutput, void *pInput, uint_fast32_t u32_size);
#endif

        /*! \brief cancel on going communication */
        fsm_rt_t  (*Cancel) (void);

        /*! \brief get transfered count */
        intalu_t  (*GetTransferedCount)(void);
    } Block;

    //! event
    struct {
        void (*Register)(vsf_spi_evt_type_t tType, vsf_spi_evt_t event);
        spi_evt_status_t (*Enable)(spi_evt_status_t tEventMask);
        spi_evt_status_t (*Disable)(spi_evt_status_t tEventMask);
        void (*Resume)(spi_evt_status_t tEventStatus);
    }Event;

end_def_interface(i_spi_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_spi_t VSF_SPI[SPI_COUNT];

/*============================ PROTOTYPES ====================================*/



#endif
