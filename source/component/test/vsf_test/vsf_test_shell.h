#ifndef __VSF_TEST_SHELL_H__
#define __VSF_TEST_SHELL_H__

#include "vsf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations — defined later in vsf_test.h */
typedef struct vsf_test_suite_t  vsf_test_suite_t;
typedef struct vsf_test_inst_t   vsf_test_inst_t;

#ifndef VSF_TEST_SHELL_MAX_MATCHES
#   define VSF_TEST_SHELL_MAX_MATCHES   96
#endif

typedef struct vsf_test_shell_t {
    const vsf_test_suite_t **suites;
    uint8_t                 suite_count;
    const vsf_test_inst_t  *instances;
    uint8_t                 instance_count;
    uint8_t                 trace_level;
} vsf_test_shell_t;

void vsf_test_shell_init(vsf_test_shell_t *shell,
                         const vsf_test_suite_t **suites, uint8_t suite_count,
                         const vsf_test_inst_t *instances, uint8_t instance_count);
void vsf_test_shell_run(vsf_test_shell_t *shell);

#ifdef __cplusplus
}
#endif

#endif
