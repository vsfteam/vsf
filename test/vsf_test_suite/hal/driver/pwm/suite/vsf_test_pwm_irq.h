#ifndef __VSF_TEST_PWM_IRQ_H__
#define __VSF_TEST_PWM_IRQ_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_PWM_IRQ_CASE_COUNT
#   define VSF_TEST_PWM_IRQ_CASE_COUNT       1
#endif
#ifndef VSF_TEST_PWM_IRQ_BUSY_WAIT_MS
#   define VSF_TEST_PWM_IRQ_BUSY_WAIT_MS                      50
#endif

#ifndef VSF_TEST_PWM_IRQ_POLL_WINDOW_TICKS
#   define VSF_TEST_PWM_IRQ_POLL_WINDOW_TICKS                500000
#endif

// Tolerance bands for IRQ wrap rate verification (fractional: num/den)
#define VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_LOW_NUM              5
#define VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_LOW_DEN               10
#define VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_HIGH_NUM              25
#define VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_HIGH_DEN              10

/*============================ PROTOTYPES ====================================*/

void vsf_test_pwm_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  slice;
    uint8_t  channel;
    uint32_t freq_hz;
    uint32_t period;
    uint32_t pulse;
    uint32_t test_ms;
} vsf_test_pwm_irq_params_t;

struct vsf_test_pwm_irq_s;
extern const struct vsf_test_pwm_irq_s vsf_test_pwm_irq;

#endif /* __VSF_TEST_PWM_IRQ_H__ */
/* EOF */
