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

#if VSF_USE_LINUX == ENABLED && VSF_USE_INPUT == ENABLED

#if VSF_LINUX_USE_DEVFS == ENABLED
// for vsf_linux_fs_bind_input
#   include <unistd.h>
#endif

#include <linux/input.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

struct input_dev * input_allocate_device(void)
{
    struct input_dev *dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev) {
        return NULL;
    }

    dev->notifier.dev = dev;
    return dev;
}

struct input_dev * devm_input_allocate_device(struct device *dev)
{
    struct input_dev *input_dev = input_allocate_device();
    if (input_dev != NULL) {
        input_dev->dev.parent = dev;
    }
    return input_dev;
}

void input_free_device(struct input_dev *dev)
{
    if (dev) {
        if (dev->absinfo) {
            kfree(dev->absinfo);
            dev->absinfo = NULL;
        }
        kfree(dev);
    }
}

int input_register_device(struct input_dev *dev)
{
#if VSF_LINUX_USE_DEVFS == ENABLED
    char path[11 + strlen(dev->phys) + 1];
    strcpy(path, "/dev/input/");
    strcat(path, dev->phys);
    return vsf_linux_fs_bind_input(path, &dev->notifier);
#else
    return 0;
#endif
}

void input_unregister_device(struct input_dev *dev)
{
}

void input_set_capability(struct input_dev *dev, unsigned int type, unsigned int code)
{
    switch (type) {
    case EV_KEY:
        if ((code >= BTN_MOUSE) && (code <= BTN_MOUSE_MAX)) {
            dev->notifier.mask |= 1 << VSF_INPUT_TYPE_MOUSE;
        } else if ((code >= BTN_GAMEPAD) && (code <= BTN_GAMEPAD_MAX)) {
            dev->notifier.mask |= 1 << VSF_INPUT_TYPE_GAMEPAD;
        } else if (code == BTN_TOUCH) {
            dev->notifier.mask |= 1 << VSF_INPUT_TYPE_TOUCHSCREEN;
        } else if (code <= KEY_MAX) {
            dev->notifier.mask |= 1 << VSF_INPUT_TYPE_KEYBOARD;
        }
        break;
    }
}

void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
{
    union {
        implement(vk_input_evt_t)
        vk_touchscreen_evt_t    ts_evt;
        vk_gamepad_evt_t        gamepad_evt;
        vk_keyboard_evt_t       keyboard_evt;
        vk_mouse_evt_t          mouse_evt;
    } evt = { 0 };
    int orig_value;

    evt.duration = 0;
    evt.dev = dev;
    switch (type) {
    case EV_SYN:
        if (dev->notifier.mask & (1 << VSF_INPUT_TYPE_TOUCHSCREEN)) {
            if (dev->abs_msk & ((1ULL << ABS_X) | (1ULL << ABS_Y))) {
                vsf_input_touchscreen_set(&evt.ts_evt,
                    dev->absinfo[ABS_MT_TRACKING_ID].value,
                    !!vsf_bitmap_get(&dev->key_bitmap, BTN_TOUCH),
                    dev->absinfo[ABS_MT_PRESSURE].value,
                    dev->absinfo[ABS_X].value,
                    dev->absinfo[ABS_Y].value);
                vsf_input_on_touchscreen(&evt.ts_evt);
            }
        }
        dev->abs_msk = 0;

        vsf_input_on_evt(VSF_INPUT_TYPE_SYNC, &evt.use_as__vk_input_evt_t);
        break;
    case EV_KEY:
        VSF_LINUX_ASSERT(code < 0x450);
        orig_value = !!vsf_bitmap_get(&dev->key_bitmap, code);
        if (value) {
            vsf_bitmap_set(&dev->key_bitmap, code);
        } else {
            vsf_bitmap_clear(&dev->key_bitmap, code);
        }

        if ((code >= BTN_MOUSE) && (code <= BTN_MOUSE_MAX)) {
            // TODO: mouse event in vsf uses absolute position, but linux use relative position
//            vsf_input_mouse_evt_button_set(&evt.mouse_evt, code - BTN_MOUSE, !!value, 0, 0);
        } else if ((code >= BTN_GAMEPAD) && (code <= BTN_GAMEPAD_MAX)) {
            evt.gamepad_evt.info.item = code - BTN_GAMEPAD;
            evt.gamepad_evt.cur.bit = !!value;
            evt.gamepad_evt.pre.bit = orig_value;
            vsf_input_on_gamepad(&evt.gamepad_evt);
        } else if (code <= KEY_MAX) {
            uint8_t modifier = 0;
            for (int i = VSF_KB_MODIFIER_START; i <= VSF_KB_MODIFIER_END; i++) {
                if (vsf_bitmap_get(&dev->key_bitmap, i)) {
                    modifier |= (1 << (i - VSF_KB_MODIFIER_START));
                }
            }
            if (vsf_bitmap_get(&dev->key_bitmap, VSF_KP_NUMLOCK)) {
                modifier |= VSF_KM_NUMLOCK;
            }
            if (vsf_bitmap_get(&dev->key_bitmap, VSF_KB_CAPSLOCK)) {
                modifier |= VSF_KM_CAPSLOCK;
            }
            if (vsf_bitmap_get(&dev->key_bitmap, VSF_KB_SCROLLLOCK)) {
                modifier |= VSF_KM_SCROLLLOCK;
            }
            vsf_input_keyboard_set(&evt.keyboard_evt, code, !!value, modifier);
            vsf_input_on_keyboard(&evt.keyboard_evt);
        }
        break;
    case EV_REL:
        VSF_LINUX_ASSERT(code < REL_CNT);
        dev->rel_msk |= 1ULL << code;
        orig_value = dev->rel_value[code];
        dev->rel_value[code] = value;
    case EV_ABS:
        if (!dev->absinfo) {
            break;
        }

        VSF_LINUX_ASSERT(code < ABS_CNT);
        dev->abs_msk |= 1ULL << code;
        orig_value = dev->absinfo[code].value;
        dev->absinfo[code].value = value;

        if (dev->notifier.mask & (1 << VSF_INPUT_TYPE_GAMEPAD)) {
            do {
                struct input_absinfo *absinfo = &dev->absinfo[code];
                if ((value >= -absinfo->flat) && (value <= absinfo->flat)) {
                    value = 0;
                }
                if (abs(value - orig_value) <= absinfo->fuzz) {
                    break;
                }

                if (ABS_X == code) {
                    evt.gamepad_evt.info.item = GAMEPAD_ID_LX;
                } else if (ABS_Y == code) {
                    evt.gamepad_evt.info.item = GAMEPAD_ID_LY;
                } else if (ABS_RX == code) {
                    evt.gamepad_evt.info.item = GAMEPAD_ID_RX;
                } else if (ABS_RY == code) {
                    evt.gamepad_evt.info.item = GAMEPAD_ID_RY;
                } else if (ABS_Z == code) {
                    evt.gamepad_evt.info.item = GAMEPAD_ID_LT;
                } else if (ABS_RZ == code) {
                    evt.gamepad_evt.info.item = GAMEPAD_ID_RT;
                } else if (ABS_HAT0X == code) {
                    if (value < 0) {
                        evt.gamepad_evt.info.item = GAMEPAD_ID_LL;
                        value = 1;
                    } else if (value > 0) {
                        value = 1;
                        evt.gamepad_evt.info.item = GAMEPAD_ID_LR;
                    } else if (orig_value < 0) {
                        evt.gamepad_evt.info.item = GAMEPAD_ID_LL;
                    } else if (orig_value > 0) {
                        evt.gamepad_evt.info.item = GAMEPAD_ID_LR;
                    }
                } else if (ABS_HAT0Y == code) {
                    if (value < 0) {
                        evt.gamepad_evt.info.item = GAMEPAD_ID_LU;
                        value = 1;
                    } else if (value > 0) {
                        value = 1;
                        evt.gamepad_evt.info.item = GAMEPAD_ID_LD;
                    } else if (orig_value < 0) {
                        evt.gamepad_evt.info.item = GAMEPAD_ID_LU;
                    } else if (orig_value > 0) {
                        evt.gamepad_evt.info.item = GAMEPAD_ID_LD;
                    }
                } else {
                    break;
                }

                if (absinfo->minimum < 0) {
                    evt.gamepad_evt.info.is_signed = 1;
                }
                int_fast8_t bitlen = vsf_msb32(absinfo->maximum - absinfo->minimum + 1);
                evt.gamepad_evt.info.bitlen = bitlen < 0 ? 0 : bitlen;
                evt.gamepad_evt.cur.val32 = value;
                evt.gamepad_evt.pre.val32 = orig_value;
                vsf_input_on_gamepad(&evt.gamepad_evt);
            } while (0);
        }
        break;
    }
}

void input_alloc_absinfo(struct input_dev *dev)
{
    if (dev->absinfo) {
        return;
    }

    dev->absinfo = kmalloc(ABS_CNT * sizeof(*dev->absinfo), GFP_KERNEL);
}

void input_set_abs_params(struct input_dev *dev, unsigned int axis, int min, int max, int fuzz, int flat)
{
    input_alloc_absinfo(dev);
    if (!dev->absinfo) {
        return;
    }

    struct input_absinfo *absinfo = &dev->absinfo[axis];
    absinfo->minimum = min;
    absinfo->maximum = max;
    absinfo->fuzz = fuzz;
    absinfo->flat = flat;

    if ((ABS_MT_POSITION_X == axis) || (ABS_MT_POSITION_Y == axis)) {
        dev->notifier.mask |= 1 << VSF_INPUT_TYPE_TOUCHSCREEN;
    }
}

void input_copy_abs(struct input_dev *dst, unsigned int dst_axis, const struct input_dev *src, unsigned int src_axis)
{
}

int input_ff_create(struct input_dev *dev, unsigned int max_effects)
{
    return 0;
}

void input_ff_destroy(struct input_dev *dev)
{
}

int input_ff_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
{
    return 0;
}

int input_ff_create_memless(struct input_dev *dev, void *data, int (*play_effect)(struct input_dev *, void *, struct ff_effect *))
{
    return 0;
}

#endif
