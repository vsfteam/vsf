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

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "utilities/vsf_utilities.h"
#include "./i_io_systick.h"
#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

/*! \note   By default, the driver tries to make all APIs thread-safe, in the
 *!         case when you want to disable it, please use following macro to
 *!         disable it:
 *!
 *!         #define __SYSTICK_ATOM_CODE(...)        \
 *!             for (int VSF_MCONNECT2(___, __LINE__) =1;VSF_MCONNECT2(___, __LINE__)--;)
 *!
 *!
 *!         NOTE: This macro should be defined in app_cfg.h or vsf_cfg.h
 */

#ifndef __SYSTICK_ATOM_CODE
#   define __SYSTICK_ATOM_CODE              vsf_interrupt_safe_simple
#endif

/*============================ TYPES =========================================*/

// TODO: should em_systick_cfg_mode_t and systick_cfg_t here be removed?
#if 0
//! \name SysTick initialization arguments definition
//! @{
typedef enum {
    //! Enable the SysTick
    DISABLE_SYSTICK                         = 0x00,
    //! Enable the SysTick
    ENABLE_SYSTICK                          = 0x01,

    //! Enable the SysTick interrupt
    DISABLE_SYSTICK_INTERRUPT               = 0x00,
    //! Enable the SysTick interrupt
    ENABLE_SYSTICK_INTERRUPT                = 0x02,

    //! Select the reference clock as SysTick source
    SYSTICK_SOURCE_REFERCLK                 = 0x00,
    //! Select the system clock as SysTick source
    SYSTICK_SOURCE_SYSCLK                   = 0x04,
} em_systick_cfg_mode_t;
//! @}

//! \name systick config struct
//! @{
typedef struct {
    uint8_t         mode;                           //!< config ward
    uint32_t        reload_value;                   //!< Match value
} systick_cfg_t;
//! @}
#endif
/*============================ PROTOTYPES ====================================*/
/*!\brief init SysTick Timer
 *! \param void
 *! \retval true : succeed
 *! \retval false: failed
 */
bool vsf_systick_init(systick_cfg_t *pCfg);

/*!\brief get count value
 *! \param void
 *! \retval return geh time count value
 */
uint32_t vsf_systick_get_count(void);

/*!\brief set reload value
 *! \param time count value
 *! \retval whether access successful or not
 */
bool vsf_systick_set_reload(uint32_t value);

/*!\brief clear count value
 *! \param none
 *! \retval none
 */
void vsf_systick_clear_count(void);
/*!\brief enable
 *! \param void
 *! \retval bool
 */
void vsf_systick_enable(void);

/*!\brief enable
 *! \param void
 *! \retval bool
 */
bool vsf_systick_disable(void);

/*!\brief enable
 *! \param void
 *! \retval bool
 */
bool vsf_systick_is_match(void);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static volatile uint32_t __systick_csr_buffer = 0;

/*============================ IMPLEMENTATION ================================*/

/*!\brief init SysTick Timer
 *! \param void
 *! \retval true : succeed
 *! \retval false: failed
 */
bool vsf_systick_init(systick_cfg_t *cfg_ptr)
{
    //! \brief check the input
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    vsf_systick_set_reload(cfg_ptr->reload_value);
    vsf_systick_clear_count();

    __SYSTICK_ATOM_CODE(
        __systick_csr_buffer = cfg_ptr->mode;
        SYSTICK_CSR = cfg_ptr->mode;
    )

    return true;
}

/*!\brief enable
 *! \param void
 *! \retval none
 */
void vsf_systick_enable(void)
{
    __SYSTICK_ATOM_CODE(
        __systick_csr_buffer |= ENABLE_SYSTICK;
        SYSTICK_CSR = __systick_csr_buffer;
    )
}
/*!\brief enable
 *! \param void
 *! \retval bool
 */
bool vsf_systick_disable(void)
{
    uint_fast32_t temp;
    __SYSTICK_ATOM_CODE(
        __systick_csr_buffer &= ~ENABLE_SYSTICK;
        SYSTICK_CSR = __systick_csr_buffer;
        temp = SYSTICK_CSR;             //! read to clear COUNTFLAG
    )
    return temp & SYSTICK_CSR_COUNTFLAG_MSK;
}



/*!\brief enable
 *! \param void
 *! \retval none
 */
void vsf_systick_int_enable(void)
{
    __SYSTICK_ATOM_CODE(
        __systick_csr_buffer |= ENABLE_SYSTICK_INTERRUPT;
        SYSTICK_CSR = __systick_csr_buffer;
    )
}
/*!\brief enable
 *! \param void
 *! \retval bool
 */
void vsf_systick_int_disable(void)
{
    __SYSTICK_ATOM_CODE(
        __systick_csr_buffer &= ~ENABLE_SYSTICK_INTERRUPT;
        SYSTICK_CSR = __systick_csr_buffer;
    )
}

/*! \brief check compare match event
 *! \param none
 *! \retval true compare matched, the flag has been cleared automatically
 *! \retval false no compare match event
 */
bool vsf_systick_is_match(void)
{
    return SYSTICK_CSR & SYSTICK_CSR_COUNTFLAG_MSK;
}


/*!\brief get count value
 *! \param void
 *! \retval return geh time count value
 */
uint32_t vsf_systick_get_count(void)
{
    return SYSTICK_CVR;
}

/*!\brief set reload value
 *! \param time count value
 *! \retval none
 */
bool vsf_systick_set_reload(uint32_t value)
{
    SYSTICK_RVR = value;
    return true;
}

/*!\brief clear count value
 *! \param none
 *! \retval none
 */
void vsf_systick_clear_count(void)
{
    SYSTICK_CVR = 0;
}
