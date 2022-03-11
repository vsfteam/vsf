#include "shell/sys/linux/vsf_linux_cfg.h"
#if VSF_USE_LINUX == ENABLED && APP_USE_LINUX_MOUNT_DEMO == ENABLED

// inherit linux fs class for bind related APIs
#define __VSF_LINUX_FS_CLASS_INHERIT__
#include <unistd.h>
#include <sys/mount.h>
#include <getopt.h>

#include <stdio.h>

/* 
 * Note that DO NOT use malloc/free for fsinfo, because fsinfo does not belong
 * to linux system, but belong to vsf fs system.
 */

#if VSF_FS_USE_LITTLEFS == ENABLED
#   include "lfs_port.h"
#endif

typedef struct __fs_param_t {
    char *device;
    uint32_t block_size;
} __fs_param_t;

typedef struct __fs_type_t __fs_type_t;
struct __fs_type_t {
    char *fs;
    const vk_fs_op_t *fsop;
    void * (*prepare_fsdata)(const __fs_type_t *fstype, __fs_param_t *param);
    void (*cleanup_fsdata)(void *fsdata);

    bool need_block_size;
};

static void * __prepare_file_mal_fsdata(const __fs_type_t *fstype, __fs_param_t *param);
static void __cleanup_file_mal_fsdata(void *fsdata);
#if VSF_FS_USE_MEMFS == ENABLED
static void * __prepare_memfs_fsdata(const __fs_type_t *fstype, __fs_param_t *param)
{
    void * fsinfo;
    if (vsf_linux_fs_get_target(param->device, &fsinfo) < 0) {
        return NULL;
    }
    return fsinfo;
}

static void __cleanup_memfs_fsdata(void *fsdata)
{
}
#endif
#if VSF_FS_USE_LITTLEFS == ENABLED
static void * __prepare_lfs_fsdata(const __fs_type_t *fstype, __fs_param_t *param)
{
    vk_file_mal_t *file_mal = __prepare_file_mal_fsdata(fstype, param);
    vk_lfs_info_t *fsinfo = vsf_heap_calloc(1, sizeof(vk_lfs_info_t));
    if (NULL == fsinfo) {
        return NULL;
    }

    fsinfo->config.context = file_mal;
// maybe read is a macro from linux subsystem in vsf
#undef read
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
    return fsinfo;
}

static void __cleanup_lfs_fsdata(void *fsdata)
{
    vk_lfs_info_t *fsinfo = fsdata;
    if (fsinfo != NULL) {
        __cleanup_file_mal_fsdata(fsinfo->config.context);
        vsf_heap_free(fsdata);
    }
}
#endif
#if VSF_FS_USE_WINFS == ENABLED
static void * __prepare_winfs_fsdata(const __fs_type_t *fstype, __fs_param_t *param)
{
    vk_winfs_info_t *fsinfo = vsf_heap_calloc(1, sizeof(vk_winfs_info_t) + strlen(param->device) + 1);
    if (NULL == fsinfo) {
        return NULL;
    }

    strcpy((char *)&fsinfo[1], param->device);
    fsinfo->root.name = (char *)&fsinfo[1];
    return fsinfo;
}

static void __cleanup_winfs_fsdata(void *fsdata)
{
    vk_winfs_info_t *fsinfo = fsdata;
    if (fsinfo != NULL) {
        vsf_heap_free(fsinfo);
    }
}
#endif
#if VSF_FS_USE_LINFS == ENABLED
static void * __prepare_linfs_fsdata(const __fs_type_t *fstype, __fs_param_t *param)
{
    vk_linfs_info_t *fsinfo = vsf_heap_calloc(1, sizeof(vk_linfs_info_t) + strlen(param->device) + 1);
    if (NULL == fsinfo) {
        return NULL;
    }

    strcpy((char *)&fsinfo[1], param->device);
    fsinfo->root.name = (char *)&fsinfo[1];
    return fsinfo;
}

static void __cleanup_linfs_fsdata(void *fsdata)
{
    vk_linfs_info_t *fsinfo = fsdata;
    if (fsinfo != NULL) {
        vsf_heap_free(fsinfo);
    }
}
#endif

static const __fs_type_t __fs_types[] = {
    {
        .fs                 = "auto",
        .fsop               = NULL,
        .prepare_fsdata     = __prepare_file_mal_fsdata,
        .cleanup_fsdata     = __cleanup_file_mal_fsdata,
        .need_block_size    = true,
    },
#if VSF_FS_USE_MEMFS == ENABLED
    {
        .fs                 = "memfs",
        .fsop               = &vk_memfs_op,
        .prepare_fsdata     = __prepare_memfs_fsdata,
        .cleanup_fsdata     = __cleanup_memfs_fsdata,
        .need_block_size    = false,
    },
#endif
#if VSF_FS_USE_FATFS == ENABLED
    {
        .fs                 = "fatfs",
        .fsop               = NULL,
        .prepare_fsdata     = __prepare_file_mal_fsdata,
        .cleanup_fsdata     = __cleanup_file_mal_fsdata,
        .need_block_size    = true,
    },
#endif
#if VSF_FS_USE_LITTLEFS == ENABLED
    {
        .fs                 = "littlefs",
        .fsop               = &vk_lfs_op,
        .prepare_fsdata     = __prepare_lfs_fsdata,
        .cleanup_fsdata     = __cleanup_lfs_fsdata,
        .need_block_size    = true,
    },
#endif
#if VSF_FS_USE_WINFS == ENABLED
    {
        .fs                 = "winfs",
        .fsop               = &vk_winfs_op,
        .prepare_fsdata     = __prepare_winfs_fsdata,
        .cleanup_fsdata     = __cleanup_winfs_fsdata,
        .need_block_size    = false,
    },
#endif
#if VSF_FS_USE_LINFS == ENABLED
    {
        .fs                 = "linfs",
        .fsop               = &vk_linfs_op,
        .prepare_fsdata     = __prepare_linfs_fsdata,
        .cleanup_fsdata     = __cleanup_linfs_fsdata,
        .need_block_size    = false,
    },
#endif
};

static void * __prepare_file_mal_fsdata(const __fs_type_t *fstype, __fs_param_t *param)
{
    vk_file_mal_t *file_mal = vsf_heap_calloc(1, sizeof(*file_mal));
    if (NULL == file_mal) {
        printf("not enough resources\r\n");
        return NULL;
    }

    vk_file_open(NULL, param->device, &file_mal->file);
    if (NULL == file_mal->file) {
        printf("fail to open src_file %s\r\n", param->device);
        goto cleanup;
    }

    file_mal->drv = &vk_file_mal_drv;
    file_mal->block_size = param->block_size;
    vk_mal_init(&file_mal->use_as__vk_mal_t);

    return file_mal;
cleanup:
    __cleanup_file_mal_fsdata(file_mal);
    return NULL;
}

static void __cleanup_file_mal_fsdata(void *fsdata)
{
    vk_file_mal_t *file_mal = fsdata;
    if (file_mal != NULL) {
        vk_file_close(file_mal->file);
        vsf_heap_free(file_mal);
    }
}

int mount_main(int argc, char *argv[])
{
    const __fs_type_t *fs_type = &__fs_types[0];
    __fs_param_t param = { 0 };
    void *fsdata = NULL;

    char ch;
    while ((ch = getopt(argc, argv, "t:b:")) >= 0) {
        switch (ch) {
        case 't': {
                bool is_supported = false;
                for (int i = 0; i < dimof(__fs_types); i++) {
                    if (!strcmp(__fs_types[i].fs, optarg)) {
                        is_supported = true;
                        fs_type = &__fs_types[i];
                        break;
                    }
                }
                if (!is_supported) {
                print_help:
                    printf("format: %s [-t vfstype] [-b block_size] device dir\r\nsupported vfstype(mandatory options): ", argv[0]);
                    for (int i = 0; i < dimof(__fs_types); i++) {
                        if (__fs_types[i].need_block_size) {
                            printf("%s(b) ", __fs_types[i].fs);
                        } else {
                            printf("%s ", __fs_types[i].fs);
                        }
                    }
                    printf("\r\n");
                    return -1;
                }
            }
            break;
        case 'b':
            param.block_size = strtoul(optarg, NULL, 0);
            break;
        }
    }
    // last 2 parameters are device and dir
    if ((argc < 3) || (optind + 2 != argc)) {
        goto print_help;
    }
    if (fs_type->need_block_size && (0 == param.block_size)) {
        printf("block size must be defined for %s\r\n", fs_type->fs);
        goto print_help;
    }

    param.device = argv[optind + 0];
    fsdata = fs_type->prepare_fsdata(fs_type, &param);
    if (NULL == fsdata) {
        printf("fail to prepare fsdata\r\n");
        goto cleanup;
    }
    if (mount(NULL, argv[optind + 1], fs_type->fsop, 0, fsdata) != 0) {
        printf("fail to mount\r\n");
        goto cleanup;
    }
    return 0;

cleanup:
    if ((fsdata != NULL) && (fs_type->cleanup_fsdata != NULL)) {
        fs_type->cleanup_fsdata(fsdata);

    }
    return -1;
}
#endif
