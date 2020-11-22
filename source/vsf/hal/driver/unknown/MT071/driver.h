/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)

#   include "__device.h"

#else

#   ifndef __HAL_DRIVER_MT071_H__
#       define __HAL_DRIVER_MT071_H__

/*============================ INCLUDES ======================================*/

#       include "hal/vsf_hal_cfg.h"
#       include "./device.h"
//#       include "./io/io.h"
#       include "./usb/usb.h"
//#       include "./pm/pm.h"
#       include "./usart/usart.h"

/*============================ MACROS ========================================*/

// __WIDTH in [6, 16, 32]
#define DMA_CTRL_DST_WIDTH(__WIDTH)         CONNECT2(DMA_CTRL_DST_WIDTH_, __WIDTH)
// __WIDTH in [6, 16, 32]
#define DMA_CTRL_SRC_WIDTH(__WIDTH)         CONNECT2(DMA_CTRL_SRC_WIDTH_, __WIDTH)
// __SIZE in [1, 4, 8, 16, 32, 64, 128, 256], does 256 supported?
#define DMA_CTRL_DST_BURST(__SIZE)          CONNECT2(DMA_CTRL_DST_BURST, __WIDTH)
// __SIZE in [1, 4, 8, 16, 32, 64, 128, 256], does 256 supported?
#define DMA_CTRL_SRC_BURST(__SIZE)          CONNECT2(DMA_CTRL_SRC_BURST, __WIDTH)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum __dma_trans_ctrl_t {
    DMA_CTRL_INT_EN         = DMA_CTL_L_INT_EN,

    DMA_CTRL_DST_WIDTH_8    = (0ul << 1),
    DMA_CTRL_DST_WIDTH_16   = (1ul << 1),
    DMA_CTRL_DST_WIDTH_32   = (2ul << 1),

    DMA_CTRL_SRC_WIDTH_8    = (0ul << 1),
    DMA_CTRL_SRC_WIDTH_16   = (1ul << 1),
    DMA_CTRL_SRC_WIDTH_32   = (2ul << 1),

    DMA_CTRL_DST_INC        = (0ul << 7),
    DMA_CTRL_DST_DEC        = (1ul << 7),
    DMA_CTRL_DST_KEEP       = (2ul << 7),

    DMA_CTRL_SRC_INC        = (0ul << 9),
    DMA_CTRL_SRC_DEC        = (1ul << 9),
    DMA_CTRL_SRC_KEEP       = (2ul << 9),

    DMA_DST_BURST_1         = (0ul << 11),
    DMA_DST_BURST_4         = (1ul << 11),
    DMA_DST_BURST_8         = (2ul << 11),
    DMA_DST_BURST_16        = (3ul << 11),
    DMA_DST_BURST_32        = (4ul << 11),
    DMA_DST_BURST_64        = (5ul << 11),
    DMA_DST_BURST_128       = (6ul << 11),
    DMA_DST_BURST_256       = (7ul << 11),

    DMA_SRC_BURST_1         = (0ul << 14),
    DMA_SRC_BURST_4         = (1ul << 14),
    DMA_SRC_BURST_8         = (2ul << 14),
    DMA_SRC_BURST_16        = (3ul << 14),
    DMA_SRC_BURST_32        = (4ul << 14),
    DMA_SRC_BURST_64        = (5ul << 14),
    DMA_SRC_BURST_128       = (6ul << 14),
    DMA_SRC_BURST_256       = (7ul << 14),

    DMA_CTRL_TRANS_TYPE_M2M = (0ul << 20),
    DMA_CTRL_TRANS_TYPE_M2P = (1ul << 20),
    DMA_CTRL_TRANS_TYPE_P2M = (2ul << 20),
    DMA_CTRL_TRANS_TYPE_P2P = (3ul << 20),
} __dma_trans_ctrl_t;

typedef struct __dma_trans_t {
    void *src;
    void *dst;
    void *nxt;
    // mask of DMA_CTRL_XXXX
    uint32_t ctrl;
    uint32_t count;

    // private
    uint32_t __remain_count;
    uint32_t __all_count;
} __dma_trans_t;

typedef struct __dma_channel_t __dma_channel_t;
struct __dma_channel_t {
    uint8_t id;
    bool is_allocated;
    bool is_chain;
    bool is_started;

    struct {
        void (*fn)(void *param, __dma_channel_t *channel);
        void *param;
    } callback;

    union {
        struct {
            uint8_t trans_num;
            __dma_trans_t *trans;
        } chain;
        __dma_trans_t trans;
    };
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void __vsf_dma_free_channel(__dma_channel_t *channel);
extern __dma_channel_t * __vsf_dma_alloc_channel(__dma_trans_t *trans, uint_fast8_t trans_num);

extern void __vsf_dma_channel_start(__dma_channel_t *channel, uint_fast64_t channel_cfg);
extern void __vsf_dma_channel_stop(__dma_channel_t *channel);

#   endif   // __HAL_DRIVER_MT071_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
