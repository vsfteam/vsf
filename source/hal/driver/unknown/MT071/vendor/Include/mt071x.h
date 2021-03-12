/**
  ******************************************************************************
  * @file    mt071x.h
  * @author  MIC Software Team
  * @version V1.0.0
  * @date    03/01/2018
  * @brief   CMSIS Cortex-M0 Device Peripheral Access Layer Header File. 
  *          This file contains all the peripheral register's definitions, bits 
  *          definitions and memory mapping for MT071x devices.  
  *          
  *          The file is the unique include file that the application programmer
  *          is using in the C source code, usually in main.c. This file contains:
  *           - Configuration section that allows to select:
  *              - The device used in the target application
  *              - To use or not the peripheral drivers in application code(i.e. 
  *                code will be based on direct access to peripheral registers 
  *                rather than drivers API), this option is controlled by 
  *                "#define USE_STDPERIPH_DRIVER"
  *              - To change few application-specific parameters such as the HSE 
  *                crystal frequency
  *           - Data structures and the address mapping for all peripherals
  *           - Peripheral's registers declarations and bits definition
  *           - Macros to access peripheral registers hardware
  *
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MIC SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2018 MIC</center></h2>
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup MIC
  * @{
  */

#ifndef __MT071X_H
#define __MT071X_H

#ifdef __cplusplus
 extern "C" {
#endif 

/** @addtogroup Library_configuration_section
  * @{
  */


#if !defined  USE_STDPERIPH_DRIVER
/**
 * @brief Comment the line below if you will not use the peripherals drivers.
   In this case, these drivers will not be included and the application code will 
   be based on direct access to peripherals registers 
   */
  /*#define USE_STDPERIPH_DRIVER*/
#endif /* USE_STDPERIPH_DRIVER */

/**
 * @brief MT071x Standard Peripheral Library version number V1.0.0
   */
#define __MT071X_STDPERIPH_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __MT071X_STDPERIPH_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __MT071X_STDPERIPH_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __MT071X_STDPERIPH_VERSION_RC     (0x00) /*!< [7:0]  release candidate */ 
#define __MT071X_STDPERIPH_VERSION        ((__MT071X_STDPERIPH_VERSION_MAIN << 24)\
                                         |(__MT071X_STDPERIPH_VERSION_SUB1 << 16)\
                                         |(__MT071X_STDPERIPH_VERSION_SUB2 << 8)\
                                         |(__MT071X_STDPERIPH_VERSION_RC))
/**
  * @}
  */

/** @addtogroup Configuration_section_for_CMSIS
  * @{
  */

/**
 * @brief MT071x Interrupt Number Definition, according to the selected device 
 *        in @ref Library_configuration_section 
 */
#define __CM0_REV                 0 /*!< Core Revision r0p0                            */
#define __MPU_PRESENT             0 /*!< MIC do not provide MPU                  */
#define __NVIC_PRIO_BITS          2 /*!< MIC uses 2 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0 /*!< Set to 1 if different SysTick Config is used  */

/*!< Interrupt Number Definition */
typedef enum IRQn
{
	/******  Cortex-M0 Processor Exceptions Numbers *****************************************************/
	NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                                 */
	HardFault_IRQn              = -13,    /*!< 3 Cortex-M0 Hard Fault Interrupt                         */
	SVC_IRQn                    = -5,     /*!< 11 Cortex-M0 SV Call Interrupt                           */
	PendSV_IRQn                 = -2,     /*!< 14 Cortex-M0 Pend SV Interrupt                           */
	SysTick_IRQn                = -1,     /*!< 15 Cortex-M0 System Tick Interrupt                       */

	/******  MT071x specific Interrupt Numbers **********************************************************/
	RTC_IRQn                    = 5,      /*!< RTC Interrupts                                           */
	USB_IRQn                    = 6,      /*!< USB Interrupts                                           */
	USB_DMA_IRQn                = 7,      /*!< USB DMA Interrupts                                       */
	FLASH_IRQn                  = 8,      /*!< FLASH Interrupt                                          */
	USART0_IRQn                 = 9,      /*!< USART0 Interrupt                                         */
	USART1_IRQn                 = 10,     /*!< USART1 Interrupts                                        */
	I2C0_IRQn                   = 11,     /*!< I2C0 Interrupts                                          */
	SPI0_IRQn                   = 13,     /*!< SPI0 Interrupts                                          */
	TIMER0_IRQn                 = 14,     /*!< TIMER0 Interrupt                                         */
	TIMER1_IRQn                 = 15,     /*!< TIMER1 Interrupt                                         */
	MCPWM_IRQn                  = 16,     /*!< MCPWM Interrupt                                          */
    DMA_IRQn                    = 17,     /*!< DMA Interrupt                                            */
	GPIO0_TO_GPIO3_IRQn         = 18,     /*!< GPIO0-GPIO3 Interrupt                                    */
	ADC_IRQn                    = 19,     /*!< ADC Interrupt                                            */
	ACMP_IRQn                   = 20,     /*!< ACMP Interrupt                                           */
	BOD_IRQn                    = 21,     /*!< BOD  Interrupt                                           */
	BOR_IRQn                    = 22,     /*!< BOR Interrupt                                            */
	GPIO4_TO_GPIO6_IRQn         = 23,     /*!< GPIO4-GPIO6 Interrupt                                    */
    DAC_IRQn                    = 24,     /*!< DAC Interrupt                                            */
	I2C1_IRQn                   = 25,     /*!< I2C1 Interrupt                                           */
	TSC_IRQn                    = 26,     /*!< TSC Interrupt                                            */
	SPI1_IRQn                   = 27,     /*!< SPI1 Interrupt                                           */
	TIMER2_IRQn                 = 29,     /*!< TIMER2 Interrupt                                         */
	TIMER3_IRQn                 = 30,     /*!< TIMER3 Interrupt                                         */
	CORDIC_IRQn                 = 31,     /*!< CORDIC Interrupt                                         */
} IRQn_Type;

#define GPIO0_IRQn              GPIO0_TO_GPIO3_IRQn
#define GPIO1_IRQn              GPIO0_TO_GPIO3_IRQn
#define GPIO2_IRQn              GPIO0_TO_GPIO3_IRQn
#define GPIO3_IRQn              GPIO0_TO_GPIO3_IRQn
#define GPIO4_IRQn              GPIO4_TO_GPIO6_IRQn
#define GPIO5_IRQn              GPIO4_TO_GPIO6_IRQn
#define GPIO6_IRQn              GPIO4_TO_GPIO6_IRQn

#define GPIO0_IRQHandler        GPIO0_TO_GPIO3_IRQHandler
#define GPIO1_IRQHandler        GPIO0_TO_GPIO3_IRQHandler
#define GPIO2_IRQHandler        GPIO0_TO_GPIO3_IRQHandler
#define GPIO3_IRQHandler        GPIO0_TO_GPIO3_IRQHandler
#define GPIO4_IRQHandler        GPIO4_TO_GPIO6_IRQHandler
#define GPIO5_IRQHandler        GPIO4_TO_GPIO6_IRQHandler
#define GPIO6_IRQHandler        GPIO4_TO_GPIO6_IRQHandler

/**
  * @}
  */

#include "core_cm0.h"
//#include "system_mt071x.h"
#include <stdint.h>

/** @addtogroup Exported_types
  * @{
  */  

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

/** @addtogroup Peripheral_registers_structures
  * @{
  */   

/** 
  * @brief Reset and Clock Control
  */
typedef struct
{
	__IO uint32_t PRESETCTRL0;      //0x00
	__IO uint32_t PRESETCTRL0_SET;
	__IO uint32_t PRESETCTRL0_CLR;
	__IO uint32_t PRESETCTRL0_TOG;
	__IO uint32_t AHBCLKCTRL0;      //0x10
	__IO uint32_t AHBCLKCTRL0_SET;
	__IO uint32_t AHBCLKCTRL0_CLR;
	__IO uint32_t AHBCLKCTRL0_TOG;
	__IO uint32_t BOOTREMAP;        //0x20
	__IO uint32_t BOOTSRAM;         //0x24
	__IO uint32_t OSC12_CTRL;       //0x28
	__IO uint32_t IRC10_CTRL;       //0x2C
	__IO uint32_t SYSPLLCTRL;       //0x30
	__IO uint32_t SYSPLLSTAT;       //0x34
	__IO uint32_t LDOCTRL;          //0x38
	__IO uint32_t USBCTRL;          //0X3C
	__IO uint32_t SYSRSTSTAT;       //0x40
		 uint32_t RESERVED1[3];
	__IO uint32_t MAINCLKSEL;       //0x50
	__IO uint32_t MAINCLKUEN;       //0x54
	__IO uint32_t USARTCLKSEL;      //0x58
	__IO uint32_t USARTCLKUEN;      //0x5C
	__IO uint32_t USBCLKSEL;        //0x60
	__IO uint32_t USBCLKUEN;        //0x64
	__IO uint32_t WWDGCLKSEL;       //0x68
	__IO uint32_t WWDGCLKUEN;       //0x6C
	__IO uint32_t OUTCLKSEL;        //0x70
	__IO uint32_t OUTCLKUEN;        //0x74
         uint32_t RESERVED2[2];
	__IO uint32_t SYSAHBCLKDIV;     //0x80
		 uint32_t RESERVED3[3];
	__IO uint32_t USART0CLKDIV;     //0x90
	__IO uint32_t USART1CLKDIV;     //0x94
		 uint32_t RESERVED4[2];
	__IO uint32_t SYSTICKCLKDIV;    //0xa0
	__IO uint32_t SPI0CLKDIV;       //0xa4
	__IO uint32_t WWDGCLKDIV;       //0xa8
	__IO uint32_t OUTCLKDIV;        //0xac
	__IO uint32_t ADCCLKDIV;        //0xb0
         uint32_t RESERVED5[7];
	__IO uint32_t SPI1CLKDIV;       //0xd0
         uint32_t RESERVED6;
	__IO uint32_t DACCLKDIV;        //0xd8
         uint32_t RESERVED7;
	__IO uint32_t WAKEUPEN;         //0xe0
         uint32_t RESERVED8[7];
	__IO uint32_t ANASTATUS;        //0x100
         uint32_t RESERVED9[7];
	__IO uint32_t SYSTICKCAL;       //0x120
		 uint32_t RESERVED10[3];
	__IO uint32_t BORCTRL;          //0x130
		 uint32_t RESERVED11[51];
	__IO uint32_t PDSLEEPCFG;       //0x200
	__IO uint32_t PDAWAKECFG;       //0x204
	__IO uint32_t PDRUNCFG;         //0x208
         uint32_t RESERVED12[5];
	__IO uint32_t DEVICEID;         //0x220
         uint32_t RESERVED13[3];
	__IO uint32_t DMACTRL;          //0x230
         uint32_t RESERVED14[19];
	__IO uint32_t PCON;             //0x280
         uint32_t RESERVED15[3];
	__IO uint32_t DBGCTRL;          //0x290
         uint32_t RESERVED16[27];
	__IO uint32_t PRESETCTRL1;      //0x300
	__IO uint32_t PRESETCTRL1_SET;
	__IO uint32_t PRESETCTRL1_CLR;
	__IO uint32_t PRESETCTRL1_TOG;
	__IO uint32_t AHBCLKCTRL1;      //0x310
	__IO uint32_t AHBCLKCTRL1_SET;
	__IO uint32_t AHBCLKCTRL1_CLR;
	__IO uint32_t AHBCLKCTRL1_TOG;
} RCC_TypeDef;

/** 
  * @brief ACMP and OP
  */
typedef struct
{
	__IO uint32_t CTRL0;            //0x00
         uint32_t RESERVED0[3];
	__IO uint32_t CTRL1;		    //0x10
         uint32_t RESERVED1[3];
	__IO uint32_t CTRL2;            //0x20
         uint32_t RESERVED2[3];
	__IO uint32_t STATUS;		    //0x30
         uint32_t RESERVED3[3];
	__IO uint32_t OP_CTRL;		    //0x40
         uint32_t RESERVED4[3];
	__IO uint32_t ADV_CTRL0;	    //0x50
         uint32_t RESERVED5[3];
	__IO uint32_t ADV_CTRL1;        //0x60
         uint32_t RESERVED6[3];
	__IO uint32_t ADV_CTRL2;        //0x70
         uint32_t RESERVED7[3];
	__IO uint32_t ADV_CTRL3;        //0x80
         uint32_t RESERVED8[7];
	__IO uint32_t CMPCLKDIV;        //0xA0
         uint32_t RESERVED9[3];
} ACMP_OP_TypeDef;

/**
  * @brief ADC Interface
  */
typedef struct
{
    __IO uint32_t ISR;              //0x00
    __IO uint32_t IER;              //0x04
    __IO uint32_t CR;               //0x08
    __IO uint32_t CFGR1;            //0x0C
    __IO uint32_t CFGR2;            //0x10
         uint32_t RESERVED0[3];
    __IO uint32_t TR;               //0x20
         uint32_t RESERVED1;
    __IO uint32_t CHSELR;           //0x28
         uint32_t RESERVED2;
    __IO uint32_t MISR;             //0x30
         uint32_t RESERVED3[3];
    __IO uint32_t DR;               //0x40
         uint32_t RESERVED4[177];
    __IO uint32_t CCR;              //0x308
         uint32_t RESERVED5[5];
    __IO uint32_t VREF;             //0x320
         uint32_t RESERVED6[55];
    __IO uint32_t ACR0;             //0x400
         uint32_t RESERVED7[3];
    __IO uint32_t DR0;              //0x410
    __IO uint32_t DR1;              //0x414
    __IO uint32_t DR2;              //0x418
    __IO uint32_t DR3;              //0x41C
    __IO uint32_t DR4;              //0x420
    __IO uint32_t DR5;              //0x424
    __IO uint32_t DR6;              //0x428
    __IO uint32_t DR7;              //0x42C
    __IO uint32_t DR8;              //0x430
    __IO uint32_t DR9;              //0x434
    __IO uint32_t DR10;             //0x438
    __IO uint32_t DR11;             //0x43C
    __IO uint32_t DR12;             //0x440
    __IO uint32_t DR13;             //0x444
    __IO uint32_t DR14;             //0x448
    __IO uint32_t DR15;             //0x44C
         uint32_t RESERVED8[4];
    __IO uint32_t ACR1;             //0x460
         uint32_t RESERVED9[3];
    __IO uint32_t ACR2;             //0x470
         uint32_t RESERVED10[3];
    __IO uint32_t DELAY_CTRL;       //0x480
         uint32_t RESERVED11[3];
    __IO uint32_t STATUS;           //0x490
}ADC_TypeDef;

/** 
  * @brief MATHS
  */
typedef struct
{
    __IO uint32_t CTRL;             //0x00
         uint32_t RESERVED0[3];
    __IO uint32_t STATUS;           //0x10
         uint32_t RESERVED1[3];
    __IO uint32_t DATA_IN_X;        //0x20
         uint32_t RESERVED2[3];
    __IO uint32_t DATA_IN_Y;        //0x30
         uint32_t RESERVED3[3];
    __IO uint32_t DATA_IN_Z;        //0x40
         uint32_t RESERVED4[3];
    __IO uint32_t DATA_OUT_X;       //0x50
         uint32_t RESERVED5[3];
    __IO uint32_t DATA_OUT_Y;       //0x60
         uint32_t RESERVED6[3];
    __IO uint32_t DATA_OUT_Z;       //0x70
         uint32_t RESERVED7[11];
    __IO uint32_t CRC_DR;           //0xA0
         uint32_t RESERVED8[3];
    __IO uint32_t CRC_IDR;          //0xB0
         uint32_t RESERVED9[3];
    __IO uint32_t CRC_CR;           //0xC0
         uint32_t RESERVED10[15];
    __IO uint32_t DIV_BEICHUSHU;    //0x100
         uint32_t RESERVED11[3];
    __IO uint32_t DIV_CHUSHU;       //0x110
         uint32_t RESERVED12[3];
    __IO uint32_t DIV_CTRL;         //0x120
         uint32_t RESERVED13[3];
    __IO uint32_t DIV_SHANG;        //0x130
         uint32_t RESERVED14[3];
    __IO uint32_t DIV_YUSHU;        //0x140
         uint32_t RESERVED15[3];
    __IO uint32_t DIV_IRQ;          //0x150
         uint32_t RESERVED16[3];
    __IO uint32_t DIV_STATUS;       //0x160
         uint32_t RESERVED17[3];
} MATHS_TypeDef;

/**
  * @brief DACx Interface
  */
typedef struct
{
  __IO uint32_t CTRL;           //0x00
       uint32_t RESERVED0[3];
  __IO uint32_t DATA;           //0x10
       uint32_t RESERVED1[3];
  __IO uint32_t STS;            //0x20
       uint32_t RESERVED2[3];
  __IO uint32_t ANACTRL;        //0x30
       uint32_t RESERVED3[3];
} DACx_TypeDef;

/**
  * @brief DAC Interface
  */
typedef struct
{
  __IO uint32_t COMCTRL;        //0x80
} DAC_TypeDef;

/** 
  * @brief DMA
  */
typedef struct
{
    __IO uint32_t SAR0;             //0x000
         uint32_t RESERVED1;
    __IO uint32_t DAR0;             //0x008
         uint32_t RESERVED2;
    __IO uint32_t LLP0;             //0x010
         uint32_t RESERVED3;
    __IO uint32_t CTL0_L;           //0x018
    __IO uint32_t CTL0_H;           //0x01c
    __IO uint32_t SSTAT0;           //0x020
         uint32_t RESERVED4;
    __IO uint32_t DSTAT0;           //0x028
         uint32_t RESERVED5;
    __IO uint32_t SSTATAR0;         //0x030
         uint32_t RESERVED6;
    __IO uint32_t DSTATAR0;         //0x038
         uint32_t RESERVED7;
    __IO uint32_t CFG0_L;           //0x040
    __IO uint32_t CFG0_H;           //0x044
    __IO uint32_t SGR0;             //0x048
         uint32_t RESERVED8;
    __IO uint32_t DSR0;             //0x050
         uint32_t RESERVED9;
    __IO uint32_t SAR1;             //0x058
         uint32_t RESERVED10;
    __IO uint32_t DAR1;             //0x060
         uint32_t RESERVED11;
    __IO uint32_t LLP1;             //0x068
         uint32_t RESERVED12;
    __IO uint32_t CTL1_L;           //0x070
    __IO uint32_t CTL1_H;           //0x074
    __IO uint32_t SSTAT1;           //0x078
         uint32_t RESERVED13;
    __IO uint32_t DSTAT1;           //0x080
         uint32_t RESERVED14;
    __IO uint32_t SSTATAR1;         //0x088
         uint32_t RESERVED15;
    __IO uint32_t DSTATAR1;         //0x090
         uint32_t RESERVED16;
    __IO uint32_t CFG1_L;           //0x098
    __IO uint32_t CFG1_H;           //0x09c
    __IO uint32_t SGR1;             //0x0a0
         uint32_t RESERVED17;
    __IO uint32_t DSR1;             //0x0a8
         uint32_t RESERVED18;
    __IO uint32_t SAR2;             //0x0b0
         uint32_t RESERVED19;
    __IO uint32_t DAR2;             //0x0b8
         uint32_t RESERVED20;
    __IO uint32_t LLP2;             //0x0c0
         uint32_t RESERVED21;
    __IO uint32_t CTL2_L;           //0x0c8
    __IO uint32_t CTL2_H;           //0x0cc
    __IO uint32_t SSTAT2;           //0x0d0
         uint32_t RESERVED22;
    __IO uint32_t DSTAT2;           //0x0d8
         uint32_t RESERVED23;
    __IO uint32_t SSTATAR2;         //0x0e0
         uint32_t RESERVED24;
    __IO uint32_t DSTATAR2;         //0x0e8
         uint32_t RESERVED25;
    __IO uint32_t CFG2_L;           //0x0f0
    __IO uint32_t CFG2_H;           //0x0f4
    __IO uint32_t SGR2;             //0x0f8
         uint32_t RESERVED26;
    __IO uint32_t DSR2;             //0x100
         uint32_t RESERVED27;
    __IO uint32_t SAR3;             //0x108
         uint32_t RESERVED28;
    __IO uint32_t DAR3;             //0x110
         uint32_t RESERVED29;
    __IO uint32_t LLP3;             //0x118
         uint32_t RESERVED30;
    __IO uint32_t CTL3_L;           //0x120
    __IO uint32_t CTL3_H;           //0x124
    __IO uint32_t SSTAT;            //0x128
         uint32_t RESERVED31;
    __IO uint32_t DSTAT;            //0x130
         uint32_t RESERVED32;
    __IO uint32_t SSTATAR3;         //0x138
         uint32_t RESERVED33;
    __IO uint32_t DSTATAR3;         //0x140
         uint32_t RESERVED34;
    __IO uint32_t CFG3_L;           //0x148
    __IO uint32_t CFG3_H;           //0x14c
    __IO uint32_t SGR3;             //0x150
         uint32_t RESERVED35;
    __IO uint32_t DSR3;             //0x158
         uint32_t RESERVED36;
    __IO uint32_t SAR4;             //0x160
         uint32_t RESERVED37;
    __IO uint32_t DAR4;             //0x168
         uint32_t RESERVED38;
    __IO uint32_t LLP4;             //0x170
         uint32_t RESERVED39;
    __IO uint32_t CTL4_L;           //0x178
    __IO uint32_t CTL4_H;           //0x17c
    __IO uint32_t SSTAT4;           //0x180
         uint32_t RESERVED40;
    __IO uint32_t DSTAT4;           //0x188
         uint32_t RESERVED41;
    __IO uint32_t SSTATAR4;         //0x190
         uint32_t RESERVED42;
    __IO uint32_t DSTATAR4;         //0x198
         uint32_t RESERVED43;
    __IO uint32_t CFG4_L;           //0x1a0
    __IO uint32_t CFG4_H;           //0x1a4
    __IO uint32_t SGR4;             //0x1a8
         uint32_t RESERVED44;
    __IO uint32_t DSR4;             //0x1b0
         uint32_t RESERVED45;
    __IO uint32_t SAR5;             //0x1b8
         uint32_t RESERVED46;
    __IO uint32_t DAR5;             //0x1c0
         uint32_t RESERVED47;
    __IO uint32_t LLP5;             //0x1c8
         uint32_t RESERVED48;
    __IO uint32_t CTL5_L;           //0x1d0
    __IO uint32_t CTL5_H;           //0x1d4
    __IO uint32_t SSTAT5;           //0x1d8
         uint32_t RESERVED49;
    __IO uint32_t DSTAT5;           //0x1e0
         uint32_t RESERVED50;
    __IO uint32_t SSTATAR5;         //0x1e8
         uint32_t RESERVED51;
    __IO uint32_t DSTATAR5;         //0x1f0
         uint32_t RESERVED52;
    __IO uint32_t CFG5_L;           //0x1f8
    __IO uint32_t CFG5_H;           //0x1fc
    __IO uint32_t SGR5;             //0x200
         uint32_t RESERVED53;
    __IO uint32_t DSR5;             //0x208
         uint32_t RESERVED54;
    __IO uint32_t SAR6;             //0x210
         uint32_t RESERVED55;
    __IO uint32_t DAR6;             //0x218
         uint32_t RESERVED56;
    __IO uint32_t LLP6;             //0x220
         uint32_t RESERVED57;
    __IO uint32_t CTL6_L;           //0x228
    __IO uint32_t CTL6_H;           //0x22c
    __IO uint32_t SSTAT6;           //0x230
         uint32_t RESERVED58;
    __IO uint32_t DSTAT6;           //0x238
         uint32_t RESERVED59;
    __IO uint32_t SSTATAR6;         //0x240
         uint32_t RESERVED60;
    __IO uint32_t DSTATAR6;         //0x248
         uint32_t RESERVED61;
    __IO uint32_t CFG6_L;           //0x250
    __IO uint32_t CFG6_H;           //0x254
    __IO uint32_t SGR6;             //0x258
         uint32_t RESERVED62;
    __IO uint32_t DSR6;             //0x260
         uint32_t RESERVED63;
    __IO uint32_t SAR7;             //0x268
         uint32_t RESERVED64;
    __IO uint32_t DAR7;             //0x270
         uint32_t RESERVED65;
    __IO uint32_t LLP7;             //0x278
         uint32_t RESERVED66;
    __IO uint32_t CTL7_L;           //0x280
    __IO uint32_t CTL7_H;           //0x284
    __IO uint32_t SSTAT7;           //0x288
         uint32_t RESERVED67;
    __IO uint32_t DSTAT7;           //0x290
         uint32_t RESERVED68;
    __IO uint32_t SSTATAR7;         //0x298
         uint32_t RESERVED69;
    __IO uint32_t DSTATAR7;         //0x2a0
         uint32_t RESERVED70;
    __IO uint32_t CFG7_L;           //0x2a8
    __IO uint32_t CFG7_H;           //0x2ac
    __IO uint32_t SGR7;             //0x2b0
         uint32_t RESERVED71;
    __IO uint32_t DSR7;             //0x2b8
         uint32_t RESERVED72;
    __IO uint32_t RawTfr;           //0x2c0
         uint32_t RESERVED73;
    __IO uint32_t RawBlock;         //0x2c8
         uint32_t RESERVED74;
    __IO uint32_t RawSrcTran;       //0x2d0
         uint32_t RESERVED75;
    __IO uint32_t RawDstTran;       //0x2d8
         uint32_t RESERVED76;
    __IO uint32_t RawErr;           //0x2e0
         uint32_t RESERVED77;
    __IO uint32_t StatusTfr;        //0x2e8
         uint32_t RESERVED78;
    __IO uint32_t StatusBlock;      //0x2f0
         uint32_t RESERVED79;
    __IO uint32_t StatusSrcTran;    //0x2f8
         uint32_t RESERVED80;
    __IO uint32_t StatusDstTran;    //0x300
         uint32_t RESERVED81;
    __IO uint32_t StatusErr;        //0x308  
         uint32_t RESERVED82;
    __IO uint32_t MaskTfr;          //0x310
         uint32_t RESERVED83;
    __IO uint32_t MaskBlock;        //0x318
         uint32_t RESERVED84;
    __IO uint32_t MaskSrcTran;      //0x320
         uint32_t RESERVED85;
    __IO uint32_t MaskDstTran;      //0x328
         uint32_t RESERVED86;
    __IO uint32_t MaskErr;          //0x330
         uint32_t RESERVED87;
    __IO uint32_t ClearTfr;         //0x338
         uint32_t RESERVED88;
    __IO uint32_t ClearBlock;       //0x340
         uint32_t RESERVED89;
    __IO uint32_t ClearSrcTran;     //0x348
         uint32_t RESERVED90;
    __IO uint32_t ClearDstTran;     //0x350
         uint32_t RESERVED91;
    __IO uint32_t ClearErr;         //0x358   
         uint32_t RESERVED92;
    __IO uint32_t StatusInt;        //0x360
         uint32_t RESERVED93;
    __IO uint32_t ReqSrcReg;        //0x368
         uint32_t RESERVED94;
    __IO uint32_t ReqDstReg;        //0x370
         uint32_t RESERVED95;
    __IO uint32_t SglReqSrcReg;     //0x378
         uint32_t RESERVED96;
    __IO uint32_t SglReqDstReg;     //0x380
         uint32_t RESERVED97;
    __IO uint32_t LstSrcReg;        //0x388
         uint32_t RESERVED98;
    __IO uint32_t LstDstReg;        //0x390
         uint32_t RESERVED99;
    __IO uint32_t DmaCfgReg;        //0x398
         uint32_t RESERVED100;
    __IO uint32_t ChEnReg;          //0x3a0
         uint32_t RESERVED101;
}DMA_TypeDef;

/** 
  * @brief FLASH Memory Control
  */
typedef struct
{
	__IO uint32_t ACR;		        //0x00
	__IO uint32_t KEYR;		        //0x04
	__IO uint32_t ERASE;	        //0x08
	__IO uint32_t SR;		        //0x0C
	__IO uint32_t CR;		        //0x10
	__IO uint32_t RESERVE0[5];
	__IO uint32_t INFO0;            //0x28
	__IO uint32_t INFO1;            //0x3C
	__IO uint32_t INFO2;            //0x30
	__IO uint32_t INFO3;            //0x34
	__I  uint32_t ID0;	            //0x38
	__I  uint32_t ID1;              //0x3C
	__I  uint32_t ID2;              //0x40
	__I  uint32_t ID3;              //0x44
} FMC_TypeDef;

/** 
  * @brief General Purpose I/O
  */
typedef struct
{
    __IO uint32_t DT;               //0x00
    __IO uint32_t DT_SET;           //0x04
    __IO uint32_t DT_CLR;           //0x08
    __IO uint32_t DT_TOG;           //0x0C
} GPIO_DATA_TypeDef;

typedef struct
{
    __IO uint32_t FILTER;           //0x00
} GPIO_FILTER_TypeDef;

typedef struct
{
    __IO uint32_t BSRR_LOW;         //0x00
    __IO uint32_t BSRR_HIGH;        //0x04
} GPIO_BSRR_TypeDef;

typedef struct
{
    __IO uint32_t DIR;              //0x00
    __IO uint32_t DIR_SET;
    __IO uint32_t DIR_CLR;
    __IO uint32_t DIR_TOG;
    __IO uint32_t IS;               //0x10
    __IO uint32_t IS_SET;
    __IO uint32_t IS_CLR;
    __IO uint32_t IS_TOG;
    __IO uint32_t IBE;              //0x20
    __IO uint32_t IBE_SET;
    __IO uint32_t IBE_CLR;
    __IO uint32_t IBE_TOG;
    __IO uint32_t IEV;              //0x30
    __IO uint32_t IEV_SET;
    __IO uint32_t IEV_CLR;
    __IO uint32_t IEV_TOG;
    __IO uint32_t IE;               //0x40
    __IO uint32_t IE_SET;
    __IO uint32_t IE_CLR;
    __IO uint32_t IE_TOG;
    __IO uint32_t IRS;              //0x50
    __IO uint32_t IRS_SET;
    __IO uint32_t IRS_CLR;
    __IO uint32_t IRS_TOG;
    __IO uint32_t MIS;              //0x60
    __IO uint32_t MIS_SET;
    __IO uint32_t MIS_CLR;
    __IO uint32_t MIS_TOG;
    __IO uint32_t IC;               //0x70
    __IO uint32_t IC_SET;
    __IO uint32_t IC_CLR;
    __IO uint32_t IC_TOG;
    __IO uint32_t DATAMASK;         //0x80
    __IO uint32_t DATAMASK_SET;
    __IO uint32_t DATAMASK_CLR;
    __IO uint32_t DATAMASK_TOG;
} GPIO_BANK_TypeDef;

typedef struct
{				
    __IO uint32_t CON;              //0x00
}IOCON_TypeDef;

/**
  * @brief Inter-integrated Circuit Interface
  */
typedef struct
{
	__IO uint32_t CON;              //0x00
	__IO uint32_t TAR;              //0x04
	__IO uint32_t SAR;              //0x08
	__IO uint32_t HS_MADDR;         //0x0C
	__IO uint32_t DATA_CMD;         //0x10
	__IO uint32_t SS_SCL_HCNT;      //0x14
	__IO uint32_t SS_SCL_LCNT;      //0x18
	__IO uint32_t FS_SCL_HCNT;      //0x1C
	__IO uint32_t FS_SCL_LCNT;      //0x20
	__IO uint32_t HS_SCL_HCNT;      //0x24
	__IO uint32_t HS_SCL_LCNT;      //0x28
	__IO uint32_t INTR_STAT;        //0x2C
	__IO uint32_t INTR_MASK;        //0x30
	__IO uint32_t RAW_INTR_STAT;    //0x34
	__IO uint32_t RX_TL;            //0x38
	__IO uint32_t TX_TL;            //0x3C
	__IO uint32_t CLR_INTR;         //0x40
	__IO uint32_t CLR_RX_UNDER;     //0x44
	__IO uint32_t CLR_RX_OVER;      //0x48
	__IO uint32_t CLR_TX_OVER;      //0x4C
	__IO uint32_t CLR_RD_REQ;       //0x50
	__IO uint32_t CLR_TX_ABRT;      //0x54
	__IO uint32_t CLR_RX_DONE;      //0x58
	__IO uint32_t CLR_ACTIVITY;     //0x5C
	__IO uint32_t CLR_STOP_DET;     //0x60
	__IO uint32_t CLR_START_DET;    //0x64
	__IO uint32_t CLR_GEN_CALL;     //0x68
	__IO uint32_t ENABLE;           //0x6C
	__IO uint32_t STAT;             //0x70
	__IO uint32_t TXFLR;            //0x74
	__IO uint32_t RXFLR;            //0x78
	     uint32_t RESERVED0;        //0x7C
	__IO uint32_t TX_ABRT_SOURCE;   //0x80
	__IO uint32_t SLV_DATA_NACK_ONLY;   //0x84
    	 uint32_t RESERVED1;        //0x88
    	 uint32_t RESERVED2;        //0x8C
    	 uint32_t RESERVED3;        //0x90
	__IO uint32_t SDA_SETUP;        //0x94
	__IO uint32_t ACK_GENERAL_CALL; //0x98
	__IO uint32_t ENABLE_STATUS;    //0x9C
}I2C_TypeDef;

/** 
  * @brief Independent WATCHDOG
  */
typedef struct
{
    __IO uint32_t KR;               //0x00
    __IO uint32_t PR;               //0x04
    __IO uint32_t RLR;              //0x08
    __IO uint32_t SR;               //0x0C
    __IO uint32_t WINR;             //0x10
} IWDG_TypeDef;

/** 
  * @brief Window WATCHDOG
  */
typedef struct
{
    __IO uint32_t CR;               //0x00
    __IO uint32_t CFR;              //0x04
    __IO uint32_t SR;               //0x08
} WWDG_TypeDef;

/** 
  * @brief MCPWM
  */
typedef struct
{
    __IO uint32_t CON;              //0x00
    __IO uint32_t CON_SET;          //0x04
    __IO uint32_t CON_CLR;          //0x08
    __IO uint32_t CAPCON;           //0x0C
    __IO uint32_t CAPCON_SET;       //0x10
    __IO uint32_t CAPCON_CLR;       //0x14
    __IO uint32_t TC0;              //0x18
    __IO uint32_t TC1;              //0x1C
    __IO uint32_t TC2;              //0x20
    __IO uint32_t LIM0;             //0x24
    __IO uint32_t LIM1;             //0x28
    __IO uint32_t LIM2;             //0x2C
    __IO uint32_t MAT0;             //0x30
    __IO uint32_t MAT1;             //0x34
    __IO uint32_t MAT2;             //0x38
    __IO uint32_t DT;               //0x3C
    __IO uint32_t CCP;              //0x40
    __IO uint32_t CAP0;             //0x44
    __IO uint32_t CAP1;             //0x48
    __IO uint32_t CAP2;             //0x4C
    __IO uint32_t INTEN;            //0x50
    __IO uint32_t INTEN_SET;        //0x54
    __IO uint32_t INTEN_CLR;        //0x58
    __IO uint32_t CNTCON;           //0x5C
    __IO uint32_t CNTCON_SET;       //0x60
    __IO uint32_t CNTCON_CLR;       //0x64
    __IO uint32_t INTF;             //0x68
    __IO uint32_t INTF_SET;         //0x6C
    __IO uint32_t INTF_CLR;         //0x70
    __IO uint32_t CAP_CLR;          //0x74
    __IO uint32_t HALL;             //0x78
    __IO uint32_t HALLS;            //0x7C
    __IO uint32_t HALL_VEL_CMP;     //0x80
    __IO uint32_t HALL_VEL_VAL;     //0x84
    __IO uint32_t HALL_VEL_TH;      //0x88
    __IO uint32_t HALL_VEL_MCIST;   //0x8C
    __IO uint32_t PR;               //0x90
} MCPWM_TypeDef;

/** 
  * @brief Real-Time Clock
  */
typedef struct
{
    __IO uint32_t CTRL;             //0x00
    __IO uint32_t LOAD;             //0x04
    __IO uint32_t CNT;              //0x08
    __IO uint32_t ALARM;            //0x0C
    __IO uint32_t STAT;             //0x10
} RTC_TypeDef;      

/**
  * @brief SPI
  */
typedef struct
{
    __IO uint32_t CR0;	            //0x00
    __IO uint32_t CR1;	            //0x04
    __IO uint32_t DR;	            //0x08
    __IO uint32_t SR;	            //0x0c
    __IO uint32_t CPSR;	            //0x10
    __IO uint32_t IMSC;	            //0x14
    __IO uint32_t RIS;	            //0x18
    __IO uint32_t MIS;	            //0x1c
    __IO uint32_t ICR;	            //0x20
    __IO uint32_t DMACR;	        //0x24
} SPI_TypeDef;

/** 
  * @brief TIMER
  */
typedef struct
{
    __IO uint32_t IR;               //0x000
         uint32_t RESERVED0[3];
    __IO uint32_t TCR;              //0x010
         uint32_t RESERVED1[3];
    __IO uint32_t DIR;              //0x020
         uint32_t RESERVED2[3];
    __IO uint32_t TC0;              //0x030
         uint32_t RESERVED3[3];
    __IO uint32_t TC1;              //0x040
         uint32_t RESERVED4[3];
    __IO uint32_t TC2;              //0x050
         uint32_t RESERVED5[3];
    __IO uint32_t TC3;              //0x060
         uint32_t RESERVED6[3];
    __IO uint32_t PR;               //0x070
         uint32_t RESERVED7[3];
    __IO uint32_t PC;               //0x080
         uint32_t RESERVED8[3];
    __IO uint32_t MCR;              //0x090
         uint32_t RESERVED9[3];
    __IO uint32_t MR0;              //0x0A0
         uint32_t RESERVED10[3];
    __IO uint32_t MR1;              //0x0B0
         uint32_t RESERVED11[3];
    __IO uint32_t MR2;              //0x0C0
         uint32_t RESERVED12[3];
    __IO uint32_t MR3;              //0x0D0
         uint32_t RESERVED13[3];
    __IO uint32_t CCR;              //0x0E0
         uint32_t RESERVED14[3];
    __IO uint32_t CR0;              //0x0F0
         uint32_t RESERVED15[3];
    __IO uint32_t CR1;              //0x100
         uint32_t RESERVED16[3];
    __IO uint32_t CR2;              //0x110
         uint32_t RESERVED17[3];
    __IO uint32_t CR3;              //0x120
         uint32_t RESERVED18[3];
    __IO uint32_t EMR;              //0x130
         uint32_t RESERVED19[3];
    __IO uint32_t PWMTH0;           //0x140
         uint32_t RESERVED20[3];
    __IO uint32_t PWMTH1;           //0x150
         uint32_t RESERVED21[3];
    __IO uint32_t PWMTH2;           //0x160
         uint32_t RESERVED22[3];
    __IO uint32_t PWMTH3;           //0x170
         uint32_t RESERVED23[3];
    __IO uint32_t CTCR;             //0x180
         uint32_t RESERVED24[3];
    __IO uint32_t PWMC;             //0x190
         uint32_t RESERVED25[3];
    __IO uint32_t PWMV0;            //0x1A0
         uint32_t RESERVED26[3];
    __IO uint32_t PWMV1;            //0x1B0
         uint32_t RESERVED27[3];
    __IO uint32_t PWMV2;            //0x1C0
         uint32_t RESERVED28[3];
    __IO uint32_t PWMV3;            //0x1D0
         uint32_t RESERVED29[3];
} TIMER_TypeDef;

/** 
  * @brief TSC
  */
typedef struct
{
    __IO uint32_t CR;               //0x000
         uint32_t RESERVED0[3];
    __IO uint32_t IER;              //0x010
         uint32_t RESERVED1[3];
    __IO uint32_t ICR;              //0x020
         uint32_t RESERVED2[3];
    __IO uint32_t ISR;              //0x030
         uint32_t RESERVED3[3];
    __IO uint32_t IOHCR;            //0x040
         uint32_t RESERVED4[3];
    __IO uint32_t IOASCR;           //0x050
         uint32_t RESERVED5[3];
    __IO uint32_t IOSCR;            //0x060
         uint32_t RESERVED6[3];
    __IO uint32_t IOCCR;            //0x070
         uint32_t RESERVED7[3];
    __IO uint32_t IOGCSR;           //0x080
         uint32_t RESERVED8[3];
    __IO uint32_t IOG1CR;           //0x090
         uint32_t RESERVED9[3];
    __IO uint32_t IOG2CR;           //0x0A0
         uint32_t RESERVED10[3];
    __IO uint32_t IOG3CR;           //0x0B0
         uint32_t RESERVED11[3];
    __IO uint32_t IOG4CR;           //0x0C0
         uint32_t RESERVED12[3];
    __IO uint32_t IOG5CR;           //0x0D0
         uint32_t RESERVED13[3];
    __IO uint32_t IOG6CR;           //0x0E0
         uint32_t RESERVED14[3];
} TSC_TypeDef;

/** 
  * @brief USART
  */
typedef struct
{
	__IO uint32_t CTRL0;            //0x00
	__IO uint32_t CTRL0_SET;
	__IO uint32_t CTRL0_CLR;
	__IO uint32_t CTRL0_TOG;
	__IO uint32_t CTRL1;            //0x10
	__IO uint32_t CTRL1_SET;
	__IO uint32_t CTRL1_CLR;
	__IO uint32_t CTRL1_TOG;
	__IO uint32_t CTRL2;            //0x20
	__IO uint32_t CTRL2_SET;
	__IO uint32_t CTRL2_CLR;
	__IO uint32_t CTRL2_TOG;
	__IO uint32_t LINECTRL;		    //0x30
	__IO uint32_t LINECTRL_SET;
	__IO uint32_t LINECTRL_CLR;
	__IO uint32_t LINECTRL_TOG;
	__IO uint32_t INTR;			    //0x40
	__IO uint32_t INTR_SET;
	__IO uint32_t INTR_CLR;
	__IO uint32_t INTR_TOG;
	__IO uint32_t DATA;			    //0x50
	     uint32_t RESERVED1[3];
	__IO uint32_t STAT;			    //0x60
	     uint32_t RESERVED2[11];
	__IO uint8_t  RS485CTRL;		//0x90
	     uint8_t  RESERVED3;
	     uint16_t RESERVED4;
	__IO uint8_t  RS485CTRL_SET;
	     uint8_t  RESERVED5;
	     uint16_t RESERVED6;
	__IO uint8_t  RS485CTRL_CLR;
	     uint8_t  RESERVED7;
	     uint16_t RESERVED8;
	__IO uint8_t  RS485CTRL_TOG;
	     uint8_t  RESERVED9;
	     uint16_t RESERVED10;
	__IO uint8_t  RS485ADRMATCH;	//0xa0
	     uint8_t  RESERVED11;
	     uint16_t RESERVED12;
	     uint32_t RESERVED13[3];
	__IO uint8_t  RS485DLY;		    //0xb0
	     uint8_t  RESERVED14;
	     uint16_t RESERVED15;
	     uint32_t RESERVED16[3];
	__IO uint16_t AUTOBAUD;		    //0xc0
	     uint16_t RESERVED17;
	__IO uint16_t AUTOBAUD_SET;
	     uint16_t RESERVED18;
	__IO uint16_t AUTOBAUD_CLR;
	     uint16_t RESERVED19;
	__IO uint16_t AUTOBAUD_TOG;
	     uint16_t RESERVED20;
	__IO uint32_t CTRL3;			//0xd0
	__IO uint32_t CTRL3_SET;
	__IO uint32_t CTRL3_CLR;
	__IO uint32_t CTRL3_TOG;
	__IO uint16_t ISO7816_CTRL;	    //0xe0
	     uint16_t RESERVED21;
	__IO uint16_t ISO7816_CTRL_SET;
	     uint16_t RESERVED22;
	__IO uint16_t ISO7816_CTRL_CLR;
	     uint16_t RESERVED23;
	__IO uint16_t ISO7816_CTRL_TOG;
	     uint16_t RESERVED24;
	__IO uint8_t  IS07816_ERR_CNT;	//0xf0
	     uint8_t  RESERVED25;
	     uint16_t RESERVED26;
	     uint32_t RESERVED27[3];
	__IO uint8_t  IS07816_STATUS;	//0x100
	     uint8_t  RESERVED28;
	     uint16_t RESERVED29;
	     uint32_t RESERVED30[3];
	__IO uint32_t LINCTRL;			//0x110
	     uint32_t RESERVED31[3];
	__IO uint32_t RXTIMEOUT;        //0x120
	     uint32_t RESERVED32[3];
} USART_TypeDef;

/**
  * @}
  */

/** @addtogroup Peripheral_memory_map
* @{
*/
/*!< Peripheral memory map */
#define FLASH_BASE1             ((uint32_t)0x00000000)      /*!< FLASH base1 address in the alias region */
#define FLASH_BASE2             ((uint32_t)0x10000000)      /*!< FLASH base2 address in the alias region */
#define APROM_SIZE				((uint32_t)0x0001E000)      /*!< APROM size */
#define LDROM_SIZE              ((uint32_t)0x00002000)      /*!< LDROM size */
#define SRAM_BASE				((uint32_t)0x20000000)      /*!< SRAM base address in the alias region */
#define SRAM_SIZE				((uint32_t)0x00004000)      /*!< SRAM size */
#define PERIPH_BASE				((uint32_t)0x40000000)      /*!< Peripheral base address in the alias region */
#define SRAM_BB_BASE            ((uint32_t)0x22000000)      /*!< SRAM base address in the bit-band region */
#define PERIPH_BB_BASE          ((uint32_t)0x42000000)      /*!< Peripheral base address in the bit-band region */

#define APBPERIPH_BASE			(PERIPH_BASE)

#define RCC_BASE                (APBPERIPH_BASE)
#define IOCON_BASE              (APBPERIPH_BASE + 0x1000)
#define TSC_BASE                (APBPERIPH_BASE + 0x1800)
#define USART0_BASE             (APBPERIPH_BASE + 0x2000)
#define USART1_BASE             (APBPERIPH_BASE + 0x2800)
#define TIMER2_BASE             (APBPERIPH_BASE + 0x3000)
#define TIMER3_BASE             (APBPERIPH_BASE + 0x3800)
#define I2C0_BASE               (APBPERIPH_BASE + 0x4000)
#define I2C1_BASE               (APBPERIPH_BASE + 0x4800)
#define ADC_BASE                (APBPERIPH_BASE + 0x5000)
#define ACMP_OP_BASE            (APBPERIPH_BASE + 0x5800)
#define MCPWM_BASE              (APBPERIPH_BASE + 0x7000)
#define SPI0_BASE               (APBPERIPH_BASE + 0x8000)
#define DAC0_BASE               (APBPERIPH_BASE + 0x8800)
#define DAC1_BASE               (APBPERIPH_BASE + 0x8840)
#define DAC_BASE                (APBPERIPH_BASE + 0x8880)
#define TIMER0_BASE             (APBPERIPH_BASE + 0x9000)
#define TIMER1_BASE             (APBPERIPH_BASE + 0xA000)
#define FMC_BASE                (APBPERIPH_BASE + 0xB000)
#define WWDG_BASE               (APBPERIPH_BASE + 0xC000)
#define IWDG_BASE               (APBPERIPH_BASE + 0xC800)
#define SPI1_BASE               (APBPERIPH_BASE + 0xD000)
#define RTC_BASE                (APBPERIPH_BASE + 0xF000)
#define MATHS_BASE              (APBPERIPH_BASE + 0xF800)
#define DMA_BASE                (APBPERIPH_BASE + 0x50000)
#define USB_BASE                (APBPERIPH_BASE + 0x60000)

#define GPIO_BASE               (APBPERIPH_BASE + 0x20000)
#define GPIO_DATA0_BASE         (GPIO_BASE)
#define GPIO_DATA1_BASE         (GPIO_DATA0_BASE + 0x10000)
#define GPIO_DATA2_BASE         (GPIO_DATA0_BASE + 0x20000)
#define GPIO_BSRR0_BASE         (GPIO_DATA0_BASE + 0x4000)
#define GPIO_BSRR1_BASE         (GPIO_DATA1_BASE + 0x4000)
#define GPIO_BSRR2_BASE         (GPIO_DATA2_BASE + 0x4000)
#define GPIO_BANK0_BASE         (GPIO_DATA0_BASE + 0x8000)
#define GPIO_BANK1_BASE         (GPIO_DATA1_BASE + 0x8000)
#define GPIO_BANK2_BASE         (GPIO_DATA2_BASE + 0x8000)
#define GPIO_FILTER_BASE        (GPIO_BASE + 0x0100)

/**
  * @}
  */

/** @addtogroup Peripheral_declaration
  * @{
  */  
#define RCC                     ((RCC_TypeDef *) RCC_BASE)
#define TSC                     ((TSC_TypeDef *) TSC_BASE)
#define USART0                  ((USART_TypeDef *) USART0_BASE)
#define USART1                  ((USART_TypeDef *) USART1_BASE)
#define TIMER2                  ((TIMER_TypeDef *) TIMER2_BASE)
#define TIMER3                  ((TIMER_TypeDef *) TIMER3_BASE)
#define I2C0                    ((I2C_TypeDef *) I2C0_BASE)
#define I2C1                    ((I2C_TypeDef *) I2C1_BASE)
#define ADC                     ((ADC_TypeDef *) ADC_BASE)
#define ACMP                    ((ACMP_OP_TypeDef *) ACMP_OP_BASE)
#define MCPWM                   ((MCPWM_TypeDef *) MCPWM_BASE)
#define SPI0                    ((SPI_TypeDef *) SPI0_BASE)
#define DAC0                    ((DACx_TypeDef *) DAC0_BASE)
#define DAC1                    ((DACx_TypeDef *) DAC1_BASE)
#define DAC                     ((DAC_TypeDef *) DAC_BASE)
#define TIMER0                  ((TIMER_TypeDef *) TIMER0_BASE)
#define TIMER1                  ((TIMER_TypeDef *) TIMER1_BASE)
#define FLASH                   ((FMC_TypeDef *) FMC_BASE)
#define WWDG                    ((WWDG_TypeDef *) WWDG_BASE)
#define IWDG                    ((IWDG_TypeDef *) IWDG_BASE)
#define SPI1                    ((SPI_TypeDef *) SPI1_BASE)
#define RTC                     ((RTC_TypeDef *) RTC_BASE)
#define MATHS                   ((MATHS_TypeDef *) MATHS_BASE)
#define DMA                     ((DMA_TypeDef *) DMA_BASE)

#define GPIODATA0               ((GPIO_DATA_TypeDef *) GPIO_DATA0_BASE)
#define GPIODATA1               ((GPIO_DATA_TypeDef *) GPIO_DATA1_BASE)
#define GPIODATA2               ((GPIO_DATA_TypeDef *) GPIO_DATA2_BASE)
#define GPIOBSRR0               ((GPIO_BSRR_TypeDef *) GPIO_BSRR0_BASE)
#define GPIOBSRR1               ((GPIO_BSRR_TypeDef *) GPIO_BSRR1_BASE)
#define GPIOBSRR2               ((GPIO_BSRR_TypeDef *) GPIO_BSRR2_BASE)
#define GPIOBANK0               ((GPIO_BANK_TypeDef *) GPIO_BANK0_BASE)
#define GPIOBANK1               ((GPIO_BANK_TypeDef *) GPIO_BANK1_BASE)
#define GPIOBANK2               ((GPIO_BANK_TypeDef *) GPIO_BANK2_BASE)
#define GPIOFILTER              ((GPIO_FILTER_TypeDef *) GPIO_FILTER_BASE)

/**
* @}
*/

#ifdef USE_STDPERIPH_DRIVER
  #include "mt071x_conf.h"
#endif

/** @addtogroup Exported_constants
  * @{
  */

/** @addtogroup Peripheral_Registers_Bits_Definition
* @{
*/

/******************************************************************************/
/*                         Peripheral Registers Bits Definition               */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                                     RCC                                    */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for RCC_PRESETCTRL register  *************/
#define RCC_PRESETCTRL_FLASH                ((uint32_t)0x00000001)          /*!< Reset FLASH */
#define RCC_PRESETCTRL_GPIO                 ((uint32_t)0x00000004)          /*!< Reset GPIO */
#define RCC_PRESETCTRL_DMA                  ((uint32_t)0x00000008)          /*!< Reset DMA */
#define RCC_PRESETCTRL_USART0               ((uint32_t)0x00000010)          /*!< Reset USART0 */
#define RCC_PRESETCTRL_USART1               ((uint32_t)0x00000020)          /*!< Reset USART1 */
#define RCC_PRESETCTRL_I2C0                 ((uint32_t)0x00000080)          /*!< Reset I2C0 */
#define RCC_PRESETCTRL_ADC                  ((uint32_t)0x00000100)          /*!< Reset ADC */
#define RCC_PRESETCTRL_IOCON                ((uint32_t)0x00000200)          /*!< Reset IOCON */
#define RCC_PRESETCTRL_WWDG                 ((uint32_t)0x00000400)          /*!< Reset WWDG */
#define RCC_PRESETCTRL_MCPWM                ((uint32_t)0x00001000)          /*!< Reset MCPWM */
#define RCC_PRESETCTRL_SPI0                 ((uint32_t)0x00002000)          /*!< Reset SPI0 */
#define RCC_PRESETCTRL_TIMER0               ((uint32_t)0x00004000)          /*!< Reset TIMER0 */
#define RCC_PRESETCTRL_TIMER1               ((uint32_t)0x00008000)          /*!< Reset TIMER1 */
#define RCC_PRESETCTRL_ACMP                 ((uint32_t)0x00010000)          /*!< Reset ACMP */
#define RCC_PRESETCTRL_TIMER2               ((uint32_t)0x00040000)          /*!< Reset TIMER2 */
#define RCC_PRESETCTRL_IWDG                 ((uint32_t)0x00080000)          /*!< Reset IWDG */
#define RCC_PRESETCTRL_SPI1                 ((uint32_t)0x00400000)          /*!< Reset SPI1 */
#define RCC_PRESETCTRL_I2C1                 ((uint32_t)0x01000000)          /*!< Reset I2C1 */
#define RCC_PRESETCTRL_TIMER3               ((uint32_t)0x02000000)          /*!< Reset TIMER3 */
#define RCC_PRESETCTRL_RTC                  ((uint32_t)0x40000000)          /*!< Reset RTC */
#define RCC_PRESETCTRL_USB                  ((uint32_t)0x80000000)          /*!< Reset USB */

/*******************  Bit definition for RCC_PRESETCTRL1 register  ************/
#define RCC_PRESETCTRL1_DAC                 ((uint32_t)0x00000001)          /*!< Reset DAC */
#define RCC_PRESETCTRL1_MATHS               ((uint32_t)0x00000002)          /*!< Reset MATHS */
#define RCC_PRESETCTRL1_TSC                 ((uint32_t)0x00000004)          /*!< Reset TSC */

/*******************  Bit definition for RCC_AHBCLKCTRL register  *************/
#define RCC_AHBCLKCTRL_FLASH                ((uint32_t)0x00000001)          /*!< FLASH clock bit */
#define RCC_AHBCLKCTRL_RAM                  ((uint32_t)0x00000002)          /*!< RAM clock bit */
#define RCC_AHBCLKCTRL_GPIO                 ((uint32_t)0x00000004)          /*!< GPIO clock bit */
#define RCC_AHBCLKCTRL_DMA                  ((uint32_t)0x00000008)          /*!< DMA clock bit */
#define RCC_AHBCLKCTRL_USART0               ((uint32_t)0x00000010)          /*!< USART0 clock bit */
#define RCC_AHBCLKCTRL_USART1               ((uint32_t)0x00000020)          /*!< USART1 clock bit */
#define RCC_AHBCLKCTRL_ROM                  ((uint32_t)0x00000040)          /*!< ROM clock bit */
#define RCC_AHBCLKCTRL_I2C0                 ((uint32_t)0x00000080)          /*!< I2C0 clock bit */
#define RCC_AHBCLKCTRL_ADC                  ((uint32_t)0x00000100)          /*!< ADC clock bit */
#define RCC_AHBCLKCTRL_IOCON                ((uint32_t)0x00000200)          /*!< IOCON clock bit */
#define RCC_AHBCLKCTRL_WWDG                 ((uint32_t)0x00000400)          /*!< WWDG clock bit */
#define RCC_AHBCLKCTRL_MCPWM                ((uint32_t)0x00001000)          /*!< MCPWM clock bit */
#define RCC_AHBCLKCTRL_SPI0                 ((uint32_t)0x00002000)          /*!< SPI0 clock bit */
#define RCC_AHBCLKCTRL_TIMER0               ((uint32_t)0x00004000)          /*!< TIMER0 clock bit */
#define RCC_AHBCLKCTRL_TIMER1               ((uint32_t)0x00008000)          /*!< TIMER1 clock bit */
#define RCC_AHBCLKCTRL_ACMP                 ((uint32_t)0x00010000)          /*!< ACMP clock bit */
#define RCC_AHBCLKCTRL_TIMER2               ((uint32_t)0x00040000)          /*!< TIMER2 clock bit */
#define RCC_AHBCLKCTRL_IWDG                 ((uint32_t)0x00080000)          /*!< IWDG clock bit */
#define RCC_AHBCLKCTRL_SPI1                 ((uint32_t)0x00400000)          /*!< SPI1 clock bit */
#define RCC_AHBCLKCTRL_I2C1                 ((uint32_t)0x01000000)          /*!< I2C1 clock bit */
#define RCC_AHBCLKCTRL_TIMER3               ((uint32_t)0x02000000)          /*!< TIMER3 clock bit */
#define RCC_AHBCLKCTRL_RTC                  ((uint32_t)0x40000000)          /*!< RTC clock bit */
#define RCC_AHBCLKCTRL_USB                  ((uint32_t)0x80000000)          /*!< USB clock bit */

/*******************  Bit definition for RCC_AHBCLKCTRL1 register  ************/
#define RCC_AHBCLKCTRL1_DAC                 ((uint32_t)0x00000001)          /*!< DAC clock bit */
#define RCC_AHBCLKCTRL1_MATHS               ((uint32_t)0x00000002)          /*!< MATHS clock bit */
#define RCC_AHBCLKCTRL1_TSC                 ((uint32_t)0x00000004)          /*!< TSC clock bit */

/*******************  Bit definition for RCC_BOOTREMAP register  **************/
#define RCC_BOOTREMAP_BOOT_IO               ((uint32_t)0x00000001)          /*!< Boot flag bit */
#define RCC_BOOTREMAP_BOOT_FLASH            ((uint32_t)0x00000002)          /*!< Boot flag bit */
#define RCC_BOOTREMAP_PROTECT0              ((uint32_t)0x00000004)          /*!< Protect0 flag bit */
#define RCC_BOOTREMAP_PROTECT1              ((uint32_t)0x00000008)          /*!< Protect1 flag bit */
#define RCC_BOOTREMAP_BOOT_REG              ((uint32_t)0x00000010)          /*!< Boot flag bit */

/*******************  Bit definition for RCC_BOOTSRAM register  ***************/
#define RCC_BOOTSRAM_SRAM_REMAP             ((uint32_t)0x000000AA)          /*!< SRAM remap */
#define RCC_BOOTSRAM_LDROM_REMAP            ((uint32_t)0x00AA0000)          /*!< LDROM remap */

/*******************  Bit definition for RCC_12OSCCTRL register  *************/
#define RCC_12OSCCTRL_OSC_EN                ((uint32_t)0x00010000)          /*!< OSC enable */
#define RCC_12OSCCTRL_OSC_OK                ((uint32_t)0x80000000)          /*!< OSC flag bit */

/*******************  Bit definition for RCC_SYSPLLCTRL register  *************/
#define RCC_SYSPLLCTRL_FREQ                 ((uint32_t)0x0000001F)          /*!< System pll frequency */
#define RCC_REF_SEL                         ((uint32_t)0x40000000)          /*!< System pll clock source */
#define RCC_SYSPLLCTRL_FORCELOCK            ((uint32_t)0x80000000)          /*!< System pll lock */

/*******************  Bit definition for RCC_SYSPLLSTAT register  *************/
#define RCC_SYSPLLSTAT_LOCK                 ((uint32_t)0x00000001)          /*!< System pll lock bit */

/*******************  Bit definition for RCC_USBCTRL register  ****************/
#define RCC_USBCTRL_REFSEL                  ((uint32_t)0x40000000)          /*!< USB pll clock source */

/*******************  Bit definition for RCC_SYSRSTSTAT register  *************/
#define RCC_SYSRSTSTAT_PORRST               ((uint32_t)0x00000001)          /*!< POR reset */
#define RCC_SYSRSTSTAT_EXTRST               ((uint32_t)0x00000002)          /*!< EXT reset */
#define RCC_SYSRSTSTAT_WWDGRST              ((uint32_t)0x00000004)          /*!< WWDG reset */
#define RCC_SYSRSTSTAT_BORRST               ((uint32_t)0x00000008)          /*!< BOR reset */
#define RCC_SYSRSTSTAT_SYSRST               ((uint32_t)0x00000010)          /*!< SYS reset */
#define RCC_SYSRSTSTAT_IWDGRST              ((uint32_t)0x00000020)          /*!< IWDG reset */

/*******************  Bit definition for RCC_MAINCLKSEL register  *************/
#define RCC_MAINCLKSEL_SEL                  ((uint32_t)0x00000003)          /*!< Main clock source */
#define RCC_MAINCLKSEL_SEL_12M_IRC          ((uint32_t)0x00000000)          /*!< Main clock source -- 12M IRC */
#define RCC_MAINCLKSEL_SEL_SYSTEM_PLL       ((uint32_t)0x00000001)          /*!< Main clock source -- SYSTEM PLL */
#define RCC_MAINCLKSEL_SEL_12M_OSC          ((uint32_t)0x00000002)          /*!< Main clock source -- 12M OSC */
#define RCC_MAINCLKSEL_SEL_10K_IRC          ((uint32_t)0x00000003)          /*!< Main clock source -- 10K IRC */

/*******************  Bit definition for RCC_USARTCLKSEL register  ************/
#define RCC_USARTCLKSEL_SEL                 ((uint32_t)0x00000003)          /*!< USART clock source */
#define RCC_USARTCLKSEL_SEL_12M_IRC         ((uint32_t)0x00000000)          /*!< USART clock source -- 12M IRC */
#define RCC_USARTCLKSEL_SEL_SYSTEM_PLL      ((uint32_t)0x00000001)          /*!< USART clock source -- SYSTEM PLL */
#define RCC_USARTCLKSEL_SEL_12M_OSC         ((uint32_t)0x00000002)          /*!< USART clock source -- 12M OSC */
#define RCC_USARTCLKSEL_SEL_10K_IRC         ((uint32_t)0x00000003)          /*!< USART clock source -- 10K IRC */

/*******************  Bit definition for RCC_USBCLKSEL register  ************/
#define RCC_USBCLKSEL_SEL                   ((uint32_t)0x00000003)          /*!< USB clock source */
#define RCC_USBCLKSEL_SEL_12M_IRC           ((uint32_t)0x00000000)          /*!< USB clock source -- 12M IRC */
#define RCC_USBCLKSEL_SEL_12M_OSC           ((uint32_t)0x00000001)          /*!< USB clock source -- 12M OSC */

/*******************  Bit definition for RCC_WWDGCLKSEL register  *************/
#define RCC_WWDGCLKSEL_SEL                  ((uint32_t)0x00000003)          /*!< WWDG clock source */
#define RCC_WWDGCLKSEL_SEL_12M_IRC          ((uint32_t)0x00000000)          /*!< WWDG clock source -- 12M IRC */
#define RCC_WWDGCLKSEL_SEL_SYSTEM_PLL       ((uint32_t)0x00000001)          /*!< WWDG clock source -- SYSTEM PLL */
#define RCC_WWDGCLKSEL_SEL_12M_OSC          ((uint32_t)0x00000002)          /*!< WWDG clock source -- 12M OSC */
#define RCC_WWDGCLKSEL_SEL_10K_IRC          ((uint32_t)0x00000003)          /*!< WWDG clock source -- 10K IRC */

/*******************  Bit definition for RCC_OUTCLKSEL register  **************/
#define RCC_OUTCLKSEL_SEL                   ((uint32_t)0x00000007)          /*!< OUTCLK clock source */
#define RCC_OUTCLKSEL_SEL_12M_IRC           ((uint32_t)0x00000000)          /*!< OUTCLK clock source -- 12M IRC */
#define RCC_OUTCLKSEL_SEL_SYSTEM_PLL        ((uint32_t)0x00000001)          /*!< OUTCLK clock source -- SYSTEM PLL */
#define RCC_OUTCLKSEL_SEL_10K_IRC           ((uint32_t)0x00000002)          /*!< OUTCLK clock source -- 10K OSC */
#define RCC_OUTCLKSEL_SEL_12M_OSC           ((uint32_t)0x00000003)          /*!< OUTCLK clock source -- 12M IRC */
#define RCC_OUTCLKSEL_SEL_32768_OSC         ((uint32_t)0x00000004)          /*!< OUTCLK clock source -- 32768 OSC */
#define RCC_OUTCLKSEL_SEL_USB_PLL           ((uint32_t)0x00000008)          /*!< OUTCLK clock source -- USB PLL */

/*******************  Bit definition for RCC_WAKEUPEN register  ***************/
#define RCC_WAKEUPEN_MASK                   ((uint32_t)0x000000FF)          /*!< Wakeup pin mask */
#define RCC_WAKEUPEN_0                      ((uint32_t)0x00000001)          /*!< wakeup pin 0 */
#define RCC_WAKEUPEN_1                      ((uint32_t)0x00000002)          /*!< wakeup pin 1 */
#define RCC_WAKEUPEN_2                      ((uint32_t)0x00000004)          /*!< wakeup pin 2 */
#define RCC_WAKEUPEN_3                      ((uint32_t)0x00000008)          /*!< wakeup pin 3 */
#define RCC_WAKEUPEN_4                      ((uint32_t)0x00000010)          /*!< wakeup pin 4 */
#define RCC_WAKEUPEN_5                      ((uint32_t)0x00000020)          /*!< wakeup pin 5 */
#define RCC_WAKEUPEN_6                      ((uint32_t)0x00000040)          /*!< wakeup pin 6 */
#define RCC_WAKEUPEN_7                      ((uint32_t)0x00000080)          /*!< wakeup pin 7 */

/*******************  Bit definition for RCC_PORSTATUS register  **************/
#define RCC_PORSTATUS_RTC                   ((uint32_t)0x00000001)          /*!< RTC POR flag bit */

/*******************  Bit definition for RCC_BORCTRL register  ****************/
#define RCC_BORCTRL_BORLEV_MASK             ((uint32_t)0x00000003)          /*!< Mask of BOR level */
#define RCC_BORCTRL_BORLEV_3_7V             ((uint32_t)0x00000000)          /*!< BOR level 3.7V */
#define RCC_BORCTRL_BORLEV_2_7V             ((uint32_t)0x00000001)          /*!< BOR level 2.7V */
#define RCC_BORCTRL_BORLEV_2_2V             ((uint32_t)0x00000002)          /*!< BOR level 2.2V */
#define RCC_BORCTRL_BORLEV_1_8V             ((uint32_t)0x00000003)          /*!< BOR level 1.8V */
#define RCC_BORCTRL_BORENA                  ((uint32_t)0x00000004)          /*!< Enable BOR */
#define RCC_BORCTRL_BODLEV_MASK             ((uint32_t)0x00180000)          /*!< Mask of BOD level */
#define RCC_BORCTRL_BODLEV_4_4V             ((uint32_t)0x00000000)          /*!< BOD level 4.4V */
#define RCC_BORCTRL_BODLEV_3_7V             ((uint32_t)0x00080000)          /*!< BOD level 3.7V */
#define RCC_BORCTRL_BODLEV_2_7V             ((uint32_t)0x00100000)          /*!< BOD level 2.7V */
#define RCC_BORCTRL_BODLEV_2_2V             ((uint32_t)0x00180000)          /*!< BOD level 2.2V */

/*******************  Bit definition for RCC_PDSLEEPCFG register  ****************/
#define RCC_PDSLEEPCFG_12MOSC               ((uint32_t)0x00000001)          /*!< Power down 12M OSC */
#define RCC_PDSLEEPCFG_ADC                  ((uint32_t)0x00000004)          /*!< Power down ADC */
#define RCC_PDSLEEPCFG_SYSPLL               ((uint32_t)0x00000008)          /*!< Power down SYSPLL */
#define RCC_PDSLEEPCFG_BOD                  ((uint32_t)0x00000010)          /*!< Power down BOD */
#define RCC_PDSLEEPCFG_BOR                  ((uint32_t)0x00000020)          /*!< Power down BOR */
#define RCC_PDSLEEPCFG_12MIRC               ((uint32_t)0x00000080)          /*!< Power down 12MIRC */
#define RCC_PDSLEEPCFG_FLASH                ((uint32_t)0x00000100)          /*!< Power down FLASH */
#define RCC_PDSLEEPCFG_ACMP                 ((uint32_t)0x00000200)          /*!< Power down ACMP */
#define RCC_PDSLEEPCFG_USB                  ((uint32_t)0x00000400)          /*!< Power down USB */
#define RCC_PDSLEEPCFG_32KOSC               ((uint32_t)0x00000800)          /*!< Power down 32KOSC */
#define RCC_PDSLEEPCFG_DAC0                 ((uint32_t)0x00001000)          /*!< Power down DAC0 */
#define RCC_PDSLEEPCFG_DAC1                 ((uint32_t)0x00002000)          /*!< Power down DAC1 */
#define RCC_PDSLEEPCFG_USBLDO               ((uint32_t)0x00004000)          /*!< Power down USB LDO */

/*******************  Bit definition for RCC_PDAWAKECFG register  ****************/
#define RCC_PDAWAKECFG_12MOSC               ((uint32_t)0x00000001)          /*!< Power on 12M OSC */
#define RCC_PDAWAKECFG_ADC                  ((uint32_t)0x00000004)          /*!< Power on ADC */
#define RCC_PDAWAKECFG_SYSPLL               ((uint32_t)0x00000008)          /*!< Power on SYSPLL */
#define RCC_PDAWAKECFG_BOD                  ((uint32_t)0x00000010)          /*!< Power on BOD */
#define RCC_PDAWAKECFG_BOR                  ((uint32_t)0x00000020)          /*!< Power on BOR */
#define RCC_PDAWAKECFG_12MIRC               ((uint32_t)0x00000080)          /*!< Power on 12MIRC */
#define RCC_PDAWAKECFG_FLASH                ((uint32_t)0x00000100)          /*!< Power on FLASH */
#define RCC_PDAWAKECFG_ACMP                 ((uint32_t)0x00000200)          /*!< Power on ACMP */
#define RCC_PDAWAKECFG_USB                  ((uint32_t)0x00000400)          /*!< Power on USB */
#define RCC_PDAWAKECFG_32KOSC               ((uint32_t)0x00000800)          /*!< Power on 32KOSC */
#define RCC_PDAWAKECFG_DAC0                 ((uint32_t)0x00001000)          /*!< Power on DAC0 */
#define RCC_PDAWAKECFG_DAC1                 ((uint32_t)0x00002000)          /*!< Power on DAC1 */
#define RCC_PDAWAKECFG_USBLDO               ((uint32_t)0x00004000)          /*!< Power on USB LDO */

/*******************  Bit definition for RCC_PDRUNCFG register  ****************/
#define RCC_PDRUNCFG_12MOSC                 ((uint32_t)0x00000001)          /*!< Power on/down 12M OSC */
#define RCC_PDRUNCFG_ADC                    ((uint32_t)0x00000004)          /*!< Power on/down ADC */
#define RCC_PDRUNCFG_SYSPLL                 ((uint32_t)0x00000008)          /*!< Power on/down SYSPLL */
#define RCC_PDRUNCFG_BOD                    ((uint32_t)0x00000010)          /*!< Power on/down BOD */
#define RCC_PDRUNCFG_BOR                    ((uint32_t)0x00000020)          /*!< Power on/down BOR */
#define RCC_PDRUNCFG_12MIRC                 ((uint32_t)0x00000080)          /*!< Power on/down 12MIRC */
#define RCC_PDRUNCFG_FLASH                  ((uint32_t)0x00000100)          /*!< Power on/down FLASH */
#define RCC_PDRUNCFG_ACMP                   ((uint32_t)0x00000200)          /*!< Power on/down ACMP */
#define RCC_PDRUNCFG_USB                    ((uint32_t)0x00000400)          /*!< Power on/down USB */
#define RCC_PDRUNCFG_32KOSC                 ((uint32_t)0x00000800)          /*!< Power on/down 32KOSC */
#define RCC_PDRUNCFG_DAC0                   ((uint32_t)0x00001000)          /*!< Power on/down DAC0 */
#define RCC_PDRUNCFG_DAC1                   ((uint32_t)0x00002000)          /*!< Power on/down DAC1 */
#define RCC_PDRUNCFG_USBLDO                 ((uint32_t)0x00004000)          /*!< Power on/down USB LDO */
 
/*******************  Bit definition for RCC_DMACTRL register  ****************/
#define RCC_DMACTRL_CH4_I2C0_TX             ((uint32_t)0x00000000)          /*!< DMA channel 4 connect to I2C0_TX */
#define RCC_DMACTRL_CH4_ADC                 ((uint32_t)0x00000010)          /*!< DMA channel 4 connect to ADC */
#define RCC_DMACTRL_CH5_I2C1_RX             ((uint32_t)0x00000000)          /*!< DMA channel 5 connect to I2C1_RX */
#define RCC_DMACTRL_CH5_DAC0                ((uint32_t)0x00000020)          /*!< DMA channel 5 connect to DAC0 */
#define RCC_DMACTRL_CH6_I2C1_TX             ((uint32_t)0x00000000)          /*!< DMA channel 6 connect to I2C1_TX */
#define RCC_DMACTRL_CH6_DAC1                ((uint32_t)0x00000040)          /*!< DMA channel 6 connect to DAC1 */

/*******************  Bit definition for RCC_PCON register  *******************/
#define RCC_PCON_DPDEN                      ((uint32_t)0x00000002)          /*!< Deep power down */
#define RCC_PCON_SLEEP                      ((uint32_t)0x00000100)          /*!< Deep sleep flag bit */
#define RCC_PCON_DPFLAG                     ((uint32_t)0x00000800)          /*!< Deep power down flag bit */
#define RCC_PCON_LOWPOWER                   ((uint32_t)0x00010000)          /*!< Change LDO source */
#define RCC_PCON_LDO_STATUS                 ((uint32_t)0x00100000)          /*!< Main LDO flag bit */

/*******************  Bit definition for RCC_DBGCTRL register  ****************/
#define RCC_DBGCTRL_IWDG                    ((uint32_t)0x00000010)          /*!< Stop IWDG count when SW debug */
#define RCC_DBGCTRL_WWDG                    ((uint32_t)0x00000020)          /*!< Stop WWDG count when SW debug */

/******************************************************************************/
/*                                                                            */
/*         Universal Synchronous Asynchronous Receiver Transmitter (USART)    */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for HW_USART_CTRL0 register  *************/
#define USART_CTRL0_XFER_COUNT              ((uint32_t)0x0000FFFF)          /*!< Number of bytes to receive. */
#define USART_CTRL0_RXTIMEOUT               ((uint32_t)0x00FF0000)          /*!< Receive Timeout Counter Value: */
#define USART_CTRL0_RXTO_ENABLE             ((uint32_t)0x01000000)          /*!< RXTIMEOUT Enable */
#define USART_CTRL0_RX_SOURCE               ((uint32_t)0x02000000)          /*!< Source of Receive Data */
#define USART_CTRL0_RUN                     ((uint32_t)0x10000000)          /*!< Tell the USART to execute the RX DMA Command. */
#define USART_CTRL0_CLKGATE                 ((uint32_t)0x40000000)          /*!< Gates of clock */
#define USART_CTRL0_SFTRST                  ((uint32_t)0x80000000)          /*!< Set to zero for normal operation,set to one to reset entire block */

/*******************  Bit definition for HW_USART_CTRL1 register  *************/
#define USART_CTRL1_XFER_COUNT         	    ((uint32_t)0x0000FFFF)          /*!< Number of bytes to transmit. */
#define USART_CTRL1_RUN                     ((uint32_t)0x10000000)          /*!< Tell the USART to execute the TX DMA Command. */

/******************  Bit definition for HW_USART_CTRL2 register  **************/
#define USART_CTRL2_USARTEN                 ((uint32_t)0x00000001)          /*!< USART Enable. */
#define USART_CTRL2_SIREN                   ((uint32_t)0x00000002)          /*!< SIR Enable. */
#define USART_CTRL2_SIRLP                   ((uint32_t)0x00000004)          /*!< IrDA SIR Low Power Mode. */
#define USART_CTRL2_LBE                     ((uint32_t)0x00000080)          /*!< Loop Back Enable. */
#define USART_CTRL2_TXE                     ((uint32_t)0x00000100)          /*!< Transmit Enable. */
#define USART_CTRL2_RXE                     ((uint32_t)0x00000200)          /*!< Receive Enable. */
#define USART_CTRL2_DTR                     ((uint32_t)0x00000400)          /*!< Data Transmit Ready.*/
#define USART_CTRL2_RTS                     ((uint32_t)0x00000800)          /*!< Request To Send. */
#define USART_CTRL2_OUT1                    ((uint32_t)0x00001000)          /*!< This bit is the complement of the USART Out1 (nUSARTOut1) modem status output. */
#define USART_CTRL2_OUT2                    ((uint32_t)0x00002000)          /*!< This bit is the complement of the USART Out2 (nUSARTOut2) modem status output. */
#define USART_CTRL2_RTSEN                   ((uint32_t)0x00004000)          /*!< RTS Hardware Flow Control Enable. */
#define USART_CTRL2_CTSEN                   ((uint32_t)0x00008000)          /*!< CTS Hardware Flow Control Enable.*/
#define USART_CTRL2_TXIFLSEL                ((uint32_t)0x000F0000)          /*!< Transmit Interrupt FIFO Level Select. */
#define USART_CTRL2_TXIFLSEL_0              ((uint32_t)0x00010000)          /*!< Transmit Interrupt FIFO Level Select. */
#define USART_CTRL2_TXIFLSEL_1              ((uint32_t)0x00020000)          /*!< Transmit Interrupt FIFO Level Select. */
#define USART_CTRL2_TXIFLSEL_2              ((uint32_t)0x00040000)          /*!< Transmit Interrupt FIFO Level Select. */
#define USART_CTRL2_RXIFLSEL                ((uint32_t)0x00F00000)          /*!< Receive Interrupt FIFO Level Select. */
#define USART_CTRL2_RXIFLSEL_0              ((uint32_t)0x00100000)          /*!< Receive Interrupt FIFO Level Select. */
#define USART_CTRL2_RXIFLSEL_1              ((uint32_t)0x00200000)          /*!< Receive Interrupt FIFO Level Select. */
#define USART_CTRL2_RXIFLSEL_2            	((uint32_t)0x00400000)          /*!< Receive Interrupt FIFO Level Select. */
#define USART_CTRL2_RXDMAE                  ((uint32_t)0x01000000)          /*!< Receive DMA Enable.*/
#define USART_CTRL2_TXDMAE                  ((uint32_t)0x02000000)          /*!< Transmit DMA Enable. */
#define USART_CTRL2_DMAONERROR            	((uint32_t)0x04000000)          /*!< DMA On Error. */

/******************  Bit definition for HW_USART_LINECTRL register  ***********/
#define USART_LINECTRL_BRK                	((uint32_t)0x00000001)          /*!< Send Break */
#define USART_LINECTRL_PEN                  ((uint32_t)0x00000002)          /*!< Parity Enable. */
#define USART_LINECTRL_EPS                  ((uint32_t)0x00000004)          /*!< Even Parity Select. */
#define USART_LINECTRL_STP2                 ((uint32_t)0x00000008)          /*!< Even Parity Select. */
#define USART_LINECTRL_FEN                	((uint32_t)0x00000010)          /*!< Enable FIFOs. */
#define USART_LINECTRL_WLEN               	((uint32_t)0x00000060)          /*!< Word length [1:0]. */
#define USART_LINECTRL_WLEN_0               ((uint32_t)0x00000020)          /*!< Word length [1:0]. */
#define USART_LINECTRL_WLEN_1               ((uint32_t)0x00000040)          /*!< Word length [1:0]. */
#define USART_LINECTRL_SPS                	((uint32_t)0x00000080)          /*!< Stick Parity Select. */
#define USART_LINECTRL_BAUD_DIVFRA          ((uint32_t)0x00003F00)          /*!< Baud Rate Fraction [5:0]. */
#define USART_LINECTRL_BAUD_DIVINT          ((uint32_t)0xFFFF0000)          /*!< Baud Rate Integer [15:0]. */

/******************  Bit definition for HW_USART_INTR register  ***************/
#define USART_INTR_RIMIS                  	((uint32_t)0x00000001)          /*!< nUSARTRI Modem Interrupt Status. */
#define USART_INTR_CTSMIS                   ((uint32_t)0x00000002)          /*!< nUSARTCTS Modem Interrupt Status. */
#define USART_INTR_DCDMIS                   ((uint32_t)0x00000004)          /*!< nUSARTDCD Modem Interrupt Status. */
#define USART_INTR_DSRMIS                 	((uint32_t)0x00000008)          /*!< nUSARTDSR Modem Interrupt Status. */
#define USART_INTR_RXIS                     ((uint32_t)0x00000010)          /*!< nUSARTDSR Modem Interrupt Status. */
#define USART_INTR_TXIS                     ((uint32_t)0x00000020)          /*!< Transmit Interrupt Status. */
#define USART_INTR_RTIS                     ((uint32_t)0x00000040)          /*!< Receive Timeout Interrupt Status. */
#define USART_INTR_FEIS                     ((uint32_t)0x00000080)          /*!< Framing Error Interrupt Status. */
#define USART_INTR_PEIS                     ((uint32_t)0x00000100)          /*!< Parity Error Interrupt Status. */
#define USART_INTR_BEIS                     ((uint32_t)0x00000200)          /*!< Break Error Interrupt Status. */
#define USART_INTR_OEIS                     ((uint32_t)0x00000400)          /*!< Break Error Interrupt Status. */
#define USART_INTR_TFEIS                    ((uint32_t)0x00000800)          /*!< Tx FIFO EMPTY Raw Interrupt status*/
#define USART_INTR_ABEO                     ((uint32_t)0x00001000)          /*!< Auto Buadrate End Interrupt Status. */
#define USART_INTR_ABTO                   	((uint32_t)0x00002000)          /*!< Auto Buadrate TimeOut Interrupt Status. */
#define USART_INTR_RIMIEN                 	((uint32_t)0x00010000)          /*!< nUSARTRI Modem Interrupt Enable. */
#define USART_INTR_CTSMIEN                  ((uint32_t)0x00020000)          /*!< nUSARTCTS Modem Interrupt Enable.*/
#define USART_INTR_DCDMIEN                  ((uint32_t)0x00040000)          /*!< nUSARTDCD Modem Interrupt Enable. */
#define USART_INTR_DSRMIEN                  ((uint32_t)0x00080000)          /*!< nUSARTDSR Modem Interrupt Enable. */
#define USART_INTR_RXIEN                    ((uint32_t)0x00100000)          /*!< Receive Interrupt Enable. */
#define USART_INTR_TXIEN                    ((uint32_t)0x00200000)          /*!< Transmit Interrupt Enable. */
#define USART_INTR_RTIEN                  	((uint32_t)0x00400000)          /*!< Receive Timeout Interrupt Enable. */
#define USART_INTR_FEIEN                    ((uint32_t)0x00800000)          /*!< Framing Error Interrupt Enable.*/
#define USART_INTR_PEIEN                  	((uint32_t)0x01000000)          /*!< Parity Error Interrupt Enable.*/
#define USART_INTR_BEIEN                    ((uint32_t)0x02000000)          /*!< Break Error Interrupt Enable. */
#define USART_INTR_OEIEN                    ((uint32_t)0x04000000)          /*!< Overrun Error Interrupt Enable. */
#define USART_INTR_TFEIEN                   ((uint32_t)0x08000000)          /*!< Overrun Error Interrupt Enable.*/
#define USART_INTR_ABEOEN                 	((uint32_t)0x10000000)          /*!< Auto Buadrate End Interrupt Enable.*/
#define USART_INTR_ABTOEN                 	((uint32_t)0x20000000)          /*!< Auto Buadrate TimeOut Interrupt Enable.*/

/******************  Bit definition for HW_USART_DATA register  ***************/
#define USART_DATA                          ((uint32_t)0xFFFFFFFF)          /*!< The status register contains the receive data flags and valid bits. */

/******************  Bit definition for HW_USART_STAT register  ***************/
#define USART_STAT_RXCOUNT                  ((uint32_t)0x0000FFFF)          /*!< Number of bytes received during a Receive DMA command. */
#define USART_STAT_FERR                     ((uint32_t)0x00010000)          /*!< Framing Error. */
#define USART_STAT_PERR                     ((uint32_t)0x00020000)          /*!< Parity Error. */
#define USART_STAT_BERR                     ((uint32_t)0x00040000)          /*!< Break Error. */
#define USART_STAT_OERR                     ((uint32_t)0x00080000)          /*!< Overrun Error.*/
#define USART_STAT_RXBYTE_INVALID           ((uint32_t)0x00F00000)          /*!< The invalid state of the last read of Receive Data. */
#define USART_STAT_RXFE                     ((uint32_t)0x01000000)          /*!< Receive FIFO Empty. */
#define USART_STAT_TXFF                   	((uint32_t)0x02000000)          /*!< Receive FIFO Empty. */
#define USART_STAT_RXFF                     ((uint32_t)0x04000000)          /*!< Receive FIFO Full. */
#define USART_STAT_TXFE                     ((uint32_t)0x08000000)          /*!< Transmit FIFO Empty. */
#define USART_STAT_CTS                    	((uint32_t)0x10000000)          /*!< Clear To Send. */
#define USART_STAT_BUSY                   	((uint32_t)0x20000000)          /*!< USART Busy. */
#define USART_STAT_HISPEED                	((uint32_t)0x40000000)          /*!< indicates that the high-speed function is present. */
#define USART_STAT_PRESENT                  ((uint32_t)0x80000000)          /*!< indicates that the Application USART function is present. */

/******************  Bit definition for HW_USART_DEBUG register  **************/
#define USART_DEBUG_RXDMARQ               	((uint32_t)0x00000001)          /*!< USART Receive DMA Request Status. */
#define USART_DEBUG_TXDMARQ               	((uint32_t)0x00000002)          /*!< USART Transmit DMA Request Status. */
#define USART_DEBUG_RXCMDEND                ((uint32_t)0x00000004)          /*!< USART Receive DMA Command End Status. */
#define USART_DEBUG_TXCMDEND              	((uint32_t)0x00000008)          /*!< USART Transmit DMA Command End Status. */
#define USART_DEBUG_RXDMARUN              	((uint32_t)0x00000010)          /*!< USART Receive DMA Command Run Status. */
#define USART_DEBUG_TXDMARUN                ((uint32_t)0x00000020)          /*!< USART Transmit DMA Command Run Status. */

/******************  Bit definition for HW_USART_ILPR register  ***************/
#define USART_ILPDVSR                       ((uint32_t)0x000000FF)          /*!< The baud rate divisor of the IRDA_LowPower */

/******************  Bit definition for HW_USART_RS485CTRL register ***********/
#define USART_RS485CTRL_NMMEN             	((uint32_t)0x00000001)          /*!< Multidrop Mode. */
#define USART_RS485CTRL_RXDIS             	((uint32_t)0x00000002)          /*!< Enable receiver. */
#define USART_RS485CTRL_AADEN             	((uint32_t)0x00000004)          /*!< Auto Address Detect. */
#define USART_RS485CTRL_SEL               	((uint32_t)0x00000008)          /*!< Direction control select. */
#define USART_RS485CTRL_DCTRL               ((uint32_t)0x00000010)          /*!< Auto Direction Control. */
#define USART_RS485CTRL_ONIV                ((uint32_t)0x00000020)          /*!< reverses the polarity of the direction control signal. */

/******************  Bit definition for HW_USART_RS485ADRMATCH register *******/
#define USART_ADRMATCH                      ((uint32_t)0x000000FF)          /*!< Contains the address match value. */

/******************  Bit definition for HW_USART_RS485DLY register  ***********/
#define USART_DLY                           ((uint32_t)0x000000FF)          /*!< Contains the direction control (RTS or DTR) delay value. */

/******************  Bit definition for HW_USART_AUTOBAUD register  ***********/
#define USART_AUTOBAUD_START                ((uint32_t)0x00000001)          /*!< Start auto baudrate. */
#define USART_AUTOBAUD_MODE                 ((uint32_t)0x00000002)          /*!< Auto-baud mode select. */
#define USART_AUTOBAUD_AUTORESTART          ((uint32_t)0x00000004)          /*!< Auto Restart. */
#define USART_AUTOBAUD_ABEOIntClr           ((uint32_t)0x00000100)          /*!< End of auto-baud interrupt clear bit. */
#define USART_AUTOBAUD_ABTOIntClr           ((uint32_t)0x00000200)          /*!< End of auto-baud interrupt clear bit. */

/******************  Bit definition for HW_USART_LINCTRL register  ************/
#define USART_LINCTRL_EN                    ((uint32_t)0x00000001)          /*!< Lin enable. */
#define USART_LINCTRL_SBK                   ((uint32_t)0x00000002)          /*!< Send break. */
#define USART_LINCTRL_LBDL                  ((uint32_t)0x00000004)          /*!< LIN break detection length. */
#define USART_LINCTRL_LBDIE                 ((uint32_t)0x00000008)          /*!< Lin interrupt enable. */
#define USART_LINCTRL_LBD                   ((uint32_t)0x00000010)          /*!< Lin interrupt flag. */

/******************  Bit definition for HW_USART_RXTIMEROUT register  *********/
#define USART_RXTIMEROUT_COUNT_MASK         ((uint32_t)0x000FFFFF)          /*!< RXTIMEROUT count. */
#define USART_RXTIMEROUT_IE                 ((uint32_t)0x00100000)          /*!< RXTIMEROUT interrupt enable. */
#define USART_RXTIMEROUT_IFLAG              ((uint32_t)0x00200000)          /*!< RXTIMEROUT interrupt flag. */
#define USART_RXTIMEROUT_STTTO              ((uint32_t)0x00400000)          /*!< STTTO. */
#define USART_RXTIMEROUT_RETTO              ((uint32_t)0x00800000)          /*!< RETTO. */

/******************************************************************************/
/*                                                                            */
/*                           Flash Memory Control                			  */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for HW_FLASH_ACR register  ***************/
#define FLASH_ACR_LATENCY                   ((uint32_t)0x00000003)          /*!< LATENCY bit (Latency) */
#define FLASH_ACR_LATENCY_0                 ((uint32_t)0x00000000)          /*!< LATENCY bit (Latency) 0 */
#define FLASH_ACR_LATENCY_1                 ((uint32_t)0x00000001)          /*!< LATENCY bit (Latency) 1 */
#define FLASH_ACR_LATENCY_2                 ((uint32_t)0x00000002)          /*!< LATENCY bit (Latency) 2 */
#define FLASH_ACR_LATENCY_3                 ((uint32_t)0x00000003)          /*!< LATENCY bit (Latency) 3 */
#define FLASH_ACR_CACHENA                   ((uint32_t)0x00000100)          /*!< Cache Enable */
#define FLASH_ACR_FLUSH                     ((uint32_t)0x00010000)          /*!< Flush Cache */

/*******************  Bit definition for HW_FLASH_KEYR register  **************/
#define FLASH_KEYR                          ((uint32_t)0xFFFFFFFF)          /*!< CR Key */

/******************  FLASH Keys  **********************************************/
#define FLASH_FKEY1                         ((uint32_t)0x76543210)          /*!< Flash program erase key1 */
#define FLASH_FKEY2                         ((uint32_t)0xFEDCBA98)          /*!< Flash program erase key2: used with FLASH_PEKEY1
                                                                                to unlock the write access to the CR. */

/*******************  Bit definition for HW_FLASH_KEYR register  **************/
#define FLASH_ERASER_NUM                    ((uint32_t)0x0000003F)          /*!< Flash sector number */
#define FLASH_ERASER_MAP_FLAG               ((uint32_t)0x10000000)          /*!< Flash address 0x10000000 */

/*******************  Bit definition for HW_FLASH_SR register  ****************/
#define FLASH_SR_EOP                        ((uint32_t)0x00000001)          /*!< End of operation */
#define FLASH_SR_EOE                        ((uint32_t)0x00000002)          /*!< End of erase */
#define FLASH_SR_BUSY                       ((uint32_t)0x00010000)          /*!< Busy */

/*******************  Bit definition for HW_FLASH_CR register  ****************/
#define FLASH_CR_PG                         ((uint32_t)0x00000001)          /*!< Programming */
#define FLASH_CR_SER                        ((uint32_t)0x00000002)          /*!< Sector erase */
#define FLASH_CR_MER                        ((uint32_t)0x00000004)          /*!< Mass erase */
#define FLASH_CR_NVR                        ((uint32_t)0x00000008)          /*!< Main or Info */
#define FLASH_CR_LDROMEN                    ((uint32_t)0x00000100)          /*!< LDROM */
#define FLASH_CR_START                      ((uint32_t)0x00010000)          /*!< Start */
#define FLASH_CR_EOPIE                      ((uint32_t)0x00100000)          /*!< End of operation interrupt enable */
#define FLASH_CR_EOEIE                      ((uint32_t)0x00200000)          /*!< End of erase interrupt enable */
#define FLASH_CR_OPENNVR                    ((uint32_t)0x20000000)          /*!< Enable NVR read */
#define FLASH_CR_LOCK                       ((uint32_t)0x80000000)          /*!< CR lock */

/******************************************************************************/
/*                                                                            */
/*                                 IOCON                                      */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for IOCON register  **********************/
#define	IOCON_FUNC_MASK                     ((uint32_t)0x00000003)          /*!< IO function mask. */
#define	IOCON_FUNC_0                        ((uint32_t)0x00000000)          /*!< IO function 0. */
#define	IOCON_FUNC_1                        ((uint32_t)0x00000001)          /*!< IO function 1. */
#define	IOCON_FUNC_2                        ((uint32_t)0x00000002)          /*!< IO function 2. */
#define	IOCON_FUNC_3                        ((uint32_t)0x00000003)          /*!< IO function 3. */
#define	IOCON_MODE_MASK                     ((uint32_t)0x00000018)          /*!< IO mode mask. */
#define	IOCON_MODE_0                        ((uint32_t)0x00000000)          /*!< IO floating. */
#define	IOCON_MODE_1                        ((uint32_t)0x00000008)          /*!< IO pull down. */
#define	IOCON_MODE_2                        ((uint32_t)0x00000010)          /*!< IO pull up. */
#define	IOCON_MODE_3                        ((uint32_t)0x00000018)          /*!< reserved. */
#define	IOCON_ANA                           ((uint32_t)0x00000020)          /*!< IO Analog mode. */
#define	IOCON_IE                            ((uint32_t)0x00000080)          /*!< IO in enable. */
#define	IOCON_DR                            ((uint32_t)0x00000100)          /*!< IO Drive Capability. */
#define	IOCON_SR                            ((uint32_t)0x00000200)          /*!< IO speed. */

/******************************************************************************/
/*                                                                            */
/*                General Purpose and Alternate Function I/O                  */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for GPIO_DATA register  ******************/
#define GPIO_DATA_BIT0                      ((uint32_t)0x00000001)          /*!< Pin data, bit 0 */
#define GPIO_DATA_BIT1                      ((uint32_t)0x00000002)          /*!< Pin data, bit 1 */
#define GPIO_DATA_BIT2                      ((uint32_t)0x00000004)          /*!< Pin data, bit 2 */
#define GPIO_DATA_BIT3                      ((uint32_t)0x00000008)          /*!< Pin data, bit 3 */
#define GPIO_DATA_BIT4                      ((uint32_t)0x00000010)          /*!< Pin data, bit 4 */
#define GPIO_DATA_BIT5                      ((uint32_t)0x00000020)          /*!< Pin data, bit 5 */
#define GPIO_DATA_BIT6                      ((uint32_t)0x00000040)          /*!< Pin data, bit 6 */
#define GPIO_DATA_BIT7                      ((uint32_t)0x00000080)          /*!< Pin data, bit 7 */
#define GPIO_DATA_BIT8                      ((uint32_t)0x00000100)          /*!< Pin data, bit 8 */
#define GPIO_DATA_BIT9                      ((uint32_t)0x00000200)          /*!< Pin data, bit 9 */
#define GPIO_DATA_BIT10                     ((uint32_t)0x00000400)          /*!< Pin data, bit 10 */
#define GPIO_DATA_BIT11                     ((uint32_t)0x00000800)          /*!< Pin data, bit 11 */
#define GPIO_DATA_BIT12                     ((uint32_t)0x00001000)          /*!< Pin data, bit 12 */
#define GPIO_DATA_BIT13                     ((uint32_t)0x00002000)          /*!< Pin data, bit 13 */
#define GPIO_DATA_BIT14                     ((uint32_t)0x00004000)          /*!< Pin data, bit 14 */
#define GPIO_DATA_BIT15                     ((uint32_t)0x00008000)          /*!< Pin data, bit 15 */
#define GPIO_DATA_BIT16                     ((uint32_t)0x00010000)          /*!< Pin data, bit 16 */
#define GPIO_DATA_BIT17                     ((uint32_t)0x00020000)          /*!< Pin data, bit 17 */
#define GPIO_DATA_BIT18                     ((uint32_t)0x00040000)          /*!< Pin data, bit 18 */
#define GPIO_DATA_BIT19                     ((uint32_t)0x00080000)          /*!< Pin data, bit 19 */
#define GPIO_DATA_BIT20                     ((uint32_t)0x00100000)          /*!< Pin data, bit 20 */
#define GPIO_DATA_BIT21                     ((uint32_t)0x00200000)          /*!< Pin data, bit 21 */
#define GPIO_DATA_BIT22                     ((uint32_t)0x00400000)          /*!< Pin data, bit 22 */
#define GPIO_DATA_BIT23                     ((uint32_t)0x00800000)          /*!< Pin data, bit 23 */
#define GPIO_DATA_BIT24                     ((uint32_t)0x01000000)          /*!< Pin data, bit 24 */
#define GPIO_DATA_BIT25                     ((uint32_t)0x02000000)          /*!< Pin data, bit 25 */
#define GPIO_DATA_BIT26                     ((uint32_t)0x04000000)          /*!< Pin data, bit 26 */
#define GPIO_DATA_BIT27                     ((uint32_t)0x08000000)          /*!< Pin data, bit 27 */
#define GPIO_DATA_BIT28                     ((uint32_t)0x10000000)          /*!< Pin data, bit 28 */
#define GPIO_DATA_BIT29                     ((uint32_t)0x20000000)          /*!< Pin data, bit 29 */
#define GPIO_DATA_BIT30                     ((uint32_t)0x40000000)          /*!< Pin data, bit 30 */
#define GPIO_DATA_BIT31                     ((uint32_t)0x80000000)          /*!< Pin data, bit 31 */

/*******************  Bit definition for GPIO_DIR register  *******************/
#define GPIO_DIR_BIT0                       ((uint32_t)0x00000001)          /*!< Pin dir, bit 0 */
#define GPIO_DIR_BIT1                       ((uint32_t)0x00000002)          /*!< Pin dir, bit 1 */
#define GPIO_DIR_BIT2                       ((uint32_t)0x00000004)          /*!< Pin dir, bit 2 */
#define GPIO_DIR_BIT3                       ((uint32_t)0x00000008)          /*!< Pin dir, bit 3 */
#define GPIO_DIR_BIT4                       ((uint32_t)0x00000010)          /*!< Pin dir, bit 4 */
#define GPIO_DIR_BIT5                       ((uint32_t)0x00000020)          /*!< Pin dir, bit 5 */
#define GPIO_DIR_BIT6                       ((uint32_t)0x00000040)          /*!< Pin dir, bit 6 */
#define GPIO_DIR_BIT7                       ((uint32_t)0x00000080)          /*!< Pin dir, bit 7 */
#define GPIO_DIR_BIT8                       ((uint32_t)0x00000100)          /*!< Pin dir, bit 8 */
#define GPIO_DIR_BIT9                       ((uint32_t)0x00000200)          /*!< Pin dir, bit 9 */
#define GPIO_DIR_BIT10                      ((uint32_t)0x00000400)          /*!< Pin dir, bit 10 */
#define GPIO_DIR_BIT11                      ((uint32_t)0x00000800)          /*!< Pin dir, bit 11 */
#define GPIO_DIR_BIT12                      ((uint32_t)0x00001000)          /*!< Pin dir, bit 12 */
#define GPIO_DIR_BIT13                      ((uint32_t)0x00002000)          /*!< Pin dir, bit 13 */
#define GPIO_DIR_BIT14                      ((uint32_t)0x00004000)          /*!< Pin dir, bit 14 */
#define GPIO_DIR_BIT15                      ((uint32_t)0x00008000)          /*!< Pin dir, bit 15 */
#define GPIO_DIR_BIT16                      ((uint32_t)0x00010000)          /*!< Pin dir, bit 16 */
#define GPIO_DIR_BIT17                      ((uint32_t)0x00020000)          /*!< Pin dir, bit 17 */
#define GPIO_DIR_BIT18                      ((uint32_t)0x00040000)          /*!< Pin dir, bit 18 */
#define GPIO_DIR_BIT19                      ((uint32_t)0x00080000)          /*!< Pin dir, bit 19 */
#define GPIO_DIR_BIT20                      ((uint32_t)0x00100000)          /*!< Pin dir, bit 20 */
#define GPIO_DIR_BIT21                      ((uint32_t)0x00200000)          /*!< Pin dir, bit 21 */
#define GPIO_DIR_BIT22                      ((uint32_t)0x00400000)          /*!< Pin dir, bit 22 */
#define GPIO_DIR_BIT23                      ((uint32_t)0x00800000)          /*!< Pin dir, bit 23 */
#define GPIO_DIR_BIT24                      ((uint32_t)0x01000000)          /*!< Pin dir, bit 24 */
#define GPIO_DIR_BIT25                      ((uint32_t)0x02000000)          /*!< Pin dir, bit 25 */
#define GPIO_DIR_BIT26                      ((uint32_t)0x04000000)          /*!< Pin dir, bit 26 */
#define GPIO_DIR_BIT27                      ((uint32_t)0x08000000)          /*!< Pin dir, bit 27 */
#define GPIO_DIR_BIT28                      ((uint32_t)0x10000000)          /*!< Pin dir, bit 28 */
#define GPIO_DIR_BIT29                      ((uint32_t)0x20000000)          /*!< Pin dir, bit 29 */
#define GPIO_DIR_BIT30                      ((uint32_t)0x40000000)          /*!< Pin dir, bit 30 */
#define GPIO_DIR_BIT31                      ((uint32_t)0x80000000)          /*!< Pin dir, bit 31 */

/*******************  Bit definition for GPIO_IS register  *******************/
#define GPIO_IS_BIT0                        ((uint32_t)0x00000001)          /*!< Pin is, bit 0 */
#define GPIO_IS_BIT1                        ((uint32_t)0x00000002)          /*!< Pin is, bit 1 */
#define GPIO_IS_BIT2                        ((uint32_t)0x00000004)          /*!< Pin is, bit 2 */
#define GPIO_IS_BIT3                        ((uint32_t)0x00000008)          /*!< Pin is, bit 3 */
#define GPIO_IS_BIT4                        ((uint32_t)0x00000010)          /*!< Pin is, bit 4 */
#define GPIO_IS_BIT5                        ((uint32_t)0x00000020)          /*!< Pin is, bit 5 */
#define GPIO_IS_BIT6                        ((uint32_t)0x00000040)          /*!< Pin is, bit 6 */
#define GPIO_IS_BIT7                        ((uint32_t)0x00000080)          /*!< Pin is, bit 7 */
#define GPIO_IS_BIT8                        ((uint32_t)0x00000100)          /*!< Pin is, bit 8 */
#define GPIO_IS_BIT9                        ((uint32_t)0x00000200)          /*!< Pin is, bit 9 */
#define GPIO_IS_BIT10                       ((uint32_t)0x00000400)          /*!< Pin is, bit 10 */
#define GPIO_IS_BIT11                       ((uint32_t)0x00000800)          /*!< Pin is, bit 11 */
#define GPIO_IS_BIT12                       ((uint32_t)0x00001000)          /*!< Pin is, bit 12 */
#define GPIO_IS_BIT13                       ((uint32_t)0x00002000)          /*!< Pin is, bit 13 */
#define GPIO_IS_BIT14                       ((uint32_t)0x00004000)          /*!< Pin is, bit 14 */
#define GPIO_IS_BIT15                       ((uint32_t)0x00008000)          /*!< Pin is, bit 15 */
#define GPIO_IS_BIT16                       ((uint32_t)0x00010000)          /*!< Pin is, bit 16 */
#define GPIO_IS_BIT17                       ((uint32_t)0x00020000)          /*!< Pin is, bit 17 */
#define GPIO_IS_BIT18                       ((uint32_t)0x00040000)          /*!< Pin is, bit 18 */
#define GPIO_IS_BIT19                       ((uint32_t)0x00080000)          /*!< Pin is, bit 19 */
#define GPIO_IS_BIT20                       ((uint32_t)0x00100000)          /*!< Pin is, bit 20 */
#define GPIO_IS_BIT21                       ((uint32_t)0x00200000)          /*!< Pin is, bit 21 */
#define GPIO_IS_BIT22                       ((uint32_t)0x00400000)          /*!< Pin is, bit 22 */
#define GPIO_IS_BIT23                       ((uint32_t)0x00800000)          /*!< Pin is, bit 23 */
#define GPIO_IS_BIT24                       ((uint32_t)0x01000000)          /*!< Pin is, bit 24 */
#define GPIO_IS_BIT25                       ((uint32_t)0x02000000)          /*!< Pin is, bit 25 */
#define GPIO_IS_BIT26                       ((uint32_t)0x04000000)          /*!< Pin is, bit 26 */
#define GPIO_IS_BIT27                       ((uint32_t)0x08000000)          /*!< Pin is, bit 27 */
#define GPIO_IS_BIT28                       ((uint32_t)0x10000000)          /*!< Pin is, bit 28 */
#define GPIO_IS_BIT29                       ((uint32_t)0x20000000)          /*!< Pin is, bit 29 */
#define GPIO_IS_BIT30                       ((uint32_t)0x40000000)          /*!< Pin is, bit 30 */
#define GPIO_IS_BIT31                       ((uint32_t)0x80000000)          /*!< Pin is, bit 31 */

/*******************  Bit definition for GPIO_IBE register  *******************/
#define GPIO_IBE_BIT0                       ((uint32_t)0x00000001)          /*!< Pin ibe, bit 0 */
#define GPIO_IBE_BIT1                       ((uint32_t)0x00000002)          /*!< Pin ibe, bit 1 */
#define GPIO_IBE_BIT2                       ((uint32_t)0x00000004)          /*!< Pin ibe, bit 2 */
#define GPIO_IBE_BIT3                       ((uint32_t)0x00000008)          /*!< Pin ibe, bit 3 */
#define GPIO_IBE_BIT4                       ((uint32_t)0x00000010)          /*!< Pin ibe, bit 4 */
#define GPIO_IBE_BIT5                       ((uint32_t)0x00000020)          /*!< Pin ibe, bit 5 */
#define GPIO_IBE_BIT6                       ((uint32_t)0x00000040)          /*!< Pin ibe, bit 6 */
#define GPIO_IBE_BIT7                       ((uint32_t)0x00000080)          /*!< Pin ibe, bit 7 */
#define GPIO_IBE_BIT8                       ((uint32_t)0x00000100)          /*!< Pin ibe, bit 8 */
#define GPIO_IBE_BIT9                       ((uint32_t)0x00000200)          /*!< Pin ibe, bit 9 */
#define GPIO_IBE_BIT10                      ((uint32_t)0x00000400)          /*!< Pin ibe, bit 10 */
#define GPIO_IBE_BIT11                      ((uint32_t)0x00000800)          /*!< Pin ibe, bit 11 */
#define GPIO_IBE_BIT12                      ((uint32_t)0x00001000)          /*!< Pin ibe, bit 12 */
#define GPIO_IBE_BIT13                      ((uint32_t)0x00002000)          /*!< Pin ibe, bit 13 */
#define GPIO_IBE_BIT14                      ((uint32_t)0x00004000)          /*!< Pin ibe, bit 14 */
#define GPIO_IBE_BIT15                      ((uint32_t)0x00008000)          /*!< Pin ibe, bit 15 */
#define GPIO_IBE_BIT16                      ((uint32_t)0x00010000)          /*!< Pin ibe, bit 16 */
#define GPIO_IBE_BIT17                      ((uint32_t)0x00020000)          /*!< Pin ibe, bit 17 */
#define GPIO_IBE_BIT18                      ((uint32_t)0x00040000)          /*!< Pin ibe, bit 18 */
#define GPIO_IBE_BIT19                      ((uint32_t)0x00080000)          /*!< Pin ibe, bit 19 */
#define GPIO_IBE_BIT20                      ((uint32_t)0x00100000)          /*!< Pin ibe, bit 20 */
#define GPIO_IBE_BIT21                      ((uint32_t)0x00200000)          /*!< Pin ibe, bit 21 */
#define GPIO_IBE_BIT22                      ((uint32_t)0x00400000)          /*!< Pin ibe, bit 22 */
#define GPIO_IBE_BIT23                      ((uint32_t)0x00800000)          /*!< Pin ibe, bit 23 */
#define GPIO_IBE_BIT24                      ((uint32_t)0x01000000)          /*!< Pin ibe, bit 24 */
#define GPIO_IBE_BIT25                      ((uint32_t)0x02000000)          /*!< Pin ibe, bit 25 */
#define GPIO_IBE_BIT26                      ((uint32_t)0x04000000)          /*!< Pin ibe, bit 26 */
#define GPIO_IBE_BIT27                      ((uint32_t)0x08000000)          /*!< Pin ibe, bit 27 */
#define GPIO_IBE_BIT28                      ((uint32_t)0x10000000)          /*!< Pin ibe, bit 28 */
#define GPIO_IBE_BIT29                      ((uint32_t)0x20000000)          /*!< Pin ibe, bit 29 */
#define GPIO_IBE_BIT30                      ((uint32_t)0x40000000)          /*!< Pin ibe, bit 30 */
#define GPIO_IBE_BIT31                      ((uint32_t)0x80000000)          /*!< Pin ibe, bit 31 */

/*******************  Bit definition for GPIO_IEV register  *******************/
#define GPIO_IEV_BIT0                       ((uint32_t)0x00000001)          /*!< Pin iev, bit 0 */
#define GPIO_IEV_BIT1                       ((uint32_t)0x00000002)          /*!< Pin iev, bit 1 */
#define GPIO_IEV_BIT2                       ((uint32_t)0x00000004)          /*!< Pin iev, bit 2 */
#define GPIO_IEV_BIT3                       ((uint32_t)0x00000008)          /*!< Pin iev, bit 3 */
#define GPIO_IEV_BIT4                       ((uint32_t)0x00000010)          /*!< Pin iev, bit 4 */
#define GPIO_IEV_BIT5                       ((uint32_t)0x00000020)          /*!< Pin iev, bit 5 */
#define GPIO_IEV_BIT6                       ((uint32_t)0x00000040)          /*!< Pin iev, bit 6 */
#define GPIO_IEV_BIT7                       ((uint32_t)0x00000080)          /*!< Pin iev, bit 7 */
#define GPIO_IEV_BIT8                       ((uint32_t)0x00000100)          /*!< Pin iev, bit 8 */
#define GPIO_IEV_BIT9                       ((uint32_t)0x00000200)          /*!< Pin iev, bit 9 */
#define GPIO_IEV_BIT10                      ((uint32_t)0x00000400)          /*!< Pin iev, bit 10 */
#define GPIO_IEV_BIT11                      ((uint32_t)0x00000800)          /*!< Pin iev, bit 11 */
#define GPIO_IEV_BIT12                      ((uint32_t)0x00001000)          /*!< Pin iev, bit 12 */
#define GPIO_IEV_BIT13                      ((uint32_t)0x00002000)          /*!< Pin iev, bit 13 */
#define GPIO_IEV_BIT14                      ((uint32_t)0x00004000)          /*!< Pin iev, bit 14 */
#define GPIO_IEV_BIT15                      ((uint32_t)0x00008000)          /*!< Pin iev, bit 15 */
#define GPIO_IEV_BIT16                      ((uint32_t)0x00010000)          /*!< Pin iev, bit 16 */
#define GPIO_IEV_BIT17                      ((uint32_t)0x00020000)          /*!< Pin iev, bit 17 */
#define GPIO_IEV_BIT18                      ((uint32_t)0x00040000)          /*!< Pin iev, bit 18 */
#define GPIO_IEV_BIT19                      ((uint32_t)0x00080000)          /*!< Pin iev, bit 19 */
#define GPIO_IEV_BIT20                      ((uint32_t)0x00100000)          /*!< Pin iev, bit 20 */
#define GPIO_IEV_BIT21                      ((uint32_t)0x00200000)          /*!< Pin iev, bit 21 */
#define GPIO_IEV_BIT22                      ((uint32_t)0x00400000)          /*!< Pin iev, bit 22 */
#define GPIO_IEV_BIT23                      ((uint32_t)0x00800000)          /*!< Pin iev, bit 23 */
#define GPIO_IEV_BIT24                      ((uint32_t)0x01000000)          /*!< Pin iev, bit 24 */
#define GPIO_IEV_BIT25                      ((uint32_t)0x02000000)          /*!< Pin iev, bit 25 */
#define GPIO_IEV_BIT26                      ((uint32_t)0x04000000)          /*!< Pin iev, bit 26 */
#define GPIO_IEV_BIT27                      ((uint32_t)0x08000000)          /*!< Pin iev, bit 27 */
#define GPIO_IEV_BIT28                      ((uint32_t)0x10000000)          /*!< Pin iev, bit 28 */
#define GPIO_IEV_BIT29                      ((uint32_t)0x20000000)          /*!< Pin iev, bit 29 */
#define GPIO_IEV_BIT30                      ((uint32_t)0x40000000)          /*!< Pin iev, bit 30 */
#define GPIO_IEV_BIT31                      ((uint32_t)0x80000000)          /*!< Pin iev, bit 31 */

/*******************  Bit definition for GPIO_IE register  *******************/
#define GPIO_IE_BIT0                        ((uint32_t)0x00000001)          /*!< Pin ie, bit 0 */
#define GPIO_IE_BIT1                        ((uint32_t)0x00000002)          /*!< Pin ie, bit 1 */
#define GPIO_IE_BIT2                        ((uint32_t)0x00000004)          /*!< Pin ie, bit 2 */
#define GPIO_IE_BIT3                        ((uint32_t)0x00000008)          /*!< Pin ie, bit 3 */
#define GPIO_IE_BIT4                        ((uint32_t)0x00000010)          /*!< Pin ie, bit 4 */
#define GPIO_IE_BIT5                        ((uint32_t)0x00000020)          /*!< Pin ie, bit 5 */
#define GPIO_IE_BIT6                        ((uint32_t)0x00000040)          /*!< Pin ie, bit 6 */
#define GPIO_IE_BIT7                        ((uint32_t)0x00000080)          /*!< Pin ie, bit 7 */
#define GPIO_IE_BIT8                        ((uint32_t)0x00000100)          /*!< Pin ie, bit 8 */
#define GPIO_IE_BIT9                        ((uint32_t)0x00000200)          /*!< Pin ie, bit 9 */
#define GPIO_IE_BIT10                       ((uint32_t)0x00000400)          /*!< Pin ie, bit 10 */
#define GPIO_IE_BIT11                       ((uint32_t)0x00000800)          /*!< Pin ie, bit 11 */
#define GPIO_IE_BIT12                       ((uint32_t)0x00001000)          /*!< Pin ie, bit 12 */
#define GPIO_IE_BIT13                       ((uint32_t)0x00002000)          /*!< Pin ie, bit 13 */
#define GPIO_IE_BIT14                       ((uint32_t)0x00004000)          /*!< Pin ie, bit 14 */
#define GPIO_IE_BIT15                       ((uint32_t)0x00008000)          /*!< Pin ie, bit 15 */
#define GPIO_IE_BIT16                       ((uint32_t)0x00010000)          /*!< Pin ie, bit 16 */
#define GPIO_IE_BIT17                       ((uint32_t)0x00020000)          /*!< Pin ie, bit 17 */
#define GPIO_IE_BIT18                       ((uint32_t)0x00040000)          /*!< Pin ie, bit 18 */
#define GPIO_IE_BIT19                       ((uint32_t)0x00080000)          /*!< Pin ie, bit 19 */
#define GPIO_IE_BIT20                       ((uint32_t)0x00100000)          /*!< Pin ie, bit 20 */
#define GPIO_IE_BIT21                       ((uint32_t)0x00200000)          /*!< Pin ie, bit 21 */
#define GPIO_IE_BIT22                       ((uint32_t)0x00400000)          /*!< Pin ie, bit 22 */
#define GPIO_IE_BIT23                       ((uint32_t)0x00800000)          /*!< Pin ie, bit 23 */
#define GPIO_IE_BIT24                       ((uint32_t)0x01000000)          /*!< Pin ie, bit 24 */
#define GPIO_IE_BIT25                       ((uint32_t)0x02000000)          /*!< Pin ie, bit 25 */
#define GPIO_IE_BIT26                       ((uint32_t)0x04000000)          /*!< Pin ie, bit 26 */
#define GPIO_IE_BIT27                       ((uint32_t)0x08000000)          /*!< Pin ie, bit 27 */
#define GPIO_IE_BIT28                       ((uint32_t)0x10000000)          /*!< Pin ie, bit 28 */
#define GPIO_IE_BIT29                       ((uint32_t)0x20000000)          /*!< Pin ie, bit 29 */
#define GPIO_IE_BIT30                       ((uint32_t)0x40000000)          /*!< Pin ie, bit 30 */
#define GPIO_IE_BIT31                       ((uint32_t)0x80000000)          /*!< Pin ie, bit 31 */

/*******************  Bit definition for GPIO_IRS register  *******************/
#define GPIO_IRS_BIT0                       ((uint32_t)0x00000001)          /*!< Pin irs, bit 0 */
#define GPIO_IRS_BIT1                       ((uint32_t)0x00000002)          /*!< Pin irs, bit 1 */
#define GPIO_IRS_BIT2                       ((uint32_t)0x00000004)          /*!< Pin irs, bit 2 */
#define GPIO_IRS_BIT3                       ((uint32_t)0x00000008)          /*!< Pin irs, bit 3 */
#define GPIO_IRS_BIT4                       ((uint32_t)0x00000010)          /*!< Pin irs, bit 4 */
#define GPIO_IRS_BIT5                       ((uint32_t)0x00000020)          /*!< Pin irs, bit 5 */
#define GPIO_IRS_BIT6                       ((uint32_t)0x00000040)          /*!< Pin irs, bit 6 */
#define GPIO_IRS_BIT7                       ((uint32_t)0x00000080)          /*!< Pin irs, bit 7 */
#define GPIO_IRS_BIT8                       ((uint32_t)0x00000100)          /*!< Pin irs, bit 8 */
#define GPIO_IRS_BIT9                       ((uint32_t)0x00000200)          /*!< Pin irs, bit 9 */
#define GPIO_IRS_BIT10                      ((uint32_t)0x00000400)          /*!< Pin irs, bit 10 */
#define GPIO_IRS_BIT11                      ((uint32_t)0x00000800)          /*!< Pin irs, bit 11 */
#define GPIO_IRS_BIT12                      ((uint32_t)0x00001000)          /*!< Pin irs, bit 12 */
#define GPIO_IRS_BIT13                      ((uint32_t)0x00002000)          /*!< Pin irs, bit 13 */
#define GPIO_IRS_BIT14                      ((uint32_t)0x00004000)          /*!< Pin irs, bit 14 */
#define GPIO_IRS_BIT15                      ((uint32_t)0x00008000)          /*!< Pin irs, bit 15 */
#define GPIO_IRS_BIT16                      ((uint32_t)0x00010000)          /*!< Pin irs, bit 16 */
#define GPIO_IRS_BIT17                      ((uint32_t)0x00020000)          /*!< Pin irs, bit 17 */
#define GPIO_IRS_BIT18                      ((uint32_t)0x00040000)          /*!< Pin irs, bit 18 */
#define GPIO_IRS_BIT19                      ((uint32_t)0x00080000)          /*!< Pin irs, bit 19 */
#define GPIO_IRS_BIT20                      ((uint32_t)0x00100000)          /*!< Pin irs, bit 20 */
#define GPIO_IRS_BIT21                      ((uint32_t)0x00200000)          /*!< Pin irs, bit 21 */
#define GPIO_IRS_BIT22                      ((uint32_t)0x00400000)          /*!< Pin irs, bit 22 */
#define GPIO_IRS_BIT23                      ((uint32_t)0x00800000)          /*!< Pin irs, bit 23 */
#define GPIO_IRS_BIT24                      ((uint32_t)0x01000000)          /*!< Pin irs, bit 24 */
#define GPIO_IRS_BIT25                      ((uint32_t)0x02000000)          /*!< Pin irs, bit 25 */
#define GPIO_IRS_BIT26                      ((uint32_t)0x04000000)          /*!< Pin irs, bit 26 */
#define GPIO_IRS_BIT27                      ((uint32_t)0x08000000)          /*!< Pin irs, bit 27 */
#define GPIO_IRS_BIT28                      ((uint32_t)0x10000000)          /*!< Pin irs, bit 28 */
#define GPIO_IRS_BIT29                      ((uint32_t)0x20000000)          /*!< Pin irs, bit 29 */
#define GPIO_IRS_BIT30                      ((uint32_t)0x40000000)          /*!< Pin irs, bit 30 */
#define GPIO_IRS_BIT31                      ((uint32_t)0x80000000)          /*!< Pin irs, bit 31 */

/*******************  Bit definition for GPIO_MIS register  *******************/
#define GPIO_MIS_BIT0                       ((uint32_t)0x00000001)          /*!< Pin mis, bit 0 */
#define GPIO_MIS_BIT1                       ((uint32_t)0x00000002)          /*!< Pin mis, bit 1 */
#define GPIO_MIS_BIT2                       ((uint32_t)0x00000004)          /*!< Pin mis, bit 2 */
#define GPIO_MIS_BIT3                       ((uint32_t)0x00000008)          /*!< Pin mis, bit 3 */
#define GPIO_MIS_BIT4                       ((uint32_t)0x00000010)          /*!< Pin mis, bit 4 */
#define GPIO_MIS_BIT5                       ((uint32_t)0x00000020)          /*!< Pin mis, bit 5 */
#define GPIO_MIS_BIT6                       ((uint32_t)0x00000040)          /*!< Pin mis, bit 6 */
#define GPIO_MIS_BIT7                       ((uint32_t)0x00000080)          /*!< Pin mis, bit 7 */
#define GPIO_MIS_BIT8                       ((uint32_t)0x00000100)          /*!< Pin mis, bit 8 */
#define GPIO_MIS_BIT9                       ((uint32_t)0x00000200)          /*!< Pin mis, bit 9 */
#define GPIO_MIS_BIT10                      ((uint32_t)0x00000400)          /*!< Pin mis, bit 10 */
#define GPIO_MIS_BIT11                      ((uint32_t)0x00000800)          /*!< Pin mis, bit 11 */
#define GPIO_MIS_BIT12                      ((uint32_t)0x00001000)          /*!< Pin mis, bit 12 */
#define GPIO_MIS_BIT13                      ((uint32_t)0x00002000)          /*!< Pin mis, bit 13 */
#define GPIO_MIS_BIT14                      ((uint32_t)0x00004000)          /*!< Pin mis, bit 14 */
#define GPIO_MIS_BIT15                      ((uint32_t)0x00008000)          /*!< Pin mis, bit 15 */
#define GPIO_MIS_BIT16                      ((uint32_t)0x00010000)          /*!< Pin mis, bit 16 */
#define GPIO_MIS_BIT17                      ((uint32_t)0x00020000)          /*!< Pin mis, bit 17 */
#define GPIO_MIS_BIT18                      ((uint32_t)0x00040000)          /*!< Pin mis, bit 18 */
#define GPIO_MIS_BIT19                      ((uint32_t)0x00080000)          /*!< Pin mis, bit 19 */
#define GPIO_MIS_BIT20                      ((uint32_t)0x00100000)          /*!< Pin mis, bit 20 */
#define GPIO_MIS_BIT21                      ((uint32_t)0x00200000)          /*!< Pin mis, bit 21 */
#define GPIO_MIS_BIT22                      ((uint32_t)0x00400000)          /*!< Pin mis, bit 22 */
#define GPIO_MIS_BIT23                      ((uint32_t)0x00800000)          /*!< Pin mis, bit 23 */
#define GPIO_MIS_BIT24                      ((uint32_t)0x01000000)          /*!< Pin mis, bit 24 */
#define GPIO_MIS_BIT25                      ((uint32_t)0x02000000)          /*!< Pin mis, bit 25 */
#define GPIO_MIS_BIT26                      ((uint32_t)0x04000000)          /*!< Pin mis, bit 26 */
#define GPIO_MIS_BIT27                      ((uint32_t)0x08000000)          /*!< Pin mis, bit 27 */
#define GPIO_MIS_BIT28                      ((uint32_t)0x10000000)          /*!< Pin mis, bit 28 */
#define GPIO_MIS_BIT29                      ((uint32_t)0x20000000)          /*!< Pin mis, bit 29 */
#define GPIO_MIS_BIT30                      ((uint32_t)0x40000000)          /*!< Pin mis, bit 30 */
#define GPIO_MIS_BIT31                      ((uint32_t)0x80000000)          /*!< Pin mis, bit 31 */

/*******************  Bit definition for GPIO_IC register  *******************/
#define GPIO_IC_BIT0                        ((uint32_t)0x00000001)          /*!< Pin ic, bit 0 */
#define GPIO_IC_BIT1                        ((uint32_t)0x00000002)          /*!< Pin ic, bit 1 */
#define GPIO_IC_BIT2                        ((uint32_t)0x00000004)          /*!< Pin ic, bit 2 */
#define GPIO_IC_BIT3                        ((uint32_t)0x00000008)          /*!< Pin ic, bit 3 */
#define GPIO_IC_BIT4                        ((uint32_t)0x00000010)          /*!< Pin ic, bit 4 */
#define GPIO_IC_BIT5                        ((uint32_t)0x00000020)          /*!< Pin ic, bit 5 */
#define GPIO_IC_BIT6                        ((uint32_t)0x00000040)          /*!< Pin ic, bit 6 */
#define GPIO_IC_BIT7                        ((uint32_t)0x00000080)          /*!< Pin ic, bit 7 */
#define GPIO_IC_BIT8                        ((uint32_t)0x00000100)          /*!< Pin ic, bit 8 */
#define GPIO_IC_BIT9                        ((uint32_t)0x00000200)          /*!< Pin ic, bit 9 */
#define GPIO_IC_BIT10                       ((uint32_t)0x00000400)          /*!< Pin ic, bit 10 */
#define GPIO_IC_BIT11                       ((uint32_t)0x00000800)          /*!< Pin ic, bit 11 */
#define GPIO_IC_BIT12                       ((uint32_t)0x00001000)          /*!< Pin ic, bit 12 */
#define GPIO_IC_BIT13                       ((uint32_t)0x00002000)          /*!< Pin ic, bit 13 */
#define GPIO_IC_BIT14                       ((uint32_t)0x00004000)          /*!< Pin ic, bit 14 */
#define GPIO_IC_BIT15                       ((uint32_t)0x00008000)          /*!< Pin ic, bit 15 */
#define GPIO_IC_BIT16                       ((uint32_t)0x00010000)          /*!< Pin ic, bit 16 */
#define GPIO_IC_BIT17                       ((uint32_t)0x00020000)          /*!< Pin ic, bit 17 */
#define GPIO_IC_BIT18                       ((uint32_t)0x00040000)          /*!< Pin ic, bit 18 */
#define GPIO_IC_BIT19                       ((uint32_t)0x00080000)          /*!< Pin ic, bit 19 */
#define GPIO_IC_BIT20                       ((uint32_t)0x00100000)          /*!< Pin ic, bit 20 */
#define GPIO_IC_BIT21                       ((uint32_t)0x00200000)          /*!< Pin ic, bit 21 */
#define GPIO_IC_BIT22                       ((uint32_t)0x00400000)          /*!< Pin ic, bit 22 */
#define GPIO_IC_BIT23                       ((uint32_t)0x00800000)          /*!< Pin ic, bit 23 */
#define GPIO_IC_BIT24                       ((uint32_t)0x01000000)          /*!< Pin ic, bit 24 */
#define GPIO_IC_BIT25                       ((uint32_t)0x02000000)          /*!< Pin ic, bit 25 */
#define GPIO_IC_BIT26                       ((uint32_t)0x04000000)          /*!< Pin ic, bit 26 */
#define GPIO_IC_BIT27                       ((uint32_t)0x08000000)          /*!< Pin ic, bit 27 */
#define GPIO_IC_BIT28                       ((uint32_t)0x10000000)          /*!< Pin ic, bit 28 */
#define GPIO_IC_BIT29                       ((uint32_t)0x20000000)          /*!< Pin ic, bit 29 */
#define GPIO_IC_BIT30                       ((uint32_t)0x40000000)          /*!< Pin ic, bit 30 */
#define GPIO_IC_BIT31                       ((uint32_t)0x80000000)          /*!< Pin ic, bit 31 */

/*******************  Bit definition for GPIO_DATAMASK register  *******************/
#define GPIO_DATAMASK_BIT0                  ((uint32_t)0x00000001)          /*!< Data mask, bit 0 */
#define GPIO_DATAMASK_BIT1                  ((uint32_t)0x00000002)          /*!< Data mask, bit 1 */
#define GPIO_DATAMASK_BIT2                  ((uint32_t)0x00000004)          /*!< Data mask, bit 2 */
#define GPIO_DATAMASK_BIT3                  ((uint32_t)0x00000008)          /*!< Data mask, bit 3 */
#define GPIO_DATAMASK_BIT4                  ((uint32_t)0x00000010)          /*!< Data mask, bit 4 */
#define GPIO_DATAMASK_BIT5                  ((uint32_t)0x00000020)          /*!< Data mask, bit 5 */
#define GPIO_DATAMASK_BIT6                  ((uint32_t)0x00000040)          /*!< Data mask, bit 6 */
#define GPIO_DATAMASK_BIT7                  ((uint32_t)0x00000080)          /*!< Data mask, bit 7 */
#define GPIO_DATAMASK_BIT8                  ((uint32_t)0x00000100)          /*!< Data mask, bit 8 */
#define GPIO_DATAMASK_BIT9                  ((uint32_t)0x00000200)          /*!< Data mask, bit 9 */
#define GPIO_DATAMASK_BIT10                 ((uint32_t)0x00000400)          /*!< Data mask, bit 10 */
#define GPIO_DATAMASK_BIT11                 ((uint32_t)0x00000800)          /*!< Data mask, bit 11 */
#define GPIO_DATAMASK_BIT12                 ((uint32_t)0x00001000)          /*!< Data mask, bit 12 */
#define GPIO_DATAMASK_BIT13                 ((uint32_t)0x00002000)          /*!< Data mask, bit 13 */
#define GPIO_DATAMASK_BIT14                 ((uint32_t)0x00004000)          /*!< Data mask, bit 14 */
#define GPIO_DATAMASK_BIT15                 ((uint32_t)0x00008000)          /*!< Data mask, bit 15 */
#define GPIO_DATAMASK_BIT16                 ((uint32_t)0x00010000)          /*!< Data mask, bit 16 */
#define GPIO_DATAMASK_BIT17                 ((uint32_t)0x00020000)          /*!< Data mask, bit 17 */
#define GPIO_DATAMASK_BIT18                 ((uint32_t)0x00040000)          /*!< Data mask, bit 18 */
#define GPIO_DATAMASK_BIT19                 ((uint32_t)0x00080000)          /*!< Data mask, bit 19 */
#define GPIO_DATAMASK_BIT20                 ((uint32_t)0x00100000)          /*!< Data mask, bit 20 */
#define GPIO_DATAMASK_BIT21                 ((uint32_t)0x00200000)          /*!< Data mask, bit 21 */
#define GPIO_DATAMASK_BIT22                 ((uint32_t)0x00400000)          /*!< Data mask, bit 22 */
#define GPIO_DATAMASK_BIT23                 ((uint32_t)0x00800000)          /*!< Data mask, bit 23 */
#define GPIO_DATAMASK_BIT24                 ((uint32_t)0x01000000)          /*!< Data mask, bit 24 */
#define GPIO_DATAMASK_BIT25                 ((uint32_t)0x02000000)          /*!< Data mask, bit 25 */
#define GPIO_DATAMASK_BIT26                 ((uint32_t)0x04000000)          /*!< Data mask, bit 26 */
#define GPIO_DATAMASK_BIT27                 ((uint32_t)0x08000000)          /*!< Data mask, bit 27 */
#define GPIO_DATAMASK_BIT28                 ((uint32_t)0x10000000)          /*!< Data mask, bit 28 */
#define GPIO_DATAMASK_BIT29                 ((uint32_t)0x20000000)          /*!< Data mask, bit 29 */
#define GPIO_DATAMASK_BIT30                 ((uint32_t)0x40000000)          /*!< Data mask, bit 30 */
#define GPIO_DATAMASK_BIT31                 ((uint32_t)0x80000000)          /*!< Data mask, bit 31 */

/*******************  Bit definition for GPIO_FILTER register  ****************/
#define GPIO_FILTER_ENABLE                  ((uint32_t)0x80000000)          /*!< Enable GPIO filter */
#define GPIO_FILTER_CYCLES_MASK             ((uint32_t)0x000000FF)          /*!< GPIO filter mask */

/******************************************************************************/
/*                                                                            */
/*                      Inter-integrated Circuit Interface                    */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for I2C_CON register  ********************/
#define I2C_CON_MASTER_MODE                 ((uint32_t)0x00000001)          /*!< I2C Master mode */
#define I2C_CON_SPEED1_2                    ((uint32_t)0x00000006)          /*!< I2C speed */
#define I2C_CON_STANDARD_MODE               ((uint32_t)0x00000002)          /*!< Standard speed mode */
#define I2C_CON_FAST_MODE                   ((uint32_t)0x00000004)          /*!< Fast speed mode */
#define I2C_CON_HIGH_SPEED_MODE             ((uint32_t)0x00000006)          /*!< High speed mode */
#define I2C_CON_10BITADDR_SLAVE             ((uint32_t)0x00000008)          /*!< 10bit address as slave */
#define I2C_CON_10BITADDR_MASTER            ((uint32_t)0x00000010)          /*!< 10bit address as master */
#define I2C_CON_RESTART_EN                  ((uint32_t)0x00000020)          /*!< Enable RESTART conditions */
#define I2C_CON_SLAVE_DISABLE               ((uint32_t)0x00000040)          /*!< I2C Slave mode */

/*******************  Bit definition for IC_TAR register  *********************/
#define I2C_TAR_TAR0_9                      ((uint32_t)0x000003FF)          /*!< I2C target address */
#define I2C_TAR_GC_OR_START                 ((uint32_t)0x00000400)          /*!< General Call or START BYTE command */
#define I2C_TAR_SPECIAL                     ((uint32_t)0x00000800)          /*!< perform special I2C command as specified in GC_OR_START bit */

/*******************  Bit definition for IC_SAR register  *********************/
#define I2C_SAR_SAR0_9                      ((uint32_t)0x000003FF)          /*!< Slave Address */

/*******************  Bit definition for IC_HS_MADDR register  ****************/
#define I2C_HS_MADDR_HS_MAR0_2              ((uint32_t)0x00000007)          /*!< I2C High Speed Master Mode Code Address */

/*******************  Bit definition for IC_DATA_CMD register  ****************/
#define I2C_DATA_CMD_DAT0_7                 ((uint32_t)0x000000FF)          /*!< Data */
#define I2C_DATA_CMD_CMD                    ((uint32_t)0x00000100)          /*!< Read or Write */

/*******************  Bit definition for IC_SS_SCL_HCNT register  *************/
#define I2C_SS_SCL_HCNT                     ((uint32_t)0x0000FFFF)          /*!< Standard Speed I2C Clock SCL High Count */

/*******************  Bit definition for IC_SS_SCL_LCNT register  *************/
#define I2C_SS_SCL_LCNT                     ((uint32_t)0x0000FFFF)          /*!< Standard Speed I2C Clock SCL Low Count */

/*******************  Bit definition for IC_FS_SCL_HCNT register  *************/
#define I2C_FS_SCL_HCNT                     ((uint32_t)0x0000FFFF)          /*!< Fast Speed I2C Clock SCL High Count */

/*******************  Bit definition for IC_FS_SCL_LCNT register  *************/
#define I2C_FS_SCL_LCNT                     ((uint32_t)0x0000FFFF)          /*!< Fast Speed I2C Clock SCL Low Count */

/*******************  Bit definition for IC_HS_SCL_HCNT register  *************/
#define I2C_HS_SCL_HCNT                     ((uint32_t)0x0000FFFF)          /*!< High Speed I2C Clock SCL High Count */

/*******************  Bit definition for IC_HS_SCL_LCNT register  *************/
#define I2C_HS_SCL_LCNT                     ((uint32_t)0x0000FFFF)          /*!< High Speed I2C Clock SCL Low Count */

/*******************  Bit definition for IC_INTR_STAT register  ***************/
#define I2C_INTR_STAT_R_RX_UNDER            ((uint32_t)0x00000001)          /*!< RX Underflow */
#define I2C_INTR_STAT_R_RX_OVER             ((uint32_t)0x00000002)          /*!< RX Overflow */
#define I2C_INTR_STAT_R_RX_FULL             ((uint32_t)0x00000004)          /*!< RX Full */
#define I2C_INTR_STAT_R_TX_OVER             ((uint32_t)0x00000008)          /*!< TX Overflow */
#define I2C_INTR_STAT_R_TX_EMPTY            ((uint32_t)0x00000010)          /*!< TX Empty */
#define I2C_INTR_STAT_R_RD_REQ              ((uint32_t)0x00000020)          /*!< Read Request */
#define I2C_INTR_STAT_R_TX_ABRT             ((uint32_t)0x00000040)          /*!< TX Abort */
#define I2C_INTR_STAT_R_RX_DONE             ((uint32_t)0x00000080)          /*!< RX Done */
#define I2C_INTR_STAT_R_ACTIVITY            ((uint32_t)0x00000100)          /*!< I2C Activity */
#define I2C_INTR_STAT_R_STOP_DET            ((uint32_t)0x00000200)          /*!< STOP Detect */
#define I2C_INTR_STAT_R_START_DET           ((uint32_t)0x00000400)          /*!< START Detect */
#define I2C_INTR_STAT_R_GEN_CALL            ((uint32_t)0x00000800)          /*!< General Call */

/*******************  Bit definition for IC_INTR_MASK register  ***************/
#define I2C_INTR_MASK_M_RX_UNDER            ((uint32_t)0x00000001)          /*!< Mask RX Underflow interrupt bit */
#define I2C_INTR_MASK_M_RX_OVER             ((uint32_t)0x00000002)          /*!< Mask RX Overflow interrupt bit */
#define I2C_INTR_MASK_M_RX_FULL             ((uint32_t)0x00000004)          /*!< Mask RX Full interrupt bit */
#define I2C_INTR_MASK_M_TX_OVER             ((uint32_t)0x00000008)          /*!< Mask TX Overflow interrupt bit */
#define I2C_INTR_MASK_M_TX_EMPTY            ((uint32_t)0x00000010)          /*!< Mask TX Empty interrupt bit */
#define I2C_INTR_MASK_M_RD_REQ              ((uint32_t)0x00000020)          /*!< Mask Read Request interrupt bit */
#define I2C_INTR_MASK_M_TX_ABRT             ((uint32_t)0x00000040)          /*!< Mask TX Abort interrupt bit */
#define I2C_INTR_MASK_M_ACTIVITY            ((uint32_t)0x00000100)          /*!< Mask I2C Activity interrupt bit */
#define I2C_INTR_MASK_M_STOP_DET            ((uint32_t)0x00000200)          /*!< Mask STOP Detect interrupt bit */
#define I2C_INTR_MASK_M_START_DET           ((uint32_t)0x00000400)          /*!< Mask START Detect interrupt bit */
#define I2C_INTR_MASK_M_GEN_CALL            ((uint32_t)0x00000800)          /*!< Mask General Call interrupt bit */

/*******************  Bit definition for IC_RAW_INTR_STAT register  **********/
#define I2C_RAW_INTR_STAT_RX_UNDER          ((uint32_t)0x00000001)          /*!< Raw RX Underflow interrupt bit */
#define I2C_RAW_INTR_STAT_RX_OVER           ((uint32_t)0x00000002)          /*!< Raw RX Overflow interrupt bit */
#define I2C_RAW_INTR_STAT_RX_FULL           ((uint32_t)0x00000004)          /*!< Raw RX Full interrupt bit */
#define I2C_RAW_INTR_STAT_TX_OVER           ((uint32_t)0x00000008)          /*!< Raw TX Overflow interrupt bit */
#define I2C_RAW_INTR_STAT_TX_EMPTY          ((uint32_t)0x00000010)          /*!< Raw TX Empty interrupt bit */
#define I2C_RAW_INTR_STAT_RD_REQ            ((uint32_t)0x00000020)          /*!< Raw Read Request interrupt bit */
#define I2C_RAW_INTR_STAT_TX_ABRT           ((uint32_t)0x00000040)          /*!< Raw TX Abort interrupt bit */
#define I2C_RAW_INTR_STAT_RX_DONE           ((uint32_t)0x00000080)          /*!< Raw RX Done interrupt bit */
#define I2C_RAW_INTR_STAT_ACTIVITY          ((uint32_t)0x00000100)          /*!< Raw I2C Activity interrupt bit */
#define I2C_RAW_INTR_STAT_STOP_DET          ((uint32_t)0x00000200)          /*!< Raw STOP Detect interrupt bit */
#define I2C_RAW_INTR_STAT_START_DET         ((uint32_t)0x00000400)          /*!< Raw START Detect interrupt bit */
#define I2C_RAW_INTR_STAT_GEN_CALL          ((uint32_t)0x00000800)          /*!< Raw General Call interrupt bit */

/*******************  Bit definition for IC_RX_TL register  ******************/
#define I2C_RX_TL_RX_TL0_7                  ((uint32_t)0x000000ff)          /*!< Receive FIFO Threshold Level */

/*******************  Bit definition for IC_TX_TL register  ******************/
#define I2C_TX_TL_TX_TL0_7                  ((uint32_t)0x000000ff)          /*!< Transmit FIFO Threshold Level */

/*******************  Bit definition for IC_CLR_INTR register  ***************/
#define I2C_CLR_INTR_CLR_INTR               ((uint32_t)0x00000001)          /*!< Clear the combined interrupt */

/*******************  Bit definition for IC_CLR_RX_UNDER register  ***********/
#define I2C_CLR_RX_UNDER_CLR_RX_UNDER       ((uint32_t)0x00000001)          /*!< Clear the RX_UNDER interrupt */

/*******************  Bit definition for IC_CLR_RX_OVER register  ************/
#define I2C_CLR_RX_OVER_CLR_RX_OVER         ((uint32_t)0x00000001)          /*!< Clear the RX_OVER interrupt */

/*******************  Bit definition for IC_CLR_TX_OVER register  ************/
#define I2C_CLR_TX_OVER_CLR_TX_OVER         ((uint32_t)0x00000001)          /*!< Clear the TX_OVER interrupt */

/*******************  Bit definition for IC_CLR_RD_REQ register  *************/
#define I2C_CLR_RD_REQ_CLR_RD_REQ           ((uint32_t)0x00000001)          /*!< Clear the RD_REQ interrupt */

/*******************  Bit definition for IC_CLR_TX_ABRT register  ************/
#define I2C_CLR_TX_ABRT_CLR_TX_ABRT         ((uint32_t)0x00000001)          /*!< Clear the TX_ABRT interrupt */

/*******************  Bit definition for IC_CLR_RX_DONE register  ************/
#define I2C_CLR_RX_DONE_CLR_RX_DONE         ((uint32_t)0x00000001)          /*!< Clear the RX_DONE interrupt */

/*******************  Bit definition for IC_CLR_ACTIVITY register  ***********/
#define I2C_CLR_ACTIVITY_CLR_ACTIVITY       ((uint32_t)0x00000001)          /*!< Clear ACTIVITY Interrupt */

/*******************  Bit definition for IC_CLR_STOP_DET register  ***********/
#define I2C_CLR_STOP_DET_CLR_STOP_DET       ((uint32_t)0x00000001)          /*!< Clear STOP_DET Interrupt */

/*******************  Bit definition for IC_CLR_START_DET register  **********/
#define I2C_CLR_START_DET_CLR_START_DET     ((uint32_t)0x00000001)          /*!< Clear START_DET Interrupt */

/*******************  Bit definition for IC_CLR_GEN_CALL register  ***********/
#define I2C_CLR_GEN_CALL_CLR_GEN_CALL       ((uint32_t)0x00000001)          /*!< Clear GEN_CALL Interrupt */

/*******************  Bit definition for IC_ENABLE register  *****************/
#define I2C_ENABLE_ENABLE                   ((uint32_t)0x00000001)          /*!< I2C Enable */

/*******************  Bit definition for IC_STATUS register  *****************/
#define I2C_STATUS_ACTIVITY                 ((uint32_t)0x00000001)          /*!< I2C Activity Status */
#define I2C_STATUS_TFNF                     ((uint32_t)0x00000002)          /*!< Transmit FIFO Not Full */
#define I2C_STATUS_TFE                      ((uint32_t)0x00000004)          /*!< Transmit FIFO Completely Empty */
#define I2C_STATUS_RFNE                     ((uint32_t)0x00000008)          /*!< Receive FIFO Not Empty */
#define I2C_STATUS_RFF                      ((uint32_t)0x00000010)          /*!< Receive FIFO Completely Full */
#define I2C_STATUS_MST_ACTIVITY             ((uint32_t)0x00000020)          /*!< Master FSM Activity Status */
#define I2C_STATUS_SLV_ACTIVITY             ((uint32_t)0x00000040)          /*!< Slave FSM Activity Status */

/*******************  Bit definition for IC_TXFLR register  ******************/
#define I2C_TXFLR_TXFLR0_3                  ((uint32_t)0x0000000f)          /*!< Transmit FIFO Level */

/*******************  Bit definition for IC_RXFLR register  ******************/
#define I2C_RXFLR_RXFLR0_3                  ((uint32_t)0x0000000f)          /*!< Receive FIFO Level */

/*******************  Bit definition for IC_TX_ABRT_SOURCE register  *********/
#define I2C_TAS_7B_ADDR_NOACK               ((uint32_t)0x00000001)          /*!< Master is in 7-bit addressing mode and the 
                                                                                 address sent was not acknowledged by any slave */
#define I2C_TAS_10ADDR1_NOACK               ((uint32_t)0x00000002)          /*!< Master is in 10-bit address mode and the first 10-bit 
                                                                                 address byte was not acknowledged by any slave */
#define I2C_TAS_10ADDR2_NOACK               ((uint32_t)0x00000004)          /*!< Master is in 10-bit address mode and the second address
                                                                                 byte of the 10-bit address was not acknowledged by any slave */
#define I2C_TAS_TXDATA_NOACK                ((uint32_t)0x00000008)          /*!< Can't receive an ack when send data */
#define I2C_TAS_GCALL_NOACK                 ((uint32_t)0x00000010)          /*!< Can't receive an ack when send GCALL */
#define I2C_TAS_GCALL_READ                  ((uint32_t)0x00000020)          /*!< user programmed the byte following the General
                                                                                 Call to be a read from the bus */
#define I2C_TAS_HS_ACKDET                   ((uint32_t)0x00000040)          /*!< High Speed Master code was acknowledged */
#define I2C_TAS_SBYTE_ACKDET                ((uint32_t)0x00000080)          /*!< Master has sent a START Byte and the
                                                                                 START Byte was acknowledged */
#define I2C_TAS_HS_NORSTRT                  ((uint32_t)0x00000100)          /*!< The restart is disabled and the user is trying to
                                                                                 use the master to transfer data in High Speed mode */
#define I2C_TAS_SBYTE_NORSTRT               ((uint32_t)0x00000200)          /*!< The restart is disabled and the user is trying to send a START Byte */
#define I2C_TAS_10B_RD_NORSTRT              ((uint32_t)0x00000400)          /*!< The restart is disabled and the master sends a
                                                                                 read command in 10-bit addressing mode */
#define I2C_TAS_MASTER_DIS                  ((uint32_t)0x00000800)          /*!< User tries to initiate a Master operation
                                                                                 with the Master mode disabled */
#define I2C_TAS_LOST                        ((uint32_t)0x00001000)          /*!< Lost arbitration */
#define I2C_TAS_SLVFLUSH_TXFIFO             ((uint32_t)0x00002000)          /*!< Slave has received a read command and some data exists in the TX FIFO
                                                                                 so the slave issues a TX_ABRT interrupt to flush old data in TX FIFO */
#define I2C_TAS_SLV_ARBLOST                 ((uint32_t)0x00004000)          /*!< Slave lost the bus */
#define I2C_TAS_SLVRD_INTX                  ((uint32_t)0x00008000)          /*!< Abort to read data from processor */

/*******************  Bit definition for IC_SLV_DATA_NACK_ONLY register ******/
#define I2C_SLV_DATA_NACK_ONLY_NACK         ((uint32_t)0x00000001)          /*!< Generate Slave Data NACK */

/*******************  Bit definition for IC_SDA_SETUP register  **************/
#define I2C_SDA_SETUP_SDA_SETUP0_7          ((uint32_t)0x000000ff)          /*!< I2C SDA Setup */

/*******************  Bit definition for I2C_ACK_GENERAL_CALL register  ******/
#define I2C_ACK_GENERAL_CALL                ((uint32_t)0x00000001)          /*!< I2C ACK General Call */

/*******************  Bit definition for IC_ENABLE_STATUS register  **********/
#define I2C_ENABLE_STATUS_EN                ((uint32_t)0x00000001)          /*!< I2C Enable status */
#define I2C_ENABLE_STATUS_DISABLED_WHILE_BUSY   ((uint32_t)0x00000002)      /*!< Slave Disabled While Busy */
#define I2C_ENABLE_STATUS_RX_DATA_LOST      ((uint32_t)0x00000004)          /*!< Slave Received Data Lost */

/******************************************************************************/
/*                                                                            */
/*                 Serial Peripheral Interface                                */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for HW_SPI_CR0 register  *****************/
#define SPI_CR0_DSS_MASK                    ((uint32_t)0x0000000F)          /*!< Data size  */
#define SPI_CR0_DSS_4BIT                    ((uint32_t)0x00000003)          /*!< Data size 4BIT */
#define SPI_CR0_DSS_5BIT                    ((uint32_t)0x00000004)          /*!< Data size 5BIT */
#define SPI_CR0_DSS_6BIT                    ((uint32_t)0x00000005)          /*!< Data size 6BIT */
#define SPI_CR0_DSS_7BIT                    ((uint32_t)0x00000006)          /*!< Data size 7BIT */
#define SPI_CR0_DSS_8BIT                    ((uint32_t)0x00000007)          /*!< Data size 8BIT */
#define SPI_CR0_DSS_9BIT                    ((uint32_t)0x00000008)          /*!< Data size 9BIT */
#define SPI_CR0_DSS_10BIT                   ((uint32_t)0x00000009)          /*!< Data size 10BIT */
#define SPI_CR0_DSS_11BIT                   ((uint32_t)0x0000000A)          /*!< Data size 11BIT */
#define SPI_CR0_DSS_12BIT                   ((uint32_t)0x0000000B)          /*!< Data size 12BIT */
#define SPI_CR0_DSS_13BIT                   ((uint32_t)0x0000000C)          /*!< Data size 13BIT */
#define SPI_CR0_DSS_14BIT                   ((uint32_t)0x0000000D)          /*!< Data size 14BIT */
#define SPI_CR0_DSS_15BIT                   ((uint32_t)0x0000000E)          /*!< Data size 15BIT */
#define SPI_CR0_DSS_16BIT                   ((uint32_t)0x0000000F)          /*!< Data size 16BIT */
#define SPI_CR0_FRF_MASK                    ((uint32_t)0x00000030)          /*!< Frame format mask */
#define SPI_CR0_FRF_MOTOROLA_SPI            ((uint32_t)0x00000000)          /*!< Motorola spi frame format */
#define SPI_CR0_FRF_TI_SYNCHRONOUS          ((uint32_t)0x00000010)          /*!< Ti synchronous frame format */
#define SPI_CR0_FRF_NATIONAL MICROWIRE      ((uint32_t)0x00000020)          /*!< National microwire frame format */
#define SPI_CR0_SPO                         ((uint32_t)0x00000040)          /*!< Clkout polarity(applicable to Motorola SPI frame format only) */
#define SPI_CR0_SPH                         ((uint32_t)0x00000080)          /*!< Clkout phase(applicable to Motorola SPI frame format only) */
#define SPI_CR0_SCR_MASK                    ((uint32_t)0x0000FF00)          /*!< Serial clock rate */

/*******************  Bit definition for HW_SPI_CR1 register  *****************/
#define	SPI_CR1_LBM                         ((uint32_t)0x00000001)          /*!< Loop back mode */
#define	SPI_CR1_SSE                         ((uint32_t)0x00000002)          /*!< Synchronous serial port enable */
#define	SPI_CR1_MS                          ((uint32_t)0x00000004)          /*!< Master or slave mode select */
#define	SPI_CR1_SOD                         ((uint32_t)0x00000008)          /*!< Slave mode output disable */

/*******************  Bit definition for HW_SPI_DR register  ******************/
#define	SPI_DR_MASK                         ((uint32_t)0x0000FFFF)          /*!< Transmit/Receive FIFO */

/*******************  Bit definition for HW_SPI_SR register  ******************/
#define	SPI_SR_TFE                          ((uint32_t)0x00000001)          /*!< Transmit FIFO empty */
#define	SPI_SR_TNF                          ((uint32_t)0x00000002)          /*!< Transmit FIFO not full */
#define	SPI_SR_RNE                          ((uint32_t)0x00000004)          /*!< Receive FIFO not empty */
#define	SPI_SR_RFF                          ((uint32_t)0x00000008)          /*!< Receive FIFO full */
#define	SPI_SR_BSY                          ((uint32_t)0x00000010)          /*!< Busy flag */

/*******************  Bit definition for HW_SPI_CPSR register  ****************/
#define	SPI_CPSR_CPSDV_MASK                 ((uint32_t)0x000000FF)          /*!< Clock prescale divisor mask */

/*******************  Bit definition for HW_SPI_IMSC register  ****************/
#define	SPI_IMSC_RORIM                      ((uint32_t)0x00000001)          /*!< Receive overrun interrupt mask */
#define	SPI_IMSC_RTIM                       ((uint32_t)0x00000002)          /*!< Receive timeout interrupt mask */
#define	SPI_IMSC_RXIM                       ((uint32_t)0x00000004)          /*!< Receive FIFO interrupt mask */
#define	SPI_IMSC_TXIM                       ((uint32_t)0x00000008)          /*!< Transmit FIFO interrupt mask */

/*******************  Bit definition for HW_SPI_RIS register  ****************/
#define	SPI_RIS_RORRIS                      ((uint32_t)0x00000001)          /*!< RORINTR interrupt */
#define	SPI_RIS_RTRIS                       ((uint32_t)0x00000002)          /*!< RTINTR interrupt */
#define	SPI_RIS_RXRIS                       ((uint32_t)0x00000004)          /*!< RXINTR interrupt */
#define	SPI_RIS_TXRIS                       ((uint32_t)0x00000008)          /*!< TXINTR interrupt */

/*******************  Bit definition for HW_SPI_MIS register  ****************/
#define	SPI_MIS_RORMIS                      ((uint32_t)0x00000001)          /*!< RORINTR interrupt */
#define	SPI_MIS_RTMIS                       ((uint32_t)0x00000002)          /*!< RTINTR interrupt */
#define	SPI_MIS_RXMIS                       ((uint32_t)0x00000004)          /*!< RXINTR interrupt */
#define	SPI_MIS_TXMIS                       ((uint32_t)0x00000008)          /*!< TXINTR interrupt */

/*******************  Bit definition for HW_SPI_ICR register  ****************/
#define	SPI_ICR_RORIC                       ((uint32_t)0x00000001)          /*!< Clear the RORINTR interrupt */
#define	SPI_ICR_RTIC                        ((uint32_t)0x00000002)          /*!< Clear the RTINTR interrupt */

/*******************  Bit definition for HW_SPI_DMACR register  ****************/
#define	SPI_DMACR_RXDMAE                    ((uint32_t)0x00000001)          /*!< Enable dma for the receive FIFO */
#define	SPI_DMACR_TXDMAE                    ((uint32_t)0x00000002)          /*!< Enable dma for the transmit FIFO */

/******************************************************************************/
/*                                                                            */
/*                 Analog to Digital Converter                                */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for HW_ADC_ISR register  *****************/
#define	ADC_ISR_EOC                         ((uint32_t)0x00000004)              /*!< End of conversion flag */
#define	ADC_ISR_EOSEQ                       ((uint32_t)0x00000008)              /*!< End of sequence flag */
#define	ADC_ISR_OVR                         ((uint32_t)0x00000010)              /*!< ADC overrun */
#define	ADC_ISR_AWD                         ((uint32_t)0x00000080)              /*!< Analog watchdog flag */

/*******************  Bit definition for HW_ADC_IER register  *****************/
#define	ADC_IER_EOCIE                       ((uint32_t)0x00000004)              /*!< End of conversion interrupt enable */
#define	ADC_IER_EOSEQIE                     ((uint32_t)0x00000008)              /*!< End of conversion sequence interrupt enable */
#define	ADC_IER_OVRIE                       ((uint32_t)0x00000010)              /*!< Overrun interrupt enable */
#define	ADC_IER_AWDIE                       ((uint32_t)0x00000080)              /*!< Analog watchdog interrupt enable */

/*******************  Bit definition for HW_ADC_CR register  ******************/
#define	ADC_CR_ADSTART                      ((uint32_t)0x00000004)              /*!< ADC start conversion command */

/*******************  Bit definition for HW_ADC_CFGR1 register  ***************/
#define	ADC_CFGR1_SCANDIR                   ((uint32_t)0x00000004)              /*!< Scan sequence direction */
#define	ADC_CFGR1_RES_MASK                  ((uint32_t)0x00000018)              /*!< Data resolution mask */
#define	ADC_CFGR1_ALIGN                     ((uint32_t)0x00000020)              /*!< Data alignment */
#define	ADC_CFGR1_EXTSEL_MASK               ((uint32_t)0x000003C0)              /*!< External trigger selection */
#define	ADC_CFGR1_EXTEN_MASK                ((uint32_t)0x00000C00)              /*!< External trigger enable and polarity selection */
#define	ADC_CFGR1_OVRMOD                    ((uint32_t)0x00001000)              /*!< Overrun management mode */
#define	ADC_CFGR1_CONT                      ((uint32_t)0x00002000)              /*!< Single / continuous conversion mode */
#define	ADC_CFGR1_WAIT                      ((uint32_t)0x00004000)              /*!< Wait conversion mode */
#define	ADC_CFGR1_DISCEN                    ((uint32_t)0x00010000)              /*!< Discontinuous mode */
#define	ADC_CFGR1_AWDSGL                    ((uint32_t)0x00400000)              /*!< Enable the watchdog on a single channel or on all channels */
#define	ADC_CFGR1_AWDEN                     ((uint32_t)0x00800000)              /*!< Analog watchdog enable */
#define	ADC_CFGR1_AWDCH_MASK                ((uint32_t)0x7F000000)              /*!< Analog watchdog channel selection mask */
#define	ADC_CFGR1_ASYNC_TRG                 ((uint32_t)0x80000000)              /*!< External trigger selection */

/*******************  Bit definition for HW_ADC_CFGR2 register  ***************/
#define	ADC_CFGR2_CLKMODE_MASK              ((uint32_t)0xC0000000)              /*!< ADC clock mode */

/*******************  Bit definition for HW_ADC_TR register  ******************/
#define	ADC_TR_LT_MASK                      ((uint32_t)0x00001FFF)              /*!< Analog watchdog lower threshold */
#define	ADC_TR_HT_MASK                      ((uint32_t)0x1FFF0000)              /*!< Analog watchdog higher threshold */

/*******************  Bit definition for HW_ADC_CHSELR register  **************/
#define	ADC_CHSELR_CH0                      ((uint32_t)0x00000001)              /*!< Channel0 selection */
#define	ADC_CHSELR_CH1                      ((uint32_t)0x00000002)              /*!< Channel1 selection */
#define	ADC_CHSELR_CH2                      ((uint32_t)0x00000004)              /*!< Channel2 selection */
#define	ADC_CHSELR_CH3                      ((uint32_t)0x00000008)              /*!< Channel3 selection */
#define	ADC_CHSELR_CH4                      ((uint32_t)0x00000010)              /*!< Channel4 selection */
#define	ADC_CHSELR_CH5                      ((uint32_t)0x00000020)              /*!< Channel5 selection */
#define	ADC_CHSELR_CH6                      ((uint32_t)0x00000040)              /*!< Channel6 selection */
#define	ADC_CHSELR_CH7                      ((uint32_t)0x00000080)              /*!< Channel7 selection */
#define	ADC_CHSELR_CH8                      ((uint32_t)0x00000100)              /*!< Channel8 selection */
#define	ADC_CHSELR_CH9                      ((uint32_t)0x00000200)              /*!< Channel9 selection */
#define	ADC_CHSELR_CH10                     ((uint32_t)0x00000400)              /*!< Channel10 selection */
#define	ADC_CHSELR_CH11                     ((uint32_t)0x00000800)              /*!< Channel11 selection */
#define	ADC_CHSELR_CH12                     ((uint32_t)0x00001000)              /*!< Channel12 selection */
#define	ADC_CHSELR_CH13                     ((uint32_t)0x00002000)              /*!< Channel13 selection */
#define	ADC_CHSELR_CH14                     ((uint32_t)0x00004000)              /*!< Channel14 selection */
#define	ADC_CHSELR_CH15                     ((uint32_t)0x00008000)              /*!< Channel15 selection*/

/*******************  Bit definition for HW_ADC_DR register  ******************/
#define	ADC_DR_DATA_MASK                    ((uint32_t)0x0000FFFF)              /*!< Converted data mask */

/*******************  Bit definition for HW_ADC_CCR register  *****************/
#define	ADC_CCR_TSEN                        ((uint32_t)0x00800000)              /*!< Temperature sensor enable */
#define	ADC_CCR_VREF_MASK                   ((uint32_t)0x00600000)              /*!< Mask of ADC VREF */
#define	ADC_CCR_VREF_PWD                    ((uint32_t)0x00100000)              /*!< Power down ADC internal vref */

/*******************  Bit definition for HW_ADC_ACR0 register  ****************/
#define	ADC_ACR0_DISCARD0_MASK              ((uint32_t)0x00000003)              /*!< Discard0 */
#define	ADC_ACR0_DISCARD1_MASK              ((uint32_t)0x00000030)              /*!< Discard1 */

/*******************  Bit definition for HW_ADC_ACR1 register  ****************/
#define	ADC_ACR1_BVOSI                      ((uint32_t)0x0000007F)              /*!< BVOSI */
#define	ADC_ACR1_RESET_CAL                  ((uint32_t)0x00000080)              /*!< Reset CAL */
#define	ADC_ACR1_RESETADC_ANA               ((uint32_t)0x00000100)              /*!< Reset analog */
#define	ADC_ACR1_LOAD_CAL                   ((uint32_t)0x00000200)              /*!< Load CAL */
#define	ADC_ACR1_BYPASS_BVOS_CAL            ((uint32_t)0x00000400)              /*!< Bypass BVOS */
#define	ADC_ACR1_BYPASS_MULTI_CAL           ((uint32_t)0x00000800)              /*!< Bypass MULTI */
#define	ADC_ACR1_START_CAL                  ((uint32_t)0x00001000)              /*!< Start CAL */

/*******************  Bit definition for HW_ADC_ACR2 register  ****************/
#define	ADC_ACR2_MULTI_SINGLE               ((uint32_t)0x00000FFF)              /*!< Multiplication */

/*******************  Bit definition for HW_ADC_STATUS register  **************/
#define	ADC_STATUS_BVOS                     ((uint32_t)0x0000007F)              /*!< BVOS */
#define	ADC_STATUS_CAL_ON                   ((uint32_t)0x80000000)              /*!< CAL enabled */

/*******************  Bit definition for HW_ADC_DR_CH register  ***************/
#define	ADC_DR_MASK                         ((uint32_t)0x0000FFFF)              /*!< Data mask */

/******************************************************************************/
/*                                                                            */
/*                                    ACMP                                    */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for ACMP_CTRL register  ******************/
#define	ACMP_CTRL_CMP_EN                    ((uint32_t)0x00000001)              /*!< ACMP enable */
#define	ACMP_CTRL_CMP_IE                    ((uint32_t)0x00000002)              /*!< ACMP interrupt enable */
#define	ACMP_CTRL_CMP_HYSEN_MASK            ((uint32_t)0x0000000C)              /*!< Mask of ACMP hysteresis */
#define	ACMP_CTRL_CMP_HYSEN_0               ((uint32_t)0x00000000)              /*!< ACMP hysteresis 0mv */
#define	ACMP_CTRL_CMP_HYSEN_10MV            ((uint32_t)0x00000004)              /*!< ACMP hysteresis 10mv */
#define	ACMP_CTRL_CMP_HYSEN_20MV            ((uint32_t)0x00000008)              /*!< ACMP hysteresis 20mv */
#define	ACMP_CTRL_CMP_HYSEN_50MV            ((uint32_t)0x0000000C)              /*!< ACMP hysteresis 50mv */
#define	ACMP_CTRL_CMP_NEG_SEL_MASK          ((uint32_t)0x00000030)              /*!< Mask of ACMP Negative source */
#define	ACMP_CTRL_CMP_NEG_SEL_EXT           ((uint32_t)0x00000000)              /*!< ACMP Negative source is external voltage */
#define	ACMP_CTRL_CMP_NEG_SEL_INT_REF       ((uint32_t)0x00000010)              /*!< ACMP Negative source is Internal reference voltage */
#define	ACMP_CTRL_CMP_NEG_SEL_DAC0          ((uint32_t)0x00000020)              /*!< ACMP Negative source is DAC0 */
#define	ACMP_CTRL_CMP_NEG_SEL_DAC1          ((uint32_t)0x00000030)              /*!< ACMP Negative source is DAC1 */
#define	ACMP_CTRL_CMP_INPUT_SEL             ((uint32_t)0x00000040)              /*!< Select ACMP input pin */
#define ACMP_CTRL_CMP_POS_OP                ((uint32_t)0x00000100)              /*!< Enable ACMP pos op */
#define ACMP_CTRL_CMP_OP_CH_MASK            ((uint32_t)0x00000600)              /*!< MASK of ACMP pos */
#define ACMP_CTRL_CMP_OP_CH_OP0             ((uint32_t)0x00000000)              /*!< ACMP pos OP0 */
#define ACMP_CTRL_CMP_OP_CH_OP1             ((uint32_t)0x00000200)              /*!< ACMP pos OP1 */
#define ACMP_CTRL_CMP_OP_CH_OP2             ((uint32_t)0x00000400)              /*!< ACMP pos OP2 */
#define ACMP_CTRL_CMP_OP_CH_OP3             ((uint32_t)0x00000600)              /*!< ACMP pos OP3 */
#define ACMP_CTRL_CMP_LVL_SEL_MASK          ((uint32_t)0x00007000)              /*!< MASK of ACMP reference voltage level */
#define ACMP_CTRL_CMP_LVL_SEL_0             ((uint32_t)0x00000000)              /*!< ACMP reference voltage 1.25V */
#define ACMP_CTRL_CMP_LVL_SEL_1             ((uint32_t)0x00001000)              /*!< ACMP reference voltage (7/8)1.25V */
#define ACMP_CTRL_CMP_LVL_SEL_2             ((uint32_t)0x00002000)              /*!< ACMP reference voltage (6/8)1.25V */
#define ACMP_CTRL_CMP_LVL_SEL_3             ((uint32_t)0x00003000)              /*!< ACMP reference voltage (5/8)1.25V */
#define ACMP_CTRL_CMP_LVL_SEL_4             ((uint32_t)0x00004000)              /*!< ACMP reference voltage (4/8)1.25V */
#define ACMP_CTRL_CMP_LVL_SEL_5             ((uint32_t)0x00005000)              /*!< ACMP reference voltage (3/8)1.25V */
#define ACMP_CTRL_CMP_LVL_SEL_6             ((uint32_t)0x00006000)              /*!< ACMP reference voltage (2/8)1.25V */
#define ACMP_CTRL_CMP_LVL_SEL_7             ((uint32_t)0x00007000)              /*!< ACMP reference voltage (1/8)1.25V */
#define ACMP_CTRL_CMP_EDGE_SEL_MASK         ((uint32_t)0x00030000)              /*!< Mask of ACMP edge select */
#define ACMP_CTRL_CMP_EDGE_SEL_FALL         ((uint32_t)0x00000000)              /*!< Select ACMP falling edge */
#define ACMP_CTRL_CMP_EDGE_SEL_RISE         ((uint32_t)0x00010000)              /*!< Select ACMP rising edge */
#define ACMP_CTRL_CMP_EDGE_SEL_DOUBLE       ((uint32_t)0x00020000)              /*!< Select ACMP double edge */
#define ACMP_CTRL_CMP_INV                   ((uint32_t)0x00100000)              /*!< ACMP inverse output state */
#define ACMP_CTRL_CMP_IO_EN                 ((uint32_t)0x00200000)              /*!< ACMP output to pin */

/*******************  Bit definition for ACMP_STATUS register  ****************/
#define	ACMP_STATUS_INT_FLAG0               ((uint32_t)0x00000001)              /*!< ACMP0 interrupt flag */
#define	ACMP_STATUS_INT_FLAG1               ((uint32_t)0x00000002)              /*!< ACMP1 interrupt flag */
#define	ACMP_STATUS_INT_FLAG2               ((uint32_t)0x00000004)              /*!< ACMP2 interrupt flag */
#define	ACMP_STATUS_RESULT0                 ((uint32_t)0x00000008)              /*!< ACMP0 result */
#define	ACMP_STATUS_RESULT1                 ((uint32_t)0x00000010)              /*!< ACMP1 result */
#define	ACMP_STATUS_RESULT2                 ((uint32_t)0x00000020)              /*!< ACMP2 result */

/*******************  Bit definition for ACMP_OP_CTRL register  ***************/
#define	ACMP_OP_CTRL_OP0_PWD                ((uint32_t)0x00000001)              /*!< Power down OP0 */
#define	ACMP_OP_CTRL_OP0_GAIN_MASK          ((uint32_t)0x0000000E)              /*!< Mask of OP0 gain */
#define	ACMP_OP_CTRL_OP0_GAIN_EXT_RES       ((uint32_t)0x00000000)              /*!< OP0 gain */
#define	ACMP_OP_CTRL_OP0_GAIN_1             ((uint32_t)0x00000002)              /*!< OP0 gain 1 */
#define	ACMP_OP_CTRL_OP0_GAIN_2             ((uint32_t)0x00000004)              /*!< OP0 gain 2 */
#define	ACMP_OP_CTRL_OP0_GAIN_3             ((uint32_t)0x00000006)              /*!< OP0 gain 3 */
#define	ACMP_OP_CTRL_OP0_GAIN_4             ((uint32_t)0x00000008)              /*!< OP0 gain 4 */
#define	ACMP_OP_CTRL_OP0_GAIN_6             ((uint32_t)0x0000000A)              /*!< OP0 gain 6 */
#define	ACMP_OP_CTRL_OP0_GAIN_8             ((uint32_t)0x0000000C)              /*!< OP0 gain 8 */
#define	ACMP_OP_CTRL_OP0_GAIN_10            ((uint32_t)0x0000000E)              /*!< OP0 gain 10 */

/*******************  Bit definition for ACMP_ADV_CTRL0 register  *************/
#define	ACMP_ADV_CTRL0_CMP0_TRGS_MASK       ((uint32_t)0x00000003)              /*!< Mask of CMP0 TRGS */
#define	ACMP_ADV_CTRL0_CMP0_TRGS_CON        ((uint32_t)0x00000000)              /*!< CMP0 TRGS continuous */
#define	ACMP_ADV_CTRL0_CMP0_TRGS_SYNC_PWM0  ((uint32_t)0x00000001)              /*!< CMP0 TRGS pwm0 */
#define	ACMP_ADV_CTRL0_CMP0_TRGS_SYNC_PWM1  ((uint32_t)0x00000002)              /*!< CMP0 TRGS pwm1 */
#define	ACMP_ADV_CTRL0_CMP0_TRGS_SYNC_PWM2  ((uint32_t)0x00000003)              /*!< CMP0 TRGS pwm2 */
#define	ACMP_ADV_CTRL0_CMP0_TRGPOL          ((uint32_t)0x00000004)              /*!< CMP0 TRGPOL */
#define	ACMP_ADV_CTRL0_CMP1_TRGS_MASK       ((uint32_t)0x00000030)              /*!< Mask of CMP1 TRGS */
#define	ACMP_ADV_CTRL0_CMP1_TRGS_CON        ((uint32_t)0x00000000)              /*!< CMP1 TRGS continuous */
#define	ACMP_ADV_CTRL0_CMP1_TRGS_SYNC_PWM0  ((uint32_t)0x00000010)              /*!< CMP1 TRGS pwm0 */
#define	ACMP_ADV_CTRL0_CMP1_TRGS_SYNC_PWM1  ((uint32_t)0x00000020)              /*!< CMP1 TRGS pwm1 */
#define	ACMP_ADV_CTRL0_CMP1_TRGS_SYNC_PWM2  ((uint32_t)0x00000030)              /*!< CMP1 TRGS pwm2 */
#define	ACMP_ADV_CTRL0_CMP1_TRGPOL          ((uint32_t)0x00000040)              /*!< CMP1 TRGPOL */
#define	ACMP_ADV_CTRL0_CMP2_TRGS_MASK       ((uint32_t)0x00000300)              /*!< Mask of CMP2 TRGS */
#define	ACMP_ADV_CTRL0_CMP2_TRGS_CON        ((uint32_t)0x00000000)              /*!< CMP2 TRGS continuous */
#define	ACMP_ADV_CTRL0_CMP2_TRGS_SYNC_PWM0  ((uint32_t)0x00000100)              /*!< CMP2 TRGS pwm0 */
#define	ACMP_ADV_CTRL0_CMP2_TRGS_SYNC_PWM1  ((uint32_t)0x00000200)              /*!< CMP2 TRGS pwm1 */
#define	ACMP_ADV_CTRL0_CMP2_TRGS_SYNC_PWM2  ((uint32_t)0x00000300)              /*!< CMP2 TRGS pwm2 */
#define	ACMP_ADV_CTRL0_CMP2_TRGPOL          ((uint32_t)0x00000400)              /*!< CMP2 TRGPOL */

/*******************  Bit definition for ACMP_ADV_CTRL1 register  *************/
#define	ACMP_ADV_CTRL1_CMP0_FT2S_MASK       ((uint32_t)0x00000007)              /*!< Mask of CMP0 filter2 parameter */
#define	ACMP_ADV_CTRL1_CMP0_FT2S_8          ((uint32_t)0x00000000)              /*!< CMP0 filter2 parameter 8 */
#define	ACMP_ADV_CTRL1_CMP0_FT2S_16         ((uint32_t)0x00000001)              /*!< CMP0 filter2 parameter 16 */
#define	ACMP_ADV_CTRL1_CMP0_FT2S_32         ((uint32_t)0x00000002)              /*!< CMP0 filter2 parameter 32 */
#define	ACMP_ADV_CTRL1_CMP0_FT2S_64         ((uint32_t)0x00000003)              /*!< CMP0 filter2 parameter 64 */
#define	ACMP_ADV_CTRL1_CMP0_FT2S_128        ((uint32_t)0x00000004)              /*!< CMP0 filter2 parameter 128 */
#define	ACMP_ADV_CTRL1_CMP0_FT2S_256        ((uint32_t)0x00000005)              /*!< CMP0 filter2 parameter 256 */
#define	ACMP_ADV_CTRL1_CMP0_FT2S_512        ((uint32_t)0x00000006)              /*!< CMP0 filter2 parameter 512 */
#define	ACMP_ADV_CTRL1_CMP0_FT2S_1024       ((uint32_t)0x00000007)              /*!< CMP0 filter2 parameter 1024 */
#define	ACMP_ADV_CTRL1_CMP0_FT2EN           ((uint32_t)0x00000008)              /*!< Enable CMP0 filter2 */
#define	ACMP_ADV_CTRL1_CMP0_FT1S_MASK       ((uint32_t)0x00000070)              /*!< Mask of CMP0 filter1 parameter */
#define	ACMP_ADV_CTRL1_CMP0_FT1S_32         ((uint32_t)0x00000000)              /*!< CMP0 filter1 parameter 32 */
#define	ACMP_ADV_CTRL1_CMP0_FT1S_64         ((uint32_t)0x00000010)              /*!< CMP0 filter1 parameter 64 */
#define	ACMP_ADV_CTRL1_CMP0_FT1S_128        ((uint32_t)0x00000020)              /*!< CMP0 filter1 parameter 128 */
#define	ACMP_ADV_CTRL1_CMP0_FT1S_256        ((uint32_t)0x00000030)              /*!< CMP0 filter1 parameter 256 */
#define	ACMP_ADV_CTRL1_CMP0_FT1S_512        ((uint32_t)0x00000040)              /*!< CMP0 filter1 parameter 512 */
#define	ACMP_ADV_CTRL1_CMP0_FT1S_1024       ((uint32_t)0x00000050)              /*!< CMP0 filter1 parameter 1024 */
#define	ACMP_ADV_CTRL1_CMP0_FT1S_2048       ((uint32_t)0x00000060)              /*!< CMP0 filter1 parameter 2048 */
#define	ACMP_ADV_CTRL1_CMP0_FT1S_4096       ((uint32_t)0x00000070)              /*!< CMP0 filter1 parameter 4096 */
#define	ACMP_ADV_CTRL1_CMP0_FT1EN           ((uint32_t)0x00000080)              /*!< Enable CMP0 filter1 */
#define	ACMP_ADV_CTRL1_CMP1_FT2S_MASK       ((uint32_t)0x00000700)              /*!< Mask of CMP1 filter2 parameter */
#define	ACMP_ADV_CTRL1_CMP1_FT2S_8          ((uint32_t)0x00000000)              /*!< CMP1 filter2 parameter 8 */
#define	ACMP_ADV_CTRL1_CMP1_FT2S_16         ((uint32_t)0x00000100)              /*!< CMP1 filter2 parameter 16 */
#define	ACMP_ADV_CTRL1_CMP1_FT2S_32         ((uint32_t)0x00000200)              /*!< CMP1 filter2 parameter 32 */
#define	ACMP_ADV_CTRL1_CMP1_FT2S_64         ((uint32_t)0x00000300)              /*!< CMP1 filter2 parameter 64 */
#define	ACMP_ADV_CTRL1_CMP1_FT2S_128        ((uint32_t)0x00000400)              /*!< CMP1 filter2 parameter 128 */
#define	ACMP_ADV_CTRL1_CMP1_FT2S_256        ((uint32_t)0x00000500)              /*!< CMP1 filter2 parameter 256 */
#define	ACMP_ADV_CTRL1_CMP1_FT2S_512        ((uint32_t)0x00000600)              /*!< CMP1 filter2 parameter 512 */
#define	ACMP_ADV_CTRL1_CMP1_FT2S_1024       ((uint32_t)0x00000700)              /*!< CMP1 filter2 parameter 1024 */
#define	ACMP_ADV_CTRL1_CMP1_FT2EN           ((uint32_t)0x00000800)              /*!< Enable CMP1 filter2 */
#define	ACMP_ADV_CTRL1_CMP1_FT1S_MASK       ((uint32_t)0x00007000)              /*!< Mask of CMP1 filter1 parameter */
#define	ACMP_ADV_CTRL1_CMP1_FT1S_32         ((uint32_t)0x00000000)              /*!< CMP1 filter1 parameter 32 */
#define	ACMP_ADV_CTRL1_CMP1_FT1S_64         ((uint32_t)0x00001000)              /*!< CMP1 filter1 parameter 64 */
#define	ACMP_ADV_CTRL1_CMP1_FT1S_128        ((uint32_t)0x00002000)              /*!< CMP1 filter1 parameter 128 */
#define	ACMP_ADV_CTRL1_CMP1_FT1S_256        ((uint32_t)0x00003000)              /*!< CMP1 filter1 parameter 256 */
#define	ACMP_ADV_CTRL1_CMP1_FT1S_512        ((uint32_t)0x00004000)              /*!< CMP1 filter1 parameter 512 */
#define	ACMP_ADV_CTRL1_CMP1_FT1S_1024       ((uint32_t)0x00005000)              /*!< CMP1 filter1 parameter 1024 */
#define	ACMP_ADV_CTRL1_CMP1_FT1S_2048       ((uint32_t)0x00006000)              /*!< CMP1 filter1 parameter 2048 */
#define	ACMP_ADV_CTRL1_CMP1_FT1S_4096       ((uint32_t)0x00007000)              /*!< CMP1 filter1 parameter 4096 */
#define	ACMP_ADV_CTRL1_CMP1_FT1EN           ((uint32_t)0x00008000)              /*!< Enable CMP1 filter1 */
#define	ACMP_ADV_CTRL1_CMP2_FT2S_MASK       ((uint32_t)0x00070000)              /*!< Mask of CMP2 filter2 parameter */
#define	ACMP_ADV_CTRL1_CMP2_FT2S_8          ((uint32_t)0x00000000)              /*!< CMP2 filter2 parameter 8 */
#define	ACMP_ADV_CTRL1_CMP2_FT2S_16         ((uint32_t)0x00010000)              /*!< CMP2 filter2 parameter 16 */
#define	ACMP_ADV_CTRL1_CMP2_FT2S_32         ((uint32_t)0x00020000)              /*!< CMP2 filter2 parameter 32 */
#define	ACMP_ADV_CTRL1_CMP2_FT2S_64         ((uint32_t)0x00030000)              /*!< CMP2 filter2 parameter 64 */
#define	ACMP_ADV_CTRL1_CMP2_FT2S_128        ((uint32_t)0x00040000)              /*!< CMP2 filter2 parameter 128 */
#define	ACMP_ADV_CTRL1_CMP2_FT2S_256        ((uint32_t)0x00050000)              /*!< CMP2 filter2 parameter 256 */
#define	ACMP_ADV_CTRL1_CMP2_FT2S_512        ((uint32_t)0x00060000)              /*!< CMP2 filter2 parameter 512 */
#define	ACMP_ADV_CTRL1_CMP2_FT2S_1024       ((uint32_t)0x00070000)              /*!< CMP2 filter2 parameter 1024 */
#define	ACMP_ADV_CTRL1_CMP2_FT2EN           ((uint32_t)0x00080000)              /*!< Enable CMP2 filter2 */
#define	ACMP_ADV_CTRL1_CMP2_FT1S_MASK       ((uint32_t)0x00700000)              /*!< Mask of CMP2 filter1 parameter */
#define	ACMP_ADV_CTRL1_CMP2_FT1S_32         ((uint32_t)0x00000000)              /*!< CMP2 filter1 parameter 32 */
#define	ACMP_ADV_CTRL1_CMP2_FT1S_64         ((uint32_t)0x00100000)              /*!< CMP2 filter1 parameter 64 */
#define	ACMP_ADV_CTRL1_CMP2_FT1S_128        ((uint32_t)0x00200000)              /*!< CMP2 filter1 parameter 128 */
#define	ACMP_ADV_CTRL1_CMP2_FT1S_256        ((uint32_t)0x00300000)              /*!< CMP2 filter1 parameter 256 */
#define	ACMP_ADV_CTRL1_CMP2_FT1S_512        ((uint32_t)0x00400000)              /*!< CMP2 filter1 parameter 512 */
#define	ACMP_ADV_CTRL1_CMP2_FT1S_1024       ((uint32_t)0x00500000)              /*!< CMP2 filter1 parameter 1024 */
#define	ACMP_ADV_CTRL1_CMP2_FT1S_2048       ((uint32_t)0x00600000)              /*!< CMP2 filter1 parameter 2048 */
#define	ACMP_ADV_CTRL1_CMP2_FT1S_4096       ((uint32_t)0x00700000)              /*!< CMP2 filter1 parameter 4096 */
#define	ACMP_ADV_CTRL1_CMP2_FT1EN           ((uint32_t)0x00800000)              /*!< Enable CMP2 filter1 */

/*******************  Bit definition for ACMP_ADV_CTRL2 register  *************/
#define	ACMP_ADV_CTRL2_CMP0_DEB_EN          ((uint32_t)0x00000001)              /*!< Enable CMP0 DEB */
#define	ACMP_ADV_CTRL2_CMP1_DEB_EN          ((uint32_t)0x00000002)              /*!< Enable CMP1 DEB */
#define	ACMP_ADV_CTRL2_CMP2_DEB_EN          ((uint32_t)0x00000004)              /*!< Enable CMP2 DEB */
#define	ACMP_ADV_CTRL2_CMP0_DEB_NUM         ((uint32_t)0x0000FF00)              /*!< CMP0 DEB number */
#define	ACMP_ADV_CTRL2_CMP1_DEB_NUM         ((uint32_t)0x00FF0000)              /*!< CMP1 DEB number */
#define	ACMP_ADV_CTRL2_CMP2_DEB_NUM         ((uint32_t)0xFF000000)              /*!< CMP2 DEB number */

/*******************  Bit definition for ACMP_ADV_CTRL3 register  *************/
#define	ACMP_ADV_CTRL3_CMP0_FLT_SEL         ((uint32_t)0x00000001)              /*!< CMP0 fault select */
#define	ACMP_ADV_CTRL3_CMP1_FLT_SEL         ((uint32_t)0x00000002)              /*!< CMP1 fault select */
#define	ACMP_ADV_CTRL3_CMP2_FLT_SEL         ((uint32_t)0x00000004)              /*!< CMP2 fault select */
#define	ACMP_ADV_CTRL3_PWM0_FLT_SEL_MASK    ((uint32_t)0x00000030)              /*!< Mask of PWM0 FLT select */
#define	ACMP_ADV_CTRL3_PWM0_FLT_SEL_ALWAYS  ((uint32_t)0x00000000)              /*!< PWM0 FLT select */
#define	ACMP_ADV_CTRL3_PWM0_FLT_SEL_CMP0    ((uint32_t)0x00000010)              /*!< PWM0 FLT select */
#define	ACMP_ADV_CTRL3_PWM0_FLT_SEL_CMP1    ((uint32_t)0x00000020)              /*!< PWM0 FLT select */
#define	ACMP_ADV_CTRL3_PWM0_FLT_SEL_CMP2    ((uint32_t)0x00000030)              /*!< PWM0 FLT select */
#define	ACMP_ADV_CTRL3_PWM1_FLT_SEL_MASK    ((uint32_t)0x000000C0)              /*!< Mask of PWM1 FLT select */
#define	ACMP_ADV_CTRL3_PWM1_FLT_SEL_ALWAYS  ((uint32_t)0x00000000)              /*!< PWM1 FLT select */
#define	ACMP_ADV_CTRL3_PWM1_FLT_SEL_CMP0    ((uint32_t)0x00000040)              /*!< PWM1 FLT select */
#define	ACMP_ADV_CTRL3_PWM1_FLT_SEL_CMP1    ((uint32_t)0x00000080)              /*!< PWM1 FLT select */
#define	ACMP_ADV_CTRL3_PWM1_FLT_SEL_CMP2    ((uint32_t)0x000000C0)              /*!< PWM1 FLT select */
#define	ACMP_ADV_CTRL3_PWM2_FLT_SEL_MASK    ((uint32_t)0x00000300)              /*!< Mask of PWM2 FLT select */
#define	ACMP_ADV_CTRL3_PWM2_FLT_SEL_ALWAYS  ((uint32_t)0x00000000)              /*!< PWM2 FLT select */
#define	ACMP_ADV_CTRL3_PWM2_FLT_SEL_CMP0    ((uint32_t)0x00000100)              /*!< PWM2 FLT select */
#define	ACMP_ADV_CTRL3_PWM2_FLT_SEL_CMP1    ((uint32_t)0x00000200)              /*!< PWM2 FLT select */
#define	ACMP_ADV_CTRL3_PWM2_FLT_SEL_CMP2    ((uint32_t)0x00000300)              /*!< PWM2 FLT select */
#define	ACMP_ADV_CTRL3_PWM3_FLT_SEL_MASK    ((uint32_t)0x00000C00)              /*!< Mask of PWM3 FLT select */
#define	ACMP_ADV_CTRL3_PWM3_FLT_SEL_ALWAYS  ((uint32_t)0x00000000)              /*!< PWM3 FLT select */
#define	ACMP_ADV_CTRL3_PWM3_FLT_SEL_CMP0    ((uint32_t)0x00000400)              /*!< PWM3 FLT select */
#define	ACMP_ADV_CTRL3_PWM3_FLT_SEL_CMP1    ((uint32_t)0x00000800)              /*!< PWM3 FLT select */
#define	ACMP_ADV_CTRL3_PWM3_FLT_SEL_CMP2    ((uint32_t)0x00000C00)              /*!< PWM3 FLT select */

/*******************  Bit definition for ACMP_CMPCLKDIV register  *************/
#define	ACMP_CMPCLKDIV_DIV_MASK             ((uint32_t)0x000000FF)              /*!< CMP clock div */

/******************************************************************************/
/*                                                                            */
/*                                    TIMER                                   */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for TIMER_IR register  *******************/
#define TIMER_IR_MR0                        ((uint32_t)0x00000001)              /*!< Matching channel 0 interrupt flag */
#define TIMER_IR_MR1                        ((uint32_t)0x00000002)              /*!< Matching channel 1 interrupt flag */
#define TIMER_IR_MR2                        ((uint32_t)0x00000004)              /*!< Matching channel 2 interrupt flag */
#define TIMER_IR_MR3                        ((uint32_t)0x00000008)              /*!< Matching channel 3 interrupt flag */
#define TIMER_IR_CR0                        ((uint32_t)0x00000010)              /*!< Capture 0 interrupt flag */
#define TIMER_IR_SING0                      ((uint32_t)0x00000020)              /*!< Channel 0 single interrupt flag */
#define TIMER_IR_SING1                      ((uint32_t)0x00000040)              /*!< Channel 1 single interrupt flag */
#define TIMER_IR_SING2                      ((uint32_t)0x00000080)              /*!< Channel 2 single interrupt flag */
#define TIMER_IR_SING3                      ((uint32_t)0x00000100)              /*!< Channel 3 single interrupt flag */
#define TIMER_IR_CR1                        ((uint32_t)0x00000200)              /*!< Capture 1 interrupt flag */

/*******************  Bit definition for TIMER_TCR register  ******************/
#define TIMER_TCR0_ENABLE                   ((uint32_t)0x00000001)              /*!< Enable timer channel 0 */
#define TIMER_TCR0_RESET                    ((uint32_t)0x00000010)              /*!< Reset timer channel 0 */
#define TIMER_TCR1_ENABLE                   ((uint32_t)0x00000002)              /*!< Enable timer channel 1 */
#define TIMER_TCR1_RESET                    ((uint32_t)0x00000020)              /*!< Reset timer channel 1 */
#define TIMER_TCR2_ENABLE                   ((uint32_t)0x00000004)              /*!< Enable timer channel 2 */
#define TIMER_TCR2_RESET                    ((uint32_t)0x00000040)              /*!< Reset timer channel 2 */
#define TIMER_TCR3_ENABLE                   ((uint32_t)0x00000008)              /*!< Enable timer channel 3 */
#define TIMER_TCR3_RESET                    ((uint32_t)0x00000080)              /*!< Reset timer channel 3 */
#define TIMER_TCR0_OUT                      ((uint32_t)0x00000300)              /*!< Channel0 idle level mask */
#define TIMER_TCR1_OUT                      ((uint32_t)0x00000C00)              /*!< Channel0 idle level mask */
#define TIMER_TCR2_OUT                      ((uint32_t)0x00003000)              /*!< Channel0 idle level mask */
#define TIMER_TCR3_OUT                      ((uint32_t)0x0000C000)              /*!< Channel0 idle level mask */
#define TIMER_TCR3_OUT                      ((uint32_t)0x0000C000)              /*!< Channel0 idle level mask */
#define TIMER_TCR0_PWMEN                    ((uint32_t)0x00010000)              /*!< Enable Channel 0 pwm */
#define TIMER_TCR1_PWMEN                    ((uint32_t)0x00020000)              /*!< Enable Channel 0 pwm */
#define TIMER_TCR2_PWMEN                    ((uint32_t)0x00040000)              /*!< Enable Channel 0 pwm */
#define TIMER_TCR3_PWMEN                    ((uint32_t)0x00080000)              /*!< Enable Channel 0 pwm */

/*******************  Bit definition for TIMER_DIR register  ******************/
#define TIMER_DIR0                          ((uint32_t)0x00000003)              /*!< Timer channel 0 Counting direction */
#define TIMER_DIR1                          ((uint32_t)0x00000030)              /*!< Timer channel 1 Counting direction */
#define TIMER_DIR2                          ((uint32_t)0x00000300)              /*!< Timer channel 2 Counting direction */
#define TIMER_DIR3                          ((uint32_t)0x00003000)              /*!< Timer channel 3 Counting direction */

/*******************  Bit definition for TIMER_TC0 register  ******************/
#define TIMER_TC0                           ((uint32_t)0xFFFFFFFF)              /*!< Timer channel 0 count */

/*******************  Bit definition for TIMER_TC1 register  ******************/
#define TIMER_TC1                           ((uint32_t)0xFFFFFFFF)              /*!< Timer channel 0 count */

/*******************  Bit definition for TIMER_TC2 register  ******************/
#define TIMER_TC2                           ((uint32_t)0xFFFFFFFF)              /*!< Timer channel 0 count */

/*******************  Bit definition for TIMER_TC3 register  ******************/
#define TIMER_TC3                           ((uint32_t)0xFFFFFFFF)              /*!< Timer channel 0 count */

/*******************  Bit definition for TIMER_PR register  *******************/
#define TIMER_PR_VAL                        ((uint32_t)0xFFFFFFFF)              /*!< Timer Prescale register */

/*******************  Bit definition for TIMER_PC register  *******************/
#define TIMER_PC_VAL                        ((uint32_t)0xFFFFFFFF)              /*!< Timer Prescale counter */

/*******************  Bit definition for TIMER_MCR register  ******************/
#define TIMER_MCR_MR0I                      ((uint32_t)0x00000001)              /*!< Enable interrupt of matching register 0 */
#define TIMER_MCR_MR0R                      ((uint32_t)0x00000002)              /*!< Reset TC0 when matching */
#define TIMER_MCR_MR0S                      ((uint32_t)0x00000004)              /*!< Disable TC0 when matching */
#define TIMER_MCR_MR1I                      ((uint32_t)0x00000008)              /*!< Enable interrupt of matching register 1 */
#define TIMER_MCR_MR1R                      ((uint32_t)0x00000010)              /*!< Reset TC1 when matching */
#define TIMER_MCR_MR1S                      ((uint32_t)0x00000020)              /*!< Disable TC1 when matching */
#define TIMER_MCR_MR2I                      ((uint32_t)0x00000040)              /*!< Enable interrupt of matching register 2 */
#define TIMER_MCR_MR2R                      ((uint32_t)0x00000080)              /*!< Reset TC2 when matching */
#define TIMER_MCR_MR2S                      ((uint32_t)0x00000100)              /*!< Disable TC2 when matching */
#define TIMER_MCR_MR3I                      ((uint32_t)0x00000200)              /*!< Enable interrupt of matching register 3 */
#define TIMER_MCR_MR3R                      ((uint32_t)0x00000400)              /*!< Reset TC3 when matching */
#define TIMER_MCR_MR3S                      ((uint32_t)0x00000800)              /*!< Disable TC3 when matching */

/*******************  Bit definition for TIMER_MR0 register  ******************/
#define TIMER_MRO_VAL                       ((uint32_t)0xFFFFFFFF)              /*!< Timer channel 0 match value */

/*******************  Bit definition for TIMER_MR1 register  ******************/
#define TIMER_MR1_VAL                       ((uint32_t)0xFFFFFFFF)              /*!< Timer channel 1 match value */

/*******************  Bit definition for TIMER_MR2 register  ******************/
#define TIMER_MR2_VAL                       ((uint32_t)0xFFFFFFFF)              /*!< Timer channel 2 match value */

/*******************  Bit definition for TIMER_MR3 register  ******************/
#define TIMER_MR3_VAL                       ((uint32_t)0xFFFFFFFF)              /*!< Timer channel 3 match value */

/*******************  Bit definition for TIMER_CCR register  ******************/
#define TIMER_CCR_CAP0RE                    ((uint32_t)0x00000001)              /*!< Capture 0 when rising edge */
#define TIMER_CCR_CAP0FE                    ((uint32_t)0x00000002)              /*!< Capture 0 when falling edge */    
#define TIMER_CCR_CAP0I                     ((uint32_t)0x00000004)              /*!< Enable Capture 0 interrupt */    
#define TIMER_CCR_CAP0FILTER                ((uint32_t)0x000000F0)              /*!< Caputre 0 filter */    
#define TIMER_CCR_CAP1RE                    ((uint32_t)0x00000100)              /*!< Capture 1 when rising edge */
#define TIMER_CCR_CAP1FE                    ((uint32_t)0x00000200)              /*!< Capture 1 when falling edge */    
#define TIMER_CCR_CAP1I                     ((uint32_t)0x00000400)              /*!< Enable Capture 1 interrupt */    
#define TIMER_CCR_CAP1FILTER                ((uint32_t)0x0000F000)              /*!< Caputre 1 filter */    
#define TIMER_CCR_CNT0CAP                   ((uint32_t)0x00010000)              /*!< Count 0 capture PIN */    
#define TIMER_CCR_CNT1CAP                   ((uint32_t)0x00020000)              /*!< Count 1 capture PIN */    
#define TIMER_CCR_CNT2CAP                   ((uint32_t)0x00040000)              /*!< Count 2 capture PIN */    
#define TIMER_CCR_CNT3CAP                   ((uint32_t)0x00080000)              /*!< Count 3 capture PIN */    

/*******************  Bit definition for TIMER_CR0 register  ******************/
#define TIMER_CR0_VAL                       ((uint32_t)0xFFFFFFFF)              /*!< Capture register 0 */    

/*******************  Bit definition for TIMER_CR1 register  ******************/
#define TIMER_CR1_VAL                       ((uint32_t)0xFFFFFFFF)              /*!< Capture register 1 */    

/*******************  Bit definition for TIMER_CR2 register  ******************/
#define TIMER_CR2_VAL                       ((uint32_t)0xFFFFFFFF)              /*!< Capture register 2 */    

/*******************  Bit definition for TIMER_CR3 register  ******************/
#define TIMER_CR3_VAL                       ((uint32_t)0xFFFFFFFF)              /*!< Capture register 3 */    

/*******************  Bit definition for TIMER_EMR register  ******************/
#define TIMER_EMR_EM0                       ((uint32_t)0x00000001)              /*!< External match 0 */
#define TIMER_EMR_EM1                       ((uint32_t)0x00000002)              /*!< External match 1 */
#define TIMER_EMR_EM2                       ((uint32_t)0x00000004)              /*!< External match 2 */
#define TIMER_EMR_EM3                       ((uint32_t)0x00000008)              /*!< External match 3 */
#define TIMER_EMR_EMC0                      ((uint32_t)0x00000030)              /*!< External match 0 configuration */
#define TIMER_EMR_EMC1                      ((uint32_t)0x000000C0)              /*!< External match 1 configuration */
#define TIMER_EMR_EMC2                      ((uint32_t)0x00000300)              /*!< External match 2 configuration */
#define TIMER_EMR_EMC3                      ((uint32_t)0x00000C00)              /*!< External match 3 configuration */
#define TIMER_EMR_EMO0                      ((uint32_t)0x00001000)              /*!< Channel 0 default level */
#define TIMER_EMR_EMO1                      ((uint32_t)0x00002000)              /*!< Channel 1 default level */
#define TIMER_EMR_EMO2                      ((uint32_t)0x00004000)              /*!< Channel 2 default level */
#define TIMER_EMR_EMO3                      ((uint32_t)0x00008000)              /*!< Channel 3 default level */

/*******************  Bit definition for TIMER_PWMTH0 register  ***************/
#define TIMER_PWMTH0                        ((uint32_t)0xFFFFFFFF)              /*!< PWM 0 threshold */

/*******************  Bit definition for TIMER_PWMTH1 register  ***************/
#define TIMER_PWMTH1                        ((uint32_t)0xFFFFFFFF)              /*!< PWM 1 threshold */

/*******************  Bit definition for TIMER_PWMTH2 register  ***************/
#define TIMER_PWMTH2                        ((uint32_t)0xFFFFFFFF)              /*!< PWM 2 threshold */

/*******************  Bit definition for TIMER_PWMTH3 register  ***************/
#define TIMER_PWMTH3                        ((uint32_t)0xFFFFFFFF)              /*!< PWM 3 threshold */

/*******************  Bit definition for TIMER_CTCR register  *****************/
#define TIMER_CTCR_MODE0_MASK               ((uint32_t)0x00000003)              /*!< TC0 mode */
#define TIMER_CTCR_MODE0_TIMER              ((uint32_t)0x00000000)              /*!< Timer  */
#define TIMER_CTCR_MODE0_COUNTR_RE          ((uint32_t)0x00000001)              /*!< Counter rising edge */
#define TIMER_CTCR_MODE0_COUNTR_FE          ((uint32_t)0x00000002)              /*!< Counter falling edge */
#define TIMER_CTCR_MODE0_COUNTR_DE          ((uint32_t)0x00000003)              /*!< Counter double edge */
#define TIMER_CTCR_MODE1_MASK               ((uint32_t)0x0000000C)              /*!< TC1 mode */
#define TIMER_CTCR_MODE1_TIMER              ((uint32_t)0x00000000)              /*!< Timer  */
#define TIMER_CTCR_MODE1_COUNTR_RE          ((uint32_t)0x00000004)              /*!< Counter rising edge */
#define TIMER_CTCR_MODE1_COUNTR_FE          ((uint32_t)0x00000008)              /*!< Counter falling edge */
#define TIMER_CTCR_MODE1_COUNTR_DE          ((uint32_t)0x0000000C)              /*!< Counter double edge */
#define TIMER_CTCR_MODE2_MASK               ((uint32_t)0x00000030)              /*!< TC2 mode */
#define TIMER_CTCR_MODE2_TIMER              ((uint32_t)0x00000000)              /*!< Timer  */
#define TIMER_CTCR_MODE2_COUNTR_RE          ((uint32_t)0x00000010)              /*!< Counter rising edge */
#define TIMER_CTCR_MODE2_COUNTR_FE          ((uint32_t)0x00000020)              /*!< Counter falling edge */
#define TIMER_CTCR_MODE2_COUNTR_DE          ((uint32_t)0x00000030)              /*!< Counter double edge */
#define TIMER_CTCR_MODE3_MASK               ((uint32_t)0x000000C0)              /*!< TC3 mode */
#define TIMER_CTCR_MODE3_TIMER              ((uint32_t)0x00000000)              /*!< Timer  */
#define TIMER_CTCR_MODE3_COUNTR_RE          ((uint32_t)0x00000040)              /*!< Counter rising edge */
#define TIMER_CTCR_MODE3_COUNTR_FE          ((uint32_t)0x00000080)              /*!< Counter falling edge */
#define TIMER_CTCR_MODE3_COUNTR_DE          ((uint32_t)0x000000C0)              /*!< Counter double edge */

/*******************  Bit definition for TIMER_PWMCR register  ****************/
#define TIMER_PWMCR_EN0                     ((uint32_t)0x00000001)              /*!< Enable PWM0 */
#define TIMER_PWMCR_EN1                     ((uint32_t)0x00000002)              /*!< Enable PWM1 */
#define TIMER_PWMCR_EN2                     ((uint32_t)0x00000004)              /*!< Enable PWM2 */
#define TIMER_PWMCR_EN3                     ((uint32_t)0x00000008)              /*!< Enable PWM3 */
#define TIMER_PWMCR_PULSE_COUNTER0          ((uint32_t)0x00000010)              /*!< Enable pulse counter 0 */
#define TIMER_PWMCR_PULSE_COUNTER1          ((uint32_t)0x00000020)              /*!< Enable pulse counter 1 */
#define TIMER_PWMCR_PULSE_COUNTER2          ((uint32_t)0x00000040)              /*!< Enable pulse counter 2 */
#define TIMER_PWMCR_PULSE_COUNTER3          ((uint32_t)0x00000080)              /*!< Enable pulse counter 3 */
#define TIMER_PWMCR_A0HIZ                   ((uint32_t)0x00000100)              /*!< CH0 Active level */
#define TIMER_PWMCR_INA0HIZ                 ((uint32_t)0x00000200)              /*!< CH0 Inactive level */
#define TIMER_PWMCR_A1HIZ                   ((uint32_t)0x00000400)              /*!< CH1 Active level */
#define TIMER_PWMCR_INA1HIZ                 ((uint32_t)0x00000800)              /*!< CH1 Inactive level */
#define TIMER_PWMCR_A2HIZ                   ((uint32_t)0x00001000)              /*!< CH2 Active level */
#define TIMER_PWMCR_INA2HIZ                 ((uint32_t)0x00002000)              /*!< CH2 Inactive level */
#define TIMER_PWMCR_A3HIZ                   ((uint32_t)0x00004000)              /*!< CH3 Active level */
#define TIMER_PWMCR_INA3HIZ                 ((uint32_t)0x00008000)              /*!< CH3 Inactive level */
#define TIMER_PWMCR_INV0                    ((uint32_t)0x00010000)              /*!< Invert CH0 */
#define TIMER_PWMCR_INV1                    ((uint32_t)0x00020000)              /*!< Invert CH1 */
#define TIMER_PWMCR_INV2                    ((uint32_t)0x00040000)              /*!< Invert CH2 */
#define TIMER_PWMCR_INV3                    ((uint32_t)0x00080000)              /*!< Invert CH3 */
#define TIMER_PWMCR_PULSE_COUNTER0_INT      ((uint32_t)0x00100000)              /*!< Enable pwm channel0 pulse counter interrupt */
#define TIMER_PWMCR_PULSE_COUNTER1_INT      ((uint32_t)0x00200000)              /*!< Enable pwm channel1 pulse counter interrupt */
#define TIMER_PWMCR_PULSE_COUNTER2_INT      ((uint32_t)0x00400000)              /*!< Enable pwm channel2 pulse counter interrupt */
#define TIMER_PWMCR_PULSE_COUNTER3_INT      ((uint32_t)0x00800000)              /*!< Enable pwm channel3 pulse counter interrupt */

/*******************  Bit definition for TIMER_PWMV0 register  ****************/
#define TIMER_PWMV0_VALUE_MASK              ((uint32_t)0xFFFFFFFF)              /*!< PWM channel0 count */

/*******************  Bit definition for TIMER_PWMV1 register  ****************/
#define TIMER_PWMV1_VALUE_MASK              ((uint32_t)0xFFFFFFFF)              /*!< PWM channel1 count */

/*******************  Bit definition for TIMER_PWMV2 register  ****************/
#define TIMER_PWMV2_VALUE_MASK              ((uint32_t)0xFFFFFFFF)              /*!< PWM channel2 count */

/*******************  Bit definition for TIMER_PWMV3 register  ****************/
#define TIMER_PWMV3_VALUE_MASK              ((uint32_t)0xFFFFFFFF)              /*!< PWM channel3 count */

/******************************************************************************/
/*                                                                            */
/*                                    MCPWM                                   */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for MCPWM_CON register  ******************/
#define MCPWM_CON_RUN0                      ((uint32_t)0x00000001)          /*!< Start or stop mcpwm0 */
#define MCPWM_CON_CENTER0                   ((uint32_t)0x00000002)          /*!< Channel 0 Edge-align or center-align */
#define MCPWM_CON_POLA0                     ((uint32_t)0x00000004)          /*!< Channel 0 status level */
#define MCPWM_CON_DTE0                      ((uint32_t)0x00000008)          /*!< Enable or disable dead time of channel 0 */
#define MCPWM_CON_DISUP0                    ((uint32_t)0x00000010)          /*!< Enable or disable update function of channel 0 */
#define MCPWM_CON_RUN1                      ((uint32_t)0x00000100)          /*!< Start or stop mcpwm1 */
#define MCPWM_CON_CENTER1                   ((uint32_t)0x00000200)          /*!< Channel 1 Edge-align or center-align */
#define MCPWM_CON_POLA1                     ((uint32_t)0x00000400)          /*!< Channel 1 status level */
#define MCPWM_CON_DTE1                      ((uint32_t)0x00000800)          /*!< Enable or disable dead time of channel 1 */
#define MCPWM_CON_DISUP1                    ((uint32_t)0x00001000)          /*!< Enable or disable update function of channel 1 */
#define MCPWM_CON_RUN2                      ((uint32_t)0x00010000)          /*!< Start or stop mcpwm2 */
#define MCPWM_CON_CENTER2                   ((uint32_t)0x00020000)          /*!< Channel 2 Edge-align or center-align */
#define MCPWM_CON_POLA2                     ((uint32_t)0x00040000)          /*!< Channel 2 status level */
#define MCPWM_CON_DTE2                      ((uint32_t)0x00080000)          /*!< Enable or disable dead time of channel 2 */
#define MCPWM_CON_DISUP2                    ((uint32_t)0x00100000)          /*!< Enable or disable update function of channel 2 */
#define MCPWM_CON_HALL                      ((uint32_t)0x01000000)          /*!< Enable or disable HALL function */
#define MCPWM_CON_VEL                       ((uint32_t)0x02000000)          /*!< Enable or disable measuring speed function */
#define MCPWM_CON_INVBDC                    ((uint32_t)0x20000000)          /*!< Polarity of MCOB */
#define MCPWM_CON_ACMODE                    ((uint32_t)0x40000000)          /*!< AC mode */
#define MCPWM_CON_DCMODE                    ((uint32_t)0x80000000)          /*!< DC mode */

/*******************  Bit definition for MCPWM_CAP register  ******************/
#define MCPWM_CAPCON_C0_SOURCE              ((uint32_t)0x0000003F)          /*!< Channel 0 capture source */
#define MCPWM_CAPCON_C0T0_RE                ((uint32_t)0x00000001)          /*!< Capture channel 0 when MCI0 rising edge */
#define MCPWM_CAPCON_C0T0_FE                ((uint32_t)0x00000002)          /*!< Capture channel 0 when MCI0 falling edge */
#define MCPWM_CAPCON_C0T1_RE                ((uint32_t)0x00000004)          /*!< Capture channel 0 when MCI1 rising edge */
#define MCPWM_CAPCON_C0T1_FE                ((uint32_t)0x00000008)          /*!< Capture channel 0 when MCI1 falling edge */
#define MCPWM_CAPCON_C0T2_RE                ((uint32_t)0x00000010)          /*!< Capture channel 0 when MCI2 rising edge */
#define MCPWM_CAPCON_C0T2_FE                ((uint32_t)0x00000020)          /*!< Capture channel 0 when MCI2 falling edge */
#define MCPWM_CAPCON_C1_SOURCE              ((uint32_t)0x00000FC0)          /*!< Channel 1 capture source */
#define MCPWM_CAPCON_C1T0_RE                ((uint32_t)0x00000040)          /*!< Capture channel 1 when MCI0 rising edge */
#define MCPWM_CAPCON_C1T0_FE                ((uint32_t)0x00000080)          /*!< Capture channel 1 when MCI0 falling edge */
#define MCPWM_CAPCON_C1T1_RE                ((uint32_t)0x00000100)          /*!< Capture channel 1 when MCI1 rising edge */
#define MCPWM_CAPCON_C1T1_FE                ((uint32_t)0x00000200)          /*!< Capture channel 1 when MCI1 falling edge */
#define MCPWM_CAPCON_C1T2_RE                ((uint32_t)0x00000400)          /*!< Capture channel 1 when MCI2 rising edge */
#define MCPWM_CAPCON_C1T2_FE                ((uint32_t)0x00000800)          /*!< Capture channel 1 when MCI2 falling edge */
#define MCPWM_CAPCON_C2_SOURCE              ((uint32_t)0x0003F000)          /*!< Channel 2 capture source */
#define MCPWM_CAPCON_C2T0_RE                ((uint32_t)0x00001000)          /*!< Capture channel 2 when MCI0 rising edge */
#define MCPWM_CAPCON_C2T0_FE                ((uint32_t)0x00002000)          /*!< Capture channel 2 when MCI0 falling edge */
#define MCPWM_CAPCON_C2T1_RE                ((uint32_t)0x00004000)          /*!< Capture channel 2 when MCI1 rising edge */
#define MCPWM_CAPCON_C2T1_FE                ((uint32_t)0x00008000)          /*!< Capture channel 2 when MCI1 falling edge */
#define MCPWM_CAPCON_C2T2_RE                ((uint32_t)0x00010000)          /*!< Capture channel 2 when MCI2 rising edge */
#define MCPWM_CAPCON_C2T2_FE                ((uint32_t)0x00020000)          /*!< Capture channel 2 when MCI2 falling edge */
#define MCPWM_CAPCON_RT0                    ((uint32_t)0x00040000)          /*!< Reset TC when capturing channel 0 */
#define MCPWM_CAPCON_RT1                    ((uint32_t)0x00080000)          /*!< Reset TC when capturing channel 1 */
#define MCPWM_CAPCON_RT2                    ((uint32_t)0x00100000)          /*!< Reset TC when capturing channel 2 */
#define MCPWM_CAPCON_HNFCAP0                ((uint32_t)0x00200000)          /*!< Enable or disable noise filter of channel 0 */
#define MCPWM_CAPCON_HNFCAP1                ((uint32_t)0x00400000)          /*!< Enable or disable noise filter of channel 0 */
#define MCPWM_CAPCON_HNFCAP2                ((uint32_t)0x00800000)          /*!< Enable or disable noise filter of channel 0 */

/*******************  Bit definition for MCPWM_TC0 register  ******************/
#define MCPWM_TC0                           ((uint32_t)0xFFFFFFFF)          /*!< TC0 */

/*******************  Bit definition for MCPWM_TC1 register  ******************/
#define MCPWM_TC1                           ((uint32_t)0xFFFFFFFF)          /*!< TC1 */

/*******************  Bit definition for MCPWM_TC2 register  ******************/
#define MCPWM_TC2                           ((uint32_t)0xFFFFFFFF)          /*!< TC2 */

/*******************  Bit definition for MCPWM_LIM0 register  *****************/
#define MCPWM_LIM0                          ((uint32_t)0xFFFFFFFF)          /*!< LIM0 */

/*******************  Bit definition for MCPWM_LIM1 register  *****************/
#define MCPWM_LIM1                          ((uint32_t)0xFFFFFFFF)          /*!< LIM1 */

/*******************  Bit definition for MCPWM_LIM2 register  *****************/
#define MCPWM_LIM2                          ((uint32_t)0xFFFFFFFF)          /*!< LIM2 */

/*******************  Bit definition for MCPWM_MAT0 register  *****************/
#define MCPWM_MAT0                          ((uint32_t)0xFFFFFFFF)          /*!< MAT0 */

/*******************  Bit definition for MCPWM_MAT1 register  *****************/
#define MCPWM_MAT1                          ((uint32_t)0xFFFFFFFF)          /*!< MAT1 */

/*******************  Bit definition for MCPWM_MAT2 register  *****************/
#define MCPWM_MAT2                          ((uint32_t)0xFFFFFFFF)          /*!< MAT2 */

/*******************  Bit definition for MCPWM_DT register  *******************/
#define MCPWM_DT_DT0                        ((uint32_t)0x000003FF)          /*!< Dead time 0 */
#define MCPWM_DT_DT1                        ((uint32_t)0x000FFC00)          /*!< Dead time 1 */
#define MCPWM_DT_DT2                        ((uint32_t)0x3FF00000)          /*!< Dead time 2 */

/*******************  Bit definition for MCPWM_CP register  *******************/
#define MCPWM_CP_CPA0                       ((uint32_t)0x00000001)          /*!< MCOA0 signal */
#define MCPWM_CP_CPB0                       ((uint32_t)0x00000002)          /*!< MCOB0 signal */
#define MCPWM_CP_CPA1                       ((uint32_t)0x00000004)          /*!< MCOA1 signal */
#define MCPWM_CP_CPB1                       ((uint32_t)0x00000008)          /*!< MCOB1 signal */
#define MCPWM_CP_CPA2                       ((uint32_t)0x00000010)          /*!< MCOA2 signal */
#define MCPWM_CP_CPB2                       ((uint32_t)0x00000020)          /*!< MCOB2 signal */

/*******************  Bit definition for MCPWM_CAP0 register  *****************/
#define MCPWM_CAP0                          ((uint32_t)0xFFFFFFFF)          /*!< Capture register of channel 0 */

/*******************  Bit definition for MCPWM_CAP1 register  *****************/
#define MCPWM_CAP1                          ((uint32_t)0xFFFFFFFF)          /*!< Capture register of channel 1 */

/*******************  Bit definition for MCPWM_CAP2 register  *****************/
#define MCPWM_CAP2                          ((uint32_t)0xFFFFFFFF)          /*!< Capture register of channel 2 */

/*******************  Bit definition for MCPWM_INTEN register  ****************/
#define MCPWM_INTEN_ILIM0                   ((uint32_t)0x00000001)          /*!< Channel 0 limit interrupt */
#define MCPWM_INTEN_IMAT0                   ((uint32_t)0x00000002)          /*!< Channel 0 match interrupt */
#define MCPWM_INTEN_ICAP0                   ((uint32_t)0x00000004)          /*!< Channel 0 capture interrupt */
#define MCPWM_INTEN_ILIM1                   ((uint32_t)0x00000010)          /*!< Channel 1 limit interrupt */
#define MCPWM_INTEN_IMAT1                   ((uint32_t)0x00000020)          /*!< Channel 1 match interrupt */
#define MCPWM_INTEN_ICAP1                   ((uint32_t)0x00000040)          /*!< Channel 1 capture interrupt */
#define MCPWM_INTEN_ILIM2                   ((uint32_t)0x00000100)          /*!< Channel 2 limit interrupt */
#define MCPWM_INTEN_IMAT2                   ((uint32_t)0x00000200)          /*!< Channel 2 match interrupt */
#define MCPWM_INTEN_ICAP2                   ((uint32_t)0x00000400)          /*!< Channel 2 capture interrupt */
#define MCPWM_INTEN_ABORT                   ((uint32_t)0x00001000)          /*!< Abort interrupt */
#define MCPWM_INTEN_STCHG                   ((uint32_t)0x00010000)          /*!< Status change interrupt */
#define MCPWM_INTEN_STERR                   ((uint32_t)0x00020000)          /*!< Status error interrupt */
#define MCPWM_INTEN_VSLOW                   ((uint32_t)0x00040000)          /*!< Velocity low interrupt */
#define MCPWM_INTEN_VFAST                   ((uint32_t)0x00080000)          /*!< Velocity fast interrupt */

/*******************  Bit definition for MCPWM CNTCON register  ***************/
#define MCPWM_CNTCON_TC0_SOURCE             ((uint32_t)0x0000003F)          /*!< TC0 Counter mode */
#define MCPWM_CNTCON_TC0I0_RE               ((uint32_t)0x00000001)          /*!< Counter mode */
#define MCPWM_CNTCON_TC0I0_FE               ((uint32_t)0x00000002)          /*!< Counter mode */
#define MCPWM_CNTCON_TC0I1_RE               ((uint32_t)0x00000004)          /*!< Counter mode */
#define MCPWM_CNTCON_TC0I1_FE               ((uint32_t)0x00000008)          /*!< Counter mode */
#define MCPWM_CNTCON_TC0I2_RE               ((uint32_t)0x00000010)          /*!< Counter mode */
#define MCPWM_CNTCON_TC0I2_FE               ((uint32_t)0x00000020)          /*!< Counter mode */
#define MCPWM_CNTCON_TC1_SOURCE             ((uint32_t)0x00000FC0)          /*!< TC1 Counter mode */
#define MCPWM_CNTCON_TC1I0_RE               ((uint32_t)0x00000040)          /*!< Counter mode */
#define MCPWM_CNTCON_TC1I0_FE               ((uint32_t)0x00000080)          /*!< Counter mode */
#define MCPWM_CNTCON_TC1I1_RE               ((uint32_t)0x00000100)          /*!< Counter mode */
#define MCPWM_CNTCON_TC1I1_FE               ((uint32_t)0x00000200)          /*!< Counter mode */
#define MCPWM_CNTCON_TC1I2_RE               ((uint32_t)0x00000400)          /*!< Counter mode */
#define MCPWM_CNTCON_TC1I2_FE               ((uint32_t)0x00000800)          /*!< Counter mode */
#define MCPWM_CNTCON_TC2_SOURCE             ((uint32_t)0x0003F000)          /*!< TC2 Counter mode */
#define MCPWM_CNTCON_TC2I0_RE               ((uint32_t)0x00001000)          /*!< Counter mode */
#define MCPWM_CNTCON_TC2I0_FE               ((uint32_t)0x00002000)          /*!< Counter mode */
#define MCPWM_CNTCON_TC2I1_RE               ((uint32_t)0x00004000)          /*!< Counter mode */
#define MCPWM_CNTCON_TC2I1_FE               ((uint32_t)0x00008000)          /*!< Counter mode */
#define MCPWM_CNTCON_TC2I2_RE               ((uint32_t)0x00010000)          /*!< Counter mode */
#define MCPWM_CNTCON_TC2I2_FE               ((uint32_t)0x00020000)          /*!< Counter mode */
#define MCPWM_CNTCON_CNTR0                  ((uint32_t)0x20000000)          /*!< Timer or Counter */
#define MCPWM_CNTCON_CNTR1                  ((uint32_t)0x40000000)          /*!< Timer or Counter */
#define MCPWM_CNTCON_CNTR2                  ((uint32_t)0x80000000)          /*!< Timer or Counter */

/*******************  Bit definition for MCPWM INTF register  *****************/
#define MCPWM_INTF_ILIM0                    ((uint32_t)0x00000001)          /*!< Channel 0 limit interrupt flag */
#define MCPWM_INTF_IMAT0                    ((uint32_t)0x00000002)          /*!< Channel 0 match interrupt flag */
#define MCPWM_INTF_ICAP0                    ((uint32_t)0x00000004)          /*!< Channel 0 capture interrupt flag */
#define MCPWM_INTF_ILIM1                    ((uint32_t)0x00000010)          /*!< Channel 1 limit interrupt flag */
#define MCPWM_INTF_IMAT1                    ((uint32_t)0x00000020)          /*!< Channel 1 match interrupt flag */
#define MCPWM_INTF_ICAP1                    ((uint32_t)0x00000040)          /*!< Channel 1 capture interrupt flag */
#define MCPWM_INTF_ILIM2                    ((uint32_t)0x00000100)          /*!< Channel 2 limit interrupt flag */
#define MCPWM_INTF_IMAT2                    ((uint32_t)0x00000200)          /*!< Channel 2 match interrupt flag */
#define MCPWM_INTF_ICAP2                    ((uint32_t)0x00000400)          /*!< Channel 2 capture interrupt flag */
#define MCPWM_INTF_ABORT                    ((uint32_t)0x00001000)          /*!< Abort interrupt flag */
#define MCPWM_INTF_STCHG                    ((uint32_t)0x00010000)          /*!< Status change interrupt flag */
#define MCPWM_INTF_STERR                    ((uint32_t)0x00020000)          /*!< Status error interrupt flag */
#define MCPWM_INTF_VSLOW                    ((uint32_t)0x00040000)          /*!< Velocity low interrupt flag */
#define MCPWM_INTF_VFAST                    ((uint32_t)0x00080000)          /*!< Velocity fast interrupt flag */

/*******************  Bit definition for MCPWM CAPCLR register  ***************/
#define MCPWM_CAPCLR_CLR0                   ((uint32_t)0x00000001)          /*!< Clear CR0 */
#define MCPWM_CAPCLR_CLR1                   ((uint32_t)0x00000002)          /*!< Clear CR1 */
#define MCPWM_CAPCLR_CLR2                   ((uint32_t)0x00000004)          /*!< Clear CR2 */

/*******************  Bit definition for MCPWM HALL register  *****************/
#define MCPWM_HALL_MCP_MASK                 ((uint32_t)0x0000003F)          /*!< MCP MASK */
#define MCPWM_HALL_MCPA0                    ((uint32_t)0x00000001)          /*!< Enable or disable MCOA0 */
#define MCPWM_HALL_MCPB0                    ((uint32_t)0x00000002)          /*!< Enable or disable MCOB0 */
#define MCPWM_HALL_MCPA1                    ((uint32_t)0x00000004)          /*!< Enable or disable MCOA2 */
#define MCPWM_HALL_MCPB1                    ((uint32_t)0x00000008)          /*!< Enable or disable MCOB1 */
#define MCPWM_HALL_MCPA2                    ((uint32_t)0x00000010)          /*!< Enable or disable MCOA2 */
#define MCPWM_HALL_MCPB2                    ((uint32_t)0x00000020)          /*!< Enable or disable MCOB2 */
#define MCPWM_HALL_INS_MASK                 ((uint32_t)0x00003F00)          /*!< Inactive status MASK */
#define MCPWM_HALL_PSLA0                    ((uint32_t)0x00000100)          /*!< Inactive status of MCOA0 */
#define MCPWM_HALL_PSLB0                    ((uint32_t)0x00000200)          /*!< Inactive status of MCOB0 */
#define MCPWM_HALL_PSLA1                    ((uint32_t)0x00000400)          /*!< Inactive status of MCOA1 */
#define MCPWM_HALL_PSLB1                    ((uint32_t)0x00000800)          /*!< Inactive status of MCOB1 */
#define MCPWM_HALL_PSLA2                    ((uint32_t)0x00001000)          /*!< Inactive status of MCOA2 */
#define MCPWM_HALL_PSLB2                    ((uint32_t)0x00002000)          /*!< Inactive status of MCOB2 */
#define MCPWM_HALL_EXPH                     ((uint32_t)0x00070000)          /*!< Expected HALL status */
#define MCPWM_HALL_CURH                     ((uint32_t)0x00700000)          /*!< Current HALL status */

/*******************  Bit definition for MCPWM HALLS register  *****************/
#define MCPWM_HALLS_MCPA0                   ((uint32_t)0x00000001)          /*!< Enable or disable MCOA0 (SHADOW) */
#define MCPWM_HALLS_MCPB0                   ((uint32_t)0x00000002)          /*!< Enable or disable MCOB0 (SHADOW) */
#define MCPWM_HALLS_MCPA1                   ((uint32_t)0x00000004)          /*!< Enable or disable MCOA2 (SHADOW) */
#define MCPWM_HALLS_MCPB1                   ((uint32_t)0x00000008)          /*!< Enable or disable MCOB1 (SHADOW) */
#define MCPWM_HALLS_MCPA2                   ((uint32_t)0x00000010)          /*!< Enable or disable MCOA2 (SHADOW) */
#define MCPWM_HALLS_MCPB2                   ((uint32_t)0x00000020)          /*!< Enable or disable MCOB2 (SHADOW) */
#define MCPWM_HALLS_PSLA0                   ((uint32_t)0x00000100)          /*!< Inactive status of MCOA0 (SHADOW) */
#define MCPWM_HALLS_PSLB0                   ((uint32_t)0x00000200)          /*!< Inactive status of MCOB0 (SHADOW) */
#define MCPWM_HALLS_PSLA1                   ((uint32_t)0x00000400)          /*!< Inactive status of MCOA1 (SHADOW) */
#define MCPWM_HALLS_PSLB1                   ((uint32_t)0x00000800)          /*!< Inactive status of MCOB1 (SHADOW) */
#define MCPWM_HALLS_PSLA2                   ((uint32_t)0x00001000)          /*!< Inactive status of MCOA2 (SHADOW) */
#define MCPWM_HALLS_PSLB2                   ((uint32_t)0x00002000)          /*!< Inactive status of MCOB2 (SHADOW) */
#define MCPWM_HALLS_EXPH                    ((uint32_t)0x00010000)          /*!< Expected HALL status (SHADOW) */
#define MCPWM_HALLS_CURH                    ((uint32_t)0x00100000)          /*!< Current HALL status (SHADOW) */

/*******************  Bit definition for MCPWM VELCMP register  ***************/
#define MCPWM_VELCMP                        ((uint32_t)0xFFFFFFFF)          /*!< Velocity comparison value */

/*******************  Bit definition for MCPWM VELVAL register  ***************/
#define MCPWM_VELVAL                        ((uint32_t)0xFFFFFFFF)          /*!< Current velocity value */

/*******************  Bit definition for MCPWM TH register  *******************/
#define MCPWM_TH                            ((uint32_t)0xFFFFFFFF)          /*!< Velocity threshold value */

/*******************  Bit definition for MCPWM MCSIT register  ****************/
#define MCPWM_MCIST                         ((uint32_t)0x00000007)          /*!< MCI level */
#define MCPWM_MCIST_MCI0                    ((uint32_t)0x00000001)          /*!< MCI0 level */
#define MCPWM_MCIST_MCI1                    ((uint32_t)0x00000002)          /*!< MCI1 level */
#define MCPWM_MCIST_MCI2                    ((uint32_t)0x00000004)          /*!< MCI2 level */

/******************************************************************************/
/*                                                                            */
/*                      Direct Memory Access Interface                        */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for SAR register  ************************/
#define DMA_SAR_MASK                        ((uint32_t)0xFFFFFFFF)          /*!< Source address */

/*******************  Bit definition for DAR register  ************************/
#define DMA_DAR_MASK                        ((uint32_t)0xFFFFFFFF)          /*!< Destination address */

/*******************  Bit definition for LLP register  ************************/
#define DMA_LLP_LOC_MASK                    ((uint32_t)0xFFFFFFFC)          /*!< Starting address in memory of next LLI if block chaining is enabled*/

/*******************  Bit definition for CTL_L register  **********************/
#define DMA_CTL_L_INT_EN                    ((uint32_t)0x00000001)          /*!< Interrupt enable bit */
#define DMA_CTL_L_DST_TR_WIDTH_MASK         ((uint32_t)0x0000000e)          /*!< Destination transfer width */
#define DMA_CTL_L_SRC_TR_WIDTH_MASK         ((uint32_t)0x00000070)          /*!< Source transfer width */
#define DMA_CTL_L_DINC_MASK                 ((uint32_t)0x00000180)          /*!< Destination address increment */
#define DMA_CTL_L_SINC_MASK                 ((uint32_t)0x00000600)          /*!< Source address increment */
#define DMA_CTL_L_DEST_MSIZE_MASK           ((uint32_t)0x00003800)          /*!< Destination burst transaction length */
#define DMA_CTL_L_SRC_MSIZE_MASK            ((uint32_t)0x0001C000)          /*!< Source burst transaction length */
#define DMA_CTL_L_SRC_GATHER_EN	            ((uint32_t)0x00020000)          /*!< Source gather enable bit */
#define DMA_CTL_L_DST_SCATTER_EN            ((uint32_t)0x00040000)          /*!< Destination scatter enable bit */
#define DMA_CTL_L_TT_FC_MASK                ((uint32_t)0x00700000)          /*!< Transfer type and flow control */
#define DMA_CTL_L_DMS_MASK                  ((uint32_t)0x01800000)          /*!< Destination master select */
#define DMA_CTL_L_SMS_MASK                  ((uint32_t)0x06000000)          /*!< Source master select */
#define DMA_CTL_L_LLP_DST_EN                ((uint32_t)0x08000000)          /*!< Block chaining is enabled on the destination side */
#define DMA_CTL_L_LLP_SRC_EN                ((uint32_t)0x10000000)          /*!< Block chaining is enabled on the source side */

/*******************  Bit definition for CTL_H register  **********************/
#define DMA_CTL_H_BLOCK_TS_MASK             ((uint32_t)0x00000fff)          /*!< Block transfer size */
#define DMA_CTL_H_DONE                      ((uint32_t)0x00001000)          /*!< Done bit */

/*******************  Bit definition for SSTAT register  **********************/
#define DMA_SSTAT_MASK                      ((uint32_t)0xFFFFFFFF)          /*!< Source status */

/*******************  Bit definition for DSTAT register  **********************/
#define DMA_DSTAT_MASK                      ((uint32_t)0xFFFFFFFF)          /*!< Destination Status */

/*******************  Bit definition for SSTATAR register  ********************/
#define DMA_SSTATAR_MASK                    ((uint32_t)0xFFFFFFFF)          /*!< Source Status Address */

/*******************  Bit definition for DSTATAR register  ********************/
#define DMA_DSTATAR_MASK                    ((uint32_t)0xFFFFFFFF)          /*!< Destination Status Address */

/*******************  Bit definition for CFG_L register  **********************/
#define DMA_CFG_L_CH_PRIOR_MASK             ((uint32_t)0x000000e0)          /*!< Channel priority mask */
#define DMA_CFG_L_CH_SUSP                   ((uint32_t)0x00000100)          /*!< Channel suspend */
#define DMA_CFG_L_FIFO_EMPTY                ((uint32_t)0x00000200)          /*!< Indicates if there is data left in the channel FIFO */
#define DMA_CFG_L_HS_SEL_DST                ((uint32_t)0x00000400)          /*!< Destination Software or Hardware Handshaking Select */
#define DMA_CFG_L_HS_SEL_SRC                ((uint32_t)0x00000800)          /*!< Source Software or Hardware Handshaking Select */
#define DMA_CFG_L_LOCK_CH_L                 ((uint32_t)0x00003000)          /*!< Channel Lock Level */
#define DMA_CFG_L_LOCK_CH_L_0      	        ((uint32_t)0x00003000)          /*!< Channel Lock Level:Over complete DMA transfer */
#define DMA_CFG_L_LOCK_CH_L_1               ((uint32_t)0x00003000)          /*!< Channel Lock Level:Over complete DMA block transfer */
#define DMA_CFG_L_LOCK_CH_L_2               ((uint32_t)0x00003000)          /*!< Channel Lock Level:Over complete DMA transaction */
#define DMA_CFG_L_LOCK_B_L                  ((uint32_t)0x0000c000)          /*!< Bus Lock Level */
#define DMA_CFG_L_LOCK_B_L_0                ((uint32_t)0x0000c000)          /*!< Bus Lock Level:Over complete DMA transfer */
#define DMA_CFG_L_LOCK_B_L_1                ((uint32_t)0x0000c000)          /*!< Bus Lock Level:Over complete DMA block transfer */
#define DMA_CFG_L_LOCK_B_L_2                ((uint32_t)0x0000c000)          /*!< Bus Lock Level:Over complete DMA transaction */
#define DMA_CFG_L_LOCK_CH                   ((uint32_t)0x00010000)          /*!< Channel Lock bit */
#define DMA_CFG_L_LOCK_B                    ((uint32_t)0x00020000)          /*!< Bus Lock bit */
#define DMA_CFG_L_DST_HS_POL                ((uint32_t)0x00040000)          /*!< Destination Handshaking Interface Polarity */
#define DMA_CFG_L_SRC_HS_POL                ((uint32_t)0x00080000)          /*!< Source Handshaking Interface Polarity */
#define DMA_CFG_L_MAX_ABRST_MASK            ((uint32_t)0x3ff00000)          /*!< Maximum AMBA Burst Length */
#define DMA_CFG_L_RELOAD_SRC                ((uint32_t)0x40000000)          /*!< Automatic Source Reload */
#define DMA_CFG_L_RELOAD_DST                ((uint32_t)0x80000000)          /*!< Automatic Destination Reload */

/*******************  Bit definition for CFG_H register  **********************/
#define DMA_CFG_H_CH_FCMODE                 ((uint32_t)0x00000001)          /*!< Flow Control Mode */
#define DMA_CFG_H_CH_FIFO_MODE              ((uint32_t)0x00000002)          /*!< FIFO Mode Select */
#define DMA_CFG_H_CH_SRC_PER                ((uint32_t)0x00000780)          /*!< Assigns a hardware handshaking interface to the source of channel */
#define DMA_CFG_H_CH_DEST_PER               ((uint32_t)0x00007800)          /*!< Assigns a hardware handshaking interface to the destination of channel */

/*******************  Bit definition for Raw interrupt registers  *************/
#define DMA_RawTfr_CHANNEL0                 ((uint32_t)0x00000001)          /*!< Raw interrupt status of Channel0 */
#define DMA_RawTfr_CHANNEL1                 ((uint32_t)0x00000002)          /*!< Raw interrupt status of Channel1 */
#define DMA_RawTfr_CHANNEL2                 ((uint32_t)0x00000004)          /*!< Raw interrupt status of Channel2 */
#define DMA_RawTfr_CHANNEL3                 ((uint32_t)0x00000008)          /*!< Raw interrupt status of Channel3 */
#define DMA_RawTfr_CHANNEL4                 ((uint32_t)0x00000010)          /*!< Raw interrupt status of Channel4 */
#define DMA_RawTfr_CHANNEL5                 ((uint32_t)0x00000020)          /*!< Raw interrupt status of Channel5 */
#define DMA_RawTfr_CHANNEL6                 ((uint32_t)0x00000040)          /*!< Raw interrupt status of Channel6 */
#define DMA_RawTfr_CHANNEL7                 ((uint32_t)0x00000080)          /*!< Raw interrupt status of Channel7 */

#define DMA_RawBlock_CHANNEL0               ((uint32_t)0x00000001)          /*!< Raw interrupt status of Channel0 */
#define DMA_RawBlock_CHANNEL1               ((uint32_t)0x00000002)          /*!< Raw interrupt status of Channel1 */
#define DMA_RawBlock_CHANNEL2               ((uint32_t)0x00000004)          /*!< Raw interrupt status of Channel2 */
#define DMA_RawBlock_CHANNEL3               ((uint32_t)0x00000008)          /*!< Raw interrupt status of Channel3 */
#define DMA_RawBlock_CHANNEL4               ((uint32_t)0x00000010)          /*!< Raw interrupt status of Channel4 */
#define DMA_RawBlock_CHANNEL5               ((uint32_t)0x00000020)          /*!< Raw interrupt status of Channel5 */
#define DMA_RawBlock_CHANNEL6               ((uint32_t)0x00000040)          /*!< Raw interrupt status of Channel6 */
#define DMA_RawBlock_CHANNEL7               ((uint32_t)0x00000080)          /*!< Raw interrupt status of Channel7 */

#define DMA_RawSrcTran_CHANNEL0             ((uint32_t)0x00000001)          /*!< Raw interrupt status of Channel0 */
#define DMA_RawSrcTran_CHANNEL1             ((uint32_t)0x00000002)          /*!< Raw interrupt status of Channel1 */
#define DMA_RawSrcTran_CHANNEL2             ((uint32_t)0x00000004)          /*!< Raw interrupt status of Channel2 */
#define DMA_RawSrcTran_CHANNEL3             ((uint32_t)0x00000008)          /*!< Raw interrupt status of Channel3 */
#define DMA_RawSrcTran_CHANNEL4             ((uint32_t)0x00000010)          /*!< Raw interrupt status of Channel4 */
#define DMA_RawSrcTran_CHANNEL5             ((uint32_t)0x00000020)          /*!< Raw interrupt status of Channel5 */
#define DMA_RawSrcTran_CHANNEL6             ((uint32_t)0x00000040)          /*!< Raw interrupt status of Channel6 */
#define DMA_RawSrcTran_CHANNEL7             ((uint32_t)0x00000080)          /*!< Raw interrupt status of Channel7 */

#define DMA_RawDstTran_CHANNEL0             ((uint32_t)0x00000001)          /*!< Raw interrupt status of Channel0 */
#define DMA_RawDstTran_CHANNEL1             ((uint32_t)0x00000002)          /*!< Raw interrupt status of Channel1 */
#define DMA_RawDstTran_CHANNEL2             ((uint32_t)0x00000004)          /*!< Raw interrupt status of Channel2 */
#define DMA_RawDstTran_CHANNEL3             ((uint32_t)0x00000008)          /*!< Raw interrupt status of Channel3 */
#define DMA_RawDstTran_CHANNEL4             ((uint32_t)0x00000010)          /*!< Raw interrupt status of Channel4 */
#define DMA_RawDstTran_CHANNEL5             ((uint32_t)0x00000020)          /*!< Raw interrupt status of Channel5 */
#define DMA_RawDstTran_CHANNEL6             ((uint32_t)0x00000040)          /*!< Raw interrupt status of Channel6 */
#define DMA_RawDstTran_CHANNEL7             ((uint32_t)0x00000080)          /*!< Raw interrupt status of Channel7 */

#define DMA_RawErr_CHANNEL0                 ((uint32_t)0x00000001)          /*!< Raw interrupt status of Channel0 */
#define DMA_RawErr_CHANNEL1                 ((uint32_t)0x00000002)          /*!< Raw interrupt status of Channel1 */
#define DMA_RawErr_CHANNEL2                 ((uint32_t)0x00000004)          /*!< Raw interrupt status of Channel2 */
#define DMA_RawErr_CHANNEL3                 ((uint32_t)0x00000008)          /*!< Raw interrupt status of Channel3 */
#define DMA_RawErr_CHANNEL4                 ((uint32_t)0x00000010)          /*!< Raw interrupt status of Channel4 */
#define DMA_RawErr_CHANNEL5                 ((uint32_t)0x00000020)          /*!< Raw interrupt status of Channel5 */
#define DMA_RawErr_CHANNEL6                 ((uint32_t)0x00000040)          /*!< Raw interrupt status of Channel6 */
#define DMA_RawErr_CHANNEL7                 ((uint32_t)0x00000080)          /*!< Raw interrupt status of Channel7 */

/*******************  Bit definition for Status interrupt registers  **********/
#define DMA_StatusTfr_CHANNEL0              ((uint32_t)0x00000001)          /*!< Interrupt status of Channel0 */
#define DMA_StatusTfr_CHANNEL1              ((uint32_t)0x00000002)          /*!< Interrupt status of Channel1 */
#define DMA_StatusTfr_CHANNEL2              ((uint32_t)0x00000004)          /*!< Interrupt status of Channel2 */
#define DMA_StatusTfr_CHANNEL3              ((uint32_t)0x00000008)          /*!< Interrupt status of Channel3 */
#define DMA_StatusTfr_CHANNEL4              ((uint32_t)0x00000010)          /*!< Interrupt status of Channel4 */
#define DMA_StatusTfr_CHANNEL5              ((uint32_t)0x00000020)          /*!< Interrupt status of Channel5 */
#define DMA_StatusTfr_CHANNEL6              ((uint32_t)0x00000040)          /*!< Interrupt status of Channel6 */
#define DMA_StatusTfr_CHANNEL7              ((uint32_t)0x00000080)          /*!< Interrupt status of Channel7 */

#define DMA_StatusBlock_CHANNEL0            ((uint32_t)0x00000001)          /*!< Interrupt status of Channel0 */
#define DMA_StatusBlock_CHANNEL1            ((uint32_t)0x00000002)          /*!< Interrupt status of Channel1 */
#define DMA_StatusBlock_CHANNEL2            ((uint32_t)0x00000004)          /*!< Interrupt status of Channel2 */
#define DMA_StatusBlock_CHANNEL3            ((uint32_t)0x00000008)          /*!< Interrupt status of Channel3 */
#define DMA_StatusBlock_CHANNEL4            ((uint32_t)0x00000010)          /*!< Interrupt status of Channel4 */
#define DMA_StatusBlock_CHANNEL5            ((uint32_t)0x00000020)          /*!< Interrupt status of Channel5 */
#define DMA_StatusBlock_CHANNEL6            ((uint32_t)0x00000040)          /*!< Interrupt status of Channel6 */
#define DMA_StatusBlock_CHANNEL7            ((uint32_t)0x00000080)          /*!< Interrupt status of Channel7 */

#define DMA_StatusSrcTran_CHANNEL0          ((uint32_t)0x00000001)          /*!< Interrupt status of Channel0 */
#define DMA_StatusSrcTran_CHANNEL1          ((uint32_t)0x00000002)          /*!< Interrupt status of Channel1 */
#define DMA_StatusSrcTran_CHANNEL2          ((uint32_t)0x00000004)          /*!< Interrupt status of Channel2 */
#define DMA_StatusSrcTran_CHANNEL3          ((uint32_t)0x00000008)          /*!< Interrupt status of Channel3 */
#define DMA_StatusSrcTran_CHANNEL4          ((uint32_t)0x00000010)          /*!< Interrupt status of Channel4 */
#define DMA_StatusSrcTran_CHANNEL5          ((uint32_t)0x00000020)          /*!< Interrupt status of Channel5 */
#define DMA_StatusSrcTran_CHANNEL6          ((uint32_t)0x00000040)          /*!< Interrupt status of Channel6 */
#define DMA_StatusSrcTran_CHANNEL7          ((uint32_t)0x00000080)          /*!< Interrupt status of Channel7 */

#define DMA_StatusDstTran_CHANNEL0          ((uint32_t)0x00000001)          /*!< Interrupt status of Channel0 */
#define DMA_StatusDstTran_CHANNEL1          ((uint32_t)0x00000002)          /*!< Interrupt status of Channel1 */
#define DMA_StatusDstTran_CHANNEL2          ((uint32_t)0x00000004)          /*!< Interrupt status of Channel2 */
#define DMA_StatusDstTran_CHANNEL3          ((uint32_t)0x00000008)          /*!< Interrupt status of Channel3 */
#define DMA_StatusDstTran_CHANNEL4          ((uint32_t)0x00000010)          /*!< Interrupt status of Channel4 */
#define DMA_StatusDstTran_CHANNEL5          ((uint32_t)0x00000020)          /*!< Interrupt status of Channel5 */
#define DMA_StatusDstTran_CHANNEL6          ((uint32_t)0x00000040)          /*!< Interrupt status of Channel6 */
#define DMA_StatusDstTran_CHANNEL7          ((uint32_t)0x00000080)          /*!< Interrupt status of Channel7 */

#define DMA_StatusErr_CHANNEL0              ((uint32_t)0x00000001)          /*!< Interrupt status of Channel0 */
#define DMA_StatusErr_CHANNEL1              ((uint32_t)0x00000002)          /*!< Interrupt status of Channel1 */
#define DMA_StatusErr_CHANNEL2              ((uint32_t)0x00000004)          /*!< Interrupt status of Channel2 */
#define DMA_StatusErr_CHANNEL3              ((uint32_t)0x00000008)          /*!< Interrupt status of Channel3 */
#define DMA_StatusErr_CHANNEL4              ((uint32_t)0x00000010)          /*!< Interrupt status of Channel4 */
#define DMA_StatusErr_CHANNEL5              ((uint32_t)0x00000020)          /*!< Interrupt status of Channel5 */
#define DMA_StatusErr_CHANNEL6              ((uint32_t)0x00000040)          /*!< Interrupt status of Channel6 */
#define DMA_StatusErr_CHANNEL7              ((uint32_t)0x00000080)          /*!< Interrupt status of Channel7 */

/*******************  Bit definition for Mask interrupt registers  ************/
#define DMA_MaskTfr_CHANNEL0                ((uint32_t)0x00000001)          /*!< Interrupt mask of Channel0 */
#define DMA_MaskTfr_CHANNEL1                ((uint32_t)0x00000002)          /*!< Interrupt mask of Channel1 */
#define DMA_MaskTfr_CHANNEL2                ((uint32_t)0x00000004)          /*!< Interrupt mask of Channel2 */
#define DMA_MaskTfr_CHANNEL3                ((uint32_t)0x00000008)          /*!< Interrupt mask of Channel3 */
#define DMA_MaskTfr_CHANNEL4                ((uint32_t)0x00000010)          /*!< Interrupt mask of Channel4 */
#define DMA_MaskTfr_CHANNEL5                ((uint32_t)0x00000020)          /*!< Interrupt mask of Channel5 */
#define DMA_MaskTfr_CHANNEL6                ((uint32_t)0x00000040)          /*!< Interrupt mask of Channel6 */
#define DMA_MaskTfr_CHANNEL7                ((uint32_t)0x00000080)          /*!< Interrupt mask of Channel7 */
#define DMA_MaskTfr_CHANNEL0_WE             ((uint32_t)0x00000100)          /*!< Interrupt mask write enable of Channel0 */
#define DMA_MaskTfr_CHANNEL1_WE             ((uint32_t)0x00000200)          /*!< Interrupt mask write enable of Channel1 */
#define DMA_MaskTfr_CHANNEL2_WE             ((uint32_t)0x00000400)          /*!< Interrupt mask write enable of Channel2 */
#define DMA_MaskTfr_CHANNEL3_WE             ((uint32_t)0x00000800)          /*!< Interrupt mask write enable of Channel3 */
#define DMA_MaskTfr_CHANNEL4_WE             ((uint32_t)0x00001000)          /*!< Interrupt mask write enable of Channel4 */
#define DMA_MaskTfr_CHANNEL5_WE             ((uint32_t)0x00002000)          /*!< Interrupt mask write enable of Channel5 */
#define DMA_MaskTfr_CHANNEL6_WE             ((uint32_t)0x00004000)          /*!< Interrupt mask write enable of Channel6 */
#define DMA_MaskTfr_CHANNEL7_WE             ((uint32_t)0x00008000)          /*!< Interrupt mask write enable of Channel7 */

#define DMA_MaskBlock_CHANNEL0              ((uint32_t)0x00000001)          /*!< Interrupt mask of Channel0 */
#define DMA_MaskBlock_CHANNEL1              ((uint32_t)0x00000002)          /*!< Interrupt mask of Channel1 */
#define DMA_MaskBlock_CHANNEL2              ((uint32_t)0x00000004)          /*!< Interrupt mask of Channel2 */
#define DMA_MaskBlock_CHANNEL3              ((uint32_t)0x00000008)          /*!< Interrupt mask of Channel3 */
#define DMA_MaskBlock_CHANNEL4              ((uint32_t)0x00000010)          /*!< Interrupt mask of Channel4 */
#define DMA_MaskBlock_CHANNEL5              ((uint32_t)0x00000020)          /*!< Interrupt mask of Channel5 */
#define DMA_MaskBlock_CHANNEL6              ((uint32_t)0x00000040)          /*!< Interrupt mask of Channel6 */
#define DMA_MaskBlock_CHANNEL7              ((uint32_t)0x00000080)          /*!< Interrupt mask of Channel7 */
#define DMA_MaskBlock_CHANNEL0_WE           ((uint32_t)0x00000100)          /*!< Interrupt mask write enable of Channel0 */
#define DMA_MaskBlock_CHANNEL1_WE           ((uint32_t)0x00000200)          /*!< Interrupt mask write enable of Channel1 */
#define DMA_MaskBlock_CHANNEL2_WE           ((uint32_t)0x00000400)          /*!< Interrupt mask write enable of Channel2 */
#define DMA_MaskBlock_CHANNEL3_WE           ((uint32_t)0x00000800)          /*!< Interrupt mask write enable of Channel3 */
#define DMA_MaskBlock_CHANNEL4_WE           ((uint32_t)0x00001000)          /*!< Interrupt mask write enable of Channel4 */
#define DMA_MaskBlock_CHANNEL5_WE           ((uint32_t)0x00002000)          /*!< Interrupt mask write enable of Channel5 */
#define DMA_MaskBlock_CHANNEL6_WE           ((uint32_t)0x00004000)          /*!< Interrupt mask write enable of Channel6 */
#define DMA_MaskBlock_CHANNEL7_WE           ((uint32_t)0x00008000)          /*!< Interrupt mask write enable of Channel7 */

#define DMA_MaskSrcTran_CHANNEL0            ((uint32_t)0x00000001)          /*!< Interrupt mask of Channel0 */
#define DMA_MaskSrcTran_CHANNEL1            ((uint32_t)0x00000002)          /*!< Interrupt mask of Channel1 */
#define DMA_MaskSrcTran_CHANNEL2            ((uint32_t)0x00000004)          /*!< Interrupt mask of Channel2 */
#define DMA_MaskSrcTran_CHANNEL3            ((uint32_t)0x00000008)          /*!< Interrupt mask of Channel3 */
#define DMA_MaskSrcTran_CHANNEL4            ((uint32_t)0x00000010)          /*!< Interrupt mask of Channel4 */
#define DMA_MaskSrcTran_CHANNEL5            ((uint32_t)0x00000020)          /*!< Interrupt mask of Channel5 */
#define DMA_MaskSrcTran_CHANNEL6            ((uint32_t)0x00000040)          /*!< Interrupt mask of Channel6 */
#define DMA_MaskSrcTran_CHANNEL7            ((uint32_t)0x00000080)          /*!< Interrupt mask of Channel7 */
#define DMA_MaskSrcTran_CHANNEL0_WE         ((uint32_t)0x00000100)          /*!< Interrupt mask write enable of Channel0 */
#define DMA_MaskSrcTran_CHANNEL1_WE         ((uint32_t)0x00000200)          /*!< Interrupt mask write enable of Channel1 */
#define DMA_MaskSrcTran_CHANNEL2_WE         ((uint32_t)0x00000400)          /*!< Interrupt mask write enable of Channel2 */
#define DMA_MaskSrcTran_CHANNEL3_WE         ((uint32_t)0x00000800)          /*!< Interrupt mask write enable of Channel3 */
#define DMA_MaskSrcTran_CHANNEL4_WE         ((uint32_t)0x00001000)          /*!< Interrupt mask write enable of Channel4 */
#define DMA_MaskSrcTran_CHANNEL5_WE         ((uint32_t)0x00002000)          /*!< Interrupt mask write enable of Channel5 */
#define DMA_MaskSrcTran_CHANNEL6_WE         ((uint32_t)0x00004000)          /*!< Interrupt mask write enable of Channel6 */
#define DMA_MaskSrcTran_CHANNEL7_WE         ((uint32_t)0x00008000)          /*!< Interrupt mask write enable of Channel7 */

#define DMA_MaskDstTran_CHANNEL0            ((uint32_t)0x00000001)          /*!< Interrupt mask of Channel0 */
#define DMA_MaskDstTran_CHANNEL1            ((uint32_t)0x00000002)          /*!< Interrupt mask of Channel1 */
#define DMA_MaskDstTran_CHANNEL2            ((uint32_t)0x00000004)          /*!< Interrupt mask of Channel2 */
#define DMA_MaskDstTran_CHANNEL3            ((uint32_t)0x00000008)          /*!< Interrupt mask of Channel3 */
#define DMA_MaskDstTran_CHANNEL4            ((uint32_t)0x00000010)          /*!< Interrupt mask of Channel4 */
#define DMA_MaskDstTran_CHANNEL5            ((uint32_t)0x00000020)          /*!< Interrupt mask of Channel5 */
#define DMA_MaskDstTran_CHANNEL6            ((uint32_t)0x00000040)          /*!< Interrupt mask of Channel6 */
#define DMA_MaskDstTran_CHANNEL7            ((uint32_t)0x00000080)          /*!< Interrupt mask of Channel7 */
#define DMA_MaskDstTran_CHANNEL0_WE         ((uint32_t)0x00000100)          /*!< Interrupt mask write enable of Channel0 */
#define DMA_MaskDstTran_CHANNEL1_WE         ((uint32_t)0x00000200)          /*!< Interrupt mask write enable of Channel1 */
#define DMA_MaskDstTran_CHANNEL2_WE         ((uint32_t)0x00000400)          /*!< Interrupt mask write enable of Channel2 */
#define DMA_MaskDstTran_CHANNEL3_WE         ((uint32_t)0x00000800)          /*!< Interrupt mask write enable of Channel3 */
#define DMA_MaskDstTran_CHANNEL4_WE         ((uint32_t)0x00001000)          /*!< Interrupt mask write enable of Channel4 */
#define DMA_MaskDstTran_CHANNEL5_WE         ((uint32_t)0x00002000)          /*!< Interrupt mask write enable of Channel5 */
#define DMA_MaskDstTran_CHANNEL6_WE         ((uint32_t)0x00004000)          /*!< Interrupt mask write enable of Channel6 */
#define DMA_MaskDstTran_CHANNEL7_WE         ((uint32_t)0x00008000)          /*!< Interrupt mask write enable of Channel7 */

#define DMA_MaskErr_CHANNEL0                ((uint32_t)0x00000001)          /*!< Interrupt mask of Channel0 */
#define DMA_MaskErr_CHANNEL1                ((uint32_t)0x00000002)          /*!< Interrupt mask of Channel1 */
#define DMA_MaskErr_CHANNEL2                ((uint32_t)0x00000004)          /*!< Interrupt mask of Channel2 */
#define DMA_MaskErr_CHANNEL3                ((uint32_t)0x00000008)          /*!< Interrupt mask of Channel3 */
#define DMA_MaskErr_CHANNEL4                ((uint32_t)0x00000010)          /*!< Interrupt mask of Channel4 */
#define DMA_MaskErr_CHANNEL5                ((uint32_t)0x00000020)          /*!< Interrupt mask of Channel5 */
#define DMA_MaskErr_CHANNEL6                ((uint32_t)0x00000040)          /*!< Interrupt mask of Channel6 */
#define DMA_MaskErr_CHANNEL7                ((uint32_t)0x00000080)          /*!< Interrupt mask of Channel7 */
#define DMA_MaskErr_CHANNEL0_WE             ((uint32_t)0x00000100)          /*!< Interrupt mask write enable of Channel0 */
#define DMA_MaskErr_CHANNEL1_WE             ((uint32_t)0x00000200)          /*!< Interrupt mask write enable of Channel1 */
#define DMA_MaskErr_CHANNEL2_WE             ((uint32_t)0x00000400)          /*!< Interrupt mask write enable of Channel2 */
#define DMA_MaskErr_CHANNEL3_WE             ((uint32_t)0x00000800)          /*!< Interrupt mask write enable of Channel3 */
#define DMA_MaskErr_CHANNEL4_WE             ((uint32_t)0x00001000)          /*!< Interrupt mask write enable of Channel4 */
#define DMA_MaskErr_CHANNEL5_WE             ((uint32_t)0x00002000)          /*!< Interrupt mask write enable of Channel5 */
#define DMA_MaskErr_CHANNEL6_WE             ((uint32_t)0x00004000)          /*!< Interrupt mask write enable of Channel6 */
#define DMA_MaskErr_CHANNEL7_WE             ((uint32_t)0x00008000)          /*!< Interrupt mask write enable of Channel7 */

/*******************  Bit definition for Clear interrupt registers  ***********/
#define DMA_ClearTfr_CHANNEL0               ((uint32_t)0x00000001)          /*!< Interrupt clear of Channel0 */
#define DMA_ClearTfr_CHANNEL1               ((uint32_t)0x00000002)          /*!< Interrupt clear of Channel1 */
#define DMA_ClearTfr_CHANNEL2               ((uint32_t)0x00000004)          /*!< Interrupt clear of Channel2 */
#define DMA_ClearTfr_CHANNEL3               ((uint32_t)0x00000008)          /*!< Interrupt clear of Channel3 */
#define DMA_ClearTfr_CHANNEL4               ((uint32_t)0x00000010)          /*!< Interrupt clear of Channel4 */
#define DMA_ClearTfr_CHANNEL5               ((uint32_t)0x00000020)          /*!< Interrupt clear of Channel5 */
#define DMA_ClearTfr_CHANNEL6               ((uint32_t)0x00000040)          /*!< Interrupt clear of Channel6 */
#define DMA_ClearTfr_CHANNEL7               ((uint32_t)0x00000080)          /*!< Interrupt clear of Channel7 */

#define DMA_ClearBlock_CHANNEL0             ((uint32_t)0x00000001)          /*!< Interrupt clear of Channel0 */
#define DMA_ClearBlock_CHANNEL1             ((uint32_t)0x00000002)          /*!< Interrupt clear of Channel1 */
#define DMA_ClearBlock_CHANNEL2             ((uint32_t)0x00000004)          /*!< Interrupt clear of Channel2 */
#define DMA_ClearBlock_CHANNEL3             ((uint32_t)0x00000008)          /*!< Interrupt clear of Channel3 */
#define DMA_ClearBlock_CHANNEL4             ((uint32_t)0x00000010)          /*!< Interrupt clear of Channel4 */
#define DMA_ClearBlock_CHANNEL5             ((uint32_t)0x00000020)          /*!< Interrupt clear of Channel5 */
#define DMA_ClearBlock_CHANNEL6             ((uint32_t)0x00000040)          /*!< Interrupt clear of Channel6 */
#define DMA_ClearBlock_CHANNEL7             ((uint32_t)0x00000080)          /*!< Interrupt clear of Channel7 */

#define DMA_ClearSrcTran_CHANNEL0           ((uint32_t)0x00000001)          /*!< Interrupt clear of Channel0 */
#define DMA_ClearSrcTran_CHANNEL1           ((uint32_t)0x00000002)          /*!< Interrupt clear of Channel1 */
#define DMA_ClearSrcTran_CHANNEL2           ((uint32_t)0x00000004)          /*!< Interrupt clear of Channel2 */
#define DMA_ClearSrcTran_CHANNEL3           ((uint32_t)0x00000008)          /*!< Interrupt clear of Channel3 */
#define DMA_ClearSrcTran_CHANNEL4           ((uint32_t)0x00000010)          /*!< Interrupt clear of Channel4 */
#define DMA_ClearSrcTran_CHANNEL5           ((uint32_t)0x00000020)          /*!< Interrupt clear of Channel5 */
#define DMA_ClearSrcTran_CHANNEL6           ((uint32_t)0x00000040)          /*!< Interrupt clear of Channel6 */
#define DMA_ClearSrcTran_CHANNEL7           ((uint32_t)0x00000080)          /*!< Interrupt clear of Channel7 */

#define DMA_ClearDstTran_CHANNEL0           ((uint32_t)0x00000001)          /*!< Interrupt clear of Channel0 */
#define DMA_ClearDstTran_CHANNEL1           ((uint32_t)0x00000002)          /*!< Interrupt clear of Channel1 */
#define DMA_ClearDstTran_CHANNEL2           ((uint32_t)0x00000004)          /*!< Interrupt clear of Channel2 */
#define DMA_ClearDstTran_CHANNEL3           ((uint32_t)0x00000008)          /*!< Interrupt clear of Channel3 */
#define DMA_ClearDstTran_CHANNEL4           ((uint32_t)0x00000010)          /*!< Interrupt clear of Channel4 */
#define DMA_ClearDstTran_CHANNEL5           ((uint32_t)0x00000020)          /*!< Interrupt clear of Channel5 */
#define DMA_ClearDstTran_CHANNEL6           ((uint32_t)0x00000040)          /*!< Interrupt clear of Channel6 */
#define DMA_ClearDstTran_CHANNEL7           ((uint32_t)0x00000080)          /*!< Interrupt clear of Channel7 */

#define DMA_ClearErr_CHANNEL0               ((uint32_t)0x00000001)          /*!< Interrupt clear of Channel0 */
#define DMA_ClearErr_CHANNEL1               ((uint32_t)0x00000002)          /*!< Interrupt clear of Channel1 */
#define DMA_ClearErr_CHANNEL2               ((uint32_t)0x00000004)          /*!< Interrupt clear of Channel2 */
#define DMA_ClearErr_CHANNEL3               ((uint32_t)0x00000008)          /*!< Interrupt clear of Channel3 */
#define DMA_ClearErr_CHANNEL4               ((uint32_t)0x00000010)          /*!< Interrupt clear of Channel4 */
#define DMA_ClearErr_CHANNEL5               ((uint32_t)0x00000020)          /*!< Interrupt clear of Channel5 */
#define DMA_ClearErr_CHANNEL6               ((uint32_t)0x00000040)          /*!< Interrupt clear of Channel6 */
#define DMA_ClearErr_CHANNEL7               ((uint32_t)0x00000080)          /*!< Interrupt clear of Channel7 */

/*******************  Bit definition for StatusInt interrupt registers  *******/
#define DMA_StatusInt_TFR                   ((uint32_t)0x00000001)          /*!< OR of the contents of StatusTfr register */
#define DMA_StatusInt_BLOCK                 ((uint32_t)0x00000002)          /*!< OR of the contents of StatusBlock register */
#define DMA_StatusInt_SRCT                  ((uint32_t)0x00000004)          /*!< OR of the contents of StatusSrcTran register */
#define DMA_StatusInt_DSTT                  ((uint32_t)0x00000008)          /*!< OR of the contents of StatusDst register */
#define DMA_StatusInt_ERR                   ((uint32_t)0x00000010)          /*!< OR of the contents of StatusErr register */

/*******************  Bit definition for ReqSrcReg interrupt registers  *******/
#define DMA_ReqSrcReg_CHANNEL0              ((uint32_t)0x00000001)          /*!< Channel0 source request */
#define DMA_ReqSrcReg_CHANNEL1              ((uint32_t)0x00000002)          /*!< Channel1 source request */
#define DMA_ReqSrcReg_CHANNEL2              ((uint32_t)0x00000004)          /*!< Channel2 source request */
#define DMA_ReqSrcReg_CHANNEL3              ((uint32_t)0x00000008)          /*!< Channel3 source request */
#define DMA_ReqSrcReg_CHANNEL4              ((uint32_t)0x00000010)          /*!< Channel4 source request */
#define DMA_ReqSrcReg_CHANNEL5              ((uint32_t)0x00000020)          /*!< Channel5 source request */
#define DMA_ReqSrcReg_CHANNEL6              ((uint32_t)0x00000040)          /*!< Channel6 source request */
#define DMA_ReqSrcReg_CHANNEL7              ((uint32_t)0x00000080)          /*!< Channel7 source request */
#define DMA_ReqSrcReg_CHANNEL0_WE           ((uint32_t)0x00000100)          /*!< Channel0 source request write enable */
#define DMA_ReqSrcReg_CHANNEL1_WE           ((uint32_t)0x00000200)          /*!< Channel1 source request write enable */
#define DMA_ReqSrcReg_CHANNEL2_WE           ((uint32_t)0x00000400)          /*!< Channel2 source request write enable */
#define DMA_ReqSrcReg_CHANNEL3_WE           ((uint32_t)0x00000800)          /*!< Channel3 source request write enable */
#define DMA_ReqSrcReg_CHANNEL4_WE           ((uint32_t)0x00001000)          /*!< Channel4 source request write enable */
#define DMA_ReqSrcReg_CHANNEL5_WE           ((uint32_t)0x00002000)          /*!< Channel5 source request write enable */
#define DMA_ReqSrcReg_CHANNEL6_WE           ((uint32_t)0x00004000)          /*!< Channel6 source request write enable */
#define DMA_ReqSrcReg_CHANNEL7_WE           ((uint32_t)0x00008000)          /*!< Channel7 source request write enable */

/*******************  Bit definition for ReqDstReg interrupt registers  *******/
#define DMA_ReqDstReg_CHANNEL0              ((uint32_t)0x00000001)          /*!< Channel0 destination request */
#define DMA_ReqDstReg_CHANNEL1              ((uint32_t)0x00000002)          /*!< Channel1 destination request */
#define DMA_ReqDstReg_CHANNEL2              ((uint32_t)0x00000004)          /*!< Channel2 destination request */
#define DMA_ReqDstReg_CHANNEL3              ((uint32_t)0x00000008)          /*!< Channel3 destination request */
#define DMA_ReqDstReg_CHANNEL4              ((uint32_t)0x00000010)          /*!< Channel4 destination request */
#define DMA_ReqDstReg_CHANNEL5              ((uint32_t)0x00000020)          /*!< Channel5 destination request */
#define DMA_ReqDstReg_CHANNEL6              ((uint32_t)0x00000040)          /*!< Channel6 destination request */
#define DMA_ReqDstReg_CHANNEL7              ((uint32_t)0x00000080)          /*!< Channel7 destination request */
#define DMA_ReqDstReg_CHANNEL0_WE           ((uint32_t)0x00000100)          /*!< Channel0 destination request write enable */
#define DMA_ReqDstReg_CHANNEL1_WE           ((uint32_t)0x00000200)          /*!< Channel1 destination request write enable */
#define DMA_ReqDstReg_CHANNEL2_WE           ((uint32_t)0x00000400)          /*!< Channel2 destination request write enable */
#define DMA_ReqDstReg_CHANNEL3_WE           ((uint32_t)0x00000800)          /*!< Channel3 destination request write enable */
#define DMA_ReqDstReg_CHANNEL4_WE           ((uint32_t)0x00001000)          /*!< Channel4 destination request write enable */
#define DMA_ReqDstReg_CHANNEL5_WE           ((uint32_t)0x00002000)          /*!< Channel5 destination request write enable */
#define DMA_ReqDstReg_CHANNEL6_WE           ((uint32_t)0x00004000)          /*!< Channel6 destination request write enable */
#define DMA_ReqDstReg_CHANNEL7_WE           ((uint32_t)0x00008000)          /*!< Channel7 destination request write enable */

/*******************  Bit definition for SglReqSrcReg interrupt registers  ****/
#define DMA_SglReqSrcReg_CHANNEL0           ((uint32_t)0x00000001)          /*!< Channel0 source single request */
#define DMA_SglReqSrcReg_CHANNEL1           ((uint32_t)0x00000002)          /*!< Channel1 source single request */
#define DMA_SglReqSrcReg_CHANNEL2           ((uint32_t)0x00000004)          /*!< Channel2 source single request */
#define DMA_SglReqSrcReg_CHANNEL3           ((uint32_t)0x00000008)          /*!< Channel3 source single request */
#define DMA_SglReqSrcReg_CHANNEL4           ((uint32_t)0x00000010)          /*!< Channel4 source single request */
#define DMA_SglReqSrcReg_CHANNEL5           ((uint32_t)0x00000020)          /*!< Channel5 source single request */
#define DMA_SglReqSrcReg_CHANNEL6           ((uint32_t)0x00000040)          /*!< Channel6 source single request */
#define DMA_SglReqSrcReg_CHANNEL7           ((uint32_t)0x00000080)          /*!< Channel7 source single request */
#define DMA_SglReqSrcReg_CHANNEL0_WE        ((uint32_t)0x00000100)          /*!< Channel0 source single request write enable */
#define DMA_SglReqSrcReg_CHANNEL1_WE        ((uint32_t)0x00000200)          /*!< Channel1 source single request write enable */
#define DMA_SglReqSrcReg_CHANNEL2_WE        ((uint32_t)0x00000400)          /*!< Channel2 source single request write enable */
#define DMA_SglReqSrcReg_CHANNEL3_WE        ((uint32_t)0x00000800)          /*!< Channel3 source single request write enable */
#define DMA_SglReqSrcReg_CHANNEL4_WE        ((uint32_t)0x00001000)          /*!< Channel4 source single request write enable */
#define DMA_SglReqSrcReg_CHANNEL5_WE        ((uint32_t)0x00002000)          /*!< Channel5 source single request write enable */
#define DMA_SglReqSrcReg_CHANNEL6_WE        ((uint32_t)0x00004000)          /*!< Channel6 source single request write enable */
#define DMA_SglReqSrcReg_CHANNEL7_WE        ((uint32_t)0x00008000)          /*!< Channel7 source single request write enable */

/*******************  Bit definition for SglReqDstReg interrupt registers  ****/
#define DMA_SglReqDstReg_CHANNEL0           ((uint32_t)0x00000001)          /*!< Channel0 destination single request */
#define DMA_SglReqDstReg_CHANNEL1           ((uint32_t)0x00000002)          /*!< Channel1 destination single request */
#define DMA_SglReqDstReg_CHANNEL2           ((uint32_t)0x00000004)          /*!< Channel2 destination single request */
#define DMA_SglReqDstReg_CHANNEL3           ((uint32_t)0x00000008)          /*!< Channel3 destination single request */
#define DMA_SglReqDstReg_CHANNEL4           ((uint32_t)0x00000010)          /*!< Channel4 destination single request */
#define DMA_SglReqDstReg_CHANNEL5           ((uint32_t)0x00000020)          /*!< Channel5 destination single request */
#define DMA_SglReqDstReg_CHANNEL6           ((uint32_t)0x00000040)          /*!< Channel6 destination single request */
#define DMA_SglReqDstReg_CHANNEL7           ((uint32_t)0x00000080)          /*!< Channel7 destination single request */
#define DMA_SglReqDstReg_CHANNEL0_WE        ((uint32_t)0x00000100)          /*!< Channel0 destination single request write enable */
#define DMA_SglReqDstReg_CHANNEL1_WE        ((uint32_t)0x00000200)          /*!< Channel1 destination single request write enable */
#define DMA_SglReqDstReg_CHANNEL2_WE        ((uint32_t)0x00000400)          /*!< Channel2 destination single request write enable */
#define DMA_SglReqDstReg_CHANNEL3_WE        ((uint32_t)0x00000800)          /*!< Channel3 destination single request write enable */
#define DMA_SglReqDstReg_CHANNEL4_WE        ((uint32_t)0x00001000)          /*!< Channel4 destination single request write enable */
#define DMA_SglReqDstReg_CHANNEL5_WE        ((uint32_t)0x00002000)          /*!< Channel5 destination single request write enable */
#define DMA_SglReqDstReg_CHANNEL6_WE        ((uint32_t)0x00004000)          /*!< Channel6 destination single request write enable */
#define DMA_SglReqDstReg_CHANNEL7_WE        ((uint32_t)0x00008000)          /*!< Channel7 destination single request write enable */

/*******************  Bit definition for LstSrcReg interrupt registers  *******/
#define DMA_LstSrcReg_CHANNEL0              ((uint32_t)0x00000001)          /*!< Channel0 source last transaction request */
#define DMA_LstSrcReg_CHANNEL1              ((uint32_t)0x00000002)          /*!< Channel1 source last transaction request */
#define DMA_LstSrcReg_CHANNEL2              ((uint32_t)0x00000004)          /*!< Channel2 source last transaction request */
#define DMA_LstSrcReg_CHANNEL3              ((uint32_t)0x00000008)          /*!< Channel3 source last transaction request */
#define DMA_LstSrcReg_CHANNEL4              ((uint32_t)0x00000010)          /*!< Channel4 source last transaction request */
#define DMA_LstSrcReg_CHANNEL5              ((uint32_t)0x00000020)          /*!< Channel5 source last transaction request */
#define DMA_LstSrcReg_CHANNEL6              ((uint32_t)0x00000040)          /*!< Channel6 source last transaction request */
#define DMA_LstSrcReg_CHANNEL7              ((uint32_t)0x00000080)          /*!< Channel7 source last transaction request */
#define DMA_LstSrcReg_CHANNEL0_WE           ((uint32_t)0x00000100)          /*!< Channel0 source last transaction request write enable */
#define DMA_LstSrcReg_CHANNEL1_WE           ((uint32_t)0x00000200)          /*!< Channel1 source last transaction request write enable */
#define DMA_LstSrcReg_CHANNEL2_WE           ((uint32_t)0x00000400)          /*!< Channel2 source last transaction request write enable */
#define DMA_LstSrcReg_CHANNEL3_WE           ((uint32_t)0x00000800)          /*!< Channel3 source last transaction request write enable */
#define DMA_LstSrcReg_CHANNEL4_WE           ((uint32_t)0x00001000)          /*!< Channel4 source last transaction request write enable */
#define DMA_LstSrcReg_CHANNEL5_WE           ((uint32_t)0x00002000)          /*!< Channel5 source last transaction request write enable */
#define DMA_LstSrcReg_CHANNEL6_WE           ((uint32_t)0x00004000)          /*!< Channel6 source last transaction request write enable */
#define DMA_LstSrcReg_CHANNEL7_WE           ((uint32_t)0x00008000)          /*!< Channel7 source last transaction request write enable */

/*******************  Bit definition for LstDstReg interrupt registers  *******/
#define DMA_LstDstReg_CHANNEL0              ((uint32_t)0x00000001)          /*!< Channel0 destination last transaction request */
#define DMA_LstDstReg_CHANNEL1              ((uint32_t)0x00000002)          /*!< Channel1 destination last transaction request */
#define DMA_LstDstReg_CHANNEL2              ((uint32_t)0x00000004)          /*!< Channel2 destination last transaction request */
#define DMA_LstDstReg_CHANNEL3              ((uint32_t)0x00000008)          /*!< Channel3 destination last transaction request */
#define DMA_LstDstReg_CHANNEL4              ((uint32_t)0x00000010)          /*!< Channel4 destination last transaction request */
#define DMA_LstDstReg_CHANNEL5              ((uint32_t)0x00000020)          /*!< Channel5 destination last transaction request */
#define DMA_LstDstReg_CHANNEL6              ((uint32_t)0x00000040)          /*!< Channel6 destination last transaction request */
#define DMA_LstDstReg_CHANNEL7              ((uint32_t)0x00000080)          /*!< Channel7 destination last transaction request */
#define DMA_LstDstReg_CHANNEL0_WE           ((uint32_t)0x00000100)          /*!< Channel0 destination last transaction request write enable */
#define DMA_LstDstReg_CHANNEL1_WE           ((uint32_t)0x00000200)          /*!< Channel1 destination last transaction request write enable */
#define DMA_LstDstReg_CHANNEL2_WE           ((uint32_t)0x00000400)          /*!< Channel2 destination last transaction request write enable */
#define DMA_LstDstReg_CHANNEL3_WE           ((uint32_t)0x00000800)          /*!< Channel3 destination last transaction request write enable */
#define DMA_LstDstReg_CHANNEL4_WE           ((uint32_t)0x00001000)          /*!< Channel4 destination last transaction request write enable */
#define DMA_LstDstReg_CHANNEL5_WE           ((uint32_t)0x00002000)          /*!< Channel5 destination last transaction request write enable */
#define DMA_LstDstReg_CHANNEL6_WE           ((uint32_t)0x00004000)          /*!< Channel6 destination last transaction request write enable */
#define DMA_LstDstReg_CHANNEL7_WE           ((uint32_t)0x00008000)          /*!< Channel7 destination last transaction request write enable */

/*******************  Bit definition for DmaCfgReg interrupt registers  *******/
#define DMA_DmaCfgReg_EN                    ((uint32_t)0x00000001)          /*!< DMA enable bit */

/*******************  Bit definition for ChEnReg interrupt registers  *********/
#define DMA_ChEnReg_CHANNEL0_EN             ((uint32_t)0x00000001)          /*!< Channel0 enable bit */
#define DMA_ChEnReg_CHANNEL1_EN             ((uint32_t)0x00000002)          /*!< Channel1 enable bit */
#define DMA_ChEnReg_CHANNEL2_EN             ((uint32_t)0x00000004)          /*!< Channel2 enable bit */
#define DMA_ChEnReg_CHANNEL3_EN             ((uint32_t)0x00000008)          /*!< Channel3 enable bit */
#define DMA_ChEnReg_CHANNEL4_EN             ((uint32_t)0x00000010)          /*!< Channel4 enable bit */
#define DMA_ChEnReg_CHANNEL5_EN             ((uint32_t)0x00000020)          /*!< Channel5 enable bit */
#define DMA_ChEnReg_CHANNEL6_EN             ((uint32_t)0x00000040)          /*!< Channel6 enable bit */
#define DMA_ChEnReg_CHANNEL7_EN             ((uint32_t)0x00000080)          /*!< Channel7 enable bit */
#define DMA_ChEnReg_CHANNEL0_EN_WE          ((uint32_t)0x00000100)          /*!< Channel0 enable write enable bit */
#define DMA_ChEnReg_CHANNEL1_EN_WE          ((uint32_t)0x00000200)          /*!< Channel1 enable write enable bit */
#define DMA_ChEnReg_CHANNEL2_EN_WE          ((uint32_t)0x00000400)          /*!< Channel2 enable write enable bit */
#define DMA_ChEnReg_CHANNEL3_EN_WE          ((uint32_t)0x00000800)          /*!< Channel3 enable write enable bit */
#define DMA_ChEnReg_CHANNEL4_EN_WE          ((uint32_t)0x00001000)          /*!< Channel4 enable write enable bit */
#define DMA_ChEnReg_CHANNEL5_EN_WE          ((uint32_t)0x00002000)          /*!< Channel5 enable write enable bit */
#define DMA_ChEnReg_CHANNEL6_EN_WE          ((uint32_t)0x00004000)          /*!< Channel6 enable write enable bit */
#define DMA_ChEnReg_CHANNEL7_EN_WE          ((uint32_t)0x00008000)          /*!< Channel7 enable write enable bit */

/******************************************************************************/
/*                                                                            */
/*                                     DAC                                    */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for DAC_CTRL register  *******************/
#define DAC_CTRL_EN                         ((uint32_t)0x00000001)          /*!< Enable DAC */
#define DAC_CTRL_IE                         ((uint32_t)0x00000002)          /*!< Enable DAC Interrupt */
#define DAC_CTRL_LSEL_MASK                  ((uint32_t)0x00000070)          /*!< Mask of DAC LSEL */
#define DAC_CTRL_PWONSTBCNT_MASK            ((uint32_t)0x003FFF00)          /*!< Mask of DAC PWONSTBCNT */

/*******************  Bit definition for DAC_DATA register  *******************/
#define DAC_DATA_MASK                       ((uint32_t)0x00000FFF)          /*!< Mask of data */

/*******************  Bit definition for DAC_STATUS register  ******************/
#define DAC_STATUS_IFG                      ((uint32_t)0x00000001)          /*!< DAC interrupt flag */
#define DAC_STATUS_BUSY                     ((uint32_t)0x00000004)          /*!< DAC busy flag */

/*******************  Bit definition for DAC_ANACTRL register  *****************/
#define DAC_ANACTRL_RMCFILT                 ((uint32_t)0x00000010)          /*!< Bypass filter resistance */
#define DAC_ANACTRL_SHORTOPA                ((uint32_t)0x00000020)          /*!< Bypass OPA */
#define DAC_ANACTRL_SHORTRFILT              ((uint32_t)0x00000040)          /*!< Bypass filter capacitance */
#define DAC_ANACTRL_VREFSEL_MASK            ((uint32_t)0x00000300)          /*!< Mask of reference voltage */

/*******************  Bit definition for DAC_COMCTRL register  *****************/
#define DAC_COMCTRL_WAITDACCONV_MASK        ((uint32_t)0x000000FF)          /*!< Mask of latency time */
#define DAC_COMCTRL_GRP                     ((uint32_t)0x00000100)          /*!< DAC group */

/******************************************************************************/
/*                                                                            */
/*                                     RTC                                    */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for RTC_CTRL register  *******************/
#define RTC_CTRL_EN                         ((uint32_t)0x00000001)          /*!< Enable RTC */
#define RTC_CTRL_1HZ_INT                    ((uint32_t)0x00000002)          /*!< Enable 1HZ interrupt */
#define RTC_CTRL_ALARM_INT                  ((uint32_t)0x00000004)          /*!< Enable Alarm interrupt */
#define RTC_CTRL_CLK_SEL_MASK               ((uint32_t)0x00000030)          /*!< Mask of clock source */
#define RTC_CTRL_CLK_SEL_10K                ((uint32_t)0x00000010)          /*!< 10K */
#define RTC_CTRL_CLK_SEL_32K                ((uint32_t)0x00000000)          /*!< 32K */
#define RTC_CTRL_CLK_SEL_IRC12M             ((uint32_t)0x00000020)          /*!< IRC 12M */
#define RTC_CTRL_CLK_SEL_OSC12M             ((uint32_t)0x00000030)          /*!< OSC 12M */
#define RTC_CTRL_PWD_32K                    ((uint32_t)0x80000000)          /*!< Power down 32K clock */

/*******************  Bit definition for RTC_LOAD register  *******************/
#define RTC_LOAD_EN                         ((uint32_t)0x00000001)          /*!< Load RTC configuration */

/*******************  Bit definition for RTC_CNT register  ********************/
#define RTC_CNT_MASK                        ((uint32_t)0xFFFFFFFF)          /*!< RTC count */

/*******************  Bit definition for RTC_ALARM register  *******************/
#define RTC_ALARM_MASK                      ((uint32_t)0x00000001)          /*!< Alarm value */

/*******************  Bit definition for RTC_STAT register  *******************/
#define RTC_STAT_1HZ_INT                    ((uint32_t)0x00000001)          /*!< RTC 1HZ interrupt flag */
#define RTC_STAT_ALARM_INT                  ((uint32_t)0x00000002)          /*!< RTC alarm interrupt flag */

/******************************************************************************/
/*                                                                            */
/*                           Independent WATCHDOG                             */
/*                                                                            */
/******************************************************************************/

/*******************  Bit definition for IWDG_KR register  ********************/
#define IWDG_KR_KEY                         ((uint16_t)0xFFFF)              /*!< Key value (write only, read 0000h) */

/*******************  Bit definition for IWDG_PR register  ********************/
#define IWDG_PR_PR                          ((uint8_t)0x07)                 /*!< PR[2:0] (Prescaler divider) */
#define IWDG_PR_PR_0                        ((uint8_t)0x01)                 /*!< Bit 0 */
#define IWDG_PR_PR_1                        ((uint8_t)0x02)                 /*!< Bit 1 */
#define IWDG_PR_PR_2                        ((uint8_t)0x04)                 /*!< Bit 2 */

/*******************  Bit definition for IWDG_RLR register  *******************/
#define IWDG_RLR_RL                         ((uint16_t)0x0FFF)              /*!< Watchdog counter reload value */

/*******************  Bit definition for IWDG_SR register  ********************/
#define IWDG_SR_PVU                         ((uint8_t)0x01)                 /*!< Watchdog prescaler value update */
#define IWDG_SR_RVU                         ((uint8_t)0x02)                 /*!< Watchdog counter reload value update */

/******************************************************************************/
/*                                                                            */
/*                      Touch Senser Controller (TSC)                         */
/*                                                                            */
/******************************************************************************/

/*******************  Bit definition for TSC_CR register  *********************/
#define TSC_CR_TSCE                         ((uint32_t)0x00000001)          /*!< Enable TSC */
#define TSC_CR_START                        ((uint32_t)0x00000002)          /*!< Start TSC sample */
#define TSC_CR_AM                           ((uint32_t)0x00000004)          /*!< Sample mode */
#define TSC_CR_SYNCPOL                      ((uint32_t)0x00000008)          /*!< Sync polarity */
#define TSC_CR_IODEF                        ((uint32_t)0x00000010)          /*!< IO default status */
#define TSC_CR_MCV_MASK                     ((uint32_t)0x000000E0)          /*!< Mask of max error count */
#define TSC_CR_PGPSC_MASK                   ((uint32_t)0x00007000)          /*!< Mask of prescaler */
#define TSC_CR_CTPL_MASK                    ((uint32_t)0x0F000000)          /*!< Mask of CTPL */
#define TSC_CR_CTPH_MASK                    ((uint32_t)0xF0000000)          /*!< Mask of CTPH */

/*******************  Bit definition for TSC_IER register  ********************/
#define TSC_IER_MCEIE                       ((uint32_t)0x00000001)          /*!< Enable interrupt for max error count interrupt */
#define TSC_IER_EOAIE                       ((uint32_t)0x00000002)          /*!< Enable interrupt for conversion complete */

/*******************  Bit definition for TSC_ICR register  ********************/
#define TSC_ICR_MCEIC                       ((uint32_t)0x00000001)          /*!< Clear max error count interrupt flag */
#define TSC_ICR_EOAIC                       ((uint32_t)0x00000002)          /*!< Clear conversion complete flag */

/*******************  Bit definition for TSC_ICR register  ********************/
#define TSC_ISR_MCEF                        ((uint32_t)0x00000001)          /*!< Max error count interrupt flag */
#define TSC_ISR_EOAF                        ((uint32_t)0x00000002)          /*!< Conversion complete flag */

/******************************************************************************/
/*                                                                            */
/*                         Window WATCHDOG (WWDG)                             */
/*                                                                            */
/******************************************************************************/

/*******************  Bit definition for WWDG_CR register  ********************/
#define WWDG_CR_T                           ((uint8_t)0x7F)                 /*!< T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define WWDG_CR_T0                          ((uint8_t)0x01)                 /*!< Bit 0 */
#define WWDG_CR_T1                          ((uint8_t)0x02)                 /*!< Bit 1 */
#define WWDG_CR_T2                          ((uint8_t)0x04)                 /*!< Bit 2 */
#define WWDG_CR_T3                          ((uint8_t)0x08)                 /*!< Bit 3 */
#define WWDG_CR_T4                          ((uint8_t)0x10)                 /*!< Bit 4 */
#define WWDG_CR_T5                          ((uint8_t)0x20)                 /*!< Bit 5 */
#define WWDG_CR_T6                          ((uint8_t)0x40)                 /*!< Bit 6 */

#define WWDG_CR_WDGA                        ((uint8_t)0x80)                 /*!< Activation bit */

/*******************  Bit definition for WWDG_CFR register  *******************/
#define WWDG_CFR_W                          ((uint16_t)0x007F)              /*!< W[6:0] bits (7-bit window value) */
#define WWDG_CFR_W0                         ((uint16_t)0x0001)              /*!< Bit 0 */
#define WWDG_CFR_W1                         ((uint16_t)0x0002)              /*!< Bit 1 */
#define WWDG_CFR_W2                         ((uint16_t)0x0004)              /*!< Bit 2 */
#define WWDG_CFR_W3                         ((uint16_t)0x0008)              /*!< Bit 3 */
#define WWDG_CFR_W4                         ((uint16_t)0x0010)              /*!< Bit 4 */
#define WWDG_CFR_W5                         ((uint16_t)0x0020)              /*!< Bit 5 */
#define WWDG_CFR_W6                         ((uint16_t)0x0040)              /*!< Bit 6 */

#define WWDG_CFR_WDGTB                      ((uint16_t)0x0180)              /*!< WDGTB[1:0] bits (Timer Base) */
#define WWDG_CFR_WDGTB0                     ((uint16_t)0x0080)              /*!< Bit 0 */
#define WWDG_CFR_WDGTB1                     ((uint16_t)0x0100)              /*!< Bit 1 */

#define WWDG_CFR_EWI                        ((uint16_t)0x0200)              /*!< Early Wakeup Interrupt */

/*******************  Bit definition for WWDG_SR register  ********************/
#define WWDG_SR_EWIF                        ((uint8_t)0x01)                 /*!< Early Wakeup Interrupt Flag */

/******************************************************************************/
/*                                                                            */
/*                                 Cordic                                     */
/*                                                                            */
/******************************************************************************/

/*******************  Bit definition for CORDIC_CTRL register  ****************/
#define MATHS_CORDIC_CTRL_EN                ((uint32_t)0x00000001)          /*!< Cordic enable bit */
#define MATHS_CORDIC_CTRL_MODE              ((uint32_t)0x00000010)          /*!< Cordic mode */
#define MATHS_CORDIC_CTRL_XYMRS_MASK        ((uint32_t)0x00000300)          /*!< Mask of divisor */
#define MATHS_CORDIC_CTRL_XYMRS_DIV1        ((uint32_t)0x00000300)          /*!< X and Y divided by 1 */
#define MATHS_CORDIC_CTRL_XYMRS_DIV2        ((uint32_t)0x00000300)          /*!< X and Y divided by 2 */
#define MATHS_CORDIC_CTRL_XYMRS_DIV4        ((uint32_t)0x00000300)          /*!< X and Y divided by 4 */
#define MATHS_CORDIC_CTRL_BYPASS            ((uint32_t)0x00001000)          /*!< Complement */
#define MATHS_CORDIC_CTRL_DIV_K             ((uint32_t)0x00010000)          /*!< X/Y divided by K */

/*******************  Bit definition for CORDIC_STATUS register  ****************/
#define MATHS_CORDIC_STATUS_READY           ((uint32_t)0x00000001)          /*!< Cordic ready bit */
#define MATHS_CORDIC_STATUS_OVF             ((uint32_t)0x00000002)          /*!< Cordic overflow bit */

/*******************  Bit definition for CORDIC_DATA_INX register  ****************/
#define MATHS_CORDIC_DATA_INX_MASK          ((uint32_t)0x00FFFFFF)          /*!< Cordic IN X */

/*******************  Bit definition for CORDIC_DATA_INY register  ****************/
#define MATHS_CORDIC_DATA_INY_MASK          ((uint32_t)0x00FFFFFF)          /*!< Cordic IN Y */

/*******************  Bit definition for CORDIC_DATA_INZ register  ****************/
#define MATHS_CORDIC_DATA_INZ_MASK          ((uint32_t)0x00FFFFFF)          /*!< Cordic IN Z */

/*******************  Bit definition for CORDIC_DATA_OUTX register  ****************/
#define MATHS_CORDIC_DATA_OUTX_MASK         ((uint32_t)0x00FFFFFF)          /*!< Cordic OUT X */

/*******************  Bit definition for CORDIC_DATA_OUTY register  ****************/
#define MATHS_CORDIC_DATA_OUTY_MASK         ((uint32_t)0x00FFFFFF)          /*!< Cordic OUT Y */

/*******************  Bit definition for CORDIC_DATA_OUTZ register  ****************/
#define MATHS_CORDIC_DATA_OUTZ_MASK         ((uint32_t)0x00FFFFFF)          /*!< Cordic OUT Z */

/*******************  Bit definition for CRC_DR register  ****************/
#define MATHS_CRC_DR                        ((uint32_t)0xFFFFFFFF)          /*!< CRC data */

/*******************  Bit definition for CRC_DR register  ****************/
#define MATHS_CRC_CR_RESET                  ((uint32_t)0x00000001)          /*!< CRC reset bit */

/*******************  Bit definition for DIV_CHUSHU register  ****************/
#define MATHS_DIV_CHUSHU                    ((uint32_t)0xFFFFFFFF)          /*!< Divisor */

/*******************  Bit definition for DIV_BEICHUSHU register  ****************/
#define MATHS_DIV_BEICHUSHU                 ((uint32_t)0xFFFFFFFF)          /*!< Dividend */

/*******************  Bit definition for DIV_CTRL register  ****************/
#define MATHS_DIV_CTRL_START                ((uint32_t)0x00000001)          /*!< Start division */
#define MATHS_DIV_CTRL_STMODE               ((uint32_t)0x00000002)          /*!< Division start mode */
#define MATHS_DIV_CTRL_USIGN                ((uint32_t)0x00000004)          /*!< Signed or Unsigned */
#define MATHS_DIV_CTRL_SHANGDIR             ((uint32_t)0x00000008)          /*!< Quotient left shift*/
#define MATHS_DIV_CTRL_SHANGCNT_MASK        ((uint32_t)0x00001F00)          /*!< Quotient Shift amount */
#define MATHS_DIV_CTRL_BEICHUSHU_SLC_MASK   ((uint32_t)0x001F0000)          /*!< Dividend Shift amount */
#define MATHS_DIV_CTRL_CHUSHU_SRC_MASK      ((uint32_t)0x1F000000)          /*!< Divisor Shift amount */

/*******************  Bit definition for DIV_IRQ register  ****************/
#define MATHS_DIV_IRQ_DONE                  ((uint32_t)0x00000001)          /*!< Division done interrupt bit */
#define MATHS_DIV_IRQ_ZEROERR               ((uint32_t)0x00000002)          /*!< Division by zero interrupt bit */
#define MATHS_DIV_IRQ_DONE_EN               ((uint32_t)0x00010000)          /*!< Enable division done interrupt*/
#define MATHS_DIV_IRQ_ZEROERR_EN            ((uint32_t)0x00020000)          /*!< Enable division by zero interrupt */

/*******************  Bit definition for DIV_SHANG register  ****************/
#define MATHS_DIV_SHANG                     ((uint32_t)0xFFFFFFFF)          /*!< Division quotient */

/*******************  Bit definition for DIV_YUSHU register  ****************/
#define MATHS_DIV_YUSHU                     ((uint32_t)0xFFFFFFFF)          /*!< Division remainder */

/*******************  Bit definition for DIV_STATUS register  ****************/
#define MATHS_DIV_STATUS_BUSY               ((uint32_t)0x00000001)          /*!< Division busy bit */

/**
  * @}
  */

 /**
  * @}
  */ 

/** @addtogroup Exported_macro
  * @{
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __MT071X_H */

/**
  * @}
  */

  /**
  * @}
  */

/************************ (C) COPYRIGHT MIC *****END OF FILE****/
