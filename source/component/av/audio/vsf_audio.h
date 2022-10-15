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

#ifndef __VSF_AUDIO_H__
#define __VSF_AUDIO_H__

#include "../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED

#include "kernel/vsf_kernel.h"

#if     defined(__VSF_AUDIO_CLASS_IMPLEMENT)
#   undef __VSF_AUDIO_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_AUDIO_CLASS_INHERIT__)
#   undef __VSF_AUDIO_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_audio_stream_drv_t {
    vsf_peda_evthandler_t       control;
    vsf_peda_evthandler_t       start;
    vsf_peda_evthandler_t       stop;
} vk_audio_stream_drv_t;

vsf_class(vk_audio_drv_t) {
    protected_member(
        vsf_peda_evthandler_t   init;
    )
};

typedef enum vk_audio_data_type_t {
    VSF_AUDIO_DATA_TYPE_FLOAT       = 1 << 0,
    VSF_AUDIO_DATA_TYPE_INT         = 0 << 0,
#define VSF_AUDIO_DATA_TYPE_IS_FLOAT(__VALUE)           ((__VALUE) & VSF_AUDIO_DATA_TYPE_FLOAT)
#define VSF_AUDIO_DATA_TYPE_IS_INT(__VALUE)             !VSF_AUDIO_DATA_TYPE_IS_FLOAT(__VALUE)

    VSF_AUDIO_DATA_TYPE_SIGNED      = 1 << 1,
    VSF_AUDIO_DATA_TYPE_UNSIGNED    = 0 << 1,
#define VSF_AUDIO_DATA_TYPE_IS_SIGNED(__VALUE)          ((__VALUE) & VSF_AUDIO_DATA_TYPE_SIGNED)
#define VSF_AUDIO_DATA_TYPE_IS_UNSIGNED(__VALUE)        !VSF_AUDIO_DATA_TYPE_IS_SIGNED(__VALUE)

    VSF_AUDIO_DATA_TYPE_BE          = 1 << 2,
    VSF_AUDIO_DATA_TYPE_LE          = 0 << 2,
#define VSF_AUDIO_DATA_TYPE_IS_BE(__VALUE)              ((__VALUE) & VSF_AUDIO_DATA_TYPE_BE)
#define VSF_AUDIO_DATA_TYPE_IS_LE(__VALUE)              !VSF_AUDIO_DATA_TYPE_IS_LE(__VALUE)

// __LEN: [1 .. 64]
#define __VSF_AUDIO_DATA_TYPE_BITLEN(__LEN)              (((((__LEN) - 1) >> 1) & 0x1F) << 3)
#define VSF_AUDIO_DATA_TYPE_BITLEN(__VALUE)              (((((__VALUE) >> 3) & 0x1F) + 1) << 1)

    VSF_AUDIO_DATA_TYPE_BEU8        = __VSF_AUDIO_DATA_TYPE_BITLEN(8)   | VSF_AUDIO_DATA_TYPE_UNSIGNED  | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_BE,
    VSF_AUDIO_DATA_TYPE_BEU16       = __VSF_AUDIO_DATA_TYPE_BITLEN(16)  | VSF_AUDIO_DATA_TYPE_UNSIGNED  | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_BE,
    VSF_AUDIO_DATA_TYPE_BEU24       = __VSF_AUDIO_DATA_TYPE_BITLEN(24)  | VSF_AUDIO_DATA_TYPE_UNSIGNED  | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_BE,
    VSF_AUDIO_DATA_TYPE_BEU32       = __VSF_AUDIO_DATA_TYPE_BITLEN(32)  | VSF_AUDIO_DATA_TYPE_UNSIGNED  | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_BE,
    VSF_AUDIO_DATA_TYPE_BES8        = __VSF_AUDIO_DATA_TYPE_BITLEN(8)   | VSF_AUDIO_DATA_TYPE_SIGNED    | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_BE,
    VSF_AUDIO_DATA_TYPE_BES16       = __VSF_AUDIO_DATA_TYPE_BITLEN(16)  | VSF_AUDIO_DATA_TYPE_SIGNED    | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_BE,
    VSF_AUDIO_DATA_TYPE_BES24       = __VSF_AUDIO_DATA_TYPE_BITLEN(24)  | VSF_AUDIO_DATA_TYPE_SIGNED    | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_BE,
    VSF_AUDIO_DATA_TYPE_BES32       = __VSF_AUDIO_DATA_TYPE_BITLEN(32)  | VSF_AUDIO_DATA_TYPE_SIGNED    | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_BE,
    VSF_AUDIO_DATA_TYPE_LEU8        = __VSF_AUDIO_DATA_TYPE_BITLEN(8)   | VSF_AUDIO_DATA_TYPE_UNSIGNED  | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_LE,
    VSF_AUDIO_DATA_TYPE_LEU16       = __VSF_AUDIO_DATA_TYPE_BITLEN(16)  | VSF_AUDIO_DATA_TYPE_UNSIGNED  | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_LE,
    VSF_AUDIO_DATA_TYPE_LEU24       = __VSF_AUDIO_DATA_TYPE_BITLEN(24)  | VSF_AUDIO_DATA_TYPE_UNSIGNED  | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_LE,
    VSF_AUDIO_DATA_TYPE_LEU32       = __VSF_AUDIO_DATA_TYPE_BITLEN(32)  | VSF_AUDIO_DATA_TYPE_UNSIGNED  | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_LE,
    VSF_AUDIO_DATA_TYPE_LES8        = __VSF_AUDIO_DATA_TYPE_BITLEN(8)   | VSF_AUDIO_DATA_TYPE_SIGNED    | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_LE,
    VSF_AUDIO_DATA_TYPE_LES16       = __VSF_AUDIO_DATA_TYPE_BITLEN(16)  | VSF_AUDIO_DATA_TYPE_SIGNED    | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_LE,
    VSF_AUDIO_DATA_TYPE_LES24       = __VSF_AUDIO_DATA_TYPE_BITLEN(24)  | VSF_AUDIO_DATA_TYPE_SIGNED    | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_LE,
    VSF_AUDIO_DATA_TYPE_LES32       = __VSF_AUDIO_DATA_TYPE_BITLEN(32)  | VSF_AUDIO_DATA_TYPE_SIGNED    | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_LE,
} vk_audio_data_type_t;

typedef union vk_audio_format_t {
    struct {
        union {
            struct {
                uint8_t         is_float : 1;
                uint8_t         is_signed : 1;
                uint8_t         is_be : 1;
                uint8_t         __bitlen : 5;           // please use VSF_AUDIO_DATA_TYPE_BITLEN to get real bitwidth
            };
            uint8_t             value;
        } PACKED datatype;
        uint8_t                 channel_num;            // eg: 2
        uint16_t                sample_rate;            // eg: 44100/48000
    } PACKED;
    // if value is 0, means adaptive format(from format in vk_audio_start)
    uint32_t                    value;
} vk_audio_format_t;

typedef struct vk_audio_stream_t {
    uint8_t                     stream_index;           // index start from 0
    uint8_t                     dir_in1out0;            // 1 for capture, 0 for playback

    vk_audio_format_t           format;
    const vk_audio_stream_drv_t *drv;
    vsf_stream_t                *stream;
} vk_audio_stream_t;

vsf_class(vk_audio_dev_t) {
    public_member(
        const vk_audio_drv_t    *drv;
        vsf_arch_prio_t         hw_prio;
        uint8_t                 stream_num;
        vk_audio_stream_t       stream[0];
    )
};

__vsf_component_peda_ifs(vk_audio_init)
__vsf_component_peda_ifs(vk_audio_control,
    vk_audio_stream_t           *audio_stream;
    vk_av_control_type_t        type;
    vk_av_control_value_t       value;
)
__vsf_component_peda_ifs(vk_audio_start,
    vk_audio_stream_t           *audio_stream;
)
__vsf_component_peda_ifs(vk_audio_stop,
    vk_audio_stream_t           *audio_stream;
)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_audio_init(vk_audio_dev_t *pthis);

extern vsf_err_t vk_audio_control(vk_audio_dev_t *pthis, uint_fast8_t stream_idx,
            vk_av_control_type_t type, vk_av_control_value_t value);
extern vsf_err_t vk_audio_start(vk_audio_dev_t *pthis, uint_fast8_t stream_idx,
            vsf_stream_t *stream, vk_audio_format_t *format);
extern vsf_err_t vk_audio_stop(vk_audio_dev_t *pthis, uint_fast8_t stream_idx);

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#if VSF_AUDIO_USE_DECODER_WAV == ENABLED
#   include "./decoder/wav/vsf_wav.h"
#endif

#endif      // VSF_USE_AUDIO
#endif      // __VSF_AUDIO_H__
