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

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_DL1X5 == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_DL1X5 == ENABLED
#   define __VSF_DISP_CLASS_INHERIT__
#   define __VSF_DL1X5_USE_DISP
#endif

#include "../../vsf_usbh.h"
#include "./vsf_usbh_dl1x5.h"

// for vsf_trace
#include "service/vsf_service.h"
#include "kernel/vsf_kernel.h"

#ifdef __VSF_DL1X5_USE_DISP
// for vsf_disp
#   include "component/vsf_component.h"
#endif

/*============================ MACROS ========================================*/

#define DL1X5_PIXELS_IN_CMD                             256

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

#ifndef VSF_USBH_DL1X5_CFG_CLS
#   define VSF_USBH_DL1X5_CFG_CLS                       ENABLED
#endif
#ifndef VSF_USBH_DL1X5_CFG_READ_EDID
#   define VSF_USBH_DL1X5_CFG_READ_EDID                 ENABLED
#endif

#ifndef VSF_USBH_DL1X5_CFG_CMD_BUFFER_SIZE
//  use minimal buffer size for single maximal write_raw16 command
#   define VSF_USBH_DL1X5_CFG_CMD_BUFFER_SIZE           (6 + 2 * DL1X5_PIXELS_IN_CMD)
#endif

#if VSF_USBH_DL1X5_CFG_CMD_BUFFER_SIZE < 146
#   error VSF_USBH_DL1X5_CFG_CMD_BUFFER_SIZE MUST be larger than max channel seq
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum dl1x5_cmd_t {
    DL1X5_CMD_WRITE_VREG        = 0x20,
    DL1X5_CMD_WRITE_RAW8        = 0x60,
    DL1X5_CMD_WRITE_RL8         = 0x61,
    DL1X5_CMD_COPY8             = 0x62,
    DL1X5_CMD_WRITE_RAW16       = 0x68,
    DL1X5_CMD_WRITE_RL16        = 0x69,
    DL1X5_CMD_COPY16            = 0x6A,
    DL1X5_CMD_NULL              = 0xA0,
} dl1x5_cmd_t;

typedef enum usb_dl1x5_req_t {
    USB_DL1X5_REQ_READ_EDID     = 0x02,
    USB_DL1X5_REQ_GET_STATUS    = 0x06,
    USB_DL1X5_REQ_SET_CHANNEL   = 0x12,
} usb_dl1x5_req_t;

typedef struct vk_usbh_dl1x5_t {
    vk_usbh_t *usbh;
    vk_usbh_dev_t *dev;
    vk_usbh_ifs_t *ifs;
    vk_usbh_urb_t urb_out;

    enum {
        DL1X5_STATE_IDLE,
        DL1X5_STATE_GET_STATUS,
        DL1X5_STATE_READ_EDID,
        DL1X5_STATE_SET_STD_CHANNEL,
        DL1X5_STATE_SET_MODE,
        DL1X5_STATE_CLS,
        DL1X5_STATE_CLS_LAST,
        DL1X5_STATE_RUNNING,
    } state;
    bool is_busy;
#if VSF_USBH_DL1X5_CFG_CLS == ENABLED
    uint32_t pixel_addr;
#endif

    vga_timing_t timing;
    uint32_t cmd_pos;
    // add 2 more bytes for DL1X5_CMD_NULL
    uint8_t cmd_buf[VSF_USBH_DL1X5_CFG_CMD_BUFFER_SIZE + 2];

    vk_usbh_dl1x5_callback_t callback;
#if VSF_USBH_DL1X5_CFG_READ_EDID == ENABLED
    vsf_teda_t task;
#else
    vsf_eda_t task;
#endif

#ifdef __VSF_DL1X5_USE_DISP
    implement(vk_disp_t)
    vk_disp_area_t disp_area;
    uint16_t *disp_buff_rgb565;
    uint32_t disp_pixel_pos;
#endif
} vk_usbh_dl1x5_t;

/*============================ PROTOTYPES ====================================*/

static void *__vk_usbh_dl1x5_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_dl1x5_disconnect(vk_usbh_t *usbh,
        vk_usbh_dev_t *dev, void *param);

#if VSF_USBH_DL1X5_CFG_CLS == ENABLED
static bool __vk_dl1x5_cls_callback(void *dev);
#endif

extern void vsf_dl1x5_on_new_dev(void *dev, edid_t *edid);
extern vsf_err_t vk_usbh_dl1x5_commit(void *dev);

#ifdef __VSF_DL1X5_USE_DISP
extern void vsf_dl1x5_on_new_disp(vk_disp_t *disp);
void vk_usbh_dl1x5_set_callback(void *dev, vk_usbh_dl1x5_callback_t callback);
static void __vk_disp_dl1x5_refresh_callback(void *dev, vsf_err_t err);
static vsf_err_t __vk_disp_dl1x5_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);
#else
extern void vsf_dl1x5_on_dev_ready(void *dev);
#endif

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_dl1x5_id[] = {
    { VSF_USBH_MATCH_VID_PID(0x17E9, 0x019E) },
    { VSF_USBH_MATCH_VID_PID(0x17E9, 0x03CD) },
};

static const uint8_t __dl1x5_std_channel_seq[] = {
    0x57, 0xCD, 0xDC, 0xA7, 0x1C, 0x88, 0x5E, 0x15,
    0x60, 0xFE, 0xC6, 0x97, 0x16, 0x3D, 0x47, 0xF2,
};

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_dl1x5_drv = {
    .name       = "dl1x5",
    .dev_id_num = dimof(__vk_usbh_dl1x5_id),
    .dev_ids    = __vk_usbh_dl1x5_id,
    .probe      = __vk_usbh_dl1x5_probe,
    .disconnect = __vk_usbh_dl1x5_disconnect,
};

#ifdef __VSF_DL1X5_USE_DISP
static const vk_disp_drv_t __vk_disp_drv_dl1x5 = {
    .refresh    = __vk_disp_dl1x5_refresh,
};
#endif

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_DL1X5_ON_NEW_DEV
WEAK(vsf_dl1x5_on_new_dev)
void vsf_dl1x5_on_new_dev(void *dev, edid_t *edid)
{
    vga_timing_t *timing = vk_usbh_dl1x5_get_timing(dev);
    if (!timing->pixel_clock_10khz) {
        // use default 800X600 @ 60Hz timing
        timing->pixel_clock_10khz = 4000;
        timing->h.active = 800;
        timing->h.front_porch = 40;
        timing->h.sync = 128;
        timing->h.back_porch = 88;
        timing->h.sync_positive = 0;
        timing->v.active = 600;
        timing->v.front_porch = 1;
        timing->v.sync = 4;
        timing->v.back_porch = 23;
        timing->v.sync_positive = 0;
    }
}
#endif

#ifdef __VSF_DL1X5_USE_DISP
#   ifndef WEAK_VSF_DL1X5_ON_NEW_DISP
WEAK(vsf_dl1x5_on_new_disp)
void vsf_dl1x5_on_new_disp(vk_disp_t *disp) {}
#   endif
#else
#   ifndef WEAK_VSF_DL1X5_ON_DEV_READY
WEAK(vsf_dl1x5_on_dev_ready)
void vsf_dl1x5_on_dev_ready(void *dev) {}
#   endif
#endif

static vsf_err_t __vk_usbh_dl1x5_get_status(vk_usbh_dl1x5_t *dl1x5)
{
    VSF_USB_ASSERT((dl1x5 != NULL) && (dl1x5->usbh != NULL) && (dl1x5->dev != NULL));
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_VENDOR | USB_DIR_IN,
        .bRequest        =  USB_DL1X5_REQ_GET_STATUS,
        .wValue          =  0,
        .wIndex          =  0,
        .wLength         =  4,
    };
    vk_usbh_urb_set_buffer(&dl1x5->dev->ep0.urb, &dl1x5->cmd_buf, 4);
    return vk_usbh_control_msg(dl1x5->usbh, dl1x5->dev, &req);
}

#if VSF_USBH_DL1X5_CFG_READ_EDID == ENABLED
static vsf_err_t __vk_usbh_dl1x5_read_edid(vk_usbh_dl1x5_t *dl1x5)
{
    VSF_USB_ASSERT((dl1x5 != NULL) && (dl1x5->usbh != NULL) && (dl1x5->dev != NULL));
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_VENDOR | USB_DIR_IN,
        .bRequest        =  USB_DL1X5_REQ_READ_EDID,
        .wValue          =  0,
        .wIndex          =  0xA1,
        .wLength         =  1 + 128,
    };
    vk_usbh_urb_set_buffer(&dl1x5->dev->ep0.urb, &dl1x5->cmd_buf, 1 + 128);
    return vk_usbh_control_msg(dl1x5->usbh, dl1x5->dev, &req);
}

static bool __vga_parse_edid(edid_t *edid, vga_timing_t *timing)
{
    const uint8_t edid_header[8] = {
        0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
    };
    uint8_t checksum = 0, *edid_buf = (uint8_t *)edid;

    if (memcmp(edid->header, edid_header, sizeof(edid_header))) {
        vsf_trace_debug("DL1X5: edid : false header" VSF_TRACE_CFG_LINEEND);
        return false;
    }
    for (uint_fast8_t i = 0; i < sizeof(*edid); i++) {
        checksum += edid_buf[i];
    }
    if (checksum != 0) {
        vsf_trace_debug("DL1X5: edid : false checksum" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    vsf_trace_debug("DL1X5: edid:" VSF_TRACE_CFG_LINEEND);
    vsf_trace_buffer(VSF_TRACE_DEBUG, edid, 128);
    vsf_trace_debug("DL1X5: edid: manufacturer = 0x%04X, product = 0x%04X, serial = 0x%08X" VSF_TRACE_CFG_LINEEND,
                    edid->product.manufacturer, edid->product.product, edid->product.serial);
    vsf_trace_debug("DL1X5: edid: produced at year %d week %d" VSF_TRACE_CFG_LINEEND,
                    1990 + edid->product.year, edid->product.week);
    vsf_trace_debug("DL1X5: edid: version = %d, revision = %d" VSF_TRACE_CFG_LINEEND,
                    edid->version.version, edid->version.revision);
    vsf_trace_debug("DL1X5: established_timings = 0x%08X" VSF_TRACE_CFG_LINEEND,
                    edid->established_timings.timings + ((uint32_t)edid->established_timings.reserved_timings << 8));

    struct edid_detailed_timing_t *detailed_timing = &edid->detailed_timings[0];
    uint16_t blanking;
    if (!detailed_timing->pixel_clock_10khz) {
        vsf_trace_debug("DL1X5: user should provide the timings" VSF_TRACE_CFG_LINEEND);
        return true;
    }
    timing->pixel_clock_10khz   = detailed_timing->pixel_clock_10khz;
    timing->h.active            = detailed_timing->h_active_low + (detailed_timing->h_active_high << 8);
    blanking                    = detailed_timing->h_blanking_low + (detailed_timing->h_blanking_high << 8);
    timing->h.front_porch       = detailed_timing->h_sync_offset_low + (detailed_timing->h_sync_offset_high << 8);
    timing->h.sync              = detailed_timing->h_sync_width_low + (detailed_timing->h_sync_width_high << 8);
    timing->h.back_porch        = blanking - timing->h.front_porch - timing->h.sync;
    if (blanking < (timing->h.front_porch + timing->h.sync)) {
        vsf_trace_debug("DL1X5: invalid detailed timing" VSF_TRACE_CFG_LINEEND);
        timing->pixel_clock_10khz = 0;
        return true;
    }
    timing->h.sync_positive     = detailed_timing->h_sync_positive;
    timing->v.active            = detailed_timing->v_active_low + (detailed_timing->v_active_high << 8);
    blanking                    = detailed_timing->v_blanking_low + (detailed_timing->v_blanking_high << 8);
    timing->v.front_porch       = detailed_timing->v_sync_offset_low + (detailed_timing->v_sync_offset_high << 8);
    timing->v.sync              = detailed_timing->v_sync_width_low + (detailed_timing->v_sync_width_high << 8);
    timing->v.back_porch        = blanking - timing->v.front_porch - timing->v.sync;
    if (blanking < (timing->v.front_porch + timing->v.sync)) {
        vsf_trace_debug("DL1X5: invalid detailed timing" VSF_TRACE_CFG_LINEEND);
        timing->pixel_clock_10khz = 0;
        return true;
    }
    timing->v.sync_positive     = detailed_timing->v_sync_positive;
    return true;
}
#endif

static vsf_err_t __vk_usbh_dl1x5_set_channel(vk_usbh_dl1x5_t *dl1x5, const char *seq, uint8_t size)
{
    VSF_USB_ASSERT((dl1x5 != NULL) && (dl1x5->usbh != NULL) && (dl1x5->dev != NULL));
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_VENDOR | USB_DIR_OUT,
        .bRequest        =  USB_DL1X5_REQ_SET_CHANNEL,
        .wValue          =  0,
        .wIndex          =  0,
        .wLength         =  size,
    };
    memcpy(dl1x5->cmd_buf, seq, size);
    vk_usbh_urb_set_buffer(&dl1x5->dev->ep0.urb, &dl1x5->cmd_buf, size);
    return vk_usbh_control_msg(dl1x5->usbh, dl1x5->dev, &req);
}

static void __vk_usbh_dl1x5_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_dl1x5_t *dl1x5 = container_of(eda, vk_usbh_dl1x5_t, task);
    vsf_usbh_free(dl1x5);
}

static void __dl1x5_write_vcmd(vk_usbh_dl1x5_t *dl1x5, uint_fast8_t cmd)
{
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = 0xAF;
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = cmd;
}

static void __dl1x5_write_vreg(vk_usbh_dl1x5_t *dl1x5, uint_fast8_t reg, uint_fast8_t val)
{
    __dl1x5_write_vcmd(dl1x5, DL1X5_CMD_WRITE_VREG);
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = reg;
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = val;
}

static void __dl1x5_write_vreg_be16(vk_usbh_dl1x5_t *dl1x5, uint_fast8_t reg, uint_fast16_t val)
{
    val = cpu_to_be16(val);
    __dl1x5_write_vreg(dl1x5, reg, val & 0xFF);
    __dl1x5_write_vreg(dl1x5, reg + 1, val >> 8);
}

static void __dl1x5_write_vreg_le16(vk_usbh_dl1x5_t *dl1x5, uint_fast8_t reg, uint_fast16_t val)
{
#if   __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wself-assign"
#endif

    val = cpu_to_le16(val);

#if   __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif
    __dl1x5_write_vreg(dl1x5, reg, val & 0xFF);
    __dl1x5_write_vreg(dl1x5, reg + 1, val >> 8);
}

static bool __dl1x5_write_rl16(vk_usbh_dl1x5_t *dl1x5, uint_fast32_t gram_addr,
        uint_fast16_t pixel_num, uint16_t color_rgb565)
{
    VSF_USB_ASSERT(pixel_num <= DL1X5_PIXELS_IN_CMD);
    if (DL1X5_PIXELS_IN_CMD == pixel_num) {
        pixel_num = 0;
    }

    if (dl1x5->cmd_pos > VSF_USBH_DL1X5_CFG_CMD_BUFFER_SIZE - 9) {
        return false;
    }

    __dl1x5_write_vcmd(dl1x5, DL1X5_CMD_WRITE_RL16);
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = gram_addr >> 16;
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = gram_addr >> 8;
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = gram_addr >> 0;
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = pixel_num;
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = pixel_num;
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = color_rgb565 >> 8;
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = color_rgb565 >> 0;
    return true;
}

static bool __dl1x5_write_raw16(vk_usbh_dl1x5_t *dl1x5, uint_fast32_t gram_addr,
        uint_fast16_t pixel_num, uint16_t *color_rgb565_ptr)
{
    VSF_USB_ASSERT(pixel_num <= DL1X5_PIXELS_IN_CMD);
    if (dl1x5->cmd_pos > (VSF_USBH_DL1X5_CFG_CMD_BUFFER_SIZE - (6 + (pixel_num << 1)))) {
        return false;
    }

    __dl1x5_write_vcmd(dl1x5, DL1X5_CMD_WRITE_RAW16);
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = gram_addr >> 16;
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = gram_addr >> 8;
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = gram_addr >> 0;
    dl1x5->cmd_buf[dl1x5->cmd_pos++] = pixel_num == DL1X5_PIXELS_IN_CMD ? 0 : pixel_num;

    for (uint_fast16_t i = 0; i < pixel_num; i++) {
        dl1x5->cmd_buf[dl1x5->cmd_pos++] = color_rgb565_ptr[i] >> 8;
        dl1x5->cmd_buf[dl1x5->cmd_pos++] = color_rgb565_ptr[i] >> 0;
    }
    return true;
}

static uint16_t __dl1x5_lfsr16(uint16_t val)
{
    uint32_t result = 0xFFFF;

    while (val--) {
        result = ((result << 1) | (((result >> 15) ^ (result >> 4) ^ (result >> 2) ^ (result >> 1)) & 1)) & 0xFFFF;
    }
    return (uint16_t)result;
}

static void __vk_usbh_dl1x5_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_dl1x5_t *dl1x5 = container_of(eda, vk_usbh_dl1x5_t, task);
    vk_usbh_dev_t *dev = dl1x5->dev;

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != __vsf_eda_crit_npb_enter(&dev->ep0.crit)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        __vk_usbh_dl1x5_get_status(dl1x5);
        dl1x5->state = DL1X5_STATE_GET_STATUS;
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vk_usbh_pipe_t pipe = vk_usbh_urb_get_pipe(&urb);
            int_fast16_t status = vk_usbh_urb_get_status(&urb);
//            uint_fast32_t actlen = vk_usbh_urb_get_actual_length(&urb);

            if (status != URB_OK) {
                vsf_trace_error("DL1X5: urb failed" VSF_TRACE_CFG_LINEEND);
                if (dl1x5->callback != NULL) {
                    dl1x5->callback(dl1x5, VSF_ERR_FAIL);
                }
                vk_usbh_remove_interface(dl1x5->usbh, dev, dl1x5->ifs);
                break;
            }

            if (0 == pipe.endpoint) {
                switch (dl1x5->state) {
                case DL1X5_STATE_GET_STATUS:
                    vsf_trace_debug("DL1X5: status = 0x%08X" VSF_TRACE_CFG_LINEEND,
                            get_unaligned_le32(dl1x5->cmd_buf));
#if VSF_USBH_DL1X5_CFG_READ_EDID == ENABLED
                    __vk_usbh_dl1x5_read_edid(dl1x5);
                    dl1x5->state = DL1X5_STATE_READ_EDID;
                    break;
                case DL1X5_STATE_READ_EDID:
                    if (    (dl1x5->cmd_buf[0] != 0)
                        ||  !__vga_parse_edid((edid_t *)&dl1x5->cmd_buf[1], &dl1x5->timing)) {
                        vsf_teda_set_timer_ms(1000);
                        break;
                    }
                    vsf_dl1x5_on_new_dev(dl1x5, (edid_t *)&dl1x5->cmd_buf[1]);
#else
                    vsf_dl1x5_on_new_dev(dl1x5, NULL);
#endif
                    __vk_usbh_dl1x5_set_channel(dl1x5, (const char *)__dl1x5_std_channel_seq, sizeof(__dl1x5_std_channel_seq));
                    dl1x5->state = DL1X5_STATE_SET_STD_CHANNEL;
                    break;
                case DL1X5_STATE_SET_STD_CHANNEL: {
                        vga_timing_t *timing = &dl1x5->timing;

                        if (0 == timing->pixel_clock_10khz) {
                            // invalid timing
                            vsf_trace_error("DL1X5: invalid timing" VSF_TRACE_CFG_LINEEND);
                            break;
                        }

                        uint_fast16_t h_cycles = timing->h.active + timing->h.front_porch + timing->h.sync + timing->h.back_porch;
                        uint_fast16_t v_cycles = timing->v.active + timing->v.front_porch + timing->v.sync + timing->v.back_porch;
                        vsf_trace_debug("DL1X5: %dx%d @ %dHz" VSF_TRACE_CFG_LINEEND,
                            timing->h.active, timing->v.active, timing->pixel_clock_10khz * 10000 / (v_cycles * h_cycles));

                        __dl1x5_write_vreg(dl1x5, 0xFF, 0x00);      // vreg_lock
                        __dl1x5_write_vreg(dl1x5, 0x20, 0);         // base16_16_23
                        __dl1x5_write_vreg(dl1x5, 0x21, 0);         // base16_8_15
                        __dl1x5_write_vreg(dl1x5, 0x22, 0);         // base16_0_7
                        __dl1x5_write_vreg(dl1x5, 0x26, 0);         // base8_16_23
                        __dl1x5_write_vreg(dl1x5, 0x27, 0);         // base8_8_15
                        __dl1x5_write_vreg(dl1x5, 0x28, 0);         // base8_0_7

                        // counter registers with lfsr16 encoded
                        uint16_t v_sync_start = 0;
                        uint16_t v_sync_end = timing->v.sync;
                        uint16_t v_display_start = v_sync_end + timing->v.back_porch;
                        uint16_t v_display_end = v_display_start + timing->v.active;
                        uint16_t v_end_count = v_display_end + timing->v.front_porch;

                        uint16_t h_sync_start = 1;
                        uint16_t h_sync_end = timing->h.sync + 1;
                        uint16_t h_display_start = h_sync_end + timing->h.back_porch - 1;
                        uint16_t h_display_end = h_display_start + timing->h.active;
                        uint16_t h_end_count = h_display_end + timing->h.front_porch - 1;

                        uint16_t h_pixels = timing->h.active;
                        uint16_t v_pixels = timing->v.active;
                        uint16_t pixel_clock_5khz = timing->pixel_clock_10khz << 1;

                        if (!(timing->h.sync_positive)) {
                            h_sync_start = h_sync_start ^ h_sync_end;
                            h_sync_end = h_sync_start ^ h_sync_end;
                            h_sync_start = h_sync_start ^ h_sync_end;
                        }
                        if (!(timing->v.sync_positive)) {
                            v_sync_start = v_sync_start ^ v_sync_end;
                            v_sync_end = v_sync_start ^ v_sync_end;
                            v_sync_start = v_sync_start ^ v_sync_end;
                        }

                        // color depth, 0: 16, 1: 24
                        __dl1x5_write_vreg(dl1x5, 0x00, 0);
                        __dl1x5_write_vreg_be16(dl1x5, 0x01, __dl1x5_lfsr16(h_display_start));
                        __dl1x5_write_vreg_be16(dl1x5, 0x03, __dl1x5_lfsr16(h_display_end));
                        __dl1x5_write_vreg_be16(dl1x5, 0x05, __dl1x5_lfsr16(v_display_start));
                        __dl1x5_write_vreg_be16(dl1x5, 0x07, __dl1x5_lfsr16(v_display_end));
                        __dl1x5_write_vreg_be16(dl1x5, 0x09, __dl1x5_lfsr16(h_end_count));
                        __dl1x5_write_vreg_be16(dl1x5, 0x0B, __dl1x5_lfsr16(h_sync_start));
                        __dl1x5_write_vreg_be16(dl1x5, 0x0D, __dl1x5_lfsr16(h_sync_end));
                        __dl1x5_write_vreg_be16(dl1x5, 0x0F, h_pixels);
                        __dl1x5_write_vreg_be16(dl1x5, 0x11, __dl1x5_lfsr16(v_end_count));
                        __dl1x5_write_vreg_be16(dl1x5, 0x13, __dl1x5_lfsr16(v_sync_start));
                        __dl1x5_write_vreg_be16(dl1x5, 0x15, __dl1x5_lfsr16(v_sync_end));
                        __dl1x5_write_vreg_be16(dl1x5, 0x17, v_pixels);
                        __dl1x5_write_vreg_le16(dl1x5, 0x1B, pixel_clock_5khz);

                        // enable hsync, vsync, video
                        __dl1x5_write_vreg(dl1x5, 0x1F, 0);
                        // flush register
                        __dl1x5_write_vreg(dl1x5, 0xFF, 0xFF);

                        dl1x5->state = DL1X5_STATE_SET_MODE;
                        vk_usbh_dl1x5_commit(dl1x5);
                    }
                    break;
                }
            } else {
                dl1x5->is_busy = false;
                dl1x5->cmd_pos = 0;
                switch (dl1x5->state) {
                case DL1X5_STATE_SET_MODE:
#if VSF_USBH_DL1X5_CFG_CLS == ENABLED
                    dl1x5->state = DL1X5_STATE_CLS;
                    dl1x5->pixel_addr = 0;
                    // fall through
                case DL1X5_STATE_CLS:
                    if (__vk_dl1x5_cls_callback(dl1x5)) {
                        if (dl1x5->cmd_pos > 0) {
                            dl1x5->state = DL1X5_STATE_CLS_LAST;
                            vk_usbh_dl1x5_commit(dl1x5);
                            break;
                        }
                        goto __DL1X5_STATE_CLS_LAST;
                    }
                    break;
                case DL1X5_STATE_CLS_LAST:
                __DL1X5_STATE_CLS_LAST:
#endif
                    dl1x5->state = DL1X5_STATE_RUNNING;
#ifdef __VSF_DL1X5_USE_DISP
                    vk_usbh_dl1x5_set_callback(dl1x5, __vk_disp_dl1x5_refresh_callback);

                    {
                        vk_disp_param_t *param = (vk_disp_param_t *)&dl1x5->param;
                        param->drv = &__vk_disp_drv_dl1x5;
                        param->width = dl1x5->timing.h.active;
                        param->height = dl1x5->timing.v.active;
                        param->color = VSF_DISP_COLOR_RGB565;
                    }
                    vsf_dl1x5_on_new_disp(&dl1x5->use_as__vk_disp_t);
#else
                    vsf_dl1x5_on_dev_ready(dl1x5);
#endif
                    break;
                case DL1X5_STATE_RUNNING:
                    if (dl1x5->callback != NULL) {
                        dl1x5->callback(dl1x5, VSF_ERR_NONE);
                    }
                    break;
                }
            }
        }
        break;
#if VSF_USBH_DL1X5_CFG_READ_EDID == ENABLED
    case VSF_EVT_TIMER:
        switch (dl1x5->state) {
        case DL1X5_STATE_READ_EDID:
            __vk_usbh_dl1x5_read_edid(dl1x5);
            break;
        }
        break;
#endif
    }
}

static void *__vk_usbh_dl1x5_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    vk_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;
    struct usb_endpoint_desc_t *desc_ep = parser_alt->desc_ep;

    vk_usbh_dl1x5_t *dl1x5 = vsf_usbh_malloc(sizeof(vk_usbh_dl1x5_t));
    if (dl1x5 == NULL) {
        return NULL;
    }
    memset(dl1x5, 0, sizeof(vk_usbh_dl1x5_t));

    for (int i = 0; i < desc_ifs->bNumEndpoints; i++) {
        if (desc_ep->bEndpointAddress == (USB_DIR_OUT | 1)) {
            vk_usbh_urb_prepare(&dl1x5->urb_out, dev, desc_ep);
            vk_usbh_alloc_urb(usbh, dev, &dl1x5->urb_out);
            break;
        }
        desc_ep = vk_usbh_get_next_ep_descriptor(desc_ep,
            parser_alt->desc_size - ((uintptr_t)desc_ep - (uintptr_t)desc_ifs));
    }
    if (desc_ep->bEndpointAddress != (USB_DIR_OUT | 1)) {
        vsf_usbh_free(dl1x5);
        return NULL;
    }

    dl1x5->usbh = usbh;
    dl1x5->dev = dev;
    dl1x5->ifs = ifs;

    dl1x5->task.fn.evthandler = __vk_usbh_dl1x5_evthandler;
    dl1x5->task.on_terminate = __vk_usbh_dl1x5_on_eda_terminate;
#if VSF_USBH_DL1X5_CFG_READ_EDID == ENABLED
#   ifdef VSF_USBH_DL1X5_CFG_PRIORITY
    vsf_teda_init(&dl1x5->task, VSF_USBH_DL1X5_CFG_PRIORITY);
#   else
    vsf_teda_init(&dl1x5->task);
#   endif
#else
#   ifdef VSF_USBH_DL1X5_CFG_PRIORITY
    vsf_eda_init(&dl1x5->task, VSF_USBH_DL1X5_CFG_PRIORITY);
#   else
    vsf_eda_init(&dl1x5->task);
#   endif
#endif
#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info((vsf_eda_t *)&dl1x5->task, "usbh_dl1x5_task", NULL, 0);
#endif
    return dl1x5;
}

static void __vk_usbh_dl1x5_disconnect(vk_usbh_t *usbh,
        vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_dl1x5_t *dl1x5 = (vk_usbh_dl1x5_t *)param;
    VSF_USB_ASSERT(dl1x5 != NULL);

    vk_usbh_free_urb(usbh, &dl1x5->urb_out);
    vsf_eda_fini((vsf_eda_t *)&dl1x5->task);
}

vga_timing_t * vk_usbh_dl1x5_get_timing(void *dev)
{
    vk_usbh_dl1x5_t *dl1x5 = (vk_usbh_dl1x5_t *)dev;
    return &dl1x5->timing;
}

void vk_usbh_dl1x5_set_callback(void *dev, vk_usbh_dl1x5_callback_t callback)
{
    vk_usbh_dl1x5_t *dl1x5 = (vk_usbh_dl1x5_t *)dev;
    dl1x5->callback = callback;
}

vsf_err_t vk_usbh_dl1x5_commit(void *dev)
{
    vk_usbh_dl1x5_t *dl1x5 = (vk_usbh_dl1x5_t *)dev;

    if (!dl1x5->cmd_pos) {
        return VSF_ERR_NOT_READY;
    }

    dl1x5->is_busy = true;
    __dl1x5_write_vcmd(dl1x5, DL1X5_CMD_NULL);
    vk_usbh_urb_set_buffer(&dl1x5->urb_out, &dl1x5->cmd_buf, dl1x5->cmd_pos);
    return vk_usbh_submit_urb_ex(dl1x5->usbh, &dl1x5->urb_out, 0, (vsf_eda_t *)&dl1x5->task);
}

bool vk_usbh_dl1x5_fill_color(void *dev, uint_fast32_t gram_pixel_addr,
            uint_fast16_t pixel_num, uint16_t color_rgb565)
{
    vk_usbh_dl1x5_t *dl1x5 = (vk_usbh_dl1x5_t *)dev;
    VSF_USB_ASSERT(!dl1x5->is_busy);
    return __dl1x5_write_rl16(dl1x5, gram_pixel_addr << 1, pixel_num, color_rgb565);
}

bool vk_usbh_dl1x5_fill_buf(void *dev, uint_fast32_t gram_pixel_addr,
            uint_fast16_t pixel_num, uint16_t *color_rgb565_ptr)
{
    vk_usbh_dl1x5_t *dl1x5 = (vk_usbh_dl1x5_t *)dev;
    VSF_USB_ASSERT(!dl1x5->is_busy);
    return __dl1x5_write_raw16(dl1x5, gram_pixel_addr << 1, pixel_num, color_rgb565_ptr);
}

#if VSF_USBH_DL1X5_CFG_CLS == ENABLED
static bool __vk_dl1x5_cls_callback(void *dev)
{
    vk_usbh_dl1x5_t *dl1x5 = (vk_usbh_dl1x5_t *)dev;
    vga_timing_t *timing = vk_usbh_dl1x5_get_timing(dev);

    while (dl1x5->pixel_addr < timing->h.active * timing->v.active) {
        if (!vk_usbh_dl1x5_fill_color(dev, dl1x5->pixel_addr, DL1X5_PIXELS_IN_CMD, 0)) {
            vsf_err_t err = vk_usbh_dl1x5_commit(dev);
            VSF_UNUSED_PARAM(err);
            VSF_USB_ASSERT(VSF_ERR_NONE == err);
            return false;
        }
        dl1x5->pixel_addr += DL1X5_PIXELS_IN_CMD;
    }

    return true;
}
#endif




#ifdef __VSF_DL1X5_USE_DISP
static void __vk_disp_dl1x5_refresh_callback(void *dev, vsf_err_t err)
{
    vk_usbh_dl1x5_t *dl1x5 = (vk_usbh_dl1x5_t *)dev;

    if (VSF_ERR_NONE != err) {
        return;
    }
    if (dl1x5->disp_buff_rgb565 != NULL) {
        vk_disp_area_t *area = &dl1x5->disp_area;
        uint_fast32_t pixel_remain = area->size.x * area->size.y - dl1x5->disp_pixel_pos;
        uint_fast32_t pixel_line_offset, pixel_line_num;
        uint_fast32_t pixel_addr;
        uint_fast16_t pixel_num;

        while (pixel_remain > 0) {
            pixel_line_num = dl1x5->disp_pixel_pos / area->size.x;
            pixel_line_offset = dl1x5->disp_pixel_pos % area->size.x;
            pixel_addr =    (area->pos.y + pixel_line_num) * dl1x5->timing.h.active
                        +   area->pos.x + pixel_line_offset;
            if (area->size.x == dl1x5->timing.h.active) {
                pixel_num = vsf_min(DL1X5_PIXELS_IN_CMD, pixel_remain);
            } else {
                pixel_line_offset = area->size.x - pixel_line_offset;
                pixel_num = vsf_min(DL1X5_PIXELS_IN_CMD, pixel_line_offset);
            }

            if (!vk_usbh_dl1x5_fill_buf(dev, pixel_addr, pixel_num, dl1x5->disp_buff_rgb565)) {
                vsf_err_t err = vk_usbh_dl1x5_commit(dev);
                VSF_UNUSED_PARAM(err);
                VSF_USB_ASSERT(VSF_ERR_NONE == err);
                return;
            }
            dl1x5->disp_pixel_pos += pixel_num;
            dl1x5->disp_buff_rgb565 += pixel_num;
            pixel_remain -= pixel_num;
        }

        dl1x5->disp_buff_rgb565 = NULL;
        if (VSF_ERR_NONE == vk_usbh_dl1x5_commit(dev)) {
            return;
        }
    }

    if (NULL == dl1x5->disp_buff_rgb565) {
        if (dl1x5->ui_on_ready != NULL) {
            dl1x5->ui_on_ready(&dl1x5->use_as__vk_disp_t);
        }
    }
}

static vsf_err_t __vk_disp_dl1x5_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    vk_usbh_dl1x5_t *dl1x5 = container_of(pthis, vk_usbh_dl1x5_t, use_as__vk_disp_t);

    dl1x5->disp_area = *area;
    dl1x5->disp_buff_rgb565 = (uint16_t *)disp_buff;
    dl1x5->disp_pixel_pos = 0;
    __vk_disp_dl1x5_refresh_callback(dl1x5, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}
#endif

#endif
