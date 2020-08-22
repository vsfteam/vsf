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

/*! \note i_reg_xxxx.h header files must be standalong and assume following 
 *!       conditions: 
 *!       a. stdint.h exists
 *!       b. anonymouse structures and unions are supported
 */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

//! \brief SysTick timer register base address
#define SYSTICK_BASE_ADDRESS        (0xE000E010UL)

//! \brief define the System Tick Timer register page
#define ARM_SYSTICK                 (*(systick_reg_t *)SYSTICK_BASE_ADDRESS)

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

typedef volatile uint8_t            reg8_t;
typedef volatile uint16_t           reg16_t;
typedef volatile uint32_t           reg32_t;

#if defined(__IAR_SYSTEMS_ICC__)                                                \
    ||  (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L)

#undef ____RESERVED
#undef __RESERVED
#define ____RESERVED(__BIT, __NAME)                                             \
        uint##__BIT##_t __unused_##__NAME : __BIT;
#define __RESERVED(__BIT, __NAME)                                               \
            ____RESERVED(__BIT, __NAME)

#define __RESERVED_B(__BYTE_CNT, __LINE)                                        \
                                    uint32_t __unused_##__LINE[__BYTE_CNT >> 2]
#define RESERVED_B(__BYTE_CNT)      __RESERVED_B(__BYTE_CNT, __LINE__)

#   ifndef RESERVED_U8           
#       define RESERVED_U8          __RESERVED( 8, __LINE__ )
#   endif

#   ifndef RESERVED_U16            
#       define RESERVED_U16         __RESERVED( 16, __LINE__ )
#   endif

#   ifndef RESERVED_U32             
#       define RESERVED_U32         __RESERVED( 32, __LINE__ )
#   endif

#   ifndef RESERVED_16B             
#       define RESERVED_16B         RESERVED_B(16);
#   endif

#   ifndef RESERVED_64B             
#       define RESERVED_64B         RESERVED_B(64);
#   endif

#   ifndef RESERVED_256B             
#       define RESERVED_256B        RESERVED_B(256);
#   endif

#   ifndef RESERVED_1K             
#       define RESERVED_1K          RESERVED_B(1024);
#   endif

#   ifndef RESERVED_4K             
#       define RESERVED_4K          RESERVED_B(4096);
#   endif

#   ifndef RESERVED_16K             
#       define RESERVED_16K         RESERVED_B(16*1024);
#   endif

#   ifndef RESERVED_64K             
#       define RESERVED_64K         RESERVED_B(64*1024);
#   endif

#   ifndef RESERVED_256K             
#       define RESERVED_256K        RESERVED_B(256*1024);
#   endif


#   ifndef RESERVED_1M             
#       define RESERVED_1M          RESERVED_B(1024*1024);
#   endif

#else
#   ifndef RESERVED_U8           
#       define RESERVED_U8          uint8_t  : 8;
#   endif

#   ifndef RESERVED_U16            
#       define RESERVED_U16         uint16_t : 16;
#   endif

#   ifndef RESERVED_U32             
#       define RESERVED_U32         uint32_t : 32;
#   endif

#   ifndef RESERVED_16B             
#       define RESERVED_16B     RESERVED_U32                                    \
                                RESERVED_U32                                    \
                                RESERVED_U32                                    \
                                RESERVED_U32
#   endif

#   ifndef RESERVED_64B             
#       define RESERVED_64B     RESERVED_16B                                    \
                                RESERVED_16B                                    \
                                RESERVED_16B                                    \
                                RESERVED_16B
#   endif

#   ifndef RESERVED_256B             
#       define RESERVED_256B    RESERVED_64B                                    \
                                RESERVED_64B                                    \
                                RESERVED_64B                                    \
                                RESERVED_64B
#   endif

#   ifndef RESERVED_1K             
#       define RESERVED_1K      RESERVED_256B                                   \
                                RESERVED_256B                                   \
                                RESERVED_256B                                   \
                                RESERVED_256B
#   endif

#   ifndef RESERVED_4K             
#       define RESERVED_4K      RESERVED_1K                                     \
                                RESERVED_1K                                     \
                                RESERVED_1K                                     \
                                RESERVED_1K
#   endif

#   ifndef RESERVED_16K             
#       define RESERVED_16K     RESERVED_4K                                     \
                                RESERVED_4K                                     \
                                RESERVED_4K                                     \
                                RESERVED_4K
#   endif

#   ifndef RESERVED_64K             
#       define RESERVED_64K     RESERVED_16K                                    \
                                RESERVED_16K                                    \
                                RESERVED_16K                                    \
                                RESERVED_16K
#   endif

#   ifndef RESERVED_256K             
#       define RESERVED_256K    RESERVED_64K                                    \
                                RESERVED_64K                                    \
                                RESERVED_64K                                    \
                                RESERVED_64K
#   endif


#   ifndef RESERVED_1M             
#       define RESERVED_1M      RESERVED_256K                                   \
                                RESERVED_256K                                   \
                                RESERVED_256K                                   \
                                RESERVED_256K
#   endif

#endif

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

#ifdef __cplusplus
}
#endif

#endif