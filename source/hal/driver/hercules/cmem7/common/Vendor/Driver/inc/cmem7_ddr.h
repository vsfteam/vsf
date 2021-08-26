/**
	*****************************************************************************
	* @file     cmem7_ddr.h
	*
	* @brief    CMEM7 AES header file
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
	
#ifndef __CMEM7_DDR_H
#define __CMEM7_DDR_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "cmem7.h"
#include "cmem7_conf.h"
   
/** @defgroup _MEM_TYPE
  * @{
  */
enum _MEM_TYPE 
{
	MEM_DDR2,
	MEM_DDR3		
} ;	
/**
  * @}
  */

/** @defgroup DDR_CHIP_INFO
  * @{
  */
typedef struct {
	uint8_t mem_type;                   /*!< ddr type @ref _MEM_TYPE */
	uint8_t Bus_width;	                /*!< bus width of DDR controller, x8, x16 */
  uint8_t Chip_bus_width;	            /*!< bus width of single chip, x8, x16 */
  uint16_t Chip_size;	                /*!< single chip size, unit is MB */
} MEM_CHIP_INFO;								
/**
  * @}
  */


/** @defgroup DDR2MEM
  * @{
  */
typedef struct {
	uint32_t tCK;       		/*!< Period of clock(ps), not data period */ 
	uint32_t tCL;						/*!< tCL */
	uint32_t tRCD;						/*!< tRCD */
	uint32_t tRP;						/*!< tRP */
	uint32_t tRC;						/*!< tRC */
	uint32_t tRAS;						/*!< tRAS */
	uint32_t tWR;						/*!< tWR */
	uint32_t tRRD;						/*!< tRRD */
	uint32_t tWTR;						/*!< tWTR */
	uint32_t tRTP;						/*!< tRTP */
	uint32_t tFAW;						/*!< tFAW */
} DDR2MEM;						
/**
  * @}
  */

/** @defgroup DDR3MEM
  * @{
  */
typedef struct {
	uint32_t tCK;       		/*!< Period of clock(ps), not data period */ 
	uint32_t tCL;						/*!< tCL */
	uint32_t tWCL;						/*!< tWCL */
	uint32_t tRCD;						/*!< tRCD */
	uint32_t tRAS;						/*!< tRAS */
	uint32_t tRP;						/*!< tRP */
	uint32_t tRC;						/*!< tRC */
	uint32_t tRRD;						/*!< tRRD */
	uint32_t tFAW;						/*!< tFAW */
	uint32_t tWR;						/*!< tWR */
	uint32_t tRTP;						/*!< tRTP */
	uint32_t tZQoper;					/*!< tZQCL */
	uint32_t tZQCS;						/*!< tZQCS */
} DDR3MEM;								
/**
  * @}
  */

/** @defgroup DDR2PREDEF
  * @{
  */
extern const DDR2MEM DDR2PREDEF[];		/*!< Pre-defined DDR2 Timing in library */
#define DDR2_400			0			          // sg5E: DDR2-400C CL=4, tCK=5000 ps
#define DDR2_667			1	              // sg3:  DDR2-667D CL=5, tCK=3000 ps
#define DDR2_533			2	              // sg3:  DDR2-533D CL=5, tCK=3000 ps
/**
  * @}
  */

/** @defgroup DDR3PREDEF
  * @{
  */
extern const DDR3MEM DDR3PREDEF[];		/*!< Pre-defined DDR3 Timing in library */
#define DDR3_400		  	0
#define DDR3_667        1
#define DDR3_533        2
/**
  * @}
  */
  
/**
  * @brief  Deinitializes the DDR Controller registers to their default reset values.
	* @param[in] None
  * @retval None
	*/ 
void DDR_DeInit(void);

/**
  * @brief DDR Timing Configuration
  * @param[in] chip information ,A pointer to struct @ref MEM_CHIP_INFO
  * @param[in] ddr A pointer to struct @ref DDR2MEM or @ref DDR3MEM.
  * @retval void
  */
BOOL  DDR_Init(const MEM_CHIP_INFO *chip_info, const void *ddr);

#ifdef __cplusplus
}
#endif

#endif /* __CMEM7_DDR_H */

