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

#ifndef __HAL_DRIVER_MMC_INTERFACE_H__
#define __HAL_DRIVER_MMC_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// application code can redefine it
#ifndef VSF_MMC_CFG_PREFIX
#   if defined(VSF_HW_MMC_COUNT) && (VSF_HW_MMC_COUNT != 0)
#       define VSF_MMC_CFG_PREFIX               vsf_hw
#   else
#       define VSF_MMC_CFG_PREFIX               vsf
#   endif
#endif

// multi-class support enabled by default for maximum availability.
#ifndef VSF_MMC_CFG_MULTI_CLASS
#   define VSF_MMC_CFG_MULTI_CLASS              ENABLED
#endif

#ifndef VSF_MMC_CFG_FUNCTION_RENAME
#   define VSF_MMC_CFG_FUNCTION_RENAME          ENABLED
#endif

#ifndef VSF_MMC_CFG_REIMPLEMENT_FEATURE
#   define VSF_MMC_CFG_REIMPLEMENT_FEATURE      DISABLED
#endif

#ifndef VSF_MMC_CFG_REIMPLEMENT_IRQ_MASK
#   define VSF_MMC_CFG_REIMPLEMENT_IRQ_MASK     DISABLED
#endif

#ifndef VSF_MMC_CFG_REIMPLEMENT_STATUS
#   define VSF_MMC_CFG_REIMPLEMENT_STATUS       DISABLED
#endif

#ifndef VSF_MMC_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_MMC_CFG_REIMPLEMENT_CAPABILITY   DISABLED
#endif

/* SD commands                                  type  argument     response */
  /* class 0 */
/* This is basically the same command as for MMC with some quirks. */
#define SD_SEND_RELATIVE_ADDR           3   /* bcr                      R6  */
#define SD_SEND_RELATIVE_ADDR_OP        (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define SD_SEND_IF_COND                 8   /* bcr  [11:0] See below    R7  */
#define SD_SEND_IF_COND_OP              (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define SD_SWITCH_VOLTAGE               11  /* ac                       R1  */
#define SD_SWITCH_VOLTAGE_OP            (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 10 */
#define SD_SWITCH                       6   /* adtc [31:0] See below    R1  */
#define SD_SWITCH_OP                    (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 5 */
#define SD_ERASE_WR_BLK_START           32   /* ac   [31:0] data addr   R1  */
#define SD_ERASE_WR_BLK_START_OP        (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define SD_ERASE_WR_BLK_END             33   /* ac   [31:0] data addr   R1  */
#define SD_ERASE_WR_BLK_END_OP          (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* Application commands */
#define SD_APP_SET_BUS_WIDTH            6   /* ac   [1:0] bus width     R1  */
#define SD_APP_SET_BUS_WIDTH_OP         (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define SD_APP_SD_STATUS                13   /* adtc                    R1  */
#define SD_APP_SD_STATUS_OP             (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define SD_APP_SEND_NUM_WR_BLKS         22   /* adtc                    R1  */
#define SD_APP_SEND_NUM_WR_BLKS_OP      (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define SD_APP_OP_COND                  41   /* bcr  [31:0] OCR         R3  */
#define SD_APP_OP_COND_OP               (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT)
#define SD_APP_SEND_SCR                 51   /* adtc                    R1  */
#define SD_APP_SEND_SCR_OP              (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 11 */
#define SD_READ_EXTR_SINGLE             48   /* adtc [31:0]             R1  */
#define SD_READ_EXTR_SINGLE_OP          (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define SD_WRITE_EXTR_SINGLE            49   /* adtc [31:0]             R1  */
#define SD_WRITE_EXTR_SINGLE_OP         (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

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
#define SD_OCR_S18R		                (1 << 24)   /* 1.8V switching request */
#define SD_ROCR_S18A		            SD_OCR_S18R /* 1.8V switching accepted by card */
#define SD_OCR_XPC		                (1 << 28)   /* SDXC power control */
#define SD_OCR_CCS		                (1 << 30)   /* Card Capacity Status */

/* Standard MMC commands (4.1)                  type  argument     response */
   /* class 1 */
#define MMC_GO_IDLE_STATE               0    /* bc                          */
#define MMC_GO_IDLE_STATE_OP            0
#define MMC_SEND_OP_COND                1    /* bcr  [31:0] OCR         R3  */
#define MMC_SEND_OP_COND_OP             (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT)
#define MMC_ALL_SEND_CID                2    /* bcr                     R2  */
#define MMC_ALL_SEND_CID_OP             (MMC_CMDOP_RESP | MMC_CMDOP_RESP_LONG_CRC)
#define MMC_SET_RELATIVE_ADDR           3    /* ac   [31:16] RCA        R1  */
#define MMC_SET_RELATIVE_ADDR_OP        (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_SET_DSR                     4    /* bc   [31:16] RCA            */
#define MMC_SLEEP_AWAKE		            5    /* ac   [31:16] RCA 15:flg R1b */
#define MMC_SLEEP_AWAKE_OP              (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_SWITCH                      6    /* ac   [31:0] See below   R1b */
#define MMC_SWITCH_OP                   (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_SELECT_CARD                 7    /* ac   [31:16] RCA        R1  */
#define MMC_SELECT_CARD_OP              (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_SEND_EXT_CSD                8    /* adtc                    R1  */
#define MMC_SEND_EXT_CSD_OP             (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_SEND_CSD                    9    /* ac   [31:16] RCA        R2  */
#define MMC_SEND_CSD_OP                 (MMC_CMDOP_RESP | MMC_CMDOP_RESP_LONG_CRC)
#define MMC_SEND_CID                    10   /* ac   [31:16] RCA        R2  */
#define MMC_SEND_CID_OP                 (MMC_CMDOP_RESP | MMC_CMDOP_RESP_LONG_CRC)
#define MMC_READ_DAT_UNTIL_STOP         11   /* adtc [31:0] dadr        R1  */
#define MMC_READ_DAT_UNTIL_STOP_OP      (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_STOP_TRANSMISSION           12   /* ac                      R1b */
#define MMC_STOP_TRANSMISSION_OP        (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_SEND_STATUS                 13   /* ac   [31:16] RCA        R1  */
#define MMC_SEND_STATUS_OP              (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_BUS_TEST_R                  14   /* adtc                    R1  */
#define MMC_BUS_TEST_R_OP               (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_GO_INACTIVE_STATE           15   /* ac   [31:16] RCA            */
#define MMC_BUS_TEST_W                  19   /* adtc                    R1  */
#define MMC_BUS_TEST_W_OP               (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_SPI_READ_OCR                58   /* spi                  spi_R3 */
#define MMC_SPI_READ_OCR_OP             (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT)
#define MMC_SPI_CRC_ON_OFF              59   /* spi  [0:0] flag      spi_R1 */
#define MMC_SPI_CRC_ON_OFF_OP           (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 2 */
#define MMC_SET_BLOCKLEN                16   /* ac   [31:0] block len   R1  */
#define MMC_SET_BLOCKLEN_OP             (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_READ_SINGLE_BLOCK           17   /* adtc [31:0] data addr   R1  */
#define MMC_READ_SINGLE_BLOCK_OP        (MMC_CMDOP_SINGLE_BLOCK | MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_READ_MULTIPLE_BLOCK         18   /* adtc [31:0] data addr   R1  */
#define MMC_READ_MULTIPLE_BLOCK_OP      (MMC_CMDOP_MULTI_BLOCK | MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_SEND_TUNING_BLOCK           19   /* adtc                    R1  */
#define MMC_SEND_TUNING_BLOCK_OP        (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_SEND_TUNING_BLOCK_HS200     21	 /* adtc R1  */
#define MMC_SEND_TUNING_BLOCK_HS200_OP  (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 3 */
#define MMC_WRITE_DAT_UNTIL_STOP        20   /* adtc [31:0] data addr   R1  */
#define MMC_WRITE_DAT_UNTIL_STOP_OP     (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 4 */
#define MMC_SET_BLOCK_COUNT             23   /* adtc [31:0] data addr   R1  */
#define MMC_SET_BLOCK_COUNT_OP          (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_WRITE_BLOCK                 24   /* adtc [31:0] data addr   R1  */
#define MMC_WRITE_BLOCK_OP              (MMC_CMDOP_SINGLE_BLOCK | MMC_CMDOP_WRITE | MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_WRITE_MULTIPLE_BLOCK        25   /* adtc                    R1  */
#define MMC_WRITE_MULTIPLE_BLOCK_OP     (MMC_CMDOP_MULTI_BLOCK | MMC_CMDOP_WRITE | MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_PROGRAM_CID                 26   /* adtc                    R1  */
#define MMC_PROGRAM_CID_OP              (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_PROGRAM_CSD                 27   /* adtc                    R1  */
#define MMC_PROGRAM_CSD_OP              (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 6 */
#define MMC_SET_WRITE_PROT              28   /* ac   [31:0] data addr   R1b */
#define MMC_SET_WRITE_PROT_OP           (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_CLR_WRITE_PROT              29   /* ac   [31:0] data addr   R1b */
#define MMC_CLR_WRITE_PROT_OP           (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_SEND_WRITE_PROT             30   /* adtc [31:0] wpdata addr R1  */
#define MMC_SEND_WRITE_PROT_OP          (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 5 */
#define MMC_ERASE_GROUP_START           35   /* ac   [31:0] data addr   R1  */
#define MMC_ERASE_GROUP_START_OP        (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_ERASE_GROUP_END             36   /* ac   [31:0] data addr   R1  */
#define MMC_ERASE_GROUP_END_OP          (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_ERASE                       38   /* ac                      R1b */
#define MMC_ERASE_OP                    (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 9 */
#define MMC_FAST_IO                     39   /* ac   <Complex>          R4  */
#define MMC_FAST_IO_OP                  (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_GO_IRQ_STATE                40   /* bcr                     R5  */
#define MMC_GO_IRQ_STATE_OP             (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 7 */
#define MMC_LOCK_UNLOCK                 42   /* adtc                    R1b */
#define MMC_LOCK_UNLOCK_OP              (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 8 */
#define MMC_APP_CMD                     55   /* ac   [31:16] RCA        R1  */
#define MMC_APP_CMD_OP                  (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_GEN_CMD                     56   /* adtc [0] RD/WR          R1  */
#define MMC_GEN_CMD_OP                  (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

  /* class 11 */
#define MMC_QUE_TASK_PARAMS             44   /* ac   [20:16] task id    R1  */
#define MMC_QUE_TASK_PARAMS_OP          (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_QUE_TASK_ADDR               45   /* ac   [31:0] data addr   R1  */
#define MMC_QUE_TASK_ADDR_OP            (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_EXECUTE_READ_TASK           46   /* adtc [20:16] task id    R1  */
#define MMC_EXECUTE_READ_TASK_OP        (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_EXECUTE_WRITE_TASK          47   /* adtc [20:16] task id    R1  */
#define MMC_EXECUTE_WRITE_TASK_OP       (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)
#define MMC_CMDQ_TASK_MGMT              48   /* ac   [20:16] task id    R1b */
#define MMC_CMDQ_TASK_MGMT_OP           (MMC_CMDOP_RESP | MMC_CMDOP_RESP_SHORT_CRC)

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_MMC_APIS(__prefix)                                                                                                                                  \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,             mmc, init,                  VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, vsf_mmc_cfg_t *cfg_ptr)        \
    __VSF_HAL_TEMPLATE_API(__prefix, void,                  mmc, irq_enable,            VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)   \
    __VSF_HAL_TEMPLATE_API(__prefix, void,                  mmc, irq_disable,           VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)   \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_mmc_status_t,      mmc, status,                VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr)                                \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_mmc_capability_t,  mmc, capability,            VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr)                                \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,             mmc, host_set_clock,        VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, uint32_t clock_hz)             \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,             mmc, host_set_buswidth,     VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, uint8_t buswidth)              \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,             mmc, host_transact_start,   VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, vsf_mmc_trans_t *trans)        \
    __VSF_HAL_TEMPLATE_API(__prefix, void,                  mmc, host_transact_stop,    VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr)

/*============================ TYPES =========================================*/

#if VSF_MMC_CFG_REIMPLEMENT_FEATURE == DISABLED
typedef enum vsf_mmc_feature_t {
    MMC_MODE_HOST               = (0x1ul << 0), // select host mode
    MMC_MODE_SLAVE              = (0x0ul << 0), // select slave mode
    MMC_MODE_MASK               = (0x1ul << 0),
} vsf_mmc_feature_t;
#endif

/**
 \~english
 @brief flags of mmc tranfer operations
 @note vsf_mmc_transop_t is implemented by specific driver.

 \~chinese
 @brief mmc 传输操作的标志
 @note vsf_mmc_transop_t 由具体驱动实现。
 */
#if VSF_MMC_CFG_REIMPLEMENT_CMDOP == DISABLED
typedef enum vsf_mmc_transop_t {
    MMC_CMDOP_SINGLE_BLOCK      = (1ul << 0),
    MMC_CMDOP_MULTI_BLOCK       = (1ul << 1),
    MMC_CMDOP_WRITE             = (1ul << 2),
    MMC_CMDOP_RESP              = (1ul << 3),
    MMC_CMDOP_RESP_SHORT        = (1ul << 4),
    MMC_CMDOP_RESP_SHORT_CRC    = (2ul << 4),
    MMC_CMDOP_RESP_LONG_CRC     = (3ul << 4),
} vsf_mmc_transop_t;
#endif

typedef struct vsf_mmc_trans_t {
    uint8_t cmd;
    uint32_t arg;
    uint8_t *resp;
    vsf_mmc_transop_t op;

    uint8_t *buffer;
    uint32_t count;
} vsf_mmc_trans_t;

#if VSF_MMC_CFG_REIMPLEMENT_IRQ_MASK == DISABLED
typedef enum vsf_mmc_irq_mask_t {
    MMC_IRQ_MASK_HOST_RESP_DONE                 = (0x1ul <<  0),
    MMC_IRQ_MASK_HOST_DATA_DONE                 = (0x1ul <<  1),
    MMC_IRQ_MASK_HOST_ALL                       = MMC_IRQ_MASK_HOST_RESP_DONE
                                                | MMC_IRQ_MASK_HOST_DATA_DONE,
} vsf_mmc_irq_mask_t;
#endif

#if VSF_MMC_CFG_REIMPLEMENT_TRANSACT_STATUS == DISABLED
typedef enum vsf_mmc_transact_status_t {
    MMC_TRANSACT_STATUS_DONE                    = 0,
    MMC_TRANSACT_STATUS_ERR_RESP_NONE           = (0x1ul <<  0),
    MMC_TRANSACT_STATUS_ERR_RESP_CRC            = (0x1ul <<  1),
    MMC_TRANSACT_STATUS_ERR_DATA_CRC            = (0x1ul <<  2),
    MMC_TRANSACT_STATUS_DATA_BUSY               = (0x1ul <<  3),
    MMC_TRANSACT_STATUS_BUSY                    = (0x1ul <<  4),
    MMC_TRANSACT_STATUS_ERR_MASK                = MMC_TRANSACT_STATUS_ERR_RESP_NONE
                                                | MMC_TRANSACT_STATUS_ERR_RESP_CRC
                                                | MMC_TRANSACT_STATUS_ERR_DATA_CRC,
} vsf_mmc_transact_status_t;
#endif

#if VSF_MMC_CFG_REIMPLEMENT_STATUS == DISABLED
typedef struct vsf_mmc_status_t {
    union {
        inherit(peripheral_status_t)
        vsf_mmc_transact_status_t mmc_status;
    };
} vsf_mmc_status_t;
#endif

#if VSF_MMC_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct vsf_mmc_capability_t {
    inherit(vsf_peripheral_capability_t)
    enum {
        MMC_CAP_BUSWIDTH_1                      = (0x1ul <<  0),
        MMC_CAP_BUSWIDTH_4                      = (0x1ul <<  1),
        MMC_CAP_BUSWIDTH_8                      = (0x1ul <<  2),
    } mmc_capability;
} vsf_mmc_capability_t;
#endif

typedef struct vsf_mmc_t vsf_mmc_t;

/**
 \~english
 @brief mmc interrupt callback function prototype.
 @note It is called after interrupt occurs.

 @param target_ptr pointer of user.
 @param mmc_ptr pointer of mmc instance.
 @param irq_mask one or more value of enum vsf_mmc_irq_mask_t
 @param status transact status.
 @param resp response.
 @return None.

 \~chinese
 @brief mmc 中断回调函数原型
 @note 这个回调函数在中断发生之后被调用

 @param target_ptr 用户指针
 @param mmc_ptr mmc实例的指针
 @param irq_mask 一个或者多个枚举 vsf_mmc_irq_mask_t 的值的按位或
 @param status 传输状态
 @param resp 应答
 @return 无。

 \~
 \code {.c}
    static void __user_mmc_irchandler(void *target_ptr, vsf_mmc_t *mmc_ptr, enum irq_mask)
    {
        if (irq_mask & MMC_IRQ_MASK_HOST_RESP_DONE) {
            // do something
        }
    }
 \endcode
 */
typedef void vsf_mmc_isr_handler_t(void *target_ptr,
                                   vsf_mmc_t *mmc_ptr,
                                   vsf_mmc_irq_mask_t irq_mask,
                                   vsf_mmc_transact_status_t status,
                                   uint32_t resp[4]);

/**
 \~english
 @brief mmc interrupt configuration

 \~chinese
 @brief mmc 中断配置
 */
typedef struct vsf_mmc_isr_t {
    vsf_mmc_isr_handler_t *handler_fn;          //!< \~english TODO
                                                //!< \~chinese 中断回调函数
    void                  *target_ptr;          //!< \~english pointer of user target
                                                //!< \~chinese 用户传入的指针
    vsf_arch_prio_t        prio;                //!< \~english interrupt priority
                                                //!< \~chinesh 中断优先级
} vsf_mmc_isr_t;

/**
 \~english
 @brief mmc configuration

 \~chinese
 @brief mmc 配置
 */
typedef struct vsf_mmc_cfg_t {
    vsf_mmc_feature_t mode;                     //!< \~english mmc mode \ref vsf_mmc_feature_t
                                                //!< \~chinese mmc 模式 \ref vsf_mmc_feature_t
    vsf_mmc_isr_t isr;                          //!< \~english mmc interrupt
                                                //!< \~chinese mmc 中断
} vsf_mmc_cfg_t;

typedef struct vsf_mmc_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_MMC_APIS(vsf)
} vsf_mmc_op_t;

#if VSF_MMC_CFG_MULTI_CLASS == ENABLED
struct vsf_mmc_t  {
    const vsf_mmc_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_mmc_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if mmc was initialized, or a negative error code

 \~chinese
 @brief 初始化一个 mmc 实例
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] cfg_ptr: 结构体 vsf_mmc_cfg_t 的指针，参考 @ref vsf_mmc_cfg_t
 @return vsf_err_t: 如果 mmc 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_mmc_init(vsf_mmc_t *mmc_ptr, vsf_mmc_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @return none

 \~chinese
 @brief 结束一个 mmc 实例
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @return none
 */
extern void vsf_mmc_fini(vsf_mmc_t *mmc_ptr);

/**
 \~english
 @brief enable interrupt masks of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] irq_mask: one or more value of enum @ref vsf_mmc_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 mmc 实例的中断
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_i2c_irq_mask_t 的值的按位或，@ref vsf_mmc_irq_mask_t
 @return 无。
 */
extern void vsf_mmc_irq_enable(vsf_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] irq_mask: one or more value of enum vsf_mmc_irq_mask_t, @ref vsf_mmc_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 mmc 实例的中断
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_mmc_irq_mask_t 的值的按位或，@ref vsf_mmc_irq_mask_t
 @return 无。
 */
extern void vsf_mmc_irq_disable(vsf_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask);

/**
 \~english
 @brief get the status of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @return vsf_mmc_status_t: return all status of current mmc

 \~chinese
 @brief 获取 mmc 实例状态
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @return vsf_mmc_status_t: 返回当前mmc的所有状态
 */
extern vsf_mmc_status_t vsf_mmc_status(vsf_mmc_t *mmc_ptr);

/**
 \~english
 @brief get the capability of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @return vsf_mmc_capability_t: return all capability of current mmc @ref vsf_mmc_capability_t

 \~chinese
 @brief 获取 mmc 实例能力
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @return vsf_mmc_capability_t: 返回当前mmc的所有能力 @ref vsf_mmc_capability_t
 */
extern vsf_mmc_capability_t vsf_mmc_capability(vsf_mmc_t *mmc_ptr);

/**
 \~english
 @brief set the clock of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] clock_hz: clock in Hz
 @return vsf_err_t: VSF_ERR_NONE if mmc was initialized, or a negative error code

 \~chinese
 @brief 设置 mmc 时钟
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] clock_hz: 时钟速度 (单位：赫兹)
 @return vsf_err_t: 如果 mmc 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_mmc_host_set_clock(vsf_mmc_t *mmc_ptr, uint32_t clock_hz);

/**
 \~english
 @brief set the bus width of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] buswidth: bus width in 1, 4, 8
 @return vsf_err_t: VSF_ERR_NONE if mmc was initialized, or a negative error code

 \~chinese
 @brief 设置 mmc 总线位宽
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] buswidth: 总线位宽，范围：1, 4, 8
 @return vsf_err_t: 如果 mmc 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_mmc_host_set_buswidth(vsf_mmc_t *mmc_ptr, uint8_t buswidth);

/**
 \~english
 @brief start mmc transaction.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] trans: a pointer to mmc transaction structure
 @return vsf_err_t: VSF_ERR_NONE if mmc was initialized, or a negative error code

 \~chinese
 @brief 启动 mmc 传输
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] trans: mmc 传输结构指针
 @return vsf_err_t: 如果 mmc 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_mmc_host_transact_start(vsf_mmc_t *mmc_ptr, vsf_mmc_trans_t *trans);

/**
 \~english
 @brief stop mmc transaction if not done.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @return none.

 \~chinese
 @brief mmc 传输未完成时，停止 mmc 传输
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @return 无。
 */
extern void vsf_mmc_host_transact_stop(vsf_mmc_t *mmc_ptr);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_MMC_CFG_FUNCTION_RENAME == ENABLED
#   define vsf_mmc_init(__MMC, ...)                                             \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_init)             ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_enable(__MMC)                                                \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_enable)           ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC)
#   define vsf_mmc_disable(__MMC)                                               \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_disable)          ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC)
#   define vsf_mmc_irq_enable(__MMC, ...)                                       \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_irq_enable)       ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_irq_disable(__MMC, ...)                                      \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_irq_disable)      ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_status(__MMC)                                                \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_status)           ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC)
#   define vsf_mmc_capability(__MMC)                                            \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_capability)       ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC)
#   define vsf_mmc_host_set_clock(__MMC, ...)                                   \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_host_set_clock)   ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_host_set_buswidth(__MMC, ...)                                \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_host_set_buswidth)((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_host_transact_start(__MMC, ...)                              \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_host_transact_start)((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_host_transact_stop(__MMC, ...)                               \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_host_transact_stop)((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_MMC_INTERFACE_H__*/
