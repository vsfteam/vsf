#include "vsf.h"
#include "vsf_board.h"
#include "component/test/vsf_test/vsf_test.h"

#include <string.h>

#include "./vsf_test_shell.h"

extern vsf_mem_stream_t VSF_DEBUG_STREAM_RX;

#define VSF_TEST_SHELL_POLL_INTERVAL_MS    1
#define VSF_TEST_SHELL_LINE_BUF_SIZE       128

/* ------------------------------------------------------------------------ */
/* Line reader                                                              */
/* ------------------------------------------------------------------------ */

static void __read_line(char *buf, size_t buf_size)
{
    size_t len = 0;
    buf[0] = '\0';
    while (1) {
        uint8_t byte;
        while (vsf_stream_read(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t,
                               &byte, 1) > 0) {
            if (byte == '\n') {
                if (len > 0 && buf[len - 1] == '\r') len--;
                buf[len] = '\0';
                return;
            }
            if (len < buf_size - 1) buf[len++] = (char)byte;
        }
        vsf_test_busy_wait_ms(VSF_TEST_SHELL_POLL_INTERVAL_MS);
    }
}

/* ------------------------------------------------------------------------ */
/* Helpers                                                                  */
/* ------------------------------------------------------------------------ */

static int __find_suite_by_name(vsf_test_shell_t *shell, const char *name)
{
    for (uint8_t i = 0; i < shell->suite_count; i++) {
        const vsf_test_suite_t *suite = shell->suites[i];
        if (suite->name != NULL && strcmp(suite->name, name) == 0) {
            return (int)i;
        }
    }
    return -1;
}

static void __print_summary(uint32_t total, uint32_t pass, uint32_t fail, uint32_t skip)
{
    VSF_TEST_TRACE_INFO("all test cases completed" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_TRACE_INFO("========== Test Summary ==========" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_TRACE_INFO("Total test cases: %u" VSF_TRACE_CFG_LINEEND, total);
    VSF_TEST_TRACE_INFO("pass: %u, fail: %u, skip: %u" VSF_TRACE_CFG_LINEEND,
                   pass, fail, skip);
}

static void __count_result(vsf_test_result_t r, uint32_t *pass, uint32_t *fail, uint32_t *skip)
{
    switch (r) {
    case VSF_TEST_RESULT_PASS: (*pass)++; break;
    case VSF_TEST_RESULT_FAIL: (*fail)++; break;
    case VSF_TEST_RESULT_SKIP: (*skip)++; break;
    default: break;
    }
}

static void __run_suite_against_instance(const vsf_test_suite_t *suite,
                                          const vsf_test_inst_t *inst,
                                          uint32_t *pass, uint32_t *fail, uint32_t *skip)
{
    if (inst->name != NULL) {
        VSF_TEST_TRACE_INFO("instance: %s" VSF_TRACE_CFG_LINEEND, inst->name);
    }
    vsf_test_hw_config(suite->peripheral_type, inst, true);

    for (uint16_t ci = 0; ci < suite->case_count; ci++) {
        vsf_test_result_t r = vsf_test_run_suite_case(suite, ci, inst);
        __count_result(r, pass, fail, skip);
    }

    vsf_test_hw_config(suite->peripheral_type, inst, false);
#if VSF_TEST_CFG_EMIT_MARKERS == ENABLED
    VSF_TEST_TRACE_INFO("%s:end" VSF_TRACE_CFG_LINEEND, suite->name);
#endif
}

static void __run_suite_with_instances(vsf_test_shell_t *shell,
                                        const vsf_test_suite_t *suite,
                                        const char *name)
{
    vsf_peripheral_type_t pt = suite->peripheral_type;
    uint32_t total_all = 0, pass_all = 0, fail_all = 0, skip_all = 0;
    uint8_t inst_run = 0;

    for (uint8_t ii = 0; ii < shell->instance_count; ii++) {
        const vsf_test_inst_t *inst = &shell->instances[ii];
        if (inst == NULL || inst->peripheral_type != pt) continue;

        VSF_TEST_TRACE_INFO("suite ack: %s (instance %u)" VSF_TRACE_CFG_LINEEND,
                       name, (unsigned)inst_run);
        __run_suite_against_instance(suite, inst, &pass_all, &fail_all, &skip_all);
        total_all += suite->case_count;
        inst_run++;
    }

    if (inst_run > 0) {
        VSF_TEST_TRACE_INFO("all test cases completed (%u instance(s))" VSF_TRACE_CFG_LINEEND,
                       (unsigned)inst_run);
        VSF_TEST_TRACE_INFO("========== Test Summary ==========" VSF_TRACE_CFG_LINEEND);
        VSF_TEST_TRACE_INFO("Total test cases: %u" VSF_TRACE_CFG_LINEEND, total_all);
        VSF_TEST_TRACE_INFO("pass: %u, fail: %u, skip: %u" VSF_TRACE_CFG_LINEEND,
                       pass_all, fail_all, skip_all);
    } else {
        VSF_TEST_TRACE_INFO("suite ack: %s" VSF_TRACE_CFG_LINEEND, name);
        vsf_test_run_suite(suite);
        // vsf_test_run_suite discards per-case results — summary is printed by the suite
    }
}

/* ------------------------------------------------------------------------ */
/* Commands                                                                 */
/* ------------------------------------------------------------------------ */

static void __cmd_list_suites(vsf_test_shell_t *shell)
{
    VSF_TEST_TRACE_INFO("scenes:" VSF_TRACE_CFG_LINEEND);
    for (uint8_t i = 0; i < shell->suite_count; i++) {
        vsf_trace_info("  %u %s" VSF_TRACE_CFG_LINEEND,
                       i, shell->suites[i]->name);
    }
}

static void __cmd_list_cases(vsf_test_shell_t *shell, const char *name)
{
    int idx = __find_suite_by_name(shell, name);
    if (idx < 0) {
        VSF_TEST_TRACE_INFO("suite not found: %s" VSF_TRACE_CFG_LINEEND, name);
        return;
    }
    const vsf_test_suite_t *suite = shell->suites[idx];
    uint16_t count = suite->case_count;
    VSF_TEST_TRACE_INFO("cases in '%s':" VSF_TRACE_CFG_LINEEND, suite->name);
    for (uint16_t i = 0; i < count; i++) {
        vsf_trace_info("  %u" VSF_TRACE_CFG_LINEEND, i);
    }
}

static void __cmd_run_all(vsf_test_shell_t *shell)
{
    for (uint8_t si = 0; si < shell->suite_count; si++) {
        const vsf_test_suite_t *s = shell->suites[si];
        __run_suite_with_instances(shell, s, s->name);
    }
}

static void __cmd_run_suite(vsf_test_shell_t *shell, const char *name)
{
    int idx = __find_suite_by_name(shell, name);
    if (idx < 0) {
        VSF_TEST_TRACE_INFO("suite not found: %s" VSF_TRACE_CFG_LINEEND, name);
        return;
    }
    __run_suite_with_instances(shell, shell->suites[idx], name);
}

static void __cmd_run_inst(vsf_test_shell_t *shell, const char *name)
{
    int idx = __find_suite_by_name(shell, name);
    if (idx < 0) {
        VSF_TEST_TRACE_INFO("suite not found: %s" VSF_TRACE_CFG_LINEEND, name);
        return;
    }
    __run_suite_with_instances(shell, shell->suites[idx], name);
}

static void __cmd_run_case(vsf_test_shell_t *shell, const char *name, int case_idx)
{
    int idx = __find_suite_by_name(shell, name);
    if (idx < 0) {
        VSF_TEST_TRACE_INFO("suite not found: %s" VSF_TRACE_CFG_LINEEND, name);
        return;
    }
    const vsf_test_suite_t *suite = shell->suites[idx];
    if (case_idx < 0 || case_idx >= (int)suite->case_count) {
        VSF_TEST_TRACE_INFO("case not found: %d" VSF_TRACE_CFG_LINEEND, case_idx);
        return;
    }
    VSF_TEST_TRACE_INFO("suite ack: %s.%d" VSF_TRACE_CFG_LINEEND, name, case_idx);

    vsf_test_result_t r = vsf_test_run_suite_case(suite, (uint16_t)case_idx, NULL);
    uint32_t pass = 0, fail = 0, skip = 0;
    __count_result(r, &pass, &fail, &skip);

    if (case_idx == (int)(suite->case_count - 1)) {
#if VSF_TEST_CFG_EMIT_MARKERS == ENABLED
        VSF_TEST_TRACE_INFO("%s:end" VSF_TRACE_CFG_LINEEND, suite->name);
#endif
    }

    __print_summary(suite->case_count, pass, fail, skip);
}

static void __cmd_trace_level(vsf_test_shell_t *shell, const char *arg)
{
    static const char *names[] = {"none", "error", "warning", "info", "debug"};
    if (arg == NULL || *arg == '\0') {
        vsf_trace_info("trace-level: %s" VSF_TRACE_CFG_LINEEND,
                       names[shell->trace_level]);
        return;
    }
    for (uint8_t i = 0; i < (uint8_t)(sizeof(names) / sizeof(names[0])); i++) {
        if (strcmp(arg, names[i]) == 0) {
            shell->trace_level = i;
            vsf_trace_info("trace-level set: %s" VSF_TRACE_CFG_LINEEND, names[i]);
            return;
        }
    }
    vsf_trace_info("Usage: vsf-test trace-level [none|error|warning|info|debug]" VSF_TRACE_CFG_LINEEND);
}

/* ------------------------------------------------------------------------ */
/* Dispatch                                                                 */
/* ------------------------------------------------------------------------ */

static void __dispatch(vsf_test_shell_t *shell, char *line)
{
    const char *prefix = "vsf-test ";
    size_t prefix_len = strlen(prefix);
    if (strncmp(line, prefix, prefix_len) != 0) {
        vsf_trace_info("Unknown command. Try 'vsf-test list-suites'" VSF_TRACE_CFG_LINEEND);
        return;
    }
    char *rest = line + prefix_len;

    if (strcmp(rest, "list-suites") == 0) {
        __cmd_list_suites(shell);
    } else if (strncmp(rest, "list-cases ", 11) == 0) {
        __cmd_list_cases(shell, rest + 11);
    } else if (strcmp(rest, "run-all") == 0) {
        __cmd_run_all(shell);
    } else if (strncmp(rest, "run-suite ", 10) == 0) {
        __cmd_run_suite(shell, rest + 10);
    } else if (strncmp(rest, "run-inst ", 9) == 0) {
        __cmd_run_inst(shell, rest + 9);
    } else if (strncmp(rest, "run-case ", 9) == 0) {
        char *name = rest + 9;
        char *space = strrchr(name, ' ');
        if (space == NULL) {
            vsf_trace_info("Usage: vsf-test run-case <name> <n>" VSF_TRACE_CFG_LINEEND);
            return;
        }
        *space = '\0';
        int n = atoi(space + 1);
        __cmd_run_case(shell, name, n);
    } else if (strncmp(rest, "trace-level", 11) == 0) {
        const char *arg = rest + 11;
        while (*arg == ' ') { arg++; }
        __cmd_trace_level(shell, *arg ? arg : NULL);
    } else {
        vsf_trace_info("Unknown command. Try 'vsf-test list-suites'" VSF_TRACE_CFG_LINEEND);
    }
}

/* ------------------------------------------------------------------------ */
/* Public API                                                               */
/* ------------------------------------------------------------------------ */

void vsf_test_shell_init(vsf_test_shell_t *shell,
                         const vsf_test_suite_t **suites, uint8_t suite_count,
                         const vsf_test_inst_t *instances, uint8_t instance_count)
{
    shell->suites         = suites;
    shell->suite_count    = suite_count;
    shell->instances      = instances;
    shell->instance_count = instance_count;
    shell->trace_level    = VSF_TRACE_INFO;
    vsf_stream_connect_rx(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t);
    VSF_TEST_TRACE_INFO("vsf-test ready" VSF_TRACE_CFG_LINEEND);
    vsf_trace_info("> " VSF_TRACE_CFG_LINEEND);
}

void vsf_test_shell_run(vsf_test_shell_t *shell)
{
    char line[VSF_TEST_SHELL_LINE_BUF_SIZE];
    while (1) {
        __read_line(line, sizeof(line));
        if (line[0] != '\0') __dispatch(shell, line);
        vsf_trace_info("> " VSF_TRACE_CFG_LINEEND);
    }
}
