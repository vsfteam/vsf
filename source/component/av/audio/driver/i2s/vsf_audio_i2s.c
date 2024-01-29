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

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#include "../../../vsf_av_cfg.h"
// for VSF_HAL_USE_I2S
#include "hal/vsf_hal.h"

#if VSF_USE_AUDIO == ENABLED && VSF_HAL_USE_I2S == ENABLED

#define __VSF_AUDIO_CLASS_INHERIT__
#define __VSF_AUDIO_I2S_CLASS_IMPLEMENT

#include "service/vsf_service.h"
#include "component/av/vsf_av.h"
#include "./vsf_audio_i2s.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __vk_audio_i2s_init)

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
dcl_vsf_peda_methods(static, __vk_audio_i2s_playback_start)
dcl_vsf_peda_methods(static, __vk_audio_i2s_playback_stop)
#endif

#if VSF_AUDIO_USE_CAPTURE == ENABLED
dcl_vsf_peda_methods(static, __vk_audio_i2s_capture_start)
dcl_vsf_peda_methods(static, __vk_audio_i2s_capture_stop)
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vk_audio_drv_t vk_audio_i2s_drv = {
    .init       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_i2s_init),
};

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
const vk_audio_stream_drv_t vk_audio_i2s_stream_drv_playback = {
    .start      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_i2s_playback_start),
    .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_i2s_playback_stop),
};
#endif

#if VSF_AUDIO_USE_CAPTURE == ENABLED
const vk_audio_stream_drv_t vk_audio_i2s_stream_drv_capture = {
    .start      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_i2s_capture_start),
    .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_audio_i2s_capture_stop),
};
#endif

/*============================ IMPLEMENTATION ================================*/

static void __vk_audio_i2s_isrhandler(void *target_ptr, vsf_i2s_t *i2s_ptr, vsf_i2s_irq_mask_t irq_mask)
{
    vk_audio_stream_t *audio_stream = target_ptr;
    vsf_stream_t *stream = audio_stream->stream;
    uint32_t buffsize = vsf_stream_get_buff_size(audio_stream->stream);

    if (audio_stream->dir_in1out0) {
        // capture/rx
#if VSF_AUDIO_USE_CAPTURE == ENABLED
        vsf_stream_write(stream, NULL, buffsize >> 1);
#endif
    } else {
        // playback/tx
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
        vsf_stream_read(stream, NULL, buffsize >> 1);
#endif
    }
}

static void __vk_audio_i2s_stream_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vk_audio_stream_t *audio_stream = param;
    vk_audio_i2s_dev_t *dev = (vk_audio_i2s_dev_t *)audio_stream->dev;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
    __try_start_stream:
#endif
        {
            vsf_i2s_cfg_t i2s_cfg;
            if (audio_stream->dir_in1out0) {
                // capture/rx
#if VSF_AUDIO_USE_CAPTURE == ENABLED
                if (vsf_stream_get_data_size(stream) != 0) {
                    vsf_trace_error("stream for i2s_rx is not empty\n");
                    return;
                }
                i2s_cfg.mode = 16 == dev->capture.sample_bitlen ? VSF_I2S_DATA_BITLEN_16 : VSF_I2S_DATA_BITLEN_24;
                i2s_cfg.data_sample_rate = dev->capture.sample_rate;
                i2s_cfg.hw_sample_rate = dev->capture.hw_sample_rate != 0 ? dev->capture.hw_sample_rate : dev->capture.sample_rate;
                i2s_cfg.channel_num = dev->capture.channel_num;
                i2s_cfg.buffer_size = vsf_stream_get_wbuf(stream, &i2s_cfg.buffer);
                i2s_cfg.isr.handler_fn = __vk_audio_i2s_isrhandler;
                i2s_cfg.isr.target_ptr = audio_stream;
                i2s_cfg.isr.prio = dev->arch_prio;
                if (VSF_ERR_NONE != vsf_i2s_rx_init(dev->i2s, &i2s_cfg)) {
                    vsf_trace_error("fail to initialize i2s_rx\n");
                } else {
                    vsf_i2s_rx_start(dev->i2s);
                    dev->capture.stream_started = true;
                }
#endif
            } else {
                // playback/tx
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
                // wait data to be filled for tick-tock operation
                if (vsf_stream_get_free_size(stream) != 0) {
                    // dummy read to accept next VSF_STREAM_ON_IN event
                    vsf_stream_read(stream, NULL, 0);
                    return;
                }
                i2s_cfg.mode = 16 == dev->playback.sample_bitlen ? VSF_I2S_DATA_BITLEN_16 : VSF_I2S_DATA_BITLEN_24;
                i2s_cfg.data_sample_rate = dev->playback.sample_rate;
                i2s_cfg.hw_sample_rate = dev->playback.hw_sample_rate != 0 ? dev->playback.hw_sample_rate : dev->playback.sample_rate;
                i2s_cfg.channel_num = dev->playback.channel_num;
                i2s_cfg.buffer_size = vsf_stream_get_rbuf(stream, &i2s_cfg.buffer);
                i2s_cfg.isr.handler_fn = __vk_audio_i2s_isrhandler;
                i2s_cfg.isr.target_ptr = audio_stream;
                i2s_cfg.isr.prio = dev->arch_prio;
                if (VSF_ERR_NONE != vsf_i2s_tx_init(dev->i2s, &i2s_cfg)) {
                    vsf_trace_error("fail to initialize i2s_tx\n");
                } else {
                    vsf_i2s_tx_start(dev->i2s);
                    dev->playback.stream_started = true;
                }
#endif
            }
        }
        break;
    case VSF_STREAM_ON_DISCONNECT:
        if (audio_stream->dir_in1out0) {
#if VSF_AUDIO_USE_CAPTURE == ENABLED
            vsf_i2s_rx_fini(dev->i2s);
            dev->capture.stream_started = false;
#endif
        } else {
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
            vsf_i2s_tx_fini(dev->i2s);
            dev->playback.stream_started = false;
#endif
        }
        break;
    case VSF_STREAM_ON_OUT:
        break;
    case VSF_STREAM_ON_IN:
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
        if (!dev->playback.stream_started) {
            goto __try_start_stream;
        }
#endif
        break;
    }
}

__vsf_component_peda_ifs_entry(__vk_audio_i2s_init, vk_audio_init)
{
    vsf_peda_begin();
    vk_audio_i2s_dev_t *dev = vsf_container_of(&vsf_this, vk_audio_i2s_dev_t, use_as__vk_audio_dev_t);

    if (0 == dev->stream_num) {
        dev->stream_num = 0;
        dev->stream = dev->__stream;
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
        dev->stream[dev->stream_num].stream_index = dev->stream_num;
        dev->stream[dev->stream_num].dir_in1out0 = 0;
        dev->stream[dev->stream_num].format.value = 0;
        dev->stream[dev->stream_num].drv = &vk_audio_i2s_stream_drv_playback;
        dev->stream[dev->stream_num].dev = &dev->use_as__vk_audio_dev_t;
        dev->stream_num++;
#endif
#if VSF_AUDIO_USE_CAPTURE == ENABLED
        dev->stream[dev->stream_num].stream_index = dev->stream_num;
        dev->stream[dev->stream_num].dir_in1out0 = 1;
        dev->stream[dev->stream_num].format.value = 0;
        dev->stream[dev->stream_num].drv = &vk_audio_i2s_stream_drv_capture;
        dev->stream[dev->stream_num].dev = &dev->use_as__vk_audio_dev_t;
        dev->stream_num++;
#endif
    }

    vsf_eda_return(vsf_i2s_init(dev->i2s, NULL));
    vsf_peda_end();
}

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
__vsf_component_peda_ifs_entry(__vk_audio_i2s_playback_start, vk_audio_start)
{
    vsf_peda_begin();
    vk_audio_i2s_dev_t *dev = vsf_container_of(&vsf_this, vk_audio_i2s_dev_t, use_as__vk_audio_dev_t);
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;
    uint8_t channel_num = audio_stream->format.channel_num;
    uint16_t sample_rate = audio_stream->format.sample_rate;
    uint8_t bitlen = VSF_AUDIO_DATA_TYPE_BITLEN(audio_stream->format.datatype.value);

    dev->playback.channel_num = channel_num;
    dev->playback.sample_bitlen = bitlen;
    dev->playback.sample_rate = sample_rate;
    audio_stream->stream->rx.evthandler = __vk_audio_i2s_stream_evthandler;
    audio_stream->stream->rx.param = audio_stream;
    vsf_stream_connect_rx(audio_stream->stream);

    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_audio_i2s_playback_stop, vk_audio_stop)
{
    vsf_peda_begin();
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;

    // TODO: make sure play.stream will not be used
    audio_stream->stream = NULL;
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}
#endif

#if VSF_AUDIO_USE_CAPTURE == ENABLED
__vsf_component_peda_ifs_entry(__vk_audio_i2s_capture_start, vk_audio_start)
{
    vsf_peda_begin();
    vk_audio_i2s_dev_t *dev = vsf_container_of(&vsf_this, vk_audio_i2s_dev_t, use_as__vk_audio_dev_t);
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;
    uint8_t channel_num = audio_stream->format.channel_num;
    uint16_t sample_rate = audio_stream->format.sample_rate;
    uint8_t bitlen = VSF_AUDIO_DATA_TYPE_BITLEN(audio_stream->format.datatype.value);

    dev->capture.channel_num = channel_num;
    dev->capture.sample_bitlen = bitlen;
    dev->capture.sample_rate = sample_rate;
    audio_stream->stream->tx.evthandler = __vk_audio_i2s_stream_evthandler;
    audio_stream->stream->tx.param = audio_stream;
    vsf_stream_connect_tx(audio_stream->stream);

    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_audio_i2s_capture_stop, vk_audio_stop)
{
    vsf_peda_begin();
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;

    // TODO: make sure capture.stream will not be used
    audio_stream->stream = NULL;
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}
#endif

#endif
