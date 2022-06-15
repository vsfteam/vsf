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

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/******************************************************************************/
/*                             Debug Streams                                  */
/******************************************************************************/

describe_mem_stream(VSF_DEBUG_STREAM_RX, VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE)

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

// debug stream


#endif      // __STM32H743XI__

/* EOF */
