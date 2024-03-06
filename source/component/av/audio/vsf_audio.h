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

/*============================ INCLUDES ======================================*/

#include "../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED

#include "kernel/vsf_kernel.h"
// for VSF_HAL_USE_I2S
#include "hal/vsf_hal.h"

#undef PUBLIC_CONST
#if     defined(__VSF_AUDIO_CLASS_IMPLEMENT)
#   undef __VSF_AUDIO_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#   define PUBLIC_CONST
#elif   defined(__VSF_AUDIO_CLASS_INHERIT__)
#   undef __VSF_AUDIO_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#   define PUBLIC_CONST
#else
#   define PUBLIC_CONST         const
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use audio
#endif

#define __describe_audio_ticktock_stream_adapter(__name, __stream_tx, __stream_rx, __frame_size)\
            vsf_audio_ticktock_stream_adapter_t __name = {                      \
                VSF_STREAM_ADAPTER_INIT((__stream_tx), (__stream_rx), (__frame_size), (__frame_size))\
            };

// prototype: describe_audio_ticktock_stream_adapter(__name, __stream_tx, __stream_rx, __frame_size)
#define describe_audio_ticktock_stream_adapter(__name, __stream_tx, __stream_rx, __frame_size)\
            __describe_audio_ticktock_stream_adapter(__name, (__stream_tx), (__stream_rx), __frame_size)

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
#define __VSF_AUDIO_DATA_TYPE_BITLEN(__LEN)             (((((__LEN) - 1) >> 1) & 0x1F) << 3)
#define VSF_AUDIO_DATA_TYPE_BITLEN(__VALUE)             (((((__VALUE) >> 3) & 0x1F) + 1) << 1)

#define VSF_AUDIO_DATA_TYPE_BEUL(__LEN)                 (VSF_AUDIO_DATA_TYPE_BEU | __VSF_AUDIO_DATA_TYPE_BITLEN(__LEN))
#define VSF_AUDIO_DATA_TYPE_BESL(__LEN)                 (VSF_AUDIO_DATA_TYPE_BES | __VSF_AUDIO_DATA_TYPE_BITLEN(__LEN))
#define VSF_AUDIO_DATA_TYPE_LEUL(__LEN)                 (VSF_AUDIO_DATA_TYPE_LEU | __VSF_AUDIO_DATA_TYPE_BITLEN(__LEN))
#define VSF_AUDIO_DATA_TYPE_LESL(__LEN)                 (VSF_AUDIO_DATA_TYPE_LES | __VSF_AUDIO_DATA_TYPE_BITLEN(__LEN))
    VSF_AUDIO_DATA_TYPE_BEU         = VSF_AUDIO_DATA_TYPE_UNSIGNED  | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_BE,
    VSF_AUDIO_DATA_TYPE_BES         = VSF_AUDIO_DATA_TYPE_SIGNED    | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_BE,
    VSF_AUDIO_DATA_TYPE_LEU         = VSF_AUDIO_DATA_TYPE_UNSIGNED  | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_LE,
    VSF_AUDIO_DATA_TYPE_LES         = VSF_AUDIO_DATA_TYPE_SIGNED    | VSF_AUDIO_DATA_TYPE_INT   | VSF_AUDIO_DATA_TYPE_LE,
    VSF_AUDIO_DATA_TYPE_BEU8        = VSF_AUDIO_DATA_TYPE_BEUL(8),
    VSF_AUDIO_DATA_TYPE_BEU16       = VSF_AUDIO_DATA_TYPE_BEUL(16),
    VSF_AUDIO_DATA_TYPE_BEU24       = VSF_AUDIO_DATA_TYPE_BEUL(24),
    VSF_AUDIO_DATA_TYPE_BEU32       = VSF_AUDIO_DATA_TYPE_BEUL(32),
    VSF_AUDIO_DATA_TYPE_BES8        = VSF_AUDIO_DATA_TYPE_BESL(8),
    VSF_AUDIO_DATA_TYPE_BES16       = VSF_AUDIO_DATA_TYPE_BESL(16),
    VSF_AUDIO_DATA_TYPE_BES24       = VSF_AUDIO_DATA_TYPE_BESL(24),
    VSF_AUDIO_DATA_TYPE_BES32       = VSF_AUDIO_DATA_TYPE_BESL(32),
    VSF_AUDIO_DATA_TYPE_LEU8        = VSF_AUDIO_DATA_TYPE_LEUL(8),
    VSF_AUDIO_DATA_TYPE_LEU16       = VSF_AUDIO_DATA_TYPE_LEUL(16),
    VSF_AUDIO_DATA_TYPE_LEU24       = VSF_AUDIO_DATA_TYPE_LEUL(24),
    VSF_AUDIO_DATA_TYPE_LEU32       = VSF_AUDIO_DATA_TYPE_LEUL(32),
    VSF_AUDIO_DATA_TYPE_LES8        = VSF_AUDIO_DATA_TYPE_LESL(8),
    VSF_AUDIO_DATA_TYPE_LES16       = VSF_AUDIO_DATA_TYPE_LESL(16),
    VSF_AUDIO_DATA_TYPE_LES24       = VSF_AUDIO_DATA_TYPE_LESL(24),
    VSF_AUDIO_DATA_TYPE_LES32       = VSF_AUDIO_DATA_TYPE_LESL(32),
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
        } VSF_CAL_PACKED datatype;
        uint8_t                 channel_num;            // eg: 2
        uint16_t                sample_rate;            // in 100Hz, eg: 441/480 means 44100/48000
    } VSF_CAL_PACKED;
    // if value is 0, means adaptive format(from format in vk_audio_start)
    uint32_t                    value;
} vk_audio_format_t;

vsf_dcl_class(vk_audio_dev_t)
vsf_class(vk_audio_stream_t) {
    public_member(
        PUBLIC_CONST uint8_t    stream_index;           // index start from 0
        PUBLIC_CONST uint8_t    dir_in1out0;            // 1 for capture, 0 for playback
        PUBLIC_CONST vk_audio_format_t format;          // format of audio stream buffer
    )
    protected_member(
        const vk_audio_stream_drv_t *drv;
        vsf_stream_t            *stream;
        vk_audio_dev_t          *dev;
    )
};

vsf_class(vk_audio_dev_t) {
    public_member(
        const vk_audio_drv_t    *drv;
        vsf_arch_prio_t         hw_prio;
        uint8_t                 stream_num;
        vk_audio_stream_t       *stream;
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

// for playback, stream_tx should be stream of audio buffer,
//  stream_rx should be stream of ticktock buffer.
// for capture, stream_tx should be stream of ticktock buffer,
//  stream_rx should be stream of audio buffer.
vsf_class(vsf_audio_ticktock_stream_adapter_t) {
    public_member(
        implement(vsf_stream_adapter_t)
    )
    private_member(
        // use eda to prevent pre-emptive of stream_adapter
        vsf_eda_t eda;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_audio_init(vk_audio_dev_t *pthis);

extern vsf_err_t vk_audio_control(vk_audio_dev_t *pthis, uint_fast8_t stream_idx,
            vk_av_control_type_t type, vk_av_control_value_t value);
extern vsf_err_t vk_audio_start(vk_audio_dev_t *pthis, uint_fast8_t stream_idx,
            vsf_stream_t *stream, vk_audio_format_t *format);
extern vsf_err_t vk_audio_stop(vk_audio_dev_t *pthis, uint_fast8_t stream_idx);

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
extern void vsf_audio_playback_ticktock_stream_adapter_init(vsf_audio_ticktock_stream_adapter_t *adapter);
#endif

#if VSF_AUDIO_USE_CAPTURE == ENABLED
extern void vsf_audio_capture_ticktock_stream_adapter_init(vsf_audio_ticktock_stream_adapter_t *adapter);
#endif

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#if VSF_AUDIO_USE_DECODER_WAV == ENABLED
#   include "./decoder/wav/vsf_wav.h"
#endif

#if VSF_AUDIO_USE_WINSOUND == ENABLED
#   include "./driver/winsound/vsf_winsound.h"
#endif

#if VSF_HAL_USE_I2S == ENABLED
#   include "./driver/i2s/vsf_audio_i2s.h"

#   if VSF_AUDIO_USE_AIC1000A == ENABLED
#       include "./driver/aic1000a/vsf_aic1000a.h"
#   endif
#endif

#endif      // VSF_USE_AUDIO
#endif      // __VSF_AUDIO_H__
