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

//! \brief define the INTC register page
#define F1CX00S_INTC                (*(intc_reg_t *)F1CX00S_INTC_BASE_ADDRESS)

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
    REG_RSVD_U32

    DEF_REG                             //!< NMI interrupt control
        reg32_t NMI_SRC_TYPE    : 2;    //!< External NMI Interrupt Source Type
        reg32_t                 : 30;
    END_DEF_REG32(NMI_INT_CTRL)           

    union {
        reg32_t PEND[2];                //!< Interrupt Pending Register 0/1
        reg32_t PENDING[2];             //!< Alias as interrupt Pending Status Register
    };
    REG_RSVD_U32
    REG_RSVD_U32

    union {
        reg32_t EN[2];                  //!< Interrupt Enable Register 0/1
        reg32_t SRC_MASK[2];            //!< Interrupt Source Mask Register 0/1                       
    };
    REG_RSVD_U32
    REG_RSVD_U32

    union {
        reg32_t MASK[2];                //!< Interrupt Mask Register 0/1
        reg32_t DISABLE[2];             //!< Alias as interrupt Disable register
    };
    REG_RSVD_U32
    REG_RSVD_U32

    reg32_t RESP[2];                    //!< Interrupt Response Register 0/1
    REG_RSVD_U32
    REG_RSVD_U32

    union {
        reg32_t FF[2];                  //!< Interrupt Fast Forcing Register 0/1
        reg32_t STIR[2];                //!< Alias as NVIC.STIR Software Trigger Interrupt Register
    };
    REG_RSVD_U32
    REG_RSVD_U32

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
            END_DEF_REG32(PRIO0)        

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
            END_DEF_REG32(PRIO1) 

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
            END_DEF_REG32(PRIO2)        

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
            END_DEF_REG32(PRIO3) 
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
