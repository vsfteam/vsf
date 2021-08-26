/**
	*****************************************************************************
	* @file     cmem7_conf.h
	*
	* @brief    CMEM7 config file
	*
	*
	* @version  V1.0
	* @date     3. September 2013
	*
	* @note               
	*           
	*****************************************************************************
	* @attention
	*
	* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
	* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
	* TIME. AS A RESULT, CAPITAL-MICRO SHALL NOT BE HELD LIABLE FOR ANY DIRECT, 
	* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
	* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
	* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
	*
	* <h2><center>&copy; COPYRIGHT 2013 Capital-micro </center></h2>
	*****************************************************************************
	*/

#ifndef __CMEM7_CONF_H
#define __CMEM7_CONF_H

//#include <string.h>	// memset

#define _ADC
#define _AES
#define _CAN
#define _DDR
#define _DMA
#define _EFUSE
#define _ETH
#define _FLASH
#define _GPIO
#define _I2C
#define _MISC
#define _RTC
#define _SPI
#define _TIM
#define _UART
//#define _USB disable becase of redefinition
#define _WDG

#define USE_FULL_ASSERT    1

#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports
  *         the name of the source file and the source line number of the call
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((unsigned char *)__FILE__, __LINE__))

	static void assert_failed(unsigned char* file, unsigned long line) {
		while (1) {
			;
		}
	}
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#undef TRUE
#undef FALSE
typedef enum _BOOL {FALSE = 0, TRUE = 1} BOOL;

/**
  * System clock frequency, unit is Hz.
  */
#define SYSTEM_CLOCK_FREQ               266666667
#define SYSTEM_CLOCK_FREQ_LP            20000000

/**
  * @brief  usecond delay 
	* @note 	It can't delay in an accurate time
	* @param[in] usec usecond to be delay
	* @retval None
	*/
static void udelay(unsigned long usec) {
  volatile unsigned long count = 0;
  volatile unsigned long utime = SYSTEM_CLOCK_FREQ / 1000000 * usec;

  while(++count < utime) __NOP();
}

/**
  * UART definition for print
	*/
#ifndef PRINT_UART
#   define PRINT_UART							UART1 //CME_M7_256EVB_V10 BGA256
#endif

#endif /* __CMEM7_CONF_H */

