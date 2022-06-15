/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#define __VSF_HEAP_CLASS_INHERIT__
#include "vsf.h"

#ifdef __STM32H743XI__

#include "stm32h7xx_hal.h"

/*============================ MACROS ========================================*/

#ifndef VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE
#   define VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE     32
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/******************************************************************************/
/*                             Debug Streams                                  */
/******************************************************************************/

describe_mem_stream(VSF_DEBUG_STREAM_RX, VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE)
static UART_HandleTypeDef DebugStream_UartHandle = {
    .Instance                   = USART1,
    .Init.BaudRate              = 912600,
    .Init.WordLength            = UART_WORDLENGTH_8B,
    .Init.StopBits              = UART_STOPBITS_1,
    .Init.Parity                = UART_PARITY_NONE,
    .Init.HwFlowCtl             = UART_HWCONTROL_NONE,
    .Init.Mode                  = UART_MODE_TX_RX,
    .Init.OverSampling          = UART_OVERSAMPLING_16,
};

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and
  *         you can add your own implementation
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  VSF_STREAM_WRITE(&VSF_DEBUG_STREAM_RX, NULL, 1);

  uint8_t *ptr;
  uint_fast32_t size = VSF_STREAM_GET_WBUF(&VSF_DEBUG_STREAM_RX, &ptr);
  VSF_ASSERT(size > 0);
  UART_Start_Receive_IT(&DebugStream_UartHandle, ptr, 1);
}

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&DebugStream_UartHandle);
}

static void __VSF_DEBUG_STREAM_TX_INIT(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_RCCEx_PeriphCLKConfig(&(RCC_PeriphCLKInitTypeDef){
        .PeriphClockSelection   = RCC_PERIPHCLK_USART16,
        .Usart16ClockSelection  = RCC_USART16CLKSOURCE_D2PCLK2,
    });

    __HAL_RCC_USART1_CLK_ENABLE();

    HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
        .Pin                    = GPIO_PIN_9,
        .Mode                   = GPIO_MODE_AF_PP,
        .Pull                   = GPIO_PULLUP,
        .Speed                  = GPIO_SPEED_FREQ_VERY_HIGH,
        .Alternate              = GPIO_AF7_USART1,
    });
    HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
        .Pin                    = GPIO_PIN_10,
        .Mode                   = GPIO_MODE_AF_PP,
        .Pull                   = GPIO_PULLUP,
        .Speed                  = GPIO_SPEED_FREQ_VERY_HIGH,
        .Alternate              = GPIO_AF7_USART1,
    });

    VSF_STREAM_CONNECT_TX(&VSF_DEBUG_STREAM_RX);
    HAL_UART_Init(&DebugStream_UartHandle);
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    uint8_t *ptr;
    uint_fast32_t size = VSF_STREAM_GET_WBUF(&VSF_DEBUG_STREAM_RX, &ptr);
    VSF_ASSERT(size > 0);
    UART_Start_Receive_IT(&DebugStream_UartHandle, ptr, 1);
}

static void __VSF_DEBUG_STREAM_TX_WRITE_BLOCKED(uint8_t *buf, uint_fast32_t size)
{
    HAL_UART_Transmit(&DebugStream_UartHandle, buf, size, HAL_MAX_DELAY);
}

#define VSF_HAL_USE_DEBUG_STREAM                ENABLED
#include "hal/driver/common/debug_stream/debug_stream_tx_blocked.inc"

/******************************************************************************/
/*                             HAL TIck                                       */
/******************************************************************************/

// avoid to use timer in HAL, over-write HAL_InitTick
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  return HAL_OK;
}

/******************************************************************************/
/*                             Screen Driver                                  */
/******************************************************************************/

//红色数据线
#define LTDC_R0_GPIO_PORT           GPIOI
#define LTDC_R0_GPIO_CLK_ENABLE()   __GPIOI_CLK_ENABLE()
#define LTDC_R0_GPIO_PIN            GPIO_PIN_15
#define LTDC_R0_AF                  GPIO_AF14_LTDC

#define LTDC_R1_GPIO_PORT           GPIOJ
#define LTDC_R1_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_R1_GPIO_PIN            GPIO_PIN_0
#define LTDC_R1_AF                  GPIO_AF14_LTDC

#define LTDC_R2_GPIO_PORT           GPIOJ
#define LTDC_R2_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_R2_GPIO_PIN            GPIO_PIN_1
#define LTDC_R2_AF                  GPIO_AF14_LTDC

#define LTDC_R3_GPIO_PORT           GPIOJ
#define LTDC_R3_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_R3_GPIO_PIN            GPIO_PIN_2
#define LTDC_R3_AF                  GPIO_AF14_LTDC

#define LTDC_R4_GPIO_PORT           GPIOJ
#define LTDC_R4_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_R4_GPIO_PIN            GPIO_PIN_3
#define LTDC_R4_AF                  GPIO_AF14_LTDC

#define LTDC_R5_GPIO_PORT           GPIOJ
#define LTDC_R5_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_R5_GPIO_PIN            GPIO_PIN_4
#define LTDC_R5_AF                  GPIO_AF14_LTDC

#define LTDC_R6_GPIO_PORT           GPIOJ
#define LTDC_R6_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_R6_GPIO_PIN            GPIO_PIN_5
#define LTDC_R6_AF                  GPIO_AF14_LTDC

#define LTDC_R7_GPIO_PORT           GPIOJ
#define LTDC_R7_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_R7_GPIO_PIN            GPIO_PIN_6
#define LTDC_R7_AF                  GPIO_AF14_LTDC
//绿色数据线
#define LTDC_G0_GPIO_PORT           GPIOJ
#define LTDC_G0_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_G0_GPIO_PIN            GPIO_PIN_7
#define LTDC_G0_AF                  GPIO_AF14_LTDC

#define LTDC_G1_GPIO_PORT           GPIOJ
#define LTDC_G1_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_G1_GPIO_PIN            GPIO_PIN_8
#define LTDC_G1_AF                  GPIO_AF14_LTDC

#define LTDC_G2_GPIO_PORT           GPIOJ
#define LTDC_G2_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_G2_GPIO_PIN            GPIO_PIN_9
#define LTDC_G2_AF                  GPIO_AF14_LTDC

#define LTDC_G3_GPIO_PORT           GPIOJ
#define LTDC_G3_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_G3_GPIO_PIN            GPIO_PIN_10
#define LTDC_G3_AF                  GPIO_AF14_LTDC

#define LTDC_G4_GPIO_PORT           GPIOJ
#define LTDC_G4_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_G4_GPIO_PIN            GPIO_PIN_11
#define LTDC_G4_AF                  GPIO_AF14_LTDC

#define LTDC_G5_GPIO_PORT           GPIOK
#define LTDC_G5_GPIO_CLK_ENABLE()   __GPIOK_CLK_ENABLE()
#define LTDC_G5_GPIO_PIN            GPIO_PIN_0
#define LTDC_G5_AF                  GPIO_AF14_LTDC

#define LTDC_G6_GPIO_PORT           GPIOK
#define LTDC_G6_GPIO_CLK_ENABLE()   __GPIOK_CLK_ENABLE()
#define LTDC_G6_GPIO_PIN            GPIO_PIN_1
#define LTDC_G6_AF                  GPIO_AF14_LTDC

#define LTDC_G7_GPIO_PORT           GPIOK
#define LTDC_G7_GPIO_CLK_ENABLE()   __GPIOK_CLK_ENABLE()
#define LTDC_G7_GPIO_PIN            GPIO_PIN_2
#define LTDC_G7_AF                  GPIO_AF14_LTDC

//蓝色数据线
#define LTDC_B0_GPIO_PORT           GPIOJ
#define LTDC_B0_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_B0_GPIO_PIN            GPIO_PIN_12
#define LTDC_B0_AF                  GPIO_AF14_LTDC

#define LTDC_B1_GPIO_PORT           GPIOJ
#define LTDC_B1_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_B1_GPIO_PIN            GPIO_PIN_13
#define LTDC_B1_AF                  GPIO_AF14_LTDC

#define LTDC_B2_GPIO_PORT           GPIOJ
#define LTDC_B2_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_B2_GPIO_PIN            GPIO_PIN_14
#define LTDC_B2_AF                  GPIO_AF14_LTDC

#define LTDC_B3_GPIO_PORT           GPIOJ
#define LTDC_B3_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_B3_GPIO_PIN            GPIO_PIN_15
#define LTDC_B3_AF                  GPIO_AF14_LTDC

#define LTDC_B4_GPIO_PORT           GPIOK
#define LTDC_B4_GPIO_CLK_ENABLE()   __GPIOK_CLK_ENABLE()
#define LTDC_B4_GPIO_PIN            GPIO_PIN_3
#define LTDC_B4_AF                  GPIO_AF14_LTDC

#define LTDC_B5_GPIO_PORT           GPIOK
#define LTDC_B5_GPIO_CLK_ENABLE()   __GPIOK_CLK_ENABLE()
#define LTDC_B5_GPIO_PIN            GPIO_PIN_4
#define LTDC_B5_AF                  GPIO_AF14_LTDC

#define LTDC_B6_GPIO_PORT           GPIOK
#define LTDC_B6_GPIO_CLK_ENABLE()   __GPIOK_CLK_ENABLE()
#define LTDC_B6_GPIO_PIN            GPIO_PIN_5
#define LTDC_B6_AF                  GPIO_AF14_LTDC

#define LTDC_B7_GPIO_PORT           GPIOK
#define LTDC_B7_GPIO_CLK_ENABLE()   __GPIOK_CLK_ENABLE()
#define LTDC_B7_GPIO_PIN            GPIO_PIN_6
#define LTDC_B7_AF                  GPIO_AF14_LTDC

//控制信号线
/*像素时钟CLK*/
#define LTDC_CLK_GPIO_PORT              GPIOI
#define LTDC_CLK_GPIO_CLK_ENABLE()      __GPIOI_CLK_ENABLE()
#define LTDC_CLK_GPIO_PIN               GPIO_PIN_14
#define LTDC_CLK_AF                     GPIO_AF14_LTDC
/*水平同步信号HSYNC*/
#define LTDC_HSYNC_GPIO_PORT            GPIOI
#define LTDC_HSYNC_GPIO_CLK_ENABLE()    __GPIOI_CLK_ENABLE()
#define LTDC_HSYNC_GPIO_PIN             GPIO_PIN_12
#define LTDC_HSYNC_AF                  GPIO_AF14_LTDC
/*垂直同步信号VSYNC*/
#define LTDC_VSYNC_GPIO_PORT            GPIOI
#define LTDC_VSYNC_GPIO_CLK_ENABLE()    __GPIOI_CLK_ENABLE()
#define LTDC_VSYNC_GPIO_PIN             GPIO_PIN_13
#define LTDC_VSYNC_AF                   GPIO_AF14_LTDC

/*数据使能信号DE*/
#define LTDC_DE_GPIO_PORT               GPIOK
#define LTDC_DE_GPIO_CLK_ENABLE()       __GPIOK_CLK_ENABLE()
#define LTDC_DE_GPIO_PIN                GPIO_PIN_7
#define LTDC_DE_AF                      GPIO_AF14_LTDC

/*液晶屏背光信号，高电平使能*/
#define LTDC_BL_GPIO_PORT               GPIOD
#define LTDC_BL_GPIO_CLK_ENABLE()       __GPIOD_CLK_ENABLE()
#define LTDC_BL_GPIO_PIN                GPIO_PIN_13

#define HBP                     46
#define VBP                     23
#define HSW                     1
#define VSW                     1
#define HFP                     22
#define VFP                     22
#define CLOCK_2BYTE             33
#define CLOCK_4BYTE             21

#define LCD_PIXEL_WIDTH         480
#define LCD_PIXEL_HEIGHT        272

static LTDC_HandleTypeDef Ltdc_Handler = {
    .Instance                   = LTDC,
    .Init.HorizontalSync        = HSW - 1,
    .Init.VerticalSync          = VSW - 1,
    .Init.AccumulatedHBP        = HSW + HBP - 1,
    .Init.AccumulatedVBP        = VSW + VBP - 1,
    .Init.AccumulatedActiveW    = HSW + HBP + LCD_PIXEL_WIDTH - 1,
    .Init.AccumulatedActiveH    = VSW + VBP + LCD_PIXEL_HEIGHT - 1,
    .Init.TotalWidth            = HSW + HBP + LCD_PIXEL_WIDTH + HFP - 1,
    .Init.TotalHeigh            = VSW + VBP + LCD_PIXEL_HEIGHT + VFP - 1,
    .Init.Backcolor.Red         = 0,
    .Init.Backcolor.Green       = 0,
    .Init.Backcolor.Blue        = 0,
    .Init.HSPolarity            = LTDC_HSPOLARITY_AL,
    .Init.VSPolarity            = LTDC_VSPOLARITY_AL,
    .Init.DEPolarity            = LTDC_DEPOLARITY_AL,
    .Init.PCPolarity            = LTDC_PCPOLARITY_IPC,

    .LayerCfg[0].ImageWidth     = LCD_PIXEL_WIDTH,
    .LayerCfg[0].ImageHeight    = LCD_PIXEL_HEIGHT,
};

/**
  * @brief  时钟配置
  * @retval lcd_clk_mhz要配置的像素时钟频率
  */
void LCD_ClockConfig(int lcd_clk_mhz)
{
  /* 液晶屏时钟配置 */
  /* 5寸屏的典型PCLK为lcd_clk_mhz MHz，因此PLL3R配置为提供此时钟 */
  /* PLL3_VCO Input = HSE_VALUE/PLL3M = 1 Mhz */
  /* PLL3_VCO Output = PLL3_VCO Input * PLL3N = 420 Mhz */
  /* PLLLCDCLK = PLL3_VCO Output/PLL3R = 420/div = lcd_clk_mhz Mhz */
  /* LTDC clock frequency = PLLLCDCLK = lcd_clk_mhz Mhz */
  RCC_PeriphCLKInitTypeDef periph_clk_init_struct = {
    .PeriphClockSelection       = RCC_PERIPHCLK_LTDC,
    .PLL3.PLL3M                 = 25,
    .PLL3.PLL3N                 = 420,
    .PLL3.PLL3P                 = 2,
    .PLL3.PLL3Q                 = 2,
    .PLL3.PLL3R                 = 420 / lcd_clk_mhz,
  };
  HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);
}

/**
 * @brief  背光LED控制
 * @param  on 1为亮，其余值为灭
 * @retval 无
 */
void LCD_BackLed_Control ( int on )
{
  if ( on )
    HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  初始化LCD层
  * @param  LayerIndex:  前景层(层1)或者背景层(层0)
  * @param  FB_Address:  每一层显存的首地址
  * @param  PixelFormat: 层的像素格式
  * @retval 无
  */
void LCD_LayerInit(uint16_t LayerIndex, uint32_t FB_Address,uint32_t PixelFormat)
{
  LTDC_LayerCfgTypeDef layer_cfg = {
    /* 层初始化 */
    .WindowX0                   = 0,                        //窗口起始位置X坐标
    .WindowX1                   = LCD_PIXEL_WIDTH,          //窗口结束位置X坐标
    .WindowY0                   = 0,                        //窗口起始位置Y坐标
    .WindowY1                   = LCD_PIXEL_HEIGHT,         //窗口结束位置Y坐标
    .PixelFormat                = PixelFormat,              //像素格式
    .FBStartAdress              = FB_Address,               //层显存首地址
    .Alpha                      = 0xFF,                     //用于混合的透明度常量，范围（0—255）0为完全透明
    .Alpha0                     = 0xFF,                     //默认透明度常量，范围（0—255）0为完全透明
    .Backcolor.Blue             = 0xFF,                     //层背景颜色蓝色分量
    .Backcolor.Green            = 0xFF,                     //层背景颜色绿色分量
    .Backcolor.Red              = 0xFF,                     //层背景颜色红色分量
    .BlendingFactor1            = LTDC_BLENDING_FACTOR1_CA, //层混合系数1
    .BlendingFactor2            = LTDC_BLENDING_FACTOR2_PAxCA,//层混合系数2
    .ImageWidth                 = LCD_PIXEL_WIDTH,          //设置图像宽度
    .ImageHeight                = LCD_PIXEL_HEIGHT,         //设置图像高度
  };

  HAL_LTDC_ConfigLayer(&Ltdc_Handler, &layer_cfg, LayerIndex); //设置选中的层参数

  __HAL_LTDC_RELOAD_CONFIG(&Ltdc_Handler);//重载层的配置参数
}

/**
  * @brief  初始化控制LCD的IO
  * @param  无
  * @retval 无
  */
static void LCD_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* 使能LCD使用到的引脚时钟 */
  LTDC_R0_GPIO_CLK_ENABLE();LTDC_R1_GPIO_CLK_ENABLE();LTDC_R2_GPIO_CLK_ENABLE();\
  LTDC_R3_GPIO_CLK_ENABLE();LTDC_R4_GPIO_CLK_ENABLE();LTDC_R5_GPIO_CLK_ENABLE();\
  LTDC_R6_GPIO_CLK_ENABLE();LTDC_R7_GPIO_CLK_ENABLE();LTDC_G0_GPIO_CLK_ENABLE();\
  LTDC_G1_GPIO_CLK_ENABLE();LTDC_G2_GPIO_CLK_ENABLE();LTDC_G3_GPIO_CLK_ENABLE();\
  LTDC_G3_GPIO_CLK_ENABLE();LTDC_G5_GPIO_CLK_ENABLE();LTDC_G6_GPIO_CLK_ENABLE();\
  LTDC_G7_GPIO_CLK_ENABLE();LTDC_B0_GPIO_CLK_ENABLE();LTDC_B1_GPIO_CLK_ENABLE();\
  LTDC_B2_GPIO_CLK_ENABLE();LTDC_B3_GPIO_CLK_ENABLE();LTDC_B4_GPIO_CLK_ENABLE();\
  LTDC_B5_GPIO_CLK_ENABLE();LTDC_B6_GPIO_CLK_ENABLE();LTDC_B7_GPIO_CLK_ENABLE();\
  LTDC_CLK_GPIO_CLK_ENABLE();LTDC_HSYNC_GPIO_CLK_ENABLE();LTDC_VSYNC_GPIO_CLK_ENABLE();\
  LTDC_DE_GPIO_CLK_ENABLE();LTDC_BL_GPIO_CLK_ENABLE();
  /* GPIO配置 */

  /* 红色数据线 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;

  GPIO_InitStruct.Pin =   LTDC_R0_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_R0_AF;
  HAL_GPIO_Init(LTDC_R0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_R1_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_R1_AF;
  HAL_GPIO_Init(LTDC_R1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_R2_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_R2_AF;
  HAL_GPIO_Init(LTDC_R2_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_R3_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_R3_AF;
  HAL_GPIO_Init(LTDC_R3_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_R4_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_R4_AF;
  HAL_GPIO_Init(LTDC_R4_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_R5_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_R5_AF;
  HAL_GPIO_Init(LTDC_R5_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_R6_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_R6_AF;
  HAL_GPIO_Init(LTDC_R6_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_R7_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_R7_AF;
  HAL_GPIO_Init(LTDC_R7_GPIO_PORT, &GPIO_InitStruct);

  /* 绿色数据线 */
  GPIO_InitStruct.Pin =   LTDC_G0_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G0_AF;
  HAL_GPIO_Init(LTDC_G0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_G1_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G1_AF;
  HAL_GPIO_Init(LTDC_G1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_G2_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G2_AF;
  HAL_GPIO_Init(LTDC_G2_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_G3_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G3_AF;
  HAL_GPIO_Init(LTDC_G3_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_G4_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G4_AF;
  HAL_GPIO_Init(LTDC_G4_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_G5_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G5_AF;
  HAL_GPIO_Init(LTDC_G5_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_G6_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G6_AF;
  HAL_GPIO_Init(LTDC_G6_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_G7_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G7_AF;
  HAL_GPIO_Init(LTDC_G7_GPIO_PORT, &GPIO_InitStruct);

  /* 蓝色数据线 */
  GPIO_InitStruct.Pin =   LTDC_B0_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B0_AF;
  HAL_GPIO_Init(LTDC_B0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_B1_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B1_AF;
  HAL_GPIO_Init(LTDC_B1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_B2_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B2_AF;
  HAL_GPIO_Init(LTDC_B2_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_B3_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B3_AF;
  HAL_GPIO_Init(LTDC_B3_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_B4_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B4_AF;
  HAL_GPIO_Init(LTDC_B4_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_B5_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B5_AF;
  HAL_GPIO_Init(LTDC_B5_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_B6_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B6_AF;
  HAL_GPIO_Init(LTDC_B6_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_B7_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B7_AF;
  HAL_GPIO_Init(LTDC_B7_GPIO_PORT, &GPIO_InitStruct);

  //控制信号线
  GPIO_InitStruct.Pin = LTDC_CLK_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_CLK_AF;
  HAL_GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_HSYNC_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_HSYNC_AF;
  HAL_GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_VSYNC_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_VSYNC_AF;
  HAL_GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_DE_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_DE_AF;
  HAL_GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);

  //背光BL
  GPIO_InitStruct.Pin = LTDC_BL_GPIO_PIN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;

  HAL_GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief LCD初始化
  * @param fb_addr 显存首地址
  * @param  lcd_clk_mhz 像素时钟频率（暂时没有使用），为 0 时直接使用推荐时钟频率
            RGB565格式推荐为30~33，
            XRGB8888格式推荐为20~22
            极限范围为15~52，其余值会超出LTDC时钟分频配置范围
  * @param pixel_format 像素格式，如LTDC_Pixelformat_ARGB8888 、LTDC_Pixelformat_RGB565等
  * @retval  None
  */
void LCD_Init(uint32_t fb_addr, int lcd_clk_mhz, uint32_t pixel_format)
{
  /* 使能LTDC时钟 */
  __HAL_RCC_LTDC_CLK_ENABLE();
  /* 使能DMA2D时钟 */
  __HAL_RCC_DMA2D_CLK_ENABLE();

  /* lcd_clk_mhz为0时使用推荐时钟频率 */
  if(lcd_clk_mhz == 0)
  {
    if (pixel_format == LTDC_PIXEL_FORMAT_RGB565 ||
        pixel_format == LTDC_PIXEL_FORMAT_ARGB1555 ||
        pixel_format == LTDC_PIXEL_FORMAT_ARGB4444 ||
        pixel_format == LTDC_PIXEL_FORMAT_L8 ||
        pixel_format == LTDC_PIXEL_FORMAT_AL88)
    {
      lcd_clk_mhz = CLOCK_2BYTE;
    }
    else if (pixel_format == LTDC_PIXEL_FORMAT_ARGB8888 ||
             pixel_format == LTDC_PIXEL_FORMAT_RGB888)
    {
      lcd_clk_mhz = CLOCK_4BYTE;
    }
  }

  /* 液晶屏时钟配置 */
  LCD_ClockConfig(lcd_clk_mhz);
  /* 初始化LCD引脚 */
  LCD_GPIO_Config();
  /* 初始化LTDC */
  HAL_LTDC_Init(&Ltdc_Handler);

  /* 初始化显示层 */
  LCD_LayerInit(0,fb_addr,pixel_format);
}

static vsf_err_t __stm32_ltdc_fb_init(void *fd, vk_disp_color_type_t color_format, void *initial_pixel_buffer)
{
    memset((void *)APP_DISP_FB_BUFFER, 0,
            LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT * APP_DISP_FB_NUM * vsf_disp_get_pixel_format_bytesize(color_format));

    uint32_t pixel_format;
    if (color_format == VSF_DISP_COLOR_RGB888_32) {
        pixel_format = LTDC_PIXEL_FORMAT_ARGB8888;
    } else if (color_format == VSF_DISP_COLOR_RGB565) {
        pixel_format = LTDC_PIXEL_FORMAT_RGB565;
    } else {
        return VSF_ERR_NOT_SUPPORT;
    }

    LCD_Init((uint32_t)APP_DISP_FB_BUFFER, 0, pixel_format);
    LCD_BackLed_Control(1);
    return VSF_ERR_NONE;
}

static vsf_err_t __stm32_ltdc_fb_present(void *fd, void *pixel_buffer)
{
    HAL_LTDC_SetAddress(&Ltdc_Handler, (uint32_t)pixel_buffer, 0);
    return VSF_ERR_NONE;
}

const vk_disp_fb_drv_t stm32_ltdc_fb_drv = {
    .init                       = __stm32_ltdc_fb_init,
    .fini                       = NULL,
    .present                    = __stm32_ltdc_fb_present,
};

/******************************************************************************/
/*                             HW initialize                                  */
/******************************************************************************/
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 400000000 (CPU Clock)
  *            HCLK(Hz)                       = 200000000 (AXI and AHBs Clock)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 160
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /*!< Supply configuration update enable */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;

  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

/* Select PLL as system clock source and configure  bus clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
                                 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}

void vsf_app_driver_init(void)
{
    HAL_Init();
    SystemClock_Config();
}

#endif      // __STM32H743XI__

/* EOF */
