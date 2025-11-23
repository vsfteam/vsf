#ifndef __VSF_LINUX_SOUND_ASEQUENCER_H__
#define __VSF_LINUX_SOUND_ASEQUENCER_H__

#define SNDRV_SEQ_VERSION                           SNDRV_PROTOCOL_VERSION(1, 0, 5)

struct snd_seq_addr {
    unsigned char client;
    unsigned char port;
};

#define SNDRV_SEQ_EVENT_UMP                         (1 << 5)

struct snd_seq_real_time {
    unsigned int tv_sec;
    unsigned int tv_nsec;
};

typedef unsigned int snd_seq_tick_time_t;

union snd_seq_timestamp {
    snd_seq_tick_time_t tick;
    struct snd_seq_real_time time;
};

struct snd_seq_system_info {
    int queues;
    int clients;
    int ports;
    int channels;
    int cur_clients;
    int cur_queues;
};

typedef int __bitwise snd_seq_client_type_t;
#define NO_CLIENT                                   ((__force snd_seq_client_type_t) 0)
#define USER_CLIENT                                 ((__force snd_seq_client_type_t) 1)

#define SNDRV_SEQ_FILTER_BROADCAST                  (1 << 0)
#define SNDRV_SEQ_FILTER_BOUNCE                     (1 << 2)
#define SNDRV_SEQ_FILTER_NO_CONVERT                 (1 << 30)
#define SNDRV_SEQ_FILTER_USE_EVENT                  (1 << 31)

struct snd_seq_client_info {
    int client;
    snd_seq_client_type_t type;
    char name[64];
    unsigned int filter;
    unsigned char event_filter[32];
    int num_ports;
    int event_lost;
    int card;
    int pid;
    unsigned int midi_version;
    unsigned int group_filter;
};

struct snd_seq_client_pool {
    int client;
    int output_pool;
    int input_pool;
    int output_room;
    int output_free;
    int input_free;
};

#define SNDRV_SEQ_REMOVE_INPUT                      (1 << 0)
#define SNDRV_SEQ_REMOVE_OUTPUT                     (1 << 1)
#define SNDRV_SEQ_REMOVE_DEST                       (1 << 2)
#define SNDRV_SEQ_REMOVE_DEST_CHANNEL               (1 << 3)
#define SNDRV_SEQ_REMOVE_TIME_BEFORE                (1 << 4)
#define SNDRV_SEQ_REMOVE_TIME_AFTER                 (1 << 5)
#define SNDRV_SEQ_REMOVE_TIME_TICK                  (1 << 6)
#define SNDRV_SEQ_REMOVE_EVENT_TYPE                 (1 << 7)
#define SNDRV_SEQ_REMOVE_IGNORE_OFF                 (1 << 8)
#define SNDRV_SEQ_REMOVE_TAG_MATCH                  (1 << 9)

struct snd_seq_remove_events {
    unsigned int remove_mode;
    union snd_seq_timestamp time;
    unsigned char queue;
    struct snd_seq_addr dest;
    unsigned char channel;
    int type;
    char tag;
};

#define SNDRV_SEQ_PORT_CAP_DUPLEX                   (1 << 4)

#define SNDRV_SEQ_PORT_TYPE_MIDI_GENERIC            (1 << 1)

#define SNDRV_SEQ_PORT_FLG_IS_MIDI1                 (1 << 3)

#define SNDRV_SEQ_PORT_FLG_GIVEN_PORT               (1 << 0)
#define SNDRV_SEQ_PORT_FLG_TIMESTAMP                (1 << 1)
#define SNDRV_SEQ_PORT_FLG_TIME_REAL                (1 << 2)

struct snd_seq_port_info {
    struct snd_seq_addr addr;
    char name[64];
    unsigned int capability;
    unsigned int type;
    int midi_channels;
    int midi_voices;
    int synth_voices;
    int read_use;
    int write_use;
    void *kernel;
    unsigned int flags;
    unsigned char time_queue;
    unsigned char direction;
    unsigned char ump_group;
};

struct snd_seq_queue_info {
    int queue;
    int owner;
    unsigned locked : 1;
    char name[64];
    unsigned int flags;
};

struct snd_seq_queue_status {
    int queue;
    int events;
    snd_seq_tick_time_t tick;
    struct snd_seq_real_time time;
    int running;
};

struct snd_seq_queue_tempo {
    int queue;
    unsigned int tempo;
    int ppq;
    unsigned int skew_value;
    unsigned int skew_base;
    unsigned short tempo_base;
};

struct snd_seq_queue_timer {
    int queue;
    int type;
    union {
        struct {
            struct snd_timer_id id;
            unsigned int resolution;
        } alsa;
    } u;
};

struct snd_seq_queue_client {
    int queue;
    int client;
    int used;
};

#define SNDRV_SEQ_PORT_SUBS_EXCLUSIVE               (1 << 0)
#define SNDRV_SEQ_PORT_SUBS_TIMESTAMP               (1 << 1)
#define SNDRV_SEQ_PORT_SUBS_TIME_REAL               (1 << 2)

struct snd_seq_port_subscribe {
    struct snd_seq_addr sender;
    struct snd_seq_addr dest;
    unsigned int flags;
    unsigned char queue;
};

struct snd_seq_query_subs {
    struct snd_seq_addr root;
    int type;
    int index;
    int num_subs;
    struct snd_seq_addr addr;
    unsigned char queue;
    unsigned int flags;
};

#define SNDRV_SEQ_CLIENT_UMP_INFO_ENDPOINT          0
#define SNDRV_SEQ_CLIENT_UMP_INFO_BLOCK             1

struct snd_seq_client_ump_info {
    int client;
    int type;
    unsigned char info[512];
};

#define SNDRV_SEQ_IOCTL_PVERSION                    _IOR('S', 0x00, int)
#define SNDRV_SEQ_IOCTL_CLIENT_ID                   _IOR('S', 0x01, int)
#define SNDRV_SEQ_IOCTL_SYSTEM_INFO                 _IOWR('S', 0x02, struct snd_seq_system_info)
#define SNDRV_SEQ_IOCTL_USER_PVERSION               _IOW('S', 0x04, int)

#define SNDRV_SEQ_IOCTL_GET_CLIENT_INFO             _IOWR('S', 0x10, struct snd_seq_client_info)
#define SNDRV_SEQ_IOCTL_SET_CLIENT_INFO             _IOW('S', 0x11, struct snd_seq_client_info)

#define SNDRV_SEQ_IOCTL_GET_CLIENT_UMP_INFO         _IOWR('S', 0x12, struct snd_seq_client_ump_info)
#define SNDRV_SEQ_IOCTL_SET_CLIENT_UMP_INFO         _IOWR('S', 0x13, struct snd_seq_client_ump_info)

#define SNDRV_SEQ_IOCTL_CREATE_PORT                 _IOWR('S', 0x20, struct snd_seq_port_info)
#define SNDRV_SEQ_IOCTL_DELETE_PORT                 _IOW ('S', 0x21, struct snd_seq_port_info)
#define SNDRV_SEQ_IOCTL_GET_PORT_INFO               _IOWR('S', 0x22, struct snd_seq_port_info)
#define SNDRV_SEQ_IOCTL_SET_PORT_INFO               _IOW ('S', 0x23, struct snd_seq_port_info)

#define SNDRV_SEQ_IOCTL_SUBSCRIBE_PORT              _IOW('S', 0x30, struct snd_seq_port_subscribe)
#define SNDRV_SEQ_IOCTL_UNSUBSCRIBE_PORT            _IOW('S', 0x31, struct snd_seq_port_subscribe)

#define SNDRV_SEQ_IOCTL_CREATE_QUEUE                _IOWR('S', 0x32, struct snd_seq_queue_info)
#define SNDRV_SEQ_IOCTL_DELETE_QUEUE                _IOW('S', 0x33, struct snd_seq_queue_info)
#define SNDRV_SEQ_IOCTL_GET_QUEUE_INFO              _IOWR('S', 0x34, struct snd_seq_queue_info)
#define SNDRV_SEQ_IOCTL_SET_QUEUE_INFO              _IOWR('S', 0x35, struct snd_seq_queue_info)
#define SNDRV_SEQ_IOCTL_GET_NAMED_QUEUE             _IOWR('S', 0x36, struct snd_seq_queue_info)
#define SNDRV_SEQ_IOCTL_GET_QUEUE_STATUS            _IOWR('S', 0x40, struct snd_seq_queue_status)
#define SNDRV_SEQ_IOCTL_GET_QUEUE_TEMPO             _IOWR('S', 0x41, struct snd_seq_queue_tempo)
#define SNDRV_SEQ_IOCTL_SET_QUEUE_TEMPO             _IOW('S', 0x42, struct snd_seq_queue_tempo)
#define SNDRV_SEQ_IOCTL_GET_QUEUE_TIMER             _IOWR('S', 0x45, struct snd_seq_queue_timer)
#define SNDRV_SEQ_IOCTL_SET_QUEUE_TIMER             _IOW('S', 0x46, struct snd_seq_queue_timer)
#define SNDRV_SEQ_IOCTL_GET_QUEUE_CLIENT            _IOWR('S', 0x49, struct snd_seq_queue_client)
#define SNDRV_SEQ_IOCTL_SET_QUEUE_CLIENT            _IOW('S', 0x4a, struct snd_seq_queue_client)
#define SNDRV_SEQ_IOCTL_GET_CLIENT_POOL             _IOWR('S', 0x4b, struct snd_seq_client_pool)
#define SNDRV_SEQ_IOCTL_SET_CLIENT_POOL             _IOW('S', 0x4c, struct snd_seq_client_pool)
#define SNDRV_SEQ_IOCTL_REMOVE_EVENTS               _IOW('S', 0x4e, struct snd_seq_remove_events)
#define SNDRV_SEQ_IOCTL_QUERY_SUBS                  _IOWR('S', 0x4f, struct snd_seq_query_subs)
#define SNDRV_SEQ_IOCTL_GET_SUBSCRIPTION            _IOWR('S', 0x50, struct snd_seq_port_subscribe)
#define SNDRV_SEQ_IOCTL_QUERY_NEXT_CLIENT           _IOWR('S', 0x51, struct snd_seq_client_info)
#define SNDRV_SEQ_IOCTL_QUERY_NEXT_PORT             _IOWR('S', 0x52, struct snd_seq_port_info)

#endif
