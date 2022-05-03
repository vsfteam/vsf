#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#include "../common/usrapp_common.h"

#if APP_USE_AUDIO_DEMO == ENABLED

#if APP_USE_LINUX_DEMO != ENABLED
#   error "audio demo depends on linux sub system"
#endif

#ifndef APP_CFG_AUDIO_BUFFER_SIZE
#   ifdef __WIN__
#       define APP_CFG_AUDIO_BUFFER_SIZE    (512 * 1024)
#   else
#       define APP_CFG_AUDIO_BUFFER_SIZE    (8 * 1024)
#   endif
#endif

static void print_help(char *exe_name)
{
    printf(
"format: \r\n\
\t%s pcm_file channel_number sample_rate bit_width\r\n\
\t%s wav_file\r\n\
\t%s mp3_file\r\n", exe_name, exe_name, exe_name);
}

describe_mem_stream(audio_mem_stream, APP_CFG_AUDIO_BUFFER_SIZE)

int audio_play_main(int argc, char *argv[])
{
    if (argc < 2) {
        print_help(argv[0]);
        return 0;
    }

    char * ext = vk_file_getfileext(argv[1]);
    for (char *tmp = ext; *tmp != '\0'; tmp++) {
        *tmp = tolower(*tmp);
    }

    audio_mem_stream.is_ticktock_read = true;
    VSF_STREAM_INIT(&audio_mem_stream);

    vk_file_stream_t file_stream;
    uint_fast32_t delay_us;
    enum {
        FILE_FORMAT_NONE,
        FILE_FORMAT_PCM,
        FILE_FORMAT_WAV,
        FILE_FORMAT_MP3,
    } file_format;
    union {
        vk_audio_format_t format;
        vk_wav_t wav;
    } ctx;
    int result = 0;

    vk_audio_dev_t *audio_dev = usrapp_audio_common.default_dev;
    vk_audio_stream_t *audio_stream = NULL;
    vk_audio_init(audio_dev);
    for (uint_fast8_t i = 0; i < audio_dev->stream_num; i++) {
        if (0 == audio_dev->stream[i].dir_in1out0) {
            audio_stream = &audio_dev->stream[i];
            break;
        }
    }
    if (NULL == audio_stream) {
        printf("no audio playback device found!\r\n");
        return -1;
    }

    if (!strcmp(ext, "pcm")) {
        file_format = FILE_FORMAT_PCM;
        if (argc != 5) {
            print_help(argv[0]);
            result = -1;
            goto cleanup;
        }

        ctx.format.channel_num      = strtoul(argv[2], NULL, 0);
        ctx.format.sample_bit_width = strtoul(argv[4], NULL, 0);
        ctx.format.sample_rate      = strtoul(argv[3], NULL, 0);

        delay_us =  1000000ULL
                *   (APP_CFG_AUDIO_BUFFER_SIZE / ((ctx.format.sample_bit_width / 8) * ctx.format.channel_num))
                /   ctx.format.sample_rate;
        vk_audio_start(audio_dev, audio_stream->stream_index, &audio_mem_stream.use_as__vsf_stream_t, &ctx.format);
    } else if (!strcmp(ext, "wav")) {
        file_format = FILE_FORMAT_WAV;
        if (argc != 2) {
            print_help(argv[0]);
            result = -1;
            goto cleanup;
        }

        ctx.wav.audio_dev           = audio_dev;
        ctx.wav.stream              = &audio_mem_stream.use_as__vsf_stream_t;
        ctx.wav.audio_stream        = audio_stream->stream_index;
        delay_us = 1000000ULL * 1;
        vk_wav_playback_start(&ctx.wav);
    } else if (!strcmp(ext, "mp3")) {
        file_format = FILE_FORMAT_MP3;
        if (argc != 2) {
            print_help(argv[0]);
            result = -1;
            goto cleanup;
        }
        printf("not supported yet\r\n");
        result = -1;
        goto cleanup;
    } else {
        print_help(argv[0]);
        file_format = FILE_FORMAT_NONE;
        goto cleanup;
    }

    int fd = open(argv[1], 0);
    if (fd < 0) {
        printf("fail to open %s\r\n", argv[1]);
        result = -1;
    } else {
        extern vk_file_t * __vsf_linux_get_fs_ex(vsf_linux_process_t *process, int fd);
        file_stream.file = __vsf_linux_get_fs_ex(NULL, fd);

        vk_file_read_stream(&file_stream, &audio_mem_stream.use_as__vsf_stream_t,
                file_stream.file->size);

        close(fd);
        usleep(delay_us);
    }

cleanup:
    switch (file_format) {
    case FILE_FORMAT_PCM:
        vk_audio_stop(audio_dev, audio_stream->stream_index);
        break;
    case FILE_FORMAT_WAV:
        vk_wav_playback_stop(&ctx.wav);
        break;
    }

    return result;
}

#endif
