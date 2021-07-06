#include "vsf.h"

#if VSF_USE_EVM == ENABLED && VSF_EVM_USE_LUA == ENABLED

#include "luat_base.h"
#include "luat_malloc.h"
#include "luat_msgbus.h"
#include "luat_timer.h"
#include "luat_gpio.h"

#define DBG_TAG           "luat.gpio"
#define DBG_LVL           DBG_INFO

static void luat_gpio_irq_callback(void *ptr) {

}

int luat_gpio_setup(luat_gpio_t *gpio) {
    return 0;
}

int luat_gpio_set(int pin, int level) {
    return 0;
}

int luat_gpio_get(int pin) {
    return 0;
}

void luat_gpio_close(int pin) {

}

#endif      // VSF_USE_EVM && VSF_EVM_USE_LUA
