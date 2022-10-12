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
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#else
#   include <unistd.h>
#endif

#ifndef __kernel_off_t
#   define __kernel_off_t           off_t
#endif
#ifndef __kernel_pid_t
#   define __kernel_pid_t           pid_t
#endif
#ifndef __bitwise
#   define __bitwise
#endif

#include <alsa/sound/type_compat.h>
#include <alsa/sound/uapi/asound.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_audio_control_priv_t {
    implement(vsf_linux_fs_priv_t)
} vsf_linux_audio_control_priv_t;

typedef struct vsf_linux_audio_play_priv_t {
    implement(vsf_linux_fs_priv_t)
} vsf_linux_audio_play_priv_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static int __vsf_linux_audio_control_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_linux_fd_op_t __vsf_linux_audio_control_fdop = {
    .priv_size          = sizeof(vsf_linux_audio_control_priv_t),
    .fn_fcntl           = __vsf_linux_audio_control_fcntl,
};

static const vsf_linux_fd_op_t __vsf_linux_audio_play_fdop = {
    .priv_size          = sizeof(vsf_linux_audio_play_priv_t),
};

/*============================ IMPLEMENTATION ================================*/

static int __vsf_linux_audio_control_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    vsf_linux_audio_control_priv_t *priv = (vsf_linux_audio_control_priv_t *)sfd->priv;
    vk_audio_dev_t *audio_dev = (vk_audio_dev_t *)(((vk_vfs_file_t *)(priv->file))->f.param);
    char *filename = ((vk_vfs_file_t *)(priv->file))->name;
    VSF_LINUX_ASSERT(!strncmp(filename, "controlC", 8));
    int card_idx = atoi(filename + 8);

    union {
        struct snd_ctl_card_info *card_info;
        uintptr_t arg;
    } u;
    u.arg = arg;

    switch (cmd) {
    case SNDRV_CTL_IOCTL_CARD_INFO:
        u.card_info->card = card_idx;
        break;
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

    snprintf(&realpath[pathlen], leftlen, "pcmC%dD0p", card_idx);
    result = vsf_linux_fs_bind_target_ex(realpath, audio_dev, &__vsf_linux_audio_play_fdop,
                NULL, NULL, 0, 0);
    if (result < 0) {
        vsf_trace_error("fail to bind %s\n", realpath);
        return result;
    }

    return 0;
}

#endif
