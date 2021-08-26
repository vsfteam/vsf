/**
	*****************************************************************************
	* @file     cmem7_ddr_tuning.h
	*
	* @brief    CMEM7 ddr header file
	*
	*
	* @version  V1.0
	* @date     6 Feb. 2015
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
	
#ifndef __CMEM7_DDR_TUNNING_H
#define __CMEM7_DDR_TUNNING_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "cmem7.h"
#include "cmem7_conf.h"
	 		
#define DDR_TUNING_DEBUG 1	 		
	 					
void ddr_tuning_get_arr_ior(void);
void ddr_tuning_prepare(void); 
void get_arr_ior(void);
void ddr_get_sensor_temp(void);
void ddr_get_osc_temp(void);		 
void ddr_tuning_run(void);
void m7_ddr_calibration(void);
	 
extern uint32_t CFG_IDLY_START (short v);
extern uint32_t CFG_ODLY_START (short v);
extern uint32_t CFG_SSEL0_START(short v);
extern void set_bits(int start, short len, short val);
extern void set_ssel90(short pad, short v);
	 
#ifdef __cplusplus
}
#endif

#endif /* __CMEM7_DDR_H */

