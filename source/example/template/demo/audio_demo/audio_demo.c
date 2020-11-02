#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#include "../common/usrapp_common.h"

#if APP_USE_AUDIO_DEMO == ENABLED

#if APP_USE_LINUX_DEMO != ENABLED
#   error "audio demo depends on linux sub system"
#endif

#ifndef APP_CFG_AUDIO_BUFFER_SIZE
#   define APP_CFG_AUDIO_BUFFER_SIZE        (8 *1024)
#endif

static void print_help(char *exe_name)
{
    printf(
"format: \r\n\
\t%s pcm_file channel_number sample_rate bit_width\r\n\
\t%s wav_file\r\n\
\t%s mp3_file\r\n", exe_name, exe_name, exe_name);
}

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

    // make sure current VSF_LINUX_CFG_STACKSIZE can hold the audio_stream
    describe_mem_stream(audio_stream, APP_CFG_AUDIO_BUFFER_SIZE)
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

    audio_stream.is_ticktock_read = true;
    vk_audio_init(usrapp_audio_common.default_dev);
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
        vk_audio_play_start(usrapp_audio_common.default_dev, &audio_stream.use_as__vsf_stream_t, &ctx.format);
    } else if (!strcmp(ext, "wav")) {
        file_format = FILE_FORMAT_WAV;
        if (argc != 2) {
            print_help(argv[0]);
            result = -1;
            goto cleanup;
        }

        ctx.wav.audio_dev   = usrapp_audio_common.default_dev;
        ctx.wav.stream      = &audio_stream.use_as__vsf_stream_t;
        delay_us = 1000000ULL * 1;
        vk_wav_play_start(&ctx.wav);
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

    vk_file_open(NULL, argv[1], 0, &file_stream.file);
    if (NULL == file_stream.file) {
        printf("fail to open %s\r\n", argv[1]);
        result = -1;
    } else {
        vk_file_read_stream(&file_stream, 0, file_stream.file->size, &audio_stream.use_as__vsf_stream_t);
        vk_file_close(file_stream.file);

        usleep(delay_us);
    }

cleanup:
    switch (file_format) {
    case FILE_FORMAT_PCM:
        vk_audio_play_stop(usrapp_audio_common.default_dev);
        break;
    case FILE_FORMAT_WAV:
        vk_wav_play_stop(&ctx.wav);
        break;
    }

    return 0;
}

#endif
