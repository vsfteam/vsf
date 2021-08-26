/**
  * This case is used to ddr ssel90 tuning
	* Hardware :
	*   
  **/

//fix ssel90_h setup bug 
	
#include <stdio.h>
#include <string.h>
#include "cmem7_ddr_tuning.h"
#include "cmem7_gpio.h"
#include "cmem7_uart.h"

#define TEMPERATURE_NUM 3
uint8_t temp_table[TEMPERATURE_NUM][2]=
{
//SSEL90_L,SSEL90_H 
  {0x1,0x1},  //-20C~5C
  {0x1,0x1},  //15C~45C
  {0x1,0x1}   //55C~85C        
};
#define TEMPERATUREL   0
#define TEMPERATUREM   1   
#define TEMPERATUREH   2

//item
#define CFG_PDR_LEN			5
#define CFG_NDR_LEN     5
#define CFG_TPD_LEN			8
#define CFG_TPU_LEN			8
#define CFG_IDLY_LEN		4
#define CFG_ODLY_LEN		4
#define CFG_SSEL0_LEN		4
#define CFG_SSEL90_LEN	4
#define CFG_DQSR_RSTN_SEL_LEN	4
#define CFG_SDLL_PD_LEN	1
//order
#define IO_DQ6		0
#define IO_DQ2		1
#define IO_DQ0		2   
#define IO_DQ11 	3   
#define IO_DQ13 	4   
#define IO_DQ15 	5   
#define IO_DQ9  	6   
#define IO_DM1  	7   
#define IO_DQS0 	8   
#define IO_DQS0N	9   
#define IO_DQ7  	10  
#define IO_DM0  	11  
#define IO_DQS1 	12  
#define IO_DQS1N	13  
#define IO_VRF0 	14  
#define IO_DQ12 	15  
#define IO_DQ14 	16  
#define IO_DQ10 	17  
#define IO_DQ8  	18  
#define IO_DQ1  	19  
#define IO_DQ3  	20  
#define IO_DQ5  	21  
#define IO_DQ4  	22 

#define CFG_IOR_ADDR          0x41700a0c  //{CFG_CTRL_BASE + {`CHAIN_OFFSET,`IO_CLK_OFFSET,7'd3}}
#define ADDR_SHFTREG_LEFT_IOB	0x41700030	//{'d12,2'b0}
#define ADDR_REGFILE_OVERFLOW	0x41700068	//{'d26,2'b0}
#define ADDR_REGFILE_AL_FULL	0x4170006c	//{'d27,2'b0}
#define ADDR_SHFTREG_UPDATE		0x41700a00	//{CFG_CTRL_BASE+{1'b1,2'b01,7'd0,2'b0}}
#define UPDATE_IOR			{\
	*(uint32_t *)(ADDR_SHFTREG_UPDATE) = 0x4;\
	*(uint32_t *)(ADDR_SHFTREG_UPDATE) = 0	;\
}
#define CFG_IOR_LEN     (217 * 32 + 4)  //6948
#define CFG_IOR_ALEN		218 *32 				//CFG_IOR_LEN + 32 - (CFG_IOR_LEN%32)
#define CFG_IOR_ENDNUM	28							//CFG_IOR_LEN%32
static uint32_t         array_ior[(CFG_IOR_LEN)/32];

#define BIT_SET(b)      array_ior[(b)/32] |=  (1<<((b)%32))
#define BIT_CLR(b)      array_ior[(b)/32] &= ~(1<<((b)%32))
#define BIT_GET(b)			array_ior[(b)/32] >> ((b)%32)

static uint32_t in_ssel90_l,   in_ssel90_h;
static uint32_t in_ssel0_l,    in_ssel0_h;
static uint32_t in_dq_l_dly,   in_dq_h_dly;
static uint32_t in_dqs_l_dly,  in_dqs_h_dly;
static uint32_t out_dq_l_dly,  out_dq_h_dly;
static uint32_t out_dqs_l_dly, out_dqs_h_dly;
uint32_t temp_old = TEMPERATUREL;
uint32_t temp_new = TEMPERATUREM;  //default tuning 1 time
uint32_t temp_val_new,temp_val_old;
uint8_t  temp_val_first=1;

// int8_t i8Temp_a[5];
// uint8_t u8cnt=0;
// int8_t i8Tempt_a[16];
// uint8_t u8cntt=0;

//--------4???
//location/address
uint32_t CFG_IDLY_START (short v)
{
	uint32_t addr;
	switch (v) 
	{
		case IO_DQ6		: addr = 72			-1;break;
		case IO_DQ2		: addr = 6910   -1;break;
		case IO_DQ0		: addr = 170    -1;break;  
		case IO_DQ11 	: addr = 6812   -1;break;  
		case IO_DQ13 	: addr = 268    -1;break;  
		case IO_DQ15 	: addr = 6714   -1;break;  
		case IO_DQ9  	: addr = 366    -1;break;  
		case IO_DM1  	: addr = 6616   -1;break;  
		case IO_DQS0 	: addr = 464    -1;break;  
		case IO_DQS0N	: addr = 6518   -1;break;  
		case IO_DQ7  	: addr = 572    -1;break;  
		case IO_DM0  	: addr = 6410   -1;break;  
		case IO_DQS1 	: addr = 670    -1;break;  
		case IO_DQS1N	: addr = 6312   -1;break;  
		case IO_VRF0 	: addr = 778		-1;break;  
		case IO_DQ12 	: addr = 6204		-1;break;  
		case IO_DQ14 	: addr = 876    -1;break;  
		case IO_DQ10 	: addr = 6106   -1;break;  
		case IO_DQ8  	: addr = 974    -1;break;  
		case IO_DQ1  	: addr = 6008   -1;break;  
		case IO_DQ3  	: addr = 1072   -1;break;  
		case IO_DQ5  	: addr = 5910   -1;break;  
		case IO_DQ4  	: addr = 1170   -1;break; 			
		
		default			  : break;
	}
	return addr;
}

uint32_t CFG_ODLY_START (short v)
{
	uint32_t addr;
	switch (v) 
	{
		case IO_DQ6		: addr = 68     -1;break;
		case IO_DQ2		: addr = 6906   -1;break;
		case IO_DQ0		: addr = 166    -1;break;  
		case IO_DQ11 	: addr = 6808   -1;break;  
		case IO_DQ13 	: addr = 264    -1;break;  
		case IO_DQ15 	: addr = 6710   -1;break;  
		case IO_DQ9  	: addr = 362    -1;break;  
		case IO_DM1  	: addr = 6612   -1;break;  
		case IO_DQS0 	: addr = 460    -1;break;  
		case IO_DQS0N	: addr = 6514   -1;break;  
		case IO_DQ7  	: addr = 568    -1;break;  
		case IO_DM0  	: addr = 6406   -1;break;  
		case IO_DQS1 	: addr = 666    -1;break;  
		case IO_DQS1N	: addr = 6308   -1;break;  
		case IO_VRF0 	: addr = 774	-1	;break;  
		case IO_DQ12 	: addr = 6200	-1	;break;  
		case IO_DQ14 	: addr = 872    -1;break;  
		case IO_DQ10 	: addr = 6102   -1;break;  
		case IO_DQ8  	: addr = 970    -1;break;  
		case IO_DQ1  	: addr = 6004   -1;break;  
		case IO_DQ3  	: addr = 1068   -1;break;  
		case IO_DQ5  	: addr = 5906   -1;break;  
		case IO_DQ4  	: addr = 1166   -1;break; 
		default			  : break;
	}
	return addr;
}

uint32_t CFG_SSEL0_START(short v)
{
	uint32_t addr;
	switch (v) 
	{
		case IO_DQS0 	: addr = 521    -1;break;
		case IO_DQS1	:	addr = 727		-1;break;
		default			  : break;
	}
	return addr;
}
//--------

//------3???
void set_bits(int start, short len, short val)
{
	short i; 
	for (i = 0; i < len; i++)
	{
		if ( (val & 0x1) == 0 ) {BIT_CLR(start+i);}
		else {BIT_SET(start+i);}
		val = val >> 1;
	}
}

void set_ssel90(short pad, short v)
{
	switch (pad) 
	{
		case IO_DQS0	:
			set_bits(527,1,v&0x1);
			set_bits(6476,3,(v>>1)&0x7);
		break;
		case IO_DQS1	:
			set_bits(733,1,v&0x1);
			set_bits(6270,3,(v>>1)&0x7);
		break;
		default				: 
		break;
	}
}

//application fun
#define set_idly(pad,v)			set_bits(CFG_IDLY_START(pad),CFG_IDLY_LEN,v)
#define set_odly(pad,v)			set_bits(CFG_ODLY_START(pad),CFG_ODLY_LEN,v)
#define set_ssel0(pad,v)		set_bits(CFG_SSEL0_START(pad),CFG_SSEL0_LEN,v)
//------

//----2???
void ddr_tuning_bytemp(uint32_t temp) 
{
	int i;
	
  if(DDR_TUNING_DEBUG)
  {	
	  printf("Function: %s\n",__func__);
	}  
  
  if(temp >= TEMPERATURE_NUM)
    temp = TEMPERATUREM;
  in_ssel90_l = temp_table[temp][0];
  in_ssel90_h = temp_table[temp][1];

// OK for 5 board but 3/5
// 	out_dqs_h_dly = 2;           //constant
// 	out_dq_h_dly = 1;            //constant 
// 	in_dqs_h_dly = 1;            //constant,from testing
// 	in_dq_h_dly = 2;             //constant,from testing
// 	in_ssel0_h = 1;              //constant 
// 	
// 	out_dqs_l_dly = 2;           //constant
// 	out_dq_l_dly = 1;            //constant
// 	in_dqs_l_dly = 1;            //constant,from testing
// 	in_dq_l_dly = 2;             //constant,from testing
// 	in_ssel0_l = 1;              //constant
	
// debug ok 150401
// 	out_dqs_h_dly = 0;           //constant
// 	out_dq_h_dly = 0;            //constant 
// 	in_dqs_h_dly = 2;            //constant,from testing
// 	in_dq_h_dly = 2;             //constant,from testing
// 	in_ssel0_h = 1;              //constant 
// 	
// 	out_dqs_l_dly = 0;           //constant
// 	out_dq_l_dly = 0;            //constant
// 	in_dqs_l_dly = 2;            //constant,from testing
// 	in_dq_l_dly = 2;             //constant,from testing
// 	in_ssel0_l = 1;              //constant
	
// debug
	out_dqs_h_dly = 0;           //constant
	out_dq_h_dly = 0;            //constant 
	in_dqs_h_dly = 0;            //constant,from testing
	in_dq_h_dly = 0;             //constant,from testing
	in_ssel0_h = 0;              //constant 
	
	out_dqs_l_dly = 0;           //constant
	out_dq_l_dly = 0;            //constant
	in_dqs_l_dly = 0;            //constant,from testing, 1 or 2 for onbox board
	in_dq_l_dly = 0;             //constant,from testing
	in_ssel0_l = 0;              //constant

//Debug	
// 	out_dqs_h_dly = 0;           //constant
// 	out_dq_h_dly = 0;            //constant 
// 	in_dqs_h_dly = 2;            //constant,from testing
// 	in_dq_h_dly = 2;             //constant,from testing
// 	in_ssel0_h = 0;              //constant 
// 	
// 	out_dqs_l_dly = 0;           //constant
// 	out_dq_l_dly = 0;            //constant
// 	in_dqs_l_dly = 3;            //constant,from testing
// 	in_dq_l_dly = 2;             //constant,from testing
// 	in_ssel0_l = 0;              //constant
	
	set_odly(IO_DQS1,out_dqs_h_dly);
	set_odly(IO_DQS1N,out_dqs_h_dly);
	set_odly(IO_DQ8,out_dq_h_dly);
	set_odly(IO_DQ9,out_dq_h_dly);
	set_odly(IO_DQ10,out_dq_h_dly);
	set_odly(IO_DQ11,out_dq_h_dly);
	set_odly(IO_DQ12,out_dq_h_dly);
	set_odly(IO_DQ13,out_dq_h_dly);
	set_odly(IO_DQ14,out_dq_h_dly);
	set_odly(IO_DQ15,out_dq_h_dly);
	set_odly(IO_DM1,out_dq_h_dly);
	
	set_idly(IO_DQS1,in_dqs_h_dly);
	set_idly(IO_DQS1N,in_dqs_h_dly);
	set_idly(IO_DQ8,in_dq_h_dly);
	set_idly(IO_DQ9,in_dq_h_dly);
	set_idly(IO_DQ10,in_dq_h_dly);
	set_idly(IO_DQ11,in_dq_h_dly);
	set_idly(IO_DQ12,in_dq_h_dly);
	set_idly(IO_DQ13,in_dq_h_dly);
	set_idly(IO_DQ14,in_dq_h_dly);
	set_idly(IO_DQ15,in_dq_h_dly);
	set_ssel0(IO_DQS1,in_ssel0_h);
	set_ssel90(IO_DQS1,in_ssel90_h);

	set_odly(IO_DQS0, out_dqs_l_dly);
	set_odly(IO_DQS0N,out_dqs_l_dly);
	set_odly(IO_DQ0,  out_dq_l_dly);
	set_odly(IO_DQ1,  out_dq_l_dly);
	set_odly(IO_DQ2,  out_dq_l_dly);
	set_odly(IO_DQ3,  out_dq_l_dly);
	set_odly(IO_DQ4,  out_dq_l_dly);
	set_odly(IO_DQ5,  out_dq_l_dly);
	set_odly(IO_DQ6,  out_dq_l_dly);
	set_odly(IO_DQ7,  out_dq_l_dly);
	set_odly(IO_DM0,	out_dq_l_dly);
	
	set_idly(IO_DQS0, in_dqs_l_dly);
	set_idly(IO_DQS0N,in_dqs_l_dly);
	set_idly(IO_DQ0,  in_dq_l_dly);
	set_idly(IO_DQ1,  in_dq_l_dly);
	set_idly(IO_DQ2,  in_dq_l_dly);
	set_idly(IO_DQ3,  in_dq_l_dly);
	set_idly(IO_DQ4,  in_dq_l_dly);
	set_idly(IO_DQ5,  in_dq_l_dly);
	set_idly(IO_DQ6,  in_dq_l_dly);
	set_idly(IO_DQ7,  in_dq_l_dly);
	set_ssel0(IO_DQS0,in_ssel0_l);
	set_ssel90(IO_DQS0,in_ssel90_l);


	//disable protection to cfg_ctrl reg
	*(uint32_t *)(0x41007c00) = 0;
	//set BL
	*(uint32_t *)(ADDR_SHFTREG_LEFT_IOB) = CFG_IOR_ENDNUM;
	for(i=CFG_IOR_ALEN/32-1;i>=0;i=i-1)
	{
		//check ready by check fifo not full
		while(((*(uint32_t *)(ADDR_REGFILE_AL_FULL)) & 0x20) != 0)
		{
			__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
      //printf(" %x ",*(uint32_t *)(ADDR_REGFILE_AL_FULL));
		}
		
		*(uint32_t *)(CFG_IOR_ADDR) = array_ior[i];		
	}	
	while(((*(uint32_t *)(ADDR_REGFILE_AL_FULL)) & 0x20) != 0)
	{
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	}
	
	UPDATE_IOR			
	
	//enable protection to cfg_ctrl reg
	*(uint32_t *)(0x41007c00) = 1;	
}
//----

//--1???
void ddr_tuning_get_arr_ior(void)		
{
	int i=0;
	
  if(DDR_TUNING_DEBUG)
  {	
	  printf("Function: %s\n",__func__);
	}

	//disable protection to cfg_ctrl reg
	*(uint32_t *)(0x41007c00) = 0;
	
	//set the clock to cfg_ctrl
	//CONSTRAIN: fCFG_CTRL < 50MHz
	//IN THIS CASE: fSYS = 333MHz, set div by 8, result as 42MHz < 50MHz
	i = *(uint32_t *)0x41007408;
	i |= 0x02000000;
	*(uint32_t *)0x41007408 = i;
		
	//check ready by check fifo empty 5 times
	for(i=0;i<5;i++)
	{
		while(((*(uint32_t *)(ADDR_REGFILE_OVERFLOW)) & 0x10) != 0x10) 
		{
			__nop();__nop();__nop();__nop();
		}
	}
	*(uint32_t *)(ADDR_SHFTREG_LEFT_IOB) = CFG_IOR_ENDNUM;
	for(i=CFG_IOR_ALEN/32-1;i>=0;i=i-1)
	{
		array_ior[i] = *(uint32_t *)(CFG_IOR_ADDR);
	}	
	
	//enable protection to cfg_ctrl reg
	*(uint32_t *)(0x41007c00) = 1;	
}

// void ddr_get_sensor_temp(void)
// {
// 	uint8_t i;
// 	int16_t k;
// 	
//   u8cntt = UART_ReceiveData(UART0, 16, i8Tempt_a);
//   
// 	if((u8cntt+u8cnt) > 5)
// 		u8cntt = 5 - u8cnt;
// 	if(u8cntt)
//   {
//     for(i=u8cnt;i<(u8cntt+u8cnt);i++)
// 		{
//       i8Temp_a[i] = i8Tempt_a[i-u8cnt];
//     }
// 		u8cnt += u8cntt;
//   }
//   
// 	if(u8cnt == 5)
// 	{
// 	  k = 0;
//     for(i=0;i<u8cnt;i++)
//     {
//       k += i8Temp_a[i];
//     }
// 		k /= 5;
// 		u8cnt = 0;
//     if(DDR_TUNING_DEBUG)		
// 		  printf("Sensor Temp_Val: %d\n",k);		
// 		
// 		if(k<5)                temp_new = 0;
// 		else if(k>=5 && k<15)	 temp_new = 0xff;
// 		else if(k>=15 && k<45) temp_new = 1;
// 		else if(k>=45 && k<55) temp_new = 0xff;
// 		else if(k>55)			     temp_new = 2;	
//     if(DDR_TUNING_DEBUG)		
// 		  printf("Sensor Temp_Range: %d\n",temp_new);		
// 	}
// }

void ddr_get_osc_temp(void)
{
	uint8_t k;	
//	uint16_t k;
// 	uint8_t i;
// 	uint32_t j;
// 	int8_t diff;
	
	temp_val_new = GPIO_Read(GPIO_GROUP_GPIO);
//	temp_val_new &= 0x0000000F;
// 	for(i=0;i<4;i++)
// 	{
//     j = GPIO_Read(GPIO_GROUP_GPIO);
// 		j &= 0x0000000F;
// 		if(j != temp_val_new)
// 			return;
// 	}
	k = (uint8_t)(temp_val_new&0x0000000F);
//	k = (uint16_t)(temp_val_new&0x0000FFFF);	
// 	if(temp_val_first)
// 	{
//     temp_val_first = 0;
// 		temp_val_old = k;
//   }
// 	else
// 	{
//     diff = k - temp_val_old;
// 		if(diff > 1)
// 			return;
// 		else if(diff < -1)
// 			return;
// 		else
// 		  temp_val_old = k;
//   }
  if(DDR_TUNING_DEBUG)		
	  printf("Osc Temp_Val: %d\n",k);		
		
	if(k==0x01)        temp_new = 0;
	else if(k==0x02)	 temp_new = 0xff;
	else if(k==0x03)   temp_new = 1;
	else if(k==0x04)   temp_new = 0xff;
	else if(k==0x05)   temp_new = 2;		
  if(DDR_TUNING_DEBUG)			
	  printf("Osc Temp_Range: %d\n",temp_new);		
}

//DDR calibration
void m7_ddr_calibration (void)
{
	uint32_t i,j,k,t,n;
	// run it before ddr_init, and rerun when Temprature change a lot
	#define R_DDRCAL 	0x414000A8UL
	// Get value
	i = *(uint32_t *)(R_DDRCAL);
	//printf("\tREAD DDRCAL reg got %08X\n",i);
	// Set cal_start 
	*(uint32_t *)(R_DDRCAL) = 0;
	//for (i = 0; i < 1000; i++);
	*(uint32_t *)(R_DDRCAL) = 0x80000000;
	//for (i = 0; i < 1000; i++);
	i = *(uint32_t *)(R_DDRCAL);
	do {
		//for (i = 0; i < 1000; i++);
		i = *(uint32_t *)(R_DDRCAL);	//printf("\tREAD DDRCAL reg got %08X\n",i);
		
	} while (((i>>30) & 1) != 1);
	*(uint32_t *)(R_DDRCAL) = 0;
	
	j = (i>>26)&0x1;
	k = (i>>27)&0x1;
	t = (i>>28)&0x1;
	n = (i>>29)&0x1;
	if ( (j|k|t|n) == 1) {
		printf("\tDDRCAL done and tpufailed%d,tpdfailed%d,pdrfailed%d,ndrfailed%d\n", j,k,t,n);
	} else {
		//printf("\tDDRCAL done\n"); 
	}
	
	j = (i    )&0xff;
	k = (i>>8 )&0xff;
	t = (i>>16)&0x1f;
	n = (i>>21)&0x1f;
	printf("\tDDRCAL got TPU=0x%08X TPD=0x%08X PDR=0x%08X NDR=0x%08X\n",j,k,t,n);
	
}

void ddr_tuning_run(void)		
{	
	ddr_tuning_get_arr_ior();	
  //if((temp_new!=0xff) && (temp_new!=temp_old))
  {
    temp_old = temp_new;
    //if(DDR_TUNING_DEBUG)		
		  printf("Tuning Temp_Range: %d\n",temp_new);
    m7_ddr_calibration();
		ddr_tuning_bytemp(temp_new);  
  }
  //ddr_get_osc_temp();			
}
//--1???
