/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *****************************************************************************/

#ifndef __VSF_ES8389_H__
#define __VSF_ES8389_H__

/*============================ INCLUDES ======================================*/

#include "../../../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED && VSF_AUDIO_USE_ES8389 == ENABLED

#include "component/av/vsf_av.h"
#include "hal/vsf_hal.h"
#include "kernel/vsf_eda.h"            // vsf_sem_t for the i2c sync member

#if     defined(__VSF_ES8389_CLASS_IMPLEMENT)
#   undef __VSF_ES8389_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif
// re-including a class header in another mode later(e.g. the board file
// in plain extern mode) needs the original flag back
#if     defined(__VSF_ES8389_CLASS_INHERIT__)
#   undef __VSF_ES8389_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_es8389_reg_op_t {
    uint8_t reg;
    uint8_t value;
    uint8_t mask;                       // 0: write, non-zero: read-modify-write
} vk_es8389_reg_op_t;

vsf_class(vk_es8389_dev_t) {
    public_member(
        implement(vk_audio_i2s_dev_t)

        // Control-bus connection and configuration; initialised by the driver.
        vsf_i2c_t *i2c_ptr;
        uint16_t i2c_addr;              // 7-bit
        uint32_t i2c_clock_hz;
        vsf_arch_prio_t i2c_isr_prio;

        // false: route the DAC reference into the recorded right channel;
        // true: independent playback and capture paths.
        bool no_dac_ref;

        // Board amplifier enable pad; NULL port = none.
        vsf_gpio_t *pa_port;
        vsf_gpio_pin_mask_t pa_pin_mask;
        bool pa_active_high;

        // Applied on each new codec session, before later standard controls.
        int16_t playback_volume_half_db;
        uint8_t capture_pga_gain_steps;
    )

    private_member(
        vsf_teda_t teda;
        vsf_sem_t cmd_sem;
        vsf_sem_t i2c_sem;
        vsf_trig_t stop_trig;
        vsf_trig_t ctrl_trig;
        volatile vsf_i2c_irq_mask_t i2c_irq;

        const vk_es8389_reg_op_t *ops;
        vk_es8389_reg_op_t fs_ops[21];
        vk_es8389_reg_op_t control_ops[4];
        uint8_t op_count;
        uint8_t op_index;
        uint8_t xfer_phase;
        uint8_t tx_buf[2];
        uint8_t rx_buf[1];

        uint16_t session;
        uint16_t work_session;
        uint16_t sample_rate[2];        // 100 Hz units, playback/capture
        uint8_t request_mask;
        uint8_t started_mask;
        uint8_t enabled_mask;
        uint8_t wait_state;
        uint8_t work;
        uint8_t retry;
        bool i2c_outstanding;
        bool i2c_timed_out;
        bool recover_retry;
        bool session_failed;
        bool fs_configured;
        bool analog_enabled;
        bool is_opened;
        bool is_ready;
        vsf_err_t last_error;

        bool stop_pending;
        bool stop_completed;
        bool stop_final;
        vsf_err_t stop_result;
        bool ctrl_pending;
        bool ctrl_completed;
        vsf_err_t ctrl_result;
        uint8_t ctrl_dir;
        vk_av_control_type_t ctrl_type;
        vk_av_control_value_t ctrl_value;

        int16_t playback_volume;
        uint8_t capture_gain;
        bool playback_muted;
        bool capture_muted;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_audio_drv_t vk_es8389_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_AUDIO && VSF_AUDIO_USE_ES8389
#endif      // __VSF_ES8389_H__
