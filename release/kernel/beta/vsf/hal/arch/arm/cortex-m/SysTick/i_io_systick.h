/*****************************************************************************
 *   Copyright(C)2009-2018 by Gorgon Meducer<Embedded_zhuoran@hotmail.com>   *
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

#ifndef __I_IO_SYS_TICK_H__
#define __I_IO_SYS_TICK_H__

/*============================ INCLUDES ======================================*/
//! \brief SysTick timer register base address
#define SYSTICK_BASE_ADDRESS        (0xE000E010UL)

//! \brief define the System Tick Timer register page
#define ARM_SYSTICK                 (*(systick_reg_t *)SYSTICK_BASE_ADDRESS)
/*============================ MACROS ========================================*/

#ifndef __REG_MACRO__
#define __REG_MACRO__
#define DEF_REG                     \
        union {                     \
            struct {
    
#define END_DEF_REG(__NAME)         \
            };                      \
            reg32_t Value;          \
        }__NAME;
#endif

//! \name REGISTER DEFIEN
//! @{
/* ---------------------------REGISTER DEFINE BEGIN ------------------------- */


//! \brief  SysTick Timer Control and status register
#define SYSTICK_CSR_REG                 ARM_SYSTICK.SYST_CSR
#define SYSTICK_CSR                     SYSTICK_CSR_REG.Value
#define SYSTICK_CSR_BIT                 SYSTICK_CSR_REG

//! \brief System Timer Reload value register
#define SYSTICK_RVR_REG                 ARM_SYSTICK.SYST_RVR
#define SYSTICK_RVR                     SYSTICK_RVR_REG.Value
#define SYSTICK_RVR_BIT                 SYSTICK_RVR_REG

                 
//! \brief System Timer Current value register
#define SYSTICK_CVR_REG                 ARM_SYSTICK.SYST_CVR
#define SYSTICK_CVR                     SYSTICK_CVR_REG.Value
#define SYSTICK_CVR_BIT                 SYSTICK_CVR_REG

//! \brief System Timer Calibration value register
#define SYSTICK_CALIB_REG               ARM_SYSTICK.SYST_CALIB
#define SYSTICK_CALIB                   SYSTICK_CALIB_REG.Value
#define SYSTICK_CALIB_BIT               SYSTICK_CALIB_REG

/* ----------------------------REGISTER DEFINE  END-------------------------- */
//! @}

//! \name DEFINE REGISTER OPERATION MACRO
//! @{
/* -----------------------DEFINE REGISTER OP MAC  BETIN---------------------- */

//! \brief SysTick Timer Control and status register
#define SYSTICK_CSR_ENABLE              0
#define SYSTICK_CSR_ENABLE_MSK          (1<<SYSTICK_CSR_ENABLE)

#define SYSTICK_CSR_TICKINT             1
#define SYSTICK_CSR_TICKINT_MSK         (1<<SYSTICK_CSR_TICKINT)

#define SYSTICK_CSR_CLKSOURCE           2
#define SYSTICK_CSR_CLKSOURCE_MSK       (1<<SYSTICK_CSR_CLKSOURCE)

#define SYSTICK_CSR_COUNTFLAG           16
#define SYSTICK_CSR_COUNTFLAG_MSK       (1<<SYSTICK_CSR_COUNTFLAG)

//! \brief System Timer Calibration value register
#define SYSTICK_CALIB_SKEW              30
#define SYSTICK_CALIB_SKEW_MSK          (1<<SYSTICK_CALIB_SKEW)

#define SYSTICK_CALIB_NOREF             31
#define SYSTICK_CALIB_NOREF_MSK         (1<<SYSTICK_CALIB_NOREF)

/* --------------------------DEFINE REGISTER OP MAC   END-------------------- */
//! @}

/*============================ MACROFIED FUNCTIONS ===========================*/

/*============================ TYPES =========================================*/
#ifndef __REG_TYPE__
#define __REG_TYPE__

typedef volatile uint8_t                reg8_t;
typedef volatile uint16_t               reg16_t;
typedef volatile uint32_t               reg32_t;

#endif

//! \name register page type
//! @{
typedef struct {
    //! \brief SysTick Timer Control and status register
    DEF_REG
        reg32_t  EN                     : 1;
        reg32_t  TICKINT                : 1;
        reg32_t  CLKSOURCE              : 1;
        reg32_t                         :13;
        reg32_t  COUNTFLAG              : 1;
        reg32_t                         :15;
    END_DEF_REG(SYST_CSR) 

    //! \brief System Timer Reload value register
    DEF_REG
        reg32_t  RELOAD                 :24;
        reg32_t                         : 8;
    END_DEF_REG(SYST_RVR) 

    //! \brief System Timer Current value register
    DEF_REG
        reg32_t CURRENT                 :24;
        reg32_t                         : 8;
    END_DEF_REG(SYST_CVR) 

    //! \brief System Timer Calibration value register
    DEF_REG
        reg32_t TENMS                   :24;
        reg32_t                         : 6;
        reg32_t SKEW                    : 1;
        reg32_t NOREF                   : 1;
    END_DEF_REG(SYST_CALIB) 
} systick_reg_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif