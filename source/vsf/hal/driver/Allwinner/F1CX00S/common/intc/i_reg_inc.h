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

#ifndef __I_REG_INC_H__
#define __I_REG_INC_H__

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

//! \brief define the INTC register page
#define F1CX00S_INTC                (*(intc_reg_t *)F1CX00S_INTC_BASE_ADDRESS)

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

/* ----------------------------REGISTER DEFINE  END-------------------------- */
//! @}

//! \name DEFINE REGISTER MACROS
//! @{
/* ----------------------- DEFINE REGISTER MACROS BETIN  -------------------- */


/* ----------------------- DEFINE REGISTER MACROS  END   -------------------- */
//! @}



/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct intc_reg_t {
    reg32_t VECTOR;                     //!< interrupt vector register

    union {
        reg32_t BASE_ADDR;              //!< vector table base address
        reg32_t VTOR;                   //!< alias as NVIC.VTOR
    };
    RESERVED_U32

    DEF_REG                             //!< NMI interrupt control
        reg32_t NMI_SRC_TYPE    : 2;    //!< External NMI Interrupt Source Type
        reg32_t                 : 30;
    END_DEF_REG(NMI_INT_CTRL)           

    union {
        reg32_t PEND[2];                //!< Interrupt Pending Register 0/1
        reg32_t PENDING[2];             //!< Alias as interrupt Pending Status Register
    };
    RESERVED_U32
    RESERVED_U32

    union {
        reg32_t EN[2];                  //!< Interrupt Enable Register 0/1
        reg32_t SRC_MASK[2];            //!< Interrupt Source Mask Register 0/1                       
    };
    RESERVED_U32
    RESERVED_U32

    union {
        reg32_t MASK[2];                //!< Interrupt Mask Register 0/1
        reg32_t ENABLE[2];              //!< Alias as interrupt Enable register
    };
    RESERVED_U32
    RESERVED_U32

    reg32_t RESP[2];                    //!< Interrupt Response Register 0/1
    RESERVED_U32
    RESERVED_U32

    union {
        reg32_t FF[2];                  //!< Interrupt Fast Forcing Register 0/1
        reg32_t STIR[2];                //!< Alias as NVIC.STIR Software Trigger Interrupt Register
    };
    RESERVED_U32
    RESERVED_U32

    union {
        struct {
            DEF_REG                     //!< Interrupt Source Priority Register 0
                reg32_t IRQ00_PRIO      : 2;
                reg32_t IRQ01_PRIO      : 2;
                reg32_t IRQ02_PRIO      : 2;
                reg32_t IRQ03_PRIO      : 2;
                reg32_t IRQ04_PRIO      : 2;
                reg32_t IRQ05_PRIO      : 2;
                reg32_t IRQ06_PRIO      : 2;
                reg32_t IRQ07_PRIO      : 2;
                reg32_t IRQ08_PRIO      : 2;
                reg32_t IRQ09_PRIO      : 2;
                reg32_t IRQ10_PRIO      : 2;
                reg32_t IRQ11_PRIO      : 2;
                reg32_t IRQ12_PRIO      : 2;
                reg32_t IRQ13_PRIO      : 2;
                reg32_t IRQ14_PRIO      : 2;
                reg32_t IRQ15_PRIO      : 2;
            END_DEF_REG(PRIO0)        

            DEF_REG                     //!< Interrupt Source Priority Register 1
                reg32_t IRQ16_PRIO      : 2;
                reg32_t IRQ17_PRIO      : 2;
                reg32_t IRQ18_PRIO      : 2;
                reg32_t IRQ19_PRIO      : 2;
                reg32_t IRQ20_PRIO      : 2;
                reg32_t IRQ21_PRIO      : 2;
                reg32_t IRQ22_PRIO      : 2;
                reg32_t IRQ23_PRIO      : 2;
                reg32_t IRQ24_PRIO      : 2;
                reg32_t IRQ25_PRIO      : 2;
                reg32_t IRQ26_PRIO      : 2;
                reg32_t IRQ27_PRIO      : 2;
                reg32_t IRQ28_PRIO      : 2;
                reg32_t IRQ29_PRIO      : 2;
                reg32_t IRQ30_PRIO      : 2;
                reg32_t IRQ31_PRIO      : 2;
            END_DEF_REG(PRIO1) 

            DEF_REG                     //!< Interrupt Source Priority Register 2
                reg32_t IRQ32_PRIO      : 2;
                reg32_t IRQ33_PRIO      : 2;
                reg32_t IRQ34_PRIO      : 2;
                reg32_t IRQ35_PRIO      : 2;
                reg32_t IRQ36_PRIO      : 2;
                reg32_t IRQ37_PRIO      : 2;
                reg32_t IRQ38_PRIO      : 2;
                reg32_t IRQ39_PRIO      : 2;
                reg32_t IRQ40_PRIO      : 2;
                reg32_t IRQ41_PRIO      : 2;
                reg32_t IRQ42_PRIO      : 2;
                reg32_t IRQ43_PRIO      : 2;
                reg32_t IRQ44_PRIO      : 2;
                reg32_t IRQ45_PRIO      : 2;
                reg32_t IRQ46_PRIO      : 2;
                reg32_t IRQ47_PRIO      : 2;
            END_DEF_REG(PRIO2)        

            DEF_REG                     //!< Interrupt Source Priority Register 3
                reg32_t IRQ48_PRIO      : 2;
                reg32_t IRQ49_PRIO      : 2;
                reg32_t IRQ50_PRIO      : 2;
                reg32_t IRQ51_PRIO      : 2;
                reg32_t IRQ52_PRIO      : 2;
                reg32_t IRQ53_PRIO      : 2;
                reg32_t IRQ54_PRIO      : 2;
                reg32_t IRQ55_PRIO      : 2;
                reg32_t IRQ56_PRIO      : 2;
                reg32_t IRQ57_PRIO      : 2;
                reg32_t IRQ58_PRIO      : 2;
                reg32_t IRQ59_PRIO      : 2;
                reg32_t IRQ60_PRIO      : 2;
                reg32_t IRQ61_PRIO      : 2;
                reg32_t IRQ62_PRIO      : 2;
                reg32_t IRQ63_PRIO      : 2;
            END_DEF_REG(PRIO3) 
        };
        
        reg32_t PRIO[4];
    };
} intc_reg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
