#include <unistd.h>
#include <stdio.h>

#if VSF_USE_LINUX == ENABLED && APP_USE_LINUX_MOUNT_FILE_DEMO == ENABLED

int mount_file_main(int argc, char *argv[])
{
    vk_file_mal_t *file_mal;
    vk_malfs_mounter_t mounter = { 0 };
    int result = 0;

    if (argc != 3) {
        printf("format: %s src_file target_dir\r\n", argv[0]);
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

    vk_file_open(NULL, argv[2], 0, &mounter.dir);
    if (NULL == mounter.dir) {
        printf("fail to open target_dir %s\r\n", argv[2]);
        result = -1;
        goto cleanup;
    }
    if (!(mounter.dir->attr & VSF_FILE_ATTR_DIRECTORY)) {
        printf("target_dir %s is not a directory\r\n", argv[2]);
        result = -1;
        goto cleanup;
    }

    file_mal->drv = &vk_file_mal_drv;
    file_mal->block_size = 512;
    vk_mal_init(&file_mal->use_as__vk_mal_t);

    mounter.mal = &file_mal->use_as__vk_mal_t;
    vk_malfs_mount_mbr(&mounter);
    if (mounter.err != VSF_ERR_NONE) {
        printf("fail to mount src_file %s\r\n", argv[1]);
        result = -1;
        goto cleanup;
    }

cleanup:
    if (result < 0) {
        if (file_mal->file != NULL) {
            vk_file_close(file_mal->file);
        }
        free(file_mal);
    }
    if (mounter.dir != NULL) {
        vk_file_close(mounter.dir);
    }
    return result;
}
#endif
