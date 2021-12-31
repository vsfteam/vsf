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

#include "../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED

#if VSF_USE_SIMPLE_STREAM == ENABLED
#   define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#endif
#define __VSF_AUDIO_CLASS_IMPLEMENT

#include "utilities/vsf_utilities.h"
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
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vk_audio_init(vk_audio_dev_t *pthis)
{
    vsf_err_t err;
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->drv != NULL)
                &&  (pthis->drv->init != NULL));

    for (uint_fast8_t i = 0; i < pthis->stream_num; i++) {
        pthis->stream[i].stream_index = i;
    }
    __vsf_component_call_peda_ifs(vk_audio_init, err, pthis->drv->init, 0, pthis);
    return err;
}

vsf_err_t vk_audio_control(vk_audio_dev_t *pthis, uint_fast8_t stream_idx,
                    vk_av_control_type_t type, vk_av_control_value_t value)
{
    vsf_err_t err;
    vk_audio_stream_t *audio_stream = &pthis->stream[stream_idx];
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->stream_num >= stream_idx)
                &&  (audio_stream->drv != NULL)
                &&  (audio_stream->drv->control != NULL));

    __vsf_component_call_peda_ifs(vk_audio_control, err, audio_stream->drv->control, 0, pthis,
        .audio_stream   = audio_stream,
        .type           = type,
        .value          = value,
    );
    return err;
}

vsf_err_t vk_audio_start(vk_audio_dev_t *pthis, uint_fast8_t stream_idx,
                    vsf_stream_t *stream, vk_audio_format_t *format)
{
    vsf_err_t err;
    vk_audio_stream_t *audio_stream = &pthis->stream[stream_idx];
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (format != NULL)
                &&  (stream != NULL)
                &&  (pthis->stream_num >= stream_idx)
                &&  (audio_stream->drv != NULL)
                &&  (audio_stream->drv->start != NULL)
                &&  (NULL == audio_stream->stream));

    audio_stream->stream = stream;
    audio_stream->format = *format;
    __vsf_component_call_peda_ifs(vk_audio_start, err, audio_stream->drv->start, 0, pthis,
        .audio_stream   = audio_stream,
    );
    return err;
}

vsf_err_t vk_audio_stop(vk_audio_dev_t *pthis, uint_fast8_t stream_idx)
{
    vsf_err_t err;
    vk_audio_stream_t *audio_stream = &pthis->stream[stream_idx];
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->stream_num >= stream_idx)
                &&  (audio_stream->drv != NULL)
                &&  (audio_stream->drv->stop != NULL));

    __vsf_component_call_peda_ifs(vk_audio_stop, err, audio_stream->drv->stop, 0, pthis,
        .audio_stream   = audio_stream,
    );
    return err;
}

#endif
