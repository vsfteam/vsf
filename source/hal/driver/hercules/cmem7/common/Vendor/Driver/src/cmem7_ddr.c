/**
  *****************************************************************************
  * @file     cmem7_ddr.c
  *
  * @brief    CMEM7 DDR source file
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

#include <stdio.h>
#include "cmem7_ddr.h"
#include "cmem7_ddr_tuning.h"
#include "cmem7_misc.h"

#define MR1_RZQ_6         0x44  // Rtt = 240/6 = 40 ohms
#define MR1_RZQ_4         0x4   // Rtt = 240/4 = 60 ohms
#define MR1_RZQ_2         0x40  // Rtt = 240/2 = 120 ohms
#define MR1_RON_7         0x2  // RON = 240/7 = 34 ohms
#define MR1_RON_6         0x0  // RON = 240/7 = 34 ohms

/* User could change */
#define DDR3_RTT	MR1_RZQ_6
#define DDR3_RON	MR1_RON_7  // RON = 240/7 = 34 ohms

#define MAX(a, b) ((a >= b) ? (a) : (b))
#define CAL_DDR3_CL(cl)   ((cl>11)?(((cl-12)<<MR_CL_OFFSET)|0x4):((cl-4)<<MR_CL_OFFSET))

/** @defgroup DDR time
  * @{
  */
  #define POWER_UP_NOP    200*1000      /*ns*//*JEDEC Standard No. 79-2E   page 17   */
  #define PRE_ALL_NOP     400           /*ns*/
  #define POWER_UP_MIN_CK 200
  #define DDR2_TMRD       2             /*JEDEC Standard No. 79-2E   page 80   */
  #define DDR3_TMRD       4             /*JEDEC Standard No. 79-2E   page 80   */
  #define DDR3_TMOD_CK    12            /*JEDEC Standard No. 79-3E   page 171  table 68   */
  #define DDR3_TMOD       15            /*ns*/  /*JEDEC Standard No. 79-3E   page 171  table 68   */
  #define DDR3_TZQINIT_CK 512           /*JEDEC Standard No. 79-3E   page 171  table 68   */
  #define DDR3_TZQINIT    640           /*ns*/      /*JEDEC Standard No. 79-3E   page 171  table 68   */
  #define DDR3_TDLLK_CK   512           /*JEDEC Standard No. 79-3E   page 171  table 68   */
  #define RSTH_NOP        500*1000      /*ns*//*JEDEC Standard No. 79-3E   page 20   */
  //#define DDR_TREF        7800          /*ns //7.8us at 0<TC<+85/3.9us at 85<TC<+95*/
  #define DDR_TREF        3900          
  /**
  * @}
  */
#ifndef NULL
#define NULL  0
#endif

/** @defgroup DDRC_SW_PROC_CMD
  * @{
  */
#define SW_CMD_NO_PARM    0
#define SW_CMD_DES        0x0             /*des 5'b00000     [31:29]=3'b000           [28:0] DES time cke hold*/
#define SW_CMD_NOP        0x20000000      /*nop 5'b00100     [31:29]=3'b001           [28:0] NOP time cke hold*/
#define SW_CMD_DESCKE     0x40000000      /*descke 5'b01000  [31:29]=3'b010           [28:0] DES time change cke high*/
#define SW_CMD_NOPCKE     0x60000000      /*nopcke 5'b01100  [31:29]=3'b011           [28:0] NOP time change cke high*/
#define SW_CMD_PREA       0x80000000      /*nopcke 5'b10000  [31:27]=5'b10000           */

#define SW_CMD_MR         0x88000000      /*JEDEC Standard No. 79-2E   page 19   */
#define SW_CMD_EMR1       0x88010000      /*JEDEC Standard No. 79-2E   page 19   */
#define SW_CMD_EMR2       0x88020000
#define SW_CMD_EMR3       0x88030000
#define SW_CMD_EMR1_TEST  0x88010004
#define SW_CMD_REF        0x90000000      /*REF 5'b10010     [31:27]=5'b10010       */
#define SW_CMD_RSTL       0xa8000000      /*RSTL */
#define SW_CMD_RSTH       0xb0000000      /*RSTH 5'b10110    [31:27]=5'b10110       */
#define SW_CMD_ZQCL       0xb8000000      /*ZQCL 5'b10111    [31:27]=5'b10111       */

//#define SW_CMD_DLL_EN     0x880103C4
//#define SW_CMD_DLL_EN     0x880103C6
//#define SW_CMD_DLL_EN 		0x88010384 //75 oHM
//#define SW_CMD_DLL_EN 		0x880103C4 //50 oHM
//#define SW_CMD_DLL_EN 		0x88010042 //OK for 200/276MHZ DDR2
//#define SW_CMD_DLL_EN 		0x88010004	// 75oHM & full strength 0520 for Winboned
#define SW_CMD_DLL_EN 		0x88010006	// 75oHM & half strength 0603 for ESMT DDR2
//#define SW_CMD_DLL_EN_333 0x880103CC
//#define SW_CMD_DLL_RST    0x88000553
// PME8094_DDR2
#define SW_CMD_DLL_RST    0x88000952
//#define SW_CMD_DLL_RST_333 0x88000953
// BL 4
#define SW_CMD_DLL_RST_333 0x88000952
/**
  * @}
  */
/** @defgroup DDR2  mode register
  * @{
  */
 /*JEDEC Standard    */
#define MR_BURST_LEN_4    0x2
#define MR_BURST_LEN_8    0x3
//Pi PM_DDR2, but error
//#define MR_BURST_LEN_8    0x5

#define MR_BT_SEQ         0x0
#define MR_BT_INT         0x08
#define MR_CL_OFFSET      4
#define MR_TM_NORMAL      0
#define MR_TM_TEST        0x80
#define MR_DLL_NO         0
#define MR_DLL_YES        0x100
#define MR_WR_OFFSET      9
#define MR_PPD_FAST       0x1000
#define MR2_CWL_OFFSET    3

/**
  * @}
  */

typedef struct {
  uint8_t type;
  uint8_t mode;
  uint8_t Bus_width;
  uint8_t Chip_bus_width;
  uint16_t Chip_size;
} INNER_MEM_CHIP_INFO;

const INNER_MEM_CHIP_INFO DDR_INFO[] = {
  // DDR2
  { MEM_DDR2, 0x20, 8, 8, 32 },
  { MEM_DDR2, 0x21, 8, 16, 32 },
  { MEM_DDR2, 0x22, 8, 16, 64 },
  { MEM_DDR2, 0x23, 8, 8, 64 },

  { MEM_DDR2, 0x24, 16, 8, 32 },
  { MEM_DDR2, 0x25, 16, 16, 32 },
  { MEM_DDR2, 0x26, 16, 16, 64 },
  { MEM_DDR2, 0x27, 16, 8, 64 },

  { MEM_DDR2, 0x28, 8, 8, 128 },
  { MEM_DDR2, 0x29, 8, 16, 128 },
  { MEM_DDR2, 0x2A, 8, 8, 256 },
  { MEM_DDR2, 0x2B, 8, 16, 256 },

  { MEM_DDR2, 0x2C, 16, 8, 128 },
  { MEM_DDR2, 0x2D, 16, 16, 128 },
  { MEM_DDR2, 0x2E, 16, 8, 256 },
  { MEM_DDR2, 0x2F, 16, 16, 256 },

  { MEM_DDR2, 0x30, 8, 8, 512 },
  { MEM_DDR2, 0x31, 8, 8, 512 },

  { MEM_DDR2, 0x32, 16, 8, 512 },
  { MEM_DDR2, 0x33, 16, 8, 512 },

  // DDR3
  { MEM_DDR3, 0x00, 8, 8, 64 },
  { MEM_DDR3, 0x01, 8, 8, 128 },
  { MEM_DDR3, 0x02, 8, 8, 256 },
  { MEM_DDR3, 0x03, 8, 8, 512 },
  { MEM_DDR3, 0x04, 8, 16, 64 },
  { MEM_DDR3, 0x05, 8, 16, 128 },
  { MEM_DDR3, 0x06, 8, 16, 256 },
  { MEM_DDR3, 0x07, 8, 16, 512 },

  { MEM_DDR3, 0x08, 16, 8, 64 },
  { MEM_DDR3, 0x09, 16, 8, 128 },
  { MEM_DDR3, 0x0A, 16, 8, 256 },
  { MEM_DDR3, 0x0B, 16, 8, 512 },
  { MEM_DDR3, 0x0C, 16, 16, 64 },
  { MEM_DDR3, 0x0D, 16, 16, 128 },
  { MEM_DDR3, 0x0E, 16, 16, 256 },
  { MEM_DDR3, 0x0F, 16, 16, 512 },

  { MEM_DDR3, 0x10, 8, 16, 1024 },
  { MEM_DDR3, 0x10, 16, 16, 1024 },
};

static void DDR_Cmd(const uint32_t cmd ,const uint32_t param)
{
  DDRC->CMD =((cmd)|(param));
}
static void DDR_Latency(const uint32_t cycle  )
{
  DDR_Cmd(SW_CMD_NOP,cycle);
}

static uint32_t DDR2_get_rfc(const INNER_MEM_CHIP_INFO *info) {
  uint32_t rfc = 0;

  assert_param(info);

  if (info->type == MEM_DDR2) {
    switch (info->Chip_size) {
      case 32 :
        rfc = 75; break;
      case 64 :
        rfc = 105; break;
      case 128 :
        rfc = 128; break;
      case 256 :
        rfc = 256; break;
      case 512 :
        rfc = 328; break;
    }
  }

  return rfc;
}

/*static*/ void DDR2_conf(const INNER_MEM_CHIP_INFO *info ,const void *ddr)
{
  uint32_t period = 0;
  const DDR2MEM *ptr = (const DDR2MEM *)ddr;
  uint32_t tRFC = 0;

  assert_param(ddr);
  tRFC = DDR2_get_rfc(info);
  period = (1000 / (SYSTEM_CLOCK_FREQ / 1000000));
  CFG_CTRL->DONE_b.CMD = 0x1;

  DDRC->MODE_b.LANE = 1;
  DDRC->MODE_b.B16 = 1;
  DDRC->MODE_b.CLKPOL = 0x3; //Add for clkpol=1(abc dqs0/1) user mode
  DDRC->REF_b.TIME =( DDR_TREF/period);
  DDRC->RP_b.RPA = 1;
  DDRC->RFC_b.DI = (tRFC/period);
  DDRC->PHUNG_b.MODE = 1;
  if (ptr->tCK == 5000) { // 200MHz
    DDRC->RD_SEL_b.PHY_SEL = 5;
	} else if (ptr->tCK == 3000) { // 333MHz
    DDRC->RD_SEL_b.PHY_SEL = 6; //Only 6
  } else if (ptr->tCK <= 4000) { // 200MHz
    DDRC->RD_SEL_b.PHY_SEL = 5;
  } 

  DDRC->DQSEN0_b.DL = 2;
  DDRC->DQSEN1_b.DL = 2;
  DDRC->DQSEN2_b.DL = 2;
  DDRC->DQSEN3_b.DL = 2;
  DDRC->INTCTL_b.DONE = 1;
  DDRC->INTCTL_b.ERR = 0;
  if (ptr->tCK == 5000) { // 200MHz
    DDRC->RDQ_b.L3 = DDRC->RDQ_b.L2 = DDRC->RDQ_b.L1 = DDRC->RDQ_b.L0 = 0x32;
	} else if (ptr->tCK == 3000) { // 333MHz
    DDRC->RDQ_b.L3 = DDRC->RDQ_b.L2 = DDRC->RDQ_b.L1 = DDRC->RDQ_b.L0 = 0x1E;
  } else if (ptr->tCK <= 4000) { // 
    DDRC->RDQ_b.L3 = DDRC->RDQ_b.L2 = DDRC->RDQ_b.L1 = DDRC->RDQ_b.L0 = 0x1E;	
  } 
//   DDRC->ITMDLY_b.I0 = DDRC->ITMDLY_b.IS0 = DDRC->ITMDLY_b.I1 = DDRC->ITMDLY_b.IS1 =
//     DDRC->ITMDLY_b.I2 = DDRC->ITMDLY_b.IS2 = DDRC->ITMDLY_b.I3 = DDRC->ITMDLY_b.IS3 = 5;

  DDR_Latency((POWER_UP_NOP/period));
  DDR_Cmd(SW_CMD_NOPCKE,(PRE_ALL_NOP/period));
  DDR_Cmd(SW_CMD_PREA,NULL);
  DDR_Latency(ptr->tRP);/*tRP*/
  DDR_Cmd(SW_CMD_EMR2,NULL);
  DDR_Latency(DDR2_TMRD);
  DDR_Cmd(SW_CMD_EMR3,NULL);
  DDR_Latency(DDR2_TMRD);
//   if (ptr->tCK == 5000)       // 200MHz
    DDR_Cmd(SW_CMD_DLL_EN,NULL);
//   else if (ptr->tCK == 3000)  // 333MHz
//     DDR_Cmd(SW_CMD_DLL_EN_333,NULL);
  DDR_Latency(DDR2_TMRD);
  if (ptr->tCK == 5000)       // 200MHz
    DDR_Cmd(SW_CMD_DLL_RST,NULL);
  else if (ptr->tCK == 3000)  // 333MHz
    DDR_Cmd(SW_CMD_DLL_RST_333,NULL);
	else if (ptr->tCK <= 4000)  // 
    DDR_Cmd(SW_CMD_DLL_RST_333,NULL);
  DDR_Latency(DDR2_TMRD);
  DDR_Cmd(SW_CMD_PREA,NULL);
  DDR_Latency(ptr->tRP);
  DDR_Cmd(SW_CMD_REF,NULL);
  DDR_Latency((tRFC/period));/*tRFC*/
  DDR_Cmd(SW_CMD_REF,NULL);
  DDR_Latency((tRFC/period));
  //DDR_Cmd(SW_CMD_MR,(((ptr->tWR-1)<<MR_WR_OFFSET)|(ptr->tCL<<MR_CL_OFFSET)|MR_BURST_LEN_8));
	DDR_Cmd(SW_CMD_MR,(((ptr->tWR-1)<<MR_WR_OFFSET)|(ptr->tCL<<MR_CL_OFFSET)|MR_BURST_LEN_4));
  DDR_Latency(POWER_UP_MIN_CK);
  DDR_Cmd(  (SW_CMD_DLL_EN  | ( 7 << 7) ),NULL);
	DDR_Latency(DDR2_TMRD);
	DDR_Cmd(SW_CMD_DLL_EN,NULL);
  DDR_Latency(DDR2_TMRD);
}

static uint32_t DDR3_get_rfc(const INNER_MEM_CHIP_INFO *info) {
  uint32_t rfc = 0;

  assert_param(info);

  if (info->type == MEM_DDR3) {
    switch (info->Chip_size) {
      case 64 :
        rfc = 90; break;
      case 128 :
        rfc = 110; break;
      case 256 :
        rfc = 160; break;
      case 512 :
        rfc = 300; break;
      case 1024 :
        rfc = 350; break;
    }
  }

  return rfc;
}

static void DDR3_conf(const INNER_MEM_CHIP_INFO *info ,const void *ddr)
{

	uint32_t period = 0;
	uint32_t tRFC = 0;
	const DDR3MEM *ptr = (const DDR3MEM *)ddr;

	tRFC = DDR3_get_rfc(info);
	period = (1000 / (SYSTEM_CLOCK_FREQ / 1000000));	
	
	DDRC->MODE_b.LANE = 0x1;
	DDRC->QUE_b.DEPTH = 1;
	DDRC->REF_b.TIME = (DDR_TREF / period);////// //2Gb periodic refresh interval 7.8us 7.8us/5ns
  DDRC->REF_b.TRIG = 1;
	DDRC->REF_b.THRD = 8; 
	DDRC->ZQCSR_b.EN = 1;
	DDRC->ZQCSI = 128 * 1000000 / period;
	DDRC->RP_b.RPA = 1;
	DDRC->RFC_b.DI = (tRFC / period);
	
	if (ptr->tCK == 5000) { // 200MHz 
    DDRC->RD_SEL_b.PHY_SEL = 5;
	} else if (ptr->tCK == 3000) { // 333MHz 
		DDRC->RD_SEL_b.PHY_SEL = 6;
	} else if (ptr->tCK <= 4000) { // 267MHz 
		DDRC->RD_SEL_b.PHY_SEL = 6;
	}
	
	DDRC->PHUNG_b.MODE = 1;	
	DDRC->DQSEN0_b.DL = 2;
	DDRC->DQSEN1_b.DL = 2;
	DDRC->DQSEN2_b.DL = 2;
	DDRC->DQSEN3_b.DL = 2;

	// config MR#
// update DDR3_conf that the Power-up Initialization Sequence:
// - start with a RSTL
// - DDR_Cmd(CMD,para) in which para can also be the latency/wait cycles
	DDR_Cmd(SW_CMD_RSTL, NULL); 
	DDR_Latency(POWER_UP_NOP / period);//RSTL,200us
	DDR_Cmd(SW_CMD_RSTH , NULL);
	DDR_Latency(RSTH_NOP / period);// RESET H & CKE L, 500us
	DDR_Cmd(SW_CMD_NOPCKE,((tRFC+10)/period));//	 cke high 170/tck tck txpr max(5nck, trfc(min) + 10ns)	 2Gb refresh time 160ns
	DDR_Cmd(SW_CMD_EMR2,((ptr->tWCL-5)<<MR2_CWL_OFFSET)); //MR2
	DDR_Latency(DDR3_TMRD);// tmrd  4nck
	DDR_Cmd(SW_CMD_EMR3,NULL);// MR3
	DDR_Latency(DDR3_TMRD); // tmrd
	DDR_Cmd(SW_CMD_EMR1,DDR3_RTT|DDR3_RON); // MR1	
	DDR_Latency(DDR3_TMRD); // tmrd
	DDR_Cmd(SW_CMD_MR,(MR_PPD_FAST|MR_DLL_YES|((ptr->tWR-1)<<MR_WR_OFFSET)|(CAL_DDR3_CL(ptr->tCL))));
														// MR0
														// A1 , A0 2'b0 fixed 8 
														// A2 CL 0 
														// A3 nibble sequential 0  Interleave 1
														// A6,A5,A4 CL 101 CL =9 
														// A7 normal mode 0
														// A8 DLL reset 1
														// A11,A10,A9 wr=(twr/ck)=(15/3)=5	=001
														// A12 DLL control for precharge PD fast exit 1
														// MR0 =0xb50
	DDR_Latency(MAX(DDR3_TMOD_CK,(DDR3_TMOD/period)));// tmod max (12nck,15ns) 
	DDR_Cmd(SW_CMD_ZQCL,NULL);// ZQCL starting ZQ calibration
	DDR_Latency(MAX(DDR3_TZQINIT_CK,(DDR3_TZQINIT/period))); // tdllk 512nck (should be 500nck)
	DDR_Latency(DDR3_TDLLK_CK); // tZQinit max(512nck,640ns) 

}

void DDR_DeInit() {
  SOFT_RESET->SOFTRST_b.DDRC_n = 0;
  SOFT_RESET->SOFTRST_b.DDRC_n = 1;
}

BOOL DDR_Init(const MEM_CHIP_INFO *chip_info, const void *ddr)
{
  uint32_t i, tCL = 0, tWR = 0, tWCL = 0, tWTR = 0, tCK = 0;
  const INNER_MEM_CHIP_INFO *info = 0;

  assert_param(chip_info);
  assert_param(ddr);

  for (i = 0; i < sizeof(DDR_INFO) / sizeof(DDR_INFO[0]); i++) {
    if ((chip_info->mem_type == DDR_INFO[i].type) &&
      (chip_info->Bus_width == DDR_INFO[i].Bus_width) &&
      (chip_info->Chip_bus_width == DDR_INFO[i].Chip_bus_width) &&
      (chip_info->Chip_size == DDR_INFO[i].Chip_size)) {
      info = &DDR_INFO[i];
      break;
    }
  }

  if (!info) {
    return FALSE;
  }

  PDPROT->LOCK_b.EN = 0;
  do {
    CFG_CTRL->PDLLSTR_b.C2R1D = 0; // rst dll_c2r1
    udelay(10);
    CFG_CTRL->PDLLSTR_b.C2R1D = 1; // releset rst
    udelay(10);
  } while ((PDLOCK->GCLK & 0x40) != 0x40);
  PDPROT->LOCK_b.EN = 1;

  DDRC->MODE_b.MODE = info->mode;
	//Changed to fix bus hold error
	DDRC->MODE_b.perfm_sel = 1;
	//Changed to burst 4 write for DDR2
	if (info->type == MEM_DDR2)
		DDRC->BURST_b.LEN = 1;

  if (info->type == MEM_DDR2) {
    const DDR2MEM *ptr = (const DDR2MEM *)ddr;
    DDRC->RL_b.VAL = ptr->tCL;
    DDRC->RCD_b.DI = ptr->tRCD;
    DDRC->RP_b.DI = ptr->tRP;
    DDRC->RC_b.DI = ptr->tRC;
    DDRC->RAS_b.DI = ptr->tRAS;
    tWR = ptr->tWR;
    tWTR = ptr->tWTR;
    DDRC->RRD_b.DI = ptr->tRRD;
    DDRC->RTP_b.DI = ptr->tRTP;
    DDRC->FAW_b.DI = ptr->tFAW;
    DDR2_conf(info, ptr);
  } else {
    const DDR3MEM *ptr = (const DDR3MEM *)ddr;
    DDRC->RL_b.VAL = ptr->tCL;
    tWCL = ptr->tWCL;
    DDRC->RCD_b.DI = ptr->tRCD;
    DDRC->RAS_b.DI = ptr->tRAS;
    DDRC->RP_b.DI = ptr->tRP;
    DDRC->RC_b.DI = ptr->tRC;
    DDRC->RRD_b.DI = ptr->tRRD;
    DDRC->FAW_b.DI = ptr->tFAW;
    tWR = ptr->tWR;
    tCK = ptr->tCK;
    DDRC->RTP_b.DI = ptr->tRTP;
    DDRC->ZQCL_b.DI = ptr->tZQoper;
    DDRC->ZQCS_b.DI = ptr->tZQCS;
    DDR3_conf(info, ptr);
  }

  tCL = DDRC->RL_b.VAL;

  DDRC->WTR_b.DI  = (DDRC->MODE_b.MODE & 0x20) ?
    (tWCL + tWTR + (DDRC->BURST_b.LEN ? 2 : 4)) :
    ((tCK * 4) > 7500) ? 18 : (tCK * 4 + 7500 - 1) / 7500;         //4

  //DDRC->CCD_b.DI  = (DDRC->MODE_b.MODE & 0x20) ? (DDRC->BURST_b.LEN ? 2 : 4) : 4;
	DDRC->CCD_b.DI  = 8;
  DDRC->RTW_b.DI  = (DDRC->MODE_b.MODE & 0x20) ? (DDRC->BURST_b.LEN ? 4 : 6) : (tCL + DDRC->CCD_b.DI - tWCL + (DDRC->BURST_b.LEN ? 0 : 2));
  DDRC->WTP_b.DI  = (DDRC->MODE_b.MODE & 0x20) ? (tCL + tWR + (DDRC->BURST_b.LEN ? 1 : 3)) : (tWCL + tWR + (DDRC->BURST_b.LEN ? 2 : 4));
  DDRC->WL_b.VAL  = (DDRC->MODE_b.MODE & 0x20) ? (MAX(tCL, 3) - 3) : (tWCL - 2);
  //DDRC->ODTH_b.DL = (DDRC->MODE_b.MODE & 0x20) ? (MAX(tCL, 4) - 4) : 0;
	DDRC->ODTH_b.DL = 0;
  if (tCK == 5000) { // 200MHz
    DDRC->ODTL_b.DL = (DDRC->MODE_b.MODE & 0x20) ? (DDRC->BURST_b.LEN ? (tCL - 1) : (tCL + 1)) : (DDRC->BURST_b.LEN ? 4 : 6);
  } else if (tCK == 3000) { // 333MHz
    DDRC->ODTL_b.DL = 0x1f;
  } else if (tCK <= 4000) { //
		DDRC->ODTL_b.DL = 0x1f;//(DDRC->MODE_b.MODE & 0x20) ? (DDRC->BURST_b.LEN ? (tCL - 1) : (tCL + 1)) : (DDRC->BURST_b.LEN ? 4 : 6);
	} 

  // DEBUG INFO HERE
  DDRC->CTRL_b.STR = 1;

  while (0 == DDRC->STA_b.EMPTY);
  while(0 == DDRC->INTRAW_b.DONE);
  DDRC->CTRL_b.STR = DDRC->CTRL_b.TO = 0;

  return TRUE;
}
