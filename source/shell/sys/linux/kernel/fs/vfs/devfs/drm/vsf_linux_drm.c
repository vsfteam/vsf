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

#if     VSF_USE_UI == ENABLED && VSF_USE_LINUX == ENABLED                       \
    &&  VSF_LINUX_USE_DEVFS == ENABLED && VSF_LINUX_USE_DRM == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_FS_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__

#include <vsf.h>

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#   include "shell/sys/linux/include/errno.h"
#   include "shell/sys/linux/include/poll.h"
#   include "shell/sys/linux/include/sys/ioctl.h"
#   include "shell/sys/linux/include/sys/stat.h"
#   include "shell/sys/linux/include/sys/mman.h"
#   include "shell/sys/linux/include/drm/drm.h"
#   include "shell/sys/linux/include/drm/drm_mode.h"
#else
#   include <unistd.h>
#   include <errno.h>
#   include <poll.h>
#   include <sys/ioctl.h>
#   include <sys/stat.h>
#   include <sys/mman.h>
#   include <drm/drm.h>
#   include <drm/drm_mode.h>
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_LINUX_DRM_CFG_MAX_FB
#   define VSF_LINUX_DRM_CFG_MAX_FB         4
#endif

#ifndef VSF_LINUX_DRM_CFG_MAX_CRTC
#   define VSF_LINUX_DRM_CFG_MAX_CRTC       1
#endif

#ifndef VSF_LINUX_DRM_CFG_MAX_CONNECTOR
#   define VSF_LINUX_DRM_CFG_MAX_CONNECTOR  1
#endif

#ifndef VSF_LINUX_DRM_CFG_MAX_ENCODER
#   define VSF_LINUX_DRM_CFG_MAX_ENCODER    1
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// DRM internal framebuffer structure
typedef struct vsf_linux_drm_fb {
    uint32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t depth;
    uint32_t handle;
    void *buffer;
    size_t size;
    bool in_use;
} vsf_linux_drm_fb_t;

// DRM internal dumb buffer structure
typedef struct vsf_linux_drm_dumb {
    uint32_t handle;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t pitch;
    uint64_t size;
    void *buffer;
    bool in_use;
} vsf_linux_drm_dumb_t;

// DRM private structure
typedef struct vsf_linux_drm_priv {
    implement(vsf_linux_fb_priv_t)

    // Framebuffers
    vsf_linux_drm_fb_t fbs[VSF_LINUX_DRM_CFG_MAX_FB];
    uint32_t fb_count;

    // Dumb buffers
    vsf_linux_drm_dumb_t dumbs[VSF_LINUX_DRM_CFG_MAX_FB];
    uint32_t dumb_count;

    // ID counters
    uint32_t next_fb_id;
    uint32_t next_handle;

    // Display info
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t pitch;

    // CRTC state
    uint32_t crtc_fb_id;
    uint32_t crtc_x;
    uint32_t crtc_y;

#define crtc_enabled    user_bits0
#define is_master       user_bits1
} vsf_linux_drm_priv_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern int __vsf_linux_default_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
extern void __vsf_linux_disp_on_ready(vk_disp_t *disp);
extern void __vsf_linux_disp_fresh_task(vsf_eda_t *eda, vsf_evt_t evt);

/*============================ LOCAL VARIABLES ===============================*/

static const char __vsf_linux_drm_name[] = "vsf_drm";
static const char __vsf_linux_drm_date[] = "20250101";
static const char __vsf_linux_drm_desc[] = "VSF DRM Driver";

/*============================ IMPLEMENTATION ================================*/

static uint32_t __vsf_linux_drm_color_to_fourcc(vk_disp_color_type_t color)
{
    switch (color) {
    case VSF_DISP_COLOR_RGB565:
        return DRM_FORMAT_RGB565;
    case VSF_DISP_COLOR_RGB888:
    case VSF_DISP_COLOR_RGB888_24:
        return DRM_FORMAT_RGB888;
    case VSF_DISP_COLOR_ARGB8888:
        return DRM_FORMAT_ARGB8888;
    case VSF_DISP_COLOR_RGB888_32:
        return DRM_FORMAT_XRGB8888;
    case VSF_DISP_COLOR_RGBA8888:
        return DRM_FORMAT_RGBA8888;
    case VSF_DISP_COLOR_BGR888_24:
        return DRM_FORMAT_BGR888;
    case VSF_DISP_COLOR_ABGR8888:
        return DRM_FORMAT_ABGR8888;
    case VSF_DISP_COLOR_BGRA8888:
        return DRM_FORMAT_BGRA8888;
    default:
        return DRM_FORMAT_XRGB8888;
    }
}

static uint32_t __vsf_linux_drm_fourcc_to_bpp(uint32_t fourcc)
{
    switch (fourcc) {
    case DRM_FORMAT_RGB565:
    case DRM_FORMAT_BGR565:
        return 16;
    case DRM_FORMAT_RGB888:
    case DRM_FORMAT_BGR888:
        return 24;
    case DRM_FORMAT_XRGB8888:
    case DRM_FORMAT_ARGB8888:
    case DRM_FORMAT_RGBA8888:
    case DRM_FORMAT_XBGR8888:
    case DRM_FORMAT_ABGR8888:
    case DRM_FORMAT_BGRA8888:
        return 32;
    default:
        return 32;
    }
}

static uint32_t __vsf_linux_drm_fourcc_to_depth(uint32_t fourcc)
{
    switch (fourcc) {
    case DRM_FORMAT_RGB565:
    case DRM_FORMAT_BGR565:
        return 16;
    case DRM_FORMAT_RGB888:
    case DRM_FORMAT_BGR888:
    case DRM_FORMAT_XRGB8888:
    case DRM_FORMAT_XBGR8888:
        return 24;
    case DRM_FORMAT_ARGB8888:
    case DRM_FORMAT_RGBA8888:
    case DRM_FORMAT_ABGR8888:
    case DRM_FORMAT_BGRA8888:
        return 32;
    default:
        return 24;
    }
}

static vsf_linux_drm_fb_t * __vsf_linux_drm_find_fb(vsf_linux_drm_priv_t *priv, uint32_t fb_id)
{
    for (int i = 0; i < VSF_LINUX_DRM_CFG_MAX_FB; i++) {
        if (priv->fbs[i].in_use && priv->fbs[i].id == fb_id) {
            return &priv->fbs[i];
        }
    }
    return NULL;
}

static vsf_linux_drm_dumb_t * __vsf_linux_drm_find_dumb(vsf_linux_drm_priv_t *priv, uint32_t handle)
{
    for (int i = 0; i < VSF_LINUX_DRM_CFG_MAX_FB; i++) {
        if (priv->dumbs[i].in_use && priv->dumbs[i].handle == handle) {
            return &priv->dumbs[i];
        }
    }
    return NULL;
}

static vsf_linux_drm_fb_t * __vsf_linux_drm_alloc_fb(vsf_linux_drm_priv_t *priv)
{
    for (int i = 0; i < VSF_LINUX_DRM_CFG_MAX_FB; i++) {
        if (!priv->fbs[i].in_use) {
            priv->fbs[i].in_use = true;
            priv->fbs[i].id = ++priv->next_fb_id;
            return &priv->fbs[i];
        }
    }
    return NULL;
}

static vsf_linux_drm_dumb_t * __vsf_linux_drm_alloc_dumb(vsf_linux_drm_priv_t *priv)
{
    for (int i = 0; i < VSF_LINUX_DRM_CFG_MAX_FB; i++) {
        if (!priv->dumbs[i].in_use) {
            priv->dumbs[i].in_use = true;
            priv->dumbs[i].handle = ++priv->next_handle;
            return &priv->dumbs[i];
        }
    }
    return NULL;
}

static void __vsf_linux_drm_free_fb(vsf_linux_drm_priv_t *priv, vsf_linux_drm_fb_t *fb)
{
    if (fb->buffer != NULL) {
        free(fb->buffer);
    }
    memset(fb, 0, sizeof(*fb));
}

static void __vsf_linux_drm_free_dumb(vsf_linux_drm_priv_t *priv, vsf_linux_drm_dumb_t *dumb)
{
    if (dumb->buffer != NULL) {
        free(dumb->buffer);
    }
    memset(dumb, 0, sizeof(*dumb));
}

static void __vsf_linux_drm_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_drm_priv_t *priv = (vsf_linux_drm_priv_t *)sfd->priv;
    vk_disp_t *disp = (vk_disp_t *)(((vk_vfs_file_t *)(priv->file))->f.param);

    priv->width = disp->param.width;
    priv->height = disp->param.height;
    priv->bpp = vsf_disp_get_pixel_bitsize(disp);
    priv->pitch = priv->width * vsf_disp_get_pixel_bytesize(disp);
    priv->frame_interval_ms = 1000 / 60;
    
    priv->next_fb_id = 0;
    priv->next_handle = 0;
    priv->fb_count = 0;
    priv->dumb_count = 0;
    
    priv->crtc_fb_id = 0;
    priv->crtc_x = 0;
    priv->crtc_y = 0;
    priv->crtc_enabled = false;
    priv->is_master = false;
    
    priv->front_buffer = NULL;
    priv->is_area_set = false;
    priv->is_refreshing = false;
    priv->eda_pending = NULL;
    
    /* Initialize triggers and task */
    vsf_eda_trig_init(&priv->fresh_trigger, false, true);
    priv->fresh_task.fn.evthandler = __vsf_linux_disp_fresh_task;
    priv->fresh_task.priority = vsf_prio_0;
    
    /* Step 1: Start display initialization, ui_data points to current task for init complete notification */
    disp->ui_on_ready = __vsf_linux_disp_on_ready;
    disp->ui_data = vsf_eda_get_cur();
    vk_disp_init(disp);
    
    /* Step 2: Wait for display initialization complete (first ui_on_ready callback) */
    vsf_thread_wfe(VSF_EVT_USER);
    
    /* Step 3: Start refresh task and switch ui_data to it */
    vsf_teda_init(&priv->fresh_task);
    disp->ui_data = &priv->fresh_task;
}

static int __vsf_linux_drm_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    vsf_linux_drm_priv_t *priv = (vsf_linux_drm_priv_t *)sfd->priv;
    vk_disp_t *disp = (vk_disp_t *)(((vk_vfs_file_t *)(priv->file))->f.param);
    
    switch (cmd) {
    case DRM_IOCTL_VERSION: {
            struct drm_version *version = (struct drm_version *)arg;
            version->version_major = 1;
            version->version_minor = 4;
            version->version_patchlevel = 0;
            version->name_len = sizeof(__vsf_linux_drm_name);
            if (version->name != NULL) {
                strncpy(version->name, __vsf_linux_drm_name, version->name_len);
            }
            version->date_len = sizeof(__vsf_linux_drm_date);
            if (version->date != NULL) {
                strncpy(version->date, __vsf_linux_drm_date, version->date_len);
            }
            version->desc_len = sizeof(__vsf_linux_drm_desc);
            if (version->desc != NULL) {
                strncpy(version->desc, __vsf_linux_drm_desc, version->desc_len);
            }
        }
        break;
        
    case DRM_IOCTL_GET_CAP: {
            struct drm_get_cap *cap = (struct drm_get_cap *)arg;
            
            /* Validate capability ID */
            VSF_LINUX_ASSERT(cap->capability <= DRM_CAP_SYNCOBJ_TIMELINE);
            
            switch (cap->capability) {
            case DRM_CAP_DUMB_BUFFER:
                cap->value = 1;
                break;
            case DRM_CAP_DUMB_PREFERRED_DEPTH:
                cap->value = priv->bpp;
                break;
            case DRM_CAP_DUMB_PREFER_SHADOW:
                cap->value = 0;
                break;
            case DRM_CAP_PRIME:
                cap->value = 0;
                break;
            case DRM_CAP_TIMESTAMP_MONOTONIC:
                cap->value = 0;
                break;
            case DRM_CAP_ASYNC_PAGE_FLIP:
                cap->value = 0;
                break;
            case DRM_CAP_CURSOR_WIDTH:
                cap->value = 64;
                break;
            case DRM_CAP_CURSOR_HEIGHT:
                cap->value = 64;
                break;
            case DRM_CAP_ADDFB2_MODIFIERS:
                cap->value = 0;
                break;
            default:
                cap->value = 0;
                break;
            }
        }
        break;
        
    case DRM_IOCTL_SET_MASTER:
        priv->is_master = true;
        break;
        
    case DRM_IOCTL_DROP_MASTER:
        priv->is_master = false;
        break;
        
    case DRM_IOCTL_MODE_GETRESOURCES: {
            struct drm_mode_card_res *res = (struct drm_mode_card_res *)arg;
            res->min_width = priv->width;
            res->min_height = priv->height;
            res->max_width = priv->width;
            res->max_height = priv->height;
            
            if (res->fb_id_ptr != 0) {
                uint32_t *fb_ids = (uint32_t *)(uintptr_t)res->fb_id_ptr;
                uint32_t count = 0;
                for (int i = 0; i < VSF_LINUX_DRM_CFG_MAX_FB && count < res->count_fbs; i++) {
                    if (priv->fbs[i].in_use) {
                        fb_ids[count++] = priv->fbs[i].id;
                    }
                }
            }
            res->count_fbs = priv->fb_count;
            
            if (res->crtc_id_ptr != 0) {
                uint32_t *crtc_ids = (uint32_t *)(uintptr_t)res->crtc_id_ptr;
                if (res->count_crtcs > 0) {
                    crtc_ids[0] = 1; // CRTC ID 1
                }
            }
            res->count_crtcs = VSF_LINUX_DRM_CFG_MAX_CRTC;
            
            if (res->connector_id_ptr != 0) {
                uint32_t *connector_ids = (uint32_t *)(uintptr_t)res->connector_id_ptr;
                if (res->count_connectors > 0) {
                    connector_ids[0] = 1; // Connector ID 1
                }
            }
            res->count_connectors = VSF_LINUX_DRM_CFG_MAX_CONNECTOR;
            
            if (res->encoder_id_ptr != 0) {
                uint32_t *encoder_ids = (uint32_t *)(uintptr_t)res->encoder_id_ptr;
                if (res->count_encoders > 0) {
                    encoder_ids[0] = 1; // Encoder ID 1
                }
            }
            res->count_encoders = VSF_LINUX_DRM_CFG_MAX_ENCODER;
        }
        break;
        
    case DRM_IOCTL_MODE_GETCRTC: {
            struct drm_mode_crtc *crtc = (struct drm_mode_crtc *)arg;
            
            /* Validate CRTC ID - only CRTC 1 is supported */
            VSF_LINUX_ASSERT(crtc->crtc_id == 1);
            if (crtc->crtc_id != 1) {
                return -EINVAL;
            }
            
            crtc->x = priv->crtc_x;
            crtc->y = priv->crtc_y;
            crtc->fb_id = priv->crtc_fb_id;
            crtc->gamma_size = 0;
            crtc->count_connectors = priv->crtc_enabled ? 1 : 0;
            if (priv->crtc_enabled && priv->crtc_fb_id != 0) {
                vsf_linux_drm_fb_t *fb = __vsf_linux_drm_find_fb(priv, priv->crtc_fb_id);
                if (fb != NULL) {
                    crtc->mode_valid = 1;
                    crtc->mode.clock = 60 * 1000;
                    crtc->mode.hdisplay = fb->width;
                    crtc->mode.hsync_start = fb->width;
                    crtc->mode.hsync_end = fb->width;
                    crtc->mode.htotal = fb->width;
                    crtc->mode.vdisplay = fb->height;
                    crtc->mode.vsync_start = fb->height;
                    crtc->mode.vsync_end = fb->height;
                    crtc->mode.vtotal = fb->height;
                    crtc->mode.vrefresh = 60;
                    crtc->mode.flags = 0;
                    crtc->mode.type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
                    strncpy(crtc->mode.name, "VSF-MODE", sizeof(crtc->mode.name));
                } else {
                    crtc->mode_valid = 0;
                }
            } else {
                crtc->mode_valid = 0;
            }
        }
        break;
        
    case DRM_IOCTL_MODE_SETCRTC: {
            struct drm_mode_crtc *crtc = (struct drm_mode_crtc *)arg;
            
            /* Validate CRTC ID - only CRTC 1 is supported */
            VSF_LINUX_ASSERT(crtc->crtc_id == 1);
            if (crtc->crtc_id != 1) {
                return -EINVAL;
            }
            
            /* Validate connector count - only single connector is supported */
            VSF_LINUX_ASSERT(crtc->count_connectors == 1);
            if (crtc->count_connectors != 1) {
                return -EINVAL;
            }
            
            /* Validate connector pointer */
            VSF_LINUX_ASSERT(crtc->set_connectors_ptr != 0);
            if (crtc->set_connectors_ptr == 0) {
                return -EINVAL;
            }
            
            /* Validate connector ID - only connector 1 is supported */
            uint32_t *connector_ids = (uint32_t *)(uintptr_t)crtc->set_connectors_ptr;
            VSF_LINUX_ASSERT(connector_ids[0] == 1);
            if (connector_ids[0] != 1) {
                return -EINVAL;
            }
            
            /* Validate mode if provided */
            if (crtc->mode_valid) {
                VSF_LINUX_ASSERT(crtc->mode.hdisplay == priv->width);
                VSF_LINUX_ASSERT(crtc->mode.vdisplay == priv->height);
            }
            
            priv->crtc_fb_id = crtc->fb_id;
            priv->crtc_x = crtc->x;
            priv->crtc_y = crtc->y;
            priv->crtc_enabled = (crtc->fb_id != 0);
            
            // Update display if fb is valid
            if (crtc->fb_id != 0) {
                vsf_linux_drm_fb_t *fb = __vsf_linux_drm_find_fb(priv, crtc->fb_id);
                if (fb != NULL && fb->buffer != NULL) {
                    priv->front_buffer = fb->buffer;
                    vsf_eda_trig_set(&priv->fresh_trigger);
                }
            }
        }
        break;
        
    case DRM_IOCTL_MODE_GETENCODER: {
            struct drm_mode_get_encoder *encoder = (struct drm_mode_get_encoder *)arg;
            
            /* Validate encoder ID - only encoder 1 is supported */
            VSF_LINUX_ASSERT(encoder->encoder_id == 1);
            if (encoder->encoder_id != 1) {
                return -EINVAL;
            }
            
            encoder->encoder_type = DRM_MODE_ENCODER_VIRTUAL;
            encoder->crtc_id = 1;
            encoder->possible_crtcs = 1;
            encoder->possible_clones = 0;
        }
        break;
        
    case DRM_IOCTL_MODE_GETCONNECTOR: {
            struct drm_mode_get_connector *connector = (struct drm_mode_get_connector *)arg;
            
            /* Validate connector ID - only connector 1 is supported */
            VSF_LINUX_ASSERT(connector->connector_id == 1);
            if (connector->connector_id != 1) {
                return -EINVAL;
            }
            
            connector->encoder_id = 1;
            connector->connector_type = DRM_MODE_CONNECTOR_VIRTUAL;
            connector->connector_type_id = 1;
            connector->connection = DRM_MODE_CONNECTED;
            connector->mm_width = priv->width * 254 / 960;  // ~96 DPI
            connector->mm_height = priv->height * 254 / 960;
            connector->subpixel = DRM_MODE_SUBPIXEL_UNKNOWN;
            
            // If modes_ptr is provided, fill in one mode
            if (connector->modes_ptr != 0 && connector->count_modes > 0) {
                struct drm_mode_modeinfo *mode = (struct drm_mode_modeinfo *)(uintptr_t)connector->modes_ptr;
                mode->clock = 60 * 1000;
                mode->hdisplay = priv->width;
                mode->hsync_start = priv->width;
                mode->hsync_end = priv->width;
                mode->htotal = priv->width;
                mode->vdisplay = priv->height;
                mode->vsync_start = priv->height;
                mode->vsync_end = priv->height;
                mode->vtotal = priv->height;
                mode->vrefresh = 60;
                mode->flags = 0;
                mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
                strncpy(mode->name, "VSF-MODE", sizeof(mode->name));
            }
            connector->count_modes = 1;
            
            if (connector->encoders_ptr != 0 && connector->count_encoders > 0) {
                uint32_t *encoders = (uint32_t *)(uintptr_t)connector->encoders_ptr;
                encoders[0] = 1;
            }
            connector->count_encoders = 1;
            
            connector->count_props = 0;
        }
        break;
        
    case DRM_IOCTL_MODE_GETFB: {
            struct drm_mode_fb_cmd *fb_cmd = (struct drm_mode_fb_cmd *)arg;
            vsf_linux_drm_fb_t *fb = __vsf_linux_drm_find_fb(priv, fb_cmd->fb_id);
            if (fb != NULL) {
                fb_cmd->width = fb->width;
                fb_cmd->height = fb->height;
                fb_cmd->pitch = fb->pitch;
                fb_cmd->bpp = fb->bpp;
                fb_cmd->depth = fb->depth;
                fb_cmd->handle = fb->handle;
            } else {
                return -ENOENT;
            }
        }
        break;
        
    case DRM_IOCTL_MODE_ADDFB: {
            struct drm_mode_fb_cmd *fb_cmd = (struct drm_mode_fb_cmd *)arg;
            vsf_linux_drm_dumb_t *dumb = __vsf_linux_drm_find_dumb(priv, fb_cmd->handle);
            if (dumb == NULL) {
                return -ENOENT;
            }
            
            vsf_linux_drm_fb_t *fb = __vsf_linux_drm_alloc_fb(priv);
            if (fb == NULL) {
                return -ENOMEM;
            }
            
            fb->width = fb_cmd->width;
            fb->height = fb_cmd->height;
            fb->pitch = fb_cmd->pitch;
            fb->bpp = fb_cmd->bpp;
            fb->depth = fb_cmd->depth;
            fb->handle = fb_cmd->handle;
            fb->buffer = dumb->buffer;
            
            fb_cmd->fb_id = fb->id;
            priv->fb_count++;
        }
        break;
        
    case DRM_IOCTL_MODE_ADDFB2: {
            struct drm_mode_fb_cmd2 *fb_cmd2 = (struct drm_mode_fb_cmd2 *)arg;
            
            /* Validate parameters */
            VSF_LINUX_ASSERT(fb_cmd2->width > 0);
            VSF_LINUX_ASSERT(fb_cmd2->height > 0);
            VSF_LINUX_ASSERT(fb_cmd2->handles[0] != 0);
            VSF_LINUX_ASSERT(fb_cmd2->pitches[0] > 0);
            
            if (fb_cmd2->width == 0 || fb_cmd2->height == 0) {
                return -EINVAL;
            }
            if (fb_cmd2->handles[0] == 0) {
                return -EINVAL;
            }
            if (fb_cmd2->pitches[0] == 0) {
                return -EINVAL;
            }
            
            /* Validate flags - no modifier support */
            VSF_LINUX_ASSERT((fb_cmd2->flags & ~DRM_MODE_FB_MODIFIERS) == 0);
            if ((fb_cmd2->flags & ~DRM_MODE_FB_MODIFIERS) != 0) {
                return -EINVAL;
            }
            
            vsf_linux_drm_dumb_t *dumb = __vsf_linux_drm_find_dumb(priv, fb_cmd2->handles[0]);
            if (dumb == NULL) {
                return -ENOENT;
            }
            
            vsf_linux_drm_fb_t *fb = __vsf_linux_drm_alloc_fb(priv);
            if (fb == NULL) {
                return -ENOMEM;
            }
            
            fb->width = fb_cmd2->width;
            fb->height = fb_cmd2->height;
            fb->pitch = fb_cmd2->pitches[0];
            fb->bpp = __vsf_linux_drm_fourcc_to_bpp(fb_cmd2->pixel_format);
            fb->depth = __vsf_linux_drm_fourcc_to_depth(fb_cmd2->pixel_format);
            fb->handle = fb_cmd2->handles[0];
            fb->buffer = dumb->buffer;
            
            fb_cmd2->fb_id = fb->id;
            priv->fb_count++;
        }
        break;
        
    case DRM_IOCTL_MODE_RMFB: {
            uint32_t *fb_id = (uint32_t *)arg;
            
            /* Validate framebuffer ID */
            VSF_LINUX_ASSERT(*fb_id != 0);
            if (*fb_id == 0) {
                return -EINVAL;
            }
            
            vsf_linux_drm_fb_t *fb = __vsf_linux_drm_find_fb(priv, *fb_id);
            if (fb != NULL) {
                __vsf_linux_drm_free_fb(priv, fb);
                priv->fb_count--;
            } else {
                return -ENOENT;
            }
        }
        break;
        
    case DRM_IOCTL_MODE_CREATE_DUMB: {
            struct drm_mode_create_dumb *create_dumb = (struct drm_mode_create_dumb *)arg;
            
            /* Validate parameters */
            VSF_LINUX_ASSERT(create_dumb->width > 0);
            VSF_LINUX_ASSERT(create_dumb->height > 0);
            VSF_LINUX_ASSERT(create_dumb->bpp == 16 || create_dumb->bpp == 24 || create_dumb->bpp == 32);
            
            if (create_dumb->width == 0 || create_dumb->height == 0) {
                return -EINVAL;
            }
            if (create_dumb->bpp != 16 && create_dumb->bpp != 24 && create_dumb->bpp != 32) {
                return -EINVAL;
            }
            
            vsf_linux_drm_dumb_t *dumb = __vsf_linux_drm_alloc_dumb(priv);
            if (dumb == NULL) {
                return -ENOMEM;
            }
            
            uint32_t bpp = create_dumb->bpp;
            uint32_t width = create_dumb->width;
            uint32_t height = create_dumb->height;
            uint32_t pitch = ((width * bpp + 31) / 32) * 4; // 32-bit aligned
            uint64_t size = (uint64_t)pitch * height;
            
            dumb->width = width;
            dumb->height = height;
            dumb->bpp = bpp;
            dumb->pitch = pitch;
            dumb->size = size;
            dumb->buffer = malloc(size);
            
            if (dumb->buffer == NULL) {
                __vsf_linux_drm_free_dumb(priv, dumb);
                return -ENOMEM;
            }
            memset(dumb->buffer, 0, size);
            
            create_dumb->handle = dumb->handle;
            create_dumb->pitch = pitch;
            create_dumb->size = size;
            priv->dumb_count++;
        }
        break;
        
    case DRM_IOCTL_MODE_MAP_DUMB: {
            struct drm_mode_map_dumb *map_dumb = (struct drm_mode_map_dumb *)arg;
            
            /* Validate handle */
            VSF_LINUX_ASSERT(map_dumb->handle != 0);
            if (map_dumb->handle == 0) {
                return -EINVAL;
            }
            
            vsf_linux_drm_dumb_t *dumb = __vsf_linux_drm_find_dumb(priv, map_dumb->handle);
            if (dumb == NULL) {
                return -ENOENT;
            }
            // Return fake offset (handle as offset for mmap)
            map_dumb->offset = (uint64_t)(uintptr_t)dumb->buffer;
        }
        break;
        
    case DRM_IOCTL_MODE_DESTROY_DUMB: {
            struct drm_mode_destroy_dumb *destroy_dumb = (struct drm_mode_destroy_dumb *)arg;
            
            /* Validate handle */
            VSF_LINUX_ASSERT(destroy_dumb->handle != 0);
            if (destroy_dumb->handle == 0) {
                return -EINVAL;
            }
            
            vsf_linux_drm_dumb_t *dumb = __vsf_linux_drm_find_dumb(priv, destroy_dumb->handle);
            if (dumb != NULL) {
                __vsf_linux_drm_free_dumb(priv, dumb);
                priv->dumb_count--;
            } else {
                return -ENOENT;
            }
        }
        break;
        
    case DRM_IOCTL_MODE_PAGE_FLIP: {
            struct drm_mode_crtc_page_flip *page_flip = (struct drm_mode_crtc_page_flip *)arg;
            
            /* Validate CRTC ID - only CRTC 1 is supported */
            VSF_LINUX_ASSERT(page_flip->crtc_id == 1);
            if (page_flip->crtc_id != 1) {
                return -EINVAL;
            }
            
            /* Validate page flip flags - only EVENT flag is supported */
            VSF_LINUX_ASSERT((page_flip->flags & ~DRM_MODE_PAGE_FLIP_EVENT) == 0);
            if ((page_flip->flags & ~DRM_MODE_PAGE_FLIP_EVENT) != 0) {
                return -EINVAL;
            }
            
            vsf_linux_drm_fb_t *fb = __vsf_linux_drm_find_fb(priv, page_flip->fb_id);
            if (fb != NULL && fb->buffer != NULL) {
                priv->crtc_fb_id = page_flip->fb_id;
                priv->front_buffer = fb->buffer;
                priv->crtc_enabled = true;
                vsf_eda_trig_set(&priv->fresh_trigger);
            } else {
                return -ENOENT;
            }
        }
        break;
        
    default:
        return __vsf_linux_default_fcntl(sfd, cmd, arg);
    }
    return 0;
}

static int __vsf_linux_drm_stat(vsf_linux_fd_t *sfd, struct stat *buf)
{
    buf->st_mode = S_IFCHR | 0666;
    return 0;
}

static int __vsf_linux_drm_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_drm_priv_t *priv = (vsf_linux_drm_priv_t *)sfd->priv;
    
    // Stop refresh task
    vsf_eda_fini(&priv->fresh_task.use_as__vsf_eda_t);
    
    // Free all framebuffers
    for (int i = 0; i < VSF_LINUX_DRM_CFG_MAX_FB; i++) {
        if (priv->fbs[i].in_use) {
            __vsf_linux_drm_free_fb(priv, &priv->fbs[i]);
        }
    }
    
    // Free all dumb buffers
    for (int i = 0; i < VSF_LINUX_DRM_CFG_MAX_FB; i++) {
        if (priv->dumbs[i].in_use) {
            __vsf_linux_drm_free_dumb(priv, &priv->dumbs[i]);
        }
    }
    
    return 0;
}

static void * __vsf_linux_drm_mmap(vsf_linux_fd_t *sfd, off64_t offset, size_t len, uint_fast32_t feature)
{
    vsf_linux_drm_priv_t *priv = (vsf_linux_drm_priv_t *)sfd->priv;
    
    // offset is actually the buffer pointer passed from DRM_IOCTL_MODE_MAP_DUMB
    if (offset != 0) {
        return (void *)(uintptr_t)offset;
    }
    
    return MAP_FAILED;
}

static int __vsf_linux_drm_msync(vsf_linux_fd_t *sfd, void *buffer)
{
    vsf_linux_drm_priv_t *priv = (vsf_linux_drm_priv_t *)sfd->priv;
    
    // Trigger display refresh if front buffer is this buffer
    if (priv->crtc_enabled && buffer == priv->front_buffer) {
        vsf_eda_trig_set(&priv->fresh_trigger);
    }
    
    return 0;
}

static const vsf_linux_fd_op_t __vsf_linux_drm_fdop = {
    .priv_size          = sizeof(vsf_linux_drm_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_init            = __vsf_linux_drm_init,
    .fn_fcntl           = __vsf_linux_drm_fcntl,
    .fn_stat            = __vsf_linux_drm_stat,
    .fn_close           = __vsf_linux_drm_close,
    .fn_mmap            = __vsf_linux_drm_mmap,
    .fn_msync           = __vsf_linux_drm_msync,
};

int vsf_linux_fs_bind_drm(char *path, vk_disp_t *disp)
{
    return vsf_linux_fs_bind_target_ex(
                path, disp, &__vsf_linux_drm_fdop, NULL, NULL,
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_DRM
