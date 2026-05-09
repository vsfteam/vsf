#ifndef __ESP_CONSOLE_H__
#define __ESP_CONSOLE_H__

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct linenoiseCompletions linenoiseCompletions;

typedef struct {
    size_t max_cmdline_length;
    size_t max_cmdline_args;
    uint32_t heap_alloc_caps;
    int hint_color;
    int hint_bold;
} esp_console_config_t;

#define ESP_CONSOLE_CONFIG_DEFAULT()           \
    {                                          \
        .max_cmdline_length = 256,             \
        .max_cmdline_args = 32,                \
        .heap_alloc_caps = MALLOC_CAP_DEFAULT, \
        .hint_color = 39,                      \
        .hint_bold = 0                         \
    }

typedef struct {
    uint32_t max_history_len;
    const char *history_save_path;
    uint32_t task_stack_size;
    uint32_t task_priority;
    int task_core_id;
    const char *prompt;
    size_t max_cmdline_length;
} esp_console_repl_config_t;

#define ESP_CONSOLE_REPL_CONFIG_DEFAULT() \
    {                                     \
        .max_history_len = 32,            \
        .history_save_path = NULL,        \
        .task_stack_size = 4096,          \
        .task_priority = 2,               \
        .task_core_id = -1,               \
        .prompt = NULL,                   \
        .max_cmdline_length = 0,          \
    }

typedef enum {
    ESP_CONSOLE_HELP_VERBOSE_LEVEL_0       = 0,
    ESP_CONSOLE_HELP_VERBOSE_LEVEL_1       = 1,
    ESP_CONSOLE_HELP_VERBOSE_LEVEL_MAX_NUM = 2
} esp_console_help_verbose_level_e;

typedef int (*esp_console_cmd_func_t)(int argc, char **argv);
typedef int (*esp_console_cmd_func_with_context_t)(void *context, int argc, char **argv);

typedef struct {
    const char *command;
    const char *help;
    const char *hint;
    esp_console_cmd_func_t func;
    void *argtable;
    esp_console_cmd_func_with_context_t func_w_context;
    void *context;
} esp_console_cmd_t;

typedef struct esp_console_repl_s esp_console_repl_t;

struct esp_console_repl_s {
    esp_err_t (*del)(esp_console_repl_t *repl);
};

esp_err_t esp_console_init(const esp_console_config_t *config);
esp_err_t esp_console_deinit(void);
esp_err_t esp_console_cmd_register(const esp_console_cmd_t *cmd);
esp_err_t esp_console_cmd_deregister(const char *cmd_name);
esp_err_t esp_console_run(const char *cmdline, int *cmd_ret);
size_t esp_console_split_argv(char *line, char **argv, size_t argv_size);

esp_err_t esp_console_register_help_command(void);
esp_err_t esp_console_deregister_help_command(void);
esp_err_t esp_console_set_help_verbose_level(esp_console_help_verbose_level_e verbose_level);

void esp_console_get_completion(const char *buf, linenoiseCompletions *lc);
const char *esp_console_get_hint(const char *buf, int *color, int *bold);

esp_err_t esp_console_new_repl_stdio(const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl);
esp_err_t esp_console_delete_repl_stdio(esp_console_repl_t *repl);
esp_err_t esp_console_start_repl(esp_console_repl_t *repl);
esp_err_t esp_console_stop_repl(esp_console_repl_t *repl);

#ifdef __cplusplus
}
#endif

#endif
