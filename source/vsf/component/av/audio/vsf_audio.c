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

#include "../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
#   define VSFSTREAM_CLASS_INHERIT
#endif
#define VSF_AUDIO_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use scsi
#endif

#if VSF_USE_KERNEL_SIMPLE_SHELL != ENABLED
#   error VSF_USE_KERNEL_SIMPLE_SHELL must be enabled
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vk_audio_init(vk_audio_dev_t *pthis)
{
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->drv != NULL)
                &&  (pthis->drv->init != NULL));
    return __vsf_call_eda((uintptr_t)pthis->drv->init, (uintptr_t)pthis);
}

#if VSF_AUDIO_CFG_USE_PLAY == ENABLED
vsf_err_t vk_audio_play_set_volume(vk_audio_dev_t *pthis, uint_fast16_t volume)
{
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->drv != NULL)
                &&  (pthis->drv->play_drv.volume != NULL));
    pthis->ctx.param.volume = volume;
    return __vsf_call_eda((uintptr_t)pthis->drv->play_drv.volume, (uintptr_t)pthis);
}

vsf_err_t vk_audio_play_set_mute(vk_audio_dev_t *pthis, bool mute)
{
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->drv != NULL)
                &&  (pthis->drv->play_drv.mute != NULL));
    pthis->ctx.param.mute = mute;
    return __vsf_call_eda((uintptr_t)pthis->drv->play_drv.mute, (uintptr_t)pthis);
}

vsf_err_t vk_audio_play_start(vk_audio_dev_t *pthis, vsf_stream_t *stream, vk_audio_format_t *format)
{
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (format != NULL)
                &&  (stream != NULL)
                &&  (pthis->drv != NULL)
                &&  (pthis->drv->play_drv.play != NULL)
                &&  (NULL == pthis->stream_play));
    pthis->ctx.param.format = *format;
    pthis->stream_play = stream;
    return __vsf_call_eda((uintptr_t)pthis->drv->play_drv.play, (uintptr_t)pthis);
}

vsf_err_t vk_audio_play_pause(vk_audio_dev_t *pthis)
{
    // TODO:
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vk_audio_play_stop(vk_audio_dev_t *pthis)
{
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->drv != NULL)
                &&  (pthis->drv->play_drv.stop != NULL));
    return __vsf_call_eda((uintptr_t)pthis->drv->play_drv.stop, (uintptr_t)pthis);
}
#endif

#endif
