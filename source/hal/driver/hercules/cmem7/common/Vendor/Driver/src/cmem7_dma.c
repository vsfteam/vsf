/**
	*****************************************************************************
	* @file     cmem7_dma.c
	*
	* @brief    CMEM7 DMA source file
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
	
#include "cmem7_dma.h"
#include "cmem7_misc.h"

typedef struct {
	union {
    uint32_t  CTL_LOW;                              
    
    struct {
      uint32_t  INT_EN     :  1;               
      uint32_t  DST_TR_WIDTH:  3;              
      uint32_t  SRC_TR_WIDTH:  3;              
      uint32_t  DINC       :  2;              
      uint32_t  SINC       :  2;               
      uint32_t  DEST_MSIZE :  3;              
      uint32_t  SRC_MSIZE  :  3;              
      uint32_t  SRC_GATHER_EN:  1;           
      uint32_t  DST_SCATTER_EN:  1;          
      uint32_t             :  1;
      uint32_t  TT_FC      :  3;           
      uint32_t  DMS        :  2;              
      uint32_t  SMS        :  2;            
      uint32_t  LLP_DST_EN :  1;              
      uint32_t  LLP_SRC_EN :  1;              
    } CTL_LOW_b;                                
  } INNER;
} INNER_CTL_LOW;

typedef struct {
  union {
    uint32_t  CTL_HI;     
    
    struct {
      uint32_t  BLOCK_TS   : 12;    
      uint32_t  DONE       :  1;                   
    } CTL_HI_b;                                      
  } INNER;
} INNER_CTL_HIGH;

typedef struct {
  union {
    uint32_t  CFG_LOW;                       
    
    struct {
      uint32_t             :  5;
      uint32_t  CH_PRIOR   :  3;               
      uint32_t  CH_SUSP    :  1;               
                                                     
      uint32_t  FIFO_EMPTY :  1;         
      uint32_t  HS_SEL_DST :  1;               
      uint32_t  HS_SEL_SRC :  1;               
      uint32_t  LOCK_CH_L  :  2;              
      uint32_t  LOCK_B_L   :  2;               
      uint32_t  LOCK_CH    :  1;               
      uint32_t  LOCK_B     :  1;               
      uint32_t  DST_HS_POL :  1;               
      uint32_t  SRC_HS_POL :  1;              
      uint32_t  MAX_ABRST  : 10;               
      uint32_t  RELOAD_SRC :  1;               
      uint32_t  RELOAD_DST :  1;               
    } CFG_LOW_b;                                 
  } INNER;
} INNER_CFG_LOW;

typedef struct {
  union {
    uint32_t  CFG_HI;       
    
    struct {
      uint32_t  FCMODE     :  1;               
      uint32_t  FIFO_MODE  :  1;               
      uint32_t  PROTCTL    :  3;               
      uint32_t  DS_UPD_EN  :  1;               
      uint32_t  SS_UPD_EN  :  1;              
    } CFG_HI_b;                                   
  }INNER;
} INNER_CFG_HIGH;
  
typedef struct {
  uint32_t SAR;
  uint32_t reserved0;
  uint32_t DAR;
  uint32_t reserved1;
  uint32_t LLP;
  uint32_t reserved2;
  INNER_CTL_LOW CTRL_LOW;
  INNER_CTL_HIGH CTRL_HIGH;  
  uint32_t SSTAT;
  uint32_t reserved3;
  uint32_t DSTAT;
  uint32_t reserved4;
  uint32_t SSTATAR;
  uint32_t reserved5;
  uint32_t DSTATAR;
  uint32_t reserved6;
  INNER_CFG_LOW CFG_LOW;
  INNER_CFG_HIGH CFG_HIGH;
  uint32_t SGR;
  uint32_t reserverd7;
  uint32_t DSR;
  uint32_t reserved8;
} INNER_CHANNEL;

typedef struct {                      
	uint32_t srcAddr;                  
	uint32_t dstAddr;                  
	uint32_t nextBlock;              
	INNER_CTL_LOW low;                 
	INNER_CTL_HIGH high;                 
} INNER_BLOCK_DESC;


#define DMA_MAX_CHANNEL_NUM                  8

#define DMA_TR_WIDTH_8_BIT                   0
#define DMA_TR_WIDTH_16_BIT                  1
#define DMA_TR_WIDTH_32_BIT                  2
#define DMA_TR_WIDTH_64_BIT                  3
#define DMA_TR_WIDTH_128_BIT                 4
#define DMA_TR_WIDTH_256_BIT                 5

#define DMA_INC_INCREMENT                    0
#define DMA_INC_DECREMENT                    1
#define DMA_INC_NO_CHANGE                    2

#define DMA_LOCK_DMA_TRANSFER                0
#define DMA_LOCK_DMA_BLOCK_TRANSFER          1
#define DMA_LOCK_DMA_BLOCK_TRANSACTION       2

void DMA_DeInit(void) {
  SOFT_RESET->SOFTRST_b.DMAC_AHB_n = 0;
	SOFT_RESET->SOFTRST_b.DMAC_AHB_n = 1;
}
     
void DMA_Init(uint8_t ch, uint8_t priority) {
  INNER_CHANNEL *p;
  
  assert_param(ch < DMA_MAX_CHANNEL_NUM);
  assert_param(priority < DMA_MAX_CHANNEL_NUM);
  
  if (!DMA->DMA_EN_b.EN) {
    DMA->DMA_EN_b.EN = TRUE;
  }
  
  // set channel disable
  DMA->CH_EN = ((0x1 << ch) << DMA_MAX_CHANNEL_NUM) | 0x0;
  
  DMA_ClearITPendingBit(ch, DMA_Int_All);
  DMA_ITConfig(ch, DMA_Int_All, FALSE);
  
  p = (INNER_CHANNEL *)((uint32_t)DMA + sizeof(INNER_CHANNEL) * ch);
  p->SAR = 0x0;
  p->DAR = 0x0;
  p->LLP = 0x0;
  p->CTRL_LOW.INNER.CTL_LOW = 0;
  p->CTRL_HIGH.INNER.CTL_HI = 0;
  
  p->SSTAT = 0;
  p->DSTAT = 0;
  p->SSTATAR = 0;
  p->DSTATAR = 0;

  p->CFG_LOW.INNER.CFG_LOW_b.CH_PRIOR 		    = 0;
	p->CFG_LOW.INNER.CFG_LOW_b.CH_SUSP 		      = 0;
	p->CFG_LOW.INNER.CFG_LOW_b.HS_SEL_DST 	    = 0;
	p->CFG_LOW.INNER.CFG_LOW_b.LOCK_B_L 		    = 0;
  p->CFG_LOW.INNER.CFG_LOW_b.HS_SEL_SRC 	    = 0;
	p->CFG_LOW.INNER.CFG_LOW_b.LOCK_CH_L 	      = DMA_LOCK_DMA_TRANSFER;
  p->CFG_LOW.INNER.CFG_LOW_b.LOCK_B_L 		    = DMA_LOCK_DMA_TRANSFER;
	p->CFG_LOW.INNER.CFG_LOW_b.LOCK_CH 		      = TRUE;
	p->CFG_LOW.INNER.CFG_LOW_b.LOCK_B 			    = TRUE;
	p->CFG_LOW.INNER.CFG_LOW_b.DST_HS_POL 	    = 0;
	p->CFG_LOW.INNER.CFG_LOW_b.SRC_HS_POL 	    = 0;
	p->CFG_LOW.INNER.CFG_LOW_b.RELOAD_SRC 	    = FALSE;
	p->CFG_LOW.INNER.CFG_LOW_b.RELOAD_DST 	    = FALSE;
  p->CFG_HIGH.INNER.CFG_HI							      = 0x0;
	
  p->SGR = 1;
  p->DSR = 0;
}

void DMA_ITConfig(uint8_t ch, uint32_t Int, BOOL enable) {
  assert_param(ch < DMA_MAX_CHANNEL_NUM);
	assert_param(IS_DMA_INT(Int));
	
  if (enable) {
		if (Int & DMA_Int_TfrComplete) {
			DMA->INT_EN_TFR = ((0x1 << ch) << DMA_MAX_CHANNEL_NUM) | (0x1 << ch);
		} 
		
		if (Int & DMA_Int_Err) {
			DMA->INT_EN_ERR = ((0x1 << ch) << DMA_MAX_CHANNEL_NUM) | (0x1 << ch);
		}
	} else {
		if (Int & DMA_Int_TfrComplete) {
			DMA->INT_EN_TFR = ((0x1 << ch) << DMA_MAX_CHANNEL_NUM) | 0x0;
		} 
		
		if (Int & DMA_Int_Err) {
			DMA->INT_EN_ERR = ((0x1 << ch) << DMA_MAX_CHANNEL_NUM) | 0x0;
		}
	}
}

BOOL DMA_GetITStatus(uint8_t ch, uint32_t Int) {
  assert_param(ch < DMA_MAX_CHANNEL_NUM);
	assert_param(IS_DMA_INT(Int));
	
	if (Int & DMA_Int_TfrComplete) {
		if (DMA->INT_TFR & (0x1 << ch)) {
			return TRUE;
		}
	}
	
	if (Int & DMA_Int_Err) {
		if (DMA->INT_ERR & (0x1 << ch)) {
			return TRUE;
		}
	}
		
	return FALSE;
}

void DMA_ClearITPendingBit(uint8_t ch, uint32_t Int) {
  assert_param(ch < DMA_MAX_CHANNEL_NUM);
	assert_param(IS_DMA_INT(Int));
	
	if (Int & DMA_Int_TfrComplete) {
		DMA->INT_CLEAR_TFR = (0x1 << ch);
	}
	
	if (Int & DMA_Int_Err) {
		DMA->INT_CLEAR_ERR = (0x1 << ch);
	}
}

BOOL DMA_IsBusy(uint8_t ch) {
  assert_param(ch < DMA_MAX_CHANNEL_NUM);
  
	return (DMA->CH_EN_b.EN & (1 << ch)) ? TRUE : FALSE;
}

BOOL DMA_Transfer(uint8_t ch, BLOCK_DESC *blockList) {
	BLOCK_DESC *p;
  INNER_CHANNEL *pCh;
  
  assert_param(ch < DMA_MAX_CHANNEL_NUM);
  
	if (!blockList) {
		return FALSE;
	}
	
	if (DMA_IsBusy(ch)) {
		return FALSE;
	}
	
	p = (BLOCK_DESC *)GLB_ConvertToMappingFromAddr((uint32_t)blockList);
  pCh = (INNER_CHANNEL *)((uint32_t)DMA + sizeof(INNER_CHANNEL) * ch);
	while (p) {
		BOOL llp = FALSE;
		INNER_BLOCK_DESC *inner = (INNER_BLOCK_DESC *)p;
		if (p->nextBlock) {
			llp = TRUE;
		}
		
		inner->srcAddr = GLB_ConvertToMappingFromAddr(p->srcAddr);
		inner->dstAddr = GLB_ConvertToMappingFromAddr(p->dstAddr);
		
		inner->high.INNER.CTL_HI = 0;
		inner->high.INNER.CTL_HI_b.BLOCK_TS = (p->number >> DMA_TR_WIDTH_32_BIT);
		inner->high.INNER.CTL_HI_b.DONE = 0;
		
		inner->nextBlock = GLB_ConvertToMappingFromAddr(p->nextBlock);
		
		inner->low.INNER.CTL_LOW = 0;
		inner->low.INNER.CTL_LOW_b.INT_EN = TRUE;
		inner->low.INNER.CTL_LOW_b.DST_TR_WIDTH 	= DMA_TR_WIDTH_32_BIT;
	  inner->low.INNER.CTL_LOW_b.SRC_TR_WIDTH 	= DMA_TR_WIDTH_32_BIT;
	  inner->low.INNER.CTL_LOW_b.DINC 					= DMA_INC_INCREMENT;
	  inner->low.INNER.CTL_LOW_b.SINC 					= DMA_INC_INCREMENT;
	  inner->low.INNER.CTL_LOW_b.DEST_MSIZE 		= 0;
	  inner->low.INNER.CTL_LOW_b.SRC_MSIZE 		  = 0;
	  inner->low.INNER.CTL_LOW_b.SRC_GATHER_EN  = FALSE;
	  inner->low.INNER.CTL_LOW_b.DST_SCATTER_EN = FALSE;
	  inner->low.INNER.CTL_LOW_b.TT_FC 					= 0;
    inner->low.INNER.CTL_LOW_b.DMS    				= 0;
	  inner->low.INNER.CTL_LOW_b.SMS    				= 0;
	  inner->low.INNER.CTL_LOW_b.LLP_DST_EN   	= llp;
	  inner->low.INNER.CTL_LOW_b.LLP_SRC_EN   	= llp;
		
		if ((uint32_t)inner == GLB_ConvertToMappingFromAddr((uint32_t)blockList)) {
			// copy to DMA
			pCh->SAR = llp ? 0x0 : inner->srcAddr;
			pCh->DAR = llp ? 0x0 : inner->dstAddr;
			
			pCh->CTRL_HIGH.INNER.CTL_HI  = llp ? 0x0 : inner->high.INNER.CTL_HI;
			pCh->CTRL_LOW.INNER.CTL_LOW = inner->low.INNER.CTL_LOW;
			
			pCh->LLP = llp ? (uint32_t)inner : 0x0;
		}
		
		p = (BLOCK_DESC *)inner->nextBlock;
	}

	// open channel
	DMA->CH_EN = ((0x1 << ch) << DMA_MAX_CHANNEL_NUM) | (0x1 << ch);
	
	return TRUE;
}

