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

#if VSF_USE_SIMPLE_STREAM == ENABLED
#   define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#endif
#define __VSF_AUDIO_CLASS_IMPLEMENT

#include "utilities/vsf_utilities.h"
#include "component/vsf_component.h"

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

    __vsf_component_call_peda_ifs(vk_audio_init, err, pthis->drv->init, 0, pthis);
    return err;
}

#if VSF_AUDIO_USE_PLAY == ENABLED
vsf_err_t vk_audio_play_set_volume(vk_audio_dev_t *pthis, uint_fast16_t volume)
{
    vsf_err_t err;
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->drv != NULL)
                &&  (pthis->drv->play_drv.volume != NULL));

    __vsf_component_call_peda_ifs(vk_audio_play_set_volume, err, pthis->drv->play_drv.volume, 0, pthis,
        .volume = volume,
    );
    return err;
}

vsf_err_t vk_audio_play_set_mute(vk_audio_dev_t *pthis, bool mute)
{
    vsf_err_t err;
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->drv != NULL)
                &&  (pthis->drv->play_drv.mute != NULL));

    __vsf_component_call_peda_ifs(vk_audio_play_set_mute, err, pthis->drv->play_drv.mute, 0, pthis,
        .mute = mute,
    );
    return err;
}

vsf_err_t vk_audio_play_start(vk_audio_dev_t *pthis, vsf_stream_t *stream, vk_audio_format_t *format)
{
    vsf_err_t err;
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (format != NULL)
                &&  (stream != NULL)
                &&  (pthis->drv != NULL)
                &&  (pthis->drv->play_drv.play != NULL)
                &&  (NULL == pthis->play.stream));

    pthis->play.stream = stream;
    pthis->play.format = *format;
    __vsf_component_call_peda_ifs(vk_audio_play_start, err, pthis->drv->play_drv.play, 0, pthis);
    return err;
}

vsf_err_t vk_audio_play_pause(vk_audio_dev_t *pthis)
{
    // TODO:
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vk_audio_play_stop(vk_audio_dev_t *pthis)
{
    vsf_err_t err;
    VSF_AV_ASSERT(  (pthis != NULL)
                &&  (pthis->drv != NULL)
                &&  (pthis->drv->play_drv.stop != NULL));

    __vsf_component_call_peda_ifs(vk_audio_play_stop, err, pthis->drv->play_drv.stop, 0, pthis);
    return err;
}
#endif

#endif
