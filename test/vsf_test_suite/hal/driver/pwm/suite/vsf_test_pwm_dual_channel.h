#ifndef __VSF_TEST_PWM_DUAL_CHANNEL_H__
#define __VSF_TEST_PWM_DUAL_CHANNEL_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT
#   define VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT       1
#endif
#ifndef VSF_TEST_PWM_DUAL_CHANNEL_BUSY_WAIT_MS
#   define VSF_TEST_PWM_DUAL_CHANNEL_BUSY_WAIT_MS             10
#endif

#ifndef VSF_TEST_PWM_DUAL_CHANNEL_FREQ_HZ
#   define VSF_TEST_PWM_DUAL_CHANNEL_FREQ_HZ                  1000
#endif

#ifndef VSF_TEST_PWM_DUAL_CHANNEL_CHANNEL_A
#   define VSF_TEST_PWM_DUAL_CHANNEL_CHANNEL_A                0
#endif

#ifndef VSF_TEST_PWM_DUAL_CHANNEL_CHANNEL_B
#   define VSF_TEST_PWM_DUAL_CHANNEL_CHANNEL_B                1
#endif

#ifndef VSF_TEST_PWM_DUAL_CHANNEL_PERIOD
#   define VSF_TEST_PWM_DUAL_CHANNEL_PERIOD                   100
#endif

#ifndef VSF_TEST_PWM_DUAL_CHANNEL_DUTY_A
#   define VSF_TEST_PWM_DUAL_CHANNEL_DUTY_A                   50
#endif

#ifndef VSF_TEST_PWM_DUAL_CHANNEL_DUTY_B
#   define VSF_TEST_PWM_DUAL_CHANNEL_DUTY_B                   25
#endif


/*============================ PROTOTYPES ====================================*/

void vsf_test_pwm_dual_channel_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  slice;
    uint8_t  channel_a;
    uint8_t  channel_b;
    uint8_t  gpio_a;
    uint8_t  gpio_b;
    uint32_t freq_hz;
    uint32_t period;
    uint32_t pulse_a;
    uint32_t pulse_b;
    uint32_t run_ms;
} vsf_test_pwm_dual_channel_params_t;

struct vsf_test_pwm_dual_channel_s;
extern const struct vsf_test_pwm_dual_channel_s vsf_test_pwm_dual_channel;

#endif /* __VSF_TEST_PWM_DUAL_CHANNEL_H__ */
/* EOF */
