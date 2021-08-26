/**
	*****************************************************************************
	* @file     cmem7_flash.h
	*
	* @brief    CMEM7 flash controller source file
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
	
#ifndef __CMEM7_FLASH_H
#define __CMEM7_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "cmem7.h"
#include "cmem7_conf.h"

/** @defgroup FLASH_READ_MODE
  * @{
  */
#define FLASH_READ_MODE_NORMAL         0        /*!< normal read, 1 bitwidth, highest freqency is 90MHz */
#define FLASH_READ_MODE_FAST           1        /*!< fast read, 1 bitwidth, highest freqency is 120MHz */
#define FLASH_READ_MODE_FAST_DUAL      2        /*!< fast read, 2 bitwidth, highest freqency is 120MHz */
#define FLASH_READ_MODE_FAST_QUAD      3        /*!< fast read, 4 bitwidth, highest freqency is 90MHz */

#define IS_FLASH_READ_MODE(MODE)       (((MODE) == FLASH_READ_MODE_NORMAL) || \
                                        ((MODE) == FLASH_READ_MODE_FAST) || \
                                        ((MODE) == FLASH_READ_MODE_FAST_DUAL) || \
																				((MODE) == FLASH_READ_MODE_FAST_QUAD))																								
/**
  * @}
  */

/**
  * @brief  UART initialization structure
	*/ 
typedef struct
{
  uint8_t FLASH_ClockDividor;       /*!< flash clock dividor, 2 in n times */
	BOOL FLASH_QuadEnable;            /*!< if allows Quad operation */
	void (*FLASH_Wait)(void);         /*!< When the former read or write operation is excuting, 
	                                       Flash has to call a callback to wait it finish. 
	                                       If null, Flash will wait forever until finish */
} FLASH_InitTypeDef;

/**
  * @brief  Deinitializes the flash controller registers to their default reset values.
	* @param[in] None
  * @retval None
	*/ 
void FLASH_Deinit(void);

/**
  * @brief  flash initialization
  * @note   This function should be called at first before any other interfaces.
	*					Users should make sure that doesn't erase or write data in a 
	*					write-protected region.
	* @param[in] init A pointer to structure FLASH_InitTypeDef
  * @retval None
	*/ 
void FLASH_Init(FLASH_InitTypeDef* init);

/**
  * @brief  Fills each FLASH_InitTypeDef member with its default value.
  * @param[in] init A pointer to structure FLASH_InitTypeDef which will be initialized.
  * @retval None
  */ 
void FLASH_StructInit(FLASH_InitTypeDef *init);

/**
  * @brief  Get identifier of flash
  * @param[out] manufacturer manufacturer of flash
  * @param[out] id identifier of flash
	* @retval None
	*/
void FLASH_GetId(uint8_t *manufacturer, uint16_t *id);

/**
  * @brief  Erase a sector, which is 4K bytes large.
  * @param[in] addr Start address of a sector
	* @retval None
	*/
void FLASH_EraseSector(uint32_t addr);

/**
  * @brief  Erase all chip
  * @param	None
	* @retval None
	*/
void FLASH_EraseChip(void);

/**
  * @brief  Enable flash power down mode or not
  * @param[in] enable The bit indicates if flash power down mode is enable or not
	* @retval None
	*/
void FLASH_EnableDeepPowerDown(BOOL enable);

/**
  * @brief  Read data from flash
	* @param[in] ReadMode Normal or fast read, ref as @ref FLASH_READ_MODE
	* @param[in] addr Start address to be read
	* @param[in] size Expected data size to be read
	* @param[out] data A user-allocated buffer to fetch data to be read
  * @retval None
	*/
void FLASH_Read(uint8_t ReadMode, uint32_t addr, uint16_t size, uint8_t* data);

/**
  * @brief  Write data to flash
	* @param[in] addr Start address to be read
	* @param[in] size Expected data size to be read
	* @param[out] data A pointer to the data to be written
  * @retval None
	*/
void FLASH_Write(uint32_t addr, uint16_t size, uint8_t* data);

#ifdef __cplusplus
}
#endif

#endif /* __CMEM7_FLASH_H */

