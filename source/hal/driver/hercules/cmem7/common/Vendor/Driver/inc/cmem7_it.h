/**
	*****************************************************************************
	* @file     cmem7_it.h
	*
	* @brief    CMEM7 system exception interrupt header file
	*
	*
	* @version  V1.0
	* @date     3. September 2013
	*
	* @note     Actually, you don't have to implement below involved function 
	*           whick were defined as weak dummy functions in startup file.
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

#ifndef __CMEM7_IT_H
#define __CMEM7_IT_H

#include "cmem7.h"

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void ETH_IRQHandler(void);
void USB_IRQHandler(void);
void DMAC_IRQHandler(void);          
void CAN0_IRQHandler(void);             
void CAN1_IRQHandler(void);          
void FP0_IRQHandler(void);         
void FP1_IRQHandler(void);          
void FP2_IRQHandler(void);          		
void FP3_IRQHandler(void);            
void FP4_IRQHandler(void);            
void FP5_IRQHandler(void);            
void FP6_IRQHandler(void);            
void FP7_IRQHandler(void);             
void FP8_IRQHandler(void);
void FP9_IRQHandler(void);
void FP10_IRQHandler(void);
void FP11_IRQHandler(void);
void FP12_IRQHandler(void);
void FP13_IRQHandler(void);          
void FP14_IRQHandler(void);             
void FP15_IRQHandler(void);          
void USART0_IRQHandler(void);         
void USART1_IRQHandler(void);          
void GPIO_IRQHandler(void);          		
void SPI1_IRQHandler(void);            
void I2C1_IRQHandler(void);            
void SPI0_IRQHandler(void);            
void I2C0_IRQHandler(void);            
void RTC_1S_IRQHandler(void);             
void RTC_1MS_IRQHandler(void);
void WDG_IRQHandler(void);         
void TIMER_IRQHandler(void);          
void DDRC_SW_PROC_IRQHandler(void);          		
void ETH_PMT_IRQHandler(void);            
void PAD_IRQHandler(void);            
void DDRC_LANE_SYNC_IRQHandler(void);            
void USART2_IRQHandler(void);            

#endif /* __CMEM7_IT_H */

