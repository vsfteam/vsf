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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SPI == ENABLED

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


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

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif

#endif
/* EOF */
