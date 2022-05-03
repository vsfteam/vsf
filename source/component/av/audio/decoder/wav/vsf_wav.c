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

#include "../../../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED && VSF_AUDIO_USE_DECODER_WAV == ENABLED

#if VSF_USE_SIMPLE_STREAM == ENABLED
#   define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#endif
#define __VSF_WAV_CLASS_IMPLEMENT
#define __VSF_EDA_CLASS_INHERIT__
#include "kernel/vsf_kernel.h"
#include "component/av/vsf_av.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use scsi
#endif

#if VSF_KERNEL_USE_SIMPLE_SHELL != ENABLED
#   error VSF_KERNEL_USE_SIMPLE_SHELL must be enabled
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_EVT_PARSE_DONE = VSF_EVT_USER + 0,
};

typedef struct vk_wav_header_t {
    char        chunk_id[4];
    uint32_t    chunk_size;
} vk_wav_header_t;

typedef struct vk_wav_riff_t {
    char        chunk_id[4];    // "RIFF"
    uint32_t    size;           // all size except chunk_id and size(file_size - 8)
    char        format[4];      // "WAVE"
} PACKED vk_wav_riff_t;

typedef struct vk_wav_format_t {
    char        sub_chunk_id[4];// "fmt "
    uint32_t    sub_chunk_size; // 16 for PCM
    uint16_t    format;         // 1: PCM
    uint16_t    channel_number;
    uint32_t    sample_rate;
    uint32_t    byte_rate;
    uint16_t    block_align;
    uint16_t    bit_width;
} PACKED vk_wav_format_t;

typedef struct vk_wav_data_t {
    char        sub_chunk_id[4];// "data"
    uint32_t    sub_chunk_size;
} PACKED vk_wav_data_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vk_wav_playback_stream_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vk_wav_t *wav = param;
    uint32_t data_size;
    union {
        vk_wav_header_t header;
        vk_wav_riff_t   riff;
        vk_wav_format_t format;
        vk_wav_data_t   data;
    } header;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN:
        while (1) {
            data_size = vsf_stream_get_data_size(stream);
            if (!data_size) {
                break;
            }

            switch (wav->state) {
            case VSF_WAV_STATE_RIFF:
                if (data_size >= sizeof(header.riff)) {
                    vsf_stream_read(stream, (uint8_t *)&header.riff, sizeof(header.riff));
                    if (    strncmp(header.riff.chunk_id, "RIFF", 4)
                        ||  strncmp(header.riff.format, "WAVE", 4)) {
                        goto failed;
                    }
                    wav->state = VSF_WAV_STATE_FORMAT;
                    break;
                }
                break;
            case VSF_WAV_STATE_FORMAT:
                if (data_size >= sizeof(header.format)) {
                    vsf_stream_read(stream, (uint8_t *)&header.format, sizeof(header.format));
                    if (    strncmp(header.format.sub_chunk_id, "fmt ", 4)
                        ||  (header.format.sub_chunk_size != 16)) {
                        goto failed;
                    }
                    wav->format.channel_num = header.format.channel_number;
                    wav->format.sample_bit_width = header.format.bit_width;
                    wav->format.sample_rate = header.format.sample_rate;
                    wav->state = VSF_WAV_STATE_SEARCH_DATA_TRUNK;
                    break;
                }
                break;
            case VSF_WAV_STATE_SEARCH_DATA_TRUNK:
                if (data_size >= sizeof(header.header)) {
                    vsf_stream_read(stream, (uint8_t *)&header.header, sizeof(header.header));
                    if (strncmp(header.header.chunk_id, "data", 4)) {
                        wav->skip_size = header.header.chunk_size;
                        wav->state = VSF_WAV_STATE_SKIP_TRUNK;
                        break;
                    }
                    wav->state = VSF_WAV_STATE_PLAYBACK;
                    wav->result = VSF_ERR_NONE;
                    vsf_eda_post_evt(&wav->eda, VSF_EVT_PARSE_DONE);
                    return;
                }
                break;
            case VSF_WAV_STATE_SKIP_TRUNK:
                if (data_size > 0) {
                    uint32_t cursize = vsf_min(data_size, wav->skip_size);
                    wav->skip_size -= cursize;
                    vsf_stream_read(stream, NULL, cursize);

                    if (0 == wav->skip_size) {
                        wav->state = VSF_WAV_STATE_SEARCH_DATA_TRUNK;
                    }
                }
                break;
            case VSF_WAV_STATE_PLAYBACK:
                return;
            }
        }
        break;
    }

    return;
failed:
    wav->result = VSF_ERR_FAIL;
    vsf_eda_post_evt(&wav->eda, VSF_EVT_PARSE_DONE);
}

static void __vk_wav_playback_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_wav_t *wav = container_of(eda, vk_wav_t, eda);

    switch (evt) {
    case VSF_EVT_INIT:
        wav->stream->rx.param = wav;
        wav->stream->rx.evthandler = __vk_wav_playback_stream_evthandler;
        vsf_stream_connect_rx(wav->stream);
        break;
    case VSF_EVT_PARSE_DONE:
        vsf_stream_disconnect_rx(wav->stream);
        vk_audio_start(wav->audio_dev, wav->audio_stream, wav->stream, &wav->format);
        break;
    }
}

vsf_err_t vk_wav_playback_start(vk_wav_t *wav)
{
    VSF_AV_ASSERT(  (wav != NULL)
                &&  (wav->audio_dev != NULL)
                &&  (wav->stream != NULL));
    wav->state = VSF_WAV_STATE_RIFF;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    wav->eda.on_terminate = NULL;
#endif
    wav->eda.fn.evthandler = __vk_wav_playback_evthandler;
    return vsf_eda_init(&wav->eda);
}

vsf_err_t vk_wav_playback_stop(vk_wav_t *wav)
{
    vsf_stream_disconnect_rx(wav->stream);
    vsf_eda_fini(&wav->eda);
    if (VSF_WAV_STATE_PLAYBACK == wav->state) {
        vk_audio_stop(wav->audio_dev, wav->audio_stream);
    }
    return VSF_ERR_NONE;
}

#endif
