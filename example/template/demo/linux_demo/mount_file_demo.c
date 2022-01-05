#include <unistd.h>
#include <sys/mount.h>

#include <stdio.h>

#if VSF_FS_USE_LITTLEFS == ENABLED
#   include "lfs_port.h"
#endif

#if VSF_USE_LINUX == ENABLED && APP_USE_LINUX_MOUNT_FILE_DEMO == ENABLED

int mount_file_main(int argc, char *argv[])
{
    vk_file_mal_t *file_mal;
    int result = 0;
    const vk_fs_op_t *filesystem = NULL;
    void *fsdata = NULL;

    if ((argc != 4) && (argc != 5)) {
    print_help:
        printf("format: %s src_file target_dir block_size [fs_type<fatfs|littlefs>]\r\n", argv[0]);
        return -1;
    }

    file_mal = malloc(sizeof(*file_mal));
    if (NULL == file_mal) {
        printf("not enough resources\r\n");
        return -1;
    }

    vk_file_open(NULL, argv[1], 0, &file_mal->file);
    if (NULL == file_mal->file) {
        printf("fail to open src_file %s\r\n", argv[1]);
        result = -1;
        goto cleanup;
    }

    file_mal->drv = &vk_file_mal_drv;
    file_mal->block_size = strtoul(argv[3], NULL, 0);
    vk_mal_init(&file_mal->use_as__vk_mal_t);

    if (5 == argc) {
#if VSF_FS_USE_FATFS == ENABLED
        if (!strcmp(argv[4], "fatfs")) {
            filesystem = NULL;
            fsdata = &file_mal->use_as__vk_mal_t;
        } else
#endif
#if VSF_FS_USE_LITTLEFS == ENABLED
        if (!strcmp(argv[4], "littlefs")) {
            filesystem = &vk_lfs_op;

            vk_lfs_info_t *fsinfo = malloc(sizeof(vk_lfs_info_t));
            if (NULL == fsinfo) {
                goto cleanup;
            }

            fsinfo->config.context = file_mal;
            fsinfo->config.read = vsf_lfs_mal_read;
            fsinfo->config.prog = vsf_lfs_mal_prog;
            fsinfo->config.erase = vsf_lfs_mal_erase;
            fsinfo->config.sync = vsf_lfs_mal_sync;
            fsinfo->config.lookahead_size = 8;
            fsinfo->config.block_cycles = 500;

            fsinfo->config.read_size = file_mal->block_size;
            fsinfo->config.prog_size = file_mal->block_size;
            fsinfo->config.block_size = file_mal->block_size;
            fsinfo->config.block_count = file_mal->size / file_mal->block_size;
            fsinfo->config.cache_size = file_mal->block_size;

            fsdata = fsinfo;
        } else
#endif
        {
            printf("fs_type %s not supported\r\n", argv[4]);
            goto print_help;
        }
    }
    return mount(NULL, argv[2], filesystem, 0, fsdata);

cleanup:
    if (result < 0) {
        if (file_mal->file != NULL) {
            vk_file_close(file_mal->file);
        }
        free(file_mal);
    }
    return result;
}
#endif
