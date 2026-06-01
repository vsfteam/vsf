#ifndef __VSF_TEST_PWM_BASIC_H__
#define __VSF_TEST_PWM_BASIC_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_PWM_BASIC_CASE_COUNT
#   define VSF_TEST_PWM_BASIC_CASE_COUNT       1
#endif
#ifndef VSF_TEST_PWM_BASIC_BUSY_WAIT_MS
#   define VSF_TEST_PWM_BASIC_BUSY_WAIT_MS                    10
#endif

#ifndef VSF_TEST_PWM_BASIC_FREQ_HZ
#   define VSF_TEST_PWM_BASIC_FREQ_HZ                         1000
#endif

#ifndef VSF_TEST_PWM_BASIC_CHANNEL
#   define VSF_TEST_PWM_BASIC_CHANNEL                         0
#endif

#ifndef VSF_TEST_PWM_BASIC_PERIOD
#   define VSF_TEST_PWM_BASIC_PERIOD                          100
#endif

#ifndef VSF_TEST_PWM_BASIC_DUTY
#   define VSF_TEST_PWM_BASIC_DUTY                            50
#endif


/*============================ PROTOTYPES ====================================*/

void vsf_test_pwm_basic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  slice;
    uint8_t  channel;
    uint8_t  gpio;
    uint32_t freq_hz;
    uint32_t period;
    uint32_t pulse;
    uint32_t run_ms;
} vsf_test_pwm_basic_params_t;

struct vsf_test_pwm_basic_s;
extern const struct vsf_test_pwm_basic_s vsf_test_pwm_basic;

#endif /* __VSF_TEST_PWM_BASIC_H__ */
/* EOF */
