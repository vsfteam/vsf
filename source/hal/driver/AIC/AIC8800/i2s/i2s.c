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

#include "../driver.h"

#if VSF_HAL_USE_I2S == ENABLED

#include "../vendor/plf/aic8800/src/driver/sysctrl/reg_sysctrl.h"
#include "../vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"
#include "../vendor/plf/aic8800/src/driver/aud_proc/aud_proc.h"
#include "../vendor/plf/aic8800/src/driver/dma/dma_api_bt.h"
#include "../vendor/audio/common/audio_types.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_I2S_CFG_MULTI_CLASS
#   define VSF_HW_I2S_CFG_MULTI_CLASS           VSF_I2S_CFG_MULTI_CLASS
#endif

#define I2S_MCLK                                12288000
#define SRC_MODE_NB                             9
#define AUD_SRC_DIV_NB                          6

// aud_intf_i2s_cfg register
#define AUD_PROC_WLEN(__N)                      (((__N) & 3) << 10)
#define AUD_PROC_BCK_LRCK(__N)                  (((__N) & 31) << 5)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_i2s_const_t {
    HWP_AUD_PROC_T *reg;
    uint8_t idx;

    struct {
        AIC_DMA_CH_ENUM dma_ch;
        uint8_t dma_irqn;
        AUD_PATH_T path;
    } tx, rx;
    // TODO: move to pm
    uint32_t oclk;
} vsf_hw_i2s_const_t;

typedef struct vsf_hw_i2s_t {
#if VSF_HW_I2S_CFG_MULTI_CLASS == ENABLED
    vsf_i2s_t vsf_i2s;
#endif

    const vsf_hw_i2s_const_t    *i2s_const;
    uint32_t mode;
    struct {
        DMA_CX_DESC_T           dma_desc[2];
        bool                    path_opened;
        vsf_i2s_isr_handler_t   *isrhandler;
        void                    *isr_param;
    } ALIGN(8) rx, tx;
} vsf_hw_i2s_t;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const uint8_t __dac_mixer_mode[5] = {
    0x00,// left: left channel data
    0x01,// mute: mute
    0x02,// right: right channel data
    0x03,// lprd2: (left channel + right channel) / 2
    0x04,// lmrd2: (left channel - right channel) / 2
};

// 100000 * out_samp_rate / in_samp_rate
// divider: 1
static const uint32_t __dac_src_mode_ratio[SRC_MODE_NB] = {
    (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_8000)),
    (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_11025)),
    (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_12000)),
    (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_16000)),
    (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_22050)),
    (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_24000)),
    (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_32000)),
    (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_44100)),
    (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_48000)),
};

// 100000 * out_samp_rate / in_samp_rate
// divider: 1 2 3 4 6 12
static const uint32_t __aud_src_mode_ratio[AUD_SRC_DIV_NB][SRC_MODE_NB] = {
    {
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_8000)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_11025)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_12000)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_16000)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_22050)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_24000)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_32000)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_44100)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_48000)),
    },
    {
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_8000 / 2)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_11025 / 2)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_12000 / 2)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_16000 / 2)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_22050 / 2)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_24000 / 2)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_32000 / 2)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_44100 / 2)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_48000 / 2)),
    },
    {
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_8000 / 3)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_11025 / 3)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_12000 / 3)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_16000 / 3)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_22050 / 3)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_24000 / 3)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_32000 / 3)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_44100 / 3)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_48000 / 3)),
    },
    {
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_8000 / 4)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_11025 / 4)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_12000 / 4)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_16000 / 4)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_22050 / 4)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_24000 / 4)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_32000 / 4)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_44100 / 4)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_48000 / 4)),
    },
    {
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_8000 / 6)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_11025 / 6)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_12000 / 6)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_16000 / 6)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_22050 / 6)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_24000 / 6)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_32000 / 6)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_44100 / 6)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_48000 / 6)),
    },
    {
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_8000 / 12)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_11025 / 12)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_12000 / 12)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_16000 / 12)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_22050 / 12)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_24000 / 12)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_32000 / 12)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_44100 / 12)),
        (uint32_t)(100000 * ((float)AUD_SAMPRATE_48000 / (float)AUD_SAMPRATE_48000 / 12)),
    },
};

// 1st row is real divider
// 2nd row is corresponding value in aud_src_ctrl2[16:13]
static const uint8_t __aud_src_div[2][AUD_SRC_DIV_NB] = {
    {1, 2, 3, 4, 6, 12},
    {0x0A, 0x09, 0x06, 0x08, 0x05, 0x04},
};

static void * __vsf_hw_i2s_irq_param[5];

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_hw_i2s_dma_irqhandler(uint8_t dma_type, uint8_t ch, uint32_t int_status)
{
    VSF_HAL_ASSERT(ch < dimof(__vsf_hw_i2s_irq_param));
    vsf_hw_i2s_t *hw_i2s_ptr = __vsf_hw_i2s_irq_param[ch];
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    const vsf_hw_i2s_const_t * hw_i2s_const = hw_i2s_ptr->i2s_const;
    VSF_HAL_ASSERT(NULL != hw_i2s_const);

    if (ch == hw_i2s_const->tx.dma_ch) {
        if (hw_i2s_ptr->tx.isrhandler != NULL) {
            hw_i2s_ptr->tx.isrhandler(hw_i2s_ptr->tx.isr_param, (vsf_i2s_t *)hw_i2s_ptr, VSF_I2S_IRQ_MASK_TX_TGL_BUFFER);
        }
    } else if (ch == hw_i2s_const->rx.dma_ch) {
        if (hw_i2s_ptr->rx.isrhandler != NULL) {
            hw_i2s_ptr->rx.isrhandler(hw_i2s_ptr->rx.isr_param, (vsf_i2s_t *)hw_i2s_ptr, VSF_I2S_IRQ_MASK_RX_TGL_BUFFER);
        }
    } else {
        VSF_HAL_ASSERT(false);
    }
}

vsf_err_t vsf_hw_i2s_init(vsf_hw_i2s_t *hw_i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    const vsf_hw_i2s_const_t * hw_i2s_const = hw_i2s_ptr->i2s_const;
    VSF_HAL_ASSERT(NULL != hw_i2s_const);
    HWP_AUD_PROC_T *reg = hw_i2s_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    // TODO: call pm
    cpusysctrl_hclkme_set(CSC_HCLKME_DMA_EN_BIT | CSC_HCLKME_VPC_EN_BIT);
    cpusysctrl_oclkme_set(hw_i2s_const->oclk);
    hw_i2s_ptr->mode = 0;

    if (cfg_ptr != NULL) {
        uint32_t mode = cfg_ptr->mode;
        uint8_t data_bitlen;
        switch (mode & VSF_I2S_DATA_BITLEN_MASK) {
        case VSF_I2S_DATA_BITLEN_16:    data_bitlen = 16;   break;
        case VSF_I2S_DATA_BITLEN_24:    data_bitlen = 24;   break;
        default:
        case VSF_I2S_DATA_BITLEN_32:    return VSF_ERR_NOT_SUPPORT;
        }
        uint8_t frame_bitlen = 0;
        switch (mode & VSF_I2S_FRAME_BITLEN_MASK) {
        case VSF_I2S_FRAME_BITLEN_16:   return VSF_ERR_NOT_SUPPORT;
        case VSF_I2S_FRAME_BITLEN_24:   frame_bitlen = 24;  break;
        default:
        case VSF_I2S_FRAME_BITLEN_32:   frame_bitlen = 32;  break;
        }
        uint8_t channel_num = cfg_ptr->channel_num;
        if (    (0 == channel_num) || (channel_num > 2)
            ||  (0 == cfg_ptr->hw_sample_rate)
            ||  (frame_bitlen < data_bitlen)) {
            return VSF_ERR_INVALID_PARAMETER;
        }

        uint32_t reg_i2s_cfg = mode & __I2S_HW_FEATURE_MASK;

        if ((mode & VSF_I2S_MODE_MASK) == VSF_I2S_MODE_MASTER) {
            reg_i2s_cfg |= AUD_PROC_WLEN(2) | AUD_PROC_BCK_LRCK(frame_bitlen - data_bitlen);
            if (mode & VSF_I2S_MCLK_OUTPUT) {
                // mclk_bck_div = mclk / bck = mclk / (frame_bitlen * channel_num) / hw_sample_rate
                uint32_t mclk_div = I2S_MCLK / (frame_bitlen * channel_num) / cfg_ptr->hw_sample_rate;
                AIC_CPUSYSCTRL->CMCLKDIV = mclk_div | (1 << 8);
            }
        } else {
            mode &= ~VSF_I2S_MCLK_OUTPUT;
        }

        if (0 == hw_i2s_const->idx) {
            reg->aud_intf_i2s_cfg0 = reg_i2s_cfg;
            if (mode & VSF_I2S_MCLK_OUTPUT) {
                AIC_CPUSYSCTRL->CBCLKSEL |= 1 << 0;
            }
        } else {
            reg->aud_intf_i2s_cfg1 = reg_i2s_cfg;
            if (mode & VSF_I2S_MCLK_OUTPUT) {
                AIC_CPUSYSCTRL->CBCLKSEL |= 1 << 2;
            }
        }
        hw_i2s_ptr->mode = mode;
    }

    return VSF_ERR_NONE;
}

static int __audio_get_dac_src_mode(uint32_t in_samp_rate, uint32_t out_samp_rate)
{
    int i = 0;
    bool found = false;

    uint32_t ratio = 0;

    VSF_HAL_ASSERT(in_samp_rate <= out_samp_rate);

    ratio = (uint32_t)(100000 * ((float)out_samp_rate / (float)in_samp_rate));

    for (i = 0; i < SRC_MODE_NB; i++) {
        if (ratio == __dac_src_mode_ratio[i]) {
            found = true;
            break;
        }
    }
    VSF_HAL_ASSERT(found);

    return i;
}

vsf_err_t vsf_hw_i2s_tx_init(vsf_hw_i2s_t *hw_i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    const vsf_hw_i2s_const_t * hw_i2s_const = hw_i2s_ptr->i2s_const;
    VSF_HAL_ASSERT(NULL != hw_i2s_const);
    HWP_AUD_PROC_T *reg = hw_i2s_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    if (!hw_i2s_ptr->mode) {
        vsf_hw_i2s_init(hw_i2s_ptr, cfg_ptr);
    }

    AUD_PATH_T path = hw_i2s_const->tx.path;
    uint32_t mode = cfg_ptr->mode;
    uint8_t data_bitlen;
    uint8_t channel_num = cfg_ptr->channel_num;
    switch (mode & VSF_I2S_DATA_BITLEN_MASK) {
    case VSF_I2S_DATA_BITLEN_16:    data_bitlen = 16;   break;
    case VSF_I2S_DATA_BITLEN_24:    data_bitlen = 24;   break;
    default:
    case VSF_I2S_DATA_BITLEN_32:    return VSF_ERR_NOT_SUPPORT;
    }
//    bool src_en = cfg_ptr->data_sample_rate && (cfg_ptr->data_sample_rate != cfg_ptr->hw_sample_rate);
    bool src_en = true;     // src_en is always true for playback

    DMA_CFG_T dma_cfg = { .dma_type = DMA_CX };
    dma_get_default_config(dma_cfg.dma_type, &dma_cfg);
    dma_cfg.src_periph = DMA_MEMORY;
    dma_cfg.trans_type = DMA_TRANS_M2P;
    dma_cfg.ch = hw_i2s_const->tx.dma_ch;
    dma_cfg.handler = __vsf_hw_i2s_dma_irqhandler;
    if (AUD_PATH_TX01 == path) {
        dma_cfg.dest_periph = DMA_AUD_PROC_TX0;
        dma_cfg.ext_req_cid = REQ_CID_AUD_PROC_TX0;
    } else if (AUD_PATH_TX23 == path) {
        dma_cfg.dest_periph = DMA_AUD_PROC_TX2;
        dma_cfg.ext_req_cid = REQ_CID_AUD_PROC_TX2;
    }
    dma_cfg.dest_addr = dma_cx_get_periph_addr(dma_cfg.dest_periph);
    dma_cfg.dest_size = AHB_WORD;
    dma_cfg.addr_fix_type = FIX_ON_DEST;
    dma_cfg.dest_tran_sz = 0;
    if (16 == data_bitlen) {
        dma_cfg.src_size = AHB_HWORD;
        dma_cfg.src_tran_sz = 2;
    } else {
        dma_cfg.src_size = AHB_WORD;
        dma_cfg.src_tran_sz = 4;
    }

    uint32_t desc_trans_size = cfg_ptr->buffer_size >> 1;
    dma_cfg.tbl1_cnt = dma_cfg.tbl2_cnt = desc_trans_size;
    dma_cfg.llist_dedicated_int_en = true;
    dma_cfg.llist_en = true;
    dma_cfg.llist_cfg_valid = false;
    dma_cfg.nxt_addr = (uint32_t)&hw_i2s_ptr->tx.dma_desc[0];
    dma_cfg.src_addr = (uint32_t)cfg_ptr->buffer;
    dma_cx_desc_link(&dma_cfg, &hw_i2s_ptr->tx.dma_desc[0], &hw_i2s_ptr->tx.dma_desc[1]);
    dma_cfg.src_addr = (uint32_t)((uint8_t *)cfg_ptr->buffer + desc_trans_size);
    dma_cx_desc_link(&dma_cfg, &hw_i2s_ptr->tx.dma_desc[1], &hw_i2s_ptr->tx.dma_desc[0]);
    dma_cfg.src_addr = (uint32_t)cfg_ptr->buffer;
    dma_cx_config(dma_cfg.ch, &dma_cfg);

    if (0 == hw_i2s_const->idx) {
        reg->aud_proc_cfg &= ~AUD_PROC_I2S_CHN_OUT_SEL;
    } else {
        reg->aud_proc_cfg |= AUD_PROC_I2S_CHN_OUT_SEL;
    }

    uint32_t val;
    if (AUD_PATH_TX01 == path) {
        uint32_t src_mode = 0;
        uint8_t src_ch_en = 0;

        if (src_en) {
            src_mode = __audio_get_dac_src_mode(cfg_ptr->data_sample_rate, cfg_ptr->hw_sample_rate);
            src_ch_en = (1 << channel_num) - 1;
        }

        /* DAC SRC enable and mode set, keep mux mode(default no mux),  keep dac volume(default 0dbm) */
        val = reg->aud_proc_dac_cfg1;
        val &= ~(AUD_PROC_SRC_MODE(0xF) | AUD_PROC_SRC_CH_EN(0x03));
        val |= ((src_en? AUD_PROC_SRC_MODE(src_mode) : AUD_PROC_SRC_MODE(8)) | AUD_PROC_SRC_CH_EN(src_ch_en));
        reg->aud_proc_dac_cfg1 = val;

        /* DAC SRC filter set, keep mixer mode(default L=L, R=R), Do not modify EQ here */
        val = reg->aud_proc_dac_cfg2;
        val &= ~(AUD_PROC_SRC_FLT_BYPASS(0x0F) | AUD_PROC_DAC_MIXER_MODE_L(0x07) | AUD_PROC_DAC_MIXER_MODE_R(0x07));
        val |= (src_en? AUD_PROC_SRC_FLT_BYPASS(0) : AUD_PROC_SRC_FLT_BYPASS(0x0F)) |
            AUD_PROC_DAC_MIXER_MODE_L(__dac_mixer_mode[PLF_DACL_MIXER_MODE]) |
            AUD_PROC_DAC_MIXER_MODE_R(__dac_mixer_mode[PLF_DACR_MIXER_MODE]);
        reg->aud_proc_dac_cfg2 = val;

        /* high speed mode, auto clock gating enable, tx01 lr fifo share enable, txrx lr fifo share enable, txrx burst mode, tx burst mode */
        val = reg->aud_proc_cfg;
        val &= ~(AUD_PROC_TX_LR_FIFO_SHARE_EN(0x01) | AUD_PROC_TXRX_LR_FIFO_SHARE_EN);
        val |= AUD_PROC_HIGH_SPEED_MODE | AUD_PROC_AUTO_CLOCK_GATING_EN | AUD_PROC_TXRX_BURST_MODE | AUD_PROC_TX_BURST_MODE;
        if (2 == channel_num) {
            val |= AUD_PROC_TX_LR_FIFO_SHARE_EN(0x01) | AUD_PROC_TXRX_LR_FIFO_SHARE_EN;
        }
        reg->aud_proc_cfg = val;

        /* Set channel mode(16 bits or 24 bits) */
        if (data_bitlen == 24) {
            reg->aud_proc_tx_cfg_ch0 |= AUD_PROC_TX_MODE_CH_0;
            reg->aud_proc_tx_cfg_ch1 |= AUD_PROC_TX_MODE_CH_1;
        } else {
            reg->aud_proc_tx_cfg_ch0 &= ~AUD_PROC_TX_MODE_CH_0;
            reg->aud_proc_tx_cfg_ch1 &= ~AUD_PROC_TX_MODE_CH_1;
        }
    } else if (AUD_PATH_TX23 == path) {
        /* tx mux config, add tx ch2 to L, add tx ch3 to R  */
        val = reg->aud_proc_dac_cfg1;
        val &= ~(AUD_PROC_TX_MUX_MODE_L(0x3) | AUD_PROC_TX_MUX_MODE_R(0x3));
        val |= (AUD_PROC_TX_MUX_MODE_L(0x0) | AUD_PROC_TX_MUX_MODE_R(0x1));
        reg->aud_proc_dac_cfg1 = val;

        /* high speed mode, auto clock gating enable, tx23 lr fifo share enable, txrx lr fifo share enable, txrx burst mode, tx burst mode */
        val = reg->aud_proc_cfg;
        val &= ~(AUD_PROC_TX_LR_FIFO_SHARE_EN(0x02) | AUD_PROC_TXRX_LR_FIFO_SHARE_EN);
        val |= AUD_PROC_HIGH_SPEED_MODE | AUD_PROC_AUTO_CLOCK_GATING_EN | AUD_PROC_TXRX_BURST_MODE | AUD_PROC_TX_BURST_MODE;
        if (2 == channel_num) {
            val |= AUD_PROC_TX_LR_FIFO_SHARE_EN(0x02) | AUD_PROC_TXRX_LR_FIFO_SHARE_EN;
        }
        reg->aud_proc_cfg = val;

        /* Set channel mode(16 bits or 24 bits) */
        if (data_bitlen == 24) {
            reg->aud_proc_tx_cfg_ch2 |= AUD_PROC_TX_MODE_CH_2;
            reg->aud_proc_tx_cfg_ch3 |= AUD_PROC_TX_MODE_CH_3;
        } else {
            reg->aud_proc_tx_cfg_ch2 &= ~AUD_PROC_TX_MODE_CH_2;
            reg->aud_proc_tx_cfg_ch3 &= ~AUD_PROC_TX_MODE_CH_3;
        }
    }

    if (cfg_ptr->isr.handler_fn != NULL) {
        hw_i2s_ptr->tx.isr_param = cfg_ptr->isr.target_ptr;
        hw_i2s_ptr->tx.isrhandler = cfg_ptr->isr.handler_fn;
        NVIC_SetPriority(hw_i2s_const->tx.dma_irqn, cfg_ptr->isr.prio);
        NVIC_EnableIRQ(hw_i2s_const->tx.dma_irqn);
    }

    VSF_HAL_ASSERT(NULL == __vsf_hw_i2s_irq_param[hw_i2s_const->tx.dma_ch]);
    __vsf_hw_i2s_irq_param[hw_i2s_const->tx.dma_ch] = hw_i2s_ptr;
    return VSF_ERR_NONE;
}

void vsf_hw_i2s_tx_fini(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    const vsf_hw_i2s_const_t * hw_i2s_const = hw_i2s_ptr->i2s_const;
    VSF_HAL_ASSERT(NULL != hw_i2s_const);
    HWP_AUD_PROC_T *reg = hw_i2s_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    hw_i2s_ptr->tx.path_opened = false;

    uint32_t val = reg->aud_proc_dac_cfg2;
    uint32_t mixer_mode = val & (AUD_PROC_DAC_MIXER_MODE_L(0x07) | AUD_PROC_DAC_MIXER_MODE_R(0x07));
    val &= ~(AUD_PROC_DAC_MIXER_MODE_L(0x07) | AUD_PROC_DAC_MIXER_MODE_R(0x07));
    val |= (AUD_PROC_DAC_MIXER_MODE_L(0x01) | AUD_PROC_DAC_MIXER_MODE_R(0x01));
    reg->aud_proc_dac_cfg2 = val;

    uint32_t delay = 0;
    uint32_t delay_samples = 20;
    //(HCLK / 1000000) * (1000 / 48) * (delay_samples / 10)
    delay = sysctrl_clock_get(SYS_HCLK) / 1000000 * 1000 / 48 * delay_samples / 10;
    for (uint32_t i = 0; i < delay; i++) {
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP();
    }

    if (!hw_i2s_ptr->tx.path_opened) {
        if (0 == hw_i2s_const->idx) {
            reg->aud_intf_i2s_cfg0 &= ~AUD_PROC_I2S_ENABLE_0;
        } else {
            reg->aud_intf_i2s_cfg1 &= ~AUD_PROC_I2S_ENABLE_1;
        }
    }

    val = reg->aud_proc_dac_cfg2;
    val &= ~(AUD_PROC_DAC_MIXER_MODE_L(0x07) | AUD_PROC_DAC_MIXER_MODE_R(0x07));
    val |= mixer_mode;
    reg->aud_proc_dac_cfg2 = val;

    dma_cx_free(hw_i2s_const->tx.dma_ch);
    dma_cx_set_lli_cntr(hw_i2s_const->tx.dma_ch, 0);

    __vsf_hw_i2s_irq_param[hw_i2s_const->tx.dma_ch] = NULL;
}

vsf_err_t vsf_hw_i2s_tx_start(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    const vsf_hw_i2s_const_t * hw_i2s_const = hw_i2s_ptr->i2s_const;
    VSF_HAL_ASSERT(NULL != hw_i2s_const);
    HWP_AUD_PROC_T *reg = hw_i2s_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    dma_cx_halt_set(hw_i2s_const->tx.dma_ch, false);
    dma_cx_enable_set(hw_i2s_const->tx.dma_ch, true);
    if (AUD_PATH_TX01 == hw_i2s_const->tx.path) {
        reg->aud_proc_tx_cfg_ch0 |= AUD_PROC_TX_EN_CH_0;
    } else {
        reg->aud_proc_tx_cfg_ch2 |= AUD_PROC_TX_EN_CH_2;
    }
    reg->aud_proc_cfg |= AUD_PROC_AUD_PROC_ENABLE;
    if (0 == hw_i2s_const->idx) {
        reg->aud_intf_i2s_cfg0 |= AUD_PROC_I2S_ENABLE_0;
    } else {
        reg->aud_intf_i2s_cfg1 |= AUD_PROC_I2S_ENABLE_1;
    }
    hw_i2s_ptr->tx.path_opened = true;

    return VSF_ERR_NONE;
}

//return audio src mode | audio src div
//bit[7:4] correspond to aud_src_ctrl2[7:4](audio_src_mode)
//bit[16:13] correspong to aud_src_ctrl2[16:13](audio_src_div)
static uint32_t __audio_get_aud_src_mode(uint32_t in_samp_rate, uint32_t out_samp_rate)
{
    int i = 0;
    int j = 0;
    uint32_t src_mode = 0;
    bool found = false;

    uint32_t ratio = 0;

    ratio = (uint32_t)(100000 * ((float)out_samp_rate / (float)in_samp_rate));

    for (i = 0; i < AUD_SRC_DIV_NB; i++) {
        if ((ratio > __aud_src_mode_ratio[i][0]) || (ratio < __aud_src_mode_ratio[i][SRC_MODE_NB - 1])) {
            continue;
        } else {
            for (j = 0; j < SRC_MODE_NB; j++) {
                if (ratio == __aud_src_mode_ratio[i][j]) {
                    found = true;
                    break;
                }
            }
            if (found == true) {
                src_mode = (j << 4) | (__aud_src_div[1][i] << 13);
                return src_mode;
            }
        }
    }
    VSF_HAL_ASSERT(found);

    return src_mode;
}

vsf_err_t vsf_hw_i2s_rx_init(vsf_hw_i2s_t *hw_i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    const vsf_hw_i2s_const_t * hw_i2s_const = hw_i2s_ptr->i2s_const;
    VSF_HAL_ASSERT(NULL != hw_i2s_const);
    HWP_AUD_PROC_T *reg = hw_i2s_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    if (!hw_i2s_ptr->mode) {
        vsf_hw_i2s_init(hw_i2s_ptr, cfg_ptr);
    }

    AUD_PATH_T path = hw_i2s_const->rx.path;
    uint32_t mode = cfg_ptr->mode;
    uint8_t data_bitlen;
    uint8_t channel_num = cfg_ptr->channel_num;
    switch (mode & VSF_I2S_DATA_BITLEN_MASK) {
    case VSF_I2S_DATA_BITLEN_16:    data_bitlen = 16;   break;
    case VSF_I2S_DATA_BITLEN_24:    data_bitlen = 24;   break;
    default:
    case VSF_I2S_DATA_BITLEN_32:    return VSF_ERR_NOT_SUPPORT;
    }
    bool src_en = cfg_ptr->data_sample_rate && (cfg_ptr->data_sample_rate != cfg_ptr->hw_sample_rate);

    DMA_CFG_T dma_cfg = { .dma_type = DMA_CX };
    dma_get_default_config(dma_cfg.dma_type, &dma_cfg);
    dma_cfg.dest_periph = DMA_MEMORY;
    dma_cfg.trans_type = DMA_TRANS_P2M;
    dma_cfg.ch = hw_i2s_const->rx.dma_ch;
    dma_cfg.handler = __vsf_hw_i2s_dma_irqhandler;
    if (AUD_PATH_RX01 == path) {
        if (src_en) {
            dma_cfg.src_periph = DMA_AUD_SRC_RX;
            dma_cfg.ext_req_cid = REQ_CID_AUD_SRC_RX;
        } else {
            dma_cfg.src_periph = DMA_AUD_PROC_RX01;
            dma_cfg.ext_req_cid = REQ_CID_AUD_PROC_RX01;
        }
    } else if (AUD_PATH_RX23 == path) {
        dma_cfg.src_periph = DMA_AUD_PROC_RX23;
        dma_cfg.ext_req_cid = REQ_CID_AUD_PROC_RX23;
    }
    dma_cfg.src_addr = dma_cx_get_periph_addr(dma_cfg.src_periph);
    dma_cfg.src_size = AHB_WORD;
    dma_cfg.addr_fix_type = FIX_ON_SRC;
    dma_cfg.src_tran_sz = 0;
    if (16 == data_bitlen) {
        dma_cfg.dest_size = AHB_HWORD;
        dma_cfg.dest_tran_sz = 2;
    } else {
        dma_cfg.dest_size = AHB_WORD;
        dma_cfg.dest_tran_sz = 4;
    }

    uint32_t desc_trans_size = cfg_ptr->buffer_size >> 1;
    dma_cfg.tbl1_cnt = dma_cfg.tbl2_cnt = desc_trans_size;
    dma_cfg.llist_dedicated_int_en = true;
    dma_cfg.llist_en = true;
    dma_cfg.llist_cfg_valid = false;
    dma_cfg.nxt_addr = (uint32_t)&hw_i2s_ptr->rx.dma_desc[0];
    dma_cfg.dest_addr = (uint32_t)cfg_ptr->buffer;
    dma_cx_desc_link(&dma_cfg, &hw_i2s_ptr->rx.dma_desc[0], &hw_i2s_ptr->rx.dma_desc[1]);
    dma_cfg.dest_addr = (uint32_t)((uint8_t *)cfg_ptr->buffer + desc_trans_size);
    dma_cx_desc_link(&dma_cfg, &hw_i2s_ptr->rx.dma_desc[1], &hw_i2s_ptr->rx.dma_desc[0]);
    dma_cfg.dest_addr = (uint32_t)cfg_ptr->buffer;
    dma_cx_config(dma_cfg.ch, &dma_cfg);

    uint32_t val;
    if (AUD_PATH_RX01 == path) {
        if (0 == hw_i2s_const->idx) {
            reg->aud_proc_cfg &= ~AUD_PROC_I2S_CHN_IN_SEL;
        } else {
            reg->aud_proc_cfg |= AUD_PROC_I2S_CHN_IN_SEL;
        }

        /* high speed mode, auto clock gating enable, rx01 lr fifo share enable, rxtx lr fifo share enable, rxtx burst mode, rx burst mode */
        val = reg->aud_proc_cfg;
        val &= ~(AUD_PROC_RX_LR_FIFO_SHARE_EN(0x01) | AUD_PROC_RXTX_LR_FIFO_SHARE_EN(0x01));
        val |= AUD_PROC_HIGH_SPEED_MODE | AUD_PROC_AUTO_CLOCK_GATING_EN | AUD_PROC_RXTX_BURST_MODE | AUD_PROC_RX_BURST_MODE;
        if (channel_num == 2) {
            val |= AUD_PROC_RX_LR_FIFO_SHARE_EN(0x01) | AUD_PROC_RXTX_LR_FIFO_SHARE_EN(0x01);
        }
        reg->aud_proc_cfg = val;

        /* Set channel mode(16 bits or 24 bits) */
        if (data_bitlen == 24) {
            reg->aud_proc_rx_cfg_ch01 |= AUD_PROC_RX_MODE_CH_01;
        } else {
            reg->aud_proc_rx_cfg_ch01 &= ~AUD_PROC_RX_MODE_CH_01;
        }

        if (src_en == true) {
            val = AUD_PROC_AUD_SRC_ENABLE | AUD_PROC_AUD_SRC_TXRX_EN_CH_0 | AUD_PROC_AUD_SRC_AUTO_CLOCK_GATING_EN | AUD_PROC_AUD_SRC_ADC_IN_EN | AUD_PROC_AUD_SRC_TXRX_BURST_MODE | AUD_PROC_AUD_SRC_TX_BURST_MODE;
            if (channel_num == 2) {
                val |= AUD_PROC_AUD_SRC_TXRX_LR_FIFO_SHARE_EN | AUD_PROC_AUD_SRC_TX_LR_FIFO_SHARE_EN | AUD_PROC_AUD_SRC_TXRX_EN_CH_1;
            }
            if (data_bitlen == 24) {
                val |= AUD_PROC_AUD_SRC_TXRX_MODE_CH | AUD_PROC_AUD_SRC_TX_MODE_CH;
            }
            reg->aud_src_ctrl0 = val;

            uint32_t src_cfg = __audio_get_aud_src_mode(cfg_ptr->data_sample_rate, cfg_ptr->hw_sample_rate);
            reg->aud_src_ctrl2 = AUD_PROC_AUD_SRC_CH_EN(0x03) | src_cfg;
        } else {
            reg->aud_src_ctrl0 &= ~AUD_PROC_AUD_SRC_ENABLE;
        }
    } else if (AUD_PATH_RX23 == path) {
        if (0 == hw_i2s_const->idx) {
            reg->aud_proc_cfg |= AUD_PROC_I2S_CHN_IN_SEL;
        } else {
            reg->aud_proc_cfg &= ~AUD_PROC_I2S_CHN_IN_SEL;
        }

        /* high speed mode, auto clock gating enable, rx01 lr fifo share enable, rxtx lr fifo share enable, rxtx burst mode, rx burst mode */
        val = reg->aud_proc_cfg;
        val &= ~(AUD_PROC_RX_LR_FIFO_SHARE_EN(0x02) | AUD_PROC_RXTX_LR_FIFO_SHARE_EN(0x02));
        val |= AUD_PROC_HIGH_SPEED_MODE | AUD_PROC_AUTO_CLOCK_GATING_EN | AUD_PROC_RXTX_BURST_MODE | AUD_PROC_RX_BURST_MODE;
        if (channel_num == 2) {
            val |= AUD_PROC_RX_LR_FIFO_SHARE_EN(0x02) | AUD_PROC_RXTX_LR_FIFO_SHARE_EN(0x02);
        }
        reg->aud_proc_cfg = val;

        /* Set channel mode(16 bits or 24 bits) */
        if (data_bitlen == AUD_BITS_24) {
            reg->aud_proc_rx_cfg_ch23 |= AUD_PROC_RX_MODE_CH_23;
        } else {
            reg->aud_proc_rx_cfg_ch23 &= ~AUD_PROC_RX_MODE_CH_23;
        }
    }

    if (cfg_ptr->isr.handler_fn != NULL) {
        hw_i2s_ptr->rx.isr_param = cfg_ptr->isr.target_ptr;
        hw_i2s_ptr->rx.isrhandler = cfg_ptr->isr.handler_fn;
        NVIC_SetPriority(hw_i2s_const->rx.dma_irqn, cfg_ptr->isr.prio);
        NVIC_EnableIRQ(hw_i2s_const->rx.dma_irqn);
    }

    VSF_HAL_ASSERT(NULL == __vsf_hw_i2s_irq_param[hw_i2s_const->rx.dma_ch]);
    __vsf_hw_i2s_irq_param[hw_i2s_const->rx.dma_ch] = hw_i2s_ptr;
    return VSF_ERR_NONE;
}

void vsf_hw_i2s_rx_fini(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    const vsf_hw_i2s_const_t * hw_i2s_const = hw_i2s_ptr->i2s_const;
    VSF_HAL_ASSERT(NULL != hw_i2s_const);
    HWP_AUD_PROC_T *reg = hw_i2s_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    hw_i2s_ptr->rx.path_opened = false;

    /* Disable AUD_PROC */
    if (AUD_PATH_RX01 == hw_i2s_const->rx.path) {
        reg->aud_proc_rx_cfg_ch01 &= ~AUD_PROC_RX_EN_CH_0;
    } else {
        reg->aud_proc_rx_cfg_ch23 &= ~AUD_PROC_RX_EN_CH_2;
    }

    /* Close I2S and AUD_PROC is tx is not opened */
    if (!hw_i2s_ptr->tx.path_opened) {
        if (0 == hw_i2s_const->idx) {
            reg->aud_intf_i2s_cfg0 &= ~AUD_PROC_I2S_ENABLE_0;
        } else {
            reg->aud_intf_i2s_cfg1 &= ~AUD_PROC_I2S_ENABLE_1;
        }
        reg->aud_proc_cfg &= ~AUD_PROC_AUD_PROC_ENABLE;
    }

    /* Disable DMA_CX */
    dma_cx_enable_set(hw_i2s_const->rx.dma_ch, false);

    dma_cx_free(hw_i2s_const->rx.dma_ch);
    dma_cx_set_lli_cntr(hw_i2s_const->rx.dma_ch, 0);

    __vsf_hw_i2s_irq_param[hw_i2s_const->rx.dma_ch] = NULL;
}

vsf_err_t vsf_hw_i2s_rx_start(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
    const vsf_hw_i2s_const_t * hw_i2s_const = hw_i2s_ptr->i2s_const;
    VSF_HAL_ASSERT(NULL != hw_i2s_const);
    HWP_AUD_PROC_T *reg = hw_i2s_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    dma_cx_halt_set(hw_i2s_const->rx.dma_ch, false);
    dma_cx_enable_set(hw_i2s_const->rx.dma_ch, true);

    if (AUD_PATH_RX01 == hw_i2s_const->rx.path) {
        reg->aud_proc_rx_cfg_ch01 |= AUD_PROC_RX_EN_CH_0;
    } else {
        reg->aud_proc_rx_cfg_ch23 |= AUD_PROC_RX_EN_CH_2;
    }
    reg->aud_proc_cfg |= AUD_PROC_AUD_PROC_ENABLE;
    if (0 == hw_i2s_const->idx) {
        reg->aud_intf_i2s_cfg0 |= AUD_PROC_I2S_ENABLE_0;
    } else {
        reg->aud_intf_i2s_cfg1 |= AUD_PROC_I2S_ENABLE_1;
    }
    hw_i2s_ptr->rx.path_opened = true;

    return VSF_ERR_NONE;
}

vsf_i2s_capability_t vsf_hw_i2s_capability(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(hw_i2s_ptr != NULL);

    vsf_i2s_capability_t capability = {
        .i2s_capability             = {
            .is_src_supported       = true,
            .is_dbuffer_supported   = true,
        },
    };
    return capability;
}

void vsf_hw_i2s_fini(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);
}

fsm_rt_t vsf_hw_i2s_enable(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_i2s_disable(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);

    return fsm_rt_cpl;
}

vsf_i2s_status_t vsf_hw_i2s_status(vsf_hw_i2s_t *hw_i2s_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_i2s_ptr);

    vsf_i2s_status_t status = { 0 };
    return status;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_I2S_CFG_IMP_PREFIX                  vsf_hw
#define VSF_I2S_CFG_IMP_UPCASE_PREFIX           VSF_HW
#define VSF_I2S_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    static const vsf_hw_i2s_const_t __vsf_hw_i2s ## __IDX ## _const = {         \
        .reg            = VSF_HW_I2S ## __IDX ## _REG,                          \
        .idx            = VSF_HW_I2S ## __IDX ## _IDX,                          \
        .tx.path        = VSF_HW_I2S ## __IDX ## _TXPATH,                       \
        .tx.dma_ch      = VSF_HW_I2S ## __IDX ## _TXDMA_CH,                     \
        .tx.dma_irqn    = VSF_HW_I2S ## __IDX ## _TXDMA_IRQN,                   \
        .rx.path        = VSF_HW_I2S ## __IDX ## _RXPATH,                       \
        .rx.dma_ch      = VSF_HW_I2S ## __IDX ## _RXDMA_CH,                     \
        .rx.dma_irqn    = VSF_HW_I2S ## __IDX ## _RXDMA_IRQN,                   \
        .oclk           = VSF_HW_I2S ## __IDX ## _OCLK,                         \
    };                                                                          \
    vsf_hw_i2s_t vsf_hw_i2s ## __IDX = {                                        \
        .i2s_const      = &__vsf_hw_i2s ## __IDX ## _const,                     \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/i2s/i2s_template.inc"

#endif /* VSF_HAL_USE_I2S */
