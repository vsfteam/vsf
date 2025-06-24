/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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

#ifndef __ST_HAL_QSPI_H__
#define __ST_HAL_QSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HAL_QSPI_MODULE_ENABLED) && VSF_HAL_USE_SPI == ENABLED && defined(VSF_SPI_CTRL_QSPI_ENABLE)

/*============================ INCLUDES ======================================*/

#include "sthal_def.h"

/*============================ MACROS ========================================*/

#define HAL_QSPI_ERROR_NONE          0x00000000U
#define HAL_QSPI_ERROR_TIMEOUT       0x00000001U
#define HAL_QSPI_ERROR_TRANSFER      0x00000002U
#define HAL_QSPI_ERROR_DMA           0x00000004U
#define HAL_QSPI_ERROR_INVALID_PARAM 0x00000008U

#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
#   define HAL_QSPI_ERROR_INVALID_CALLBACK 0x00000010U
#endif

#ifdef VSF_SPI_QSPI_SAMPLE_SHIFTING_NONE
#   define QSPI_SAMPLE_SHIFTING_NONE VSF_SPI_QSPI_SAMPLE_SHIFTING_NONE
#else
#   define QSPI_SAMPLE_SHIFTING_NONE 0
#endif

#ifdef VSF_SPI_QSPI_SAMPLE_SHIFTING_HALFCYCLE
#   define QSPI_SAMPLE_SHIFTING_HALFCYCLE VSF_SPI_QSPI_SAMPLE_SHIFTING_HALFCYCLE
#else
#   define QSPI_SAMPLE_SHIFTING_HALFCYCLE 0
#endif

#ifdef VSF_SPI_QSPI_CS_HIGH_TIME_1_CYCLE
#   define QSPI_CS_HIGH_TIME_1_CYCLE VSF_SPI_QSPI_CS_HIGH_TIME_1_CYCLE
#else
#   define QSPI_CS_HIGH_TIME_1_CYCLE 0
#endif

#ifdef VSF_SPI_QSPI_CS_HIGH_TIME_2_CYCLE
#   define QSPI_CS_HIGH_TIME_2_CYCLE VSF_SPI_QSPI_CS_HIGH_TIME_2_CYCLE
#else
#   define QSPI_CS_HIGH_TIME_2_CYCLE 0
#endif

#ifdef VSF_SPI_QSPI_CS_HIGH_TIME_3_CYCLE
#   define QSPI_CS_HIGH_TIME_3_CYCLE VSF_SPI_QSPI_CS_HIGH_TIME_3_CYCLE
#else
#   define QSPI_CS_HIGH_TIME_3_CYCLE 0
#endif

#ifdef VSF_SPI_QSPI_CS_HIGH_TIME_4_CYCLE
#   define QSPI_CS_HIGH_TIME_4_CYCLE VSF_SPI_QSPI_CS_HIGH_TIME_4_CYCLE
#else
#   define QSPI_CS_HIGH_TIME_4_CYCLE 0
#endif

#ifdef VSF_SPI_QSPI_CS_HIGH_TIME_5_CYCLE
#   define QSPI_CS_HIGH_TIME_5_CYCLE VSF_SPI_QSPI_CS_HIGH_TIME_5_CYCLE
#else
#   define QSPI_CS_HIGH_TIME_5_CYCLE 0
#endif

#ifdef VSF_SPI_QSPI_CS_HIGH_TIME_6_CYCLE
#   define QSPI_CS_HIGH_TIME_6_CYCLE VSF_SPI_QSPI_CS_HIGH_TIME_6_CYCLE
#else
#   define QSPI_CS_HIGH_TIME_6_CYCLE 0
#endif

#ifdef VSF_SPI_QSPI_CS_HIGH_TIME_7_CYCLE
#   define QSPI_CS_HIGH_TIME_7_CYCLE VSF_SPI_QSPI_CS_HIGH_TIME_7_CYCLE
#else
#   define QSPI_CS_HIGH_TIME_7_CYCLE 0
#endif

#ifdef VSF_SPI_QSPI_CS_HIGH_TIME_8_CYCLE
#   define QSPI_CS_HIGH_TIME_8_CYCLE VSF_SPI_QSPI_CS_HIGH_TIME_8_CYCLE
#else
#   define QSPI_CS_HIGH_TIME_8_CYCLE 0
#endif

#define QSPI_CLOCK_MODE_0 VSF_SPI_MODE_0
#define QSPI_CLOCK_MODE_3 VSF_SPI_MODE_3

#ifdef VSF_SPI_QSPI_DUALFLASH_ENABLE
#   define QSPI_DUALFLASH_ENABLE VSF_SPI_QSPI_DUALFLASH_ENABLE
#else
#   define QSPI_DUALFLASH_ENABLE 0
#endif

#ifdef VSF_SPI_QSPI_DUALFLASH_DISABLE
#   define QSPI_DUALFLASH_DISABLE VSF_SPI_QSPI_DUALFLASH_DISABLE
#else
#   define QSPI_DUALFLASH_DISABLE 0
#endif

#ifdef VSF_SPI_QSPI_FLASH_ID_1
#   define QSPI_FLASH_ID_1 VSF_SPI_QSPI_FLASH_ID_1
#else
#   define QSPI_FLASH_ID_1 0
#endif

#ifdef VSF_SPI_QSPI_FLASH_ID_2
#   define QSPI_FLASH_ID_2 VSF_SPI_QSPI_FLASH_ID_2
#else
#   define QSPI_FLASH_ID_2 0
#endif

#ifdef VSF_SPI_CTRL_QSPI_ADDRESS_BITS_8
#   define QSPI_ADDRESS_8_BITS VSF_SPI_CTRL_QSPI_ADDRESS_BITS_8
#else
#   define QSPI_ADDRESS_8_BITS 0
#endif

#ifdef VSF_SPI_CTRL_QSPI_ADDRESS_BITS_16
#   define QSPI_ADDRESS_16_BITS VSF_SPI_CTRL_QSPI_ADDRESS_BITS_16
#else
#   define QSPI_ADDRESS_16_BITS 0
#endif

#ifdef VSF_SPI_CTRL_QSPI_ADDRESS_BITS_24
#   define QSPI_ADDRESS_24_BITS VSF_SPI_CTRL_QSPI_ADDRESS_BITS_24
#else
#   define QSPI_ADDRESS_24_BITS 0
#endif

#ifdef VSF_SPI_CTRL_QSPI_ADDRESS_BITS_32
#   define QSPI_ADDRESS_32_BITS VSF_SPI_CTRL_QSPI_ADDRESS_BITS_32
#else
#   define QSPI_ADDRESS_32_BITS 0
#endif

#ifdef VSF_SPI_QSPI_ALTERNATE_BYTES_8_BITS
#   define QSPI_ALTERNATE_BYTES_8_BITS VSF_SPI_QSPI_ALTERNATE_BYTES_8_BITS
#else
#   define QSPI_ALTERNATE_BYTES_8_BITS 0
#endif

#ifdef VSF_SPI_QSPI_ALTERNATE_BYTES_16_BITS
#   define QSPI_ALTERNATE_BYTES_16_BITS VSF_SPI_QSPI_ALTERNATE_BYTES_16_BITS
#else
#   define QSPI_ALTERNATE_BYTES_16_BITS 0
#endif

#ifdef VSF_SPI_QSPI_ALTERNATE_BYTES_24_BITS
#   define QSPI_ALTERNATE_BYTES_24_BITS VSF_SPI_QSPI_ALTERNATE_BYTES_24_BITS
#else
#   define QSPI_ALTERNATE_BYTES_24_BITS 0
#endif

#ifdef VSF_SPI_QSPI_ALTERNATE_BYTES_32_BITS
#   define QSPI_ALTERNATE_BYTES_32_BITS VSF_SPI_QSPI_ALTERNATE_BYTES_32_BITS
#else
#   define QSPI_ALTERNATE_BYTES_32_BITS 0
#endif

#ifdef VSF_SPI_QSPI_ALTERNATE_BYTES_NONE
#   define QSPI_ALTERNATE_BYTES_NONE VSF_SPI_QSPI_ALTERNATE_BYTES_NONE
#else
#   define QSPI_ALTERNATE_BYTES_NONE 0
#endif

#ifdef VSF_SPI_QSPI_ALTERNATE_BYTES_1_LINE
#   define QSPI_ALTERNATE_BYTES_1_LINE VSF_SPI_QSPI_ALTERNATE_BYTES_1_LINE
#else
#   define QSPI_ALTERNATE_BYTES_1_LINE 0
#endif

#ifdef VSF_SPI_QSPI_ALTERNATE_BYTES_2_LINES
#   define QSPI_ALTERNATE_BYTES_2_LINES VSF_SPI_QSPI_ALTERNATE_BYTES_2_LINES
#else
#   define QSPI_ALTERNATE_BYTES_2_LINES 0
#endif

#ifdef VSF_SPI_QSPI_ALTERNATE_BYTES_4_LINES
#   define QSPI_ALTERNATE_BYTES_4_LINES VSF_SPI_QSPI_ALTERNATE_BYTES_4_LINES
#else
#   define QSPI_ALTERNATE_BYTES_4_LINES 0
#endif

#define QSPI_INSTRUCTION_NONE    0
#define QSPI_INSTRUCTION_1_LINE  1
#define QSPI_INSTRUCTION_2_LINES 2
#define QSPI_INSTRUCTION_4_LINES 3

#define QSPI_ADDRESS_NONE    0
#define QSPI_ADDRESS_1_LINE  1
#define QSPI_ADDRESS_2_LINES 2
#define QSPI_ADDRESS_4_LINES 3

#ifdef VSF_SPI_QSPI_DATA_NONE
#   define QSPI_DATA_NONE VSF_SPI_CTRL_QSPI_DATA_PHASE_SINGLE
#else
#   define QSPI_DATA_NONE 0
#endif

#ifdef VSF_SPI_CTRL_QSPI_DATA_PHASE_SINGLE
#   define QSPI_DATA_1_LINE VSF_SPI_CTRL_QSPI_DATA_PHASE_SINGLE
#else
#   define QSPI_DATA_1_LINE 0
#endif

#ifdef VSF_SPI_CTRL_QSPI_DATA_PHASE_DUAL
#   define QSPI_DATA_2_LINES VSF_SPI_CTRL_QSPI_DATA_PHASE_DUAL
#else
#   define QSPI_DATA_2_LINES 0
#endif

#ifdef VSF_SPI_CTRL_QSPI_DATA_PHASE_QUAD
#   define QSPI_DATA_4_LINES VSF_SPI_CTRL_QSPI_DATA_PHASE_QUAD
#else
#   define QSPI_DATA_4_LINES 0
#endif

#ifdef VSF_SPI_QSPI_DDR_MODE_DISABLE
#   define QSPI_DDR_MODE_DISABLE VSF_SPI_QSPI_DDR_MODE_DISABLE
#else
#   define QSPI_DDR_MODE_DISABLE 0
#endif

#ifdef VSF_SPI_QSPI_DDR_MODE_ENABLE
#   define QSPI_DDR_MODE_ENABLE VSF_SPI_QSPI_DDR_MODE_ENABLE
#else
#   define QSPI_DDR_MODE_ENABLE 0
#endif

#ifdef VSF_SPI_QSPI_DDR_HHC_ANALOG_DELAY
#   define QSPI_DDR_HHC_ANALOG_DELAY VSF_SPI_QSPI_DDR_HHC_ANALOG_DELAY
#else
#   define QSPI_DDR_HHC_ANALOG_DELAY 0
#endif

#ifdef VSF_SPI_QSPI_DDR_HHC_HALF_CLK_DELAY
#   define QSPI_DDR_HHC_HALF_CLK_DELAY VSF_SPI_QSPI_DDR_HHC_HALF_CLK_DELAY
#else
#   define QSPI_DDR_HHC_HALF_CLK_DELAY 0
#endif

#ifdef VSF_SPI_QSPI_SIOO_INST_EVERY_CMD
#   define QSPI_SIOO_INST_EVERY_CMD VSF_SPI_QSPI_SIOO_INST_EVERY_CMD
#else
#   define QSPI_SIOO_INST_EVERY_CMD 0
#endif

#ifdef VSF_SPI_QSPI_SIOO_INST_ONLY_FIRST_CMD
#   define QSPI_SIOO_INST_ONLY_FIRST_CMD VSF_SPI_QSPI_SIOO_INST_ONLY_FIRST_CMD
#else
#   define QSPI_SIOO_INST_ONLY_FIRST_CMD 0
#endif

#ifdef VSF_SPI_QSPI_MATCH_MODE_AND
#   define QSPI_MATCH_MODE_AND VSF_SPI_QSPI_MATCH_MODE_AND
#else
#   define QSPI_MATCH_MODE_AND 0x00000000U
#endif

#ifdef VSF_SPI_QSPI_MATCH_MODE_OR
#   define QSPI_MATCH_MODE_OR VSF_SPI_QSPI_MATCH_MODE_OR
#else
#   define QSPI_MATCH_MODE_OR 0
#endif

#ifdef VSF_SPI_QSPI_AUTOMATIC_STOP_DISABLE
#   define QSPI_AUTOMATIC_STOP_DISABLE VSF_SPI_QSPI_AUTOMATIC_STOP_DISABLE
#else
#   define QSPI_AUTOMATIC_STOP_DISABLE 0x00000000U
#endif

#ifdef VSF_SPI_QSPI_AUTOMATIC_STOP_ENABLE
#   define QSPI_AUTOMATIC_STOP_ENABLE VSF_SPI_QSPI_AUTOMATIC_STOP_ENABLE
#else
#   define QSPI_AUTOMATIC_STOP_ENABLE 0
#endif

#ifdef VSF_SPI_QSPI_TIMEOUT_COUNTER_DISABLE
#   define QSPI_TIMEOUT_COUNTER_DISABLE VSF_SPI_QSPI_TIMEOUT_COUNTER_DISABLE
#else
#   define QSPI_TIMEOUT_COUNTER_DISABLE 0
#endif

#ifdef VSF_SPI_QSPI_TIMEOUT_COUNTER_ENABLE
#   define QSPI_TIMEOUT_COUNTER_ENABLE VSF_SPI_QSPI_TIMEOUT_COUNTER_ENABLE
#else
#   define QSPI_TIMEOUT_COUNTER_ENABLE 0
#endif

#define HAL_QSPI_TIMEOUT_DEFAULT_VALUE 5000U

#define IS_QSPI_CLOCK_PRESCALER(PRESCALER) ((PRESCALER) <= 0xFFU)
#define IS_QSPI_FIFO_THRESHOLD(THR)        (((THR) > 0U) && ((THR) <= 32U))
#define IS_QSPI_SSHIFT(SSHIFT)                                                 \
    (((SSHIFT) == QSPI_SAMPLE_SHIFTING_NONE) ||                                \
     ((SSHIFT) == QSPI_SAMPLE_SHIFTING_HALFCYCLE))
#define IS_QSPI_FLASH_SIZE(FSIZE) (((FSIZE) <= 31U))
#define IS_QSPI_CS_HIGH_TIME(CSHTIME)                                          \
    (((CSHTIME) == QSPI_CS_HIGH_TIME_1_CYCLE) ||                               \
     ((CSHTIME) == QSPI_CS_HIGH_TIME_2_CYCLE) ||                               \
     ((CSHTIME) == QSPI_CS_HIGH_TIME_3_CYCLE) ||                               \
     ((CSHTIME) == QSPI_CS_HIGH_TIME_4_CYCLE) ||                               \
     ((CSHTIME) == QSPI_CS_HIGH_TIME_5_CYCLE) ||                               \
     ((CSHTIME) == QSPI_CS_HIGH_TIME_6_CYCLE) ||                               \
     ((CSHTIME) == QSPI_CS_HIGH_TIME_7_CYCLE) ||                               \
     ((CSHTIME) == QSPI_CS_HIGH_TIME_8_CYCLE))
#define IS_QSPI_CLOCK_MODE(CLKMODE)                                            \
    (((CLKMODE) == QSPI_CLOCK_MODE_0) || ((CLKMODE) == QSPI_CLOCK_MODE_3))
#define IS_QSPI_FLASH_ID(FLASH_ID)                                             \
    (((FLASH_ID) == QSPI_FLASH_ID_1) || ((FLASH_ID) == QSPI_FLASH_ID_2))
#define IS_QSPI_DUAL_FLASH_MODE(MODE)                                          \
    (((MODE) == QSPI_DUALFLASH_ENABLE) || ((MODE) == QSPI_DUALFLASH_DISABLE))
#define IS_QSPI_INSTRUCTION(INSTRUCTION) ((INSTRUCTION) <= 0xFFU)
#define IS_QSPI_ADDRESS_SIZE(ADDR_SIZE)                                        \
    (((ADDR_SIZE) == QSPI_ADDRESS_8_BITS) ||                                   \
     ((ADDR_SIZE) == QSPI_ADDRESS_16_BITS) ||                                  \
     ((ADDR_SIZE) == QSPI_ADDRESS_24_BITS) ||                                  \
     ((ADDR_SIZE) == QSPI_ADDRESS_32_BITS))
#define IS_QSPI_ALTERNATE_BYTES_SIZE(SIZE)                                     \
    (((SIZE) == QSPI_ALTERNATE_BYTES_8_BITS) ||                                \
     ((SIZE) == QSPI_ALTERNATE_BYTES_16_BITS) ||                               \
     ((SIZE) == QSPI_ALTERNATE_BYTES_24_BITS) ||                               \
     ((SIZE) == QSPI_ALTERNATE_BYTES_32_BITS))
#define IS_QSPI_DUMMY_CYCLES(DCY) ((DCY) <= 31U)
#define IS_QSPI_INSTRUCTION_MODE(MODE)                                         \
    (((MODE) == QSPI_INSTRUCTION_NONE) ||                                      \
     ((MODE) == QSPI_INSTRUCTION_1_LINE) ||                                    \
     ((MODE) == QSPI_INSTRUCTION_2_LINES) ||                                   \
     ((MODE) == QSPI_INSTRUCTION_4_LINES))
#define IS_QSPI_ADDRESS_MODE(MODE)                                             \
    (((MODE) == QSPI_ADDRESS_NONE) || ((MODE) == QSPI_ADDRESS_1_LINE) ||       \
     ((MODE) == QSPI_ADDRESS_2_LINES) || ((MODE) == QSPI_ADDRESS_4_LINES))
#define IS_QSPI_ALTERNATE_BYTES_MODE(MODE)                                     \
    (((MODE) == QSPI_ALTERNATE_BYTES_NONE) ||                                  \
     ((MODE) == QSPI_ALTERNATE_BYTES_1_LINE) ||                                \
     ((MODE) == QSPI_ALTERNATE_BYTES_2_LINES) ||                               \
     ((MODE) == QSPI_ALTERNATE_BYTES_4_LINES))
#define IS_QSPI_DATA_MODE(MODE)                                                \
    (((MODE) == QSPI_DATA_NONE) || ((MODE) == QSPI_DATA_1_LINE) ||             \
     ((MODE) == QSPI_DATA_2_LINES) || ((MODE) == QSPI_DATA_4_LINES))
#define IS_QSPI_DDR_MODE(DDR_MODE)                                             \
    (((DDR_MODE) == QSPI_DDR_MODE_DISABLE) ||                                  \
     ((DDR_MODE) == QSPI_DDR_MODE_ENABLE))
#define IS_QSPI_DDR_HHC(DDR_HHC)                                               \
    (((DDR_HHC) == QSPI_DDR_HHC_ANALOG_DELAY) ||                               \
     ((DDR_HHC) == QSPI_DDR_HHC_HALF_CLK_DELAY))
#define IS_QSPI_SIOO_MODE(SIOO_MODE)                                           \
    (((SIOO_MODE) == QSPI_SIOO_INST_EVERY_CMD) ||                              \
     ((SIOO_MODE) == QSPI_SIOO_INST_ONLY_FIRST_CMD))
#define IS_QSPI_INTERVAL(INTERVAL)      ((INTERVAL) <= 0xFFFFUL)
#define IS_QSPI_STATUS_BYTES_SIZE(SIZE) (((SIZE) >= 1U) && ((SIZE) <= 4U))
#define IS_QSPI_MATCH_MODE(MODE)                                               \
    (((MODE) == QSPI_MATCH_MODE_AND) || ((MODE) == QSPI_MATCH_MODE_OR))
#define IS_QSPI_AUTOMATIC_STOP(APMS)                                           \
    (((APMS) == QSPI_AUTOMATIC_STOP_DISABLE) ||                                \
     ((APMS) == QSPI_AUTOMATIC_STOP_ENABLE))
#define IS_QSPI_TIMEOUT_ACTIVATION(TCEN)                                       \
    (((TCEN) == QSPI_TIMEOUT_COUNTER_DISABLE) ||                               \
     ((TCEN) == QSPI_TIMEOUT_COUNTER_ENABLE))
#define IS_QSPI_TIMEOUT_PERIOD(PERIOD) ((PERIOD) <= 0xFFFFU)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef vsf_spi_t QUADSPI_TypeDef;

typedef struct {
    uint32_t ClockPrescaler;
    uint32_t FifoThreshold;
    uint32_t SampleShifting;
    uint32_t FlashSize;
    uint32_t ChipSelectHighTime;
    uint32_t ClockMode;
    uint32_t FlashID;
    uint32_t DualFlash;
} QSPI_InitTypeDef;

typedef enum {
    HAL_QSPI_STATE_RESET             = 0x00U,
    HAL_QSPI_STATE_READY             = 0x01U,
    HAL_QSPI_STATE_BUSY              = 0x02U,
    HAL_QSPI_STATE_BUSY_INDIRECT_TX  = 0x12U,
    HAL_QSPI_STATE_BUSY_INDIRECT_RX  = 0x22U,
    HAL_QSPI_STATE_BUSY_AUTO_POLLING = 0x42U,
    HAL_QSPI_STATE_BUSY_MEM_MAPPED   = 0x82U,
    HAL_QSPI_STATE_ABORT             = 0x08U,
    HAL_QSPI_STATE_ERROR             = 0x04U
} HAL_QSPI_StateTypeDef;

typedef struct {
    /* 指令相关 */
    uint32_t Instruction;     /* QSPI 指令码 */
    uint32_t InstructionMode; /* 指令传输模式 (无/单线/双线/四线) */
    uint32_t SIOOMode;        /* 指令只在首次发送还是每次命令都发送 */

    /* 地址相关 */
    uint32_t Address;     /* QSPI 地址 */
    uint32_t AddressSize; /* 地址大小 (8/16/24/32位) */
    uint32_t AddressMode; /* 地址传输模式 (无/单线/双线/四线) */

    /* 交替字节相关 */
    uint32_t AlternateBytes;     /* QSPI 交替字节 */
    uint32_t AlternateBytesSize; /* 交替字节大小 (8/16/24/32位) */
    uint32_t AlternateByteMode;  /* 交替字节传输模式 (无/单线/双线/四线) */

    /* 数据相关 */
    uint32_t DataMode; /* 数据传输模式 (无/单线/双线/四线) */
    uint32_t NbData;   /* 要传输的数据字节数 */

    /* 时序控制相关 */
    uint32_t DummyCycles;      /* 空周期数 */
    uint32_t DdrMode;          /* 是否启用 DDR 模式 */
    uint32_t DdrHoldHalfCycle; /* DDR 模式下的半周期保持配置 */
} QSPI_CommandTypeDef;

typedef struct {
    uint32_t Match;
    uint32_t Mask;
    uint32_t Interval;
    uint32_t StatusBytesSize;
    uint32_t MatchMode;
    uint32_t AutomaticStop;
} QSPI_AutoPollingTypeDef;

typedef struct {
    uint32_t TimeOutPeriod;
    uint32_t TimeOutActivation;
} QSPI_MemoryMappedTypeDef;

#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
typedef enum {
    HAL_QSPI_ERROR_CB_ID          = 0x00U,
    HAL_QSPI_ABORT_CB_ID          = 0x01U,
    HAL_QSPI_FIFO_THRESHOLD_CB_ID = 0x02U,
    HAL_QSPI_CMD_CPLT_CB_ID       = 0x03U,
    HAL_QSPI_RX_CPLT_CB_ID        = 0x04U,
    HAL_QSPI_TX_CPLT_CB_ID        = 0x05U,
    HAL_QSPI_RX_HALF_CPLT_CB_ID   = 0x06U,
    HAL_QSPI_TX_HALF_CPLT_CB_ID   = 0x07U,
    HAL_QSPI_STATUS_MATCH_CB_ID   = 0x08U,
    HAL_QSPI_TIMEOUT_CB_ID        = 0x09U,
    HAL_QSPI_MSP_INIT_CB_ID       = 0x0AU,
    HAL_QSPI_MSP_DEINIT_CB_ID     = 0x0BU
} HAL_QSPI_CallbackIDTypeDef;
typedef void (*pQSPI_CallbackTypeDef)(QSPI_HandleTypeDef *hqspi);
#endif

typedef struct __QSPI_HandleTypeDef {
    QUADSPI_TypeDef               *Instance;
    QSPI_InitTypeDef               Init;
    uint8_t                       *pTxBuffPtr;
    volatile uint32_t              TxXferSize;
    volatile uint32_t              TxXferCount;
    uint8_t                       *pRxBuffPtr;
    volatile uint32_t              RxXferSize;
    volatile uint32_t              RxXferCount;
    DMA_HandleTypeDef             *hdma;
    volatile HAL_LockTypeDef       Lock;
    volatile HAL_QSPI_StateTypeDef State;
    volatile uint32_t              ErrorCode;
    uint32_t                       Timeout;

    uint32_t NbData;
    uint32_t DummyCycles; /* Flash ID for dual flash mode */
#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
    void (*ErrorCallback)(struct __QSPI_HandleTypeDef *hqspi);
    void (*AbortCpltCallback)(struct __QSPI_HandleTypeDef *hqspi);
    void (*FifoThresholdCallback)(struct __QSPI_HandleTypeDef *hqspi);
    void (*CmdCpltCallback)(struct __QSPI_HandleTypeDef *hqspi);
    void (*RxCpltCallback)(struct __QSPI_HandleTypeDef *hqspi);
    void (*TxCpltCallback)(struct __QSPI_HandleTypeDef *hqspi);
    void (*RxHalfCpltCallback)(struct __QSPI_HandleTypeDef *hqspi);
    void (*TxHalfCpltCallback)(struct __QSPI_HandleTypeDef *hqspi);
    void (*StatusMatchCallback)(struct __QSPI_HandleTypeDef *hqspi);
    void (*TimeOutCallback)(struct __QSPI_HandleTypeDef *hqspi);
    void (*MspInitCallback)(struct __QSPI_HandleTypeDef *hqspi);
    void (*MspDeInitCallback)(struct __QSPI_HandleTypeDef *hqspi);
#endif
} QSPI_HandleTypeDef;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

HAL_StatusTypeDef HAL_QSPI_Init(QSPI_HandleTypeDef *hqspi);
HAL_StatusTypeDef HAL_QSPI_DeInit(QSPI_HandleTypeDef *hqspi);
void              HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi);
void              HAL_QSPI_MspDeInit(QSPI_HandleTypeDef *hqspi);

#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_QSPI_RegisterCallback(
    QSPI_HandleTypeDef *hqspi, HAL_QSPI_CallbackIDTypeDef CallbackId,
    pQSPI_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_QSPI_UnRegisterCallback(
    QSPI_HandleTypeDef *hqspi, HAL_QSPI_CallbackIDTypeDef CallbackId);
#endif

HAL_StatusTypeDef HAL_QSPI_Command(QSPI_HandleTypeDef  *hqspi,
                                   QSPI_CommandTypeDef *cmd, uint32_t Timeout);
HAL_StatusTypeDef HAL_QSPI_Transmit(QSPI_HandleTypeDef *hqspi, uint8_t *pData,
                                    uint32_t Timeout);
HAL_StatusTypeDef HAL_QSPI_Receive(QSPI_HandleTypeDef *hqspi, uint8_t *pData,
                                   uint32_t Timeout);
HAL_StatusTypeDef HAL_QSPI_Command_IT(QSPI_HandleTypeDef  *hqspi,
                                      QSPI_CommandTypeDef *cmd);
HAL_StatusTypeDef HAL_QSPI_Transmit_IT(QSPI_HandleTypeDef *hqspi,
                                       uint8_t            *pData);
HAL_StatusTypeDef HAL_QSPI_Receive_IT(QSPI_HandleTypeDef *hqspi,
                                      uint8_t            *pData);
HAL_StatusTypeDef HAL_QSPI_Transmit_DMA(QSPI_HandleTypeDef *hqspi,
                                        uint8_t            *pData);
HAL_StatusTypeDef HAL_QSPI_Receive_DMA(QSPI_HandleTypeDef *hqspi,
                                       uint8_t            *pData);
HAL_StatusTypeDef HAL_QSPI_AutoPolling(QSPI_HandleTypeDef      *hqspi,
                                       QSPI_CommandTypeDef     *cmd,
                                       QSPI_AutoPollingTypeDef *cfg,
                                       uint32_t                 Timeout);
HAL_StatusTypeDef HAL_QSPI_AutoPolling_IT(QSPI_HandleTypeDef      *hqspi,
                                          QSPI_CommandTypeDef     *cmd,
                                          QSPI_AutoPollingTypeDef *cfg);
HAL_StatusTypeDef HAL_QSPI_MemoryMapped(QSPI_HandleTypeDef       *hqspi,
                                        QSPI_CommandTypeDef      *cmd,
                                        QSPI_MemoryMappedTypeDef *cfg);
void              HAL_QSPI_ErrorCallback(QSPI_HandleTypeDef *hqspi);
void              HAL_QSPI_AbortCpltCallback(QSPI_HandleTypeDef *hqspi);
void              HAL_QSPI_FifoThresholdCallback(QSPI_HandleTypeDef *hqspi);
void              HAL_QSPI_CmdCpltCallback(QSPI_HandleTypeDef *hqspi);
void              HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi);
void              HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi);
void              HAL_QSPI_RxHalfCpltCallback(QSPI_HandleTypeDef *hqspi);
void              HAL_QSPI_TxHalfCpltCallback(QSPI_HandleTypeDef *hqspi);
void              HAL_QSPI_StatusMatchCallback(QSPI_HandleTypeDef *hqspi);
void              HAL_QSPI_TimeOutCallback(QSPI_HandleTypeDef *hqspi);
HAL_QSPI_StateTypeDef HAL_QSPI_GetState(const QSPI_HandleTypeDef *hqspi);
uint32_t              HAL_QSPI_GetError(const QSPI_HandleTypeDef *hqspi);
HAL_StatusTypeDef     HAL_QSPI_Abort(QSPI_HandleTypeDef *hqspi);
HAL_StatusTypeDef     HAL_QSPI_Abort_IT(QSPI_HandleTypeDef *hqspi);
void HAL_QSPI_SetTimeout(QSPI_HandleTypeDef *hqspi, uint32_t Timeout);
HAL_StatusTypeDef HAL_QSPI_SetFifoThreshold(QSPI_HandleTypeDef *hqspi,
                                            uint32_t            Threshold);
uint32_t          HAL_QSPI_GetFifoThreshold(const QSPI_HandleTypeDef *hqspi);
HAL_StatusTypeDef HAL_QSPI_SetFlashID(QSPI_HandleTypeDef *hqspi,
                                      uint32_t            FlashID);

#endif /* defined(HAL_QSPI_MODULE_ENABLED) && VSF_HAL_USE_SPI == ENABLED && defined(VSF_SPI_CTRL_QSPI_ENABLE) */

#ifdef __cplusplus
}
#endif

#endif /* __ST_HAL_QSPI_H__ */
