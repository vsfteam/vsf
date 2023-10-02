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

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_DEVFS == ENABLED && VSF_LINUX_DEVFS_USE_ALSA == ENABLED

#define __VSF_FS_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#define __VSF_AUDIO_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#   include "shell/sys/linux/include/errno.h"
#else
#   include <unistd.h>
#   include <errno.h>
#endif
#include <limits.h>
#include <stdint.h>

#ifndef __kernel_off_t
#   define __kernel_off_t           off_t
#endif
#ifndef __kernel_pid_t
#   define __kernel_pid_t           pid_t
#endif
#ifndef __bitwise
#   define __bitwise
#endif

#include <sound/type_compat.h>
#include <sound/uapi/asound.h>

/*============================ MACROS ========================================*/

#ifndef VSF_LINUX_AUDIO_CFG_MIN_PERIOD_US
#   define VSF_LINUX_AUDIO_CFG_MIN_PERIOD_US            (20 * 1000)
#endif
#ifndef VSF_LINUX_AUDIO_CFG_MAX_PERIOD_US
#   define VSF_LINUX_AUDIO_CFG_MAX_PERIOD_US            (200 * 1000)
#endif

// periods count in buffer
#ifndef VSF_LINUX_AUDIO_CFG_MIN_PERIODS_CNT
#   define VSF_LINUX_AUDIO_CFG_MIN_PERIODS_CNT          2
#endif
#ifndef VSF_LINUX_AUDIO_CFG_MAX_PERIODS_CNT
#   define VSF_LINUX_AUDIO_CFG_MAX_PERIODS_CNT          4
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct snd_pcm_hw_constraints {
    struct snd_mask masks[SNDRV_PCM_HW_PARAM_LAST_MASK - SNDRV_PCM_HW_PARAM_FIRST_MASK + 1];
    struct snd_interval intervals[SNDRV_PCM_HW_PARAM_LAST_INTERVAL - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL + 1];
};

struct pcm_format_data {
    unsigned char width;
    unsigned char phys;
    signed char le;
    signed char signd;
};

struct __snd_pcm_hw_rule;
typedef int (*__snd_pcm_hw_rule_func_t)(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int mask, struct __snd_pcm_hw_rule *rule);

struct __snd_pcm_hw_rule {
    unsigned int cond;
    unsigned int dep_mask;
    __snd_pcm_hw_rule_func_t func;
};

typedef struct vsf_linux_audio_control_priv_t {
    implement(vsf_linux_fs_priv_t)
} vsf_linux_audio_control_priv_t;

typedef struct vsf_linux_audio_play_priv_t {
    implement(vsf_linux_stream_priv_t)
    implement(vsf_fifo_stream_t)

    struct snd_pcm_hw_constraints hw_constraints;
    struct snd_pcm_hw_params hw_params;
    struct {
        struct snd_pcm_mmap_control control;
        struct snd_pcm_mmap_status status;
    } mmap;
    bool is_started;
} vsf_linux_audio_play_priv_t;

typedef struct vsf_linux_audio_capture_priv_t {
    implement(vsf_linux_stream_priv_t)
} vsf_linux_audio_capture_priv_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void __vsf_linux_rx_stream_init(vsf_linux_stream_priv_t *priv_tx);
extern void __vsf_linux_tx_stream_init(vsf_linux_stream_priv_t *priv_rx);
extern void __vsf_linux_tx_stream_drain(vsf_linux_stream_priv_t *priv_tx);
extern void __vsf_linux_tx_stream_fini(vsf_linux_stream_priv_t *priv_tx);
extern void __vsf_linux_rx_stream_fini(vsf_linux_stream_priv_t *priv_rx);

extern ssize_t __vsf_linux_stream_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
extern ssize_t __vsf_linux_stream_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);

static void __vsf_linux_audio_play_init(vsf_linux_fd_t *sfd);
static int __vsf_linux_audio_play_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);

static int __vsf_linux_audio_control_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);

static int snd_pcm_hw_rule_format(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);
static int snd_pcm_hw_rule_sample_bits(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);
static int snd_pcm_hw_rule_frame_bits(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);
static int snd_pcm_hw_rule_channels(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);
static int snd_pcm_hw_rule_rate(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);
static int snd_pcm_hw_rule_period_time(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);
static int snd_pcm_hw_rule_period_size(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);
static int snd_pcm_hw_rule_period_bytes(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);
static int snd_pcm_hw_rule_periods(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);
static int snd_pcm_hw_rule_buffer_time(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);
static int snd_pcm_hw_rule_buffer_size(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);
static int snd_pcm_hw_rule_buffer_bytes(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule);

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_linux_fd_op_t __vsf_linux_audio_control_fdop = {
    .priv_size          = sizeof(vsf_linux_audio_control_priv_t),
    .fn_fcntl           = __vsf_linux_audio_control_fcntl,
};

static const vsf_linux_fd_op_t __vsf_linux_audio_play_fdop = {
    .priv_size          = sizeof(vsf_linux_audio_play_priv_t),
    .fn_init            = __vsf_linux_audio_play_init,
    .fn_write           = __vsf_linux_stream_write,
    .fn_fcntl           = __vsf_linux_audio_play_fcntl,
};

static const vsf_linux_fd_op_t __vsf_linux_audio_capture_fdop = {
    .priv_size          = sizeof(vsf_linux_audio_capture_priv_t),
};

static const struct pcm_format_data pcm_formats[SNDRV_PCM_FORMAT_FLOAT64_BE + 1] = {
    [SNDRV_PCM_FORMAT_S8] = {
        .width = 8, .phys = 8, .le = -1, .signd = 1,
    },
    [SNDRV_PCM_FORMAT_U8] = {
        .width = 8, .phys = 8, .le = -1, .signd = 0,
    },
    [SNDRV_PCM_FORMAT_S16_LE] = {
        .width = 16, .phys = 16, .le = 1, .signd = 1,
    },
    [SNDRV_PCM_FORMAT_S16_BE] = {
        .width = 16, .phys = 16, .le = 0, .signd = 1,
    },
    [SNDRV_PCM_FORMAT_U16_LE] = {
        .width = 16, .phys = 16, .le = 1, .signd = 0,
    },
    [SNDRV_PCM_FORMAT_U16_BE] = {
        .width = 16, .phys = 16, .le = 0, .signd = 0,
    },
    [SNDRV_PCM_FORMAT_S24_LE] = {
        .width = 24, .phys = 32, .le = 1, .signd = 1,
    },
    [SNDRV_PCM_FORMAT_S24_BE] = {
        .width = 24, .phys = 32, .le = 0, .signd = 1,
    },
    [SNDRV_PCM_FORMAT_U24_LE] = {
        .width = 24, .phys = 32, .le = 1, .signd = 0,
    },
    [SNDRV_PCM_FORMAT_U24_BE] = {
        .width = 24, .phys = 32, .le = 0, .signd = 0,
    },
    [SNDRV_PCM_FORMAT_S32_LE] = {
        .width = 32, .phys = 32, .le = 1, .signd = 1,
    },
    [SNDRV_PCM_FORMAT_S32_BE] = {
        .width = 32, .phys = 32, .le = 0, .signd = 1,
    },
    [SNDRV_PCM_FORMAT_U32_LE] = {
        .width = 32, .phys = 32, .le = 1, .signd = 0,
    },
    [SNDRV_PCM_FORMAT_U32_BE] = {
        .width = 32, .phys = 32, .le = 0, .signd = 0,
    },
    [SNDRV_PCM_FORMAT_FLOAT_LE] = {
        .width = 32, .phys = 32, .le = 1, .signd = -1,
    },
    [SNDRV_PCM_FORMAT_FLOAT_BE] = {
        .width = 32, .phys = 32, .le = 0, .signd = -1,
    },
    [SNDRV_PCM_FORMAT_FLOAT64_LE] = {
        .width = 64, .phys = 64, .le = 1, .signd = -1,
    },
    [SNDRV_PCM_FORMAT_FLOAT64_BE] = {
        .width = 64, .phys = 64, .le = 0, .signd = -1,
    },
};

static const struct __snd_pcm_hw_rule pcm_rules[SNDRV_PCM_HW_PARAM_LAST_INTERVAL + 1] = {
    [SNDRV_PCM_HW_PARAM_FORMAT] = {
        .dep_mask               = 1 << SNDRV_PCM_HW_PARAM_SAMPLE_BITS,
        .func                   = snd_pcm_hw_rule_format,
    },
    [SNDRV_PCM_HW_PARAM_SAMPLE_BITS] = {
        .dep_mask               = (1 << SNDRV_PCM_HW_PARAM_FORMAT)
                                | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS) | (1 << SNDRV_PCM_HW_PARAM_CHANNELS),
        .func                   = snd_pcm_hw_rule_sample_bits,
    },
    [SNDRV_PCM_HW_PARAM_FRAME_BITS] = {
        .dep_mask               = (1 << SNDRV_PCM_HW_PARAM_SAMPLE_BITS) | (1 << SNDRV_PCM_HW_PARAM_CHANNELS)
                                | (1 << SNDRV_PCM_HW_PARAM_PERIOD_BYTES) | (1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE)
                                | (1 << SNDRV_PCM_HW_PARAM_BUFFER_BYTES) | (1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE),
        .func                   = snd_pcm_hw_rule_frame_bits,
    },
    [SNDRV_PCM_HW_PARAM_CHANNELS] = {
        .dep_mask               = (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS) | (1 << SNDRV_PCM_HW_PARAM_SAMPLE_BITS),
        .func                   = snd_pcm_hw_rule_channels,
    },
    [SNDRV_PCM_HW_PARAM_RATE] = {
        .dep_mask               = (1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIOD_TIME)
                                | (1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_BUFFER_TIME),
        .func                   = snd_pcm_hw_rule_rate,
    },
    [SNDRV_PCM_HW_PARAM_PERIOD_TIME] = {
        .dep_mask               = (1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_RATE),
        .func                   = snd_pcm_hw_rule_period_time,
    },
    [SNDRV_PCM_HW_PARAM_PERIOD_SIZE] = {
        .dep_mask               = (1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIODS)
                                | (1 << SNDRV_PCM_HW_PARAM_PERIOD_BYTES) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS)
                                | (1 << SNDRV_PCM_HW_PARAM_PERIOD_TIME) | (1 << SNDRV_PCM_HW_PARAM_RATE),
        .func                   = snd_pcm_hw_rule_period_size,
    },
    [SNDRV_PCM_HW_PARAM_PERIOD_BYTES] = {
        .dep_mask               = (1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS),
        .func                   = snd_pcm_hw_rule_period_bytes,
    },
    [SNDRV_PCM_HW_PARAM_PERIODS] = {
        .dep_mask               = (1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE),
        .func                   = snd_pcm_hw_rule_periods,
    },
    [SNDRV_PCM_HW_PARAM_BUFFER_TIME] = {
        .dep_mask               = (1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_RATE),
        .func                   = snd_pcm_hw_rule_buffer_time,
    },
    [SNDRV_PCM_HW_PARAM_BUFFER_SIZE] = {
        .dep_mask               = (1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIODS)
                                | (1 << SNDRV_PCM_HW_PARAM_BUFFER_BYTES) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS)
                                | (1 << SNDRV_PCM_HW_PARAM_BUFFER_TIME) | (1 << SNDRV_PCM_HW_PARAM_RATE),
        .func                   = snd_pcm_hw_rule_buffer_size,
    },
    [SNDRV_PCM_HW_PARAM_BUFFER_BYTES] = {
        .dep_mask               = (1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS),
        .func                   = snd_pcm_hw_rule_buffer_bytes,
    },
};

/*============================ IMPLEMENTATION ================================*/

static inline int hw_is_mask(int var)
{
    return var >= SNDRV_PCM_HW_PARAM_FIRST_MASK && var <= SNDRV_PCM_HW_PARAM_LAST_MASK;
}

static inline int hw_is_interval(int var)
{
    return var >= SNDRV_PCM_HW_PARAM_FIRST_INTERVAL && var <= SNDRV_PCM_HW_PARAM_LAST_INTERVAL;
}

int snd_mask_empty(const struct snd_mask *mask)
{
    for (int i = 0; i < dimof(mask->bits); i++) {
        if (mask->bits[i] != 0) {
            return 0;
        }
    }
    return 1;
}

int snd_mask_eq(const struct snd_mask *mask, const struct snd_mask *v)
{
    return !memcmp(mask, v, sizeof(mask->bits));
}

int snd_mask_test(const struct snd_mask *mask, unsigned int val)
{
    return mask->bits[val >> 5] & (1 << (val & 0x1F));
}

void snd_mask_reset(struct snd_mask *mask, unsigned int val)
{
    mask->bits[val >> 5] &= ~(1 << (val & 0x1F));
}

void snd_mask_intersect(struct snd_mask *mask, const struct snd_mask *v)
{
    for (int i = 0; i < dimof(mask->bits); i++) {
        mask->bits[i] &= v->bits[i];
    }
}

int snd_mask_single(const struct snd_mask *mask)
{
    int i, c = 0;
    for (i = 0; i < dimof(mask->bits); i++) {
        if (!mask->bits[i]) {
            continue;
        }
        if (mask->bits[i] & (mask->bits[i] - 1)) {
            return 0;
        }
        if (c) {
            return 0;
        }
        c++;
    }
    return 1;
}

unsigned int snd_mask_min(const struct snd_mask *mask)
{
    int i;
    for (i = 0; i < dimof(mask->bits); i++) {
        if (mask->bits[i]) {
            return vsf_ffs32(mask->bits[i]) + (i << 5);
        }
    }
    return 0;
}

int snd_mask_refine(struct snd_mask *mask, const struct snd_mask *v)
{
    struct snd_mask old = *mask;
    snd_mask_intersect(mask, v);
    if (snd_mask_empty(mask)) {
        return -EINVAL;
    }
    return !snd_mask_eq(mask, &old);
}

int snd_interval_checkempty(const struct snd_interval *i)
{
    return (i->min > i->max || (i->min == i->max && (i->openmin || i->openmax)));
}

int snd_interval_value(const struct snd_interval *i)
{
    return (i->openmin && !i->openmax) ? i->max : i->min;
}

int snd_interval_single(const struct snd_interval *i)
{
    return (i->min == i->max || (i->min + 1 == i->max && (i->openmin || i->openmax)));
}

int snd_interval_eq(const struct snd_interval *i1, const struct snd_interval *i2)
{
    if (i1->empty)
        return i2->empty;
    if (i2->empty)
        return i1->empty;
    return i1->min == i2->min && i1->openmin == i2->openmin &&
        i1->max == i2->max && i1->openmax == i2->openmax;
}

int snd_interval_refine(struct snd_interval *i, const struct snd_interval *v)
{
    int changed = 0;
    if (i->min < v->min) {
        i->min = v->min;
        i->openmin = v->openmin;
        changed = 1;
    } else if ((i->min == v->min) && !i->openmin && v->openmin) {
        i->openmin = 1;
        changed = 1;
    }
    if (i->max > v->max) {
        i->max = v->max;
        i->openmax = v->openmax;
        changed = 1;
    } else if ((i->max == v->max) && !i->openmax && v->openmax) {
        i->openmax = 1;
        changed = 1;
    }
    if (!i->integer && v->integer) {
        i->integer = 1;
        changed = 1;
    }
    if (i->integer) {
        if (i->openmin) {
            i->min++;
            i->openmin = 0;
        }
        if (i->openmax) {
            i->max--;
            i->openmax = 0;
        }
    } else if (!i->openmin && !i->openmax && (i->min == i->max)) {
        i->integer = 1;
    }
    if (snd_interval_checkempty(i)) {
        i->empty = 1;
        return -EINVAL;
    }
    return changed;
}

static vk_audio_stream_t * __vsf_linux_audio_get_stream(vk_audio_dev_t *audio_dev, int device_idx, bool is_playback)
{
    vk_audio_stream_t *audio_stream = audio_dev->stream;

    for (int i = 0; (device_idx >= 0) && (i < audio_dev->stream_num); i++) {
        if (    (is_playback && !audio_stream->dir_in1out0)
            ||  (!is_playback && audio_stream->dir_in1out0)) {
            if (0 == device_idx--) {
                return audio_stream;
            }
        }
    }
    return NULL;
}

static inline struct snd_mask * param_mask(struct snd_mask *masks, snd_pcm_hw_param_t var)
{
    return masks + (var - SNDRV_PCM_HW_PARAM_FIRST_MASK);
}

static inline struct snd_interval * param_interval(struct snd_interval *intervals, snd_pcm_hw_param_t var)
{
    return intervals + (var - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL);
}

static int snd_pcm_hw_rule_format(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_mask *mask = param_mask(masks, SNDRV_PCM_HW_PARAM_FORMAT);
    struct snd_interval *interval = param_interval(intervals, SNDRV_PCM_HW_PARAM_SAMPLE_BITS);
    int bits;

    for (int i = 0; i < dimof(pcm_formats); i++) {
        bits = pcm_formats[i].phys;
        if (!snd_mask_test(mask, i) || (bits <= 0)) {
            continue;
        }
        if (((unsigned)bits < interval->min) || ((unsigned)bits > interval->max)) {
            snd_mask_reset(mask, i);
        }
    }
    return (1 << SNDRV_PCM_HW_PARAM_SAMPLE_BITS);
}

static int snd_pcm_hw_rule_sample_bits(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_interval *interval_samplebits = param_interval(intervals, SNDRV_PCM_HW_PARAM_SAMPLE_BITS);

    if (cmask & (1 << SNDRV_PCM_HW_PARAM_FORMAT)) {
        cmask = (1 << SNDRV_PCM_HW_PARAM_FORMAT);
        struct snd_mask *mask = param_mask(masks, SNDRV_PCM_HW_PARAM_FORMAT);
        unsigned int min = (unsigned)-1, max = 0;
        int bits;

        for (int i = 0; i < dimof(pcm_formats); i++) {
            bits = pcm_formats[i].phys;
            if (!snd_mask_test(mask, i) || (bits <= 0)) {
                continue;
            }
            if (bits > max) {
                max = bits;
            }
            if (bits < min) {
                min = bits;
            }
        }
        if (interval_samplebits->min < min) {
            interval_samplebits->min = min;
        }
        if (interval_samplebits->max > max) {
            interval_samplebits->max = max;
        }
    } else if (cmask & ((1 << SNDRV_PCM_HW_PARAM_FRAME_BITS) | (1 << SNDRV_PCM_HW_PARAM_CHANNELS))) {
        cmask = (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS) | (1 << SNDRV_PCM_HW_PARAM_CHANNELS);
        struct snd_interval *interval_framebits = param_interval(intervals, SNDRV_PCM_HW_PARAM_FRAME_BITS);
        struct snd_interval *interval_channels = param_interval(intervals, SNDRV_PCM_HW_PARAM_FRAME_BITS);

        interval_samplebits->min = interval_framebits->min / interval_channels->max;
        interval_samplebits->max = interval_framebits->max / interval_channels->min;
    } else {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    return cmask;
}

static int snd_pcm_hw_rule_frame_bits(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_interval *interval_framebits = param_interval(intervals, SNDRV_PCM_HW_PARAM_FRAME_BITS);

    if (cmask & ((1 << SNDRV_PCM_HW_PARAM_SAMPLE_BITS) | (1 << SNDRV_PCM_HW_PARAM_CHANNELS))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_SAMPLE_BITS) | (1 << SNDRV_PCM_HW_PARAM_CHANNELS));
        struct snd_interval *interval_samplebits = param_interval(intervals, SNDRV_PCM_HW_PARAM_SAMPLE_BITS);
        struct snd_interval *interval_channels = param_interval(intervals, SNDRV_PCM_HW_PARAM_CHANNELS);

        interval_framebits->min = interval_samplebits->min * interval_channels->min;
        interval_framebits->max = interval_samplebits->max * interval_channels->max;
    } else if (cmask & ((1 << SNDRV_PCM_HW_PARAM_PERIOD_BYTES) | (1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_PERIOD_BYTES) | (1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE));
        struct snd_interval *interval_periodbytes = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_BYTES);
        struct snd_interval *interval_periodsize = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_SIZE);

        interval_framebits->min = (interval_periodbytes->min << 3) / interval_periodsize->max;
        interval_framebits->max = (interval_periodbytes->max << 3) / interval_periodsize->min;
    } else if (cmask & ((1 << SNDRV_PCM_HW_PARAM_BUFFER_BYTES) | (1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_BUFFER_BYTES) | (1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE));
        struct snd_interval *interval_bufferbytes = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_BYTES);
        struct snd_interval *interval_buffersize = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_SIZE);

        interval_framebits->min = (interval_bufferbytes->min << 3) / interval_buffersize->max;
        interval_framebits->max = (interval_bufferbytes->max << 3) / interval_buffersize->min;
    } else {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    return cmask;
}

static int snd_pcm_hw_rule_channels(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_interval *interval_channels = param_interval(intervals, SNDRV_PCM_HW_PARAM_CHANNELS);

    if (cmask & ((1 << SNDRV_PCM_HW_PARAM_FRAME_BITS) | (1 << SNDRV_PCM_HW_PARAM_SAMPLE_BITS))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_FRAME_BITS) | (1 << SNDRV_PCM_HW_PARAM_SAMPLE_BITS));
        struct snd_interval *interval_framebits = param_interval(intervals, SNDRV_PCM_HW_PARAM_FRAME_BITS);
        struct snd_interval *interval_samplebits = param_interval(intervals, SNDRV_PCM_HW_PARAM_SAMPLE_BITS);

        interval_channels->min = interval_framebits->min / interval_samplebits->max;
        interval_channels->max = interval_framebits->max / interval_samplebits->min;
    } else {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    return cmask;
}

static int snd_pcm_hw_rule_rate(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_interval *interval_rate = param_interval(intervals, SNDRV_PCM_HW_PARAM_RATE);

    if (cmask & ((1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIOD_TIME))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIOD_TIME));
        struct snd_interval *interval_periodsize = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_SIZE);
        struct snd_interval *interval_periodtime = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_TIME);

        interval_rate->min = interval_periodsize->min * 1000000ULL / interval_periodtime->max;
        interval_rate->max = interval_periodsize->max * 1000000ULL / interval_periodtime->min;
    } else if (cmask & ((1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_BUFFER_TIME))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_BUFFER_TIME));
        struct snd_interval *interval_buffersize = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_SIZE);
        struct snd_interval *interval_buffertime = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_TIME);

        interval_rate->min = interval_buffersize->min * 1000000ULL / interval_buffertime->max;
        interval_rate->max = interval_buffersize->max * 1000000ULL / interval_buffertime->min;
    } else {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    return cmask;
}

static int snd_pcm_hw_rule_period_time(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_interval *interval_periodtime = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_TIME);

    if (cmask & ((1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_RATE))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_RATE));
        struct snd_interval *interval_periodsize = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_SIZE);
        struct snd_interval *interval_rate = param_interval(intervals, SNDRV_PCM_HW_PARAM_RATE);

        interval_periodtime->min = interval_periodsize->min * 1000000ULL / interval_rate->max;
        interval_periodtime->max = interval_periodsize->max * 1000000ULL / interval_rate->min;
    } else {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    return cmask;
}

static int snd_pcm_hw_rule_period_size(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_interval *interval_periodsize = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_SIZE);

    if (cmask & ((1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIODS))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIODS));
        struct snd_interval *interval_buffersize = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_SIZE);
        struct snd_interval *interval_periods = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIODS);

        interval_periodsize->min = interval_buffersize->min / interval_periods->max;
        interval_periodsize->max = interval_buffersize->max / interval_periods->min;
    } else if (cmask & ((1 << SNDRV_PCM_HW_PARAM_PERIOD_BYTES) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_PERIOD_BYTES) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS));
        struct snd_interval *interval_periodbytes = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_BYTES);
        struct snd_interval *interval_framebits = param_interval(intervals, SNDRV_PCM_HW_PARAM_FRAME_BITS);

        interval_periodsize->min = interval_periodbytes->min / (interval_framebits->max >> 3);
        interval_periodsize->max = interval_periodbytes->max / (interval_framebits->min >> 3);
    } else if (cmask & ((1 << SNDRV_PCM_HW_PARAM_PERIOD_TIME) | (1 << SNDRV_PCM_HW_PARAM_RATE))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_PERIOD_TIME) | (1 << SNDRV_PCM_HW_PARAM_RATE));
        struct snd_interval *interval_periodtime = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_TIME);
        struct snd_interval *interval_rate = param_interval(intervals, SNDRV_PCM_HW_PARAM_RATE);

        interval_periodsize->min = (uint64_t)interval_periodtime->min * interval_rate->min / 1000000;
        interval_periodsize->max = (uint64_t)interval_periodtime->max * interval_rate->max / 1000000;
    } else {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    return cmask;
}

static int snd_pcm_hw_rule_period_bytes(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_interval *interval_periodbytes = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_BYTES);

    if (cmask & ((1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS));
        struct snd_interval *interval_periodsize = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_SIZE);
        struct snd_interval *interval_framebits = param_interval(intervals, SNDRV_PCM_HW_PARAM_FRAME_BITS);

        interval_periodbytes->min = interval_periodsize->min * (interval_framebits->min >> 3);
        interval_periodbytes->max = interval_periodsize->max * (interval_framebits->max >> 3);
    } else {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    return cmask;
}

static int snd_pcm_hw_rule_periods(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_interval *interval_periods = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIODS);

    if (cmask & ((1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE));
        struct snd_interval *interval_buffersize = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_SIZE);
        struct snd_interval *interval_periodsize = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_SIZE);

        interval_periods->min = interval_buffersize->min / interval_periodsize->max;
        interval_periods->max = interval_buffersize->max / interval_periodsize->min;
    } else {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    return cmask;
}

static int snd_pcm_hw_rule_buffer_time(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_interval *interval_buffertime = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_TIME);

    if (cmask & ((1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_RATE))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_RATE));
        struct snd_interval *interval_buffersize = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_SIZE);
        struct snd_interval *interval_rate = param_interval(intervals, SNDRV_PCM_HW_PARAM_RATE);

        interval_buffertime->min = interval_buffersize->min * 1000000ULL / interval_rate->max;
        interval_buffertime->max = interval_buffersize->max * 1000000ULL / interval_rate->min;
    } else {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    return cmask;
}

static int snd_pcm_hw_rule_buffer_size(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_interval *interval_buffersize = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_SIZE);

    if (cmask & ((1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIODS))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_PERIOD_SIZE) | (1 << SNDRV_PCM_HW_PARAM_PERIODS));
        struct snd_interval *interval_periodsize = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIOD_SIZE);
        struct snd_interval *interval_periods = param_interval(intervals, SNDRV_PCM_HW_PARAM_PERIODS);

        interval_buffersize->min = interval_periodsize->min * interval_periods->min;
        interval_buffersize->max = interval_periodsize->max * interval_periods->max;
    } else if (cmask & ((1 << SNDRV_PCM_HW_PARAM_BUFFER_BYTES) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_BUFFER_BYTES) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS));
        struct snd_interval *interval_bufferbytes = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_BYTES);
        struct snd_interval *interval_framebits = param_interval(intervals, SNDRV_PCM_HW_PARAM_FRAME_BITS);

        interval_buffersize->min = interval_bufferbytes->min / (interval_framebits->max >> 3);
        interval_buffersize->max = interval_bufferbytes->max / (interval_framebits->min >> 3);
    } else if (cmask & ((1 << SNDRV_PCM_HW_PARAM_BUFFER_TIME) | (1 << SNDRV_PCM_HW_PARAM_RATE))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_BUFFER_TIME) | (1 << SNDRV_PCM_HW_PARAM_RATE));
        struct snd_interval *interval_buffertime = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_TIME);
        struct snd_interval *interval_rate = param_interval(intervals, SNDRV_PCM_HW_PARAM_RATE);

        interval_buffersize->min = ((uint64_t)interval_buffertime->min * interval_rate->min) / 1000000;
        interval_buffersize->max = ((uint64_t)interval_buffertime->max * interval_rate->max) / 1000000;
    } else {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    return cmask;
}

static int snd_pcm_hw_rule_buffer_bytes(struct snd_mask *masks, struct snd_interval *intervals,
            unsigned int cmask, struct __snd_pcm_hw_rule *rule)
{
    struct snd_interval *interval_bufferbytes = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_BYTES);

    if (cmask & ((1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS))) {
        cmask = ((1 << SNDRV_PCM_HW_PARAM_BUFFER_SIZE) | (1 << SNDRV_PCM_HW_PARAM_FRAME_BITS));
        struct snd_interval *interval_buffersize = param_interval(intervals, SNDRV_PCM_HW_PARAM_BUFFER_SIZE);
        struct snd_interval *interval_framebits = param_interval(intervals, SNDRV_PCM_HW_PARAM_FRAME_BITS);

        interval_bufferbytes->min = interval_buffersize->min * (interval_framebits->min >> 3);
        interval_bufferbytes->max = interval_buffersize->max * (interval_framebits->max >> 3);
    } else {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    return cmask;
}

int params_update(struct snd_mask *masks, struct snd_interval *intervals, unsigned int *cmask,
            unsigned int flags, bool sefl_constraints)
{
    struct __snd_pcm_hw_rule *rule;
    struct snd_mask mask_old, *mask_cur;
    struct snd_interval interval_old, *interval_cur;
    unsigned int new_cmask = *cmask, next_cmask = 0, cur_cmask = *cmask, rule_cmask;
    int ret, changed;

again:
    for (int i = 0; i < dimof(pcm_rules); i++) {
        rule = (struct __snd_pcm_hw_rule *)&pcm_rules[i];
        if (    (NULL == rule->func)
            ||  (rule->cond && !(rule->cond & flags))
            ||  !(cur_cmask & rule->dep_mask)
            ||  (cur_cmask & (1 << i))
            ||  (new_cmask & (1 << i))
            ) {
            continue;
        }

        rule_cmask = cur_cmask;
    try_next:
        if (hw_is_mask(i)) {
            mask_cur = param_mask(masks, i);
            mask_old = *mask_cur;
        } else {
            interval_cur = param_interval(intervals, i);
            interval_old = *interval_cur;
        }
        ret = rule->func(masks, intervals, rule_cmask, rule);
        if (hw_is_mask(i)) {
            if (sefl_constraints) {
                snd_mask_intersect(mask_cur, &mask_old);
            }
            changed = !snd_mask_eq(mask_cur, &mask_old);
        } else {
            if (sefl_constraints) {
                if ((interval_cur->min > interval_old.max) || (interval_cur->max < interval_old.min)) {
                    interval_cur->min = interval_old.min;
                    interval_cur->max = interval_old.max;
                } else {
                    if (interval_cur->min < interval_old.min) {
                        interval_cur->min = interval_old.min;
                    }
                    if (interval_cur->max > interval_old.max) {
                        interval_cur->max = interval_old.max;
                   }
                }
            }
            changed = !snd_interval_eq(interval_cur, &interval_old);
        }
        if (ret < 0) {
            return ret;
        } else if (!changed) {
            // not changed
            rule_cmask &= ~ret;
            if (rule_cmask & rule->dep_mask) {
                goto try_next;
            }
            continue;
        }

        next_cmask |= 1 << i;
        new_cmask |= 1 << i;
    }

    if (next_cmask) {
        cur_cmask = next_cmask;
        next_cmask = 0;
        goto again;
    }
    *cmask = new_cmask & ~*cmask;
    return 0;
}

static void __vsf_linux_audio_play_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_audio_play_priv_t *priv = (vsf_linux_audio_play_priv_t *)sfd->priv;
    vk_audio_dev_t *audio_dev = (vk_audio_dev_t *)(((vk_vfs_file_t *)(priv->file))->f.param);
    vk_audio_stream_t *audio_stream;
    char *filename = ((vk_vfs_file_t *)(priv->file))->name;
    int card_idx, device_idx;
    char stream;

    int match = sscanf(filename, "pcmC%dD%d%c", &card_idx, &device_idx, &stream);
    if (match != 3) {
        VSF_LINUX_ASSERT(false);
        return;
    }
    audio_stream = __vsf_linux_audio_get_stream(audio_dev, device_idx, stream == 'p');
    if (NULL == audio_stream) {
        VSF_LINUX_ASSERT(false);
        return;
    }

    struct snd_mask *mask = param_mask(priv->hw_constraints.masks, SNDRV_PCM_HW_PARAM_ACCESS);
    mask->bits[0] = (1 << SNDRV_PCM_ACCESS_MMAP_INTERLEAVED) | (1 << SNDRV_PCM_ACCESS_RW_INTERLEAVED);

    uint32_t pcm_datatype;
    vk_audio_format_t audio_format = audio_stream->format;
    uint8_t audio_datatype = audio_format.datatype.value;
    uint8_t audio_bitlen = VSF_AUDIO_DATA_TYPE_BITLEN(audio_datatype);
    if (0 == audio_datatype) {
        pcm_datatype =  (1 << SNDRV_PCM_FORMAT_U8) | (1 << SNDRV_PCM_FORMAT_S8)
                    |   (1 << SNDRV_PCM_FORMAT_U16_LE) | (1 << SNDRV_PCM_FORMAT_U16_BE)
                    |   (1 << SNDRV_PCM_FORMAT_S16_LE) | (1 << SNDRV_PCM_FORMAT_S16_BE)
                    |   (1 << SNDRV_PCM_FORMAT_U24_LE) | (1 << SNDRV_PCM_FORMAT_U24_BE)
                    |   (1 << SNDRV_PCM_FORMAT_S24_LE) | (1 << SNDRV_PCM_FORMAT_S24_BE)
                    |   (1 << SNDRV_PCM_FORMAT_U32_LE) | (1 << SNDRV_PCM_FORMAT_U32_BE)
                    |   (1 << SNDRV_PCM_FORMAT_S32_LE) | (1 << SNDRV_PCM_FORMAT_S32_BE);
        audio_bitlen = 0;
    } else if (VSF_AUDIO_DATA_TYPE_IS_BE(audio_datatype)) {
        if (VSF_AUDIO_DATA_TYPE_IS_INT(audio_datatype)) {
            if (VSF_AUDIO_DATA_TYPE_IS_SIGNED(audio_datatype)) {
                switch (audio_bitlen) {
                case 8:     pcm_datatype = SNDRV_PCM_FORMAT_S8;     break;
                case 16:    pcm_datatype = SNDRV_PCM_FORMAT_S16_BE; break;
                case 24:    pcm_datatype = SNDRV_PCM_FORMAT_S24_BE; break;
                case 32:    pcm_datatype = SNDRV_PCM_FORMAT_S32_BE; break;
                default:    VSF_LINUX_ASSERT(false);                return;
                }
            } else {
                switch (audio_bitlen) {
                case 8:     pcm_datatype = SNDRV_PCM_FORMAT_U8;     break;
                case 16:    pcm_datatype = SNDRV_PCM_FORMAT_U16_BE; break;
                case 24:    pcm_datatype = SNDRV_PCM_FORMAT_U24_BE; break;
                case 32:    pcm_datatype = SNDRV_PCM_FORMAT_U32_BE; break;
                default:    VSF_LINUX_ASSERT(false);                return;
                }
            }
        } else {
            switch (audio_bitlen) {
            case 32:    pcm_datatype = SNDRV_PCM_FORMAT_FLOAT_BE;   break;
            case 64:    pcm_datatype = SNDRV_PCM_FORMAT_FLOAT64_BE; break;
            default:    VSF_LINUX_ASSERT(false);                    return;
            }
        }
        pcm_datatype = 1 << pcm_datatype;
    } else {
        if (VSF_AUDIO_DATA_TYPE_IS_INT(audio_datatype)) {
            if (VSF_AUDIO_DATA_TYPE_IS_SIGNED(audio_datatype)) {
                switch (audio_bitlen) {
                case 8:     pcm_datatype = SNDRV_PCM_FORMAT_S8;     break;
                case 16:    pcm_datatype = SNDRV_PCM_FORMAT_S16_LE; break;
                case 24:    pcm_datatype = SNDRV_PCM_FORMAT_S24_LE; break;
                case 32:    pcm_datatype = SNDRV_PCM_FORMAT_S32_LE; break;
                default:    VSF_LINUX_ASSERT(false);                return;
                }
            } else {
                switch (audio_bitlen) {
                case 8:     pcm_datatype = SNDRV_PCM_FORMAT_U8;     break;
                case 16:    pcm_datatype = SNDRV_PCM_FORMAT_U16_LE; break;
                case 24:    pcm_datatype = SNDRV_PCM_FORMAT_U24_LE; break;
                case 32:    pcm_datatype = SNDRV_PCM_FORMAT_U32_LE; break;
                default:    VSF_LINUX_ASSERT(false);                return;
                }
            }
        } else {
            switch (audio_bitlen) {
            case 32:    pcm_datatype = SNDRV_PCM_FORMAT_FLOAT_LE;   break;
            case 64:    pcm_datatype = SNDRV_PCM_FORMAT_FLOAT64_LE; break;
            default:    VSF_LINUX_ASSERT(false);                    return;
            }
        }
        pcm_datatype = 1 << pcm_datatype;
    }
    mask = param_mask(priv->hw_constraints.masks, SNDRV_PCM_HW_PARAM_FORMAT);
    mask->bits[0] = pcm_datatype;

    mask = param_mask(priv->hw_constraints.masks, SNDRV_PCM_HW_PARAM_SUBFORMAT);
    mask->bits[0] = 1 << SNDRV_PCM_SUBFORMAT_STD;

    struct snd_interval *interval;
    interval = param_interval(priv->hw_constraints.intervals, SNDRV_PCM_HW_PARAM_SAMPLE_BITS);
    interval->min = 1;
    interval->max = 64;

    interval = param_interval(priv->hw_constraints.intervals, SNDRV_PCM_HW_PARAM_CHANNELS);
    if (0 == audio_format.channel_num) {
        interval->min = 1;
        interval->max = 32;
    } else {
        interval->max = interval->min = audio_format.channel_num;
    }
    interval->integer = true;

    interval = param_interval(priv->hw_constraints.intervals, SNDRV_PCM_HW_PARAM_RATE);
    if (0 == audio_format.sample_rate) {
        interval->min = 8 * 1000;
        interval->max = 384 * 1000;
    } else {
        interval->max = interval->min = audio_format.sample_rate * 100;
    }
    interval->integer = true;

    interval = param_interval(priv->hw_constraints.intervals, SNDRV_PCM_HW_PARAM_PERIOD_TIME);
    interval->min = VSF_LINUX_AUDIO_CFG_MIN_PERIOD_US;
    interval->max = VSF_LINUX_AUDIO_CFG_MAX_PERIOD_US;
    interval->integer = true;

    interval = param_interval(priv->hw_constraints.intervals, SNDRV_PCM_HW_PARAM_PERIODS);
    interval->min = VSF_LINUX_AUDIO_CFG_MIN_PERIODS_CNT;
    interval->max = VSF_LINUX_AUDIO_CFG_MAX_PERIODS_CNT;
    interval->integer = true;

    unsigned int cmask =    (1 << SNDRV_PCM_HW_PARAM_FORMAT) | (1 << SNDRV_PCM_HW_PARAM_CHANNELS)
                        |   (1 << SNDRV_PCM_HW_PARAM_RATE) | (1 << SNDRV_PCM_HW_PARAM_PERIOD_TIME)
                        |   (1 << SNDRV_PCM_HW_PARAM_PERIODS);
    params_update(priv->hw_constraints.masks, priv->hw_constraints.intervals, &cmask, 0, false);
}

static int __vsf_linux_audio_play_start(vk_audio_dev_t *audio_dev, vk_audio_stream_t *audio_stream,
            vsf_stream_t *stream)
{
    return vk_audio_start(audio_dev, audio_stream->stream_index, stream, &audio_stream->format);
}

static int __vsf_linux_audio_play_stop(vk_audio_dev_t *audio_dev, vk_audio_stream_t *audio_stream)
{
    return vk_audio_stop(audio_dev, audio_stream->stream_index);
}

static int __vsf_linux_audio_play_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    vsf_linux_audio_play_priv_t *priv = (vsf_linux_audio_play_priv_t *)sfd->priv;
    vk_audio_dev_t *audio_dev = (vk_audio_dev_t *)(((vk_vfs_file_t *)(priv->file))->f.param);
    vk_audio_stream_t *audio_stream;
    char *filename = ((vk_vfs_file_t *)(priv->file))->name;
    int card_idx, device_idx;
    char stream;

    int match = sscanf(filename, "pcmC%dD%d%c", &card_idx, &device_idx, &stream);
    if (match != 3) {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }
    bool is_playback = stream == 'p';
    audio_stream = __vsf_linux_audio_get_stream(audio_dev, device_idx, is_playback);
    if (NULL == audio_stream) {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }

    union {
        int *version;                           // SNDRV_PCM_IOCTL_PVERSION
        struct snd_pcm_info *info;              // SNDRV_PCM_IOCTL_INFO
        int *time_stamp;                        // SNDRV_PCM_IOCTL_TSTAMP
        int *user_version;                      // SNDRV_PCM_IOCTL_USER_PVERSION
        struct snd_pcm_hw_params *hw_params;    // SNDRV_PCM_IOCTL_HW_REFINE/SNDRV_PCM_IOCTL_HW_PARAMS
        struct snd_pcm_sw_params *sw_params;    // SNDRV_PCM_IOCTL_SW_PARAMS
        struct snd_pcm_sync_ptr *sync_ptr;      // SNDRV_PCM_IOCTL_SYNC_PTR
        struct snd_pcm_channel_info *chinfo;    // SNDRV_PCM_IOCTL_CHANNEL_INFO
        struct snd_xferi *xferi;                // SNDRV_PCM_IOCTL_WRITEI_FRAMES/SNDRV_PCM_IOCTL_READI_FRAMES

        uintptr_t arg;
    } u;
    u.arg = arg;

    switch (cmd) {
    case F_SETFL:
        break;
    case SNDRV_PCM_IOCTL_PVERSION:
        *u.version = SNDRV_PCM_VERSION;
        break;
    case SNDRV_PCM_IOCTL_INFO:
        u.info->device = device_idx;
        u.info->subdevice = 0;
        u.info->card = card_idx;
        strcpy((char *)u.info->id, "0001");
        strcpy((char *)u.info->subname, "subdevice #0");
        switch (stream) {
        case 'p':
            u.info->stream = SNDRV_PCM_STREAM_PLAYBACK;
            strncpy((char *)u.info->name, "vsf_audio_card_pcm_playback", sizeof(u.info->name));
            break;
        case 'c':
            u.info->stream = SNDRV_PCM_STREAM_CAPTURE;
            strncpy((char *)u.info->name, "vsf_audio_card_pcm_capture", sizeof(u.info->name));
            break;
        }
        break;
    case SNDRV_PCM_IOCTL_TSTAMP:
        break;
    case SNDRV_PCM_IOCTL_USER_PVERSION:
        break;
    case SNDRV_PCM_IOCTL_HW_REFINE: {
            int changed;

            struct snd_mask *mask;
            for (int i = SNDRV_PCM_HW_PARAM_FIRST_MASK; i <= SNDRV_PCM_HW_PARAM_LAST_MASK; i++) {
                mask = param_mask(u.hw_params->masks, i);
                if (snd_mask_empty(mask)) {
                    errno = EINVAL;
                    return -1;
                }
                if (!(u.hw_params->rmask & (1 << i))) {
                    continue;
                }

                changed = snd_mask_refine(mask, &priv->hw_constraints.masks[i - SNDRV_PCM_HW_PARAM_FIRST_MASK]);
                if (changed < 0) {
                    errno = -changed;
                    return changed;
                } else if (changed) {
                    u.hw_params->cmask |= 1 << i;
                }
            }

            struct snd_interval *interval;
            for (int i = SNDRV_PCM_HW_PARAM_FIRST_INTERVAL; i <= SNDRV_PCM_HW_PARAM_LAST_INTERVAL; i++) {
                interval = param_interval(u.hw_params->intervals, i);
                if (interval->empty) {
                    errno = EINVAL;
                    return -1;
                }
                if (!(u.hw_params->rmask & (1 << i))) {
                    continue;
                }

                changed = snd_interval_refine(interval, (const struct snd_interval *)&priv->hw_constraints.intervals[i - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL]);
                if (changed < 0) {
                    errno = -changed;
                    return changed;
                } else if (changed) {
                    u.hw_params->cmask |= 1 << i;
                }
            }

            unsigned int cmask = u.hw_params->rmask;
            changed = params_update(u.hw_params->masks, u.hw_params->intervals, &cmask, u.hw_params->flags, true);
            if (changed < 0) {
                errno = -changed;
                return changed;
            }

            if (u.hw_params->rmask & (1 << SNDRV_PCM_HW_PARAM_SAMPLE_BITS)) {
                interval = param_interval(u.hw_params->intervals, SNDRV_PCM_HW_PARAM_SAMPLE_BITS);
                if (snd_interval_single(interval)) {
                    u.hw_params->msbits = snd_interval_value(interval);
                } else {
                    u.hw_params->msbits = 0;
                }
            }
            if (u.hw_params->rmask & (1 << SNDRV_PCM_HW_PARAM_RATE)) {
                interval = param_interval(u.hw_params->intervals, SNDRV_PCM_HW_PARAM_RATE);
                if (snd_interval_single(interval)) {
                    u.hw_params->rate_num = snd_interval_value(interval);
                    u.hw_params->rate_den = 1;
                } else {
                    u.hw_params->msbits = 0;
                    u.hw_params->rate_den = 0;
                }
            }

            mask = &u.hw_params->masks[SNDRV_PCM_HW_PARAM_FORMAT - SNDRV_PCM_HW_PARAM_FIRST_MASK];
            interval = &u.hw_params->intervals[SNDRV_PCM_HW_PARAM_CHANNELS - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
            if (snd_mask_single(mask) && snd_interval_single(interval)) {
                snd_pcm_format_t format = (snd_pcm_format_t)snd_mask_min(mask);
                int channels = snd_interval_value(interval);
                ssize_t frame_size = pcm_formats[format].phys >> 3;
                // TODO: fix fifo_size
                u.hw_params->fifo_size = channels * frame_size;
            } else {
                u.hw_params->fifo_size = 0;
            }

            u.hw_params->info = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID
                            |   SNDRV_PCM_INFO_INTERLEAVED
                            |   SNDRV_PCM_INFO_RESUME | SNDRV_PCM_INFO_PAUSE;
            u.hw_params->rmask = 0;
        }
        break;
    case SNDRV_PCM_IOCTL_HW_PARAMS: {
            priv->hw_params = *u.hw_params;

            struct snd_mask *mask = param_mask(&priv->hw_params.masks[0], SNDRV_PCM_HW_PARAM_FORMAT);
            if (!snd_mask_single(mask)) {
                errno = EINVAL;
                return -1;
            }
            snd_pcm_format_t format = (snd_pcm_format_t)snd_mask_min(mask);
            audio_stream->format.datatype.value =
                    __VSF_AUDIO_DATA_TYPE_BITLEN(pcm_formats[format].phys)
                |   (pcm_formats[format].signd ? VSF_AUDIO_DATA_TYPE_SIGNED : VSF_AUDIO_DATA_TYPE_UNSIGNED)
                |   (pcm_formats[format].le ? VSF_AUDIO_DATA_TYPE_LE : VSF_AUDIO_DATA_TYPE_BE)
                |   VSF_AUDIO_DATA_TYPE_INT;

            struct snd_interval *interval = param_interval(&priv->hw_params.intervals[0], SNDRV_PCM_HW_PARAM_CHANNELS);
            if (!snd_interval_single(interval)) {
                errno = EINVAL;
                return -1;
            }
            audio_stream->format.channel_num = interval->min;

            interval = param_interval(priv->hw_params.intervals, SNDRV_PCM_HW_PARAM_RATE);
            if (!snd_interval_single(interval)) {
                errno = EINVAL;
                return -1;
            }
            audio_stream->format.sample_rate = interval->min / 100;
        }
        break;
    case SNDRV_PCM_IOCTL_HW_FREE:
        if (priv->is_started) {
            struct snd_interval *interval = param_interval(&priv->hw_params.intervals[0], SNDRV_PCM_HW_PARAM_BUFFER_TIME);
            // wait to make sure the data is processed by hw
            usleep(interval->max);
            __vsf_linux_audio_play_stop(audio_dev, audio_stream);
            priv->is_started = false;
        }
        if (is_playback) {
            if (priv->stream_tx != NULL) {
                __vsf_linux_tx_stream_fini(&priv->use_as__vsf_linux_stream_priv_t);
                vsf_stream_fini(priv->stream_tx);
            }
        } else {
            if (priv->stream_rx != NULL) {
                __vsf_linux_rx_stream_fini(&priv->use_as__vsf_linux_stream_priv_t);
                vsf_stream_fini(priv->stream_rx);
            }
        }
        if (priv->buffer != NULL) {
            vsf_linux_free_res(priv->buffer);
            priv->buffer = NULL;
        }
        break;
    case SNDRV_PCM_IOCTL_SW_PARAMS:
        break;
    case SNDRV_PCM_IOCTL_SYNC_PTR:
        if (u.sync_ptr->flags & SNDRV_PCM_SYNC_PTR_APPL) {
            u.sync_ptr->c.control.appl_ptr = priv->mmap.control.appl_ptr;
        } else {
            priv->mmap.control.appl_ptr = u.sync_ptr->c.control.appl_ptr;
        }
        if (u.sync_ptr->flags & SNDRV_PCM_SYNC_PTR_AVAIL_MIN) {
            u.sync_ptr->c.control.avail_min = priv->mmap.control.avail_min;
        } else {
            priv->mmap.control.avail_min = u.sync_ptr->c.control.avail_min;
        }
        u.sync_ptr->s.status = priv->mmap.status;
        break;
    case SNDRV_PCM_IOCTL_CHANNEL_INFO:
        break;
    case SNDRV_PCM_IOCTL_PREPARE: {
            struct snd_interval *interval = param_interval(priv->hw_params.intervals, SNDRV_PCM_HW_PARAM_BUFFER_BYTES);
            if (!snd_interval_single(interval)) {
                errno = EINVAL;
                return -1;
            }

            priv->op = &vsf_fifo_stream_op;
            priv->size = interval->min;
            priv->buffer = vsf_linux_malloc_res(interval->min);

            if (is_playback) {
                priv->stream_tx = &priv->use_as__vsf_stream_t;
                vsf_stream_init(priv->stream_tx);
                __vsf_linux_tx_stream_init(&priv->use_as__vsf_linux_stream_priv_t);
            } else {
                priv->stream_rx = &priv->use_as__vsf_stream_t;
                vsf_stream_init(priv->stream_rx);
                __vsf_linux_rx_stream_init(&priv->use_as__vsf_linux_stream_priv_t);
            }
        }
        break;
    case SNDRV_PCM_IOCTL_RESET:
        break;
    case SNDRV_PCM_IOCTL_START:
        if (!priv->is_started && !__vsf_linux_audio_play_start(audio_dev, audio_stream, priv->stream_tx)) {
            priv->is_started = true;
        }
        break;
    case SNDRV_PCM_IOCTL_DROP:
        extern void __vsf_linux_rx_stream_drop(vsf_linux_stream_priv_t *priv_rx);
        extern void __vsf_linux_tx_stream_drop(vsf_linux_stream_priv_t *priv_tx);
        if (is_playback) {
            __vsf_linux_tx_stream_drop(&priv->use_as__vsf_linux_stream_priv_t);
        } else {
            __vsf_linux_rx_stream_drop(&priv->use_as__vsf_linux_stream_priv_t);
        }
        break;
    case SNDRV_PCM_IOCTL_DRAIN:
        if (is_playback) {
            __vsf_linux_tx_stream_drain(&priv->use_as__vsf_linux_stream_priv_t);
        }
        break;
    case SNDRV_PCM_IOCTL_PAUSE:
        break;
    case SNDRV_PCM_IOCTL_REWIND:
        break;
    case SNDRV_PCM_IOCTL_RESUME:
        break;
    case SNDRV_PCM_IOCTL_WRITEI_FRAMES:
        if (!priv->is_started && !__vsf_linux_audio_play_start(audio_dev, audio_stream, priv->stream_tx)) {
            priv->is_started = true;
        }
        {
            uint8_t frame_size = VSF_AUDIO_DATA_TYPE_BITLEN(audio_stream->format.datatype.value) >> 3;
            u.xferi->result = vsf_stream_write(priv->stream_tx, u.xferi->buf, u.xferi->frames * frame_size) / frame_size;
        }
        break;
    case SNDRV_PCM_IOCTL_READI_FRAMES:
        if (!priv->is_started && !__vsf_linux_audio_play_start(audio_dev, audio_stream, priv->stream_tx)) {
            priv->is_started = true;
        }
        {
            uint8_t frame_size = VSF_AUDIO_DATA_TYPE_BITLEN(audio_stream->format.datatype.value) >> 3;
            u.xferi->result = vsf_stream_read(priv->stream_rx, u.xferi->buf, u.xferi->frames * frame_size) / frame_size;
        }
        break;
    default:
        errno = EOPNOTSUPP;
        return -1;
    }
    return 0;
}

static int __vsf_linux_audio_control_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    vsf_linux_audio_control_priv_t *priv = (vsf_linux_audio_control_priv_t *)sfd->priv;
    char *filename = ((vk_vfs_file_t *)(priv->file))->name;
    int card_idx;

    int match = sscanf(filename, "controlC%d", &card_idx);
    if (match != 1) {
        VSF_LINUX_ASSERT(false);
        errno = EINVAL;
        return -1;
    }

    union {
        int *version;                           // SNDRV_CTL_IOCTL_PVERSION
        struct snd_ctl_card_info *card_info;    // SNDRV_CTL_IOCTL_CARD_INFO
        int *device;                            // SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE
        struct snd_pcm_info *pcm_info;          // SNDRV_CTL_IOCTL_PCM_INFO
        int *subdevice;                         // SNDRV_CTL_IOCTL_PCM_PREFER_SUBDEVICE

        uintptr_t arg;
    } u;
    u.arg = arg;

    switch (cmd) {
    case SNDRV_CTL_IOCTL_PVERSION:
        *u.version = SNDRV_CTL_VERSION;
        break;
    case SNDRV_CTL_IOCTL_CARD_INFO:
        u.card_info->card = card_idx;
        strncpy((char *)u.card_info->id, "0001", sizeof(u.card_info->id));
        strncpy((char *)u.card_info->name, "vsf_audio_card", sizeof(u.card_info->name));
        break;
    case SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE:
        if (*u.device < 0) {
            *u.device = 0;
        } else {
            *u.device = -1;
        }
        break;
    case SNDRV_CTL_IOCTL_PCM_INFO:
        u.pcm_info->card = card_idx;
        strcpy((char *)u.pcm_info->id, "0001");
        switch (u.pcm_info->stream) {
        case SNDRV_PCM_STREAM_PLAYBACK:
            strncpy((char *)u.pcm_info->name, "vsf_audio_card_pcm_playback", sizeof(u.pcm_info->name));
            break;
        case SNDRV_PCM_STREAM_CAPTURE:
            strncpy((char *)u.pcm_info->name, "vsf_audio_card_pcm_capture", sizeof(u.pcm_info->name));
            break;
        }
        break;
    case SNDRV_CTL_IOCTL_PCM_PREFER_SUBDEVICE:
        if (*u.subdevice != 0) {
            errno = EINVAL;
            return -1;
        }
        break;
    default:
        errno = EOPNOTSUPP;
        return -1;
    }
    return 0;
}

int vsf_linux_fs_bind_audio(char *path, int card_idx, vk_audio_dev_t *audio_dev)
{
    int pathlen = strlen(path), leftlen = 16 + 1;
    char realpath[pathlen + leftlen];
    int result;

    strcpy(realpath, (const char *)path);
    if ('/' != realpath[pathlen - 1]) {
        realpath[pathlen++] = '/';
        realpath[pathlen] = '\0';
        leftlen--;
    }

    snprintf(&realpath[pathlen], leftlen, "controlC%d", card_idx);
    result = vsf_linux_fs_bind_target_ex(realpath, audio_dev, &__vsf_linux_audio_control_fdop,
                NULL, NULL, 0, 0);
    if (result < 0) {
        vsf_trace_error("fail to bind %s\n", realpath);
        return result;
    }

    vk_audio_stream_t *audio_stream;
    int play_device_idx = 0, capture_device_idx = 0;
    for (uint8_t i = 0; i < audio_dev->stream_num; i++) {
        audio_stream = &audio_dev->stream[i];
        if (audio_stream->dir_in1out0) {
            snprintf(&realpath[pathlen], leftlen, "pcmC%dD%dc", capture_device_idx++, card_idx);
            result = vsf_linux_fs_bind_target_ex(realpath, audio_dev, &__vsf_linux_audio_capture_fdop,
                NULL, NULL, 0, 0);
            if (result < 0) {
                vsf_trace_error("fail to bind %s\n", realpath);
                return result;
            }
        } else {
            snprintf(&realpath[pathlen], leftlen, "pcmC%dD%dp", play_device_idx++, card_idx);
            result = vsf_linux_fs_bind_target_ex(realpath, audio_dev, &__vsf_linux_audio_play_fdop,
                NULL, NULL, 0, 0);
            if (result < 0) {
                vsf_trace_error("fail to bind %s\n", realpath);
                return result;
            }
        }
    }

    return 0;
}

#endif
