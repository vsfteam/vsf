#ifndef __ESP_CONSOLE_H__
#define __ESP_CONSOLE_H__

#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_console_config {
    int max_cmdline_length;
    int max_cmdline_args;
    int hint_color;
} esp_console_config_t;

typedef struct esp_console_cmd {
    const char *command;
    const char *help;
    const char *hint;
    int (*func)(int argc, char **argv);
    void *argtable;
} esp_console_cmd_t;

static inline esp_err_t esp_console_init(const esp_console_config_t *config)
{
    (void)config;
    return ESP_ERR_NOT_SUPPORTED;
}

static inline esp_err_t esp_console_deinit(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

static inline esp_err_t esp_console_cmd_register(const esp_console_cmd_t *cmd)
{
    (void)cmd;
    return ESP_ERR_NOT_SUPPORTED;
}

static inline esp_err_t esp_console_run(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

#ifdef __cplusplus
}
#endif

#endif
