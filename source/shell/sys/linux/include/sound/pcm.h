#ifndef __VSF_LINUX_SOUND_PCM_H__
#define __VSF_LINUX_SOUND_PCM_H__

#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <sound/asound.h>

#define snd_pcm_substream_chip(substream)       ((substream)->private_data)

struct snd_pcm_hardware {
    unsigned int info;
    u64 formats;
    unsigned int rates;
    unsigned int rate_min, rate_max;
    unsigned int channels_min, channels_max;
    size_t buffer_bytes_max;
    size_t period_bytes_min, period_bytes_max;
    unsigned int periods_min, periods_max;
    size_t fifo_size;
};

struct page;
struct snd_pcm_substream;
struct snd_pcm_ops {
	int (*open)(struct snd_pcm_substream *substream);
	int (*close)(struct snd_pcm_substream *substream);
	int (*ioctl)(struct snd_pcm_substream *substream, unsigned int cmd, void *arg);
	int (*hw_params)(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params);
	int (*hw_free)(struct snd_pcm_substream *substream);
	int (*prepare)(struct snd_pcm_substream *substream);
	int (*trigger)(struct snd_pcm_substream *substream, int cmd);
	snd_pcm_uframes_t (*pointer)(struct snd_pcm_substream *substream);
	struct page *(*page)(struct snd_pcm_substream *substream, unsigned long offset);
};

struct snd_pcm_runtime {
    struct snd_pcm_hardware hw;
    unsigned int frame_bits;
    unsigned int sample_bits;
    snd_pcm_uframes_t period_size;
    unsigned int periods;
    snd_pcm_uframes_t buffer_size;
    size_t byte_align;

    struct snd_pcm_mmap_status *status;
    struct snd_pcm_mmap_control *control;

    unsigned char *dma_area;
    size_t dma_bytes;
};

struct snd_pcm_substream {
    struct snd_pcm *pcm;
    struct snd_pcm_str *pstr;
    void *private_data;
    int number;

    int stream;
    const struct snd_pcm_ops *ops;
    struct snd_pcm_runtime *runtime;
    vsf_slist_node_t list_node;
};

struct snd_pcm_str {
    int stream;
    struct snd_pcm *pcm;

    unsigned int substream_count;
    vsf_slist_t substream_list;

    struct device *dev;

    union {
        spinlock_t lock;
        struct mutex mutex;
    };
};

struct snd_pcm {
    struct snd_card *card;
    struct list_head list;
    int device;

    char id[64];
    char name[80];
    struct snd_pcm_str streams[2];

    void *private_data;
    void (*private_free)(struct snd_pcm *pcm);

    bool nonatomic;
};

#define SNDRV_PCM_TRIGGER_STOP              0
#define SNDRV_PCM_TRIGGER_START             1
#define SNDRV_PCM_TRIGGER_PAUSE_PUSH        3
#define SNDRV_PCM_TRIGGER_PAUSE_RELEASE     4
#define SNDRV_PCM_TRIGGER_SUSPEND           5
#define SNDRV_PCM_TRIGGER_RESUME            6
#define SNDRV_PCM_TRIGGER_DRAIN             7

#define SNDRV_PCM_RATE_5512                 (1 << 0)
#define SNDRV_PCM_RATE_8000                 (1 << 1)
#define SNDRV_PCM_RATE_11025                (1 << 2)
#define SNDRV_PCM_RATE_16000                (1 << 3)
#define SNDRV_PCM_RATE_22050                (1 << 4)
#define SNDRV_PCM_RATE_32000                (1 << 5)
#define SNDRV_PCM_RATE_44100                (1 << 6)
#define SNDRV_PCM_RATE_48000                (1 << 7)
#define SNDRV_PCM_RATE_64000                (1 << 8)
#define SNDRV_PCM_RATE_88200                (1 << 9)
#define SNDRV_PCM_RATE_96000                (1 << 10)
#define SNDRV_PCM_RATE_176400               (1 << 11)
#define SNDRV_PCM_RATE_192000               (1 << 12)

#define SNDRV_PCM_RATE_CONTINUOUS           (1 << 30)
#define SNDRV_PCM_RATE_KNOT                 (1 << 31)

#define _SNDRV_PCM_FMTBIT(fmt)              (1ULL << (int)SNDRV_PCM_FORMAT_##fmt)
#define SNDRV_PCM_FMTBIT_S8                 _SNDRV_PCM_FMTBIT(S8)
#define SNDRV_PCM_FMTBIT_U8                 _SNDRV_PCM_FMTBIT(U8)
#define SNDRV_PCM_FMTBIT_S16_LE             _SNDRV_PCM_FMTBIT(S16_LE)
#define SNDRV_PCM_FMTBIT_S16_BE             _SNDRV_PCM_FMTBIT(S16_BE)
#define SNDRV_PCM_FMTBIT_U16_LE             _SNDRV_PCM_FMTBIT(U16_LE)
#define SNDRV_PCM_FMTBIT_U16_BE             _SNDRV_PCM_FMTBIT(U16_BE)
#define SNDRV_PCM_FMTBIT_S24_LE             _SNDRV_PCM_FMTBIT(S24_LE)
#define SNDRV_PCM_FMTBIT_S24_BE             _SNDRV_PCM_FMTBIT(S24_BE)
#define SNDRV_PCM_FMTBIT_U24_LE             _SNDRV_PCM_FMTBIT(U24_LE)
#define SNDRV_PCM_FMTBIT_U24_BE             _SNDRV_PCM_FMTBIT(U24_BE)
#define SNDRV_PCM_FMTBIT_S32_LE             _SNDRV_PCM_FMTBIT(S32_LE)
#define SNDRV_PCM_FMTBIT_S32_BE             _SNDRV_PCM_FMTBIT(S32_BE)
#define SNDRV_PCM_FMTBIT_U32_LE             _SNDRV_PCM_FMTBIT(U32_LE)
#define SNDRV_PCM_FMTBIT_U32_BE             _SNDRV_PCM_FMTBIT(U32_BE)
#define SNDRV_PCM_FMTBIT_FLOAT_LE           _SNDRV_PCM_FMTBIT(FLOAT_LE)
#define SNDRV_PCM_FMTBIT_FLOAT_BE           _SNDRV_PCM_FMTBIT(FLOAT_BE)
#define SNDRV_PCM_FMTBIT_FLOAT64_LE         _SNDRV_PCM_FMTBIT(FLOAT64_LE)
#define SNDRV_PCM_FMTBIT_FLOAT64_BE         _SNDRV_PCM_FMTBIT(FLOAT64_BE)
#define SNDRV_PCM_FMTBIT_IEC958_SUBFRAME_LE _SNDRV_PCM_FMTBIT(IEC958_SUBFRAME_LE)
#define SNDRV_PCM_FMTBIT_IEC958_SUBFRAME_BE _SNDRV_PCM_FMTBIT(IEC958_SUBFRAME_BE)
#define SNDRV_PCM_FMTBIT_MU_LAW             _SNDRV_PCM_FMTBIT(MU_LAW)
#define SNDRV_PCM_FMTBIT_A_LAW              _SNDRV_PCM_FMTBIT(A_LAW)
#define SNDRV_PCM_FMTBIT_IMA_ADPCM          _SNDRV_PCM_FMTBIT(IMA_ADPCM)
#define SNDRV_PCM_FMTBIT_MPEG               _SNDRV_PCM_FMTBIT(MPEG)
#define SNDRV_PCM_FMTBIT_GSM                _SNDRV_PCM_FMTBIT(GSM)
#define SNDRV_PCM_FMTBIT_SPECIAL            _SNDRV_PCM_FMTBIT(SPECIAL)
#define SNDRV_PCM_FMTBIT_S24_3LE            _SNDRV_PCM_FMTBIT(S24_3LE)
#define SNDRV_PCM_FMTBIT_U24_3LE            _SNDRV_PCM_FMTBIT(U24_3LE)
#define SNDRV_PCM_FMTBIT_S24_3BE            _SNDRV_PCM_FMTBIT(S24_3BE)
#define SNDRV_PCM_FMTBIT_U24_3BE            _SNDRV_PCM_FMTBIT(U24_3BE)
#define SNDRV_PCM_FMTBIT_S20_3LE            _SNDRV_PCM_FMTBIT(S20_3LE)
#define SNDRV_PCM_FMTBIT_U20_3LE            _SNDRV_PCM_FMTBIT(U20_3LE)
#define SNDRV_PCM_FMTBIT_S20_3BE            _SNDRV_PCM_FMTBIT(S20_3BE)
#define SNDRV_PCM_FMTBIT_U20_3BE            _SNDRV_PCM_FMTBIT(U20_3BE)
#define SNDRV_PCM_FMTBIT_S18_3LE            _SNDRV_PCM_FMTBIT(S18_3LE)
#define SNDRV_PCM_FMTBIT_U18_3LE            _SNDRV_PCM_FMTBIT(U18_3LE)
#define SNDRV_PCM_FMTBIT_S18_3BE            _SNDRV_PCM_FMTBIT(S18_3BE)
#define SNDRV_PCM_FMTBIT_U18_3BE            _SNDRV_PCM_FMTBIT(U18_3BE)
#define SNDRV_PCM_FMTBIT_G723_24            _SNDRV_PCM_FMTBIT(G723_24)
#define SNDRV_PCM_FMTBIT_G723_24_1B         _SNDRV_PCM_FMTBIT(G723_24_1B)
#define SNDRV_PCM_FMTBIT_G723_40            _SNDRV_PCM_FMTBIT(G723_40)
#define SNDRV_PCM_FMTBIT_G723_40_1B         _SNDRV_PCM_FMTBIT(G723_40_1B)
#define SNDRV_PCM_FMTBIT_DSD_U8             _SNDRV_PCM_FMTBIT(DSD_U8)
#define SNDRV_PCM_FMTBIT_DSD_U16_LE         _SNDRV_PCM_FMTBIT(DSD_U16_LE)
#define SNDRV_PCM_FMTBIT_DSD_U32_LE         _SNDRV_PCM_FMTBIT(DSD_U32_LE)
#define SNDRV_PCM_FMTBIT_DSD_U16_BE         _SNDRV_PCM_FMTBIT(DSD_U16_BE)
#define SNDRV_PCM_FMTBIT_DSD_U32_BE         _SNDRV_PCM_FMTBIT(DSD_U32_BE)

static inline int snd_pcm_running(struct snd_pcm_substream *substream)
{
    return  substream->runtime->status->state == SNDRV_PCM_STATE_RUNNING
        ||  (   substream->runtime->status->state == SNDRV_PCM_STATE_DRAINING
            &&  substream->stream == SNDRV_PCM_STREAM_PLAYBACK);
}


static inline ssize_t bytes_to_samples(struct snd_pcm_runtime *runtime, ssize_t size)
{
	return size * 8 / runtime->sample_bits;
}

static inline snd_pcm_sframes_t bytes_to_frames(struct snd_pcm_runtime *runtime, ssize_t size)
{
	return size * 8 / runtime->frame_bits;
}

static inline ssize_t samples_to_bytes(struct snd_pcm_runtime *runtime, ssize_t size)
{
	return size * runtime->sample_bits / 8;
}

static inline ssize_t frames_to_bytes(struct snd_pcm_runtime *runtime, snd_pcm_sframes_t size)
{
	return size * runtime->frame_bits / 8;
}

static inline int frame_aligned(struct snd_pcm_runtime *runtime, ssize_t bytes)
{
	return bytes % runtime->byte_align == 0;
}

/**
 * snd_pcm_lib_buffer_bytes - Get the buffer size of the current PCM in bytes
 * @substream: PCM substream
 */
static inline size_t snd_pcm_lib_buffer_bytes(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	return frames_to_bytes(runtime, runtime->buffer_size);
}

static inline const struct snd_interval * hw_param_interval_c(const struct snd_pcm_hw_params *params, snd_pcm_hw_param_t var)
{
	return &params->intervals[var - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
}

static inline unsigned int params_buffer_bytes(const struct snd_pcm_hw_params *p)
{
    return hw_param_interval_c(p, SNDRV_PCM_HW_PARAM_BUFFER_BYTES)->min;
}

#define snd_pcm_stream_lock_irqsave(__substream, __flags) ({                    \
    if ((__substream)->pcm->nonatomic) {                                        \
        mutex_lock(&(__substream)->pstr->mutex);                                \
    } else {                                                                    \
        spin_lock_irqsave(&(__substream)->pstr->lock, (__flags));               \
    }                                                                           \
    __flags;                                                                    \
})

#define snd_pcm_stream_unlock_irqrestore(__substream, __flags) ({               \
    if ((__substream)->pcm->nonatomic) {                                        \
        mutex_unlock(&(__substream)->pstr->mutex);                              \
    } else {                                                                    \
        spin_unlock_irqrestore(&(__substream)->pstr->lock, (__flags));          \
    }                                                                           \
})

int snd_pcm_new(struct snd_card *card, const char *id, int device, int playback_count, int capture_count, struct snd_pcm **rpcm);

void snd_pcm_set_ops(struct snd_pcm *pcm, int direction, const struct snd_pcm_ops *ops);

void snd_pcm_period_elapsed(struct snd_pcm_substream *substream);

int snd_pcm_lib_alloc_vmalloc_buffer(struct snd_pcm_substream *substream, size_t size);
int snd_pcm_lib_free_vmalloc_buffer(struct snd_pcm_substream *substream);
struct page * snd_pcm_lib_get_vmalloc_page(struct snd_pcm_substream *substream, unsigned long offset);

int snd_pcm_lib_ioctl(struct snd_pcm_substream *substream, unsigned int cmd, void *arg);     

#endif
