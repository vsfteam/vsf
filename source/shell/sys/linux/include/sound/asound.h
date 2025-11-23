#ifndef __VSF_LINUX_SOUND_ASOUND_H__
#define __VSF_LINUX_SOUND_ASOUND_H__

#include <linux/ioctl.h>

#if     defined(__LITTLE_ENDIAN)
#   define SNDRV_LITTLE_ENDIAN
#elif   defined(__BIG_ENDIAN)
#   define NDRV_BIG_ENDIAN
#else
#   error "Unsupported endian..."
#endif

#define SNDRV_PROTOCOL_VERSION(major, minor, subminor) (((major)<<16)|((minor)<<8)|(subminor))
#define SNDRV_PROTOCOL_MAJOR(version) (((version)>>16)&0xffff)
#define SNDRV_PROTOCOL_MINOR(version) (((version)>>8)&0xff)
#define SNDRV_PROTOCOL_MICRO(version) ((version)&0xff)
#define SNDRV_PROTOCOL_INCOMPATIBLE(kversion, uversion)                         \
    (   SNDRV_PROTOCOL_MAJOR(kversion) != SNDRV_PROTOCOL_MAJOR(uversion)        \
    || (    SNDRV_PROTOCOL_MAJOR(kversion) == SNDRV_PROTOCOL_MAJOR(uversion)    \
        &&  SNDRV_PROTOCOL_MINOR(kversion) != SNDRV_PROTOCOL_MINOR(uversion)))

#define AES_IEC958_STATUS_SIZE                      24

struct snd_aes_iec958 {

};

struct snd_hwdep_info {
    unsigned int device;
    int card;
    unsigned char id[64];
    unsigned char name[80];
    int iface;
};

struct snd_hwdep_dsp_status {
    unsigned int version;
    unsigned char id[32];
    unsigned int num_dsps;
    unsigned int dsp_loaded;
    unsigned int chip_ready;
};

struct snd_hwdep_dsp_image {
    unsigned int index;
    unsigned char name[64];
    unsigned char *image;
    size_t length;
};

#define SNDRV_HWDEP_IOCTL_PVERSION                  _IOR('H', 0x00, int)
#define SNDRV_HWDEP_IOCTL_INFO                      _IOR('H', 0x01, struct snd_hwdep_info)
#define SNDRV_HWDEP_IOCTL_DSP_STATUS                _IOR('H', 0x02, struct snd_hwdep_dsp_status)
#define SNDRV_HWDEP_IOCTL_DSP_LOAD                  _IOW('H', 0x03, struct snd_hwdep_dsp_image)
#define SNDRV_UMP_IOCTL_ENDPOINT_INFO               _IOR('W', 0x40, struct snd_ump_endpoint_info)
#define SNDRV_UMP_IOCTL_BLOCK_INFO                  _IOR('W', 0x41, struct snd_ump_block_info)

// PCM

#define SNDRV_PCM_VERSION                           SNDRV_PROTOCOL_VERSION(2, 0, 15)

typedef unsigned long snd_pcm_uframes_t;
typedef signed long snd_pcm_sframes_t;

enum {
    SNDRV_PCM_STREAM_PLAYBACK = 0,
    SNDRV_PCM_STREAM_CAPTURE,
    SNDRV_PCM_STREAM_LAST = SNDRV_PCM_STREAM_CAPTURE,
};

typedef int __bitwise snd_pcm_access_t;
#define SNDRV_PCM_ACCESS_MMAP_INTERLEAVED           ((snd_pcm_access_t) 0)
#define SNDRV_PCM_ACCESS_MMAP_NONINTERLEAVED        ((snd_pcm_access_t) 1)
#define SNDRV_PCM_ACCESS_MMAP_COMPLEX               ((snd_pcm_access_t) 2)
#define SNDRV_PCM_ACCESS_RW_INTERLEAVED             ((snd_pcm_access_t) 3)
#define SNDRV_PCM_ACCESS_RW_NONINTERLEAVED          ((snd_pcm_access_t) 4)
#define SNDRV_PCM_ACCESS_LAST                       SNDRV_PCM_ACCESS_RW_NONINTERLEAVED

typedef int __bitwise snd_pcm_format_t;
#define SNDRV_PCM_FORMAT_S8                         ((snd_pcm_format_t) 0)
#define SNDRV_PCM_FORMAT_U8                         ((snd_pcm_format_t) 1)
#define SNDRV_PCM_FORMAT_S16_LE                     ((snd_pcm_format_t) 2)
#define SNDRV_PCM_FORMAT_S16_BE                     ((snd_pcm_format_t) 3)
#define SNDRV_PCM_FORMAT_U16_LE                     ((snd_pcm_format_t) 4)
#define SNDRV_PCM_FORMAT_U16_BE                     ((snd_pcm_format_t) 5)
#define SNDRV_PCM_FORMAT_S24_LE                     ((snd_pcm_format_t) 6)
#define SNDRV_PCM_FORMAT_S24_BE                     ((snd_pcm_format_t) 7)
#define SNDRV_PCM_FORMAT_U24_LE                     ((snd_pcm_format_t) 8)
#define SNDRV_PCM_FORMAT_U24_BE                     ((snd_pcm_format_t) 9)
#define SNDRV_PCM_FORMAT_S32_LE                     ((snd_pcm_format_t) 10)
#define SNDRV_PCM_FORMAT_S32_BE                     ((snd_pcm_format_t) 11)
#define SNDRV_PCM_FORMAT_U32_LE                     ((snd_pcm_format_t) 12)
#define SNDRV_PCM_FORMAT_U32_BE                     ((snd_pcm_format_t) 13)
#define SNDRV_PCM_FORMAT_FLOAT_LE                   ((snd_pcm_format_t) 14)
#define SNDRV_PCM_FORMAT_FLOAT_BE                   ((snd_pcm_format_t) 15)
#define SNDRV_PCM_FORMAT_FLOAT64_LE                 ((snd_pcm_format_t) 16)
#define SNDRV_PCM_FORMAT_FLOAT64_BE                 ((snd_pcm_format_t) 17)
#define SNDRV_PCM_FORMAT_IEC958_SUBFRAME_LE         ((snd_pcm_format_t) 18)
#define SNDRV_PCM_FORMAT_IEC958_SUBFRAME_BE         ((snd_pcm_format_t) 19)
#define SNDRV_PCM_FORMAT_MU_LAW                     ((snd_pcm_format_t) 20)
#define SNDRV_PCM_FORMAT_A_LAW                      ((snd_pcm_format_t) 21)
#define SNDRV_PCM_FORMAT_IMA_ADPCM                  ((snd_pcm_format_t) 22)
#define SNDRV_PCM_FORMAT_MPEG                       ((snd_pcm_format_t) 23)
#define SNDRV_PCM_FORMAT_GSM                        ((snd_pcm_format_t) 24)
#define SNDRV_PCM_FORMAT_S20_LE                     ((snd_pcm_format_t) 25)
#define SNDRV_PCM_FORMAT_S20_BE                     ((snd_pcm_format_t) 26)
#define SNDRV_PCM_FORMAT_U20_LE                     ((snd_pcm_format_t) 27)
#define SNDRV_PCM_FORMAT_U20_BE                     ((snd_pcm_format_t) 28)
#define SNDRV_PCM_FORMAT_SPECIAL                    ((snd_pcm_format_t) 31)
#define SNDRV_PCM_FORMAT_S24_3LE                    ((snd_pcm_format_t) 32)
#define SNDRV_PCM_FORMAT_S24_3BE                    ((snd_pcm_format_t) 33)
#define SNDRV_PCM_FORMAT_U24_3LE                    ((snd_pcm_format_t) 34)
#define SNDRV_PCM_FORMAT_U24_3BE                    ((snd_pcm_format_t) 35)
#define SNDRV_PCM_FORMAT_S20_3LE                    ((snd_pcm_format_t) 36)
#define SNDRV_PCM_FORMAT_S20_3BE                    ((snd_pcm_format_t) 37)
#define SNDRV_PCM_FORMAT_U20_3LE                    ((snd_pcm_format_t) 38)
#define SNDRV_PCM_FORMAT_U20_3BE                    ((snd_pcm_format_t) 39)
#define SNDRV_PCM_FORMAT_S18_3LE                    ((snd_pcm_format_t) 40)
#define SNDRV_PCM_FORMAT_S18_3BE                    ((snd_pcm_format_t) 41)
#define SNDRV_PCM_FORMAT_U18_3LE                    ((snd_pcm_format_t) 42)
#define SNDRV_PCM_FORMAT_U18_3BE                    ((snd_pcm_format_t) 43)
#define SNDRV_PCM_FORMAT_G723_24                    ((snd_pcm_format_t) 44)
#define SNDRV_PCM_FORMAT_G723_24_1B                 ((snd_pcm_format_t) 45)
#define SNDRV_PCM_FORMAT_G723_40                    ((snd_pcm_format_t) 46)
#define SNDRV_PCM_FORMAT_G723_40_1B                 ((snd_pcm_format_t) 47)
#define SNDRV_PCM_FORMAT_DSD_U8                     ((snd_pcm_format_t) 48)
#define SNDRV_PCM_FORMAT_DSD_U16_LE                 ((snd_pcm_format_t) 49)
#define SNDRV_PCM_FORMAT_DSD_U32_LE                 ((snd_pcm_format_t) 50)
#define SNDRV_PCM_FORMAT_DSD_U16_BE                 ((snd_pcm_format_t) 51)
#define SNDRV_PCM_FORMAT_DSD_U32_BE                 ((snd_pcm_format_t) 52)
#define SNDRV_PCM_FORMAT_LAST                       SNDRV_PCM_FORMAT_DSD_U32_BE
#define SNDRV_PCM_FORMAT_FIRST                      SNDRV_PCM_FORMAT_S8

#if     defined(SNDRV_LITTLE_ENDIAN)
#define SNDRV_PCM_FORMAT_S16                        SNDRV_PCM_FORMAT_S16_LE
#define SNDRV_PCM_FORMAT_U16                        SNDRV_PCM_FORMAT_U16_LE
#define SNDRV_PCM_FORMAT_S24                        SNDRV_PCM_FORMAT_S24_LE
#define SNDRV_PCM_FORMAT_U24                        SNDRV_PCM_FORMAT_U24_LE
#define SNDRV_PCM_FORMAT_S32                        SNDRV_PCM_FORMAT_S32_LE
#define SNDRV_PCM_FORMAT_U32                        SNDRV_PCM_FORMAT_U32_LE
#define SNDRV_PCM_FORMAT_FLOAT                      SNDRV_PCM_FORMAT_FLOAT_LE
#define SNDRV_PCM_FORMAT_FLOAT64                    SNDRV_PCM_FORMAT_FLOAT64_LE
#define SNDRV_PCM_FORMAT_IEC958_SUBFRAME            SNDRV_PCM_FORMAT_IEC958_SUBFRAME_LE
#define SNDRV_PCM_FORMAT_S20                        SNDRV_PCM_FORMAT_S20_LE
#define SNDRV_PCM_FORMAT_U20                        SNDRV_PCM_FORMAT_U20_LE
#elif   defined(NDRV_BIG_ENDIAN)
#define SNDRV_PCM_FORMAT_S16                        SNDRV_PCM_FORMAT_S16_BE
#define SNDRV_PCM_FORMAT_U16                        SNDRV_PCM_FORMAT_U16_BE
#define SNDRV_PCM_FORMAT_S24                        SNDRV_PCM_FORMAT_S24_BE
#define SNDRV_PCM_FORMAT_U24                        SNDRV_PCM_FORMAT_U24_BE
#define SNDRV_PCM_FORMAT_S32                        SNDRV_PCM_FORMAT_S32_BE
#define SNDRV_PCM_FORMAT_U32                        SNDRV_PCM_FORMAT_U32_BE
#define SNDRV_PCM_FORMAT_FLOAT                      SNDRV_PCM_FORMAT_FLOAT_BE
#define SNDRV_PCM_FORMAT_FLOAT64                    SNDRV_PCM_FORMAT_FLOAT64_BE
#define SNDRV_PCM_FORMAT_IEC958_SUBFRAME            SNDRV_PCM_FORMAT_IEC958_SUBFRAME_BE
#define SNDRV_PCM_FORMAT_S20                        SNDRV_PCM_FORMAT_S20_BE
#define SNDRV_PCM_FORMAT_U20                        SNDRV_PCM_FORMAT_U20_BE
#endif

typedef int __bitwise snd_pcm_subformat_t;
#define SNDRV_PCM_SUBFORMAT_STD                     ((snd_pcm_subformat_t) 0)
#define SNDRV_PCM_SUBFORMAT_MSBITS_MAX              ((snd_pcm_subformat_t) 1)
#define SNDRV_PCM_SUBFORMAT_MSBITS_20               ((snd_pcm_subformat_t) 2)
#define SNDRV_PCM_SUBFORMAT_MSBITS_24               ((snd_pcm_subformat_t) 3)
#define SNDRV_PCM_SUBFORMAT_LAST                    SNDRV_PCM_SUBFORMAT_MSBITS_24

#define SNDRV_PCM_INFO_MMAP                         (1 << 0)
#define SNDRV_PCM_INFO_MMAP_VALID                   (1 << 1)
#define SNDRV_PCM_INFO_DOUBLE                       (1 << 2)
#define SNDRV_PCM_INFO_BATCH                        (1 << 4)
#define SNDRV_PCM_INFO_PERFECT_DRAIN                (1 << 6)
#define SNDRV_PCM_INFO_INTERLEAVED                  (1 << 8)
#define SNDRV_PCM_INFO_NONINTERLEAVED               (1 << 9)
#define SNDRV_PCM_INFO_COMPLEX                      (1 << 10)
#define SNDRV_PCM_INFO_BLOCK_TRANSFER               (1 << 16)
#define SNDRV_PCM_INFO_OVERRANGE                    (1 << 17)
#define SNDRV_PCM_INFO_RESUME                       (1 << 18)
#define SNDRV_PCM_INFO_PAUSE                        (1 << 19)
#define SNDRV_PCM_INFO_HALF_DUPLEX                  (1 << 20)
#define SNDRV_PCM_INFO_JOINT_DUPLEX                 (1 << 21)
#define SNDRV_PCM_INFO_SYNC_START                   (1 << 22)
#define SNDRV_PCM_INFO_NO_PERIOD_WAKEUP             (1 << 23)
#define SNDRV_PCM_INFO_HAS_WALL_CLOCK               (1 << 24)
#define SNDRV_PCM_INFO_HAS_LINK_ATIME               (1 << 24)
#define SNDRV_PCM_INFO_HAS_LINK_ABSOLUTE_ATIME      (1 << 25)
#define SNDRV_PCM_INFO_HAS_LINK_ESTIMATED_ATIME     (1 << 26)
#define SNDRV_PCM_INFO_HAS_LINK_SYNCHRONIZED_ATIME  (1 << 27)

typedef int __bitwise snd_pcm_state_t;
#define SNDRV_PCM_STATE_PREPARED                    ((snd_pcm_state_t) 2)
#define SNDRV_PCM_STATE_RUNNING                     ((snd_pcm_state_t) 3)
#define SNDRV_PCM_STATE_XRUN                        ((snd_pcm_state_t) 4)
#define SNDRV_PCM_STATE_DRAINING                    ((snd_pcm_state_t) 5)
#define SNDRV_PCM_STATE_PAUSED                      ((snd_pcm_state_t) 6)
#define SNDRV_PCM_STATE_SUSPENDED                   ((snd_pcm_state_t) 7)
#define SNDRV_PCM_STATE_DISCONNECTED                ((snd_pcm_state_t) 8)
#define SNDRV_PCM_STATE_LAST                        SNDRV_PCM_STATE_DISCONNECTED

enum {
    SNDRV_PCM_MMAP_OFFSET_STATUS = 0x80000000,
    SNDRV_PCM_MMAP_OFFSET_CONTROL = 0x81000000,
};

struct snd_pcm_info {
    unsigned int device;
    unsigned int subdevice;
    int stream;
    int card;
    unsigned char id[64];
    unsigned char name[80];
    unsigned char subname[32];
    int dev_class;
    int dev_subclass;
    unsigned int subdevices_count;
    unsigned int subdevices_avail;
};

typedef int snd_pcm_hw_param_t;
#define SNDRV_PCM_HW_PARAM_ACCESS                   0
#define SNDRV_PCM_HW_PARAM_FORMAT                   1
#define SNDRV_PCM_HW_PARAM_SUBFORMAT                2
#define SNDRV_PCM_HW_PARAM_FIRST_MASK               SNDRV_PCM_HW_PARAM_ACCESS
#define SNDRV_PCM_HW_PARAM_LAST_MASK                SNDRV_PCM_HW_PARAM_SUBFORMAT

#define SNDRV_PCM_HW_PARAM_SAMPLE_BITS              8
#define SNDRV_PCM_HW_PARAM_FRAME_BITS               9
#define SNDRV_PCM_HW_PARAM_CHANNELS                 10
#define SNDRV_PCM_HW_PARAM_RATE                     11
#define SNDRV_PCM_HW_PARAM_PERIOD_TIME              12
#define SNDRV_PCM_HW_PARAM_PERIOD_SIZE              13
#define SNDRV_PCM_HW_PARAM_PERIOD_BYTES             14
#define SNDRV_PCM_HW_PARAM_PERIODS                  15
#define SNDRV_PCM_HW_PARAM_BUFFER_TIME              16
#define SNDRV_PCM_HW_PARAM_BUFFER_SIZE              17
#define SNDRV_PCM_HW_PARAM_BUFFER_BYTES             18
#define SNDRV_PCM_HW_PARAM_TICK_TIME                19
#define SNDRV_PCM_HW_PARAM_FIRST_INTERVAL           SNDRV_PCM_HW_PARAM_SAMPLE_BITS
#define SNDRV_PCM_HW_PARAM_LAST_INTERVAL            SNDRV_PCM_HW_PARAM_TICK_TIME

#define SNDRV_PCM_HW_PARAMS_NORESAMPLE              (1 << 0)
#define SNDRV_PCM_HW_PARAMS_EXPORT_BUFFER           (1 << 1)
#define SNDRV_PCM_HW_PARAMS_NO_PERIOD_WAKEUP        (1 << 2)
#define SNDRV_PCM_HW_PARAMS_NO_DRAIN_SILENCE        (1 << 3)

struct snd_interval {
    unsigned int min, max;
    unsigned int openmin : 1, openmax : 1, integer : 1, empty : 1;
};

#define SNDRV_MASK_MAX                              256
struct snd_mask {
    __u32 bits[(SNDRV_MASK_MAX + 31) / 32];
};

struct snd_pcm_hw_params {
    unsigned int flags;
    struct snd_mask masks[SNDRV_PCM_HW_PARAM_LAST_MASK - SNDRV_PCM_HW_PARAM_FIRST_MASK + 1];
    struct snd_interval intervals[SNDRV_PCM_HW_PARAM_LAST_INTERVAL - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL + 1];
    unsigned int rmask;
    unsigned int cmask;
    unsigned int info;
    unsigned int msbits;
    unsigned int rate_num;
    unsigned int rate_den;
    snd_pcm_uframes_t fifo_size;
    unsigned char sync[16];
};

struct snd_pcm_sw_params {
    int tstamp_mode;
    unsigned int period_step;
    unsigned int sleep_min;
    snd_pcm_uframes_t avail_min;
    snd_pcm_uframes_t xfer_align;
    snd_pcm_uframes_t start_threshold;
    snd_pcm_uframes_t stop_threshold;
    snd_pcm_uframes_t silence_threshold;
    snd_pcm_uframes_t silence_size;
    snd_pcm_uframes_t boundary;
    unsigned int proto;
    unsigned int tstamp_type;
    unsigned char reserved[56];
};

struct snd_pcm_channel_info {
    unsigned int channel;
    __kernel_off_t offset;
    unsigned int first;
    unsigned int step;
};

enum {
    SNDRV_PCM_AUDIO_TSTAMP_TYPE_COMPAT = 0,
    SNDRV_PCM_AUDIO_TSTAMP_TYPE_DEFAULT = 1,
    SNDRV_PCM_AUDIO_TSTAMP_TYPE_LINK = 2,
    SNDRV_PCM_AUDIO_TSTAMP_TYPE_LINK_ABSOLUTE = 3,
    SNDRV_PCM_AUDIO_TSTAMP_TYPE_LINK_ESTIMATED = 4,
    SNDRV_PCM_AUDIO_TSTAMP_TYPE_LINK_SYNCHRONIZED = 5,
    SNDRV_PCM_AUDIO_TSTAMP_TYPE_LAST = SNDRV_PCM_AUDIO_TSTAMP_TYPE_LINK_SYNCHRONIZED,
};

struct snd_pcm_status {
    snd_pcm_state_t state;
    struct timespec trigger_tstamp;
    struct timespec tstamp;
    snd_pcm_uframes_t appl_ptr;
    snd_pcm_uframes_t hw_ptr;
    snd_pcm_uframes_t delay;
    snd_pcm_uframes_t avail;
    snd_pcm_uframes_t avail_max;
    snd_pcm_uframes_t overrange;
    __u32 audio_tstamp_data;
    struct timespec audio_tstamp;
    struct timespec driver_tstamp;
    __u32 audio_tstamp_accuracy;
};

struct snd_pcm_mmap_status {
    snd_pcm_state_t state;
    snd_pcm_uframes_t hw_ptr;
    struct timespec tstamp;
};

struct snd_pcm_mmap_control {
    snd_pcm_uframes_t appl_ptr;
    snd_pcm_uframes_t avail_min;
};

#define SNDRV_PCM_SYNC_PTR_HWSYNC                   (1 << 0)
#define SNDRV_PCM_SYNC_PTR_APPL                     (1 << 1)
#define SNDRV_PCM_SYNC_PTR_AVAIL_MIN                (1 << 2)

struct snd_pcm_sync_ptr {
    unsigned int flags;
    union {
        struct snd_pcm_mmap_status status;
    } s;
    union {
        struct snd_pcm_mmap_control control;
    } c;
};

struct snd_xferi {
    snd_pcm_sframes_t result;
    void *buf;
    snd_pcm_uframes_t frames;
};

struct snd_xfern {
    snd_pcm_sframes_t result;
    void **bufs;
    snd_pcm_uframes_t frames;
};

enum {
    SNDRV_PCM_TSTAMP_TYPE_GETTIMEOFDAY = 0,
    SNDRV_PCM_TSTAMP_TYPE_MONOTONIC,
    SNDRV_PCM_TSTAMP_TYPE_MONOTONIC_RAW,
    SNDRV_PCM_TSTAMP_TYPE_LAST = SNDRV_PCM_TSTAMP_TYPE_MONOTONIC_RAW,
};

enum {
    SNDRV_CHMAP_UNKNOWN = 0,
    SNDRV_CHMAP_NA,
    SNDRV_CHMAP_MONO,
    SNDRV_CHMAP_FL,
    SNDRV_CHMAP_FR,
    SNDRV_CHMAP_RL,
    SNDRV_CHMAP_RR,
    SNDRV_CHMAP_FC,
    SNDRV_CHMAP_LFE,
    SNDRV_CHMAP_SL,
    SNDRV_CHMAP_SR,
    SNDRV_CHMAP_RC,
    SNDRV_CHMAP_FLC,
    SNDRV_CHMAP_FRC,
    SNDRV_CHMAP_RLC,
    SNDRV_CHMAP_RRC,
    SNDRV_CHMAP_FLW,
    SNDRV_CHMAP_FRW,
    SNDRV_CHMAP_FLH,
    SNDRV_CHMAP_FCH,
    SNDRV_CHMAP_FRH,
    SNDRV_CHMAP_TC,
    SNDRV_CHMAP_TFL,
    SNDRV_CHMAP_TFR,
    SNDRV_CHMAP_TFC,
    SNDRV_CHMAP_TRL,
    SNDRV_CHMAP_TRR,
    SNDRV_CHMAP_TRC,
    SNDRV_CHMAP_TFLC,
    SNDRV_CHMAP_TFRC,
    SNDRV_CHMAP_TSL,
    SNDRV_CHMAP_TSR,
    SNDRV_CHMAP_LLFE,
    SNDRV_CHMAP_RLFE,
    SNDRV_CHMAP_BC,
    SNDRV_CHMAP_BLC,
    SNDRV_CHMAP_BRC,
    SNDRV_CHMAP_LAST = SNDRV_CHMAP_BRC,
};

#define SNDRV_PCM_IOCTL_PVERSION                    _IOR('A', 0x00, int)
#define SNDRV_PCM_IOCTL_INFO                        _IOR('A', 0x01, struct snd_pcm_info)
#define SNDRV_PCM_IOCTL_TSTAMP                      _IOW('A', 0x02, int)
#define SNDRV_PCM_IOCTL_TTSTAMP                     _IOW('A', 0x03, int)
#define SNDRV_PCM_IOCTL_USER_PVERSION               _IOW('A', 0x04, int)
#define SNDRV_PCM_IOCTL_HW_REFINE                   _IOWR('A', 0x10, struct snd_pcm_hw_params)
#define SNDRV_PCM_IOCTL_HW_PARAMS                   _IOWR('A', 0x11, struct snd_pcm_hw_params)
#define SNDRV_PCM_IOCTL_HW_FREE                     _IO('A', 0x12)
#define SNDRV_PCM_IOCTL_SW_PARAMS                   _IOWR('A', 0x13, struct snd_pcm_sw_params)
#define SNDRV_PCM_IOCTL_STATUS                      _IOR('A', 0x20, struct snd_pcm_status)
#define SNDRV_PCM_IOCTL_DELAY                       _IOR('A', 0x21, snd_pcm_sframes_t)
#define SNDRV_PCM_IOCTL_HWSYNC                      _IO('A', 0x22)
#define SNDRV_PCM_IOCTL_SYNC_PTR                    _IOWR('A', 0x23, struct snd_pcm_sync_ptr)
#define SNDRV_PCM_IOCTL_STATUS_EXT                  _IOWR('A', 0x24, struct snd_pcm_status)
#define SNDRV_PCM_IOCTL_CHANNEL_INFO                _IOR('A', 0x32, struct snd_pcm_channel_info)
#define SNDRV_PCM_IOCTL_PREPARE                     _IO('A', 0x40)
#define SNDRV_PCM_IOCTL_RESET                       _IO('A', 0x41)
#define SNDRV_PCM_IOCTL_START                       _IO('A', 0x42)
#define SNDRV_PCM_IOCTL_DROP                        _IO('A', 0x43)
#define SNDRV_PCM_IOCTL_DRAIN                       _IO('A', 0x44)
#define SNDRV_PCM_IOCTL_PAUSE                       _IOW('A', 0x45, int)
#define SNDRV_PCM_IOCTL_REWIND                      _IOW('A', 0x46, snd_pcm_uframes_t)
#define SNDRV_PCM_IOCTL_RESUME                      _IO('A', 0x47)
#define SNDRV_PCM_IOCTL_XRUN                        _IO('A', 0x48)
#define SNDRV_PCM_IOCTL_FORWARD                     _IOW('A', 0x49, snd_pcm_uframes_t)
#define SNDRV_PCM_IOCTL_WRITEI_FRAMES               _IOW('A', 0x50, struct snd_xferi)
#define SNDRV_PCM_IOCTL_READI_FRAMES                _IOR('A', 0x51, struct snd_xferi)
#define SNDRV_PCM_IOCTL_WRITEN_FRAMES               _IOW('A', 0x52, struct snd_xfern)
#define SNDRV_PCM_IOCTL_READN_FRAMES                _IOR('A', 0x53, struct snd_xfern)
#define SNDRV_PCM_IOCTL_LINK                        _IOW('A', 0x60, int)
#define SNDRV_PCM_IOCTL_UNLINK                      _IO('A', 0x61)

// MIDI

#define SNDRV_RAWMIDI_VERSION                       SNDRV_PROTOCOL_VERSION(2, 0, 5)

#define SNDRV_RAWMIDI_STREAM_OUTPUT                 (1 << 0)
#define SNDRV_RAWMIDI_STREAM_INPUT                  (1 << 1)
#define SNDRV_RAWMIDI_INFO_UMP                      (1 << 3)

struct snd_rawmidi_info {
    unsigned int device;
    unsigned int subdevice;
    int stream;
    int card;
    unsigned int flags;
    unsigned char id[64];
    unsigned char name[80];
    unsigned char subname[32];
    unsigned int subdevices_count;
    unsigned int subdevices_avail;
    int tied_device;
};

#define SNDRV_RAWMIDI_MODE_FRAMING_MASK             (7 << 0)
#define SNDRV_RAWMIDI_MODE_FRAMING_SHIFT            0
#define SNDRV_RAWMIDI_MODE_FRAMING_NONE             (0 << 0)
#define SNDRV_RAWMIDI_MODE_FRAMING_TSTAMP           (1 << 0)
#define SNDRV_RAWMIDI_MODE_CLOCK_MASK               (7 << 3)
#define SNDRV_RAWMIDI_MODE_CLOCK_SHIFT              3
#define SNDRV_RAWMIDI_MODE_CLOCK_NONE               (0 << 3)
#define SNDRV_RAWMIDI_MODE_CLOCK_REALTIME           (1 << 3)
#define SNDRV_RAWMIDI_MODE_CLOCK_MONOTONIC          (2 << 3)
#define SNDRV_RAWMIDI_MODE_CLOCK_MONOTONIC_RAW      (3 << 3)

struct snd_rawmidi_params {
    int stream;
    size_t buffer_size;
    size_t avail_min;
    unsigned int no_active_sensing : 1;
    unsigned int mode;
    unsigned char reserved[12];
};

#define SNDRV_RAWMIDI_FRAMING_DATA_LENGTH           16

struct snd_rawmidi_framing_tstamp {
    __u8 frame_type;
    __u8 length;
    __u8 reserved[2];
    __u32 tv_nsec;
    __u64 tv_sec;
    __u8 data[SNDRV_RAWMIDI_FRAMING_DATA_LENGTH];
} VSF_CAL_PACKED;

struct snd_rawmidi_status {
    int stream;
    struct timespec tstamp;
    size_t avail;
    size_t xruns;
};

struct snd_ump_endpoint_info {
    int card;
    int device;
    unsigned int flags;
    unsigned int protocol_caps;
    unsigned int protocol;
    unsigned int num_blocks;
    unsigned short version;
    unsigned short family_id;
    unsigned short model_id;
    unsigned int manufacturer_id;
    unsigned char sw_revision[4];
    unsigned short padding;
    unsigned char name[128];
    unsigned product_id[128];
} VSF_CAL_PACKED;

struct snd_ump_block_info {
    int card;
    int device;
    unsigned char block_id;
    unsigned char direction;
    unsigned char active;
    unsigned char first_group;
    unsigned char num_groups;
    unsigned char midi_ci_version;
    unsigned char sysex8_streams;
    unsigned char ui_hint;
    unsigned int flags;
    unsigned char name[128];
} VSF_CAL_PACKED;

#define SNDRV_RAWMIDI_IOCTL_PVERSION                _IOR('W', 0x00, int)
#define SNDRV_RAWMIDI_IOCTL_INFO                    _IOR('W', 0x01, struct snd_rawmidi_info)
#define SNDRV_RAWMIDI_IOCTL_USER_PVERSION           _IOW('W', 0x02, int)
#define SNDRV_RAWMIDI_IOCTL_PARAMS                  _IOWR('W', 0x10, struct snd_rawmidi_params)
#define SNDRV_RAWMIDI_IOCTL_STATUS                  _IOWR('W', 0x20, struct snd_rawmidi_status)
#define SNDRV_RAWMIDI_IOCTL_DROP                    _IOW('W', 0x30, int)
#define SNDRV_RAWMIDI_IOCTL_DRAIN                   _IOW('W', 0x31, int)

// timer

#define SNDRV_TIMER_FLG_SLAVE                       (1 << 0)

struct snd_timer_id {
    int dev_class;
    int dev_sclass;
    int card;
    int device;
    int subdevice;
};

struct snd_timer_ginfo {
    struct snd_timer_id tid;
    unsigned int flags;
    int card;
    unsigned char id[64];
    unsigned char name[80];
    unsigned long reserved0;
    unsigned long resolution;
    unsigned long resolution_min;
    unsigned long resolution_max;
    unsigned int clients;
};

struct snd_timer_gparams {

};

struct snd_timer_gstatus {
};

struct snd_timer_select {
    struct snd_timer_id id;
};

struct snd_timer_info {
    unsigned int flags;
    int card;
    unsigned char id[64];
    unsigned char name[80];
    unsigned long reserved0;
    unsigned long resolution;
};

#define SNDRV_TIMER_PSFLG_AUTO                      (1 << 0)
#define SNDRV_TIMER_PSFLG_EXCLUSIVE                 (1 << 1)
#define SNDRV_TIMER_PSFLG_EARLY_EVENT               (1 << 2)

struct snd_timer_params {
    unsigned int flags;
    unsigned int ticks;
    unsigned int queue_size;
    unsigned int filter;
};

struct snd_timer_status {
    struct timespec tstamp;
    unsigned int resolution;
    unsigned int lost;
    unsigned int overrun;
    unsigned int queue;
};

#define SNDRV_TIMER_IOCTL_PVERSION                  _IOR('T', 0x00, int)
#define SNDRV_TIMER_IOCTL_NEXT_DEVICE               _IOWR('T', 0x01, struct snd_timer_id)
#define SNDRV_TIMER_IOCTL_TREAD                     _IOW('T', 0x02, int)
#define SNDRV_TIMER_IOCTL_GINFO                     _IOWR('T', 0x03, struct snd_timer_ginfo)
#define SNDRV_TIMER_IOCTL_GPARAMS                   _IOW('T', 0x04, struct snd_timer_gparams)
#define SNDRV_TIMER_IOCTL_GSTATUS                   _IOWR('T', 0x05, struct snd_timer_gstatus)
#define SNDRV_TIMER_IOCTL_SELECT                    _IOW('T', 0x10, struct snd_timer_select)
#define SNDRV_TIMER_IOCTL_INFO                      _IOR('T', 0x11, struct snd_timer_info)
#define SNDRV_TIMER_IOCTL_PARAMS                    _IOW('T', 0x12, struct snd_timer_params)
#define SNDRV_TIMER_IOCTL_STATUS                    _IOR('T', 0x14, struct snd_timer_status)
#define SNDRV_TIMER_IOCTL_START                     _IO('T', 0xa0)
#define SNDRV_TIMER_IOCTL_STOP                      _IO('T', 0xa1)
#define SNDRV_TIMER_IOCTL_CONTINUE                  _IO('T', 0xa2)

struct snd_ctl_card_info {
    int card;
    unsigned char id[16];
    unsigned char driver[16];
    unsigned char name[32];
    unsigned char longname[80];
    unsigned char mixername[80];
    unsigned char components[128];
};

typedef int __bitwise snd_ctl_elem_type_t;
#define SNDRV_CTL_ELEM_TYPE_NONE                    ((snd_ctl_elem_type_t) 0)
#define SNDRV_CTL_ELEM_TYPE_BOOLEAN                 ((snd_ctl_elem_type_t) 1)
#define SNDRV_CTL_ELEM_TYPE_INTEGER                 ((snd_ctl_elem_type_t) 2)
#define SNDRV_CTL_ELEM_TYPE_ENUMERATED              ((snd_ctl_elem_type_t) 3)
#define SNDRV_CTL_ELEM_TYPE_BYTES                   ((snd_ctl_elem_type_t) 4)
#define SNDRV_CTL_ELEM_TYPE_INTEGER64               ((snd_ctl_elem_type_t) 6)

typedef int __bitwise snd_ctl_elem_iface_t;
#define SNDRV_CTL_ELEM_IFACE_MIXER                  ((snd_ctl_elem_iface_t) 2)

#define SNDRV_CTL_ELEM_ACCESS_READ                  (1 << 0)
#define SNDRV_CTL_ELEM_ACCESS_WRITE                 (1 << 1)
#define SNDRV_CTL_ELEM_ACCESS_READWRITE             (SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_WRITE)
#define SNDRV_CTL_ELEM_ACCESS_VOLATILE              (1 << 2)
#define SNDRV_CTL_ELEM_ACCESS_TLV_READ              (1 << 4)
#define SNDRV_CTL_ELEM_ACCESS_TLV_WRITE             (1 << 5)
#define SNDRV_CTL_ELEM_ACCESS_TLV_READWRITE         (SNDRV_CTL_ELEM_ACCESS_TLV_READ | SNDRV_CTL_ELEM_ACCESS_TLV_WRITE)
#define SNDRV_CTL_ELEM_ACCESS_TLV_COMMAND           (1 << 6)
#define SNDRV_CTL_ELEM_ACCESS_INACTIVE              (1 << 8)
#define SNDRV_CTL_ELEM_ACCESS_LOCK                  (1 << 9)
#define SNDRV_CTL_ELEM_ACCESS_OWNER                 (1 << 10)
#define SNDRV_CTL_ELEM_ACCESS_TLV_CALLBACK          (1 << 28)
#define SNDRV_CTL_ELEM_ACCESS_USER                  (1 << 29)

#define SNDRV_CTL_ELEM_ID_NAME_MAXLEN               44

struct snd_ctl_elem_id {
    unsigned int numid;
    snd_ctl_elem_iface_t iface;
    unsigned int device;
    unsigned int subdevice;
    unsigned char name[SNDRV_CTL_ELEM_ID_NAME_MAXLEN];
    unsigned int index;
};

struct snd_ctl_elem_list {
    unsigned int offset;
    unsigned int space;
    unsigned int used;
    unsigned int count;
    struct snd_ctl_elem_id *pids;
};

struct snd_ctl_elem_info {
    struct snd_ctl_elem_id id;
    snd_ctl_elem_type_t type;
    unsigned int access;
    unsigned int count;
    __kernel_pid_t owner;
    union {
        struct {
            long min;
            long max;
            long step;
        } integer;
        struct {
            long long min;
            long long max;
            long long step;
        } integer64;
        struct {
            unsigned int items;
            unsigned int item;
            char name[64];
            u64 names_ptr;
            unsigned int names_length;
        } enumerated;
    } value;
};

struct snd_ctl_elem_value {
    struct snd_ctl_elem_id id;
    union {
        union {
            long value[128];
            long *value_ptr;
        } integer;
        union {
            long long value[64];
            long long *value_ptr;
        } integer64;
        union {
            unsigned int item[128];
            unsigned int *item_ptr;
        } enumerated;
        union {
            unsigned char data[512];
            unsigned char *data_ptr;
        } bytes;
        struct snd_aes_iec958 iec958;
    } value;
};

struct snd_ctl_tlv {
    unsigned int numid;
    unsigned int length;
    unsigned int tlv[];
};

#define SNDRV_CTL_IOCTL_PVERSION                    _IOR('U', 0x00, int)
#define SNDRV_CTL_IOCTL_CARD_INFO                   _IOR('U', 0x01, struct snd_ctl_card_info)
#define SNDRV_CTL_IOCTL_ELEM_LIST                   _IOWR('U', 0x10, struct snd_ctl_elem_list)
#define SNDRV_CTL_IOCTL_ELEM_INFO                   _IOWR('U', 0x11, struct snd_ctl_elem_info)
#define SNDRV_CTL_IOCTL_ELEM_READ                   _IOWR('U', 0x12, struct snd_ctl_elem_value)
#define SNDRV_CTL_IOCTL_ELEM_WRITE                  _IOWR('U', 0x13, struct snd_ctl_elem_value)
#define SNDRV_CTL_IOCTL_ELEM_LOCK                   _IOW('U', 0x14, struct snd_ctl_elem_id)
#define SNDRV_CTL_IOCTL_ELEM_UNLOCK                 _IOW('U', 0x15, struct snd_ctl_elem_id)
#define SNDRV_CTL_IOCTL_SUBSCRIBE_EVENTS            _IOWR('U', 0x16, int)
#define SNDRV_CTL_IOCTL_ELEM_ADD                    _IOWR('U', 0x17, struct snd_ctl_elem_info)
#define SNDRV_CTL_IOCTL_ELEM_REPLACE                _IOWR('U', 0x18, struct snd_ctl_elem_info)
#define SNDRV_CTL_IOCTL_ELEM_REMOVE                 _IOWR('U', 0x19, struct snd_ctl_elem_id)
#define SNDRV_CTL_IOCTL_TLV_READ                    _IOWR('U', 0x1a, struct snd_ctl_tlv)
#define SNDRV_CTL_IOCTL_TLV_WRITE                   _IOWR('U', 0x1b, struct snd_ctl_tlv)
#define SNDRV_CTL_IOCTL_TLV_COMMAND                 _IOWR('U', 0x1c, struct snd_ctl_tlv)
#define SNDRV_CTL_IOCTL_HWDEP_NEXT_DEVICE           _IOWR('U', 0x20, int)
#define SNDRV_CTL_IOCTL_HWDEP_INFO                  _IOR('U', 0x21, struct snd_hwdep_info)
#define SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE             _IOR('U', 0x30, int)
#define SNDRV_CTL_IOCTL_PCM_INFO                    _IOWR('U', 0x31, struct snd_pcm_info)
#define SNDRV_CTL_IOCTL_PCM_PREFER_SUBDEVICE        _IOW('U', 0x32, int)
#define SNDRV_CTL_IOCTL_RAWMIDI_NEXT_DEVICE         _IOWR('U', 0x40, int)
#define SNDRV_CTL_IOCTL_RAWMIDI_INFO                _IOWR('U', 0x41, struct snd_rawmidi_info)
#define SNDRV_CTL_IOCTL_RAWMIDI_PREFER_SUBDEVICE    _IOW('U', 0x42, int)
#define SNDRV_CTL_IOCTL_UMP_NEXT_DEVICE             _IOWR('U', 0x43, int)
#define SNDRV_CTL_IOCTL_UMP_ENDPOINT_INFO           _IOWR('U', 0x44, struct snd_ump_endpoint_info)
#define SNDRV_CTL_IOCTL_UMP_BLOCK_INFO              _IOWR('U', 0x45, struct snd_ump_block_info)
#define SNDRV_CTL_IOCTL_POWER                       _IOWR('U', 0xd0, int)
#define SNDRV_CTL_IOCTL_POWER_STATE                 _IOR('U', 0xd1, int)

enum sndrv_ctl_event_type {
    SNDRV_CTL_EVENT_ELEM = 0,
    SNDRV_CTL_EVENT_LAST = SNDRV_CTL_EVENT_ELEM,
};

#define SNDRV_CTL_EVENT_MASK_VALUE                  (1 << 0)
#define SNDRV_CTL_EVENT_MASK_INFO                   (1 << 1)
#define SNDRV_CTL_EVENT_MASK_ADD                    (1 << 2)
#define SNDRV_CTL_EVENT_MASK_TLV                    (1 << 3)
#define SNDRV_CTL_EVENT_MASK_REMOVE                 (~0U)

struct snd_ctl_event {
    int type;
    union {
        struct {
            unsigned int mask;
            struct snd_ctl_elem_id id;
        } elem;
    } data;
};

#endif
