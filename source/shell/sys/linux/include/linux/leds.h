#ifndef __VSF_LINUX_LEDS_H__
#define __VSF_LINUX_LEDS_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

enum led_brightness {
    LED_OFF                         = 0,
    LED_ON                          = 1,
    LED_HALF                        = 127,
    LED_FULL                        = 255,
};

struct led_classdev {
    const char                      *name;
    unsigned int                    brightness;
    unsigned int                    max_brightness;

    int                             flags;
#define LED_SUSPENDED               BIT(0)
#define LED_UNREGISTERING           BIT(1)

    void (*brightness_set)(struct led_classdev *led_cdev, enum led_brightness brightness);
    enum led_brightness (*brightness_get)(struct led_classdev *led_cdev);

    const struct attribute_group    **groups;
};

#ifdef __cplusplus
}
#endif

#endif
