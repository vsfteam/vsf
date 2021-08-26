/**
	*****************************************************************************
	* @file     cmem7_spi.c
	*
	* @brief    CMEM7 SPI source file
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
	
#include "cmem7_spi.h"

#define SPI_DEFAULT_FREQ   1*1000*1000
static uint32_t spi0ReadSkipSize = 0;
static uint32_t spi1ReadSkipSize = 0;
static __inline uint32_t getSkipSize(SPI0_Type* SPIx) {
  if (SPIx == SPI0) {
    return spi0ReadSkipSize;
  }
  
  return spi1ReadSkipSize;
}

static __inline void setSkipSize(SPI0_Type* SPIx, uint32_t size) {
  if (SPIx == SPI0) {
    spi0ReadSkipSize = size;
  } else {
    spi1ReadSkipSize = size;
  }
}
static uint32_t spi_GetClock(SPI0_Type* SPIx) {
	uint32_t dividor = 0;

	if ((uint32_t)SPIx == (uint32_t)SPI0) {
		dividor = GLOBAL_CTRL->CLK_SEL_0_b.SPI0_CLK;
	} else if ((uint32_t)SPIx == (uint32_t)SPI1) {
		dividor = GLOBAL_CTRL->CLK_SEL_0_b.SPI1_CLK;
	} 
	
	return SYSTEM_CLOCK_FREQ / (1 << (dividor + 1));
}
void SPI_DeInit(SPI0_Type* SPIx) {
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  
  if (SPIx == SPI0) {
		SOFT_RESET->SOFTRST_b.SPI0_n = 0;
		SOFT_RESET->SOFTRST_b.SPI0_n = 1;    
	}	else {
		SOFT_RESET->SOFTRST_b.SPI1_n = 0;
		SOFT_RESET->SOFTRST_b.SPI1_n = 1;
	}    
}

void SPI_RSTN(SPI0_Type* SPIx) {
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  
  if (SPIx == SPI0) {
		SOFT_RESET->SOFTRST_b.SPI0_n = 0;
		SOFT_RESET->SOFTRST_b.SPI0_n = 1;    
	}	else {
		SOFT_RESET->SOFTRST_b.SPI1_n = 0;
		SOFT_RESET->SOFTRST_b.SPI1_n = 1;
	}    
}

void SPI_Init(SPI0_Type* SPIx, SPI_InitTypeDef *init) {
	assert_param(IS_SPI_ALL_PERIPH(SPIx));
	assert_param(init);
	assert_param(IS_SPI_MODE(init->SPI_Mode));
    
	SPI_RSTN(SPIx);
	
	if (init->SPI_Mode == SPI_MODE_CPOL_0_CPHA_0) {
		SPIx->CTRL_b.CLK_HIGH = FALSE;
		SPIx->CTRL_b.NEG_EDGE = TRUE;
  } else if (init->SPI_Mode == SPI_MODE_CPOL_0_CPHA_1) {
		SPIx->CTRL_b.CLK_HIGH = FALSE;
		SPIx->CTRL_b.NEG_EDGE = FALSE;
  } else if (init->SPI_Mode == SPI_MODE_CPOL_1_CPHA_0) {
		SPIx->CTRL_b.CLK_HIGH = TRUE;
		SPIx->CTRL_b.NEG_EDGE = FALSE;
  } else {
		SPIx->CTRL_b.CLK_HIGH = TRUE;
		SPIx->CTRL_b.NEG_EDGE = TRUE;
  } 		

	SPIx->CTRL_b.RX_BIT_SEQUENCE = 0;
    SPIx->CTRL_b.TX_BIT_SEQUENCE = 0;
  
   // SPIx->BCNT_b.CNT = 31;
	SPIx->BCNT_b.CNT = 8;  // 9bit *2
    if (spi_GetClock(SPIx) % (init->SPI_Freq << 1)) {
        SPIx->DIV = spi_GetClock(SPIx) /(init->SPI_Freq << 1);
    }	else {
        SPIx->DIV = spi_GetClock(SPIx) /(init->SPI_Freq << 1) - 1;
    }
	SPIx->GAP = 0;
  
  setSkipSize(SPIx, 0);
}

void SPI_StructInit(SPI_InitTypeDef *init) {
  assert_param(init);
  
  init->SPI_Mode = SPI_MODE_CPOL_0_CPHA_0;
  init->SPI_Freq = SPI_DEFAULT_FREQ;
}

void SPI_Cmd(SPI0_Type* SPIx, BOOL enable) {
	assert_param(IS_SPI_ALL_PERIPH(SPIx));
	
  SPIx->CTRL_b.EN = enable;
}

void SPI_ITConfig(SPI0_Type* SPIx, uint32_t Int, BOOL enable) {
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
	assert_param(IS_SPI_INT(Int));
	
  if (enable) {
	  SPIx->INT_MASK &= ~Int;
	} else {
		SPIx->INT_MASK |= Int;
	}
	
	SPIx->INT_MASK &= SPI_INT_ALL;
}

BOOL SPI_GetITStatus(SPI0_Type* SPIx, uint32_t Int) {
	assert_param(IS_SPI_ALL_PERIPH(SPIx));
	assert_param(IS_SPI_INT(Int));
	
	if (0 != (SPIx->INT_STATUS & Int)) {
		return TRUE;
	}
	
	return FALSE;
}
void SPI_ClearITPendingBit(SPI0_Type* SPIx, uint32_t Int) {
	assert_param(IS_SPI_ALL_PERIPH(SPIx));
	assert_param(IS_SPI_INT(Int));
	
	SPIx->INT_STATUS = Int;
}

BOOL SPI_GetFlagStatus(SPI0_Type* SPIx, uint32_t Flag) {
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_SPI_FLAG(Flag));
  
  if (0 != (SPIx->STATUS & Flag)) {
    return TRUE;
  }
  
  return FALSE;
}

uint32_t SPI_ReadFifo(SPI0_Type* SPIx, uint32_t size, uint8_t* data) {
	uint32_t count, i, skip;
	
	assert_param(IS_SPI_ALL_PERIPH(SPIx));
	assert_param(data);
	
	if (!SPIx->CTRL_b.EN) {
		return 0;
	}

	count = 0;
  while (!SPIx->STATUS_b.RFIFO_EMPTY && (size != 0)) {
//  	uint32_t d =	SPIx->RW_DATA;
//    
//    // skip data 
//    skip = getSkipSize(SPIx);
//    if (skip >= 4) {
//      setSkipSize(SPIx, skip - 4);
//      continue;
//    } else {
//      setSkipSize(SPIx, 0);
//      
//      d <<= 31 - SPIx->BCNT_b.CNT;
//      for (i = 0; i < 4 - skip; i++) {    
//        *(data + count++) = (d >> ((3 - skip - i) << 3)) & 0xFF;
//        size --;
//        if (size == 0) {
//          break;
//        }   
//      }
//    }
	 *(data + count++) = SPIx->RW_DATA & 0xFF;		
		size --;			
	}	

	return count;
}
	
uint32_t SPI_WriteFifo(SPI0_Type* SPIx, uint32_t size, uint8_t* data) {
	uint8_t count, i;
	
	assert_param(IS_SPI_ALL_PERIPH(SPIx));
	assert_param(data);
	
	if (!SPIx->CTRL_b.EN) {
		return 0;
	}
	
	count = 0;
//  while (!SPIx->STATUS_b.TFIFO_ALMOST_FULL && (size != 0)) {
//		uint32_t d = 0;
//    
//    for (i = 0; i < 4; i++) {
//      d |= *(data + count++) << ((3 - i) << 3);
//      size --;
//      if (size == 0) {
//        break;
//      }
//    }
//		
//		SPIx->RW_DATA = d;
		
		size --;
		
		SPIx->RW_DATA = *(data + count++) << 24;
//	}
  
	return count;
}

uint32_t SPI_WriteFifo9bit(SPI0_Type* SPIx, uint32_t data) {
	uint32_t tmp;
	assert_param(IS_SPI_ALL_PERIPH(SPIx));
	assert_param(data);
	
	if (!SPIx->CTRL_b.EN) {
		return 0;
	}
	tmp=  data<<23;
	SPIx->RW_DATA = tmp;
	SPIx->TRANS_CNT =0;
	SPIx->TRANS_START_b.TX_TRIGGER = TRUE;

	return tmp;
}

BOOL SPI_Request(SPI0_Type* SPIx, uint32_t outSize, uint32_t inSize) {
  BOOL isRdReq;
  uint32_t size;
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  
  if (!SPIx->CTRL_b.EN) {
		return FALSE;
	}

//  size = outSize + inSize;
	size = outSize;
  //if ((outSize == 0) || (size == 0) || (size > 1024)) {
	if ((size == 0) || (size > 1024)) {
    return FALSE;
  }
  
  isRdReq = (inSize == 0) ? FALSE : TRUE;
  SPIx->CTRL_b.RX_EN = isRdReq ? 1 : 0;
     
  if (isRdReq) {
    setSkipSize(SPIx, outSize);
  }
  
  // Bit count is used to decide CS valid length
  //SPIx->BCNT_b.CNT = ((size << 3) > 32) ? 31 : (size << 3) - 1;
	SPIx->BCNT_b.CNT = 8-1;
  
//  size = (size + 3) >> 2;
  SPIx->TRANS_CNT = size - 1;
	SPIx->TRANS_START_b.TX_TRIGGER = TRUE;
  
  return TRUE;
}


