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

#ifndef __VSF_TEMPLATE_SDIO_H__
#define __VSF_TEMPLATE_SDIO_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/**
 * \~english
 * @brief Enable multi-class support by default for maximum availability
 *
 * \~chinese
 * @brief 默认启用多类特性支持以获得最大可用性
 */
#ifndef VSF_SDIO_CFG_MULTI_CLASS
#   define VSF_SDIO_CFG_MULTI_CLASS              ENABLED
#endif

/**
 * \~english
 * @brief Define SDIO hardware mask if count is defined
 *
 * \~chinese
 * @brief 如果定义了SDIO硬件数量,则定义对应的掩码
 */
#if defined(VSF_HW_SDIO_COUNT) && !defined(VSF_HW_SDIO_MASK)
#   define VSF_HW_SDIO_MASK             VSF_HAL_COUNT_TO_MASK(VSF_HW_SDIO_COUNT)
#endif

/**
 * \~english
 * @brief Define SDIO hardware count if mask is defined
 *
 * \~chinese
 * @brief 如果定义了SDIO硬件掩码，则定义对应的数量
 */
#if defined(VSF_HW_SDIO_MASK) && !defined(VSF_HW_SDIO_COUNT)
#   define VSF_HW_SDIO_COUNT            VSF_HAL_MASK_TO_COUNT(VSF_HW_SDIO_MASK)
#endif

/**
 * \~english
 * @brief We can redefine macro VSF_SDIO_CFG_PREFIX to specify a prefix
 * to call a specific driver directly in the application code.
 * \~chinese
 * @brief 可重新定义宏 VSF_SDIO_CFG_PREFIX，以在应用代码中直接调用
 * 特定驱动。
 */
#ifndef VSF_SDIO_CFG_PREFIX
#   if VSF_SDIO_CFG_MULTI_CLASS == ENABLED
#       define VSF_SDIO_CFG_PREFIX                  vsf
#   elif defined(VSF_HW_SDIO_COUNT) && (VSF_HW_SDIO_COUNT != 0)
#       define VSF_SDIO_CFG_PREFIX                  vsf_hw
#   else
#       define VSF_SDIO_CFG_PREFIX                  vsf
#   endif
#endif

/**
 * \~english
 * @brief Disable VSF_SDIO_CFG_FUNCTION_RENAME to use the original function
 * names (e.g., vsf_sdio_init()).
 *
 * \~chinese
 * @brief 禁用 VSF_SDIO_CFG_FUNCTION_RENAME 以使用原始函数名
 * (例如 vsf_sdio_init())
 */
#ifndef VSF_SDIO_CFG_FUNCTION_RENAME
#   define VSF_SDIO_CFG_FUNCTION_RENAME             ENABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_SDIO_CFG_REIMPLEMENT_TYPE_MODE in specific hardware
 * drivers to redefine enum @ref vsf_sdio_mode_t.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_SDIO_CFG_REIMPLEMENT_TYPE_MODE
 * 来重新定义枚举 @ref vsf_sdio_mode_t
 */
#ifndef VSF_SDIO_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_SDIO_CFG_REIMPLEMENT_TYPE_MODE       DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_SDIO_CFG_REIMPLEMENT_TYPE_IRQ_MASK to redefine enum
 * @ref vsf_sdio_irq_mask_t.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_SDIO_CFG_REIMPLEMENT_TYPE_IRQ_MASK
 * 来重新定义枚举 @ref vsf_sdio_irq_mask_t。
 */
#ifndef VSF_SDIO_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_SDIO_CFG_REIMPLEMENT_TYPE_IRQ_MASK   DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_SDIO_CFG_REIMPLEMENT_TYPE_STATUS to redefine struct
 * @ref vsf_sdio_status_t.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_SDIO_CFG_REIMPLEMENT_TYPE_STATUS
 * 来重新定义结构体 @ref vsf_sdio_status_t。
 */
#ifndef VSF_SDIO_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_SDIO_CFG_REIMPLEMENT_TYPE_STATUS     DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_SDIO_CFG_REIMPLEMENT_TYPE_CFG to redefine struct
 * @ref vsf_sdio_cfg_t. For compatibility, members should not be deleted
 * when redefining.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_SDIO_CFG_REIMPLEMENT_TYPE_CFG
 * 来重新定义结构体 @ref vsf_sdio_cfg_t。为保证兼容性，
 * 重新定义时不应删除成员。
 */
#ifndef VSF_SDIO_CFG_REIMPLEMENT_TYPE_CFG
#    define VSF_SDIO_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_SDIO_CFG_REIMPLEMENT_TYPE_CAPABILITY to redefine struct
 * @ref vsf_sdio_capability_t. For compatibility, members should not be
 * deleted when redefining.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_SDIO_CFG_REIMPLEMENT_TYPE_CAPABILITY
 * 来重新定义结构体 @ref vsf_sdio_capability_t。为保证兼容性，
 * 重新定义时不应删除成员。
 */
#ifndef VSF_SDIO_CFG_REIMPLEMENT_TYPE_CAPABILITY
#    define VSF_SDIO_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_SDIO_CFG_INHERIT_HAL_CAPABILITY to inherit the structure
 * @ref vsf_peripheral_capability_t.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_SDIO_CFG_INHERIT_HAL_CAPABILITY
 * 来继承结构体 @ref vsf_peripheral_capability_t。
 */
#ifndef VSF_SDIO_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_SDIO_CFG_INHERIT_HAL_CAPABILITY       ENABLED
#endif

/* SD commands                                  type  argument     response */
  /* class 0 */
/* This is basically the same command as for MMC with some quirks. */
#define SD_SEND_RELATIVE_ADDR           3   /* bcr                      R6  */
#define SD_SEND_RELATIVE_ADDR_OP        (SDIO_RESP_R6)
#define SD_SEND_IF_COND                 8   /* bcr  [11:0] See below    R7  */
#define SD_SEND_IF_COND_OP              (SDIO_RESP_R7)
#define SD_SWITCH_VOLTAGE               11  /* ac                       R1  */
#define SD_SWITCH_VOLTAGE_OP            (SDIO_RESP_R1 | SDIO_CMDOP_CLKHOLD)

  /* class 10 */
#define SD_SWITCH                       6   /* adtc [31:0] See below    R1  */
#define SD_SWITCH_OP                    (SDIO_RESP_R1)

  /* class 5 */
#define SD_ERASE_WR_BLK_START           32   /* ac   [31:0] data addr   R1  */
#define SD_ERASE_WR_BLK_START_OP        (SDIO_RESP_R1)
#define SD_ERASE_WR_BLK_END             33   /* ac   [31:0] data addr   R1  */
#define SD_ERASE_WR_BLK_END_OP          (SDIO_RESP_R1)

  /* Application commands */
#define SD_APP_SET_BUS_WIDTH            6   /* ac   [1:0] bus width     R1  */
#define SD_APP_SET_BUS_WIDTH_OP         (SDIO_RESP_R1)
#   define SD_BUS_WIDTH_1               0
#   define SD_BUS_WIDTH_4               2
#   define SD_BUS_WIDTH_8               3
#define SD_APP_SD_STATUS                13   /* adtc                    R1  */
#define SD_APP_SD_STATUS_OP             (SDIO_RESP_R1)
#define SD_APP_SEND_NUM_WR_BLKS         22   /* adtc                    R1  */
#define SD_APP_SEND_NUM_WR_BLKS_OP      (SDIO_RESP_R1)
#define SD_APP_OP_COND                  41   /* bcr  [31:0] OCR         R3  */
#define SD_APP_OP_COND_OP               (SDIO_RESP_R3)
#define SD_APP_SEND_SCR                 51   /* adtc                    R1  */
#define SD_APP_SEND_SCR_OP              (SDIO_RESP_R1)

  /* class 11 */
#define SD_READ_EXTR_SINGLE             48   /* adtc [31:0]             R1  */
#define SD_READ_EXTR_SINGLE_OP          (SDIO_RESP_R1)
#define SD_WRITE_EXTR_SINGLE            49   /* adtc [31:0]             R1  */
#define SD_WRITE_EXTR_SINGLE_OP         (SDIO_RESP_R1)

/* OCR bit definitions */
#define SD_OCR_VDD_165_195              0x00000080  /* VDD voltage 1.65 - 1.95 */
#define SD_OCR_VDD_20_21                0x00000100  /* VDD voltage 2.0 ~ 2.1 */
#define SD_OCR_VDD_21_22                0x00000200  /* VDD voltage 2.1 ~ 2.2 */
#define SD_OCR_VDD_22_23                0x00000400  /* VDD voltage 2.2 ~ 2.3 */
#define SD_OCR_VDD_23_24                0x00000800  /* VDD voltage 2.3 ~ 2.4 */
#define SD_OCR_VDD_24_25                0x00001000  /* VDD voltage 2.4 ~ 2.5 */
#define SD_OCR_VDD_25_26                0x00002000  /* VDD voltage 2.5 ~ 2.6 */
#define SD_OCR_VDD_26_27                0x00004000  /* VDD voltage 2.6 ~ 2.7 */
#define SD_OCR_VDD_27_28                0x00008000  /* VDD voltage 2.7 ~ 2.8 */
#define SD_OCR_VDD_28_29                0x00010000  /* VDD voltage 2.8 ~ 2.9 */
#define SD_OCR_VDD_29_30                0x00020000  /* VDD voltage 2.9 ~ 3.0 */
#define SD_OCR_VDD_30_31                0x00040000  /* VDD voltage 3.0 ~ 3.1 */
#define SD_OCR_VDD_31_32                0x00080000  /* VDD voltage 3.1 ~ 3.2 */
#define SD_OCR_VDD_32_33                0x00100000  /* VDD voltage 3.2 ~ 3.3 */
#define SD_OCR_VDD_33_34                0x00200000  /* VDD voltage 3.3 ~ 3.4 */
#define SD_OCR_VDD_34_35                0x00400000  /* VDD voltage 3.4 ~ 3.5 */
#define SD_OCR_VDD_35_36                0x00800000  /* VDD voltage 3.5 ~ 3.6 */
#define SD_OCR_VDD_HIGH                 0x00FF8000  /* VDD voltage 2.7 ~ 3.6 */
#define SD_OCR_VDD_LOW                  0x00007F80  /* VDD voltage 1.65 ~ 2.7 */
#define SD_OCR_VDD                      (SD_OCR_VDD_HIGH | SD_OCR_VDD_LOW)
#define SD_OCR_S18R                     (1 << 24)   /* 1.8V switching request */
#define SD_ROCR_S18A                    SD_OCR_S18R /* 1.8V switching accepted by card */
#define SD_OCR_XPC                      (1 << 28)   /* SDXC power control */
#define SD_OCR_CCS                      (1 << 30)   /* Card Capacity Status */

/* Standard MMC commands (4.1)                  type  argument     response */
   /* class 1 */
#define MMC_GO_IDLE_STATE               0    /* bc                          */
#define MMC_GO_IDLE_STATE_OP            (SDIO_RESP_NONE)
#define MMC_SEND_OP_COND                1    /* bcr  [31:0] OCR         R3  */
#define MMC_SEND_OP_COND_OP             (SDIO_RESP_R3)
#define MMC_ALL_SEND_CID                2    /* bcr                     R2  */
#define MMC_ALL_SEND_CID_OP             (SDIO_RESP_R2)
#define MMC_SET_RELATIVE_ADDR           3    /* ac   [31:16] RCA        R1  */
#define MMC_SET_RELATIVE_ADDR_OP        (SDIO_RESP_R1)
#define MMC_SET_DSR                     4    /* bc   [31:16] RCA            */
#define MMC_SET_DSR_OP                  (SDIO_RESP_NONE)
#define MMC_SLEEP_AWAKE                 5    /* ac   [31:16] RCA 15:flg R1b */
#define MMC_SLEEP_AWAKE_OP              (SDIO_RESP_R1B)
#define MMC_SWITCH                      6    /* ac   [31:0] See below   R1b */
#define MMC_SWITCH_OP                   (SDIO_RESP_R1B)
#define MMC_SELECT_CARD                 7    /* ac   [31:16] RCA        R1  */
#define MMC_SELECT_CARD_OP              (SDIO_RESP_R1)
#define MMC_SEND_EXT_CSD                8    /* adtc                    R1  */
#define MMC_SEND_EXT_CSD_OP             (SDIO_RESP_R1)
#define MMC_SEND_CSD                    9    /* ac   [31:16] RCA        R2  */
#define MMC_SEND_CSD_OP                 (SDIO_RESP_R2)
#define MMC_SEND_CID                    10   /* ac   [31:16] RCA        R2  */
#define MMC_SEND_CID_OP                 (SDIO_RESP_R2)
#define MMC_READ_DAT_UNTIL_STOP         11   /* adtc [31:0] dadr        R1  */
#define MMC_READ_DAT_UNTIL_STOP_OP      (SDIO_RESP_R1)
#define MMC_STOP_TRANSMISSION           12   /* ac                      R1b */
#define MMC_STOP_TRANSMISSION_OP        (SDIO_RESP_R1B | SDIO_CMDOP_TRANS_STOP)
#define MMC_SEND_STATUS                 13   /* ac   [31:16] RCA        R1  */
#define MMC_SEND_STATUS_OP              (SDIO_RESP_R1)
#define MMC_BUS_TEST_R                  14   /* adtc                    R1  */
#define MMC_BUS_TEST_R_OP               (SDIO_RESP_R1)
#define MMC_GO_INACTIVE_STATE           15   /* ac   [31:16] RCA            */
#define MMC_BUS_TEST_W                  19   /* adtc                    R1  */
#define MMC_BUS_TEST_W_OP               (SDIO_RESP_R1)
#define MMC_SPI_READ_OCR                58   /* spi                  spi_R3 */
#define MMC_SPI_READ_OCR_OP             (SDIO_RESP_SPI_R3)
#define MMC_SPI_CRC_ON_OFF              59   /* spi  [0:0] flag      spi_R1 */
#define MMC_SPI_CRC_ON_OFF_OP           (SDIO_RESP_SPI_R1)

  /* class 2 */
#define MMC_SET_BLOCKLEN                16   /* ac   [31:0] block len   R1  */
#define MMC_SET_BLOCKLEN_OP             (SDIO_RESP_R1)
#define MMC_READ_SINGLE_BLOCK           17   /* adtc [31:0] data addr   R1  */
#define MMC_READ_SINGLE_BLOCK_OP        (SDIO_RESP_R1 | SDIO_CMDOP_SINGLE_BLOCK | SDIO_CMDOP_READ)
#define MMC_READ_MULTIPLE_BLOCK         18   /* adtc [31:0] data addr   R1  */
#define MMC_READ_MULTIPLE_BLOCK_OP      (SDIO_RESP_R1 | SDIO_CMDOP_MULTI_BLOCK | SDIO_CMDOP_READ)
#define MMC_SEND_TUNING_BLOCK           19   /* adtc                    R1  */
#define MMC_SEND_TUNING_BLOCK_OP        (SDIO_RESP_R1)
#define MMC_SEND_TUNING_BLOCK_HS200     21   /* adtc                    R1  */
#define MMC_SEND_TUNING_BLOCK_HS200_OP  (SDIO_RESP_R1)

  /* class 3 */
#define MMC_WRITE_DAT_UNTIL_STOP        20   /* adtc [31:0] data addr   R1  */
#define MMC_WRITE_DAT_UNTIL_STOP_OP     (SDIO_RESP_R1)

  /* class 4 */
#define MMC_SET_BLOCK_COUNT             23   /* adtc [31:0] data addr   R1  */
#define MMC_SET_BLOCK_COUNT_OP          (SDIO_RESP_R1)
#define MMC_WRITE_BLOCK                 24   /* adtc [31:0] data addr   R1  */
#define MMC_WRITE_BLOCK_OP              (SDIO_RESP_R1 | SDIO_CMDOP_SINGLE_BLOCK | SDIO_CMDOP_WRITE)
#define MMC_WRITE_MULTIPLE_BLOCK        25   /* adtc                    R1  */
#define MMC_WRITE_MULTIPLE_BLOCK_OP     (SDIO_RESP_R1 | SDIO_CMDOP_MULTI_BLOCK | SDIO_CMDOP_WRITE)
#define MMC_PROGRAM_CID                 26   /* adtc                    R1  */
#define MMC_PROGRAM_CID_OP              (SDIO_RESP_R1)
#define MMC_PROGRAM_CSD                 27   /* adtc                    R1  */
#define MMC_PROGRAM_CSD_OP              (SDIO_RESP_R1)

  /* class 6 */
#define MMC_SET_WRITE_PROT              28   /* ac   [31:0] data addr   R1b */
#define MMC_SET_WRITE_PROT_OP           (SDIO_RESP_R1B)
#define MMC_CLR_WRITE_PROT              29   /* ac   [31:0] data addr   R1b */
#define MMC_CLR_WRITE_PROT_OP           (SDIO_RESP_R1B)
#define MMC_SEND_WRITE_PROT             30   /* adtc [31:0] wpdata addr R1  */
#define MMC_SEND_WRITE_PROT_OP          (SDIO_RESP_R1)

  /* class 5 */
#define MMC_ERASE_GROUP_START           35   /* ac   [31:0] data addr   R1  */
#define MMC_ERASE_GROUP_START_OP        (SDIO_RESP_R1)
#define MMC_ERASE_GROUP_END             36   /* ac   [31:0] data addr   R1  */
#define MMC_ERASE_GROUP_END_OP          (SDIO_RESP_R1)
#define MMC_ERASE                       38   /* ac                      R1b */
#define MMC_ERASE_OP                    (SDIO_RESP_R1B)

  /* class 9 */
#define MMC_FAST_IO                     39   /* ac   <Complex>          R4  */
#define MMC_FAST_IO_OP                  (SDIO_RESP_R4)
#define MMC_GO_IRQ_STATE                40   /* bcr                     R5  */
#define MMC_GO_IRQ_STATE_OP             (SDIO_RESP_R5)

  /* class 7 */
#define MMC_LOCK_UNLOCK                 42   /* adtc                    R1b */
#define MMC_LOCK_UNLOCK_OP              (SDIO_RESP_R1B)

  /* class 8 */
#define MMC_APP_CMD                     55   /* ac   [31:16] RCA        R1  */
#define MMC_APP_CMD_OP                  (SDIO_RESP_R1)
#define MMC_GEN_CMD                     56   /* adtc [0] RD/WR          R1  */
#define MMC_GEN_CMD_OP                  (SDIO_RESP_R1)

  /* class 11 */
#define MMC_QUE_TASK_PARAMS             44   /* ac   [20:16] task id    R1  */
#define MMC_QUE_TASK_PARAMS_OP          (SDIO_RESP_R1)
#define MMC_QUE_TASK_ADDR               45   /* ac   [31:0] data addr   R1  */
#define MMC_QUE_TASK_ADDR_OP            (SDIO_RESP_R1)
#define MMC_EXECUTE_READ_TASK           46   /* adtc [20:16] task id    R1  */
#define MMC_EXECUTE_READ_TASK_OP        (SDIO_RESP_R1)
#define MMC_EXECUTE_WRITE_TASK          47   /* adtc [20:16] task id    R1  */
#define MMC_EXECUTE_WRITE_TASK_OP       (SDIO_RESP_R1)
#define MMC_CMDQ_TASK_MGMT              48   /* ac   [20:16] task id    R1b */
#define MMC_CMDQ_TASK_MGMT_OP           (SDIO_RESP_R1B)

/* SD/MMC version bits; 8 flags, 8 major, 8 minor, 8 change */
#define SD_VERSION_SD                   (1U << 31)
#define MMC_VERSION_MMC                 (1U << 30)

#define MAKE_SDMMC_VERSION(a, b, c)                                             \
    ((((uint32_t)(a)) << 16) | ((uint32_t)(b) << 8) | (uint32_t)(c))
#define MAKE_SD_VERSION(a, b, c)                                                \
    (SD_VERSION_SD | MAKE_SDMMC_VERSION(a, b, c))
#define MAKE_MMC_VERSION(a, b, c)                                               \
    (MMC_VERSION_MMC | MAKE_SDMMC_VERSION(a, b, c))

#define EXTRACT_SDMMC_MAJOR_VERSION(x)                                          \
    (((uint32_t)(x) >> 16) & 0xff)
#define EXTRACT_SDMMC_MINOR_VERSION(x)                                          \
    (((uint32_t)(x) >> 8) & 0xff)
#define EXTRACT_SDMMC_CHANGE_VERSION(x)                                         \
    ((uint32_t)(x) & 0xff)

#define SD_VERSION_3                    MAKE_SD_VERSION(3, 0, 0)
#define SD_VERSION_2                    MAKE_SD_VERSION(2, 0, 0)
#define SD_VERSION_1_0                  MAKE_SD_VERSION(1, 0, 0)
#define SD_VERSION_1_10                 MAKE_SD_VERSION(1, 10, 0)

#define MMC_VERSION_UNKNOWN             MAKE_MMC_VERSION(0, 0, 0)
#define MMC_VERSION_1_2                 MAKE_MMC_VERSION(1, 2, 0)
#define MMC_VERSION_1_4                 MAKE_MMC_VERSION(1, 4, 0)
#define MMC_VERSION_2_2                 MAKE_MMC_VERSION(2, 2, 0)
#define MMC_VERSION_3                   MAKE_MMC_VERSION(3, 0, 0)
#define MMC_VERSION_4                   MAKE_MMC_VERSION(4, 0, 0)
#define MMC_VERSION_4_1                 MAKE_MMC_VERSION(4, 1, 0)
#define MMC_VERSION_4_2                 MAKE_MMC_VERSION(4, 2, 0)
#define MMC_VERSION_4_3                 MAKE_MMC_VERSION(4, 3, 0)
#define MMC_VERSION_4_4                 MAKE_MMC_VERSION(4, 4, 0)
#define MMC_VERSION_4_41                MAKE_MMC_VERSION(4, 4, 1)
#define MMC_VERSION_4_5                 MAKE_MMC_VERSION(4, 5, 0)
#define MMC_VERSION_5_0                 MAKE_MMC_VERSION(5, 0, 0)
#define MMC_VERSION_5_1                 MAKE_MMC_VERSION(5, 1, 0)

#define IS_SD(x)                        ((x) & SD_VERSION_SD)
#define IS_MMC(x)                       ((x) & MMC_VERSION_MMC)

// r1 response card status
#define R1_OUT_OF_RANGE                 (1 << 31)   /* er,  c */
#define R1_ADDRESS_ERROR                (1 << 30)   /* erx, c */
#define R1_BLOCK_LEN_ERROR              (1 << 29)   /* er,  c */
#define R1_ERASE_SEQ_ERROR              (1 << 28)   /* er,  c */
#define R1_ERASE_PARAM                  (1 << 27)   /* ex,  c */
#define R1_WP_VIOLATION                 (1 << 26)   /* erx, c */
#define R1_CARD_IS_LOCKED               (1 << 25)   /* sx,  a */
#define R1_LOCK_UNLOCK_FAILED           (1 << 24)   /* erx, c */
#define R1_COM_CRC_ERROR                (1 << 23)   /* er,  b */
#define R1_ILLEGAL_COMMAND              (1 << 22)   /* er,  b */
#define R1_CARD_ECC_FAILED              (1 << 21)   /* ex,  c */
#define R1_CC_ERROR                     (1 << 20)   /* erx, c */
#define R1_ERROR                        (1 << 19)   /* erx, c */
#define R1_UNDERRUN                     (1 << 18)   /* ex,  c */
#define R1_OVERRUN                      (1 << 17)   /* ex,  c */
#define R1_CID_CSD_OVERWRITE            (1 << 16)   /* erx, c, CID/CSD overwrite */
#define R1_WP_ERASE_SKIP                (1 << 15)   /* sx,  c */
#define R1_CARD_ECC_DISABLED            (1 << 14)   /* sx,  a */
#define R1_ERASE_RESET                  (1 << 13)   /* sr,  c */
#define R1_STATUS(x)                    (x & 0xFFF9A000)
#define R1_CURRENT_STATE(x)             ((x & 0x00001E00) >> 9) /* sx, b (4 bits) */
#define R1_READY_FOR_DATA               (1 << 8)    /* sx,  a */
#define R1_SWITCH_ERROR                 (1 << 7)    /* sx,  c */
#define R1_EXCEPTION_EVENT              (1 << 6)    /* sr,  a */
#define R1_APP_CMD                      (1 << 5)    /* sr,  c */

#define R1_STATE_IDLE                   0
#define R1_STATE_READY                  1
#define R1_STATE_IDENT                  2
#define R1_STATE_STBY                   3
#define R1_STATE_TRAN                   4
#define R1_STATE_DATA                   5
#define R1_STATE_RCV                    6
#define R1_STATE_PRG                    7
#define R1_STATE_DIS                    8
#define R1_STATE_MASK                   0x0FUL

#define R1_CUR_STATE(__S)               ((__S) << 9)

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief SDIO API template, used to generate SDIO type, specific prefix
 * function declarations, etc.
 * @param[in] __prefix_name The prefix used for generating SDIO functions.
 * \~chinese
 * @brief SDIO API 模板，用于生成 SDIO 类型、特定前缀的函数声明等。
 * @param[in] __prefix_name 用于生成 SDIO 函数的前缀。
 */
#define VSF_SDIO_APIS(__prefix_name)                                                                                                                                     \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             sdio, init,                 VSF_MCONNECT(__prefix_name, _t) *sdio_ptr, vsf_sdio_cfg_t *cfg_ptr)         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  sdio, fini,                 VSF_MCONNECT(__prefix_name, _t) *sdio_ptr)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             sdio, get_configuration,    VSF_MCONNECT(__prefix_name, _t) *sdio_ptr, vsf_sdio_cfg_t *cfg_ptr)         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  sdio, irq_enable,           VSF_MCONNECT(__prefix_name, _t) *sdio_ptr, vsf_sdio_irq_mask_t irq_mask)    \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  sdio, irq_disable,          VSF_MCONNECT(__prefix_name, _t) *sdio_ptr, vsf_sdio_irq_mask_t irq_mask)    \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_sdio_status_t,     sdio, status,               VSF_MCONNECT(__prefix_name, _t) *sdio_ptr)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_sdio_capability_t, sdio, capability,           VSF_MCONNECT(__prefix_name, _t) *sdio_ptr)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             sdio, set_clock,            VSF_MCONNECT(__prefix_name, _t) *sdio_ptr, uint32_t clock_hz, bool is_ddr)  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             sdio, set_bus_width,        VSF_MCONNECT(__prefix_name, _t) *sdio_ptr, uint8_t bus_width)               \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             sdio, host_request,         VSF_MCONNECT(__prefix_name, _t) *sdio_ptr, vsf_sdio_req_t *req)

/*============================ TYPES =========================================*/

typedef union vsf_sdio_csd_t {
//               name               bitlen      offset
// refer to: Part_1_Physical_Layer_Specification_Ver3.01_Final_100218.pdf
    struct {
        uint32_t ALWAY1             : 1;        //  0
        uint32_t CRC                : 7;        //  1

        // different part from mmc
        uint32_t                    : 2;        //  8

        uint32_t FILE_FORMAT        : 2;        //  10
        uint32_t TMP_WRITE_PROTECT  : 1;        //  12
        uint32_t PERM_WRITE_PROTECT : 1;        //  13
        uint32_t COPY               : 1;        //  14
        uint32_t FILE_FORMAT_GRP    : 1;        //  15

        // different part from mmc
        uint32_t                    : 5;        //  16

        uint32_t WRITE_BL_PARTIAL   : 1;        //  21
        uint32_t WRITE_BL_LEN       : 4;        //  22
        uint32_t R2W_FACTOR         : 3;        //  26

        // different part from mmc
        uint32_t                    : 2;        //  29

        uint32_t WP_GRP_ENABLE      : 1;        //  31
        uint32_t WP_GRP_SIZE        : 7;        //  32
        uint32_t SECTOR_SIZE        : 7;        //  39
        uint32_t ERASE_BLK_EN       : 1;        //  46

        // different part from sd_v2
        uint32_t C_SIZE_MULT        : 3;        //  47
        uint32_t VDD_W_CURR_MAX     : 3;        //  50
        uint32_t VDD_W_CURR_MIN     : 3;        //  53
        uint32_t VDD_R_CURR_MAX     : 3;        //  56
        uint32_t VDD_R_CURR_MIN     : 3;        //  59
        uint32_t C_SIZE             : 12;       //  62
        uint32_t                    : 2;        //  74

        uint32_t DSR_IMP            : 1;        //  76
        uint32_t READ_BLK_MISALIGN  : 1;        //  77
        uint32_t WRITE_BLK_MISALIGN : 1;        //  78
        uint32_t READ_BL_PARTIAL    : 1;        //  79
        uint32_t READ_BL_LEN        : 4;        //  80
        uint32_t CCC                : 12;       //  84
        uint32_t TRANS_SPEED        : 8;        //  96
        uint32_t NSAC               : 8;        //  104
        uint32_t TAAC               : 8;        //  112

        // different part from mmc
        uint32_t                    : 6;        //  120

        uint32_t CSD_STRUCTURE      : 2;        //  126
    } VSF_CAL_PACKED sd_v1;
// refer to: Part_1_Physical_Layer_Specification_Ver3.01_Final_100218.pdf
    struct {
        uint32_t                    : 1;        //  0
        uint32_t CRC                : 7;        //  1

        // different part from mmc
        uint32_t                    : 2;        //  8

        uint32_t FILE_FORMAT        : 2;        //  10
        uint32_t TMP_WRITE_PROTECT  : 1;        //  12
        uint32_t PERM_WRITE_PROTECT : 1;        //  13
        uint32_t COPY               : 1;        //  14
        uint32_t FILE_FORMAT_GRP    : 1;        //  15

        // different part from mmc
        uint32_t                    : 5;        //  16

        uint32_t WRITE_BL_PARTIAL   : 1;        //  21
        uint32_t WRITE_BL_LEN       : 4;        //  22
        uint32_t R2W_FACTOR         : 3;        //  26

        // different part from mmc
        uint32_t                    : 2;        //  29

        uint32_t WP_GRP_ENABLE      : 1;        //  31
        uint32_t WP_GRP_SIZE        : 7;        //  32
        uint32_t SECTOR_SIZE        : 7;        //  39
        uint32_t ERASE_BLK_EN       : 1;        //  46

        // different part from sd_v1 and mmc
        uint32_t                    : 1;        //  47
        uint32_t C_SIZE             : 22;       //  48
        uint32_t                    : 6;        //  70

        uint32_t DSR_IMP            : 1;        //  76
        uint32_t READ_BLK_MISALIGN  : 1;        //  77
        uint32_t WRITE_BLK_MISALIGN : 1;        //  78
        uint32_t READ_BL_PARTIAL    : 1;        //  79
        uint32_t READ_BL_LEN        : 4;        //  80
        uint32_t CCC                : 12;       //  84
        uint32_t TRANS_SPEED        : 8;        //  96
        uint32_t NSAC               : 8;        //  104
        uint32_t TAAC               : 8;        //  112

        // different part from mmc
        uint32_t                    : 2;        //  120

        uint32_t CSD_STRUCTURE      : 2;        //  126
    } VSF_CAL_PACKED sd_v2;
// refer to: mmc specification
    struct {
        uint32_t                    : 1;        //  0
        uint32_t CRC                : 7;        //  1

        // different part from sd_v1 and sd_v2
        uint32_t ECC                : 2;        //  8

        uint32_t FILE_FORMAT        : 2;        //  10
        uint32_t TMP_WRITE_PROTECT  : 1;        //  12
        uint32_t PERM_WRITE_PROTECT : 1;        //  13
        uint32_t COPY               : 1;        //  14
        uint32_t FILE_FORMAT_GRP    : 1;        //  15

        // different part from sd_v1 and sd_v2
        uint32_t CONTENT_PROT_APP   : 1;        //  16
        uint32_t                    : 4;        //  17

        uint32_t WRITE_BL_PARTIAL   : 1;        //  21
        uint32_t WRITE_BL_LEN       : 4;        //  22
        uint32_t R2W_FACTOR         : 3;        //  26

        // different part from sd_v1 and sd_v2
        uint32_t DEFAULT_ECC        : 2;        //  29

        uint32_t WP_GRP_ENABLE      : 1;        //  31

        // different part from sd_v1 and sd_v2
        uint32_t WP_GRP_SIZE        : 5;        //  32
        uint32_t ERASE_GRP_MULT     : 5;        //  37
        uint32_t ERASE_GRP_SIZE     : 5;        //  42

        // different part from sd_v2
        uint32_t C_SIZE_MULT        : 3;        //  47
        uint32_t VDD_W_CURR_MAX     : 3;        //  50
        uint32_t VDD_W_CURR_MIN     : 3;        //  53
        uint32_t VDD_R_CURR_MAX     : 3;        //  56
        uint32_t VDD_R_CURR_MIN     : 3;        //  59
        uint32_t C_SIZE             : 12;       //  62
        uint32_t                    : 2;        //  74

        uint32_t DSR_IMP            : 1;        //  76
        uint32_t READ_BLK_MISALIGN  : 1;        //  77
        uint32_t WRITE_BLK_MISALIGN : 1;        //  78
        uint32_t READ_BL_PARTIAL    : 1;        //  79
        uint32_t READ_BL_LEN        : 4;        //  80
        uint32_t CCC                : 12;       //  84
        uint32_t TRANS_SPEED        : 8;        //  96
        uint32_t NSAC               : 8;        //  104
        uint32_t TAAC               : 8;        //  112

        // different part from sd_v1 and sd_v2
        uint32_t                    : 2;        //  120
        uint32_t SPEC_VERS          : 4;        //  122

        uint32_t CSD_STRUCTURE      : 2;        //  126
    } VSF_CAL_PACKED mmc;
} VSF_CAL_PACKED vsf_sdio_csd_t;

typedef struct vsf_sdio_cid_t {
// refer to: Part_1_Physical_Layer_Specification_Ver3.01_Final_100218.pdf
    uint64_t                        : 1;        //  0
    uint64_t CRC                    : 7;        //  1
    uint64_t MDT                    : 12;       //  8   Manufacturing date
    uint64_t                        : 4;        //  20
    uint64_t PSN                    : 32;       //  24  Product serial number
    uint64_t PRV                    : 8;        //  56  Product revision
    uint64_t PNM                    : 40;       //  64  Product name
    uint64_t OID                    : 16;       //  104 OEM/Application ID
    uint64_t MID                    : 8;        //  120 Manufacturer ID
} VSF_CAL_PACKED vsf_sdio_cid_t;

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
typedef enum vsf_sdio_mode_t {
    SDIO_MODE_HOST                  = (0x1ul << 0), // select host mode
    SDIO_MODE_SLAVE                 = (0x0ul << 0), // select slave mode
    SDIO_MODE_MASK                  = (0x1ul << 0),
} vsf_sdio_mode_t;
#endif

/**
 \~english
 @brief flags of sdio request operations
 @note vsf_sdio_reqop_t is implemented by specific driver.

 \~chinese
 @brief sdio 传输操作的标志
 @note vsf_sdio_reqop_t 由具体驱动实现。
 */
#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_REQOP == DISABLED
typedef enum vsf_sdio_reqop_t {
    SDIO_CMDOP_BYTE                 = (0ul << 0),
    SDIO_CMDOP_STREAM               = (1ul << 0),
    SDIO_CMDOP_SINGLE_BLOCK         = (2ul << 0),
    SDIO_CMDOP_MULTI_BLOCK          = (3ul << 0),

    SDIO_CMDOP_WRITE                = (1ul << 2),
    SDIO_CMDOP_READ                 = (0ul << 2),

    SDIO_CMDOP_RESP_BUSY            = (1ul << 3),
    // prefix __ means private, not mandatory, different names can be used according to different hw
    __SDIO_CMDOP_RESP               = (1ul << 4),
    __SDIO_CMDOP_RESP_SHORT         = (1ul << 5),
    __SDIO_CMDOP_RESP_SHORT_CRC     = (2ul << 5),
    __SDIO_CMDOP_RESP_LONG_CRC      = (3ul << 5),
    // SDIO_RESP_R1 etc are mandatory
#define SDIO_RESP_NONE              0
#define SDIO_RESP_R1                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)
#define SDIO_RESP_R1B               (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC | SDIO_CMDOP_RESP_BUSY)
#define SDIO_RESP_R2                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_LONG_CRC)
#define SDIO_RESP_R3                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT)
#define SDIO_RESP_R4                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT)
#define SDIO_RESP_R5                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)
#define SDIO_RESP_R6                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)
#define SDIO_RESP_R7                (__SDIO_CMDOP_RESP | __SDIO_CMDOP_RESP_SHORT_CRC)

    // used for CMD11(SD_SWITCH_VOLTAGE) only, hold clock after resp, ignore if no resp
    SDIO_CMDOP_CLKHOLD              = (1ul << 7),
    // used for CMD12(MMC_STOP_TRANSMISSION) only
    SDIO_CMDOP_TRANS_STOP           = (1ul << 8),
} vsf_sdio_reqop_t;
#endif

typedef struct vsf_sdio_req_t {
    uint8_t cmd;
    uint32_t arg;
    vsf_sdio_reqop_t op;

    // block_size will be 1 << block_size_bits
    uint8_t block_size_bits;
    uint8_t *buffer;
    uint32_t count;
} vsf_sdio_req_t;

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_sdio_irq_mask_t {
    // TODO: add irq mask for stream mode
    SDIO_IRQ_MASK_HOST_RESP_DONE    = (0x1ul <<  0),
    SDIO_IRQ_MASK_HOST_DATA_DONE    = (0x1ul <<  1),
    SDIO_IRQ_MASK_HOST_DATA_ABORT   = (0x1ul <<  2),        // aborted by CMD12
    SDIO_IRQ_MASK_HOST_ALL          = SDIO_IRQ_MASK_HOST_RESP_DONE
                                    | SDIO_IRQ_MASK_HOST_DATA_DONE
                                    | SDIO_IRQ_MASK_HOST_DATA_ABORT,
} vsf_sdio_irq_mask_t;
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_REQSTS == DISABLED
typedef enum vsf_sdio_reqsts_t {
    SDIO_REQSTS_DONE                = 0,
    SDIO_REQSTS_ERR_RESP_NONE       = (0x1ul <<  0),
    SDIO_REQSTS_ERR_RESP_CRC        = (0x1ul <<  1),
    SDIO_REQSTS_ERR_DATA_CRC        = (0x1ul <<  2),
    SDIO_REQSTS_DATA_BUSY           = (0x1ul <<  3),
    SDIO_REQSTS_BUSY                = (0x1ul <<  4),
    SDIO_REQSTS_ERR_MASK            = SDIO_REQSTS_ERR_RESP_NONE
                                    | SDIO_REQSTS_ERR_RESP_CRC
                                    | SDIO_REQSTS_ERR_DATA_CRC,
} vsf_sdio_reqsts_t;
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_sdio_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        vsf_sdio_reqsts_t req_status;
        vsf_sdio_irq_mask_t irq_status;
    };
} vsf_sdio_status_t;
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
typedef struct vsf_sdio_capability_t {
#if VSF_SDIO_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    enum {
        SDIO_CAP_BUS_WIDTH_1        = (0x1ul <<  0),
        SDIO_CAP_BUS_WIDTH_4        = (0x1ul <<  1),
        SDIO_CAP_BUS_WIDTH_8        = (0x1ul <<  2),
    } bus_width;
    uint32_t max_freq_hz;
    // data alignment(address and size) in bytes
    uint16_t data_ptr_alignment;    // alignment of data pointer
    uint16_t data_size_alignment;   // alignment of data size
    bool support_ddr;
} vsf_sdio_capability_t;
#endif

#if VSF_SDIO_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
typedef struct vsf_sdio_t vsf_sdio_t;

/**
 \~english
 @brief sdio interrupt callback function prototype.
 @note It is called after interrupt occurs.

 @param target_ptr pointer of user.
 @param sdio_ptr pointer of sdio instance.
 @param irq_mask one or more value of enum vsf_sdio_irq_mask_t
 @param status request status.
 @param resp response.
 @return None.

 \~chinese
 @brief sdio 中断回调函数原型
 @note 这个回调函数在中断发生之后被调用

 @param target_ptr 用户指针
 @param sdio_ptr sdio 实例的指针
 @param irq_mask 一个或者多个枚举 vsf_sdio_irq_mask_t 的值的按位或
 @param status 传输状态
 @param resp 应答
 @return 无

 \~
 \code {.c}
    static void __user_sdio_irchandler(void *target_ptr, vsf_sdio_t *sdio_ptr,
        vsf_sdio_irq_mask_t irq_mask, vsf_sdio_reqsts_t req_status, uint32_t resp[4])
    {
        if (irq_mask & SDIO_IRQ_MASK_HOST_RESP_DONE) {
            // do something
        }
    }
 \endcode
 */
typedef void vsf_sdio_isr_handler_t(void *target_ptr,
                                   vsf_sdio_t *sdio_ptr,
                                   vsf_sdio_irq_mask_t irq_mask,
                                   vsf_sdio_reqsts_t status,
                                   uint32_t resp[4]);

/**
 \~english
 @brief sdio interrupt configuration

 \~chinese
 @brief sdio 中断配置
 */
typedef struct vsf_sdio_isr_t {
    vsf_sdio_isr_handler_t *handler_fn;          //!< \~english TODO
                                                //!< \~chinese 中断回调函数
    void                  *target_ptr;          //!< \~english pointer of user target
                                                //!< \~chinese 用户传入的指针
    vsf_arch_prio_t        prio;                //!< \~english interrupt priority
                                                //!< \~chinesh 中断优先级
} vsf_sdio_isr_t;

/**
 \~english
 @brief sdio configuration

 \~chinese
 @brief sdio 配置
 */
typedef struct vsf_sdio_cfg_t {
    vsf_sdio_mode_t mode;                     //!< \~english sdio mode \ref vsf_sdio_mode_t
                                                //!< \~chinese sdio 模式 \ref vsf_sdio_mode_t
    vsf_sdio_isr_t isr;                          //!< \~english sdio interrupt
                                                //!< \~chinese sdio 中断
} vsf_sdio_cfg_t;
#endif

typedef struct vsf_sdio_op_t {
/// @cond
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_SDIO_APIS(vsf_sdio)
} vsf_sdio_op_t;

#if VSF_SDIO_CFG_MULTI_CLASS == ENABLED
struct vsf_sdio_t  {
    const vsf_sdio_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief Initialize a sdio instance.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_sdio_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if sdio was initialized, or a negative error code

 @note It is not necessary to call vsf_sdio_fini() to deinitialization.
       vsf_sdio_init() should be called before any other sdio API except vsf_sdio_capability().

 \~chinese
 @brief 初始化一个 sdio 实例
 @param[in] sdio_ptr: 指向结构体 @ref vsf_sdio_t 的指针
 @param[in] cfg_ptr: 指向结构体 @ref vsf_sdio_cfg_t 的指针
 @return vsf_err_t: 如果 sdio 初始化成功返回 VSF_ERR_NONE , 失败返回负数

 @note 失败后不需要调用 vsf_sdio_fini() 反初始化。
       vsf_sdio_init() 应该在除 vsf_sdio_capability() 之外的其他 sdio API 之前调用。
 */
extern vsf_err_t vsf_sdio_init(vsf_sdio_t *sdio_ptr, vsf_sdio_cfg_t *cfg_ptr);

/**
 \~english
 @brief Finalize a sdio instance.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @return none

 \~chinese
 @brief 结束一个 sdio 实例
 @param[in] sdio_ptr: 指向结构体 @ref vsf_sdio_t 的指针
 @return 无
 */
extern void vsf_sdio_fini(vsf_sdio_t *sdio_ptr);

/**
 * \~english
 * @brief Get the current configuration of an SDIO instance
 * @param[in] sdio_ptr: pointer to structure @ref vsf_sdio_t
 * @param[out] cfg_ptr: pointer to configuration structure @ref vsf_sdio_cfg_t to store the current configuration
 * @return vsf_err_t: VSF_ERR_NONE if successful, otherwise return error code
 *
 * @note This function retrieves the current SDIO configuration including mode and interrupt settings.
 *       The SDIO instance should be initialized before calling this function.
 *
 * \~chinese
 * @brief 获取 SDIO 实例的当前配置
 * @param[in] sdio_ptr: 指向结构体 @ref vsf_sdio_t 的指针
 * @param[out] cfg_ptr: 指向配置结构体 @ref vsf_sdio_cfg_t 的指针，用于存储当前配置
 * @return vsf_err_t: 如果成功返回 VSF_ERR_NONE，否则返回错误码
 *
 * @note 此函数检索当前 SDIO 配置，包括模式和中断设置。
 *       调用此函数前应先初始化 SDIO 实例。
 */
extern vsf_err_t vsf_sdio_get_configuration(vsf_sdio_t *sdio_ptr, vsf_sdio_cfg_t *cfg_ptr);

/**
 \~english
 @brief Enable interrupt masks of sdio instance.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] irq_mask: one or more value of enum @ref vsf_sdio_irq_mask_t
 @return none.
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 启用 sdio 实例的中断
 @param[in] sdio_ptr: 指向结构体 @ref vsf_sdio_t 的指针
 @param[in] irq_mask: 一个或者多个枚举 vsf_i2c_irq_mask_t 的值的按位或，@ref vsf_sdio_irq_mask_t
 @return 无
 @note 在中断启用之前，应该清除所有挂起的中断。
 */
extern void vsf_sdio_irq_enable(vsf_sdio_t *sdio_ptr, vsf_sdio_irq_mask_t irq_mask);

/**
 \~english
 @brief Disable interrupt masks of sdio instance.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] irq_mask: one or more value of enum vsf_sdio_irq_mask_t, @ref vsf_sdio_irq_mask_t
 @return none.

 \~chinese
 @brief 禁用 sdio 实例的中断
 @param[in] sdio_ptr: 指向结构体 @ref vsf_sdio_t 的指针
 @param[in] irq_mask: 一个或者多个枚举 vsf_sdio_irq_mask_t 的值的按位或，@ref vsf_sdio_irq_mask_t
 @return 无
 */
extern void vsf_sdio_irq_disable(vsf_sdio_t *sdio_ptr, vsf_sdio_irq_mask_t irq_mask);

/**
 \~english
 @brief Get the status of sdio instance.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @return vsf_sdio_status_t: return all status of current sdio

 \~chinese
 @brief 获取 sdio 实例状态
 @param[in] sdio_ptr: 指向结构体 @ref vsf_sdio_t 的指针
 @return vsf_sdio_status_t: 返回当前 sdio 的所有状态
 */
extern vsf_sdio_status_t vsf_sdio_status(vsf_sdio_t *sdio_ptr);

/**
 \~english
 @brief Get the capability of sdio instance.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @return vsf_sdio_capability_t: return all capability of current sdio @ref vsf_sdio_capability_t

 \~chinese
 @brief 获取 sdio 实例能力
 @param[in] sdio_ptr: 指向结构体 @ref vsf_sdio_t 的指针
 @return vsf_sdio_capability_t: 返回当前 sdio 的所有能力 @ref vsf_sdio_capability_t
 */
extern vsf_sdio_capability_t vsf_sdio_capability(vsf_sdio_t *sdio_ptr);

/**
 \~english
 @brief Set the clock of sdio instance.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] clock_hz: clock in Hz
 @param[in] is_ddr: is DDR mode, true means DDR mode while false means SDR mode
 @return vsf_err_t: VSF_ERR_NONE if sdio set clock was successfully, or a negative error code

 \~chinese
 @brief 设置 sdio 时钟
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] clock_hz: 时钟速度（单位：赫兹）
 @param[in] is_ddr: 是否是 DDR 模式，true 代表 DDR 模式，false 代表 SDR 模式
 @return vsf_err_t: 如果 sdio 设置时钟成功返回 VSF_ERR_NONE，否则返回负数
 */
extern vsf_err_t vsf_sdio_set_clock(vsf_sdio_t *sdio_ptr, uint32_t clock_hz, bool is_ddr);

/**
 \~english
 @brief Set the bus width of sdio instance.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] bus_width: bus width in 1, 4, 8
 @return vsf_err_t: VSF_ERR_NONE if sdio was successfully, or a negative error code

 \~chinese
 @brief 设置 sdio 总线位宽
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] bus_width: 总线位宽，范围：1, 4, 8
 @return vsf_err_t: 如果 sdio 设置总线宽度成功返回 VSF_ERR_NONE，否则返回负数
 */
extern vsf_err_t vsf_sdio_set_bus_width(vsf_sdio_t *sdio_ptr, uint8_t bus_width);

/**
 \~english
 @brief Start a new request for sdio instance.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] req: pointer to structure @ref vsf_sdio_req_t
 @return vsf_err_t: VSF_ERR_NONE if sdio was successfully, or a negative error code

 \~chinese
 @brief 启动 sdio 请求
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] req: sdio 请求结构指针
 @return vsf_err_t: 如果 sdio 主机传输开始返回 VSF_ERR_NONE，否则返回负数
 */
extern vsf_err_t vsf_sdio_host_request(vsf_sdio_t *sdio_ptr, vsf_sdio_req_t *req);

/**
 \~english
 @brief Set the single voltage mode of sdio instance.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] bus_width: bus width in 1, 4, 8
 @return vsf_err_t: VSF_ERR_NONE if sdio was successfully, or a negative error code

 \~chinese
 @brief 设置 sdio 单电压模式
 @param[in] sdio_ptr: 指向结构体 @ref vsf_sdio_t 的指针
 @param[in] bus_width: 总线位宽，范围：1, 4, 8
 @return vsf_err_t: 如果 sdio 设置成功返回 VSF_ERR_NONE，否则返回负数
 */
extern vsf_err_t vsf_sdio_single_voltage(vsf_sdio_t *sdio_ptr, uint8_t bus_width);

// TODO: add APIs for stream mode

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_SDIO_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_sdio_t                             VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_t)
#   define vsf_sdio_init(__SDIO, ...)               VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_init)               ((__vsf_sdio_t *)(__SDIO), ##__VA_ARGS__)
#   define vsf_sdio_get_configuration(__SDIO, ...)  VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_get_configuration)  ((__vsf_sdio_t *)(__SDIO), ##__VA_ARGS__)
#   define vsf_sdio_enable(__SDIO)                  VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_enable)             ((__vsf_sdio_t *)(__SDIO))
#   define vsf_sdio_disable(__SDIO)                 VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_disable)            ((__vsf_sdio_t *)(__SDIO))
#   define vsf_sdio_irq_enable(__SDIO, ...)         VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_irq_enable)         ((__vsf_sdio_t *)(__SDIO), ##__VA_ARGS__)
#   define vsf_sdio_irq_disable(__SDIO, ...)        VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_irq_disable)        ((__vsf_sdio_t *)(__SDIO), ##__VA_ARGS__)
#   define vsf_sdio_status(__SDIO)                  VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_status)             ((__vsf_sdio_t *)(__SDIO))
#   define vsf_sdio_capability(__SDIO)              VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_capability)         ((__vsf_sdio_t *)(__SDIO))
#   define vsf_sdio_set_clock(__SDIO, ...)          VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_set_clock)          ((__vsf_sdio_t *)(__SDIO), ##__VA_ARGS__)
#   define vsf_sdio_set_bus_width(__SDIO, ...)      VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_set_bus_width)      ((__vsf_sdio_t *)(__SDIO), ##__VA_ARGS__)
#   define vsf_sdio_host_request(__SDIO, ...)       VSF_MCONNECT(VSF_SDIO_CFG_PREFIX, _sdio_host_request)       ((__vsf_sdio_t *)(__SDIO), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_SDIO_H__*/
