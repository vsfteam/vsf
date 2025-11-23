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
        kfree(dev);
    }
}

int input_register_device(struct input_dev *dev)
{
    return 0;
}

void input_unregister_device(struct input_dev *dev)
{
}

void input_set_capability(struct input_dev *dev, unsigned int type, unsigned int code)
{
}

void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
{
}

void input_alloc_absinfo(struct input_dev *dev)
{
}

void input_set_abs_params(struct input_dev *dev, unsigned int axis, int min, int max, int fuzz, int flat)
{
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
