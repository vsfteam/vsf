/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  You may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#define __VSF_I2C_UTIL_CLASS_INHERIT__
#define __VSF_INPUT_GT9XX_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

// for vsf_input
#include "component/vsf_component.h"

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_GT9XX == ENABLED

#if VSF_INPUT_GT9XX_CFG_SUPPORT_EXTI != ENABLED && VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER != ENABLED
#   error gt9xx poll mode needs VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER
#endif

/*============================ MACROS ========================================*/

#define GT9XX_MAX_TOUCH                     5

// 16-bit big-endian register map
#define GT9XX_REG_PRODUCT_ID                0x8140
#define GT9XX_REG_READ_STATUS               0x814E
#define GT9XX_REG_POINTS                    0x814F

// 7-bit i2c addresses latched by the int pin level at the reset rising edge
#define GT9XX_ADDRESS_HIGH                  0x5D
#define GT9XX_ADDRESS_LOW                   0x14

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    GT9XX_STATE_PROBE,
    GT9XX_STATE_IDLE,       // ready and no frame in flight(poll entry point)
    GT9XX_STATE_READ,
    GT9XX_STATE_READ_POINTS,
    GT9XX_STATE_CLEAR,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const uint8_t __vk_input_gt9xx_addr_tab[] = {
    GT9XX_ADDRESS_HIGH, GT9XX_ADDRESS_LOW,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vk_input_gt9xx_report(vk_input_gt9xx_t *gt9xx, uint8_t id,
                bool is_down, uint16_t x, uint16_t y, uint16_t pressure)
{
    vk_touchscreen_evt_t ts_evt = {
        .dev            = gt9xx,
        .info.width     = gt9xx->width,
        .info.height    = gt9xx->height,
    };
    vsf_input_touchscreen_set(&ts_evt, id, is_down, pressure, x, y);
    vsf_input_on_touchscreen(&ts_evt);
}

static void __vk_input_gt9xx_read_touch(vk_input_gt9xx_t *gt9xx)
{
    uint8_t status = gt9xx->buffer[0];
    uint8_t touch_num = status & 0x0F;
    uint8_t cur_down = 0;

    if ((status & 0x80) && (touch_num <= GT9XX_MAX_TOUCH)) {
        for (uint8_t i = 0; i < touch_num; i++) {
            uint8_t *point = &gt9xx->buffer[1 + (i * 8)];
            uint8_t id = point[0] & 0x0F;
            uint16_t x = point[1] | ((uint16_t)point[2] << 8);
            uint16_t y = point[3] | ((uint16_t)point[4] << 8);
            uint16_t size = point[5] | ((uint16_t)point[6] << 8);

            cur_down |= 1 << id;
            __vk_input_gt9xx_report(gt9xx, id, true, x, y, size);
        }
    }
    // releases for track ids no longer reported(the controller only
    // reports active fingers; a vanished one is an implicit release)
    for (uint8_t id = 0; id < GT9XX_MAX_TOUCH; id++) {
        if ((gt9xx->last_down & ~cur_down) & (1 << id)) {
            __vk_input_gt9xx_report(gt9xx, id, false, 0, 0, 0);
        }
    }
    gt9xx->last_down = cur_down;
}

#if VSF_INPUT_GT9XX_CFG_SUPPORT_EXTI == ENABLED
static void __vk_input_gt9xx_int_isrhandler(void *target_ptr, vsf_gpio_t *gpio_ptr,
                vsf_gpio_pin_mask_t pin_mask);

static void __vk_input_gt9xx_arm_int(vk_input_gt9xx_t *gt9xx)
{
    vsf_gpio_exti_irq_enable(gt9xx->gpio_int, 1 << gt9xx->gpio_int_pin);
}

// first arm after the probe succeeded: pad + exti configuration, then arm.
// int pin: data-ready pulses; BOTH edges(the vendor driver arms
// rising+falling on its input-mode int pad - the panel's configured edge
// direction is not reliably known, 0x804D readback interpretation is
// uncertain)
static void __vk_input_gt9xx_trigger(vk_input_gt9xx_t *gt9xx)
{
    if (gt9xx->gpio_int != NULL) {
        vsf_gpio_cfg_t cfg = {
            .mode           = VSF_GPIO_NO_PULL_UP_DOWN | VSF_GPIO_EXTI
                            | VSF_GPIO_EXTI_MODE_RISING_FALLING,
        };
        vsf_gpio_port_config_pins(gt9xx->gpio_int, 1 << gt9xx->gpio_int_pin, &cfg);
        vsf_gpio_exti_irq_config(gt9xx->gpio_int, &(vsf_gpio_exti_irq_cfg_t){
            .handler_fn     = __vk_input_gt9xx_int_isrhandler,
            .target_ptr     = gt9xx,
            .prio           = gt9xx->prio,
        });
        __vk_input_gt9xx_arm_int(gt9xx);
    }
}
#endif

// write-to-clear 0x814E as ONE write transaction(register address + data
// together) - see the note at the GT9XX_STATE_CLEAR issue site for why the
// regacc two-phase write must not be used here
static void __vk_input_gt9xx_clear_status(vk_input_gt9xx_t *gt9xx)
{
    gt9xx->buffer[0] = (uint8_t)(GT9XX_REG_READ_STATUS >> 8);
    gt9xx->buffer[1] = (uint8_t)GT9XX_REG_READ_STATUS;
    gt9xx->buffer[2] = 0;
    gt9xx->state = GT9XX_STATE_CLEAR;
    vsf_i2c_master_request(gt9xx->regacc.i2c_ptr, gt9xx->regacc.i2c_addr,
        VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE, 3, gt9xx->buffer);
}

// start one frame read: status byte + point slot 0 in a single regacc;
// slots 1..4 are read in a second stage only when the status byte reports
// them(the vendor driver's conditional shape)
static void __vk_input_gt9xx_read_status(vk_input_gt9xx_t *gt9xx)
{
    gt9xx->state = GT9XX_STATE_READ;
    vsf_i2c_regacc(&gt9xx->regacc, GT9XX_REG_READ_STATUS, true,
        gt9xx->buffer, 9);
}

static void __vk_input_gt9xx_on_frame_done(vk_input_gt9xx_t *gt9xx)
{
    gt9xx->state = GT9XX_STATE_IDLE;
#if VSF_INPUT_GT9XX_CFG_SUPPORT_EXTI == ENABLED
    if (gt9xx->gpio_int != NULL) {
        __vk_input_gt9xx_arm_int(gt9xx);
    }
#endif
}

#if VSF_INPUT_GT9XX_CFG_SUPPORT_EXTI == ENABLED
static void __vk_input_gt9xx_int_isrhandler(void *target_ptr, vsf_gpio_t *gpio_ptr,
                vsf_gpio_pin_mask_t pin_mask)
{
    vk_input_gt9xx_t *gt9xx = target_ptr;
    vsf_gpio_exti_irq_disable(gt9xx->gpio_int, 1 << gt9xx->gpio_int_pin);
    __vk_input_gt9xx_read_status(gt9xx);
}
#endif

static void __vk_input_gt9xx_i2c_isrhandler(void *target_ptr, vsf_i2c_t *i2c_ptr,
                vsf_i2c_irq_mask_t irq_mask)
{
    vk_input_gt9xx_t *gt9xx = target_ptr;
    vsf_err_t err = vsf_i2c_regacc_irqhandler(&gt9xx->regacc, irq_mask);
    if (err < 0) {
        if (GT9XX_STATE_PROBE == gt9xx->state) {
            // no ack at the current candidate address, walk to the next
            if (++gt9xx->addr_idx < dimof(__vk_input_gt9xx_addr_tab)) {
                gt9xx->regacc.i2c_addr = __vk_input_gt9xx_addr_tab[gt9xx->addr_idx];
                vsf_i2c_regacc(&gt9xx->regacc, GT9XX_REG_PRODUCT_ID, true,
                    gt9xx->buffer, 4);
            } else {
                gt9xx->probe_failed = 1;
            }
        } else {
            // runtime failure: re-arm and let the next trigger retry(isr
            // side no tracing, see the class comment)
            __vk_input_gt9xx_on_frame_done(gt9xx);
        }
        return;
    } else if (VSF_ERR_NONE != err) {
        // regaddr phase done, data phase still running
        return;
    }

    switch (gt9xx->state) {
    case GT9XX_STATE_PROBE: {
            // product id is ascii, "911"/"9147"/"1157"/"1151"... across
            // variants; two printable leading characters accept the whole
            // family while rejecting bus garbage(0x00/0xFF from a dead
            // but acknowledging device). isr side NO tracing here(see the
            // class comment): outcomes go to the is_ready/probe_failed
            // flags, threadside reads them
            uint8_t *id = gt9xx->buffer;
            if (((id[0] >= 0x20) && (id[0] < 0x7F))
                &&  ((id[1] >= 0x20) && (id[1] < 0x7F))) {
                gt9xx->is_ready = 1;
                gt9xx->last_down = 0;
                // hand over to the frame trigger: the exti arm below for
                // int-pad boards, the already-armed poll timer otherwise
                gt9xx->state = GT9XX_STATE_IDLE;
#if VSF_INPUT_GT9XX_CFG_SUPPORT_EXTI == ENABLED
                if (gt9xx->gpio_int != NULL) {
                    __vk_input_gt9xx_trigger(gt9xx);
                }
#endif
            } else {
                gt9xx->probe_failed = 1;
            }
        }
        break;
    case GT9XX_STATE_READ: {
            uint8_t status = gt9xx->buffer[0];
            uint8_t touch_num = status & 0x0F;
            // no frame yet(status bit7 clear): the int trigger reads on a
            // data-ready edge so this is a rare spurious edge; the poll
            // trigger hits it between frames on purpose. report nothing
            // and do not clear - an implicit all-up here would churn held
            // fingers in poll mode
            if (!(status & 0x80)) {
                __vk_input_gt9xx_on_frame_done(gt9xx);
                break;
            }
            if ((touch_num > 1) && (touch_num <= GT9XX_MAX_TOUCH)) {
                gt9xx->state = GT9XX_STATE_READ_POINTS;
                vsf_i2c_regacc(&gt9xx->regacc, GT9XX_REG_POINTS + 8, true,
                    &gt9xx->buffer[9], (touch_num - 1) * 8);
                break;
            }
            // 0-touch frame / single-touch(point 0 already in buffer[1..8]):
            // release tracking still runs(read_touch sees touch_num=0 and
            // only emits missing ups)
            __vk_input_gt9xx_read_touch(gt9xx);
            __vk_input_gt9xx_clear_status(gt9xx);
        }
        break;
    case GT9XX_STATE_READ_POINTS:
        __vk_input_gt9xx_read_touch(gt9xx);
        // the status flag is write-to-clear: 0 to 0x814E ends the cycle
        // and drops the int line. the clear MUST be a single write
        // transaction carrying register address + data(GT9xx parses the
        // first 2 bytes of any write as the 16-bit register address; the
        // regacc two-phase write would deliver the data byte as a
        // standalone 1-byte write, which the controller takes as an
        // address-pointer set - the clear never lands and the status
        // byte freezes, so no frame is ever reported again)
        __vk_input_gt9xx_clear_status(gt9xx);
        break;
    case GT9XX_STATE_CLEAR:
        __vk_input_gt9xx_on_frame_done(gt9xx);
        break;
    }
}

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
static void __vk_input_gt9xx_on_poll(vsf_callback_timer_t *timer)
{
    vk_input_gt9xx_t *gt9xx = vsf_container_of(timer, vk_input_gt9xx_t, poll_timer);

#if VSF_INPUT_GT9XX_CFG_SUPPORT_EXTI == ENABLED
    // int pad wired boards do not run the poll timer at all
    if (gt9xx->gpio_int != NULL) {
        return;
    }
#endif
    // poll tick: one frame read unless a frame cycle is still in flight
    // (the cycle itself completes in interrupts; a cycle = status read ->
    // point slots -> clear, the next tick re-reads the status register)
    if ((GT9XX_STATE_IDLE == gt9xx->state) && gt9xx->is_ready) {
        __vk_input_gt9xx_read_status(gt9xx);
    }
    vsf_callback_timer_add_ms(&gt9xx->poll_timer, VSF_INPUT_GT9XX_CFG_POLL_MS);
}
#endif

vsf_err_t vk_input_gt9xx_init(vk_input_gt9xx_t *gt9xx, vsf_arch_prio_t prio)
{
    VSF_INPUT_ASSERT((gt9xx != NULL) && (prio != vsf_arch_prio_invalid));

    gt9xx->prio = prio;
    gt9xx->state = GT9XX_STATE_PROBE;
    // 16-bit big-endian register address(vsf_i2c_regacc extension)
    gt9xx->regacc.reg_addr_len = 2;
    gt9xx->addr_idx = 0;
    gt9xx->regacc.i2c_addr = __vk_input_gt9xx_addr_tab[0];

    vsf_i2c_init(gt9xx->regacc.i2c_ptr, &(vsf_i2c_cfg_t){
        .mode           = VSF_I2C_MODE_MASTER | VSF_I2C_SPEED_FAST_MODE | VSF_I2C_ADDR_7_BITS,
        .clock_hz       = 400 * 1000,
        .isr            = {
            .handler_fn = __vk_input_gt9xx_i2c_isrhandler,
            .target_ptr = gt9xx,
            .prio       = prio,
        },
    });
    vsf_i2c_irq_enable(gt9xx->regacc.i2c_ptr, VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                                |   VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                                |   VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT);
    vsf_i2c_enable(gt9xx->regacc.i2c_ptr);

#if VSF_INPUT_GT9XX_CFG_SUPPORT_EXTI == ENABLED
    if (gt9xx->gpio_int != NULL) {
        // the exti stays disarmed until the address probe has succeeded
        // (reads before that would race the i2c)
        vsf_gpio_exti_irq_disable(gt9xx->gpio_int, 1 << gt9xx->gpio_int_pin);
    }
#endif

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
#   if VSF_INPUT_GT9XX_CFG_SUPPORT_EXTI == ENABLED
    if (gt9xx->gpio_int == NULL)
#   endif
    {
        vsf_callback_timer_init(&gt9xx->poll_timer);
        gt9xx->poll_timer.on_timer = __vk_input_gt9xx_on_poll;
        vsf_callback_timer_add_ms(&gt9xx->poll_timer, VSF_INPUT_GT9XX_CFG_POLL_MS);
    }
#endif

    return vsf_i2c_regacc(&gt9xx->regacc, GT9XX_REG_PRODUCT_ID, true,
        gt9xx->buffer, 4);
}

#endif
/* EOF */
