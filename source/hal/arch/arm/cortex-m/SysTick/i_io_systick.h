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
#define SYSTICK_BASE_ADDRESS            (0xE000E010UL)

//! \brief define the System Tick Timer register page
#define ARM_SYSTICK                     (*(systick_reg_t *)SYSTICK_BASE_ADDRESS)

#ifndef __REG_MACRO__
#define __REG_MACRO__
#define DEF_REG                                                                 \
        union {                                                                 \
            struct {
    
#define END_DEF_REG32(__NAME)                                                   \
            };                                                                  \
            reg32_t VALUE;                                                      \
        }__NAME;
#endif

//! \name REGISTER DEFIEN
//! @{
/* ---------------------------REGISTER DEFINE BEGIN ------------------------- */


//! \brief  SysTick Timer Control and status register
#define SYSTICK_CSR_REG                 ARM_SYSTICK.SYST_CSR
#define SYSTICK_CSR                     SYSTICK_CSR_REG.VALUE
#define SYSTICK_CSR_BIT                 SYSTICK_CSR_REG

//! \brief System Timer Reload value register
#define SYSTICK_RVR_REG                 ARM_SYSTICK.SYST_RVR
#define SYSTICK_RVR                     SYSTICK_RVR_REG.VALUE
#define SYSTICK_RVR_BIT                 SYSTICK_RVR_REG

                 
//! \brief System Timer Current value register
#define SYSTICK_CVR_REG                 ARM_SYSTICK.SYST_CVR
#define SYSTICK_CVR                     SYSTICK_CVR_REG.VALUE
#define SYSTICK_CVR_BIT                 SYSTICK_CVR_REG

//! \brief System Timer Calibration value register
#define SYSTICK_CALIB_REG               ARM_SYSTICK.SYST_CALIB
#define SYSTICK_CALIB                   SYSTICK_CALIB_REG.VALUE
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

// copied from utilities/compiler/__common/__type.h
#ifndef __REG_TYPE__
#define __REG_TYPE__

typedef volatile uint8_t            reg8_t;
typedef volatile uint16_t           reg16_t;
typedef volatile uint32_t           reg32_t;

#if defined(__IAR_SYSTEMS_ICC__)                                                \
    ||  (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L)
#   define __REG_CONNECT(__A, __B)  __A##__B
#   define __REG_RSVD_NAME(__NAME)  __REG_CONNECT(__unused_, __NAME)
#else
#   define __REG_RSVD_NAME(__NAME)
#endif

#define ____REG_RSVD(__NAME, __BIT)                                             \
        reg##__BIT##_t              __NAME : __BIT;
#define ____REG_RSVD_N(__NAME, __BIT, __N)                                      \
        reg##__BIT##_t              __NAME[__N];
#define __REG_RSVD(__BIT)           ____REG_RSVD(REG_RSVD_NAME, __BIT)
#define __REG_RSVD_N(__BIT, __N)    ____REG_RSVD_N(REG_RSVD_NAME, __BIT, (__N))

#define REG_RSVD_NAME               __REG_RSVD_NAME(__LINE__)
#define REG_RSVD(__BIT)             __REG_RSVD(__BIT)
#define REG_RSVD_N(__BIT, __N)      __REG_RSVD_N(__BIT, (__N))

#define REG_RSVD_U8                 REG_RSVD(8)
#define REG_RSVD_U16                REG_RSVD(16)
#define REG_RSVD_U32                REG_RSVD(32)

#define REG_RSVD_U8N(__N)           REG_RSVD_N(8, (__N))
#define REG_RSVD_U16N(__N)          REG_RSVD_N(16, (__N))
#define REG_RSVD_U32N(__N)          REG_RSVD_N(32, (__N))

#define REG8_RSVD_N(__N)            REG_RSVD_U8N(__N)
#define REG8_RSVD_B(__BYTE_CNT)     REG8_RSVD_N(__BYTE_CNT)
#define REG8_RSVD_8B                REG8_RSVD_B(8)
#define REG8_RSVD_16B               REG8_RSVD_B(16)
#define REG8_RSVD_32B               REG8_RSVD_B(32)
#define REG8_RSVD_64B               REG8_RSVD_B(64)
#define REG8_RSVD_128B              REG8_RSVD_B(128)
#define REG8_RSVD_256B              REG8_RSVD_B(256)
#define REG8_RSVD_512B              REG8_RSVD_B(512)
#define REG8_RSVD_1K                REG8_RSVD_B(1024)
#define REG8_RSVD_2K                REG8_RSVD_B(2048)
#define REG8_RSVD_4K                REG8_RSVD_B(4096)
#define REG8_RSVD_8K                REG8_RSVD_B(8192)
#define REG8_RSVD_16K               REG8_RSVD_B(16 * 1024)
#define REG8_RSVD_32K               REG8_RSVD_B(32 * 1024)
#define REG8_RSVD_64K               REG8_RSVD_B(64 * 1024)
#define REG8_RSVD_128K              REG8_RSVD_B(128 * 1024)
#define REG8_RSVD_256K              REG8_RSVD_B(256 * 1024)
#define REG8_RSVD_512K              REG8_RSVD_B(512 * 1024)
#define REG8_RSVD_1M                REG8_RSVD_B(1024 * 1024)

#define REG16_RSVD_N(__N)           REG_RSVD_U16N(__N)
// __BYTE_CNT MUST be mutiple of 2
#define REG16_RSVD_B(__BYTE_CNT)    REG16_RSVD_N(__BYTE_CNT >> 1)
#define REG16_RSVD_8B               REG16_RSVD_B(8)
#define REG16_RSVD_16B              REG16_RSVD_B(16)
#define REG16_RSVD_32B              REG16_RSVD_B(32)
#define REG16_RSVD_64B              REG16_RSVD_B(64)
#define REG16_RSVD_128B             REG16_RSVD_B(128)
#define REG16_RSVD_256B             REG16_RSVD_B(256)
#define REG16_RSVD_512B             REG16_RSVD_B(512)
#define REG16_RSVD_1K               REG16_RSVD_B(1024)
#define REG16_RSVD_2K               REG16_RSVD_B(2048)
#define REG16_RSVD_4K               REG16_RSVD_B(4096)
#define REG16_RSVD_8K               REG16_RSVD_B(8192)
#define REG16_RSVD_16K              REG16_RSVD_B(16 * 1024)
#define REG16_RSVD_32K              REG16_RSVD_B(32 * 1024)
#define REG16_RSVD_64K              REG16_RSVD_B(64 * 1024)
#define REG16_RSVD_128K             REG16_RSVD_B(128 * 1024)
#define REG16_RSVD_256K             REG16_RSVD_B(256 * 1024)
#define REG16_RSVD_512K             REG16_RSVD_B(512 * 1024)
#define REG16_RSVD_1M               REG16_RSVD_B(1024 * 1024)

#define REG32_RSVD_N(__N)           REG_RSVD_U32N(__N)
// __BYTE_CNT MUST be mutiple of 4
#define REG32_RSVD_B(__BYTE_CNT)    REG_RSVD_U32N(__BYTE_CNT >> 2)
#define REG32_RSVD_8B               REG32_RSVD_B(8)
#define REG32_RSVD_16B              REG32_RSVD_B(16)
#define REG32_RSVD_32B              REG32_RSVD_B(32)
#define REG32_RSVD_64B              REG32_RSVD_B(64)
#define REG32_RSVD_128B             REG32_RSVD_B(128)
#define REG32_RSVD_256B             REG32_RSVD_B(256)
#define REG32_RSVD_512B             REG32_RSVD_B(512)
#define REG32_RSVD_1K               REG32_RSVD_B(1024)
#define REG32_RSVD_2K               REG32_RSVD_B(2048)
#define REG32_RSVD_4K               REG32_RSVD_B(4096)
#define REG32_RSVD_8K               REG32_RSVD_B(8192)
#define REG32_RSVD_16K              REG32_RSVD_B(16 * 1024)
#define REG32_RSVD_32K              REG32_RSVD_B(32 * 1024)
#define REG32_RSVD_64K              REG32_RSVD_B(64 * 1024)
#define REG32_RSVD_128K             REG32_RSVD_B(128 * 1024)
#define REG32_RSVD_256K             REG32_RSVD_B(256 * 1024)
#define REG32_RSVD_512K             REG32_RSVD_B(512 * 1024)
#define REG32_RSVD_1M               REG32_RSVD_B(1024 * 1024)

#endif      // __REG_TYPE__

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
    END_DEF_REG32(SYST_CSR) 

    //! \brief System Timer Reload value register
    DEF_REG
        reg32_t  RELOAD                 :24;
        reg32_t                         : 8;
    END_DEF_REG32(SYST_RVR) 

    //! \brief System Timer Current value register
    DEF_REG
        reg32_t CURRENT                 :24;
        reg32_t                         : 8;
    END_DEF_REG32(SYST_CVR) 

    //! \brief System Timer Calibration value register
    DEF_REG
        reg32_t TENMS                   :24;
        reg32_t                         : 6;
        reg32_t SKEW                    : 1;
        reg32_t NOREF                   : 1;
    END_DEF_REG32(SYST_CALIB) 
} systick_reg_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif