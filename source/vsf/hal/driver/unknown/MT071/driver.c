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

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "./device.h"
#include "./driver.h"

/*============================ MACROS ========================================*/

#define PLL_SRC                         CLKREF_12MIRC
#define MAINCLK_SRC                     CLKSRC_PLL

#define MAINCLK_FREQ_HZ                 __SYSTEM_FREQ

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_HAL_USE_DMA == ENABLED
typedef struct vsf_dma_t {
    __dma_channel_t channels[DMA_CHANNEL_COUNT];
} vsf_dma_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_HAL_USE_DMA == ENABLED
static NO_INIT vsf_dma_t __dma;
const uint16_t __dma_max_trans_size[DMA_CHANNEL_COUNT] = {
    DMA_CHANNEL_MAX_TRANS_SIZES
};
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_HAL_USE_DMA == ENABLED

static void __vsf_dma_commit(uint_fast8_t id, __dma_trans_t *trans)
{
    trans->count = min(trans->__remain_count, __dma_max_trans_size[id]);
    trans->__remain_count -= trans->count;

    *(uint32_t *)((uint32_t)(&(DMA->SAR0)) + 0x58 * id) = (uint32_t)trans->src;
    *(uint32_t *)((uint32_t)(&(DMA->DAR0)) + 0x58 * id) = (uint32_t)trans->dst;
    *(uint32_t *)((uint32_t)(&(DMA->LLP0)) + 0x58 * id) = (uint32_t)trans->nxt;
    *(uint32_t *)((uint32_t)(&(DMA->CTL0_L)) + 0x58 * id) = trans->ctrl;
    *(uint32_t *)((uint32_t)(&(DMA->CTL0_H)) + 0x58 * id) = trans->count;
}

void DMA_IRQHandler(void)
{
    __dma_channel_t *channel = __dma.channels;

    for (uint_fast8_t i = 0; i < DMA_CHANNEL_COUNT; i++, channel++) {
        if (DMA->StatusTfr & (1 << i)) {
            DMA->ClearTfr = 1 << i;

            if (!channel->is_chain) {
                __dma_trans_t *trans = &channel->trans;
                if (trans->__remain_count) {
                    __vsf_dma_commit(i, trans);
                    continue;
                }
            }

            if (channel->callback.fn != NULL) {
                channel->callback.fn(channel->callback.param, channel);
            }

            vsf_protect_t orig = vsf_protect_int();
                DMA->MaskTfr = 1 << (8 + i);
            vsf_unprotect_int(orig);
            channel->is_started = false;
        }
    }
}

// DMA, for internal usage only
void __vsf_dma_channel_start(__dma_channel_t *channel, uint_fast64_t channel_cfg)
{
    VSF_HAL_ASSERT(channel != NULL);
    VSF_HAL_ASSERT(channel->is_allocated && !channel->is_started);

    uint_fast8_t id = channel->id;
    __dma_trans_t *trans;

    if (channel->is_chain) {
        for (uint_fast8_t i = 0; i < channel->chain.trans_num; i++) {
            VSF_HAL_ASSERT(trans[i].count <= __dma_max_trans_size[channel->id]);
            if (i == (channel->chain.trans_num - 1)) {
                trans[i].nxt = NULL;
            } else {
                trans[i].nxt = &trans[i + 1];
            }
        }
        trans = channel->chain.trans;
    } else {
        trans = &channel->trans;
    }
    trans->__all_count = trans->__remain_count = trans->count;

    channel->is_started = true;
    *(uint32_t *)((uint32_t)(&(DMA->CFG0_L)) + 0x58 * id) = (uint32_t)(channel_cfg >> 0);
    *(uint32_t *)((uint32_t)(&(DMA->CFG0_H)) + 0x58 * id) = (uint32_t)(channel_cfg >> 32);
    __vsf_dma_commit(id, trans);

    vsf_protect_t orig = vsf_protect_int();
        DMA->ClearTfr = 1 << id;
        if (channel->callback.fn != NULL) {
            DMA->MaskTfr = (1 << id) | (1 << (8 + id));
        } else {
            DMA->MaskTfr = 1 << (8 + id);
        }
        DMA->ChEnReg = (1 << id) | (1 << (8 + id));
    vsf_unprotect_int(orig);
}

void __vsf_dma_channel_stop(__dma_channel_t *channel)
{
    uint_fast8_t id = channel->id;
    vsf_protect_t orig = vsf_protect_int();
        DMA->ChEnReg = 1 << (8 + id);
        DMA->ClearTfr = 1 << id;
    vsf_unprotect_int(orig);
}

void __vsf_dma_free_channel(__dma_channel_t *channel)
{
    VSF_HAL_ASSERT(channel != NULL);
    VSF_HAL_ASSERT(channel->is_allocated);
    channel->is_allocated = false;
}

__dma_channel_t * __vsf_dma_alloc_channel(__dma_trans_t *trans, uint_fast8_t trans_num)
{
    vsf_protect_t orig;
    for (uint_fast8_t i = 0; i < dimof(__dma.channels); i++) {
        orig = vsf_protect_int();
        if (!__dma.channels[i].is_allocated) {
            __dma.channels[i].is_allocated = true;
            vsf_unprotect_int(orig);

            if (trans != NULL) {
                __dma.channels[i].chain.trans_num = trans_num;
                __dma.channels[i].chain.trans = trans;
                __dma.channels[i].is_chain = true;
            } else {
                __dma.channels[i].is_chain = false;
            }
            return &__dma.channels[i];
        } else {
            vsf_unprotect_int(orig);
        }
    }
    return NULL;
}

// MT071 will stop sram clock while sleeping
//  so if DMA is enabled, over-write the vsf_arch_sleep to avoid sleeping if there is on-gong dma operation
void vsf_arch_sleep(uint32_t mode)
{
    UNUSED_PARAM(mode);

    for (uint_fast8_t i = 0; i < dimof(__dma.channels); i++) {
        if (__dma.channels[i].is_allocated && __dma.channels[i].is_started) {
            return;
        }
    }
    __WFE();
}
#endif

int __low_level_init(void)
{
    return 1;
}

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_driver_init(void)
{
    // test only, remove later
    uint_fast32_t timeout = 0;

    // enable clocks
#if (PLL_SRC == CLKREF_12MOSC) || (MAINCLK_SRC == CLKSRC_12MOSC)
//    RCC->PDRUNCFG &= ~RCC_PDRUNCFG_12MOSC;
//    RCC->OSC12_CTRL = RCC_12OSCCTRL_OSC_EN | (1 << 17) | (RCC->OSC12_CTRL & 0xFF); //bit17´ý²âÊÔ
//    while (!(RCC->OSC12_CTRL & RCC_12OSCCTRL_OSC_OK)) {
//        if((timeout++) >= 0x8000) {
//            break;
//        }
//    }
#endif

#if PLL_SRC == CLKREF_12MOSC || PLL_SRC == CLKREF_12MIRC
    uint_fast8_t div = __PLL_FREQ_HZ / 12000000;
    if (div < 4) {
        ASSERT(false);
    }
    RCC->PDRUNCFG &= ~RCC_PDRUNCFG_SYSPLL;
    RCC->SYSPLLCTRL = RCC_SYSPLLCTRL_FORCELOCK | (PLL_SRC << 30) | (div - 4);

    while (RCC->SYSPLLSTAT != RCC_SYSPLLSTAT_LOCK) {
        if(timeout++ >= 0x8000) {
            break;
        }
    }
#endif

    if (MAINCLK_FREQ_HZ < 40 * 1000 * 1000) {
        FLASH->ACR = FLASH_ACR_LATENCY_0 | FLASH_ACR_CACHENA;
    } else if (MAINCLK_FREQ_HZ < 80 * 1000 * 1000) {
        FLASH->ACR = FLASH_ACR_LATENCY_1 | FLASH_ACR_CACHENA;
    } else if (MAINCLK_FREQ_HZ < 120 * 1000 * 1000) {
        FLASH->ACR = FLASH_ACR_LATENCY_2 | FLASH_ACR_CACHENA;
    } else {
        FLASH->ACR = FLASH_ACR_LATENCY_3 | FLASH_ACR_CACHENA;
    }

    RCC->MAINCLKSEL = MAINCLK_SRC;
    RCC->MAINCLKUEN = 0;
    RCC->MAINCLKUEN = 1;

#if VSF_HAL_USE_DMA == ENABLED
    for (uint_fast8_t i = 0; i < dimof(__dma.channels); i++) {
        __dma.channels[i].id = i;
        __dma.channels[i].is_allocated = false;
        __dma.channels[i].is_started = false;
    }

    RCC->AHBCLKCTRL0_SET = RCC_AHBCLKCTRL_DMA;
    DMA->DmaCfgReg = DMA_DmaCfgReg_EN;
    // TODO: how about priority of DMA_IRQn?
    NVIC_EnableIRQ(DMA_IRQn);
#endif

    return true;
}


/* EOF */
